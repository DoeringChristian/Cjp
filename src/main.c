#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jp.h"

const char text[] = "\
{\
    \"glossary\": {\
        \"title\": \"example glossary\",\
        \"GlossDiv\": {\
            \"title\": \"S\",\
            \"GlossList\": {\
                \"GlossEntry\": {\
                    \"ID\": \"SGML\",\
                    \"SortAs\": \"SGML\",\
                    \"GlossTerm\": \"Standard Generalized Markup Language\",\
                    \"Acronym\": \"SGML\",\
                    \"test\": 100,\
                    \"Abbrev\": \"ISO 8879:1986\",\
                    \"GlossDef\": {\
                        \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\",\
                        \"GlossSeeAlso\": [\"GML\\u262d\", \"XML\"]\
                    },\
                    \"GlossSee\": \"markup\"\
                }\
            }\
        }\
    }\
}\
";


int main(){

    struct jp_element e = jp_element(text);

    struct jp_member m1 = jp_element_member(e);

    struct jp_element e1 = jp_member_element(m1);

    struct jp_member m2 = jp_element_member(e1);
    m2 = jp_members_search(m2, "GlossDiv");
    //printf("%s\n\n", m2.state.c);

    struct jp_member m3 = jp_member_member(m2);

    m3 = jp_members_search(m3, "GlossList");
    m3 = jp_member_member(m3);
    m3 = jp_member_member(m3);
    //printf("%s\n\n", m3.state.c);
    jp_members_foreach(m3, i){
        char buf[100] = {0};
        jp_member_name(i, buf, 100);
        printf("%s\n\n", buf);
    }
    m3 = jp_members_search(m3, "GlossDef");
    m3 = jp_member_member(m3);
    m3 = jp_member_next(m3);
    struct jp_array a1 = jp_member_array(m3);
    struct jp_element e3 = jp_array_element(a1);

    jp_elements_foreach(e3, i){
        char buf[100] = {0};
        jp_element_string(i, buf, 100);
        printf("%s\n\n", buf);
    }

    printf("%s\n\n", e3.state.buf);

    FILE *fp = fopen("res/test.json", "r");

    e = jp_element_file(fp);

    struct jp_member m = jp_element_member(e);

    char buf[100] = {0};

    jp_member_name(m, buf, 100);

    double d;
    jp_member_number(m, &d);

    printf("%s: %lf\n", buf, d);


#if 0
    struct jp_element e2 = jp_member_element(m2);

    char buf[100];

    jp_value_string(jp_element_value(e2), buf, 100);

    printf("%s\n\n", jp_element_value(e2).state.c);
    printf("%s\n", buf);
#endif

    return 0;

#if 0
    const char *cur = text;

    struct jp_state p;

    jp_parser_init(&p, text);
    struct jp_element e1;
    jp_parser_element(&p, &e1);
    struct jp_member m1;
    jp_element_member(&e1, &m1);
    struct jp_string s1;
    jp_member_string(&m1, &s1);
    printf("%.*s\n", (int)s1.str_size, s1.p.c);
    struct jp_element e2;
    jp_member_element(&m1, &e2);
    struct jp_member m2;
    jp_element_member(&e2, &m2);
#endif



#if 0
    cur = jp_object_first(cur);
    printf("%s\n", cur);
    cur = jp_object_member(cur, "glossary");
    printf("\n");
    printf("%s\n", cur);
    cur = jp_member_cont(cur);
    printf("\n");
    printf("%s\n", cur);
    cur = jp_object_first(cur);
    printf("\n");
    printf("%s\n", cur);
    cur = jp_object_member(cur, "title");
    cur = jp_member_next(cur);
    //cur = jp_member_next(cur);
    printf("\n");
    printf("%s\n", cur);
#endif

    return 0;
}

