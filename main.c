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

/* ----------------------------------------------------------
 * Build output filename:  "test.txt" -> "test_encoded.txt"
 * ---------------------------------------------------------- */
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

static void usage(const char *prog)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  Encode a file:  %s -e <input_file>\n", prog);
    fprintf(stderr, "  Decode a file:  %s -d <input_file>\n", prog);
}

/* ----------------------------------------------------------
 * main
 * ---------------------------------------------------------- */
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