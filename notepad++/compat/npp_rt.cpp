// npp_rt.cpp: what MSVC 14 expects of its runtime that PINT's older one lacks.
#include <stdlib.h>
extern "C" void __std_terminate(void) { abort(); }
