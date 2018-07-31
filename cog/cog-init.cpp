#include <aws/core/Aws.h>

#include "cog/cog-init.h"

/**
 * SECTION:init
 * @title: Initialization
 * @short_description: Initializing and shutting down the AWS SDK
 *
 * To use the AWS SDK and therefore Cognito, you should properly initialize it
 * with cog_init_default() before creating service clients and using them.
 * You should then shut down the AWS SDK with cog_shutdown().
 *
 * You can set additional run-time options.
 *
 * # Initializing and Shutting Down the AWS SDK #
 *
 * A basic skeleton application looks like this:
 *
 * |[<!-- language="C" -->
 * #include &lt;cog/cog.h&gt;
 * int
 * main (int argc,
 *       const char *argv[])
 * {
 *   cog_init_default ();
 *   // make your SDK calls here.
 *   cog_shutdown ();
 *   return 0;
 * }
 * ]|
 */

static Aws::SDKOptions options;
static bool is_inited;

/**
 * cog_init_default:
 *
 * Initialize the AWS SDK with all the default logging, crypto, HTTP client, and
 * memory management initialization options.
 * This method must be called before any other Libcog methods, except methods
 * that set initialization options.
 */
void
cog_init_default (void)
{
  g_return_if_fail (!is_inited);

  Aws::InitAPI (options);
  is_inited = true;
}

/**
 * cog_is_inited:
 *
 * Checks if the library was already initialized.
 * It's not allowed to call cog_init_default() or any other initialization
 * functions more than once.
 *
 * Returns: %TRUE if the library has already been initialized, %FALSE otherwise.
 */
gboolean
cog_is_inited (void)
{
  return is_inited;
}

/**
 * cog_shutdown:
 *
 * Shutdown global state for the AWS SDK.
 * This method must be called when you are finished using Libcog.
 * Do not call any other Libcog methods after calling cog_shutdown().
 */
void
cog_shutdown (void)
{
  g_return_if_fail (is_inited);

  Aws::ShutdownAPI (options);
}
