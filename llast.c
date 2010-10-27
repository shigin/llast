#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_OFFSET (64*1024)
#define DEFAULT_FORMAT "%Y-%m-%d %H:%M:%S"
static char *progname;
static void die(const char *message) {
    fprintf(stderr, "%s: %s\n", progname, message);
    exit(1);
}

static void find_line_start(FILE *file) {
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') break;
    }
}

time_t read_time(FILE *file, char *format);

int main(int argc, char **argv) {
    int param;
    long old_pos;
    unsigned seconds = 0;
    char *endptr;
    char *format = strdup(DEFAULT_FORMAT);
    time_t now = time(NULL);

    progname = *argv;
    while ((param = getopt(argc, argv, "s:f:h")) != -1) {
        switch (param) {
          case 's':
            seconds = strtol(optarg, &endptr, 10);
            if (*endptr != '\0') {
                die("can't parse seconds");
            }
            break;
          case 'f':
            format = strdup(optarg);
            break;
          case '?':
            die("unknown option");
          default:
            die("unknown option");
        }
    }

    if (optind == argc) {
        die("need at least one file to process");
    }

    for (; optind < argc; ++optind) {
        char buffer[DEFAULT_OFFSET];
        size_t r;
        FILE *file = fopen(argv[optind], "r");

        if (file == 0) {
            fprintf(stderr, "%s: can't open '%s': %s\n",
                progname, argv[optind], strerror(errno));
            continue;
        }
        if (fseek(file, -DEFAULT_OFFSET, SEEK_END) == -1) {
            if (fseek(file, 0, SEEK_END) != -1) {
                fseek(file, 0, SEEK_SET); /* it's short file */
            } else {
                fprintf(stderr, "%s: can't seek in '%s': %s\n",
                    progname, argv[optind], strerror(errno));
                continue;
            }
        }
        if (ftell(file) != 0) {
            find_line_start(file);
        }

        while (ftell(file) != 0) {
            time_t current = read_time(file, format);
            if (current == 0) {
                /* can't match line */
                fprintf(stderr, "match failed\n");
                break;
            } else {
                if (current < now - seconds) {
                    break;
                }
            }
            if (fseek(file, -DEFAULT_OFFSET, SEEK_CUR) == -1) {
                /* it's file start  */
                break;
            }
            if (ftell(file) != 0) {
                find_line_start(file);
            }
        }

        while (!feof(file)) {
            old_pos = ftell(file);
            time_t current = read_time(file, format);
            find_line_start(file);
            if (old_pos == ftell(file)) break;
            if (current == 0) continue;
            if (current >= now - seconds) {
                fseek(file, old_pos, SEEK_SET);
                break;
            }
        }

        while ((r = fread(buffer, 1, DEFAULT_OFFSET, file)) != 0) {
            fwrite(buffer, 1, r, stdout);
        }
    }

    free(format);
    return 0;
}
