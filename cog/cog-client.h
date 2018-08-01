#pragma once

#if !(defined(_COG_INSIDE_COG_H) || defined(COMPILING_LIBCOG))
#error "Please do not include this header file directly."
#endif

#include <glib-object.h>

#include "cog/cog-macros.h"

G_BEGIN_DECLS

#define COG_TYPE_CLIENT (cog_client_get_type())

COG_AVAILABLE_IN_ALL
G_DECLARE_FINAL_TYPE (CogClient, cog_client, COG, CLIENT, GObject)

struct _CogClientClass
{
  GObjectClass parent_class;
};

COG_AVAILABLE_IN_ALL
CogClient *cog_client_new (void);

G_END_DECLS
