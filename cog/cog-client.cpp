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
#include <aws/cognito-idp/model/GetUserRequest.h>
#include <aws/cognito-idp/model/InitiateAuthRequest.h>
#include <aws/cognito-idp/model/SignUpRequest.h>
#include <aws/cognito-idp/model/UpdateUserAttributesRequest.h>
#include <aws/core/utils/Outcome.h>
#include <gio/gio.h>

#include "cog/cog-analytics-metadata.h"
#include "cog/cog-authentication-result.h"
#include "cog/cog-boxed-private.h"
#include "cog/cog-client.h"
#include "cog/cog-enums.h"
#include "cog/cog-user-context-data.h"
#include "cog/cog-utils-private.h"
#include "cog/cog-utils.h"

#define GET_PRIVATE(o) (static_cast<CogClientPrivate *> (cog_client_get_instance_private (COG_CLIENT (o))))

using Aws::Client::AsyncCallerContext;
using Aws::Client::ClientConfiguration;
using Aws::CognitoIdentityProvider::CognitoIdentityProviderClient;
using Aws::CognitoIdentityProvider::Model::AuthFlowType;
using Aws::CognitoIdentityProvider::Model::AttributeType;
using Aws::CognitoIdentityProvider::Model::GetUserOutcome;
using Aws::CognitoIdentityProvider::Model::GetUserRequest;
using Aws::CognitoIdentityProvider::Model::GetUserResult;
using Aws::CognitoIdentityProvider::Model::InitiateAuthOutcome;
using Aws::CognitoIdentityProvider::Model::InitiateAuthRequest;
using Aws::CognitoIdentityProvider::Model::InitiateAuthResult;
using Aws::CognitoIdentityProvider::Model::SignUpOutcome;
using Aws::CognitoIdentityProvider::Model::SignUpRequest;
using Aws::CognitoIdentityProvider::Model::SignUpResult;
using Aws::CognitoIdentityProvider::Model::UpdateUserAttributesOutcome;
using Aws::CognitoIdentityProvider::Model::UpdateUserAttributesRequest;
using Aws::CognitoIdentityProvider::Model::UpdateUserAttributesResult;

class GTaskAsyncContext : public AsyncCallerContext {
  GTask *m_task;
public:
  explicit GTaskAsyncContext (GTask *task) : m_task (G_TASK (g_object_ref (task))) {}
  ~GTaskAsyncContext () { g_object_unref (m_task); }
  GTask *task (void) const { return m_task; }
};

typedef struct
{
  CognitoIdentityProviderClient internal;
  CogRegion region;
} CogClientPrivate;

struct _CogClient {
  GObject parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (CogClient, cog_client, G_TYPE_OBJECT)

enum {
  PROP_REGION = 1,
  N_PROPERTIES
};

static void
cog_client_set_property (GObject *object,
                         unsigned property_id,
                         const GValue *value,
                         GParamSpec *pspec)
{
  CogClient *self = COG_CLIENT (object);
  CogClientPrivate *priv = GET_PRIVATE (self);

  switch (property_id) {
    case PROP_REGION:
      priv->region = (CogRegion) g_value_get_enum (value);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static void
cog_client_get_property (GObject *object,
                         unsigned property_id,
                         GValue *value,
                         GParamSpec *pspec)
{
  CogClient *self = COG_CLIENT (object);
  CogClientPrivate *priv = GET_PRIVATE (self);

  switch (property_id) {
    case PROP_REGION:
      g_value_set_enum (value, priv->region);
      break;
    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

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
  ClientConfiguration config = Aws::Client::ClientConfiguration();
  G_OBJECT_CLASS (cog_client_parent_class)->constructed (object);

  switch (priv->region) {
    case COG_REGION_US_EAST_1:
      config.region = Aws::Region::US_EAST_1;
      break;
    case COG_REGION_US_EAST_2:
      config.region = Aws::Region::US_EAST_2;
      break;
    case COG_REGION_US_WEST_1:
      config.region = Aws::Region::US_WEST_1;
      break;
    case COG_REGION_US_WEST_2:
      config.region = Aws::Region::US_WEST_2;
      break;
    case COG_REGION_EU_WEST_1:
      config.region = Aws::Region::EU_WEST_1;
      break;
    case COG_REGION_EU_WEST_2:
      config.region = Aws::Region::EU_WEST_2;
      break;
    case COG_REGION_EU_WEST_3:
      config.region = Aws::Region::EU_WEST_3;
      break;
    case COG_REGION_EU_CENTRAL_1:
      config.region = Aws::Region::EU_CENTRAL_1;
      break;
    case COG_REGION_AP_SOUTHEAST_1:
      config.region = Aws::Region::AP_SOUTHEAST_1;
      break;
    case COG_REGION_AP_SOUTHEAST_2:
      config.region = Aws::Region::AP_SOUTHEAST_2;
      break;
    case COG_REGION_AP_NORTHEAST_1:
      config.region = Aws::Region::AP_NORTHEAST_1;
      break;
    case COG_REGION_AP_NORTHEAST_2:
      config.region = Aws::Region::AP_NORTHEAST_2;
      break;
    case COG_REGION_SA_EAST_1:
      config.region = Aws::Region::SA_EAST_1;
      break;
    case COG_REGION_CA_CENTRAL_1:
      config.region = Aws::Region::CA_CENTRAL_1;
      break;
    case COG_REGION_AP_SOUTH_1:
      config.region = Aws::Region::AP_SOUTH_1;
      break;
    case COG_REGION_CN_NORTH_1:
      config.region = Aws::Region::CN_NORTH_1;
      break;
    case COG_REGION_CN_NORTHWEST_1:
      config.region = Aws::Region::CN_NORTHWEST_1;
      break;
    case COG_REGION_US_GOV_WEST_1:
      config.region = Aws::Region::US_GOV_WEST_1;
      break;
    default:
      break;
  }

  new (&priv->internal) CognitoIdentityProviderClient(config);
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

  object_class->set_property = cog_client_set_property;
  object_class->get_property = cog_client_get_property;

  g_object_class_install_property (object_class,
                                   PROP_REGION,
                                   g_param_spec_enum ("region",
                                                      "Region",
                                                      "AWS region",
                                                      COG_TYPE_REGION,
                                                      COG_REGION_US_EAST_1,
                                                      (GParamFlags)
                                                      (G_PARAM_CONSTRUCT_ONLY |
                                                       G_PARAM_READWRITE)));
}

static void
cog_client_init (CogClient *self G_GNUC_UNUSED)
{
}

/* METHODS */

static gboolean
get_user_validate_in_parameters (const char *access_token)
{
  g_return_val_if_fail(access_token, FALSE);
  g_return_val_if_fail (_cog_is_valid_access_token (access_token), FALSE);
  return TRUE;
}

static gboolean
get_user_validate_out_parameters (char **username,
                                  GHashTable **user_attributes,
                                  GList **mfa_options,
                                  char **preferred_mfa_setting,
                                  char ***user_mfa_settings_list)
{
  g_return_val_if_fail (username, FALSE);
  g_return_val_if_fail (user_attributes, FALSE);
  g_return_val_if_fail (mfa_options, FALSE);
  g_return_val_if_fail (preferred_mfa_setting, FALSE);
  g_return_val_if_fail (user_mfa_settings_list, FALSE);
  return TRUE;
}

static GetUserRequest
get_user_build_request (const char *access_token)
{
  return GetUserRequest ().WithAccessToken (access_token);
}

static void
get_user_unpack_result (GetUserResult& result,
                        char **username,
                        GHashTable **user_attributes,
                        GList **mfa_options,
                        char **preferred_mfa_setting,
                        char ***user_mfa_settings_list)
{
  *username = g_strdup (result.GetUsername ().c_str ());
  *preferred_mfa_setting = g_strdup (result.GetPreferredMfaSetting ().c_str ());

  *user_attributes = g_hash_table_new_full (g_str_hash, g_str_equal,
                                            g_free, g_free);
  _cog_vector_to_hash_table (result.GetUserAttributes (), *user_attributes);

  *mfa_options = NULL;
  for (auto& option : result.GetMFAOptions ())
    *mfa_options = g_list_prepend (*mfa_options,
                                   _cog_mfa_option_from_internal (option));
  *mfa_options = g_list_reverse (*mfa_options);

  *user_mfa_settings_list = _cog_vector_to_strv (result.GetUserMFASettingList ());
}

/* Work around the lack of a copy or move constructor for GetUserResult. Returns
 * a GetUserResult allocated on the heap, suitable for returning as a pointer
 * with g_task_return_pointer() */
static GetUserResult *
get_user_move_result (const GetUserResult&& result)
{
  auto *retval = new GetUserResult ();
  retval->SetUsername (std::move (result.GetUsername ()));
  retval->SetUserAttributes (std::move (result.GetUserAttributes ()));
  retval->SetMFAOptions (std::move (result.GetMFAOptions ()));
  retval->SetPreferredMfaSetting (std::move (result.GetPreferredMfaSetting ()));
  retval->SetUserMFASettingList (std::move (result.GetUserMFASettingList ()));
  return retval;
}

static void
get_user_handle_request (const CognitoIdentityProviderClient *client G_GNUC_UNUSED,
                         const GetUserRequest& request G_GNUC_UNUSED,
                         const GetUserOutcome& outcome,
                         const std::shared_ptr<const AsyncCallerContext>& cx)
{
  GTask *task = std::static_pointer_cast<const GTaskAsyncContext> (cx)->task();

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int(aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_task_return_error (task, new_error);
      return;
    }

  GetUserResult *result_ref = get_user_move_result (std::move (outcome.GetResult ()));
  g_task_return_pointer (task, result_ref, [](void *data)
    {
      delete static_cast<GetUserResult *> (data);
    });
}

/**
 * cog_client_get_user:
 * @self: the #CogClient
 * @access_token: the access token returned by the server response
 * @cancellable: (nullable): optional #GCancellable object
 * @username: (out): the username of the user retrieved
 * @user_attributes: (out) (element-type utf8 utf8): a dictionary of user
 *   attributes
 * @mfa_options: (out) (element-type CogMFAOption): the options for MFA (e.g.,
 *   email or phone number)
 * @preferred_mfa_setting: (out): the user's preferred MFA setting
 * @user_mfa_settings_list: (out): list of the user's MFA settings
 * @error: error location
 *
 * Gets the user attributes and metadata for a user.
 *
 * For custom attributes, `custom:` will be prepended to the attribute keys in
 * @user_attributes.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_get_user (CogClient *self,
                     const char *access_token,
                     GCancellable *cancellable,
                     char **username,
                     GHashTable **user_attributes,
                     GList **mfa_options,
                     char **preferred_mfa_setting,
                     char ***user_mfa_settings_list,
                     GError **error)
{
  g_return_val_if_fail(COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail(get_user_validate_in_parameters (access_token), FALSE);
  g_return_val_if_fail(
    get_user_validate_out_parameters (username, user_attributes, mfa_options,
                                      preferred_mfa_setting,
                                      user_mfa_settings_list), FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  CogClientPrivate *priv = GET_PRIVATE (self);
  GetUserRequest request = get_user_build_request (access_token);
  auto outcome = priv->internal.GetUser (request);

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int (aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_propagate_error (error, new_error);
      return FALSE;
    }

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  get_user_unpack_result(outcome.GetResult (), username, user_attributes,
                         mfa_options, preferred_mfa_setting,
                         user_mfa_settings_list);

  return TRUE;
}

/**
 * cog_client_get_user_async:
 * @self: the #CogClient
 * @access_token: the access token returned by the server response
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (nullable): a callback to call when the operation is complete
 * @user_data: (nullable): the data to pass to @callback
 *
 * See cog_client_get_user() for documentation.
 * This version completes the request without blocking and calls @callback when
 * finished.
 * In your @callback, you must call cog_client_get_user_finish() to get the
 * results of the request.
 */
void
cog_client_get_user_async (CogClient *self,
                           const char *access_token,
                           GCancellable *cancellable,
                           GAsyncReadyCallback callback,
                           gpointer user_data)
{
  g_return_if_fail(COG_IS_CLIENT (self));
  g_return_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail(get_user_validate_in_parameters (access_token));

  GTask *task = g_task_new (self, cancellable, callback, user_data);

  CogClientPrivate *priv = GET_PRIVATE (self);
  GetUserRequest request = get_user_build_request (access_token);

  priv->internal.GetUserAsync (request, get_user_handle_request,
    Aws::MakeShared<GTaskAsyncContext> (_COG_ALLOCATION_TAG, task));
}

/**
 * cog_client_get_user_finish:
 * @self: the #CogClient
 * @res: the #GAsyncResult passed to your callback
 * @username: (out): the username of the user retrieved
 * @user_attributes: (out) (element-type utf8 utf8): a dictionary of user
 *   attributes
 * @mfa_options: (out) (element-type CogMFAOption): the options for MFA (e.g.,
 *   email or phone number)
 * @preferred_mfa_setting: (out): the user's preferred MFA setting
 * @user_mfa_settings_list: (out): list of the user's MFA settings
 * @error: error location
 *
 * See cog_client_get_user() for documentation.
 * After starting an asynchronous request with cog_client_get_user_async(), you
 * must call this in your callback to finish the request and receive the return
 * values or handle the errors.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_get_user_finish (CogClient *self,
                            GAsyncResult *res,
                            char **username,
                            GHashTable **user_attributes,
                            GList **mfa_options,
                            char **preferred_mfa_setting,
                            char ***user_mfa_settings_list,
                            GError **error)
{
  g_return_val_if_fail (COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (res), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail (
    get_user_validate_out_parameters (username, user_attributes, mfa_options,
                                      preferred_mfa_setting,
                                      user_mfa_settings_list), FALSE);

  auto *result =
    static_cast<GetUserResult *> (g_task_propagate_pointer (G_TASK (res), error));
  if (!result)
    return FALSE;

  get_user_unpack_result (*result, username, user_attributes, mfa_options,
                          preferred_mfa_setting, user_mfa_settings_list);
  delete result;
  return TRUE;
}

static gboolean
initiate_auth_validate_in_parameters (CogAuthFlow auth_flow,
                                      GHashTable *auth_parameters,
                                      const char *client_id,
                                      GHashTable *client_metadata G_GNUC_UNUSED,
                                      CogAnalyticsMetadata *analytics_metadata G_GNUC_UNUSED,
                                      CogUserContextData *user_context_data G_GNUC_UNUSED)
{
  g_return_val_if_fail(auth_flow != COG_AUTH_FLOW_NOT_SET &&
                       auth_flow != COG_AUTH_FLOW_ADMIN_NO_SRP_AUTH,
                       FALSE);
  g_return_val_if_fail(auth_parameters, FALSE);
  g_return_val_if_fail(client_id, FALSE);
  g_return_val_if_fail (*client_id, FALSE);
  g_return_val_if_fail (strlen (client_id) <= 128, FALSE);
  g_return_val_if_fail (_cog_is_valid_client_id (client_id), FALSE);

  switch (auth_flow)
    {
    case COG_AUTH_FLOW_CUSTOM_AUTH:
      g_return_val_if_fail (g_hash_table_contains (auth_parameters, COG_PARAMETER_USERNAME), FALSE);
      break;
    case COG_AUTH_FLOW_REFRESH_TOKEN:
    case COG_AUTH_FLOW_REFRESH_TOKEN_AUTH:
      g_return_val_if_fail (g_hash_table_contains (auth_parameters, COG_PARAMETER_REFRESH_TOKEN), FALSE);
      break;
    case COG_AUTH_FLOW_USER_PASSWORD_AUTH:
      g_return_val_if_fail (g_hash_table_contains (auth_parameters, COG_PARAMETER_USERNAME), FALSE);
      g_return_val_if_fail (g_hash_table_contains (auth_parameters, COG_PARAMETER_PASSWORD), FALSE);
      break;
    case COG_AUTH_FLOW_USER_SRP_AUTH:
      g_return_val_if_fail (g_hash_table_contains (auth_parameters, COG_PARAMETER_USERNAME), FALSE);
      g_return_val_if_fail (g_hash_table_contains (auth_parameters, COG_PARAMETER_SRP_A), FALSE);
      break;
    case COG_AUTH_FLOW_NOT_SET:
    case COG_AUTH_FLOW_ADMIN_NO_SRP_AUTH:
    default:
      g_assert_not_reached ();
    }

  return TRUE;
}

static gboolean
initiate_auth_validate_out_parameters (CogAuthenticationResult **auth_result,
                                       CogChallengeName *challenge_name,
                                       GHashTable **challenge_parameters,
                                       char * const *session)
{
  g_return_val_if_fail (auth_result, FALSE);
  g_return_val_if_fail (challenge_name, FALSE);
  g_return_val_if_fail (challenge_parameters, FALSE);
  g_return_val_if_fail (session, FALSE);
  return TRUE;
}

static InitiateAuthRequest
initiate_auth_build_request (CogAuthFlow auth_flow,
                             GHashTable *auth_parameters,
                             const char *client_id,
                             GHashTable *client_metadata,
                             CogAnalyticsMetadata *analytics_metadata,
                             CogUserContextData *user_context_data)
{
  InitiateAuthRequest request;
  request.WithAuthFlow (AuthFlowType (auth_flow))
    .SetClientId (client_id);

  g_hash_table_foreach (auth_parameters, [](void *key, void *value, void *data)
    {
      auto *request = static_cast<InitiateAuthRequest *> (data);
      request->AddAuthParameters(static_cast<const char *> (key),
                                 static_cast<const char *> (value));
    },
    &request);

  if (client_metadata)
    {
      g_hash_table_foreach (client_metadata, [](void *key, void *value, void *data)
        {
          auto *request = static_cast<InitiateAuthRequest *> (data);
          request->AddClientMetadata(static_cast<const char *> (key),
                                     static_cast<const char *> (value));
        },
        &request);
    }

  if (analytics_metadata)
    request.SetAnalyticsMetadata (_cog_analytics_metadata_to_internal (analytics_metadata));

  if (user_context_data)
    request.SetUserContextData (_cog_user_context_data_to_internal (user_context_data));

  return request;
}

static void
initiate_auth_unpack_result (InitiateAuthResult& result,
                             CogAuthenticationResult **auth_result,
                             CogChallengeName *challenge_name,
                             GHashTable **challenge_parameters,
                             char **session)
{
  *challenge_name = CogChallengeName (result.GetChallengeName ());
  if (*challenge_name != COG_CHALLENGE_NAME_NOT_SET)
    {
      *auth_result = NULL;
      *session = g_strdup (result.GetSession ().c_str ());

      *challenge_parameters = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
      for (auto const& kv : result.GetChallengeParameters ())
        {
          g_hash_table_insert (*challenge_parameters,
                               g_strdup (kv.first.c_str()),
                               g_strdup (kv.second.c_str()));
        }

      return;
    }

  *auth_result = _cog_authentication_result_from_internal (result.GetAuthenticationResult ());
  *challenge_parameters = NULL;
  *session = NULL;
}

/* Work around the lack of a copy or move constructor. See above */
static InitiateAuthResult *
initiate_auth_move_result (const InitiateAuthResult&& result)
{
  auto *retval = new InitiateAuthResult ();
  retval->SetAuthenticationResult (std::move (result.GetAuthenticationResult ()));
  retval->SetChallengeName (std::move (result.GetChallengeName ()));
  retval->SetChallengeParameters (std::move (result.GetChallengeParameters ()));
  retval->SetSession (std::move (result.GetSession ()));
  return retval;
}

static void
initiate_auth_handle_request (const CognitoIdentityProviderClient *client G_GNUC_UNUSED,
                              const InitiateAuthRequest& request G_GNUC_UNUSED,
                              const InitiateAuthOutcome& outcome,
                              const std::shared_ptr<const AsyncCallerContext>& cx)
{
  GTask *task = std::static_pointer_cast<const GTaskAsyncContext> (cx)->task();

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int(aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_task_return_error (task, new_error);
      return;
    }

  InitiateAuthResult *result_ref = initiate_auth_move_result (std::move (outcome.GetResult ()));
  g_task_return_pointer (task, result_ref, [](void *data)
    {
      delete static_cast<InitiateAuthResult *> (data);
    });
}

/**
 * cog_client_initiate_auth:
 * @self: the #CogClient
 * @auth_flow: the authentication flow for this call to execute
 * @auth_parameters: (element-type utf8 utf8): the authentication parameters
 * @client_id: the app client ID
 * @client_metadata: (nullable) (element-type utf8 utf8): a map for custom
 *   parameters
 * @analytics_metadata: (nullable): Amazon Pinpoint analytics metadata for
 *   collecting metrics
 * @user_context_data: (nullable): contextual data for security analysis
 * @cancellable: (nullable): optional #GCancellable object
 * @auth_result: (out) (nullable): the result of the authentication response, or
 *   %NULL if you need to pass another challenge
 * @challenge_name: (out): the name of the challenge to which you need to
 *   respond to complete this call, or %COG_CHALLENGE_NAME_NOT_SET if you do not
 *   need to pass another challenge
 * @challenge_parameters: (out) (nullable): the challenge parameters, or %NULL
 *   if you do not need to pass another challenge
 * @session: (out) (nullable): a session ID, or %NULL if you do not need to pass
 *   another challenge
 * @error: error location
 *
 * Initiates the authentication flow.
 * The API action will depend on the value of @auth_flow.
 * For example:
 * - %COG_AUTH_FLOW_REFRESH_TOKEN_AUTH will take in a valid refresh token and
 *   return new tokens.
 * - %COG_AUTH_FLOW_USER_SRP_AUTH will take in `USERNAME` and `SRP_A` and return
 *   the SRP variables to be used for the next challenge execution.
 * - %COG_AUTH_FLOW_USER_PASSWORD_AUTH will take in `USERNAME` and `PASSWORD`
 *   and return the next challenge or tokens.
 * %COG_AUTH_FLOW_ADMIN_NO_SRP_AUTH is not a valid value.
 *
 * The @auth_parameters are inputs corresponding to the @auth_flow that you are
 * invoking.
 * The required values depend on the value of @auth_flow:
 * - For %COG_AUTH_FLOW_USER_SRP_AUTH: `USERNAME` (required), `SRP_A`
 *   (required), `SECRET_HASH` (required if the app client is configured with a
 *   client secret), `DEVICE_KEY`
 * - For %COG_AUTH_FLOW_REFRESH_TOKEN_AUTH / %COG_AUTH_FLOW_REFRESH_TOKEN:
 *   `REFRESH_TOKEN` (required), `SECRET_HASH` (required if the app client is
 *   configured with a client secret), `DEVICE_KEY`
 * - For %COG_AUTH_FLOW_CUSTOM_AUTH: `USERNAME` (required), `SECRET_HASH`
 *   (required if the app client is configured with a client secret),
 *   `DEVICE_KEY`
 *
 * @client_metadata is an optional key-value pair map which can contain any key
 * and will be passed to your PreAuthentication Lambda trigger as-is.
 * It can be used to implement additional validations around authentication.
 *
 * @user_context_data is optional contextual data such as the user's device
 * fingerprint, IP address, or location used for evaluating the risk of an
 * unexpected event by Amazon Cognito advanced security.
 *
 * @auth_result is only returned if the caller does not need to pass another
 * challenge.
 * If the caller does need to pass another challenge before it gets tokens,
 * @challenge_name, @challenge_parameters, and @session are returned.
 * See #CogChallengeName for valid values for @challenge_name.
 * The responses in @challenge_parameters should be used to compute inputs to
 * the next call (cog_client_respond_to_auth_challenge()).
 * Note that all of these challenges require %COG_PARAMETER_USERNAME and
 * %COG_PARAMETER_SECRET_HASH (if applicable) in the parameters.
 * The @session should be passed both ways in challenge-response calls to the
 * service.
 * If the cog_client_initiate_auth() or cog_client_respond_to_auth_challenge()
 * call determines that the caller needs to go through another challenge, they
 * return a @session with other challenge parameters.
 * This @session should be passed as it is to the next
 * cog_client_respond_to_auth_challenge() call.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_initiate_auth (CogClient *self,
                          CogAuthFlow auth_flow,
                          GHashTable *auth_parameters,
                          const char *client_id,
                          GHashTable *client_metadata,
                          CogAnalyticsMetadata *analytics_metadata,
                          CogUserContextData *user_context_data,
                          GCancellable *cancellable,
                          CogAuthenticationResult **auth_result,
                          CogChallengeName *challenge_name,
                          GHashTable **challenge_parameters,
                          char **session,
                          GError **error)
{
  g_return_val_if_fail(COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail(
    initiate_auth_validate_in_parameters (auth_flow, auth_parameters, client_id,
                                          client_metadata, analytics_metadata,
                                          user_context_data), FALSE);
  g_return_val_if_fail(
    initiate_auth_validate_out_parameters (auth_result, challenge_name,
                                           challenge_parameters, session),
    FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  CogClientPrivate *priv = GET_PRIVATE (self);
  InitiateAuthRequest request =
    initiate_auth_build_request (auth_flow, auth_parameters, client_id,
                                 client_metadata, analytics_metadata,
                                 user_context_data);
  auto outcome = priv->internal.InitiateAuth (request);

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int (aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_propagate_error (error, new_error);
      return FALSE;
    }

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  initiate_auth_unpack_result(outcome.GetResult (), auth_result, challenge_name,
                              challenge_parameters, session);

  return TRUE;
}

/**
 * cog_client_initiate_auth_async:
 * @self: the #CogClient
 * @auth_flow: the authentication flow for this call to execute
 * @auth_parameters: (element-type utf8 utf8): the authentication parameters
 * @client_id: the app client ID
 * @client_metadata: (nullable) (element-type utf8 utf8): a map for custom
 *   parameters
 * @analytics_metadata: (nullable): Amazon Pinpoint analytics metadata for
 *   collecting metrics
 * @user_context_data: (nullable): contextual data for security analysis
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (nullable): a callback to call when the operation is complete
 * @user_data: (nullable): the data to pass to @callback
 *
 * See cog_client_initiate_auth() for documentation.
 * This version completes the request without blocking and calls @callback when
 * finished.
 * In your @callback, you must call cog_client_initiate_auth_finish() to get the
 * results of the request.
 */
void
cog_client_initiate_auth_async (CogClient *self,
                                CogAuthFlow auth_flow,
                                GHashTable *auth_parameters,
                                const char *client_id,
                                GHashTable *client_metadata,
                                CogAnalyticsMetadata *analytics_metadata,
                                CogUserContextData *user_context_data,
                                GCancellable *cancellable,
                                GAsyncReadyCallback callback,
                                gpointer user_data)
{
  g_return_if_fail(COG_IS_CLIENT (self));
  g_return_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail(
    initiate_auth_validate_in_parameters (auth_flow, auth_parameters, client_id,
                                          client_metadata, analytics_metadata,
                                          user_context_data));

  GTask *task = g_task_new (self, cancellable, callback, user_data);

  CogClientPrivate *priv = GET_PRIVATE (self);
  InitiateAuthRequest request =
    initiate_auth_build_request (auth_flow, auth_parameters, client_id,
                                 client_metadata, analytics_metadata,
                                 user_context_data);

  priv->internal.InitiateAuthAsync (request, initiate_auth_handle_request,
    Aws::MakeShared<GTaskAsyncContext> (_COG_ALLOCATION_TAG, task));
}

/**
 * cog_client_initiate_auth_finish:
 * @self: the #CogClient
 * @res: the #GAsyncResult passed to your callback
 * @auth_result: (out) (nullable): the result of the authentication response, or
 *   %NULL if you need to pass another challenge
 * @challenge_name: (out): the name of the challenge to which you need to
 *   respond to complete this call, or %COG_CHALLENGE_NAME_NOT_SET if you do not
 *   need to pass another challenge
 * @challenge_parameters: (out) (nullable): the challenge parameters, or %NULL
 *   if you do not need to pass another challenge
 * @session: (out) (nullable): a session ID, or %NULL if you do not need to pass
 *   another challenge
 * @error: error location
 *
 * See cog_client_initiate_auth() for documentation.
 * After starting an asynchronous request with cog_client_initiate_auth_async(),
 * you must call this in your callback to finish the request and receive the
 * return values or handle the errors.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_initiate_auth_finish (CogClient *self,
                                 GAsyncResult *res,
                                 CogAuthenticationResult **auth_result,
                                 CogChallengeName *challenge_name,
                                 GHashTable **challenge_parameters,
                                 char **session,
                                 GError **error)
{
  g_return_val_if_fail (COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (res), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail (
    initiate_auth_validate_out_parameters (auth_result, challenge_name,
                                           challenge_parameters, session),
    FALSE);

  auto *result =
    static_cast<InitiateAuthResult *> (g_task_propagate_pointer (G_TASK (res), error));
  if (!result)
    return FALSE;

  initiate_auth_unpack_result (*result, auth_result, challenge_name,
                               challenge_parameters, session);
  delete result;
  return TRUE;
}

static gboolean
sign_up_validate_in_parameters (const char *client_id,
                                const char *secret_hash,
                                const char *username,
                                const char *password,
                                GHashTable *user_attributes G_GNUC_UNUSED,
                                GHashTable *validation_data G_GNUC_UNUSED,
                                CogAnalyticsMetadata *analytics_metadata G_GNUC_UNUSED,
                                CogUserContextData *user_context_data G_GNUC_UNUSED)
{
  g_return_val_if_fail (client_id, FALSE);
  g_return_val_if_fail (*client_id, FALSE);
  g_return_val_if_fail (strlen (client_id) <= 128, FALSE);
  g_return_val_if_fail (_cog_is_valid_client_id (client_id), FALSE);
  g_return_val_if_fail (username, FALSE);
  g_return_val_if_fail (*username, FALSE);
  g_return_val_if_fail (strlen (username) <= 128, FALSE);
  g_return_val_if_fail (_cog_is_valid_username (username), FALSE);
  g_return_val_if_fail (password, FALSE);
  g_return_val_if_fail (*password, FALSE);
  g_return_val_if_fail (strlen (password) >= 6 && strlen (password) <= 256,
                        FALSE);
  g_return_val_if_fail (_cog_is_valid_password (password), FALSE);
  if (secret_hash)
    {
      g_return_val_if_fail (*secret_hash, FALSE);
      g_return_val_if_fail (strlen (secret_hash) > 128, FALSE);
      g_return_val_if_fail (_cog_is_valid_secret_hash (secret_hash), FALSE);
    }

  return TRUE;
}

static gboolean
sign_up_validate_out_parameters (gboolean *user_confirmed,
                                 CogCodeDeliveryDetails **code_delivery_details,
                                 const char **user_sub)
{
  g_return_val_if_fail (user_confirmed, FALSE);
  g_return_val_if_fail (code_delivery_details, FALSE);
  g_return_val_if_fail (user_sub, FALSE);
  return TRUE;
}

static SignUpRequest
sign_up_build_request (const char *client_id,
                       const char *secret_hash,
                       const char *username,
                       const char *password,
                       GHashTable *user_attributes,
                       GHashTable *validation_data,
                       CogAnalyticsMetadata *analytics_metadata,
                       CogUserContextData *user_context_data)
{
  SignUpRequest request;
  request.WithClientId (client_id)
    .WithUsername (username)
    .SetPassword (password);

  if (secret_hash)
    request.SetSecretHash (secret_hash);

  if (user_attributes)
    {
      Aws::Vector<AttributeType> vector;
      _cog_hash_table_to_vector (user_attributes, &vector);
      request.SetUserAttributes (vector);
    }

  if (validation_data)
    {
      Aws::Vector<AttributeType> vector;
      _cog_hash_table_to_vector (validation_data, &vector);
      request.SetValidationData (vector);
    }

  if (analytics_metadata)
    request.SetAnalyticsMetadata (_cog_analytics_metadata_to_internal (analytics_metadata));

  if (user_context_data)
    request.SetUserContextData (_cog_user_context_data_to_internal (user_context_data));

  return request;
}

static void
sign_up_unpack_result (SignUpResult& result,
                       gboolean *user_confirmed,
                       CogCodeDeliveryDetails **code_delivery_details,
                       const char **user_sub)
{
  *user_confirmed = result.GetUserConfirmed ();
  *code_delivery_details = _cog_code_delivery_details_from_internal (result.GetCodeDeliveryDetails ());
  *user_sub = g_strdup (result.GetUserSub ().c_str ());
}

/* Work around the lack of a copy or move constructor. See above. */
static SignUpResult *
sign_up_move_result (const SignUpResult&& result)
{
  auto *retval = new SignUpResult ();
  retval->SetUserConfirmed (result.GetUserConfirmed ());
  retval->SetCodeDeliveryDetails (std::move (result.GetCodeDeliveryDetails ()));
  retval->SetUserSub (std::move (result.GetUserSub ()));
  return retval;
}

static void
sign_up_handle_request (const CognitoIdentityProviderClient *client G_GNUC_UNUSED,
                        const SignUpRequest& request G_GNUC_UNUSED,
                        const SignUpOutcome& outcome,
                        const std::shared_ptr<const AsyncCallerContext>& cx)
{
  GTask *task = std::static_pointer_cast<const GTaskAsyncContext> (cx)->task();

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int(aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_task_return_error (task, new_error);
      return;
    }

  SignUpResult *result_ref = sign_up_move_result (std::move (outcome.GetResult ()));
  g_task_return_pointer (task, result_ref, [](void *data)
    {
      delete static_cast<SignUpResult *> (data);
    });
}

/**
 * cog_client_sign_up:
 * @self: the #CogClient
 * @client_id: the ID of the client associated with the user pool
 * @secret_hash: (nullable): hash of client secret, username, and client ID
 * @username: the user name of the user you wish to register
 * @password: the password of the user you wish to register
 * @user_attributes: (nullable) (element-type utf8 utf8): a dictionary of user
 *   attributes
 * @validation_data: (nullable) (element-type utf8 utf8): the validation data
 * @analytics_metadata: (nullable): Amazon Pinpoint analytics metadata for
 *   collecting metrics
 * @user_context_data: (nullable): contextual data for security analysis
 * @cancellable: (nullable): optional #GCancellable object
 * @user_confirmed: (out): a response from the server indicating that a user
 *   registration has been confirmed
 * @code_delivery_details: (out): the code delivery details returned by the
 *   server
 * @user_sub: (out): the UUID of the authenticated user
 * @error: error location
 *
 * Registers the user in the specified user pool and creates a user name,
 * password, and user attributes.
 *
 * If given, @secret_hash must be a keyed-hash message authentication code
 * (HMAC) calculated using the secret key of a user pool client and username
 * plus the client ID in the message.
 *
 * If including custom attributes in @user_attributes, you must prepend the
 * `custom:` prefix to the attribute key.
 *
 * @user_context_data is optional contextual data such as the user's device
 * fingerprint, IP address, or location used for evaluating the risk of an
 * unexpected event by Amazon Cognito advanced security.
 *
 * The returned @user_sub is not the same as @username.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_sign_up (CogClient *self,
                    const char *client_id,
                    const char *secret_hash,
                    const char *username,
                    const char *password,
                    GHashTable *user_attributes,
                    GHashTable *validation_data,
                    CogAnalyticsMetadata *analytics_metadata,
                    CogUserContextData *user_context_data,
                    GCancellable *cancellable,
                    gboolean *user_confirmed,
                    CogCodeDeliveryDetails **code_delivery_details,
                    const char **user_sub,
                    GError **error)
{
  g_return_val_if_fail(COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail(
    sign_up_validate_in_parameters (client_id, secret_hash, username, password,
                                    user_attributes, validation_data,
                                    analytics_metadata, user_context_data),
    FALSE);
  g_return_val_if_fail(
    sign_up_validate_out_parameters (user_confirmed, code_delivery_details,
                                     user_sub), FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  CogClientPrivate *priv = GET_PRIVATE (self);
  SignUpRequest request =
    sign_up_build_request (client_id, secret_hash, username, password,
                           user_attributes, validation_data, analytics_metadata,
                           user_context_data);
  auto outcome = priv->internal.SignUp (request);

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int (aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_propagate_error (error, new_error);
      return FALSE;
    }

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  sign_up_unpack_result(outcome.GetResult (), user_confirmed,
                        code_delivery_details, user_sub);

  return TRUE;
}

/**
 * cog_client_sign_up_async:
 * @self: the #CogClient
 * @client_id: the ID of the client associated with the user pool
 * @secret_hash: (nullable): hash of client secret, username, and client ID
 * @username: the user name of the user you wish to register
 * @password: the password of the user you wish to register
 * @user_attributes: (nullable) (element-type utf8 utf8): a dictionary of user
 *   attributes
 * @validation_data: (nullable) (element-type utf8 utf8): the validation data
 * @analytics_metadata: (nullable): Amazon Pinpoint analytics metadata for
 *   collecting metrics
 * @user_context_data: (nullable): contextual data for security analysis
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (nullable): a callback to call when the operation is complete
 * @user_data: (nullable): the data to pass to @callback
 *
 * See cog_client_sign_up() for documentation.
 * This version completes the request without blocking and calls @callback when
 * finished.
 * In your @callback, you must call cog_client_sign_up_finish() to get the
 * results of the request.
 */
void
cog_client_sign_up_async (CogClient *self,
                          const char *client_id,
                          const char *secret_hash,
                          const char *username,
                          const char *password,
                          GHashTable *user_attributes,
                          GHashTable *validation_data,
                          CogAnalyticsMetadata *analytics_metadata,
                          CogUserContextData *user_context_data,
                          GCancellable *cancellable,
                          GAsyncReadyCallback callback,
                          gpointer user_data)
{
  g_return_if_fail(COG_IS_CLIENT (self));
  g_return_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail(
    sign_up_validate_in_parameters (client_id, secret_hash, username, password,
                                    user_attributes, validation_data,
                                    analytics_metadata, user_context_data));

  GTask *task = g_task_new (self, cancellable, callback, user_data);

  CogClientPrivate *priv = GET_PRIVATE (self);
  SignUpRequest request =
    sign_up_build_request (client_id, secret_hash, username, password,
                           user_attributes, validation_data, analytics_metadata,
                           user_context_data);

  priv->internal.SignUpAsync (request, sign_up_handle_request,
    Aws::MakeShared<GTaskAsyncContext> (_COG_ALLOCATION_TAG, task));
}

/**
 * cog_client_sign_up_finish:
 * @self: the #CogClient
 * @res: the #GAsyncResult passed to your callback
 * @user_confirmed: (out): a response from the server indicating that a user
 *   registration has been confirmed
 * @code_delivery_details: (out): the code delivery details returned by the
 *   server
 * @user_sub: (out): the UUID of the authenticated user
 * @error: error location
 *
 * See cog_client_sign_up() for documentation.
 * After starting an asynchronous request with cog_client_sign_up_async(),
 * you must call this in your callback to finish the request and receive the
 * return values or handle the errors.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_sign_up_finish (CogClient *self,
                           GAsyncResult *res,
                           gboolean *user_confirmed,
                           CogCodeDeliveryDetails **code_delivery_details,
                           const char **user_sub,
                           GError **error)
{
  g_return_val_if_fail (COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (res), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail(
    sign_up_validate_out_parameters (user_confirmed, code_delivery_details,
                                     user_sub), FALSE);

  auto *result =
    static_cast<SignUpResult *> (g_task_propagate_pointer (G_TASK (res), error));
  if (!result)
    return FALSE;

  sign_up_unpack_result (*result, user_confirmed, code_delivery_details,
                         user_sub);
  delete result;
  return TRUE;
}

static gboolean
update_user_attributes_validate_in_parameters (const char *access_token,
                                               GHashTable *user_attributes)
{
  g_return_val_if_fail (access_token, FALSE);
  g_return_val_if_fail (_cog_is_valid_access_token (access_token), FALSE);
  g_return_val_if_fail (user_attributes, FALSE);
  return TRUE;
}

static gboolean
update_user_attributes_validate_out_parameters (GList **code_delivery_details)
{
  g_return_val_if_fail (code_delivery_details, FALSE);
  return TRUE;
}

static UpdateUserAttributesRequest
update_user_attributes_build_request (const char *access_token,
                                      GHashTable *user_attributes)
{
  UpdateUserAttributesRequest request;
  request.SetAccessToken (access_token);

  Aws::Vector<AttributeType> vector;
  _cog_hash_table_to_vector (user_attributes, &vector);
  request.SetUserAttributes (vector);

  return request;
}

static void
update_user_attributes_unpack_result (UpdateUserAttributesResult& result,
                                      GList **code_delivery_details_list)
{
  *code_delivery_details_list = NULL;
  for (auto& details : result.GetCodeDeliveryDetailsList ())
    *code_delivery_details_list =
      g_list_prepend (*code_delivery_details_list,
                      _cog_code_delivery_details_from_internal (details));
  *code_delivery_details_list = g_list_reverse (*code_delivery_details_list);
}

/* Work around the lack of a copy or move constructor. See above. */
static UpdateUserAttributesResult *
update_user_attributes_move_result (const UpdateUserAttributesResult&& result)
{
  auto *retval = new UpdateUserAttributesResult ();
  retval->SetCodeDeliveryDetailsList (std::move (result.GetCodeDeliveryDetailsList ()));
  return retval;
}

static void
update_user_attributes_handle_request (const CognitoIdentityProviderClient *client G_GNUC_UNUSED,
                                       const UpdateUserAttributesRequest& request G_GNUC_UNUSED,
                                       const UpdateUserAttributesOutcome& outcome,
                                       const std::shared_ptr<const AsyncCallerContext>& cx)
{
  GTask *task = std::static_pointer_cast<const GTaskAsyncContext> (cx)->task();

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int(aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_task_return_error (task, new_error);
      return;
    }

  UpdateUserAttributesResult *result_ref =
    update_user_attributes_move_result (std::move (outcome.GetResult ()));
  g_task_return_pointer (task, result_ref, [](void *data)
    {
      delete static_cast<UpdateUserAttributesResult *> (data);
    });
}

/**
 * cog_client_update_user_attributes:
 * @self: the #CogClient
 * @access_token: the access token returned by the server response
 * @user_attributes: (element-type utf8 utf8): a dictionary of user attributes
 * @cancellable: (nullable): optional #GCancellable object
 * @code_delivery_details_list: (out) (element-type CogCodeDeliveryDetails):
 *   list of code delivery details returned by the server
 * @error: error location
 *
 * Allows a user to update a specific attribute (one at a time).
 *
 * If including custom attributes in @user_attributes, you must prepend the
 * `custom:` prefix to the attribute key.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_update_user_attributes (CogClient *self,
                                   const char *access_token,
                                   GHashTable *user_attributes,
                                   GCancellable *cancellable,
                                   GList **code_delivery_details_list,
                                   GError **error)
{
  g_return_val_if_fail(COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail(
    update_user_attributes_validate_in_parameters (access_token,
                                                   user_attributes), FALSE);
  g_return_val_if_fail(
    update_user_attributes_validate_out_parameters (code_delivery_details_list),
    FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  CogClientPrivate *priv = GET_PRIVATE (self);
  UpdateUserAttributesRequest request =
    update_user_attributes_build_request (access_token, user_attributes);
  auto outcome = priv->internal.UpdateUserAttributes (request);

  if (!outcome.IsSuccess ())
    {
      auto& aws_error = outcome.GetError ();
      GError *new_error = g_error_new_literal (COG_IDENTITY_PROVIDER_ERROR,
                                               int (aws_error.GetErrorType ()),
                                               aws_error.GetMessage ().c_str ());
      g_propagate_error (error, new_error);
      return FALSE;
    }

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  update_user_attributes_unpack_result(outcome.GetResult (),
                                       code_delivery_details_list);

  return TRUE;
}

/**
 * cog_client_update_user_attributes_async:
 * @self: the #CogClient
 * @access_token: the access token returned by the server response
 * @user_attributes: (element-type utf8 utf8): a dictionary of user attributes
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (nullable): a callback to call when the operation is complete
 * @user_data: (nullable): the data to pass to @callback
 *
 * See cog_client_update_user_attributes() for documentation.
 * This version completes the request without blocking and calls @callback when
 * finished.
 * In your @callback, you must call cog_client_update_user_attributes_finish()
 * to get the results of the request.
 */
void
cog_client_update_user_attributes_async (CogClient *self,
                                         const char *access_token,
                                         GHashTable *user_attributes,
                                         GCancellable *cancellable,
                                         GAsyncReadyCallback callback,
                                         gpointer user_data)
{
  g_return_if_fail(COG_IS_CLIENT (self));
  g_return_if_fail(!cancellable || G_IS_CANCELLABLE (cancellable));
  g_return_if_fail(
    update_user_attributes_validate_in_parameters (access_token,
                                                   user_attributes));

  GTask *task = g_task_new (self, cancellable, callback, user_data);

  CogClientPrivate *priv = GET_PRIVATE (self);
  UpdateUserAttributesRequest request =
    update_user_attributes_build_request (access_token, user_attributes);

  priv->internal.UpdateUserAttributesAsync (request,
    update_user_attributes_handle_request,
    Aws::MakeShared<GTaskAsyncContext> (_COG_ALLOCATION_TAG, task));
}

/**
 * cog_client_update_user_attributes_finish:
 * @self: the #CogClient
 * @res: the #GAsyncResult passed to your callback
 * @code_delivery_details_list: (out) (element-type CogCodeDeliveryDetails):
 *   list of code delivery details returned by the server
 * @error: error location
 *
 * See cog_client_update_user_attributes() for documentation.
 * After starting an asynchronous request with
 * cog_client_update_user_attributes_async(), you must call this in your
 * callback to finish the request and receive the return values or handle the
 * errors.
 *
 * Returns: %TRUE if the request completed successfully, %FALSE on error
 */
gboolean
cog_client_update_user_attributes_finish (CogClient *self,
                                          GAsyncResult *res,
                                          GList **code_delivery_details_list,
                                          GError **error)
{
  g_return_val_if_fail (COG_IS_CLIENT (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (res), FALSE);
  g_return_val_if_fail(!error || !*error, FALSE);
  g_return_val_if_fail(
    update_user_attributes_validate_out_parameters (code_delivery_details_list),
    FALSE);

  auto *result =
    static_cast<UpdateUserAttributesResult *> (g_task_propagate_pointer (G_TASK (res), error));
  if (!result)
    return FALSE;

  update_user_attributes_unpack_result (*result, code_delivery_details_list);
  delete result;
  return TRUE;
}
