#pragma once

#if !(defined(_COG_INSIDE_COG_H) || defined(COMPILING_LIBCOG))
#error "Please do not include this header file directly."
#endif

#include <glib-object.h>
#include <gio/gio.h>

#include "cog/cog-analytics-metadata.h"
#include "cog/cog-authentication-result.h"
#include "cog/cog-macros.h"
#include "cog/cog-user-context-data.h"

G_BEGIN_DECLS

/**
 * CogAuthFlow:
 * @COG_AUTH_FLOW_NOT_SET: None, invalid value.
 * @COG_AUTH_FLOW_USER_SRP_AUTH: Authentication flow for the Secure Remote
 *   Password (SRP) protocol.
 * @COG_AUTH_FLOW_REFRESH_TOKEN_AUTH: Authentication flow for refreshing
 *   the access token and ID token by supplying a valid refresh token.
 * @COG_AUTH_FLOW_REFRESH_TOKEN: See %COG_AUTH_FLOW_REFRESH_TOKEN_AUTH
 * @COG_AUTH_FLOW_CUSTOM_AUTH: Custom authentication flow.
 * @COG_AUTH_FLOW_ADMIN_NO_SRP_AUTH: Admin authentication flow for use in
 *   backend applications.
 * @COG_AUTH_FLOW_USER_PASSWORD_AUTH: Non-SRP authentication flow;
 *   `USERNAME` and `PASSWORD` are passed directly.
 *   If a user migration Lambda trigger is set, this flow will invoke the user
 *   migration Lambda if the `USERNAME` is not found in the user pool.
 *
 * Type of authentication flow.
 */
typedef enum {
  COG_AUTH_FLOW_NOT_SET,
  COG_AUTH_FLOW_USER_SRP_AUTH,
  COG_AUTH_FLOW_REFRESH_TOKEN_AUTH,
  COG_AUTH_FLOW_REFRESH_TOKEN,
  COG_AUTH_FLOW_CUSTOM_AUTH,
  COG_AUTH_FLOW_ADMIN_NO_SRP_AUTH,
  COG_AUTH_FLOW_USER_PASSWORD_AUTH,
} CogAuthFlow;

/**
 * CogChallengeName:
 * @COG_CHALLENGE_NAME_NOT_SET: None, invalid value.
 * @COG_CHALLENGE_NAME_SMS_MFA: Next challenge is to supply a
 *   %COG_PARAMETER_SMS_MFA_CODE, delivered via SMS.
 * @COG_CHALLENGE_NAME_SOFTWARE_TOKEN_MFA:
 * @COG_CHALLENGE_NAME_SELECT_MFA_TYPE:
 * @COG_CHALLENGE_NAME_MFA_SETUP:
 * @COG_CHALLENGE_NAME_PASSWORD_VERIFIER: Next challenge is to supply
 *   %COG_PARAMETER_PASSWORD_CLAIM_SIGNATURE,
 *   %COG_PARAMETER_PASSWORD_CLAIM_SECRET_BLOCK, and %COG_PARAMETER_TIMESTAMP
 *   after the client-side SRP calculations.
 * @COG_CHALLENGE_NAME_CUSTOM_CHALLENGE: This is returned if your custom
 *   authentication flow determines that the user should pass another challenge
 *   before tokens are issued.
 * @COG_CHALLENGE_NAME_DEVICE_SRP_AUTH: If device tracking was enabled on your
 *   user pool and the previous challenges were passed, this challenge is
 *   returned so that Amazon Cognito can start tracking this device.
 * @COG_CHALLENGE_NAME_DEVICE_PASSWORD_VERIFIER: Similar to
 *   %COG_CHALLENGE_NAME_PASSWORD_VERIFIER, but for devices only.
 * @COG_CHALLENGE_NAME_ADMIN_NO_SRP_AUTH:
 * @COG_CHALLENGE_NAME_NEW_PASSWORD_REQUIRED: For users which are required to
 *   change their passwords after successful first login.
 *   This challenge should be passed with %COG_PARAMETER_NEW_PASSWORD and any
 *   other required attributes.
 *
 * Type of subsequent authentication challenge required to complete a login.
 */
typedef enum {
  COG_CHALLENGE_NAME_NOT_SET,
  COG_CHALLENGE_NAME_SMS_MFA,
  COG_CHALLENGE_NAME_SOFTWARE_TOKEN_MFA,
  COG_CHALLENGE_NAME_SELECT_MFA_TYPE,
  COG_CHALLENGE_NAME_MFA_SETUP,
  COG_CHALLENGE_NAME_PASSWORD_VERIFIER,
  COG_CHALLENGE_NAME_CUSTOM_CHALLENGE,
  COG_CHALLENGE_NAME_DEVICE_SRP_AUTH,
  COG_CHALLENGE_NAME_DEVICE_PASSWORD_VERIFIER,
  COG_CHALLENGE_NAME_ADMIN_NO_SRP_AUTH,
  COG_CHALLENGE_NAME_NEW_PASSWORD_REQUIRED,
} CogChallengeName;

/* Defines for hashtable keys */

/**
 * COG_PARAMETER_DEVICE_KEY:
 *
 * Device key.
 */
#define COG_PARAMETER_DEVICE_KEY "DEVICE_KEY"
/**
 * COG_PARAMETER_NEW_PASSWORD:
 */
#define COG_PARAMETER_NEW_PASSWORD "NEW_PASSWORD"
/**
 * COG_PARAMETER_PASSWORD:
 *
 * Password.
 */
#define COG_PARAMETER_PASSWORD "PASSWORD"
/**
 * COG_PARAMETER_PASSWORD_CLAIM_SECRET_BLOCK:
 */
#define COG_PARAMETER_PASSWORD_CLAIM_SECRET_BLOCK "PASSWORD_CLAIM_SECRET_BLOCK"
/**
 * COG_PARAMETER_PASSWORD_CLAIM_SIGNATURE:
 */
#define COG_PARAMETER_PASSWORD_CLAIM_SIGNATURE "PASSWORD_CLAIM_SIGNATURE"
/**
 * COG_PARAMETER_REFRESH_TOKEN:
 *
 * Refresh token as received from a previous authentication.
 * See `auth_parameters` in cog_client_initiate_auth().
 */
#define COG_PARAMETER_REFRESH_TOKEN "REFRESH_TOKEN"
/**
 * COG_PARAMETER_SECRET_HASH:
 *
 * Hash of the client secret.
 */
#define COG_PARAMETER_SECRET_HASH "SECRET_HASH"
/**
 * COG_PARAMETER_SMS_MFA_CODE:
 */
#define COG_PARAMETER_SMS_MFA_CODE "SMS_MFA_CODE"
/**
 * COG_PARAMETER_SRP_A:
 *
 * The value named *A* in the [Secure Remote Password
 * protocol](https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol), as
 * a hex string.
 */
#define COG_PARAMETER_SRP_A "SRP_A"
/**
 * COG_PARAMETER_TIMESTAMP:
 */
#define COG_PARAMETER_TIMESTAMP "TIMESTAMP"
/**
 * COG_PARAMETER_USERNAME:
 *
 * Username.
 */
#define COG_PARAMETER_USERNAME "USERNAME"

#define COG_TYPE_CLIENT (cog_client_get_type())

COG_AVAILABLE_IN_ALL
G_DECLARE_FINAL_TYPE (CogClient, cog_client, COG, CLIENT, GObject)

struct _CogClientClass
{
  GObjectClass parent_class;
};

COG_AVAILABLE_IN_ALL
CogClient *cog_client_new (void);

COG_AVAILABLE_IN_ALL
gboolean cog_client_initiate_auth (CogClient *self,
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
                                   GError **error);

COG_AVAILABLE_IN_ALL
void cog_client_initiate_auth_async (CogClient *self,
                                     CogAuthFlow auth_flow,
                                     GHashTable *auth_parameters,
                                     const char *client_id,
                                     GHashTable *client_metadata,
                                     CogAnalyticsMetadata *analytics_metadata,
                                     CogUserContextData *user_context_data,
                                     GCancellable *cancellable,
                                     GAsyncReadyCallback callback,
                                     gpointer user_data);

COG_AVAILABLE_IN_ALL
gboolean cog_client_initiate_auth_finish (CogClient *self,
                                          GAsyncResult *res,
                                          CogAuthenticationResult **auth_result,
                                          CogChallengeName *challenge_name,
                                          GHashTable **challenge_parameters,
                                          char **session,
                                          GError **error);

G_END_DECLS
