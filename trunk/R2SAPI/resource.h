#include "build.h"

// version
#define _STR(x) _VAL(x)
#define _VAL(x) #x
#define FILEVER                         1,36,0,APIVER
#define FILEVERSTR                      "1.36.0." _STR(APIVER) /*" BETA"*/ "\0"
#define PRODUCTVER                      FILEVER
#define PRODUCTVERSTR                   FILEVERSTR

// res
#define IDT_TYPELIB1                    1

