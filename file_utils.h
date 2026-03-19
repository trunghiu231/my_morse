#ifndef FILE_UTILS_H
#define FILE_UTILS_H

/* Read file into buffer. Returns bytes read, -1 on error */
int read_file(const char *filepath, char *buffer, int buf_size);

/* Write content to file. Returns 0 on success, -1 on error */
int write_file(const char *filepath, const char *content);

/* Print at most max_chars characters of str, then newline */
void print_limited(const char *str, int max_chars);

#endif /* FILE_UTILS_H */