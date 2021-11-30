#include "jp.h"

int jp_parse_ws(struct jp_state src){
    size_t i = 0;
    char c;
    while((c = jp_state_getat(src, i)) == ' ' || c == '\n' || c == '\r' || c == '\t')
        i++;
    return i;
}

int jp_parse_sign(struct jp_state src){
    size_t i = 0;
    char c;
    if((c = jp_state_getat(src, i)) == '+' || c == '-')
        i++;
    return i;
}

int jp_parse_onenine(struct jp_state src){
    size_t i = 0;
    char c;
    if((c = jp_state_getat(src, i)) >= '1' && c <= '9')
        i++;
    return i;
}

int jp_parse_digit(struct jp_state src){
    size_t i = 0;
    if(jp_state_getat(src, i) == '0')
        i++;
    else if(jp_parse_onenine(src) == 1)
        i++;
    return i;
}

int jp_parse_digits(struct jp_state src){
    size_t i = 0;
    if(jp_parse_digit(jp_state_offset(src, i))){
        for(;jp_parse_digit(jp_state_offset(src, i)) == 1;i++);
    }
    return i;
}

int jp_parse_fraction(struct jp_state src){
    size_t i = 0;
    if(jp_state_getat(src, i) == '.'){
        i++;
        i += jp_parse_digits(jp_state_offset(src, i));
    }
    return i;
}

int jp_parse_exponent(struct jp_state src){
    size_t i = 0;
    if(jp_state_getat(src, i) == 'E' || jp_state_getat(src, i) == 'e'){
        i++;
        i += jp_parse_sign(jp_state_offset(src, i));
        i += jp_parse_digits(jp_state_offset(src, i));
    }
    return i;
}

int jp_parse_intager(struct jp_state src){
    size_t i = 0;
    i += jp_parse_sign(jp_state_offset(src, i));
    i += jp_parse_digits(jp_state_offset(src, i));
    return i;
}

int jp_parse_number(struct jp_state src){
    size_t i = 0;
    i += jp_parse_intager(jp_state_offset(src, i));
    i += jp_parse_fraction(jp_state_offset(src, i));
    i += jp_parse_exponent(jp_state_offset(src, i));
    return i;
}

int jp_parse_hex(struct jp_state src){
    size_t i = 0;
    if(jp_parse_digit(jp_state_offset(src, i)) == 1)
        i++;
    else if(jp_state_getat(src, i) >= 'A' && jp_state_getat(src, i) <= 'F')
        i++;
    else if(jp_state_getat(src, i) >= 'a' && jp_state_getat(src, i) <= 'f')
        i++;
    return i;
}

int jp_parse_escape(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    if(jp_state_getat(src, i) == '"')
        i++;
    else if(jp_state_getat(src, i) == '\\')
        i++;
    else if(jp_state_getat(src, i) == '/')
        i++;
    else if(jp_state_getat(src, i) == 'b')
        i++;
    else if(jp_state_getat(src, i) == 'f')
        i++;
    else if(jp_state_getat(src, i) == 'n')
        i++;
    else if(jp_state_getat(src, i) == 'r')
        i++;
    else if(jp_state_getat(src, i) == 't')
        i++;
    else if(jp_state_getat(src, i) == 'u'){
        i++;
        j += jp_parse_hex(jp_state_offset(src, i+j));
        j += jp_parse_hex(jp_state_offset(src, i+j));
        j += jp_parse_hex(jp_state_offset(src, i+j));
        j += jp_parse_hex(jp_state_offset(src, i+j));
        if(j == 4)
            i += j;
        else
            i = 0;
    }
    return i;
}

int jp_parse_character(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    if(jp_state_getat(src, i) >= ' ' && jp_state_getat(src, i) <= '~' && jp_state_getat(src, i) != '"' && jp_state_getat(src, i) != '\\'){
        i++;
    }
    else if(jp_state_getat(src, i) == '\\'){
        i++;
        j = jp_parse_escape(jp_state_offset(src, i+j));
        if(j > 0)
            i += j;
        else
            i = 0;
    }
    return i;
}

int jp_parse_characters(struct jp_state src){
    size_t i = 0, j = 0;

    while((j = jp_parse_character(jp_state_offset(src, i))) > 0)
        i += j;
    return i;
}

int jp_parse_string(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    if(jp_state_getat(src, i) == '"')
        i++;
    if(i == 1)
        j += jp_parse_characters(jp_state_offset(src, i));
    i += j;
    if(jp_state_getat(src, i) == '"')
        return i+1;
    return 0;
}

int jp_parse_element(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    i += jp_parse_ws(jp_state_offset(src, i));
    j = jp_parse_value(jp_state_offset(src, i));
    if(j > 0){
        i += j;
        i += jp_parse_ws(jp_state_offset(src, i));
        return i;
    }
    return 0;
}

int jp_parse_elements(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    while((j = jp_parse_element(jp_state_offset(src, i))) > 0){
        i += j;
        if(jp_state_getat(src, i) == ',')
            i++;
        else
            break;
    }
    return i;
}

int jp_parse_array(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    if(jp_state_getat(src, i) == '['){
        i++;
        if((j = jp_parse_elements(jp_state_offset(src, i))) > 0)
            i += j;
        else if((j = jp_parse_ws(jp_state_offset(src, i))) > 0)
            i += j;
        if(jp_state_getat(src, i) == ']'){
            i++;
            return i;
        }
    }
    return 0;
}

int jp_parse_member(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    i += jp_parse_ws(jp_state_offset(src, i));
    if((j = jp_parse_string(jp_state_offset(src, i))) > 0){
        i += j;
        i += jp_parse_ws(jp_state_offset(src, i));
        if(jp_state_getat(src, i) == ':'){
            i++;
            if((j = jp_parse_element(jp_state_offset(src, i))) > 0){
                i += j;
                return i;
            }
        }
    }
    return 0;
}

int jp_parse_members(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    while((j = jp_parse_member(jp_state_offset(src, i))) > 0){
        i += j;
        if(jp_state_getat(src, i) == ',')
            i++;
        else
            break;
    }
    return i;
}

int jp_parse_object(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    if(jp_state_getat(src, i) == '{'){
        i++;
        if((j = jp_parse_members(jp_state_offset(src, i))) > 0)
            i += j;
        else if((j = jp_parse_ws(jp_state_offset(src, i))) > 0)
            i += j;
        if(jp_state_getat(src, i) == '}'){
            i++;
            return i;
        }
    }
    return 0;
}
int jp_parse_value(struct jp_state src){
    size_t i = 0;
    size_t j = 0;
    if((j = jp_parse_object(jp_state_offset(src, i))) > 0)
        i += j;
    else if((j = jp_parse_array(jp_state_offset(src, i))) > 0)
        i += j;
    else if((j = jp_parse_string(jp_state_offset(src, i))) > 0)
        i += j;
    else if((j = jp_parse_number(jp_state_offset(src, i))) > 0)
        i += j;
    else if(jp_state_memcmp(jp_state_offset(src, i), "true", 4) == 0)
        i += 4;
    else if(jp_state_memcmp(jp_state_offset(src, i), "false", 5) == 0)
        i += 5;
    else if(jp_state_memcmp(jp_state_offset(src, i), "null", 4) == 0)
        i += 4;
    return i;
}
int jp_parse_json(struct jp_state src){
    return jp_parse_element(src);
}

int jp_string_read(struct jp_state src, char *dst, size_t dst_size){
    size_t i = 0, j = 0;
    uint32_t utf = 0;
    char buf[4];
    if(jp_state_getat(src, i) == '"'){
        i++;
        int escape = 0;
        for(j = 0;escape || jp_state_getat(src, i) != '"' & j < dst_size;i++){
            char c = jp_state_getat(src, i);
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
                    i++;
                    jp_atoix32(jp_state_offset(src, i), &utf);
                    i += 3;
                    size_t n = jp_utf8_encode(buf, utf);
                    if((j + n) >= dst_size)
                        return j;
                    memcpy(&dst[j], buf, n);
                    j += n;
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
int jp_string_len(struct jp_state src){
    size_t i = 0, j = 0;
    uint32_t utf = 0;
    if(jp_state_getat(src, i) == '"'){
        i++;
        int escape = 0;
        for(j = 0;escape || jp_state_getat(src, i) != '"';i++){
            char c = jp_state_getat(src, i);
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
                    i++;
                    jp_atoix32(jp_state_offset(src, i), &utf);
                    i += 3;
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
int jp_string_comp(struct jp_state src, const char *str){
    size_t i = 0, j = 0;
    uint32_t utf = 0;
    size_t str_len = strlen(str);
    if(jp_state_getat(src, i) == '"'){
        i++;
        int escape = 0;
        for(j = 0;escape || jp_state_getat(src, i) != '"';i++){
            if(j >= str_len)
                return 0;
            char c = jp_state_getat(src, i);
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
                    i++;
                    jp_atoix32(jp_state_offset(src, i), &utf);
                    i += 3;
                    char buf[4];
                    size_t n = jp_utf8_encode(&buf[j], utf);
                    if((j + n) >= str_len)
                        return 0;
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
    if(utf <= 0x7F){
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
int jp_atoix32(struct jp_state src, uint32_t *dst){
    *dst = 0;
    for(size_t i = 0;i < 4;i++){
        *dst *= 0x10;
        char c = jp_state_getat(src, i);
        if(c >= '0' && c <= '9')
            *dst += c - '0';
        else if(c >= 'a' && c <= 'f')
            *dst += c - 'a' + 0x0a;
        else if(c >= 'A' && c <= 'F')
            *dst += c - 'A' + 0x0a;
    }
    return 1;
}





int jp_parser_init_buf(struct jp_parser *dst, const char *buf){
    dst->buffer.buf = buf;
    dst->type = JP_TYPE_BUF;
    return 1;
}
int jp_parser_init_file(struct jp_parser *dst, FILE *fp){
    dst->file.fp = fp;
    dst->type = JP_TYPE_FILE;
    return 1;
}
struct jp_parser jp_parser(const char *buf){
    struct jp_parser dst;
    jp_parser_init_buf(&dst, buf);
    return dst;
}
struct jp_parser jp_parser_file(FILE *fp){
    struct jp_parser dst;
    jp_parser_init_file(&dst, fp);
    return dst;
}

struct jp_state jp_state(struct jp_parser *src){
    struct jp_state dst = {
        .parser = src,
        .valid = 1,
        .size = 0,
#if 1
        .index = 0,
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

struct jp_element jp_element(struct jp_parser *src){
    return (struct jp_element){.state = jp_state(src)};
}

struct jp_member jp_member(struct jp_parser *src){
    return jp_element_member(jp_element(src));
}

struct jp_element jp_element_next(struct jp_element src){
    if(!src.state.valid)
        return (struct jp_element)JP_COPY(src);

    size_t i = 0;
    struct jp_element dst = JP_COPY(src);
    i += jp_parse_element(jp_state_offset(src.state, i));
    if(jp_state_getat(src.state, i) == ','){
        i++;
        dst.state = jp_state_offset(src.state, i);
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
    i += jp_parse_member(jp_state_offset(src.state, i));
    if(jp_state_getat(src.state, i) == ','){
        i++;
        i += jp_parse_ws(jp_state_offset(src.state, i));
        dst.state = jp_state_offset(src.state, i);
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
    i += jp_parse_ws(jp_state_offset(src.state, i));
    dst.state = jp_state_offset(src.state, i);
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
    i += jp_parse_ws(jp_state_offset(src.state, i));
    i += jp_parse_string(jp_state_offset(src.state, i));
    i += jp_parse_ws(jp_state_offset(src.state, i));
    if(jp_state_getat(src.state, i) == ':'){
        i++;
        dst.state = jp_state_offset(src.state, i);
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
    if(jp_state_getat(src.state, i) == '{'){
        i++;
        i += jp_parse_ws(jp_state_offset(src.state, i));
        if(jp_state_getat(src.state, i) == '}')
            dst.state.valid = 0;
        dst.state = jp_state_offset(src.state, i);
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
    i += jp_parse_ws(jp_state_offset(src.state, i));
    if(jp_state_getat(src.state, i) == '['){
        i++;
        i += jp_parse_ws(jp_state_offset(src.state, i));
        if(jp_state_getat(src.state, i) == ']')
            dst.state.valid = 0;
        dst.state = jp_state_offset(src.state, i);
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
    dst.state = jp_state_offset(src.state, i);
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
    dst.state = jp_state_offset(src.state, i);
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
    i += jp_parse_ws(jp_state_offset(src.state, i));
    i += jp_string_read(jp_state_offset(src.state, i), dst, dst_size);
    return i;
}
int jp_value_number(const struct jp_value src, double *dst){
    if(!src.state.valid)
        return 0;

    int i = 0;
    i += jp_atod(src.state, dst);
    return i-1;
}
int jp_member_name(const struct jp_member src, char *dst, size_t dst_size){
    if(!src.state.valid)
        return 0;

    size_t i = 0;
    i += jp_string_read(jp_state_offset(src.state, i), dst, dst_size);
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
    i += jp_parse_ws(jp_state_offset(src.state, i));
    if(!jp_string_comp(jp_state_offset(src.state, i), str))
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
    struct jp_member dst = JP_COPY(src);
    dst.state.valid = 0;
    return dst;
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

    if(jp_state_memcmp(src.state, "true", 4) == 0)
        return JP_TFN_TRUE;
    else if(jp_state_memcmp(src.state, "false", 5) == 0)
        return JP_TFN_FALSE;
    else if(jp_state_memcmp(src.state, "null", 4) == 0)
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
    i += jp_parse_ws(jp_state_offset(src.state, i));
    j = jp_string_len(jp_state_offset(src.state, i));
    return j;
}
int jp_value_strlen(struct jp_value src){
    if(!src.state.valid)
        return 0;

    size_t i = 0, j = 0;
    i += jp_parse_ws(jp_state_offset(src.state, i));
    j = jp_string_len(jp_state_offset(src.state, i));
    return j;
}
int jp_element_strlen(struct jp_element src){
    return jp_value_strlen(jp_element_value(src));
}
int jp_member_strlen(struct jp_member src){
    return jp_element_strlen(jp_member_element(src));
}

char jp_state_getat(struct jp_state src, size_t offset){
    return jp_parser_getat(src.parser, src.index + offset);
}
struct jp_state jp_state_offset(struct jp_state src, size_t offset){
    struct jp_state dst = src;
    dst.index += offset;
    return dst;
}
int jp_state_memcmp(struct jp_state src1, const void *src2, size_t src_size){
    for(size_t i = 0;i < src_size;i++){
        if(jp_state_getat(src1, i) != ((char *)src2)[i])
            return 1;
    }
    return 0;
}
int jp_atod(struct jp_state src, double *dst){
    if(!src.valid)
        return 0;

    size_t i = 0;
    *dst = 0;
    int neg = 0, exp_neg = 0;
    char c = 0;
    double tmp1 = 0, tmp2 = 0;
    if(jp_state_getat(src, i) == '-')
        neg = 1;
    for(;(c = jp_state_getat(src, i)) >= '0' && c <= '9';i++){
        *dst *= 10;
        *dst += c - '0';
    }
    if((c = jp_state_getat(src, i)) == '.'){
        i++;
        tmp2 = 1;
        for(;(c = jp_state_getat(src, i)) >= '0' && c <= '9';i++){
            tmp1 *= 10;
            tmp2 /= 10;
            tmp1 += c - '0';
        }
        *dst += tmp1 * tmp2;
    }
    if((c = jp_state_getat(src, i)) == 'e' || c == 'E'){
        i++;
        c = jp_state_getat(src, i);
        if(c == '+'){
            i++;
            exp_neg = 0;
        }
        else if(c == '-'){
            i++;
            exp_neg = 1;
        }
        tmp1 = 0;
        for(;(c = jp_state_getat(src, i)) >= '0' && c <= '9';i++){
            tmp1 *= 10;
            tmp1 += c - '0';
        }
        *dst *= pow(10, tmp1);
    }
    return i;
}
char jp_parser_getat(struct jp_parser *src, size_t index){
    char c;
    switch(src->type){
    case JP_TYPE_BUF:
        return src->buffer.buf[index];
        break;
    case JP_TYPE_FILE:
        if(ftell(src->file.fp) != index)
            if(fseek(src->file.fp, index, SEEK_SET) != 0)
                return 0;
        if((c = getc(src->file.fp)) >= 0)
            return c;
        break;
    }
    return 0;

}
struct jp_member jp_element_member_search(struct jp_element src, const char *str){
    return jp_members_search(jp_element_member(src), str);
}
