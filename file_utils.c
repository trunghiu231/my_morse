#include <stdio.h>
#include <string.h>
#include "file_utils.h"

/* ----------------------------------------------------------
 * read_file
 * ---------------------------------------------------------- */
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

/* ----------------------------------------------------------
 * write_file
 * ---------------------------------------------------------- */
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

/* ----------------------------------------------------------
 * print_limited
 *   Print at most max_chars characters, then newline.
 *   If output is longer, print a truncation notice.
 * ---------------------------------------------------------- */
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