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
    const char *c;
    int size;
#if 0
    enum jp_type type;
    size_t index;
    union{
        const char *buf;
        FILE *fp;
    };
#endif
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

char jp_state_getc(const struct jp_state *src, size_t offset);

struct jp_state jp_state(const char *src);

struct jp_element jp_element(const char *src);

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


int jp_parse_ws(const char *cur);
int jp_parse_sign(const char *cur);
int jp_parse_onenine(const char *cur);
int jp_parse_digit(const char *cur);
int jp_parse_digits(const char *cur);
int jp_parse_fraction(const char *cur);
int jp_parse_exponent(const char *cur);
int jp_parse_intager(const char *cur);
int jp_parse_number(const char *cur);
int jp_parse_hex(const char *cur);
int jp_parse_escape(const char *cur);
int jp_parse_character(const char *cur);
int jp_parse_characters(const char *cur);
int jp_parse_string(const char *cur);

int jp_parse_element(const char *cur);
int jp_parse_elements(const char *cur);
int jp_parse_array(const char *cur);
int jp_parse_member(const char *cur);
int jp_parse_members(const char *cur);
int jp_parse_object(const char *cur);
int jp_parse_value(const char *cur);
int jp_parse_json(const char *cur);

int jp_string_read(const char *cur, char *dst, size_t dst_size);
int jp_string_len(const char *cur);
int jp_string_comp(const char *cur, const char *str);

int jp_utf8_encode(char *dst, uint32_t utf);
int jp_atix32(const char *src, uint32_t *dst);

#endif //JP_H
