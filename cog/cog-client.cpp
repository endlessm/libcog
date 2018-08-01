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
#include <aws/cognito-idp/model/InitiateAuthRequest.h>
#include <aws/core/utils/Outcome.h>
#include <gio/gio.h>

#include "cog/cog-analytics-metadata.h"
#include "cog/cog-authentication-result.h"
#include "cog/cog-boxed-private.h"
#include "cog/cog-client.h"
#include "cog/cog-user-context-data.h"
#include "cog/cog-utils.h"
#include "cog/cog-utils-private.h"

#define GET_PRIVATE(o) (static_cast<CogClientPrivate *> (cog_client_get_instance_private (COG_CLIENT (o))))

using Aws::Client::AsyncCallerContext;
using Aws::CognitoIdentityProvider::CognitoIdentityProviderClient;
using Aws::CognitoIdentityProvider::Model::AuthFlowType;
using Aws::CognitoIdentityProvider::Model::InitiateAuthOutcome;
using Aws::CognitoIdentityProvider::Model::InitiateAuthRequest;
using Aws::CognitoIdentityProvider::Model::InitiateAuthResult;

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

/* METHODS */

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

/* Work around the lack of a copy or move constructor for InitiateAuthResult.
 * Returns an InitiateAuthResult allocated on the heap, suitable for returning
 * as a pointer with g_task_return_pointer() */
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
