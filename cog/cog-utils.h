#pragma once

#if !(defined(_COG_INSIDE_COG_H) || defined(COMPILING_LIBCOG))
#error "Please do not include this header file directly."
#endif

#include <glib.h>

#include "cog/cog-macros.h"

G_BEGIN_DECLS

/**
 * CogDeliveryMedium:
 * @COG_DELIVERY_MEDIUM_NOT_SET: None, invalid value.
 * @COG_DELIVERY_MEDIUM_SMS:
 * @COG_DELIVERY_MEDIUM_EMAIL:
 *
 * The delivery medium (email message or phone number).
 */
typedef enum {
  COG_DELIVERY_MEDIUM_NOT_SET,
  COG_DELIVERY_MEDIUM_SMS,
  COG_DELIVERY_MEDIUM_EMAIL,
} CogDeliveryMedium;

/**
 * CogIdentityProviderError:
 * @COG_IDENTITY_PROVIDER_ERROR_INCOMPLETE_SIGNATURE: The request signature does
 *   not conform to AWS standards. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INTERNAL_FAILURE: The request processing has
 *   failed because of an unknown error, exception or failure. (500)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_ACTION: The action or operation
 *   requested is invalid.
 *   Verify that the action is typed correctly. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_CLIENT_TOKEN_ID: The X.509 certificate
 *   or AWS access key ID provided does not exist in our records. (403)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_PARAMETER_COMBINATION: Parameters that
 *   must not be used together were used together. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_QUERY_PARAMETER: The AWS query string is
 *   malformed or does not adhere to AWS standards. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_PARAMETER_VALUE: An invalid or
 *   out-of-range value was supplied for the input parameter. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_MISSING_ACTION: The request is missing an action
 *   or a required parameter. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_MISSING_AUTHENTICATION_TOKEN: The request must
 *   contain either a valid (registered) AWS access key ID or X.509 certificate.
 *   (403)
 * @COG_IDENTITY_PROVIDER_ERROR_MISSING_PARAMETER: A required parameter for the
 *   specified action is not supplied. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_OPT_IN_REQUIRED: The AWS access key ID needs a
 *   subscription for the service. (403)
 * @COG_IDENTITY_PROVIDER_ERROR_REQUEST_EXPIRED: The request reached the service
 *   more than 15 minutes after the date stamp on the request or more than 15
 *   minutes after the request expiration date (such as for pre-signed URLs), or
 *   the date stamp on the request is more than 15 minutes in the future. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_SERVICE_UNAVAILABLE: The request has failed due
 *   to a temporary failure of the server. (503)
 * @COG_IDENTITY_PROVIDER_ERROR_THROTTLING: The request was denied due to
 *   request throttling. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_VALIDATION: The input fails to satisfy the
 *   constraints specified by an AWS service. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_ACCESS_DENIED: You do not have sufficient access
 *   to perform this action. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_RESOURCE_NOT_FOUND: This exception is thrown
 *   when the Amazon Cognito service cannot find the requested resource. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_UNRECOGNIZED_CLIENT:
 * @COG_IDENTITY_PROVIDER_ERROR_MALFORMED_QUERY_STRING: The query string
 *   contains a syntax error. (404)
 * @COG_IDENTITY_PROVIDER_ERROR_SLOW_DOWN:
 * @COG_IDENTITY_PROVIDER_ERROR_REQUEST_TIME_TOO_SKEWED:
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_SIGNATURE:
 * @COG_IDENTITY_PROVIDER_ERROR_SIGNATURE_DOES_NOT_MATCH:
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_ACCESS_KEY_ID:
 * @COG_IDENTITY_PROVIDER_ERROR_NETWORK_CONNECTION:
 * @COG_IDENTITY_PROVIDER_ERROR_UNKNOWN:
 * @COG_IDENTITY_PROVIDER_ERROR_ALIAS_EXISTS: This exception is thrown when a
 *   user tries to confirm the account with an email or phone number that has
 *   already been supplied as an alias from a different account.
 *   This exception tells user the that an account with this email or phone
 *   already exists. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_CODE_DELIVERY_FAILURE: This exception is thrown
 *   when a verification code fails to deliver successfully. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_CODE_MISMATCH: This exception is thrown if the
 *   provided code does not match what the server was expecting. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_CONCURRENT_MODIFICATION:
 * @COG_IDENTITY_PROVIDER_ERROR_DUPLICATE_PROVIDER:
 * @COG_IDENTITY_PROVIDER_ERROR_ENABLE_SOFTWARE_TOKEN_M_F_A:
 * @COG_IDENTITY_PROVIDER_ERROR_EXPIRED_CODE:
 * @COG_IDENTITY_PROVIDER_ERROR_GROUP_EXISTS:
 * @COG_IDENTITY_PROVIDER_ERROR_INTERNAL_ERROR: This exception is thrown when
 *   Amazon Cognito encounters an internal error. (500)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_EMAIL_ROLE_ACCESS_POLICY: This exception
 *   is thrown when Amazon Cognito is not allowed to use your email identity.
 *   (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_LAMBDA_RESPONSE: This exception is
 *   thrown when the Amazon Cognito service encounters an invalid AWS Lambda
 *   response. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_O_AUTH_FLOW:
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_PARAMETER: This exception is thrown when
 *   the Amazon Cognito service encounters an invalid parameter. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_PASSWORD: This exception is thrown when
 *   the Amazon Cognito service encounters an invalid password. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_SMS_ROLE_ACCESS_POLICY: This exception
 *   is returned when the role provided for SMS configuration does not have
 *   permission to publish using Amazon SNS. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_SMS_ROLE_TRUST_RELATIONSHIP: This
 *   exception is thrown when the trust relationship is invalid for the role
 *   provided for SMS configuration.
 *   This can happen if you do not trust `cognito-idp.amazonaws.com` or the
 *   external ID provided in the role does not match what is provided in the SMS
 *   configuration for the user pool. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_INVALID_USER_POOL_CONFIGURATION: This exception
 *   is thrown when the user pool configuration is invalid. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_LIMIT_EXCEEDED:
 * @COG_IDENTITY_PROVIDER_ERROR_M_F_A_METHOD_NOT_FOUND:
 * @COG_IDENTITY_PROVIDER_ERROR_NOT_AUTHORIZED: This exception is thrown when a
 *   user is not authorized. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_PASSWORD_RESET_REQUIRED: This exception is
 *   thrown when a password reset is required. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_PRECONDITION_NOT_MET:
 * @COG_IDENTITY_PROVIDER_ERROR_SCOPE_DOES_NOT_EXIST:
 * @COG_IDENTITY_PROVIDER_ERROR_SOFTWARE_TOKEN_M_F_A_NOT_FOUND:
 * @COG_IDENTITY_PROVIDER_ERROR_TOO_MANY_FAILED_ATTEMPTS:
 * @COG_IDENTITY_PROVIDER_ERROR_TOO_MANY_REQUESTS: This exception is thrown when
 *   the user has made too many requests for a given operation. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_UNEXPECTED_LAMBDA: This exception is thrown when
 *   the Amazon Cognito service encounters an unexpected exception with the AWS
 *   Lambda service. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_UNSUPPORTED_IDENTITY_PROVIDER:
 * @COG_IDENTITY_PROVIDER_ERROR_UNSUPPORTED_USER_STATE:
 * @COG_IDENTITY_PROVIDER_ERROR_USERNAME_EXISTS: This exception is thrown when
 *   Amazon Cognito encounters a user name that already exists in the user pool.
 *   (400)
 * @COG_IDENTITY_PROVIDER_ERROR_USER_IMPORT_IN_PROGRESS:
 * @COG_IDENTITY_PROVIDER_ERROR_USER_LAMBDA_VALIDATION: This exception is thrown
 *   when the Amazon Cognito service encounters a user validation exception with
 *   the AWS Lambda service. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_USER_NOT_CONFIRMED: This exception is thrown
 *   when a user is not confirmed successfully. (400)
 * @COG_IDENTITY_PROVIDER_ERROR_USER_NOT_FOUND: This exception is thrown when a
 *   user is not found. (400)
 *
 * Error enumeration for errors from the remote Cognito Identity Provider API.
 * The corresponding HTTP response codes are given in parentheses.
 */
typedef enum {
  COG_IDENTITY_PROVIDER_ERROR_INCOMPLETE_SIGNATURE,
  COG_IDENTITY_PROVIDER_ERROR_INTERNAL_FAILURE,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_ACTION,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_CLIENT_TOKEN_ID,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_PARAMETER_COMBINATION,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_QUERY_PARAMETER,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_PARAMETER_VALUE,
  COG_IDENTITY_PROVIDER_ERROR_MISSING_ACTION,
  COG_IDENTITY_PROVIDER_ERROR_MISSING_AUTHENTICATION_TOKEN,
  COG_IDENTITY_PROVIDER_ERROR_MISSING_PARAMETER,
  COG_IDENTITY_PROVIDER_ERROR_OPT_IN_REQUIRED,
  COG_IDENTITY_PROVIDER_ERROR_REQUEST_EXPIRED,
  COG_IDENTITY_PROVIDER_ERROR_SERVICE_UNAVAILABLE,
  COG_IDENTITY_PROVIDER_ERROR_THROTTLING,
  COG_IDENTITY_PROVIDER_ERROR_VALIDATION,
  COG_IDENTITY_PROVIDER_ERROR_ACCESS_DENIED,
  COG_IDENTITY_PROVIDER_ERROR_RESOURCE_NOT_FOUND,
  COG_IDENTITY_PROVIDER_ERROR_UNRECOGNIZED_CLIENT,
  COG_IDENTITY_PROVIDER_ERROR_MALFORMED_QUERY_STRING,
  COG_IDENTITY_PROVIDER_ERROR_SLOW_DOWN,
  COG_IDENTITY_PROVIDER_ERROR_REQUEST_TIME_TOO_SKEWED,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_SIGNATURE,
  COG_IDENTITY_PROVIDER_ERROR_SIGNATURE_DOES_NOT_MATCH,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_ACCESS_KEY_ID,
  COG_IDENTITY_PROVIDER_ERROR_NETWORK_CONNECTION,
  COG_IDENTITY_PROVIDER_ERROR_UNKNOWN,
  COG_IDENTITY_PROVIDER_ERROR_ALIAS_EXISTS,
  COG_IDENTITY_PROVIDER_ERROR_CODE_DELIVERY_FAILURE,
  COG_IDENTITY_PROVIDER_ERROR_CODE_MISMATCH,
  COG_IDENTITY_PROVIDER_ERROR_CONCURRENT_MODIFICATION,
  COG_IDENTITY_PROVIDER_ERROR_DUPLICATE_PROVIDER,
  COG_IDENTITY_PROVIDER_ERROR_ENABLE_SOFTWARE_TOKEN_M_F_A,
  COG_IDENTITY_PROVIDER_ERROR_EXPIRED_CODE,
  COG_IDENTITY_PROVIDER_ERROR_GROUP_EXISTS,
  COG_IDENTITY_PROVIDER_ERROR_INTERNAL_ERROR,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_EMAIL_ROLE_ACCESS_POLICY,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_LAMBDA_RESPONSE,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_O_AUTH_FLOW,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_PARAMETER,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_PASSWORD,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_SMS_ROLE_ACCESS_POLICY,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_SMS_ROLE_TRUST_RELATIONSHIP,
  COG_IDENTITY_PROVIDER_ERROR_INVALID_USER_POOL_CONFIGURATION,
  COG_IDENTITY_PROVIDER_ERROR_LIMIT_EXCEEDED,
  COG_IDENTITY_PROVIDER_ERROR_M_F_A_METHOD_NOT_FOUND,
  COG_IDENTITY_PROVIDER_ERROR_NOT_AUTHORIZED,
  COG_IDENTITY_PROVIDER_ERROR_PASSWORD_RESET_REQUIRED,
  COG_IDENTITY_PROVIDER_ERROR_PRECONDITION_NOT_MET,
  COG_IDENTITY_PROVIDER_ERROR_SCOPE_DOES_NOT_EXIST,
  COG_IDENTITY_PROVIDER_ERROR_SOFTWARE_TOKEN_M_F_A_NOT_FOUND,
  COG_IDENTITY_PROVIDER_ERROR_TOO_MANY_FAILED_ATTEMPTS,
  COG_IDENTITY_PROVIDER_ERROR_TOO_MANY_REQUESTS,
  COG_IDENTITY_PROVIDER_ERROR_UNEXPECTED_LAMBDA,
  COG_IDENTITY_PROVIDER_ERROR_UNSUPPORTED_IDENTITY_PROVIDER,
  COG_IDENTITY_PROVIDER_ERROR_UNSUPPORTED_USER_STATE,
  COG_IDENTITY_PROVIDER_ERROR_USERNAME_EXISTS,
  COG_IDENTITY_PROVIDER_ERROR_USER_IMPORT_IN_PROGRESS,
  COG_IDENTITY_PROVIDER_ERROR_USER_LAMBDA_VALIDATION,
  COG_IDENTITY_PROVIDER_ERROR_USER_NOT_CONFIRMED,
  COG_IDENTITY_PROVIDER_ERROR_USER_NOT_FOUND,
  COG_IDENTITY_PROVIDER_ERROR_USER_POOL_ADD_ON_NOT_ENABLED,
  COG_IDENTITY_PROVIDER_ERROR_USER_POOL_TAGGING,
} CogIdentityProviderError;

/**
 * COG_IDENTITY_PROVIDER_ERROR:
 *
 * Error domain for the #CogIdentityProviderError enumeration.
 */
#define COG_IDENTITY_PROVIDER_ERROR cog_identity_provider_error_quark ()

COG_AVAILABLE_IN_ALL
GQuark cog_identity_provider_error_quark (void);

G_END_DECLS
