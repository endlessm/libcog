#include <glib.h>

#include "cog/cog-utils.h"

/**
 * SECTION:types
 * @title: Types
 * @short_description: Miscellaneous types
 *
 * Enumerations, string constants, and exception types used by Libcog.
 */

G_DEFINE_QUARK(cog-identity-provider-error, cog_identity_provider_error)

static GRegex *client_id_regex = NULL;

void
_cog_free_static_data (void)
{
  g_clear_pointer (&client_id_regex, g_regex_unref);
}

gboolean
_cog_is_valid_client_id (const char *string)
{
  if (g_once_init_enter (&client_id_regex))
    {
      GError *error = NULL;
      GRegex *regex = g_regex_new ("^[\\w+]+$", G_REGEX_OPTIMIZE,
                                   GRegexMatchFlags(0), &error);
      if (!regex)
        g_error ("Programmer error: %s", error->message);
      g_once_init_leave (&client_id_regex, regex);
    }

  return g_regex_match (client_id_regex, string, GRegexMatchFlags(0), NULL);
}
