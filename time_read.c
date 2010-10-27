#define _XOPEN_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_LINE 1024

static int skip_word(char **buffer) {
    while (!isspace(**buffer)) {
        if (**buffer == '\0') return 0;
        ++(*buffer);
    }
    return 1;
}

time_t read_time_buffer(char *buffer, char *format) {
    struct tm tms;
    memset(&tms, 0, sizeof(tms));

    while (*format) {
        char *at;
        char *ptime;
        char save;

        switch (*format) {
          case '\\':
            ++format;
            switch (*format) {
              case 'w':
                if (!skip_word(&buffer))
                    return 0;
                break;
              default:
                fprintf(stderr, "read_time: invalid special specification: %c\n",
                    *format);
                return 0;
            }
            ++format;
            break;
          case '%':
            at = strchr(format, '\\');
            if (at) {
                save = *at;
                *at = '\0';
            }
            ptime = strptime(buffer, format, &tms);
            if (at) {
                *at = save;
                format = at;
            }
            
            if (ptime == NULL) {
                return 0;
            }
            buffer = ptime;
            if (!at) {
                return mktime(&tms);
            }
            break;
          default:
            if (*format == *buffer) {
                ++format;
                ++buffer;
            } else
                return 0;
        }
    }
    return mktime(&tms);
}

time_t read_time(FILE *file, char *format) {
    char _buffer[MAX_LINE];
    char *buffer = _buffer;

    fgets(buffer, MAX_LINE, file);
    //fprintf(stderr, "match %s with %s\n", buffer, format);
    ungetc('\n', file);
    return read_time_buffer(buffer, format);
}
