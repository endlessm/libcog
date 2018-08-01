/**
 * SECTION:client
 * @title: CogClient
 * @short_description: Object through which to access the Cognito User Pools API
 *
 * Using the Amazon Cognito User Pools API, you can create a user pool to manage
 * directories and users.
 * You can authenticate a user to obtain tokens related to user identity and
 * access policies.
 */

#include <aws/cognito-idp/CognitoIdentityProviderClient.h>

#include "cog/cog-client.h"

#define GET_PRIVATE(o) (static_cast<CogClientPrivate *> (cog_client_get_instance_private (COG_CLIENT (o))))

using Aws::CognitoIdentityProvider::CognitoIdentityProviderClient;

typedef struct
{
  CognitoIdentityProviderClient internal;
} CogClientPrivate;

struct _CogClient {
  GObject parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (CogClient, cog_client, G_TYPE_OBJECT)

/**
 * cog_client_new:
 *
 * Create a new API client using the default credential provider, the default
 * HTTP client, and the default values for the client's configuration.
 *
 * Returns: (transfer full): a newly created #CogClient
 */
CogClient *
cog_client_new (void)
{
  return COG_CLIENT (g_object_new (COG_TYPE_CLIENT, NULL));
}

static void
cog_client_constructed (GObject *object)
{
  CogClientPrivate *priv = GET_PRIVATE (object);

  new (&priv->internal) CognitoIdentityProviderClient();
}

static void
cog_client_finalize (GObject *object)
{
  CogClient *self = COG_CLIENT (object);
  CogClientPrivate *priv = GET_PRIVATE (self);

  priv->internal.~CognitoIdentityProviderClient();

  G_OBJECT_CLASS (cog_client_parent_class)->finalize (object);
}

static void
cog_client_class_init (CogClientClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = cog_client_constructed;
  object_class->finalize = cog_client_finalize;
}

static void
cog_client_init (CogClient *self G_GNUC_UNUSED)
{
}
