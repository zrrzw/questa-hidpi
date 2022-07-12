#define _GNU_SOURCE
#undef NDEBUG
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tk.h>

#include "frida-gum.h"

typedef int (*Tk_ConfigureWidget_fptr_t)(Tcl_Interp *interp, Tk_Window tkwin,
                                         const Tk_ConfigSpec *specs, int argc, const char **argv,
                                         char *widgRec, int flags);

int Tk_ConfigureWidget(Tcl_Interp *interp, Tk_Window tkwin, const Tk_ConfigSpec *specs, int argc,
                       const char **argv, char *widgRec, int flags) {
    static Tk_ConfigureWidget_fptr_t orig_func = NULL;
    if (!orig_func) {
        orig_func = dlsym(RTLD_NEXT, "Tk_ConfigureWidget");
        assert(orig_func);
    }
    printf("fuck\n");
    int res = orig_func(interp, tkwin, specs, argc, argv, widgRec, flags);
    return res;
}
