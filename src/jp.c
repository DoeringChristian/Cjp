#include "jp.h"

int jp_parse_ws(const char *cur){
    size_t i = 0;
    while(cur[i] == ' ' || cur[i] == '\n' || cur[i] == '\r' || cur[i] == '\t')
        i++;
    return i;
}

int jp_parse_sign(const char *cur){
    size_t i = 0;
    if(cur[i] == '+' || cur[i] == '-')
        i++;
    return i;
}

int jp_parse_onenine(const char *cur){
    size_t i = 0;
    if(cur[i] >= '1' && cur[i] <= '9')
        i++;
    return i;
}

int jp_parse_digit(const char *cur){
    size_t i = 0;
    if(cur[i] == '0')
        i++;
    else if(jp_parse_onenine(cur) == 1)
        i++;
    return i;
}

int jp_parse_digits(const char *cur){
    size_t i = 0;
    if(jp_parse_digit(&cur[0]) == 1){
        for(;jp_parse_digit(&cur[i]) == 1;i++);
    }
    return i;
}

int jp_parse_fraction(const char *cur){
    size_t i = 0;
    if(cur[i] == '.'){
        i++;
        i += jp_parse_digits(&cur[i]);
    }
    return i;
}

int jp_parse_exponent(const char *cur){
    size_t i = 0;
    if(cur[i] == 'E' || cur[i] == 'e'){
        i++;
        i += jp_parse_sign(&cur[i]);
        i += jp_parse_digits(&cur[i]);
    }
    return i;
}

int jp_parse_intager(const char *cur){
    size_t i = 0;
    i += jp_parse_sign(&cur[i]);
    i += jp_parse_digits(&cur[i]);
    return i;
}

int jp_parse_number(const char *cur){
    size_t i = 0;
    i += jp_parse_intager(&cur[i]);
    i += jp_parse_fraction(&cur[i]);
    i += jp_parse_exponent(&cur[i]);
    return i;
}

int jp_parse_hex(const char *cur){
    size_t i = 0;
    if(jp_parse_digit(&cur[i]) == 1)
        i++;
    else if(cur[i] >= 'A' && cur[i] <= 'F')
        i++;
    else if(cur[i] >= 'a' && cur[i] <= 'f')
        i++;
    return i;
}

int jp_parse_escape(const char *cur){
    size_t i = 0;
    size_t j = 0;
    if(cur[i] == '"')
        i++;
    else if(cur[i] == '\\')
        i++;
    else if(cur[i] == '/')
        i++;
    else if(cur[i] == 'b')
        i++;
    else if(cur[i] == 'f')
        i++;
    else if(cur[i] == 'n')
        i++;
    else if(cur[i] == 'r')
        i++;
    else if(cur[i] == 't')
        i++;
    else if(cur[i] == 'u'){
        i++;
        j += jp_parse_hex(&cur[i+j]);
        j += jp_parse_hex(&cur[i+j]);
        j += jp_parse_hex(&cur[i+j]);
        j += jp_parse_hex(&cur[i+j]);
        if(j == 3)
            i += j;
        else
            i = 0;
    }
    return i;
}

int jp_parse_character(const char *cur){
    size_t i = 0;
    size_t j = 0;
    if(cur[i] >= ' ' && cur[i] <= '~' && cur[i] != '"' && cur[i] != '\\'){
        i++;
    }
    else if(cur[i] == '\\'){
        i++;
        j = jp_parse_escape(&cur[i+j]);
        if(j == 1)
            i += j;
        else
            i = 0;
    }
    return i;
}

int jp_parse_characters(const char *cur){
    size_t i = 0;

    while(jp_parse_character(&cur[i]) == 1)
        i++;
    return i;
}

int jp_parse_string(const char *cur){
    size_t i = 0;
    size_t j = 0;
    if(cur[i] == '"')
        i++;
    if(i == 1)
        j += jp_parse_characters(&cur[i]);
    i += j;
    if(cur[i] == '"')
        return i+1;
    return 0;
}

int jp_parse_element(const char *cur){
    size_t i = 0;
    size_t j = 0;
    i += jp_parse_ws(&cur[i]);
    j = jp_parse_value(&cur[i]);
    if(j > 0){
        i += j;
        i += jp_parse_ws(&cur[i]);
        return i;
    }
    return 0;
}

int jp_parse_elements(const char *cur){
    size_t i = 0;
    size_t j = 0;
    while((j = jp_parse_element(&cur[i])) > 0){
        i += j;
        if(cur[i] == ',')
            i++;
        else
            break;
    }
    return i;
}

int jp_parse_array(const char *cur){
    size_t i = 0;
    size_t j = 0;
    if(cur[i] == '['){
        i++;
        if((j = jp_parse_elements(&cur[i])) > 0)
            i += j;
        else if((j = jp_parse_ws(&cur[i])) > 0)
            i += j;
        if(cur[i] == ']'){
            i++;
            return i;
        }
    }
    return 0;
}

int jp_parse_member(const char *cur){
    size_t i = 0;
    size_t j = 0;
    i += jp_parse_ws(&cur[i]);
    if((j = jp_parse_string(&cur[i])) > 0){
        i += j;
        i += jp_parse_ws(&cur[i]);
        if(cur[i] == ':'){
            i++;
            if((j = jp_parse_element(&cur[i])) > 0){
                i += j;
                return i;
            }
        }
    }
    return 0;
}

int jp_parse_members(const char *cur){
    size_t i = 0;
    size_t j = 0;
    while((j = jp_parse_member(&cur[i])) > 0){
        i += j;
        if(cur[i] == ',')
            i++;
        else
            break;
    }
    return i;
}

int jp_parse_object(const char *cur){
    size_t i = 0;
    size_t j = 0;
    if(cur[i] == '{'){
        i++;
        if((j = jp_parse_members(&cur[i])) > 0)
            i += j;
        else if((j = jp_parse_ws(&cur[i])) > 0)
            i += j;
        if(cur[i] == '}'){
            i++;
            return i;
        }
    }
    return 0;
}
int jp_parse_value(const char *cur){
    size_t i = 0;
    size_t j = 0;
    if((j = jp_parse_object(&cur[i])) > 0)
        i += j;
    else if((j = jp_parse_array(&cur[i])) > 0)
        i += j;
    else if((j = jp_parse_string(&cur[i])) > 0)
        i += j;
    else if((j = jp_parse_number(&cur[i])) > 0)
        i += j;
    else if(memcmp(&cur[i], "true", 4) == 0)
        i += 4;
    else if(memcmp(&cur[i], "false", 5) == 0)
        i += 5;
    else if(memcmp(&cur[i], "null", 4) == 0)
        i += 4;
    return i;
}
int jp_parse_json(const char *cur){
    return jp_parse_element(cur);
}


#if 0
char jp_state_getc(const struct jp_state *src, size_t offset){
    switch(src->type){
    case JP_TYPE_BUF:
        return src->buf[src->index + offset];
    case JP_TYPE_FILE:
        return 0;
    }
}
#endif





struct jp_state jp_state(const char *src){
    struct jp_state dst = {
        .valid = 1,
        .c = src, 
        .size = 0,
#if 0
        .type = JP_TYPE_BUF,
        .index = 0,
        .buf = src,
#endif
    };
#ifdef JP_CALCULATE_SIZE
    dst.size = jp_parse_json(src);
#endif
#ifdef JP_VALIDATE
    if(jp_parse_json(src) == 0)
        dst.valid = 0;
#endif
    return dst;
}

struct jp_element jp_element(const char *src){
    return (struct jp_element){.state = jp_state(src)};
}

struct jp_member jp_member(const char *src){
    return jp_element_member(jp_element(src));
}

struct jp_element jp_element_next(struct jp_element src){
    if(!src.state.valid)
        return (struct jp_element)JP_COPY(src);

    size_t i = 0;
    struct jp_element dst = JP_COPY(src);
    i += jp_parse_element(&src.state.c[i]);
    if(src.state.c[i] == ','){
        i++;
        dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
        dst.state.size = jp_parse_element(dst.state.c);
#endif
    }
    else
        dst.state.valid = 0;
    return dst;
}
struct jp_member jp_member_next(struct jp_member src){
    if(!src.state.valid)
        return (struct jp_member)JP_COPY(src);

    size_t i = 0;
    struct jp_member dst = JP_COPY(src);
    i += jp_parse_member(&src.state.c[i]);
    if(src.state.c[i] == ','){
        i++;
        dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
        dst.state.size = jp_parse_member(dst.state.c)
#endif 
    }
    else
        dst.state.valid = 0;
    return dst;
}
struct jp_value jp_element_value(struct jp_element src){
    if(!src.state.valid)
        return (struct jp_value)JP_COPY(src);

    size_t i = 0;
    struct jp_value dst = JP_COPY(src);
    i += jp_parse_ws(&src.state.c[i]);
    dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
    dst.state.size = jp_parse_value(dst.state.c);
#endif
    return dst;
}
struct jp_element jp_member_element(struct jp_member src){
    if(!src.state.valid)
        return (struct jp_element)JP_COPY(src);

    size_t i = 0;
    struct jp_element dst = JP_COPY(src);
    i += jp_parse_ws(&src.state.c[i]);
    i += jp_parse_string(&src.state.c[i]);
    i += jp_parse_ws(&src.state.c[i]);
    if(src.state.c[i] == ':'){
        i++;
        dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
        dst.state.size = jp_parse_element(dst.state.c);
#endif
    }
    else
        dst.state.valid = 0;
    return dst;
}
struct jp_member jp_object_member(struct jp_object src){
    if(!src.state.valid)
        return (struct jp_member)JP_COPY(src);

    size_t i = 0;
    struct jp_member dst = JP_COPY(src);
    if(src.state.c[i] == '{'){
        i++;
        dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
        dst.state.size = jp_parse_member(dst.state.c);
#endif 
    }
    else
        dst.state.valid = 0;
    return dst;
}
struct jp_element jp_array_element(struct jp_array src){
    if(!src.state.valid)
        return (struct jp_element)JP_COPY(src);

    size_t i = 0;
    struct jp_element dst = JP_COPY(src);
    i += jp_parse_ws(&src.state.c[i]);
    if(src.state.c[i] == '['){
        i++;
        dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
        dst.state.size = jp_parse_element(dst.state.c);
#endif
    }
    else
        dst.state.valid = 0;
    return dst;
}
struct jp_object jp_value_object(struct jp_value src){
    if(!src.state.valid)
        return (struct jp_object)JP_COPY(src);

    size_t i = 0;
    struct jp_object dst = JP_COPY(src);
    dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
    dst.state.size = jp_parse_object(dst.state.c);
#endif
    return dst;
}
struct jp_array jp_value_array(struct jp_value src){
    if(!src.state.valid)
        return (struct jp_array)JP_COPY(src);

    size_t i = 0;
    struct jp_array dst = JP_COPY(src);
    dst.state.c = &src.state.c[i];
#ifdef JP_CALCULATE_SIZE
    dst.state.size = jp_parse_array(dst.state.c);
#endif
    return dst;
}
#if 1
int jp_value_string(struct jp_value src, char *dst, size_t dst_size){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    if(src.state.c[i] == '"'){
        i++;
        j += jp_parse_string(&src.state.c[0]);
        memcpy(dst, &src.state.c[i], MIN(dst_size, j-2));
        return MIN(dst_size, j-2);
    }
    return -1;
}
int jp_value_number(const struct jp_value src, double *dst){
    if(!src.state.valid)
        return 0;

    int i = 0;
    sscanf(src.state.c, "%lf%n", dst, &i);
    return i-1;
}
int jp_member_name(const struct jp_member src, char *dst, size_t dst_size){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    i += jp_parse_ws(&src.state.c[i]);
    if(src.state.c[i] == '"'){
        j += jp_parse_string(&src.state.c[i]);
        memcpy(dst, &src.state.c[i], MIN(j, dst_size));
        return MIN(j, dst_size);
    }
    return -1;
}
#endif

struct jp_member jp_element_member(struct jp_element src){
    if(!src.state.valid)
        return (struct jp_member)JP_COPY(src);

    size_t i = 0;
    struct jp_member dst = JP_COPY(src);
    dst = jp_object_member(jp_value_object(jp_element_value(src)));
    return dst;
}

struct jp_array jp_element_array(struct jp_element src){
    return jp_value_array(jp_element_value(src));
}
struct jp_object jp_element_object(struct jp_element src){
    return jp_value_object(jp_element_value(src));
}
int jp_element_string(struct jp_element src, char *dst, size_t dst_size){
    return jp_value_string(jp_element_value(src), dst, dst_size);
}
int jp_element_number(struct jp_element src, double *dst){
    return jp_value_number(jp_element_value(src), dst);
}
int jp_member_namecomp(const struct jp_member src, const char *str){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    i += jp_parse_ws(&src.state.c[i]);
    j = jp_parse_string(&src.state.c[i]);
    if(strlen(str) == j-2){
        if(memcmp(str, &src.state.c[i+1], j-2) == 0)
            return 1;
    }
    return 0;
}
struct jp_member jp_members_search(struct jp_member src, const char *str){
    if(!src.state.valid)
        return (struct jp_member)JP_COPY(src);

    size_t i = 0, j = 0;
    for(;src.state.valid; src = jp_member_next(src)){
        if(jp_member_namecomp(src, str))
            return src;
    }
    return (struct jp_member){.state = (struct jp_state){.c = src.state.c, .valid = 0}};
}
struct jp_member jp_member_member(struct jp_member src){
    return jp_element_member(jp_member_element(src));
}
struct jp_array jp_member_array(struct jp_member src){
    return jp_element_array(jp_member_element(src));
}
int jp_member_string(struct jp_member src, char *dst, size_t dst_size){
    return jp_element_string(jp_member_element(src), dst, dst_size);
}
int jp_member_number(struct jp_member src, double *dst){
    return jp_element_number(jp_member_element(src), dst);
}
enum jp_tfn jp_value_tfn(struct jp_value src){
    if(!src.state.valid)
        return JP_TFN_ERR;

    if(memcmp(src.state.c, "true", 4) == 0)
        return JP_TFN_TRUE;
    else if(memcmp(src.state.c, "false", 5) == 0)
        return JP_TFN_FALSE;
    else if(memcmp(src.state.c, "null", 4) == 0)
        return JP_TFN_NULL;
    else
        return JP_TFN_ERR;
}
enum jp_tfn jp_element_tfn(struct jp_element src){
    return jp_value_tfn(jp_element_value(src));
}
enum jp_tfn jp_member_tfn(struct jp_member src){
    return jp_value_tfn(jp_member_value(src));
}
struct jp_value jp_member_value(struct jp_member src){
    return jp_element_value(jp_member_element(src));
}
int jp_member_namelen(struct jp_member src){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    i += jp_parse_ws(&src.state.c[i]);
    j = jp_parse_string(&src.state.c[i]);
    return j-2;
}
int jp_value_strlen(struct jp_value src){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    i += jp_parse_ws(&src.state.c[i]);
    j = jp_parse_string(&src.state.c[i]);
    return j-2;
}
int jp_element_strlen(struct jp_element src){
    return jp_value_strlen(jp_element_value(src));
}
int jp_member_strlen(struct jp_member src){
    return jp_element_strlen(jp_member_element(src));
}
