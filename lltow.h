#ifndef LLTOW_H_
#define LLTOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

size_t lltow_upper(int64_t value, wchar_t *sp, int radix);
size_t ulltow_upper(uint64_t value, wchar_t *sp, int radix);

#ifdef __cplusplus
}
#endif

#endif
