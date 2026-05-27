#include "utils.h"
#include <stdio.h>

/**
 * Duplicates a string using malloc
 * @param s Source string to duplicate
 * @return New string or NULL on error
 */
char *duplicate_string(const char *s) {
  if (s == NULL)
    return NULL;

  size_t len = strlen(s) + 1;
  char *dup = malloc(len);
  if (dup != NULL) {
    strcpy(dup, s);
  }
  return dup;
}

// Internal helper: convert a single hex digit to its value, returns -1 on error
static int hex_value(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return 10 + (c - 'a');
  if (c >= 'A' && c <= 'F')
    return 10 + (c - 'A');
  return -1;
}

// Internal helper: parse #RRGGBB into r,g,b (0-255). Returns 0 on success
static int parse_hex_color(const char *s, int *r, int *g, int *b) {
  if (s == NULL || s[0] != '#')
    return -1;
  // expect 7 chars: # + 6 hex
  if (strlen(s) != 7)
    return -1;
  int v[6];
  for (int i = 0; i < 6; i++) {
    int hv = hex_value(s[1 + i]);
    if (hv < 0)
      return -1;
    v[i] = hv;
  }
  *r = v[0] * 16 + v[1];
  *g = v[2] * 16 + v[3];
  *b = v[4] * 16 + v[5];
  return 0;
}

// Internal helper: map a limited set of named colors to RGB
static int named_color_rgb(const char *name, int *r, int *g, int *b) {
  if (name == NULL)
    return -1;
  // normalize comparisons to lowercase using strcasecmp alternative
  // Since we must keep C99 and avoid non-standard, implement simple checks
  // assuming inputs match cases used in .geo samples.
  if (strcmp(name, "black") == 0) {
    *r = 0;
    *g = 0;
    *b = 0;
    return 0;
  } else if (strcmp(name, "white") == 0) {
    *r = 255;
    *g = 255;
    *b = 255;
    return 0;
  } else if (strcmp(name, "red") == 0) {
    *r = 255;
    *g = 0;
    *b = 0;
    return 0;
  } else if (strcmp(name, "green") == 0) {
    *r = 0;
    *g = 128;
    *b = 0;
    return 0;
  } else if (strcmp(name, "blue") == 0) {
    *r = 0;
    *g = 0;
    *b = 255;
    return 0;
  } else if (strcmp(name, "yellow") == 0) {
    *r = 255;
    *g = 255;
    *b = 0;
    return 0;
  } else if (strcmp(name, "pink") == 0) {
    *r = 255;
    *g = 192;
    *b = 203;
    return 0;
  } else if (strcmp(name, "cyan") == 0) {
    *r = 0;
    *g = 255;
    *b = 255;
    return 0;
  } else if (strcmp(name, "orange") == 0) {
    *r = 255;
    *g = 165;
    *b = 0;
    return 0;
  } else if (strcmp(name, "teal") == 0) {
    *r = 0;
    *g = 128;
    *b = 128;
    return 0;
  } else if (strcmp(name, "purple") == 0) {
    *r = 128;
    *g = 0;
    *b = 128;
    return 0;
  }
  return -1;
}

char *invert_color(const char *color) {
  if (color == NULL)
    return NULL;
  int r = 0, g = 0, b = 0;
  int ok = parse_hex_color(color, &r, &g, &b);
  if (ok != 0) {
    if (named_color_rgb(color, &r, &g, &b) != 0) {
      // Unrecognized: return duplicate of input
      return duplicate_string(color);
    }
  }

  int ir = 255 - r;
  int ig = 255 - g;
  int ib = 255 - b;

  char *out = (char *)malloc(8); // "#RRGGBB" + NUL
  if (out == NULL)
    return NULL;
  // snprintf is C99
  int n = snprintf(out, 8, "#%02X%02X%02X", ir, ig, ib);
  if (n < 0 || n >= 8) {
    free(out);
    return NULL;
  }
  return out;
}