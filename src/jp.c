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
        if(j == 4)
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
        if(j > 0)
            i += j;
        else
            i = 0;
    }
    return i;
}

int jp_parse_characters(const char *cur){
    size_t i = 0, j = 0;

    while((j = jp_parse_character(&cur[i])) > 0)
        i += j;
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

int jp_string_read(const char *cur, char *dst, size_t dst_size){
    size_t i = 0, j = 0;
    uint32_t utf = 0;
    if(cur[i] == '"'){
        i++;
        int escape = 0;
        for(j = 0;escape || cur[i] != '"' & j < dst_size;i++){
            char c = cur[i];
            if(c == '\\')
                escape = 1;
            else if(escape){
                switch(c){
                case '"':
                    dst[j] = '"';
                    j++;
                    break;
                case '\\':
                    dst[j] = '\\';
                    j++;
                    break;
                case '/':
                    dst[j] = '/';
                    j++;
                    break;
                case 'b':
                    dst[j] = '\b';
                    j++;
                    break;
                case 'f':
                    dst[j] = '\f';
                    j++;
                    break;
                case 'n':
                    dst[j] = '\n';
                    j++;
                    break;
                case 'r':
                    dst[j] = '\r';
                    j++;
                    break;
                case 't':
                    dst[j] = '\t';
                    j++;
                    break;
                case 'u':
                    jp_atix32(&cur[i], &utf);
                    i += 4;
                    j += jp_utf8_encode(&dst[j], utf);
                    break;
                }
                escape = 0;
            }
            else{
                dst[j] = c;
                j++;
            }
        }
        return j;
    }
    return 0;
}
int jp_string_len(const char *cur){
    size_t i = 0, j = 0;
    uint32_t utf = 0;
    if(cur[i] == '"'){
        i++;
        int escape = 0;
        for(j = 0;escape || cur[i] != '"';i++){
            char c = cur[i];
            if(c == '\\')
                escape = 1;
            else if(escape){
                switch(c){
                case '"':
                    j++;
                    break;
                case '\\':
                    j++;
                    break;
                case '/':
                    j++;
                    break;
                case 'b':
                    j++;
                    break;
                case 'f':
                    j++;
                    break;
                case 'n':
                    j++;
                    break;
                case 'r':
                    j++;
                    break;
                case 't':
                    j++;
                    break;
                case 'u':
                    jp_atix32(&cur[i], &utf);
                    i += 4;
                    if(utf <= 0xF7)
                        j += 1;
                    else if(utf <= 0x07FF)
                        j += 2;
                    else if(utf <= 0xFFFF)
                        j += 3;
                    else if(utf <= 0x10FFFF)
                        j += 4;
                    break;
                }
                escape = 0;
            }
            else{
                j++;
            }
        }
        return j;
    }
    return 0;
}
int jp_string_comp(const char *cur, const char *str){
    size_t i = 0, j = 0;
    uint32_t utf = 0;
    size_t str_len = strlen(str);
    if(cur[i] == '"'){
        i++;
        int escape = 0;
        for(j = 0;escape || cur[i] != '"';i++){
            if(j >= str_len)
                return 0;
            char c = cur[i];
            if(c == '\\')
                escape = 1;
            else if(escape){
                switch(c){
                case '"':
                    if(str[j] != '"')
                        return 0;
                    j++;
                    break;
                case '\\':
                    if(str[j] != '\\')
                        return 0;
                    j++;
                    break;
                case '/':
                    if(str[j] != '/')
                        return 0;
                    j++;
                    break;
                case 'b':
                    if(str[j] != '\b')
                        return 0;
                    j++;
                    break;
                case 'f':
                    if(str[j] != '\f')
                        return 0;
                    j++;
                    break;
                case 'n':
                    if(str[j] != '\n')
                        return 0;
                    j++;
                    break;
                case 'r':
                    if(str[j] != '\r')
                        return 0;
                    j++;
                    break;
                case 't':
                    if(str[j] != '\t')
                        return 0;
                    j++;
                    break;
                case 'u':
                    jp_atix32(&cur[i], &utf);
                    i += 4;
                    char buf[4];
                    size_t n = jp_utf8_encode(&buf[j], utf);
                    for(size_t k = 0;k < n;k++)
                        if(buf[k] != str[n+j])
                            return 0;
                    j += n;
                    break;
                }
                escape = 0;
            }
            else{
                if(str[j] != c)
                    return 0;
                j++;
            }
        }
    }
    if(j < str_len)
        return 0;
    return 1;
}

int jp_utf8_encode(char *dst, uint32_t utf){
    if(utf <= 0xF7){
        dst[0] = (char)utf;
        return 1;
    }
    else if(utf <= 0x07FF){
        dst[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
        dst[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
        return 2;
    }
    else if(utf <= 0xFFFF){
        dst[0] = (char) (((utf >>12) & 0x0F) | 0xE0);
        dst[1] = (char) (((utf >> 6) & 0x3F) | 0x80);
        dst[2] = (char) (((utf >> 0) & 0x3F) | 0x80);
        return 3;
    }
    else if(utf <= 0x10FFFF){
        dst[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
        dst[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
        dst[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
        dst[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
        return 4;
    }
    else{
        dst[0] = (char) 0xEF;
        dst[1] = (char) 0xBF;
        dst[2] = (char) 0xBD;
        return 3;
    }
}
int jp_atix32(const char *src, uint32_t *dst){
    *dst = 0;
    for(size_t i = 0;i < 4;i++){
        *dst *= 0x10;
        if(src[i] >= 0 && src[i] <= 9)
            *dst += src[i] - '0';
        else if(src[i] >= 'a' && src[i] <= 'f')
            *dst += src[i] - 'a' + 10;
        else if(src[i] >= 'A' && src[i] >= 'F')
            *dst += src[i] - 'A' + 10;
    }
    return 1;
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
        i += jp_parse_ws(&src.state.c[i]);
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
        i += jp_parse_ws(&src.state.c[i]);
        if(src.state.c[i] == '}')
            dst.state.valid = 0;
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
        i += jp_parse_ws(&src.state.c[i]);
        if(src.state.c[i] == ']')
            dst.state.valid = 0;
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

    size_t i = 0;
    i += jp_parse_ws(&src.state.c[i]);
    i += jp_string_read(&src.state.c[i], dst, dst_size);
    return i;
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

    size_t i = 0;
    i += jp_string_read(&src.state.c[i], dst, dst_size);
    return i;
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
    struct jp_value v = jp_element_value(src);
    return jp_value_string(v, dst, dst_size);
}
int jp_element_number(struct jp_element src, double *dst){
    return jp_value_number(jp_element_value(src), dst);
}
int jp_member_namecomp(const struct jp_member src, const char *str){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    i += jp_parse_ws(&src.state.c[i]);
    if(!jp_string_comp(&src.state.c[i], str))
        return 0;
    return 1;
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
    j = jp_string_len(&src.state.c[i]);
    return j;
}
int jp_value_strlen(struct jp_value src){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    i += jp_parse_ws(&src.state.c[i]);
    j = jp_string_len(&src.state.c[i]);
    return j;
}
int jp_element_strlen(struct jp_element src){
    return jp_value_strlen(jp_element_value(src));
}
int jp_member_strlen(struct jp_member src){
    return jp_element_strlen(jp_member_element(src));
}
