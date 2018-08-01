#pragma once

#include <glib.h>

/* This "allocation tag" is passed to all AWS memory management functions */
#define _COG_ALLOCATION_TAG "libcog"

void _cog_free_static_data (void);

gboolean _cog_is_valid_client_id (const char *string);
