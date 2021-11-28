#ifndef JP_H
#define JP_H

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define JP_COPY(_x) {.state = (_x).state};

#ifndef MIN
#define MIN(_x, _y) ((_x) > (_y) ? (_y) : (_x))
#endif

#define jp_members_foreach(_members, _iter) for(struct jp_member (_iter) = _members; (_iter).state.valid; (_iter) = jp_member_next((_iter)))

#define jp_elements_foreach(_elements, _iter) for(struct jp_element (_iter) = _elements; (_iter).state.valid; (_iter) = jp_element_next((_iter)))

#define jp_array_foreach(_array, _iter) for(struct jp_element (_iter) = jp_array_element(_array); (_iter).state.valid; (_iter) = jp_element_next((_iter)))

// define JP_VALIDATE to enable validation
// define JP_CALCULATE_SIZE to calculate the size of each parse object.

enum jp_tfn{
    JP_TFN_TRUE,
    JP_TFN_FALSE,
    JP_TFN_NULL,
    JP_TFN_ERR,
};

enum jp_type{
    JP_TYPE_BUF,
    JP_TYPE_FILE,
};

static const double d_nan = (0.0f / 0.0f);

struct jp_state{
    int valid;
    int size;
    enum jp_type type;
    size_t index;
    union{
        struct{
            const char *buf;
        };
        struct{
            FILE *fp;
        };
    };
};

struct jp_object{
    struct jp_state state;
};

struct jp_element{
    struct jp_state state;
};

struct jp_value{
    struct jp_state state;
};

struct jp_member{
    struct jp_state state;
};

struct jp_array{
    struct jp_state state;
};

char jp_state_getc(const struct jp_state src, size_t offset);
struct jp_state jp_state_offset(struct jp_state src, size_t offset);
int jp_state_memcmp(struct jp_state src, const void *src2, size_t src_size);

struct jp_state jp_state(const char *src);
struct jp_state jp_state_file(FILE *fp);

struct jp_element jp_element(const char *src);
struct jp_element jp_element_file(FILE *fp);

struct jp_element jp_element_next(struct jp_element src);
struct jp_value jp_element_value(struct jp_element src);
struct jp_member jp_element_member(struct jp_element src);
struct jp_member jp_member_next(struct jp_member src);
struct jp_element jp_member_element(struct jp_member src);
struct jp_array jp_element_array(struct jp_element src);
struct jp_object jp_element_object(struct jp_element src);
int jp_element_strlen(struct jp_element src);
int jp_element_string(struct jp_element src, char *dst, size_t dst_size);
int jp_element_number(struct jp_element src, double *dst);
enum jp_tfn jp_element_tfn(struct jp_element src);

struct jp_member jp_member(const char *src);

struct jp_member jp_members_search(struct jp_member src, const char *str);
struct jp_member jp_member_member(struct jp_member src);
struct jp_array jp_member_array(struct jp_member src);
enum jp_tfn jp_member_tfn(struct jp_member src);
struct jp_value jp_member_value(struct jp_member src);
int jp_member_namelen(struct jp_member src);
int jp_member_name(struct jp_member src, char *dst, size_t dst_size);
int jp_member_namecomp(const struct jp_member src, const char *str);
int jp_member_strlen(struct jp_member src);
int jp_member_string(struct jp_member src, char *dst, size_t dst_size);
int jp_member_number(struct jp_member src, double *dst);

struct jp_member jp_object_member(struct jp_object src);

struct jp_element jp_array_element(struct jp_array src);

struct jp_object jp_value_object(struct jp_value src);
struct jp_array jp_value_array(struct jp_value src);
int jp_value_strlen(struct jp_value src);
int jp_value_string(struct jp_value src, char *dst, size_t dst_size);
int jp_value_number(struct jp_value src, double *dst);
enum jp_tfn jp_value_tfn(struct jp_value src);


int jp_parse_ws(struct jp_state src);
int jp_parse_sign(struct jp_state src);
int jp_parse_onenine(struct jp_state src);
int jp_parse_digit(struct jp_state src);
int jp_parse_digits(struct jp_state src);
int jp_parse_fraction(struct jp_state src);
int jp_parse_exponent(struct jp_state src);
int jp_parse_intager(struct jp_state src);
int jp_parse_number(struct jp_state src);
int jp_parse_hex(struct jp_state src);
int jp_parse_escape(struct jp_state src);
int jp_parse_character(struct jp_state src);
int jp_parse_characters(struct jp_state src);
int jp_parse_string(struct jp_state src);

int jp_parse_element(struct jp_state src);
int jp_parse_elements(struct jp_state src);
int jp_parse_array(struct jp_state src);
int jp_parse_member(struct jp_state src);
int jp_parse_members(struct jp_state src);
int jp_parse_object(struct jp_state src);
int jp_parse_value(struct jp_state src);
int jp_parse_json(struct jp_state src);

int jp_string_read(struct jp_state src, char *dst, size_t dst_size);
int jp_string_len(struct jp_state src);
int jp_string_comp(struct jp_state src, const char *str);

int jp_utf8_encode(char *dst, uint32_t utf);
int jp_atoix32(struct jp_state src, uint32_t *dst);
int jp_atod(struct jp_state src, double *dst);

#endif //JP_H
