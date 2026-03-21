#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "morse.h"

/*
 * Custom encoding (from the assignment):
 *   dot  '.'  -> '+'
 *   dash '-'  -> '==='   (exactly 3 '=' chars)
 *
 * Letters in a word  : separated by single space ' '
 * Words              : separated by double space '  '
 *
 * Standard morse reference:
 *   A .-    B -...  C -.-.  D -..   E .
 *   F ..-.  G --.   H ....  I ..    J .---
 *   K -.-   L .-..  M --    N -.    O ---
 *   P .--.  Q --.-  R .-.   S ...   T -
 *   U ..-   V ...-  W .--   X -..-  Y -.--
 *   Z --..
 *   0 -----  1 .----  2 ..---  3 ...--  4 ....-
 *   5 .....  6 -....  7 --...  8 ---..  9 ----.
 */

typedef struct {
    char        ch;
    const char *code;   /* custom code: '+' = dot, '===' = dash */
} MorseEntry;

static const MorseEntry TABLE[] = {
    {'A', "+==="},            /* .-    */
    {'B', "===+++"},          /* -...  */
    {'C', "===+===+"},        /* -.-.  */
    {'D', "===++"},           /* -..   */
    {'E', "+"},               /* .     */
    {'F', "++===+"},          /* ..-.  */
    {'G', "======+"},         /* --.   (-- = 6 '=', . = '+') */
    {'H', "++++"},            /* ....  */
    {'I', "++"},              /* ..    */
    {'J', "+========="},      /* .---  (. = '+', --- = 9 '=') */
    {'K', "===+==="},         /* -.-   */
    {'L', "+===++"},          /* .-..  */
    {'M', "======"},          /* --    (6 '=') */
    {'N', "===+"},            /* -.    */
    {'O', "========="},       /* ---   (9 '=') */
    {'P', "+======+"},        /* .--.  */
    {'Q', "======+==="},      /* --.-  */
    {'R', "+===+"},           /* .-.   */
    {'S', "+++"},             /* ...   */
    {'T', "==="},             /* -     */
    {'U', "++==="},           /* ..-   */
    {'V', "+++==="},          /* ...-  */
    {'W', "+======"},         /* .--   */
    {'X', "===++==="},        /* -..-  */
    {'Y', "===+======"},      /* -.--  */
    {'Z', "======++"},        /* --..  */
    {'0', "==============="}, /* ----- (15 '=') */
    {'1', "+============"},   /* .---- ('+' + 12 '=') */
    {'2', "++========="},     /* ..--- ('++' + 9 '=') */
    {'3', "+++======"},       /* ...-- ('+++' + 6 '=') */
    {'4', "++++==="},         /* ....- ('++++' + 3 '=') */
    {'5', "+++++"},           /* .....  */
    {'6', "===++++"},         /* -....  */
    {'7', "======+++"},       /* --...  */
    {'8', "=========++"},     /* ---..  */
    {'9', "=============+"},  /* ----.  (12 '=' + '+') */
    {'\0', NULL}
};

/**
 * @brief Chuyển đổi một token Morse tùy chỉnh sang dạng Morse chuẩn.
 *        Quét token từ trái sang phải theo quy tắc:
 *          - Gặp '+' -> ghi '.' (dot)
 *          - Gặp '===' (đúng 3 dấu '=') -> ghi '-' (dash)
 *          - Gặp '=' đơn, '==' hoặc ký tự khác -> lỗi
 *        Hàm này là static (nội bộ), chỉ dùng bởi morse_to_char() và validate_morse().
 *
 * @param token     Con trỏ đến token Morse tùy chỉnh cần chuyển đổi
 *                  (ví dụ: "+===" cho chữ A)
 * @param out       Buffer lưu kết quả Morse chuẩn
 *                  (ví dụ: ".-" cho chữ A)
 * @param out_size  Kích thước tối đa của buffer out
 * @return int      0 nếu thành công, -1 nếu token không hợp lệ
 *                  hoặc buffer quá nhỏ
 */
static int parse_token_to_std(const char *token, char *out, int out_size)
{
    int i   = 0;
    int pos = 0;
    int len = (int)strlen(token);

    while (i < len) {
        if (token[i] == '+') {
            if (pos + 1 >= out_size) return -1;
            out[pos++] = '.';
            i++;
        } else if (token[i] == '=') {
            if (i + 2 < len && token[i+1] == '=' && token[i+2] == '=') {
                if (pos + 1 >= out_size) return -1;
                out[pos++] = '-';
                i += 3;
            } else {
                return -1; /* lone or double '=' — invalid */
            }
        } else {
            return -1;
        }
    }
    out[pos] = '\0';
    return 0;
}

/**
 * @brief Tra bảng mã Morse, trả về chuỗi mã tùy chỉnh tương ứng
 *        với ký tự đầu vào. Ký tự in thường được tự động chuyển
 *        sang in hoa trước khi tra bảng.
 *
 * @param c         Ký tự cần mã hóa (A-Z, a-z, 0-9)
 * @return const char*  Con trỏ đến chuỗi mã Morse tùy chỉnh
 *                      (ví dụ: "+===" cho 'A', "===+++" cho 'B')
 *                      Trả về NULL nếu ký tự không có trong bảng
 */
const char *char_to_morse(char c)
{
    c = (char)toupper((unsigned char)c);
    for (int i = 0; TABLE[i].ch != '\0'; i++) {
        if (TABLE[i].ch == c)
            return TABLE[i].code;
    }
    return NULL;
}

/**
 * @brief Tra bảng mã Morse, trả về ký tự tương ứng với token
 *        Morse tùy chỉnh đầu vào. Thực hiện bằng cách:
 *          1. Chuyển token đầu vào sang Morse chuẩn qua parse_token_to_std()
 *          2. Duyệt toàn bộ bảng, chuyển từng code trong bảng sang
 *             Morse chuẩn rồi so sánh
 *          3. Trả về ký tự tương ứng nếu khớp
 *
 * @param token     Con trỏ đến token Morse tùy chỉnh cần giải mã
 *                  (ví dụ: "+===" cho chữ A)
 * @return char     Ký tự tương ứng nếu tìm thấy (ví dụ: 'A')
 *                  Trả về '\0' nếu token không hợp lệ hoặc không
 *                  tìm thấy trong bảng
 */
char morse_to_char(const char *token)
{
    char std_token[32];
    if (parse_token_to_std(token, std_token, sizeof(std_token)) != 0)
        return '\0';

    for (int i = 0; TABLE[i].ch != '\0'; i++) {
        char std_tbl[32];
        if (parse_token_to_std(TABLE[i].code, std_tbl, sizeof(std_tbl)) == 0) {
            if (strcmp(std_tbl, std_token) == 0)
                return TABLE[i].ch;
        }
    }
    return '\0';
}

/**
 * @brief Kiểm tra tính hợp lệ của chuỗi Morse tùy chỉnh đầu vào
 *        trước khi tiến hành giải mã. Thực hiện hai bước kiểm tra:
 *          - Bước 1: Duyệt từng ký tự, chỉ cho phép '+', '=', ' '
 *          - Bước 2: Tách chuỗi theo dấu cách, từng token phải khớp
 *                    với ít nhất một mã trong bảng
 *
 * @param input     Con trỏ đến chuỗi Morse tùy chỉnh cần kiểm tra
 * @return int      1 nếu chuỗi hợp lệ, 0 nếu không hợp lệ
 *                  (chuỗi rỗng, ký tự lạ, hoặc token không xác định)
 */
int validate_morse(const char *input)
{
    if (!input || input[0] == '\0') {
        fprintf(stderr, "[VALIDATOR] Empty input.\n");
        return 0;
    }

    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] != '+' && input[i] != '=' && input[i] != ' ') {
            fprintf(stderr,
                    "[VALIDATOR] Invalid character '%c' at position %d.\n",
                    input[i], i);
            return 0;
        }
    }

    char buf[MAX_MORSE_LEN * 5 + 2];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *tok = strtok(buf, " ");
    while (tok != NULL) {
        if (strlen(tok) > 0) {
            if (morse_to_char(tok) == '\0') {
                fprintf(stderr,
                        "[VALIDATOR] Unknown token: \"%s\".\n", tok);
                return 0;
            }
        }
        tok = strtok(NULL, " ");
    }

    return 1;
}

/**
 * @brief Mã hóa chuỗi văn bản thuần túy sang chuỗi Morse tùy chỉnh.
 *        Duyệt từng ký tự trong input theo quy tắc:
 *          - Ký tự chữ/số: tra bảng lấy code, thêm vào output
 *          - Giữa các ký tự trong cùng một từ: chèn một dấu cách
 *          - Giữa các từ (khi gặp space trong input): chèn hai dấu cách
 *          - Ký tự không hỗ trợ: bỏ qua, in cảnh báo ra stderr
 *        Bộ nhớ cho output phải được cấp phát bằng malloc() trước khi
 *        gọi hàm này vì kích thước có thể lên đến 16 lần input.
 *
 * @param input     Con trỏ đến chuỗi văn bản cần mã hóa (A-Z, a-z, 0-9)
 * @param output    Con trỏ đến buffer lưu kết quả Morse tùy chỉnh
 * @param out_size  Kích thước tối đa của buffer output
 * @return int      0 nếu mã hóa thành công, -1 nếu lỗi
 *                  (tham số không hợp lệ hoặc buffer quá nhỏ)
 */
int encode(const char *input, char *output, int out_size)
{
    if (!input || !output || out_size <= 0) return -1;

    int pos          = 0;
    int first_letter = 1;
    int in_word      = 0;

    output[0] = '\0';

    for (int i = 0; input[i] != '\0'; i++) {
        char c = input[i];

        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            if (in_word) {
                if (pos + 2 < out_size) {
                    output[pos++] = ' ';
                    output[pos++] = ' ';
                    output[pos]   = '\0';
                }
                in_word      = 0;
                first_letter = 1;
            }
            continue;
        }

        const char *code = char_to_morse(c);
        if (!code) {
            fprintf(stderr, "[ENCODE] Skipping unsupported char: '%c'\n", c);
            continue;
        }

        if (!first_letter) {
            if (pos + 1 < out_size) {
                output[pos++] = ' ';
                output[pos]   = '\0';
            }
        }

        int clen = (int)strlen(code);
        if (pos + clen >= out_size) {
            fprintf(stderr, "[ENCODE] Output buffer too small.\n");
            return -1;
        }
        strcpy(output + pos, code);
        pos += clen;

        first_letter = 0;
        in_word      = 1;
    }

    output[pos] = '\0';
    return 0;
}

/**
 * @brief Mã hóa chuỗi văn bản thuần túy sang chuỗi Morse tùy chỉnh.
 *        Duyệt từng ký tự trong input theo quy tắc:
 *          - Ký tự chữ/số: tra bảng lấy code, thêm vào output
 *          - Giữa các ký tự trong cùng một từ: chèn một dấu cách
 *          - Giữa các từ (khi gặp space trong input): chèn hai dấu cách
 *          - Ký tự không hỗ trợ: bỏ qua, in cảnh báo ra stderr
 *        Bộ nhớ cho output phải được cấp phát bằng malloc() trước khi
 *        gọi hàm này vì kích thước có thể lên đến 16 lần input.
 *
 * @param input     Con trỏ đến chuỗi văn bản cần mã hóa (A-Z, a-z, 0-9)
 * @param output    Con trỏ đến buffer lưu kết quả Morse tùy chỉnh
 * @param out_size  Kích thước tối đa của buffer output
 * @return int      0 nếu mã hóa thành công, -1 nếu lỗi
 *                  (tham số không hợp lệ hoặc buffer quá nhỏ)
 */
int decode(const char *input, char *output, int out_size)
{
    if (!input || !output || out_size <= 0) return -1;

    if (!validate_morse(input)) {
        fprintf(stderr, "[DECODE] Invalid morse input.\n");
        return -1;
    }

    output[0] = '\0';

    int out_pos    = 0;
    int len        = (int)strlen(input);
    int tok_start  = 0;
    int tok_len    = 0;
    int i          = 0;

    while (i <= len) {
        char cur = (i < len) ? input[i] : '\0';

        if (cur != ' ' && cur != '\0') {
            if (tok_len == 0) tok_start = i;
            tok_len++;
            i++;
        } else {
            /* Flush accumulated token */
            if (tok_len > 0) {
                char tok[64];
                int  copy = (tok_len < 63) ? tok_len : 63;
                strncpy(tok, input + tok_start, copy);
                tok[copy] = '\0';

                char ch = morse_to_char(tok);
                if (ch == '\0') {
                    fprintf(stderr, "[DECODE] Unknown token \"%s\".\n", tok);
                    return -1;
                }
                if (out_pos + 1 < out_size) {
                    output[out_pos++] = ch;
                    output[out_pos]   = '\0';
                }
                tok_len = 0;
            }

            if (cur == ' ') {
                /* Peek: double space = word separator */
                if (i + 1 < len && input[i + 1] == ' ') {
                    if (out_pos + 1 < out_size) {
                        output[out_pos++] = ' ';
                        output[out_pos]   = '\0';
                    }
                    /* Skip past all consecutive spaces */
                    while (i < len && input[i] == ' ') i++;
                } else {
                    i++; /* single space — letter separator, just skip */
                }
            } else {
                i++; /* '\0' — advance past end to exit loop */
            }
        }
    }

    output[out_pos] = '\0';
    return 0;
}