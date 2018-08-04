#pragma once

#include <aws/cognito-idp/model/AnalyticsMetadataType.h>
#include <aws/cognito-idp/model/AuthenticationResultType.h>
#include <aws/cognito-idp/model/CodeDeliveryDetailsType.h>
#include <aws/cognito-idp/model/NewDeviceMetadataType.h>
#include <aws/cognito-idp/model/UserContextDataType.h>

#include "cog/cog-analytics-metadata.h"
#include "cog/cog-authentication-result.h"
#include "cog/cog-code-delivery-details.h"
#include "cog/cog-new-device-metadata.h"
#include "cog/cog-user-context-data.h"

/* Functions that shouldn't be exposed in the API, for marshalling boxed types
 * to and from their AWS SDK equivalents */

Aws::CognitoIdentityProvider::Model::AnalyticsMetadataType _cog_analytics_metadata_to_internal (CogAnalyticsMetadata *self);
Aws::CognitoIdentityProvider::Model::UserContextDataType _cog_user_context_data_to_internal (CogUserContextData *self);
CogAuthenticationResult *_cog_authentication_result_from_internal (const Aws::CognitoIdentityProvider::Model::AuthenticationResultType& internal);
CogCodeDeliveryDetails *_cog_code_delivery_details_from_internal (const Aws::CognitoIdentityProvider::Model::CodeDeliveryDetailsType& internal);
CogNewDeviceMetadata *_cog_new_device_metadata_from_internal (const Aws::CognitoIdentityProvider::Model::NewDeviceMetadataType& internal);
