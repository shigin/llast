#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t read_time(FILE *file, char *format);
time_t read_time_buffer(char *buffer, char *format);

static char *tb(time_t t) {
    char *s = strdup(ctime(&t));
    s[strlen(s) - 1] = '\0'; /* chop \n */
    return s;
}

static void _ta(const char *file, unsigned line, time_t l, time_t r) {
    if (l != r) {
        char *ls = tb(l);
        char *rs = tb(r);
        fprintf(stderr, "%s:%u: assertion failed %s != %s (%ld != %ld)\n",
            file, line, ls, rs, l, r);
        free(ls);
        free(rs);
        abort();
    }
}

#define ta(l, r) _ta(__FILE__, __LINE__, l, r)

time_t gt(const char *x) {
    struct tm tms;
    memset(&tms, 0, sizeof(tms));
    strptime(x, "%Y-%m-%d %H:%M:%S", &tms);
    return mktime(&tms);
}

void _ta2(const char *file, unsigned line,
        const char *ll, const char *lf, const char *r) {
    char *_ll = strdup(ll);
    char *_lf = strdup(lf);

    _ta(file, line, read_time_buffer(_ll, _lf), gt(r));
    free(_ll);
    free(_lf);
}

#define ta2(l, l2, r) _ta2(__FILE__, __LINE__, l, l2, r)
int main() {
    ta2("2010-01-01 00:12:12", "%Y-%m-%d %H:%M:%S", "2010-01-01 00:12:12");
    ta2("123.231.12.1 2010-01-01 00:12:12", "\\w %Y-%m-%d %H:%M:%S", "2010-01-01 00:12:12");
    ta2("www 2010-01-01 ddd 00:12:12", "\\w %Y-%m-%d \\w %H:%M:%S", "2010-01-01 00:12:12");
    ta2("2010-01-01\\01:12:12", "%Y-%m-%d\\\\%H:%M:%S", "2010-01-01 01:12:12");
    ta2("01-01 03:12:12", "%m-%d %H:%M:%S", "2010-01-01 03:12:12");
    return 0;
}
