#pragma once

#include <aws/cognito-idp/model/AttributeType.h>
#include <aws/core/utils/memory/stl/AWSVector.h>
#include <glib.h>

/* This "allocation tag" is passed to all AWS memory management functions */
#define _COG_ALLOCATION_TAG "libcog"

void _cog_free_static_data (void);

gboolean _cog_is_valid_access_token (const char *string);
gboolean _cog_is_valid_client_id (const char *string);
gboolean _cog_is_valid_password (const char *string);
gboolean _cog_is_valid_secret_hash (const char *string);
gboolean _cog_is_valid_username (const char *string);

void _cog_hash_table_to_vector (GHashTable *hash_table,
                                Aws::Vector<Aws::CognitoIdentityProvider::Model::AttributeType> *vector);

void _cog_vector_to_hash_table (const Aws::Vector<Aws::CognitoIdentityProvider::Model::AttributeType>& vector,
                                GHashTable *hash_table);

char **_cog_vector_to_strv (const Aws::Vector<Aws::String>& vector);
