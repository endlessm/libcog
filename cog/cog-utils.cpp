#include <aws/cognito-idp/model/AttributeType.h>
#include <aws/core/utils/memory/stl/AWSVector.h>
#include <glib.h>

#include "cog/cog-utils.h"
#include "cog/cog-utils-private.h"

using Aws::CognitoIdentityProvider::Model::AttributeType;

/**
 * SECTION:types
 * @title: Types
 * @short_description: Miscellaneous types
 *
 * Enumerations, string constants, and exception types used by Libcog.
 */

G_DEFINE_QUARK(cog-identity-provider-error, cog_identity_provider_error)

#define DEFINE_REGEX_VALIDATOR(name, pattern) \
  static GRegex * name ## _regex = NULL; \
  gboolean \
  _cog_is_valid_ ## name (const char *string) \
  { \
    if (g_once_init_enter (& name ## _regex)) \
      { \
        GError *error = NULL; \
        GRegex *regex = g_regex_new ("^" pattern "$", G_REGEX_OPTIMIZE, \
                                     GRegexMatchFlags (0), &error); \
        if (!regex) \
          g_error ("Programmer error: %s", error->message); \
        g_once_init_leave (& name ## _regex, regex); \
      } \
    return g_regex_match (name ## _regex, string, GRegexMatchFlags (0), NULL); \
  }

DEFINE_REGEX_VALIDATOR(client_id, "[\\w+]+")
DEFINE_REGEX_VALIDATOR(password, "[\\S]+")
DEFINE_REGEX_VALIDATOR(secret_hash, "[\\w+=/]+")
DEFINE_REGEX_VALIDATOR(username, "[\\p{L}\\p{M}\\p{S}\\p{N}\\p{P}]+")

void
_cog_free_static_data (void)
{
  g_clear_pointer (&client_id_regex, g_regex_unref);
  g_clear_pointer (&password_regex, g_regex_unref);
  g_clear_pointer (&secret_hash_regex, g_regex_unref);
  g_clear_pointer (&username_regex, g_regex_unref);
}

void
_cog_hash_table_to_vector (GHashTable *hash_table,
                           Aws::Vector<AttributeType> *vector)
{
  g_hash_table_foreach (hash_table, [](void *key, void *value, void *data)
    {
      auto *vector = static_cast<Aws::Vector<AttributeType> *> (data);
      AttributeType attribute;
      attribute.WithName(static_cast<char *> (key))
        .SetValue(static_cast<char *> (value));
      vector->push_back(attribute);
    },
    vector);
}
