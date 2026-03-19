#ifndef MORSE_H
#define MORSE_H

/* Max length of plain text input for encoding (10,000,000 chars) */
#define MAX_INPUT_LEN    10000000

/* Max length of morse string for decoding (100 chars per requirement) */
#define MAX_MORSE_LEN    100

/* Max characters to print on screen */
#define MAX_DISPLAY      20

/* Lookup: char -> custom morse string ('+' = dot, '===' = dash) */
const char *char_to_morse(char c);

/* Lookup: custom morse token -> char */
char morse_to_char(const char *code);

/* Validate a morse string. Returns 1 if valid, 0 otherwise */
int validate_morse(const char *input);

/* Encode plain text to morse. Returns 0 on success, -1 on error */
int encode(const char *input, char *output, int out_size);

/* Decode morse to plain text. Returns 0 on success, -1 on error */
int decode(const char *input, char *output, int out_size);

#endif /* MORSE_H */