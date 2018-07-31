#pragma once

#if !(defined(_COG_INSIDE_COG_H) || defined(COMPILING_LIBCOG))
#error "Please do not include this header file directly."
#endif

#include <glib.h>
#include "cog/cog-macros.h"

G_BEGIN_DECLS

COG_AVAILABLE_IN_ALL
void cog_init_default (void);

COG_AVAILABLE_IN_ALL
gboolean cog_is_inited (void);

COG_AVAILABLE_IN_ALL
void cog_shutdown (void);

G_END_DECLS
