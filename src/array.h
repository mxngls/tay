#ifndef ARRAY_H

#define ARRAY_H

#define array_push(arr, val)                                                                       \
    ({                                                                                             \
        int _ret = 0;                                                                              \
        if ((arr)->len >= (arr)->cap) {                                                            \
            size_t _new_cap = (arr)->cap == 0 ? 128 : (arr)->cap * 2;                              \
            void*  _grown = realloc((arr)->items, _new_cap * sizeof(*(arr)->items));               \
            if (!_grown) {                                                                         \
                fprintf(stderr, "Error: re-allocation failed\n");                                  \
                _ret = -1;                                                                         \
            } else {                                                                               \
                (arr)->items = _grown;                                                             \
                (arr)->cap = _new_cap;                                                             \
                (arr)->items[(arr)->len++] = (val);                                                \
            }                                                                                      \
        } else {                                                                                   \
            (arr)->items[(arr)->len++] = (val);                                                    \
        }                                                                                          \
        _ret;                                                                                      \
    })
#define array_pop(arr)                                                                             \
    ({                                                                                             \
        int _ret = 0;                                                                              \
        if ((arr)->len == 0) {                                                                     \
            fprintf(stderr, "Error: array empty\n");                                               \
            _ret = -1;                                                                             \
        } else {                                                                                   \
            (arr)->len--;                                                                          \
        }                                                                                          \
        _ret;                                                                                      \
    })
#define array_back(arr) ((arr)->items[(arr)->len - 1])

#endif
