#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    fmt_INT,
    fmt_DOUBLE,
    fmt_STRING,
    fmt_BOOL,
} fmt_arg_type;

typedef struct {
    fmt_arg_type type;
    union {
        int i;
        double d;
        const char *s;
        bool b;
    } data;
} fmt_arg;

static inline fmt_arg fmt__int(int x) { return (fmt_arg){fmt_INT, .data.i = x}; }
static inline fmt_arg fmt__double(double x) { return  (fmt_arg){fmt_DOUBLE, .data.d = x}; }
static inline fmt_arg fmt__string(const char *x) { return (fmt_arg){fmt_STRING, .data.s = x}; }
static inline fmt_arg fmt__bool(bool x) { return (fmt_arg){fmt_BOOL, .data.b = x}; }

#define fmt_arg(x) _Generic((x), \
                int: fmt__int, \
                long: fmt__int, \
                double: fmt__double, \
                float: fmt__double, \
                char*: fmt__string, \
                const char*: fmt__string,\
                bool: fmt__bool \
            )(x)

void fmt_impl(const char* format_str, const fmt_arg* args, int arg_count) {
    const char* p = format_str;
    int auto_index = 0; // Tracks sequential usage when {} is empty
    
    while (*p != '\0') {
        /* escape double braces {{ */
        if (*p == '{' && *(p + 1) == '{') {
            putchar('{');
            p += 2;
            continue;
        }
        
        /* escape double closing braces }} */
        if (*p == '}' && *(p + 1) == '}') {
            putchar('}');
            p += 2;
            continue;
        }
        
        if (*p == '{') {
            const char* start = p + 1;
            const char* end = strchr(start, '}');
            
            if (end != NULL) {
                int spec_len = end - start;
                char spec[64] = {0};
                if (spec_len > 0 && spec_len < sizeof(spec)) {
                    strncpy(spec, start, spec_len);
                }
                
                int target_index = 0;
                int precision = -1;
                
                /* if braces are completely empty {} use the automatic sequential counter */
                if (spec_len == 0) {
                    target_index = auto_index++;
                } else {
                    /* else parse the positional index and check for modifiers */
                    target_index = atoi(spec);
                    
                    char* separator = strchr(spec, ':');
                    if (!separator) {
                        separator = strchr(spec, '.');
                    }
                    
                    if (separator != NULL) {
                        char* dot = strchr(separator, '.');
                        if (dot != NULL) {
                            precision = atoi(dot + 1);
                        } else if (*separator == ':') {
                            precision = atoi(separator + 1);
                        }
                    }
                }
                
                if (target_index >= 0 && target_index < arg_count) {
                    fmt_arg arg = args[target_index];
                    
                    switch (arg.type) {
                        case fmt_INT:
                            printf("%d", arg.data.i);
                            break;
                        case fmt_DOUBLE:
                            if (precision >= 0) {
                                char fmt_buf[16];
                                snprintf(fmt_buf, sizeof(fmt_buf), "%%.%df", precision);
                                printf(fmt_buf, arg.data.d);
                            } else {
                                printf("%f", arg.data.d);
                            }
                            break;
                        case fmt_STRING:
                            printf("%s", arg.data.s);
                            break;
                        case fmt_BOOL:
                            printf("%s", arg.data.b ? "true" : "false");
                            break;
                    }
                } else {
                    putchar('{');
                    fwrite(start, 1, spec_len, stdout);
                    putchar('}');
                }
                
                p = end + 1;
                continue;
            }
        }
        
        putchar(*p);
        p++;
    }
}

#define fmt(format_str, ...) \
    fmt_impl(format_str, (fmt_arg[]){ __VA_ARGS__ }, sizeof((fmt_arg[]){ __VA_ARGS__ }) / sizeof(fmt_arg))

int main(void) {

    fmt("Config data: {{ \"name\": \"{0}\", \"version\": {1}, \"radius\": {2:.3} }}\n",
            fmt_arg("re"),
            fmt_arg(2),
            fmt_arg(123.321321321312));

    fmt("this {} so {}, king\n",
            fmt_arg("is"),
            fmt_arg(true));

    return 0;
}
