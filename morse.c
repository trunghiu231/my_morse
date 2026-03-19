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

/* ----------------------------------------------------------
 * parse_token_to_std: convert a custom token to standard morse
 *   '+' -> '.'
 *   '===' -> '-'   (consume exactly 3 '=')
 * Returns 0 on success, -1 on invalid input.
 * ---------------------------------------------------------- */
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

/* ----------------------------------------------------------
 * char_to_morse
 * ---------------------------------------------------------- */
const char *char_to_morse(char c)
{
    c = (char)toupper((unsigned char)c);
    for (int i = 0; TABLE[i].ch != '\0'; i++) {
        if (TABLE[i].ch == c)
            return TABLE[i].code;
    }
    return NULL;
}

/* ----------------------------------------------------------
 * morse_to_char: parse a custom token, then look up the char
 * ---------------------------------------------------------- */
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

/* ----------------------------------------------------------
 * validate_morse
 *   1. Only '+', '=', ' ' allowed
 *   2. Every space-separated token must be a known code
 * ---------------------------------------------------------- */
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

/* ----------------------------------------------------------
 * encode: plain text -> custom morse
 *
 * Letter separator : single space
 * Word separator   : double space
 * ---------------------------------------------------------- */
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

/* ----------------------------------------------------------
 * decode: custom morse -> plain text
 *
 * Scan char by char:
 *   - collect non-space runs as tokens
 *   - single space  = letter separator
 *   - double space  = word separator (insert ' ' in output)
 * ---------------------------------------------------------- */
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