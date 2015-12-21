/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2009-2015 Piers Harding                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Piers Harding <piers@ompka.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id: php_sapnwrfc.h,v 1.1.1.1 2004/02/04 21:13:46 boerger Exp $ */

#ifndef PHP_SAPNWRFC_H
#define PHP_SAPNWRFC_H

extern zend_module_entry sapnwrfc_module_entry;
#define phpext_sapnwrfc_ptr &sapnwrfc_module_entry

#ifdef PHP_WIN32
#define PHP_SAPNWRFC_API __declspec(dllexport)
#else
#define PHP_SAPNWRFC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(sapnwrfc);
PHP_MSHUTDOWN_FUNCTION(sapnwrfc);
PHP_RINIT_FUNCTION(sapnwrfc);
PHP_RSHUTDOWN_FUNCTION(sapnwrfc);
PHP_MINFO_FUNCTION(sapnwrfc);

PHP_FUNCTION(confirm_sapnwrfc_compiled);	/* For testing, remove later. */

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(sapnwrfc)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(sapnwrfc)
*/

/* In every utility function you add that needs to use variables
   in php_sapnwrfc_globals, call TSRM_FETCH(); after declaring other
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as SAPNWRFC_G(variable).  You are
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define SAPNWRFC_G(v) TSRMG(sapnwrfc_globals_id, zend_sapnwrfc_globals *, v)
#else
#define SAPNWRFC_G(v) (sapnwrfc_globals.v)
#endif

#endif	/* PHP_SAPNWRFC_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
