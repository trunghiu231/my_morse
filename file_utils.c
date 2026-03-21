#include <stdio.h>
#include <string.h>
#include "file_utils.h"

/**
 * @brief Đọc toàn bộ nội dung file văn bản vào buffer.
 *        Dừng đọc khi gặp EOF hoặc buffer sắp đầy (buf_size - 1),
 *        sau đó tự động null-terminate chuỗi.
 *
 * @param filepath  Đường dẫn đến file cần đọc
 * @param buffer    Con trỏ đến vùng nhớ lưu nội dung đọc được
 * @param buf_size  Kích thước tối đa của buffer (tính cả ký tự '\0')
 * @return int      Số byte đọc được nếu thành công, -1 nếu lỗi
 *                  (tham số không hợp lệ hoặc không mở được file)
 */
int read_file(const char *filepath, char *buffer, int buf_size)
{
    if (!filepath || !buffer || buf_size <= 0) return -1;

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "[FILE] Cannot open: %s\n", filepath);
        return -1;
    }

    int count = 0;
    int c;
    while ((c = fgetc(fp)) != EOF && count < buf_size - 1) {
        buffer[count++] = (char)c;
    }
    buffer[count] = '\0';

    fclose(fp);
    return count;
}

/**
 * @brief Ghi chuỗi nội dung vào file văn bản.
 *        Nếu file chưa tồn tại thì tạo mới, nếu đã tồn tại thì ghi đè.
 *
 * @param filepath  Đường dẫn đến file cần ghi
 * @param content   Chuỗi nội dung cần ghi vào file
 * @return int      0 nếu ghi thành công, -1 nếu lỗi
 *                  (tham số không hợp lệ hoặc không mở được file)
 */
int write_file(const char *filepath, const char *content)
{
    if (!filepath || !content) return -1;

    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[FILE] Cannot write: %s\n", filepath);
        return -1;
    }

    fputs(content, fp);
    fclose(fp);
    return 0;
}

/**
 * @brief In tối đa max_chars ký tự của chuỗi ra màn hình.
 *        Nếu độ dài chuỗi nhỏ hơn hoặc bằng max_chars thì in toàn bộ.
 *        Nếu dài hơn thì chỉ in max_chars ký tự đầu tiên và hiển thị
 *        thông báo kết quả đã được lưu đầy đủ vào file output.
 *
 * @param str       Chuỗi cần in ra màn hình
 * @param max_chars Số ký tự tối đa được phép in (theo đề bài là 20)
 * @return void
 */
void print_limited(const char *str, int max_chars)
{
    if (!str) return;

    int len = (int)strlen(str);
    if (len <= max_chars) {
        printf("%s\n", str);
    } else {
        for (int i = 0; i < max_chars; i++) {
            putchar(str[i]);
        }
        printf("\n[... truncated. Full result saved to output file.]\n");
    }
}