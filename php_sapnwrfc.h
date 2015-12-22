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

/* $Id$ */

#ifndef PHP_SAPNWRFC_H
#define PHP_SAPNWRFC_H

extern zend_module_entry sapnwrfc_module_entry;
#define phpext_sapnwrfc_ptr &sapnwrfc_module_entry

#define PHP_SAPNWRFC_VERSION "1.0.0"

#ifdef PHP_WIN32
#define PHP_SAPNWRFC_API __declspec(dllexport)
#else
#define PHP_SAPNWRFC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(sapnwrfc);
PHP_MINFO_FUNCTION(sapnwrfc);

/* functions in sapnwrfc.c */
PHP_FUNCTION(sapnwrfc_version);
PHP_FUNCTION(sapnwrfc_version_array);
PHP_FUNCTION(sapnwrfc_rfcversion);
PHP_FUNCTION(sapnwrfc_setinipath);
PHP_FUNCTION(sapnwrfc_reloadinifile);
PHP_FUNCTION(sapnwrfc_removefunction);

#ifdef ZTS
#define SAPNWRFC_G(v) TSRMG(sapnwrfc_globals_id, zend_sapnwrfc_globals *, v)
#else
#define SAPNWRFC_G(v) (sapnwrfc_globals.v)
#endif

#endif	/* PHP_SAPNWRFC_H */
