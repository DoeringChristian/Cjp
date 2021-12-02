#ifndef UTEST_H
#define UTEST_H

#define ASSERT(_x, _out, ...){\
    if(!(_x)){\
        printf(_out, __VA_ARGS__);\
        assert(0);\
    }\
}

#endif //UTEST_H
