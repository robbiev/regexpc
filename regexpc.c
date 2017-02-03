#include <stdio.h>
#include <string.h>

int matchstar(int c, char *regexp, char *text);
int matchstar2(int c, char *regexp, char *text);
int matchhere(char *regexp, char *text);
int match(char *regexp, char *text);

/* leftmost longest match, needed for find/replace */
int matchstar2(int c, char *regexp, char *text) {
  char *t;

  for (t = text; *t != '\0' && (*t == c || c == '.'); t++) {}

  do {
    if (matchhere(regexp, t)) {
      return 1;
    }
  } while (t-- > text);

  return 0;
}

/* leftmost first match */
int matchstar(int c, char *regexp, char *text) {
  /* do/while because a star can match zero characters */
  do {
    /* match until we find a match against the next part of the regex */
    if (matchhere(regexp, text)) {
      return 1;
    }
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}

/* search for regexp at beginning of text */
int matchhere(char *regexp, char *text) {
  if (regexp[0] == '\0') {
    return 1;
  }

  /* a character with a star applied */
  if (regexp[1] == '*') {
    return matchstar(regexp[0], regexp+2, text);
  }

  /* match the end of the string */
  if (regexp[0] == '$' && regexp[1] == '\0') {
    return *text == '\0';
  }

  /* match any character or exact character */
  if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text)) {
    return matchhere(regexp+1, text+1);
  }
  return 0;
}

/* search for regexp anywhere in text */
int match(char *regexp, char *text) {
  if (regexp[0] == '^') {
    return matchhere(regexp+1, text);
  }
  do { /* must look even if string is empty */
    if (matchhere(regexp, text)) {
      return 1;
    }
  } while (*text++ != '\0');
  return 0;
}

int grep(char *regexp, FILE *f, char *fname) {
  int n, nmatch;
  char buf[BUFSIZ];

  nmatch = 0;

  while (fgets(buf, sizeof buf, f) != NULL) {
    n = strlen(buf);
    if (n > 0 && buf[n-1] == '\n') {
      buf[n-1] = '\0';
    }
    if (match(regexp, buf)) {
      nmatch++;
      if (fname != NULL) {
        printf("%s: ", fname);
      }
      printf("%s\n", buf);
    }
  }
  return nmatch;
}

/* 
 * Simple grep from stdin. Only ^.*$ are supported.
 * exponential: a*a*a*a*a*b with input aaaaaaaaac as this will descend into
 * matchstar for each a* and for each iterate over the entire word only to find
 * at the end that the string doesn't match.
 */
int main(int argc, char* argv[]) {
  int nmatch;

  if (argc != 2) {
    printf("nope\n");
    return 1;
  }
  nmatch = 0;

  if (grep(argv[1], stdin, NULL)) {
    nmatch++;
  }

  return nmatch == 0;
}
