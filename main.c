#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "morse.h"
#include "file_utils.h"

/*
 * NOTE on buffer sizes:
 * MAX_INPUT_LEN is 10,000,000 characters.
 * Declaring buffers that large on the STACK causes Segmentation Fault
 * because the default Linux stack limit is only ~8 MB.
 * All large buffers must be heap-allocated with malloc() / free().
 */

/**
 * @brief Tạo tên file output từ tên file input và hậu tố cho trước.
 *        Loại bỏ phần mở rộng của file input (nếu có) rồi ghép
 *        thêm hậu tố và đuôi ".txt".
 *        Ví dụ: "test.txt" + "encoded" -> "test_encoded.txt"
 *               "input"   + "decoded" -> "input_decoded.txt"
 *
 * @param inpath        Đường dẫn file input gốc (ví dụ: "test.txt")
 * @param suffix        Hậu tố cần thêm vào (ví dụ: "encoded", "decoded")
 * @param outpath       Buffer lưu đường dẫn file output được tạo ra
 * @param outpath_size  Kích thước tối đa của buffer outpath
 * @return void
 */
static void make_outfile(const char *inpath, const char *suffix,
                         char *outpath, int outpath_size)
{
    char base[512];
    strncpy(base, inpath, sizeof(base) - 1);
    base[sizeof(base) - 1] = '\0';

    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';

    snprintf(outpath, outpath_size, "%s_%s.txt", base, suffix);
}
/**
 * @brief In hướng dẫn sử dụng chương trình ra stderr.
 *        Được gọi khi người dùng truyền sai số lượng tham số
 *        hoặc sử dụng flag không hợp lệ.
 *
 * @param prog  Tên chương trình (thường là argv[0])
 * @return void
 */
static void usage(const char *prog)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  Encode a file:  %s -e <input_file>\n", prog);
    fprintf(stderr, "  Decode a file:  %s -d <input_file>\n", prog);
}

/**
 * @brief Điểm vào chương trình. Phân tích tham số dòng lệnh và
 *        điều phối luồng xử lý encode hoặc decode.
 * 
 *        Luồng xử lý ENCODE (-e):
 *          1. Cấp phát buffer input (MAX_INPUT_LEN + 1) và output
 *             (MAX_INPUT_LEN * 16 + 1) trên heap bằng malloc() để
 *             tránh Segmentation Fault khi kích thước lên đến 10 triệu ký tự
 *          2. Đọc file input vào buffer
 *          3. Gọi encode() để mã hóa
 *          4. In tối đa MAX_DISPLAY ký tự ra màn hình
 *          5. Lưu toàn bộ kết quả vào file <input>_encoded.txt
 *          6. Giải phóng bộ nhớ bằng free()
 *
 *        Luồng xử lý DECODE (-d):
 *          1. Khai báo buffer trên stack (kích thước nhỏ, tối đa 100 ký tự)
 *          2. Đọc file input vào buffer, xóa ký tự newline thừa cuối chuỗi
 *          3. Gọi validate_morse() để kiểm tra tính hợp lệ
 *          4. Gọi decode() để giải mã
 *          5. In tối đa MAX_DISPLAY ký tự ra màn hình
 *          6. Lưu toàn bộ kết quả vào file <input>_decoded.txt
 *
 * @param argc  Số lượng tham số dòng lệnh (phải đúng bằng 3)
 * @param argv  Mảng chuỗi tham số:
 *                argv[0]: tên chương trình
 *                argv[1]: flag "-e" hoặc "-d"
 *                argv[2]: đường dẫn file input
 * @return int  EXIT_SUCCESS (0) nếu thành công
 *              EXIT_FAILURE (1) nếu có lỗi (sai tham số, lỗi file,
 *              lỗi mã hóa/giải mã, hoặc hết bộ nhớ)
 */

int main(int argc, char *argv[])
{
    if (argc != 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *flag   = argv[1];
    const char *inpath = argv[2];

    if (strcmp(flag, "-e") != 0 && strcmp(flag, "-d") != 0) {
        fprintf(stderr, "[ERROR] Unknown flag: %s\n", flag);
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* ==================== ENCODE ==================== */
    if (strcmp(flag, "-e") == 0) {

        /* Each morse symbol is at most 15 chars + 1 space = 16 chars per input char */
        size_t in_size  = (size_t)MAX_INPUT_LEN + 1;
        size_t out_size = (size_t)MAX_INPUT_LEN * 16 + 1;

        char *input_buf  = malloc(in_size);
        char *output_buf = malloc(out_size);

        if (!input_buf || !output_buf) {
            fprintf(stderr, "[ERROR] Out of memory.\n");
            free(input_buf);
            free(output_buf);
            return EXIT_FAILURE;
        }

        if (read_file(inpath, input_buf, (int)in_size) < 0) {
            free(input_buf); free(output_buf);
            return EXIT_FAILURE;
        }

        if (input_buf[0] == '\0') {
            fprintf(stderr, "[ERROR] Input file is empty.\n");
            free(input_buf); free(output_buf);
            return EXIT_FAILURE;
        }

        printf("[INFO]   Mode       : ENCODE\n");
        printf("[INFO]   Input file : %s\n", inpath);
        /* Only print first 80 chars of input to avoid flooding terminal */
        printf("[INFO]   Input text : %.80s%s\n",
               input_buf, strlen(input_buf) > 80 ? "..." : "");

        if (encode(input_buf, output_buf, (int)out_size) != 0) {
            fprintf(stderr, "[ERROR] Encoding failed.\n");
            free(input_buf); free(output_buf);
            return EXIT_FAILURE;
        }

        printf("[RESULT] Morse output (first %d chars):\n", MAX_DISPLAY);
        print_limited(output_buf, MAX_DISPLAY);

        char outpath[512];
        make_outfile(inpath, "encoded", outpath, sizeof(outpath));
        if (write_file(outpath, output_buf) == 0)
            printf("[INFO]   Output saved: %s\n", outpath);
        else {
            free(input_buf); free(output_buf);
            return EXIT_FAILURE;
        }

        free(input_buf);
        free(output_buf);
    }

    /* ==================== DECODE ==================== */
    else {

        /* Decode input is max 100 chars — stack is fine here */
        char input_buf[MAX_MORSE_LEN * 5 + 2];
        char output_buf[MAX_MORSE_LEN + 2];

        if (read_file(inpath, input_buf, sizeof(input_buf)) < 0)
            return EXIT_FAILURE;

        if (input_buf[0] == '\0') {
            fprintf(stderr, "[ERROR] Input file is empty.\n");
            return EXIT_FAILURE;
        }

        /* Strip trailing newline */
        int len = (int)strlen(input_buf);
        while (len > 0 &&
               (input_buf[len - 1] == '\n' || input_buf[len - 1] == '\r'))
            input_buf[--len] = '\0';

        printf("[INFO]   Mode        : DECODE\n");
        printf("[INFO]   Input file  : %s\n", inpath);
        printf("[INFO]   Morse input : %s\n", input_buf);

        if (!validate_morse(input_buf)) {
            fprintf(stderr, "[ERROR] Input is NOT a valid Morse string.\n");
            return EXIT_FAILURE;
        }
        printf("[INFO]   Validation  : PASSED\n");

        if (decode(input_buf, output_buf, sizeof(output_buf)) != 0) {
            fprintf(stderr, "[ERROR] Decoding failed.\n");
            return EXIT_FAILURE;
        }

        printf("[RESULT] Decoded text (first %d chars):\n", MAX_DISPLAY);
        print_limited(output_buf, MAX_DISPLAY);

        char outpath[512];
        make_outfile(inpath, "decoded", outpath, sizeof(outpath));
        if (write_file(outpath, output_buf) == 0)
            printf("[INFO]   Output saved: %s\n", outpath);
        else
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}