#define _XOPEN_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_LINE 1024

time_t read_time_buffer(char *buffer, char *format) {
    int special = 0;
    struct tm tms;
    memset(&tms, 0, sizeof(tms));

    while (*format) {
        char *at;
        char *ptime;
        char save;

        if (special) {
            switch (*format) {
              case 'w':
                if (*buffer == '\0') return 0;
                if (!isspace(*buffer)) {
                    ++buffer;
                } else {
                    ++format;
                    special = 0;
                }
                break;
              default:
                fprintf(stderr, "read_time: invalid special specification: %c\n",
                    *format);
                return 0;
            }
        } else switch (*format) {
          case '\\':
            special = 1;
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
