#include <stdlib.h>
#include <string.h>

#include <tk.h>

#define __USE_POSIX
#undef G_DISABLE_ASSERT
#include "frida-gum.h"

typedef int (*Tk_ConfigureWidget_fptr_t)(Tcl_Interp *interp, Tk_Window tkwin,
                                         const Tk_ConfigSpec *specs, int argc, const char **argv,
                                         char *widgRec, int flags);

typedef struct _TkListener TkListener;
typedef enum _TkHookId TkHookId;
typedef struct _TkInvocationData TkInvocationData;

struct _TkListener {
    GObject parent;
};

enum _TkHookId {
    HOOK_TK_CONFIGUREWIDGET,
};

struct _TkInvocationData {
    int argc;
    char **argv_mod;
};

static void tk_listener_iface_init(gpointer g_iface, gpointer iface_data);

#define TK_TYPE_LISTENER (tk_listener_get_type())
G_DECLARE_FINAL_TYPE(TkListener, tk_listener, TK, LISTENER, GObject)
G_DEFINE_TYPE_EXTENDED(TkListener, tk_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER, tk_listener_iface_init))

static void tk_listener_on_enter(GumInvocationListener *listener, GumInvocationContext *ic) {
    TkListener *self = TK_LISTENER(listener);
    TkHookId hook_id = GUM_IC_GET_FUNC_DATA(ic, TkHookId);

    switch (hook_id) {
    case HOOK_TK_CONFIGUREWIDGET: {
        TkInvocationData *id = GUM_IC_GET_INVOCATION_DATA(ic, TkInvocationData);
        int argc             = GPOINTER_TO_INT(gum_invocation_context_get_nth_argument(ic, 3));
        id->argc             = argc;
        const char **argv    = (const char **)gum_invocation_context_get_nth_argument(ic, 4);
        int width            = -1;
        int width_idx;
        char new_width_buf[32];
        int height = -1;
        int height_idx;
        char new_height_buf[32];
        for (int i; i < argc; ++i) {
            if (!strcmp(argv[i], "-width")) {
                width_idx = i + 1;
                width     = atoi(argv[width_idx]);
            }
            if (!strcmp(argv[i], "-height")) {
                height_idx = i + 1;
                height     = atoi(argv[height_idx]);
            }
        }
        if (width != -1 || height != -1) {
            char **argv_mod = malloc(sizeof(char **) * argc);
            g_assert(argv_mod);
            for (int i = 0; i < argc; ++i) {
                size_t len  = strlen(argv[i]);
                argv_mod[i] = malloc(len + 1);
                g_assert(argv_mod[i]);
                strcpy(argv_mod[i], argv[i]);
            }
            id->argv_mod = argv_mod;
            gum_invocation_context_replace_nth_argument(ic, 4, argv_mod);
            g_print("[*] Tk_ConfigureWidget(width = %d, height = %d)\n", width, height);
        } else {
            id->argv_mod = NULL;
        }
    } break;
    default:
        g_assert(!"unhandled case");
    }
}

static void tk_listener_on_leave(GumInvocationListener *listener, GumInvocationContext *ic) {
    TkListener *self = TK_LISTENER(listener);
    TkHookId hook_id = GUM_IC_GET_FUNC_DATA(ic, TkHookId);

    switch (hook_id) {
    case HOOK_TK_CONFIGUREWIDGET: {
        TkInvocationData *id = GUM_IC_GET_INVOCATION_DATA(ic, TkInvocationData);
        if (id->argv_mod) {
            for (int i = 0; i < id->argc; ++i) {
                free(id->argv_mod[i]);
            }
            free(id->argv_mod);
        }
    } break;
    default:
        g_assert(!"unhandled case");
    }
}

static void tk_listener_class_init(TkListenerClass *klass) {
    (void)TK_IS_LISTENER;
    (void)glib_autoptr_cleanup_TkListener;
}

static void tk_listener_iface_init(gpointer g_iface, gpointer iface_data) {
    GumInvocationListenerInterface *iface = g_iface;

    iface->on_enter = tk_listener_on_enter;
    iface->on_leave = tk_listener_on_leave;
}

static void tk_listener_init(TkListener *self) {}

static GumInterceptor *interceptor;
static GumInvocationListener *listener;
static gpointer Tk_ConfigureWidget_fptr;

__attribute__((constructor)) static void questa_hidpi_hook_ctor(void) {
    gum_init_embedded();

    gpointer Tk_ConfigureWidget_fptr =
        GSIZE_TO_POINTER(gum_module_find_export_by_name("libtk8.6.so", "Tk_ConfigureWidget"));
    if (!Tk_ConfigureWidget_fptr) {
        Tk_ConfigureWidget_fptr =
            GSIZE_TO_POINTER(gum_module_find_export_by_name(NULL, "Tk_ConfigureWidget"));
    }
    if (!Tk_ConfigureWidget_fptr) {
        return;
    }

    interceptor = gum_interceptor_obtain();
    g_assert(interceptor);
    listener = g_object_new(TK_TYPE_LISTENER, NULL);
    g_assert(listener);

    gum_interceptor_begin_transaction(interceptor);
    gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(Tk_ConfigureWidget_fptr), listener,
                           GSIZE_TO_POINTER(HOOK_TK_CONFIGUREWIDGET));
    gum_interceptor_end_transaction(interceptor);
}

__attribute__((destructor)) static void questa_hidpi_hook_dtor(void) {
    if (Tk_ConfigureWidget_fptr) {
        g_assert(listener);
        g_object_unref(listener);
        g_assert(interceptor);
        g_object_unref(interceptor);
    }
    gum_deinit_embedded();
}
