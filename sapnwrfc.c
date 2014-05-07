/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2010 Piers Harding                                |
  +----------------------------------------------------------------------+
  | This package is released under the same terms as PHP itself.         |
  +----------------------------------------------------------------------+
  | Author: Piers Harding <piers@ompka.net>                               |
  +----------------------------------------------------------------------+
*/

//	DECL_EXP RFC_RC SAP_API RfcGetPartnerSNCName(RFC_CONNECTION_HANDLE rfcHandle, SAP_UC *sncName, unsigned length, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_RC SAP_API RfcGetPartnerSNCKey(RFC_CONNECTION_HANDLE rfcHandle, SAP_RAW *sncKey, unsigned *length, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_RC SAP_API RfcSNCNameToKey(SAP_UC const *sncLib, SAP_UC const *sncName, SAP_RAW *sncKey, unsigned *keyLength, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_RC SAP_API RfcSNCKeyToName(SAP_UC const *sncLib, SAP_RAW const *sncKey, unsigned keyLength ,SAP_UC *sncName, unsigned nameLength, RFC_ERROR_INFO* errorInfo);


//	DECL_EXP RFC_RC SAP_API RfcGetTransactionID(RFC_CONNECTION_HANDLE rfcHandle, RFC_TID tid, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_TRANSACTION_HANDLE SAP_API RfcCreateTransaction(RFC_CONNECTION_HANDLE rfcHandle, RFC_TID tid, SAP_UC const *queueName, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_RC SAP_API RfcInvokeInTransaction(RFC_TRANSACTION_HANDLE tHandle, RFC_FUNCTION_HANDLE funcHandle, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_RC SAP_API RfcSubmitTransaction(RFC_TRANSACTION_HANDLE tHandle, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_RC SAP_API RfcConfirmTransaction(RFC_TRANSACTION_HANDLE tHandle, RFC_ERROR_INFO* errorInfo);
//	DECL_EXP RFC_RC SAP_API RfcDestroyTransaction(RFC_TRANSACTION_HANDLE tHandle, RFC_ERROR_INFO* errorInfo);


//  DECL_EXP RFC_RC SAP_API RfcSetParameterActive(RFC_FUNCTION_HANDLE funcHandle, SAP_UC const* paramName, int isActive, RFC_ERROR_INFO* errorInfo);
//  DECL_EXP RFC_RC SAP_API RfcIsParameterActive(RFC_FUNCTION_HANDLE funcHandle, SAP_UC const* paramName, int *isActive, RFC_ERROR_INFO* errorInfo);
// add_property_bool( return_value, pszKey, bData );
//  void zend_update_property_bool(zend_class_entry *scope, zval *object, char *name, int name_length, long value TSRMLS_DC);
//  zval *zend_read_property(zend_class_entry *scope, zval *object, char *name, int name_length, zend_bool silent TSRMLS_DC);
// int zend_is_true(zval *op);

//	DECL_EXP RFC_RC SAP_API RfcEnableBASXML(RFC_FUNCTION_DESC_HANDLE funcDesc, RFC_ERROR_INFO* errorInfo);
//  DECL_EXP RFC_RC SAP_API RfcIsBASXMLSupported(RFC_FUNCTION_DESC_HANDLE funcDesc, int* isEnabled, RFC_ERROR_INFO* errorInfo);




#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "zend_compile.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include "fopen_wrappers.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/php_filestat.h"

#include "php_sapnwrfc.h"
#include "sapnwrfc.h"


SAP_UC * u8to16c(char * str);
SAP_UC * u8to16(zval *str);
zval* u16to8c(SAP_UC * str, int len);
zval* u16to8(SAP_UC * str);
static void SAPNW_rfc_conn_error(char *msg, int code, zval *key, zval *message);

static zval * get_field_value(DATA_CONTAINER_HANDLE hcont, RFC_FIELD_DESC fieldDesc);
void set_field_value(DATA_CONTAINER_HANDLE hcont, RFC_FIELD_DESC fieldDesc, zval * value);
static zval * get_table_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name);
void set_table_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value);


/* declare the class entry */
extern zend_class_entry *zend_ce_iterator;

#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_RuntimeException;
#endif

typedef enum { false, true } mybool;

mybool sapnwrfc_global_error;

/* forward declaration for all methods use (class-name, method-name) */
PHP_METHOD(sapnwrfc, __construct);
PHP_METHOD(sapnwrfc, connection_attributes);
PHP_METHOD(sapnwrfc, close);
PHP_METHOD(sapnwrfc, function_lookup);
PHP_METHOD(sapnwrfc, ping);
PHP_METHOD(sapnwrfc, get_sso_ticket);

PHP_METHOD(sapnwrfc_function, __construct);
PHP_METHOD(sapnwrfc_function, invoke);
PHP_METHOD(sapnwrfc_function, activate);
PHP_METHOD(sapnwrfc_function, deactivate);

/* declare method parameters */
/* supply a name and default to call by parameter */
#if (PHP_MAJOR_VERSION == 5 & PHP_MINOR_VERSION < 3)
static
#endif
ZEND_BEGIN_ARG_INFO(arginfo_sapnwrfc___construct, 0)
ZEND_ARG_INFO(0, connection_parameters)  /* parameter name */
ZEND_END_ARG_INFO();

/* the method table */
/* each method can have its own parameters and visibility */
static zend_function_entry sapnwrfc_class_functions[] = {
	PHP_ME(sapnwrfc, __construct, arginfo_sapnwrfc___construct, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc, connection_attributes, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc, close, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc, function_lookup, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc, ping, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc, get_sso_ticket, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry sapnwrfc_function_class_functions[] = {
	PHP_ME(sapnwrfc_function, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc_function, invoke, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc_function, activate, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(sapnwrfc_function, deactivate, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

static zend_function_entry sapnwrfc_connection_exception_class_functions[] = {
	{NULL, NULL, NULL}
};

static zend_function_entry sapnwrfc_call_exception_class_functions[] = {
	{NULL, NULL, NULL}
};


/* declare the class handlers */
static zend_object_handlers sapnwrfc_handlers;
static zend_object_handlers sapnwrfc_function_handlers;


/* declare the class entry */
static zend_class_entry *sapnwrfc_ce;
static zend_class_entry *sapnwrfc_function_ce;
static zend_class_entry *sapnwrfc_connection_exception_ce;
static zend_class_entry *sapnwrfc_call_exception_ce;


/* the overloaded class structure */

/* overloading the structure results in the need of having
   dedicated creatin/cloning/destruction functions */
typedef struct _sapnwrfc_connection_object {
	zend_object       std;
	RFC_CONNECTION_HANDLE handle;
	zval *connection_parameters;
} sapnwrfc_object;

typedef struct _sapnwrfc_function_description_object {
	zend_object       std;
	RFC_FUNCTION_DESC_HANDLE handle;
	RFC_CONNECTION_HANDLE conn_handle;
	zval *name;
} sapnwrfc_function_object;

typedef struct _sapnwrfc_connection_exception_class_object {
	zend_object       std;
} sapnwrfc_connection_exception_object;

typedef struct _sapnwrfc_call_exception_class_object {
	zend_object       std;
} sapnwrfc_call_exception_object;

#if PHP_MAJOR_VERSION >= 6 | (PHP_MAJOR_VERSION == 5 & PHP_MINOR_VERSION >= 2)
#define EXTSRM TSRMLS_C
#else
#define EXTSRM
#endif


SAP_UC * u8to16c(char * str) {
	// RFC_RC rc;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *sapuc;
	unsigned sapucSize, resultLength;

	sapucSize = strlen(str) + 1;
	sapuc = mallocU(sapucSize);
	memsetU(sapuc, 0, sapucSize);
	resultLength = 0;
	// rc = RfcUTF8ToSAPUC((RFC_BYTE *)str, strlen(str), sapuc, &sapucSize, &resultLength, &errorInfo);
	RfcUTF8ToSAPUC((RFC_BYTE *)str, strlen(str), sapuc, &sapucSize, &resultLength, &errorInfo);
	return sapuc;
}


SAP_UC * u8to16(zval *str) {
	// RFC_RC rc;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *sapuc;
	unsigned sapucSize, resultLength;

	sapucSize = Z_STRLEN_P(str) + 1;
	sapuc = mallocU(sapucSize);
	memsetU(sapuc, 0, sapucSize);
	resultLength = 0;
	// rc = RfcUTF8ToSAPUC((RFC_BYTE *)Z_STRVAL_P(str), Z_STRLEN_P(str), sapuc, &sapucSize, &resultLength, &errorInfo);
	RfcUTF8ToSAPUC((RFC_BYTE *)Z_STRVAL_P(str), Z_STRLEN_P(str), sapuc, &sapucSize, &resultLength, &errorInfo);
	return sapuc;
}


zval* u16to8(SAP_UC * str) {
	// RFC_RC rc;
	RFC_ERROR_INFO errorInfo;
	unsigned utf8Size, resultLength;
	char * utf8;
	zval * php_str;

	utf8Size = strlenU(str) * 4;
	utf8 = malloc(utf8Size + 2);
	memset(utf8, 0, utf8Size + 2);
	resultLength = 0;
	// rc = RfcSAPUCToUTF8(str, strlenU(str), (RFC_BYTE *)utf8, &utf8Size, &resultLength, &errorInfo);
	RfcSAPUCToUTF8(str, strlenU(str), (RFC_BYTE *)utf8, &utf8Size, &resultLength, &errorInfo);
	MAKE_STD_ZVAL( php_str );
	ZVAL_STRINGL( php_str, utf8, resultLength, TRUE );
	free(utf8);
	return php_str;
}


zval* u16to8c(SAP_UC * str, int len) {
	// RFC_RC rc;
	RFC_ERROR_INFO errorInfo;
	unsigned utf8Size, resultLength;
	char * utf8;
	zval * php_str;

	utf8Size = len * 4;
	utf8 = malloc(utf8Size + 2);
	memset(utf8, 0, utf8Size + 2);
	resultLength = 0;
	// rc = RfcSAPUCToUTF8(str, len, (RFC_BYTE *)utf8, &utf8Size, &resultLength, &errorInfo);
	RfcSAPUCToUTF8(str, len, (RFC_BYTE *)utf8, &utf8Size, &resultLength, &errorInfo);
	MAKE_STD_ZVAL( php_str );
	ZVAL_STRINGL( php_str, (char*)utf8, resultLength, TRUE );
	free(utf8);
	return php_str;
}


static void * make_space(int len){

    char * ptr;
    ptr = malloc( len + 2 );
    if ( ptr == NULL )
	    return NULL;
    memset(ptr, 0, len + 2);
    return ptr;
}

mybool my_is_empty(zval *value) {
	 if (Z_TYPE_P(value) == IS_NULL ||
	     (Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) == 0) ||
	     (Z_TYPE_P(value) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(value)) == 0)) {
		 return TRUE;
	 }
	 else {
		 return FALSE;
	 }
}


zval * make_long(int i) {

	zval * val;
	ALLOC_INIT_ZVAL(val);
	ZVAL_LONG(val, i);
	return val;
}


zval * make_double(double f) {

	zval * val;
	ALLOC_INIT_ZVAL(val);
	ZVAL_DOUBLE(val, f);
	return val;
}


zval * make_string(char *str) {

	zval * val;
	ALLOC_INIT_ZVAL(val);
	ZVAL_STRING(val, (char*)str, FALSE);
	return val;
}

zval * make_stringc(char *str) {

	zval * val;
	ALLOC_INIT_ZVAL(val);
	ZVAL_STRING(val, (char*)str, TRUE);
	return val;
}

zval * make_stringl(char *str, int len) {

	zval * val;
	ALLOC_INIT_ZVAL(val);
	ZVAL_STRINGL(val, (char*)str, len, FALSE);
	return val;
}

zval * make_stringcl(char *str, int len) {

	zval * val;
	ALLOC_INIT_ZVAL(val);
	ZVAL_STRINGL(val, (char*)str, len, TRUE);
	return val;
}


zval * make_empty() {

	zval * val;
	ALLOC_INIT_ZVAL(val);
	ZVAL_EMPTY_STRING(val);
	return val;
}


static void SAPNW_rfc_conn_error(char *msg, int code, zval *key, zval *message) {
    zval    *conn_ex;

    TSRMLS_FETCH();
    MAKE_STD_ZVAL(conn_ex);
    object_init_ex(conn_ex, sapnwrfc_connection_exception_ce);
	zend_update_property_string(sapnwrfc_connection_exception_ce, conn_ex, "message", sizeof("message") - 1, Z_STRVAL_P(message) TSRMLS_CC);
    zend_update_property_string(sapnwrfc_connection_exception_ce, conn_ex, "key", sizeof("key") - 1, Z_STRVAL_P(key) TSRMLS_CC);
    zend_update_property_long(sapnwrfc_connection_exception_ce, conn_ex, "code", sizeof("code") - 1, code TSRMLS_CC);
    zend_throw_exception_object(conn_ex TSRMLS_CC);
}

static char * my_concatc2c(char *part1, char *part2){
	char *ptr;

	ptr = make_space(strlen(part1) + strlen(part2));
	memcpy((char *)ptr, part1, strlen(part1));
	memcpy((char *)ptr+strlen(part1), part2, strlen(part2));
	return ptr;
}


static void SAPNW_rfc_call_error(char *msg, int code, zval *key, zval *message) {
    zval    *call_ex;

    TSRMLS_FETCH();
	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);
    MAKE_STD_ZVAL(call_ex);
    object_init_ex(call_ex, sapnwrfc_call_exception_ce);
	zend_update_property_string(sapnwrfc_call_exception_ce, call_ex, "message", sizeof("message") - 1, Z_STRVAL_P(message) TSRMLS_CC);
    zend_update_property_string(sapnwrfc_call_exception_ce, call_ex, "key", sizeof("key") - 1, Z_STRVAL_P(key) TSRMLS_CC);
    zend_update_property_long(sapnwrfc_call_exception_ce, call_ex, "code", sizeof("code") - 1, code TSRMLS_CC);
    zend_throw_exception_object(call_ex TSRMLS_CC);
	sapnwrfc_global_error = true;
	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
}


static void SAPNW_rfc_call_error1(char * msg, char * part1) {
    zval    *call_ex;

    TSRMLS_FETCH();
	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);
    MAKE_STD_ZVAL(call_ex);
    object_init_ex(call_ex, sapnwrfc_call_exception_ce);
	zend_update_property_string(sapnwrfc_call_exception_ce, call_ex, "message", sizeof("message") - 1, my_concatc2c(msg, part1) TSRMLS_CC);
    zend_update_property_string(sapnwrfc_call_exception_ce, call_ex, "key", sizeof("key") - 1, "" TSRMLS_CC);
    zend_update_property_long(sapnwrfc_call_exception_ce, call_ex, "code", sizeof("code") - 1, 0 TSRMLS_CC);
    zend_throw_exception_object(call_ex TSRMLS_CC);
	sapnwrfc_global_error = true;
	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
}


static zval * get_time_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_TIME timeBuff;
	zval * val = NULL;

	rc = RfcGetTime(hcont, name, timeBuff, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetTime: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	val = u16to8c(timeBuff, 6);
	return val;
}


static zval * get_date_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_DATE dateBuff;
	zval * val = NULL;

	rc = RfcGetDate(hcont, name, dateBuff, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetDate: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	val = u16to8c(dateBuff, 8);
	return val;
}


static zval * get_int_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_INT rfc_int;
	zval * val = NULL;

	rc = RfcGetInt(hcont, name, &rfc_int, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetInt: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	return make_long(rfc_int);
}


static zval * get_int1_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_INT1 rfc_int1;
	zval * val = NULL;

	rc = RfcGetInt1(hcont, name, &rfc_int1, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetInt1: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	return make_long(( int ) rfc_int1);
}


static zval * get_int2_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_INT2 rfc_int2;
	zval * val = NULL;

	rc = RfcGetInt2(hcont, name, &rfc_int2, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetInt2: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	return make_long(( int ) rfc_int2);
}


static zval * get_float_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_FLOAT rfc_float;
	zval * val = NULL;

	rc = RfcGetFloat(hcont, name, &rfc_float, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetFloat: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	return make_double(( double ) rfc_float);
}


static zval * get_string_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	zval * val= NULL;
	unsigned strLen, retStrLen;
	char * buffer;

	rc = RfcGetStringLength(hcont, name, &strLen, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetStringLength: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	/* bail out if string is empty */
	if (strLen == 0)
		return make_empty();

	buffer = make_space(strLen*4);
	rc = RfcGetString(hcont, name, (SAP_UC *)buffer, strLen + 2, &retStrLen, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetString: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	val = u16to8c((SAP_UC *)buffer, retStrLen);
	free(buffer);
	return val;
}


static zval * get_xstring_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	zval * val= NULL;
	unsigned strLen, retStrLen;
	char * buffer;

	rc = RfcGetStringLength(hcont, name, &strLen, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetXStringLength in XSTRING: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	/* bail out if string is empty */
	if (strLen == 0)
		return make_empty();

	buffer = make_space(strLen);
	rc = RfcGetXString(hcont, name, (SAP_RAW *)buffer, strLen, &retStrLen, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetXString: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
}

	val = make_stringcl(buffer, strLen);
	free(buffer);
	return val;
}



static zval * get_num_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, unsigned ulen){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	char * buffer;
	zval * val = NULL;

	buffer = make_space(ulen*2); /* seems that you need 2 null bytes to terminate a string ...*/
	rc = RfcGetNum(hcont, name, (RFC_NUM *)buffer, ulen, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetNum: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	val = u16to8((SAP_UC *)buffer);
	free(buffer);

	return val;
}


static zval * get_bcd_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	zval * val = NULL;
	unsigned strLen, retStrLen;
	char * buffer;

	/* select a random long length for a BCD */
	strLen = 100;

	buffer = make_space(strLen*2);
	rc = RfcGetString(hcont, name, (SAP_UC *)buffer, strLen, &retStrLen, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetString in NUMC: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	val = u16to8c((SAP_UC *)buffer, retStrLen);
	free(buffer);
	// XXX need to convert this to a float
	return val;
}


static zval * get_char_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, unsigned ulen){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	char * buffer;
	zval * val = NULL;

	buffer = make_space(ulen*4); /* seems that you need 2 null bytes to terminate a string ...*/

	rc = RfcGetChars(hcont, name, (RFC_CHAR *)buffer, ulen, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetChars: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	val = u16to8((SAP_UC *)buffer);
	free(buffer);

	return val;
}


static zval * get_byte_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, unsigned len){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	char * buffer;
	zval * val = NULL;

	buffer = make_space(len);
	rc = RfcGetBytes(hcont, name, (SAP_RAW *)buffer, len, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetBytes: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	val = make_stringcl(buffer, len);
	free(buffer);

	return val;
}


static zval * get_structure_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_STRUCTURE_HANDLE line;
	RFC_TYPE_DESC_HANDLE typeHandle;
	RFC_FIELD_DESC fieldDesc;
	unsigned fieldCount, i;
	zval * val= NULL;

	rc = RfcGetStructure(hcont, name, &line, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetStructure: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	typeHandle = RfcDescribeType(line, &errorInfo);
	if (typeHandle == NULL) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcDescribeType: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	rc = RfcGetFieldCount(typeHandle, &fieldCount, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetFieldCount: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	ALLOC_INIT_ZVAL(val);
	array_init(val);
	for (i = 0; i < fieldCount; i++) {
		rc = RfcGetFieldDescByIndex(typeHandle, i, &fieldDesc, &errorInfo);
		if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetFieldDescByIndex: ",
										Z_STRVAL_P(u16to8(name))),
										errorInfo.code,
										u16to8(errorInfo.key),
										u16to8(errorInfo.message));
			ZVAL_NULL(val);
			return val;
		}

		/* process each field type ...*/
		add_assoc_zval(val, Z_STRVAL_P(u16to8(fieldDesc.name)), get_field_value(line, fieldDesc));
	}

	return val;
}


static zval * get_field_value(DATA_CONTAINER_HANDLE hcont, RFC_FIELD_DESC fieldDesc){

	zval * pvalue;
	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_TABLE_HANDLE tableHandle;

	pvalue = NULL;
	switch (fieldDesc.type) {
	case RFCTYPE_DATE:
		  pvalue = get_date_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_TIME:
		  pvalue = get_time_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_NUM:
		  pvalue = get_num_value(hcont, fieldDesc.name, fieldDesc.nucLength);
		  break;
	case RFCTYPE_BCD:
		  pvalue = get_bcd_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_CHAR:
		  pvalue = get_char_value(hcont, fieldDesc.name, fieldDesc.nucLength);
		  break;
	case RFCTYPE_BYTE:
		  pvalue = get_byte_value(hcont, fieldDesc.name, fieldDesc.nucLength);
		  break;
	case RFCTYPE_FLOAT:
		  pvalue = get_float_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_INT:
		  pvalue = get_int_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_INT2:
		  pvalue = get_int2_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_INT1:
		  pvalue = get_int1_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_STRUCTURE:
		  pvalue = get_structure_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_TABLE:
		  rc = RfcGetTable(hcont, fieldDesc.name, &tableHandle, &errorInfo);
		  if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetTable: ",
										Z_STRVAL_P(u16to8(fieldDesc.name))),
										errorInfo.code,
										u16to8(errorInfo.key),
										u16to8(errorInfo.message));
			ZVAL_NULL(pvalue);
			return pvalue;
		  }
		  pvalue = get_table_value(tableHandle, fieldDesc.name);
		  break;
	case RFCTYPE_XMLDATA:
		  fprintf(stderr, "shouldnt get a XMLDATA type parameter - abort\n");
			exit(1);
		  break;
	case RFCTYPE_STRING:
		  pvalue = get_string_value(hcont, fieldDesc.name);
		  break;
	case RFCTYPE_XSTRING:
		  pvalue = get_xstring_value(hcont, fieldDesc.name);
		  break;
		default:
		  fprintf(stderr, "This type is not implemented (%d) - abort\n", fieldDesc.type);
			exit(1);
		  break;
	}

	return pvalue;
}


static zval * get_table_line(RFC_STRUCTURE_HANDLE line){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_TYPE_DESC_HANDLE typeHandle;
	RFC_FIELD_DESC fieldDesc;
	unsigned fieldCount, i;
	zval * val = NULL;

	typeHandle = RfcDescribeType(line, &errorInfo);
	if (typeHandle == NULL) {
		SAPNW_rfc_call_error("Problem with RfcDescribeType ",
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	rc = RfcGetFieldCount(typeHandle, &fieldCount, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error("Problem with RfcGetFieldCount ",
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}

	ALLOC_INIT_ZVAL(val);
	array_init(val);
	for (i = 0; i < fieldCount; i++) {
		rc = RfcGetFieldDescByIndex(typeHandle, i, &fieldDesc, &errorInfo);
		if (rc != RFC_OK) {
			SAPNW_rfc_call_error("Problem with RfcGetFieldDescByIndex ",
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
			ZVAL_NULL(val);
			return val;
		}

		/* process each field type ...*/
		add_assoc_zval(val, Z_STRVAL_P(u16to8(fieldDesc.name)), get_field_value(line, fieldDesc));
	}

	return val;
}


static zval * get_table_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	zval * val = NULL;
	unsigned tabLen, r;
	RFC_STRUCTURE_HANDLE line;

	rc = RfcGetRowCount(hcont, &tabLen, NULL);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetRowCount: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		ZVAL_NULL(val);
		return val;
	}
	ALLOC_INIT_ZVAL(val);
	array_init(val);
	for (r = 0; r < tabLen; r++){
		RfcMoveTo(hcont, r, NULL);
		line = RfcGetCurrentRow(hcont, NULL);
		add_next_index_zval(val, get_table_line(line));
		if (sapnwrfc_global_error) {
			ZVAL_NULL(val);
			return val;
		}
	}

	return val;
}


static zval * get_parameter_value(zval * name, RFC_FUNCTION_HANDLE funcHandle, RFC_FUNCTION_DESC_HANDLE funcDescHandle){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_TABLE_HANDLE tableHandle;
	RFC_PARAMETER_DESC paramDesc;
	SAP_UC *p_name;
	zval * pvalue = NULL;

	/* get the parameter description */
	rc = RfcGetParameterDescByName(funcDescHandle, (p_name = u8to16(name)), &paramDesc, &errorInfo);

	/* bail on a bad call for parameter description */
	if (rc != RFC_OK) {
		free(p_name);
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetParameterDescByName: ",
								Z_STRVAL_P(name)),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		ZVAL_NULL(pvalue);
		return pvalue;
	}

	pvalue = NULL;
	switch (paramDesc.type) {
		case RFCTYPE_DATE:
			pvalue = get_date_value(funcHandle, p_name);
			break;
		case RFCTYPE_TIME:
			pvalue = get_time_value(funcHandle, p_name);
			break;
		case RFCTYPE_NUM:
			pvalue = get_num_value(funcHandle, p_name, paramDesc.nucLength);
			break;
		case RFCTYPE_BCD:
			pvalue = get_bcd_value(funcHandle, p_name);
			break;
		case RFCTYPE_CHAR:
			pvalue = get_char_value(funcHandle, p_name, paramDesc.nucLength);
			break;
		case RFCTYPE_BYTE:
			pvalue = get_byte_value(funcHandle, p_name, paramDesc.nucLength);
			break;
		case RFCTYPE_FLOAT:
			pvalue = get_float_value(funcHandle, p_name);
			break;
		case RFCTYPE_INT:
			pvalue = get_int_value(funcHandle, p_name);
			break;
		case RFCTYPE_INT2:
			pvalue = get_int2_value(funcHandle, p_name);
			break;
		case RFCTYPE_INT1:
			pvalue = get_int1_value(funcHandle, p_name);
			break;
		case RFCTYPE_STRUCTURE:
			pvalue = get_structure_value(funcHandle, p_name);
			break;
		case RFCTYPE_TABLE:
			rc = RfcGetTable(funcHandle, p_name, &tableHandle, &errorInfo);
			if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetTable: ",
									Z_STRVAL_P(name)),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
				ZVAL_NULL(pvalue);
				return pvalue;
			}
			pvalue = get_table_value(tableHandle, p_name);
			break;
		case RFCTYPE_XMLDATA:
			fprintf(stderr, "shouldnt get a XMLDATA type parameter - abort\n");
			exit(1);
			break;
		case RFCTYPE_STRING:
			pvalue = get_string_value(funcHandle, p_name);
			break;
		case RFCTYPE_XSTRING:
			pvalue = get_xstring_value(funcHandle, p_name);
			break;
		default:
			fprintf(stderr, "This type is not implemented (%d) - abort\n", paramDesc.type);
			exit(1);
			break;
	}
	free(p_name);
	return pvalue;
}


void set_date_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *p_value;
	RFC_DATE date_value;

	if (Z_TYPE_P(value) != IS_STRING){
		SAPNW_rfc_call_error1("RfcSetDate invalid Input value type:", Z_STRVAL_P(u16to8(name)));
		return;
	}
	if (Z_STRLEN_P(value) != 8) {
		SAPNW_rfc_call_error1("RfcSetDate invalid date format:", Z_STRVAL_P(value));
		return;
	}
	p_value = u8to16(value);
	memcpy((char *)date_value+0, (char *)p_value, 16);
	free(p_value);

	rc = RfcSetDate(hcont, name, date_value, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetDate: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
	}
	return;
}


void set_time_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *p_value;
	RFC_TIME time_value;

	if (Z_TYPE_P(value) != IS_STRING){
		SAPNW_rfc_call_error1("RfcSetTime invalid Input value type:", Z_STRVAL_P(u16to8(name)));
		return;
	}
	if (Z_STRLEN_P(value) != 6) {
		SAPNW_rfc_call_error1("RfcSetTime invalid date format:", Z_STRVAL_P(value));
		return;
	}
	p_value = u8to16(value);
	memcpy((char *)time_value+0, (char *)p_value, 12);
	free(p_value);

	rc = RfcSetTime(hcont, name, time_value, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetTime: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
	}
	return;
}


void set_num_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value, unsigned max){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *p_value;

	if (Z_TYPE_P(value) != IS_STRING){
		SAPNW_rfc_call_error1("RfcSetNum invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	if (Z_STRLEN_P(value) > max) {
		SAPNW_rfc_call_error1("RfcSetNum string too long: ", Z_STRVAL_P(value));
		return;
	}

	p_value = u8to16(value);
	rc = RfcSetNum(hcont, name, (RFC_NUM *)p_value, strlenU(p_value), &errorInfo);
	free(p_value);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetNum: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_bcd_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *p_value;

	/* make sure that the BCD source value is a string */
	if (Z_TYPE_P(value) != IS_STRING){
		SAPNW_rfc_call_error1("(bcd)RfcSetString invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}

	p_value = u8to16(value);
	rc = RfcSetString(hcont, name, p_value, strlenU(p_value), &errorInfo);
	free(p_value);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetBCD: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_char_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value, unsigned max){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *p_value;

	if (Z_TYPE_P(value) != IS_STRING){
    	SAPNW_rfc_call_error1("RfcSetChar invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
//	if (Z_STRLEN_P(value) > max) {
//		SAPNW_rfc_call_error1("RfcSetChar string too long: ", Z_STRVAL_P(u16to8(name)));
//		return;
//	}

	p_value = u8to16(value);
	rc = RfcSetChars(hcont, name, p_value, strlenU(p_value), &errorInfo);
	free(p_value);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetChars: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_byte_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value, unsigned max){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;

	if (Z_TYPE_P(value) != IS_STRING){
		SAPNW_rfc_call_error1("RfcSetByte invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	if (Z_STRLEN_P(value) > max) {
		SAPNW_rfc_call_error1("RfcSetByte string too long:", Z_STRVAL_P(value));
		return;
	}
	rc = RfcSetBytes(hcont, name, (SAP_RAW *)Z_STRVAL_P(value), Z_STRLEN_P(value), &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetBytes: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_float_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;

	if (Z_TYPE_P(value) != IS_DOUBLE){
		SAPNW_rfc_call_error1("RfcSetFloat invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	rc = RfcSetFloat(hcont, name, (RFC_FLOAT) Z_DVAL_P(value), &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetFloat: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_int_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;

	if (Z_TYPE_P(value) != IS_LONG){
		SAPNW_rfc_call_error1("RfcSetInt invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	rc = RfcSetInt(hcont, name, (RFC_INT) Z_LVAL_P(value), &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetInt: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}

	return;
}


void set_int1_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;

	if (Z_TYPE_P(value) != IS_LONG){
		SAPNW_rfc_call_error1("RfcSetInt1 invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	if (Z_LVAL_P(value) > 255){
		SAPNW_rfc_call_error1("RfcSetInt1 invalid Input value too big on: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	rc = RfcSetInt1(hcont, name, (RFC_INT1) Z_LVAL_P(value), &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetInt1: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_int2_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;

	if (Z_TYPE_P(value) != IS_LONG){
		SAPNW_rfc_call_error1("RfcSetInt2 invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	if (Z_LVAL_P(value) > 4095){
		SAPNW_rfc_call_error1("RfcSetInt2 invalid Input value too big on: ", Z_STRVAL_P(u16to8(name)));
		return;
	}
	rc = RfcSetInt2(hcont, name, (RFC_INT2) Z_LVAL_P(value), &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetInt2: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_string_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	SAP_UC *p_value;

	if (Z_TYPE_P(value) != IS_STRING){
		SAPNW_rfc_call_error1("RfcSetString invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}

	p_value = u8to16(value);
	rc = RfcSetString(hcont, name, p_value, strlenU(p_value), &errorInfo);
	free(p_value);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetString: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_xstring_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;

	if (Z_TYPE_P(value) != IS_STRING){
		SAPNW_rfc_call_error1("RfcSetXString invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}

	rc = RfcSetXString(hcont, name, (SAP_RAW *)Z_STRVAL_P(value), Z_STRLEN_P(value), &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcSetXString: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}
	return;
}


void set_structure_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_STRUCTURE_HANDLE line;
	RFC_TYPE_DESC_HANDLE typeHandle;
	RFC_FIELD_DESC fieldDesc;
	SAP_UC *p_name;
	int i;
	zval **data;
	HashTable *arr_hash;
	HashPosition pointer;
	char *str_index;
	ulong num_index;
	uint str_length;

	if (Z_TYPE_P(value) != IS_ARRAY){
		SAPNW_rfc_call_error1("RfcSetStructure invalid Input value type: ", Z_STRVAL_P(u16to8(name)));
		return;
	}

	arr_hash = Z_ARRVAL_P(value);
	// idx = zend_hash_num_elements(arr_hash);

	rc = RfcGetStructure(hcont, name, &line, &errorInfo);
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetStructure: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}

	typeHandle = RfcDescribeType(line, &errorInfo);
	if (typeHandle == NULL) {
		SAPNW_rfc_call_error(my_concatc2c("Problem with RfcDescribeType: ",
								Z_STRVAL_P(u16to8(name))),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}

	i = 0;
	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {
		if (zend_hash_get_current_key_ex (arr_hash, &str_index, &str_length, &num_index, 0, &pointer) != HASH_KEY_IS_STRING) {
			zend_error(E_ERROR, "RfcSetStructure all structure keys must be strings");
			return;
		}
		  rc = RfcGetFieldDescByName(typeHandle, (p_name = u8to16c(str_index)), &fieldDesc, &errorInfo);
		if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetFieldDescByName: ",
									str_index),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
			return;
		}
		// XXX bad copy
		memcpy(fieldDesc.name, p_name, strlenU(p_name)*2+2);
		free(p_name);
		set_field_value(line, fieldDesc, *data);
		if (sapnwrfc_global_error) {
			return;
		}
		i++;
	}
	return;
}


void set_field_value(DATA_CONTAINER_HANDLE hcont, RFC_FIELD_DESC fieldDesc, zval * value){
	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_TABLE_HANDLE tableHandle;

	switch (fieldDesc.type) {
		case RFCTYPE_DATE:
			  set_date_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_TIME:
			  set_time_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_NUM:
			  set_num_value(hcont, fieldDesc.name, value, fieldDesc.nucLength);
			  break;
		case RFCTYPE_BCD:
			  set_bcd_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_CHAR:
			  set_char_value(hcont, fieldDesc.name, value, fieldDesc.nucLength);
			  break;
		case RFCTYPE_BYTE:
			  set_byte_value(hcont, fieldDesc.name, value, fieldDesc.nucLength);
			  break;
		case RFCTYPE_FLOAT:
			  set_float_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_INT:
			  set_int_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_INT2:
			  set_int2_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_INT1:
			  set_int1_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_STRUCTURE:
			  set_structure_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_TABLE:
		  rc = RfcGetTable(hcont, fieldDesc.name, &tableHandle, &errorInfo);
		  if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("(set_table)Problem with RfcGetTable: ",
									Z_STRVAL_P(u16to8(fieldDesc.name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
			return;
			}
			  set_table_value(tableHandle, fieldDesc.name, value);
			  break;
		case RFCTYPE_XMLDATA:
			  fprintf(stderr, "shouldnt get a XMLDATA type parameter - abort\n");
				exit(1);
			  break;
		case RFCTYPE_STRING:
			  set_string_value(hcont, fieldDesc.name, value);
			  break;
		case RFCTYPE_XSTRING:
			  set_xstring_value(hcont, fieldDesc.name, value);
			  break;
			default:
			  fprintf(stderr, "Set field - This type is not implemented (%d) - abort\n", fieldDesc.type);
				exit(1);
			  break;
	}

	return;
}


void set_table_line(RFC_STRUCTURE_HANDLE line, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_TYPE_DESC_HANDLE typeHandle;
	RFC_FIELD_DESC fieldDesc;
	SAP_UC * p_name;
	int i;
	zval **data;
	HashTable *arr_hash;
	HashPosition pointer;
	char *str_index;
	ulong num_index;
	uint str_length;


	if (Z_TYPE_P(value) != IS_ARRAY){
		SAPNW_rfc_call_error1("set_table_line invalid Input value type", "");
		return;
	}
	arr_hash = Z_ARRVAL_P(value);
	// idx = zend_hash_num_elements(arr_hash);

	typeHandle = RfcDescribeType(line, &errorInfo);
	if (typeHandle == NULL) {
		SAPNW_rfc_call_error("(set_table_line)Problem with RfcDescribeType",
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}

	i = 0;
	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {
		if (zend_hash_get_current_key_ex (arr_hash, &str_index, &str_length, &num_index, 0, &pointer) != HASH_KEY_IS_STRING) {
			zend_error(E_ERROR, "RfcSetStructure all structure keys must be strings");
		}
		  rc = RfcGetFieldDescByName(typeHandle, (p_name = u8to16c(str_index)), &fieldDesc, &errorInfo);
		if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("(set_table_line)Problem with RfcGetFieldDescByName: ",
									Z_STRVAL_P(u16to8(fieldDesc.name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
			return;
		}
		// XXX bad copy
		memcpy(fieldDesc.name, p_name, strlenU(p_name)*2+2);
		free(p_name);
		set_field_value(line, fieldDesc, *data);
		if (sapnwrfc_global_error) {
			return;
		}
		i++;
	}
	return;
}


void set_table_value(DATA_CONTAINER_HANDLE hcont, SAP_UC *name, zval * value){

	RFC_ERROR_INFO errorInfo;
	RFC_STRUCTURE_HANDLE line;
	zval **data;
	HashTable *arr_hash;
	HashPosition pointer;

	if (Z_TYPE_P(value) != IS_ARRAY){
		SAPNW_rfc_call_error1("set_table invalid Input value type:", Z_STRVAL_P(u16to8(name)));
		return;
	}
	arr_hash = Z_ARRVAL_P(value);

	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {
		line = RfcAppendNewRow(hcont, &errorInfo);
		if (line == NULL) {
			SAPNW_rfc_call_error(my_concatc2c("(set_table)Problem with RfcAppendNewRow: ",
									Z_STRVAL_P(u16to8(name))),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
			return;
		}
		set_table_line(line, *data);
		if (sapnwrfc_global_error) {
			return;
		}
	}
	return;
}


void set_parameter_value(RFC_FUNCTION_HANDLE funcHandle, RFC_FUNCTION_DESC_HANDLE funcDescHandle, char * name, zval * value){

	RFC_RC rc = RFC_OK;
	RFC_ERROR_INFO errorInfo;
	RFC_TABLE_HANDLE tableHandle;
	RFC_PARAMETER_DESC paramDesc;
	SAP_UC *p_name;

	if (my_is_empty(value)) {
	  return;
	}

	/* get the parameter description */
	rc = RfcGetParameterDescByName(funcDescHandle, (p_name = u8to16c(name)), &paramDesc, &errorInfo);

	/* bail on a bad call for parameter description */
	if (rc != RFC_OK) {
	  free(p_name);
		SAPNW_rfc_call_error(my_concatc2c("(set)Problem with RfcGetParameterDescByName: ",
								name),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		return;
	}

	switch (paramDesc.type) {
		case RFCTYPE_DATE:
			  set_date_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_TIME:
			  set_time_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_NUM:
			  set_num_value(funcHandle, p_name, value, paramDesc.nucLength);
			  break;
		case RFCTYPE_BCD:
			  set_bcd_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_CHAR:
			  set_char_value(funcHandle, p_name, value, paramDesc.nucLength);
			  break;
		case RFCTYPE_BYTE:
			  set_byte_value(funcHandle, p_name, value, paramDesc.nucLength);
			  break;
		case RFCTYPE_FLOAT:
			  set_float_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_INT:
			  set_int_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_INT2:
			  set_int2_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_INT1:
			  set_int1_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_STRUCTURE:
			  set_structure_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_TABLE:
			  rc = RfcGetTable(funcHandle, p_name, &tableHandle, &errorInfo);
			  if (rc != RFC_OK) {
					SAPNW_rfc_call_error(my_concatc2c("(set_table)Problem with RfcGetTable: ",
											name),
											errorInfo.code,
											u16to8(errorInfo.key),
											u16to8(errorInfo.message));
					return;
				}
			  set_table_value(tableHandle, p_name, value);
			  break;
		case RFCTYPE_XMLDATA:
			  fprintf(stderr, "shouldnt get a XMLDATA type parameter - abort\n");
				exit(1);
			  break;
		case RFCTYPE_STRING:
			  set_string_value(funcHandle, p_name, value);
			  break;
		case RFCTYPE_XSTRING:
			  set_xstring_value(funcHandle, p_name, value);
			  break;
			default:
			  fprintf(stderr, "This type is not implemented (%d) - abort\n", paramDesc.type);
				exit(1);
			  break;
	}
	free(p_name);

	return;
}


/* {{{ sapnwrfc_function_object_free_storage */
/* close all resources and the memory allocated for the object */
static void sapnwrfc_function_object_free_storage(void *object TSRMLS_DC) {
	RFC_ERROR_INFO errorInfo;
	RFC_RC rc = RFC_OK;

	sapnwrfc_function_object *intern = (sapnwrfc_function_object *)object;

	if (intern->handle != NULL) {
		//  fprintf(stderr, "func_desc_handle_free: -> start %p\n", ptr);
		rc = RfcDestroyFunctionDesc(intern->handle, &errorInfo);
		intern->handle = NULL;
		if (rc != RFC_OK) {
//		  SAPNW_rfc_conn_error("Problem destroying RFC description handle",
//								errorInfo.code,
//								u16to8(errorInfo.key),
//								u16to8(errorInfo.message));
		//		  fprintfU(stderr, cU("RFC ERR %s: %s\n"), errorInfo.key, errorInfo.message);
		}
		intern->handle = NULL;
		intern->conn_handle = NULL;
	}

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	efree(object);
}
/* }}} */


/* {{{ sapnwrfc_function_object_new */
/* creates the object by
   - allocating memory
   - initializing the object members
   - storing the object
   - setting it's handlers

   called from
   - clone
   - new
 */
static zend_object_value sapnwrfc_function_object_new_ex(zend_class_entry *class_type, sapnwrfc_function_object **obj TSRMLS_DC) {
	zend_object_value retval;
	sapnwrfc_function_object *intern;
	zval *tmp = NULL;

	intern = emalloc(sizeof(sapnwrfc_function_object));
	memset(intern, 0, sizeof(sapnwrfc_function_object));
	intern->std.ce = class_type;
	*obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	#if PHP_VERSION_ID < 50399
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
    #else
    object_properties_init(&(intern->std), class_type);
    #endif

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) sapnwrfc_function_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &sapnwrfc_function_handlers;
	return retval;
}
/* }}} */


/* {{{ sapnwrfc_function_object_new */
/* See sapnwrfc_function_object_new_ex */
static zend_object_value sapnwrfc_function_object_new(zend_class_entry *class_type TSRMLS_DC) {
	sapnwrfc_function_object *tmp;
	return sapnwrfc_function_object_new_ex(class_type, &tmp TSRMLS_CC);
}
/* }}} */




/* {{{ sapnwrfc_object_clone */
/* Local zend_object_value creation (on stack)
   Load the 'other' object
   Create a new empty object (See sapnwrfc_object_new_ex)
   Open the directory
   Clone other members (properties)
 */
static zend_object_value sapnwrfc_function_object_clone(zval *zobject TSRMLS_DC) {
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	sapnwrfc_function_object *intern;

	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = sapnwrfc_function_object_new_ex(old_object->ce, &intern TSRMLS_CC);
	new_object = &intern->std;
	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);
	return new_obj_val;
}
/* }}} */



/* {{{ sapnwrfc_function_get_ce */
#if MBO_0
static zend_class_entry *sapnwrfc_function_get_ce(zval *object TSRMLS_DC)
{
	return sapnwrfc_function_ce;
}
#endif
/* }}} */


/* {{{ sapnwrfc_object_free_storage */
/* close all resources and the memory allocated for the object */
static void sapnwrfc_object_free_storage(void *object TSRMLS_DC) {
	RFC_ERROR_INFO errorInfo;
	RFC_RC rc = RFC_OK;

	sapnwrfc_object *intern = (sapnwrfc_object *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	if (intern->handle != NULL) {
		rc = RfcCloseConnection(intern->handle, &errorInfo);
		intern->handle = NULL;
		if (rc != RFC_OK) {
		  SAPNW_rfc_conn_error("Problem closing RFC connection handle",
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		}
	}
	efree(object);
}
/* }}} */


/* {{{ sapnwrfc_object_new */
/* creates the object by
   - allocating memory
   - initializing the object members
   - storing the object
   - setting it's handlers

   called from
   - clone
   - new
 */
static zend_object_value sapnwrfc_object_new_ex(zend_class_entry *class_type, sapnwrfc_object **obj TSRMLS_DC) {
	zend_object_value retval;
	sapnwrfc_object *intern;
	zval *tmp = NULL;

	intern = emalloc(sizeof(sapnwrfc_object));
	memset(intern, 0, sizeof(sapnwrfc_object));
	intern->std.ce = class_type;
	*obj = intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	#if PHP_VERSION_ID < 50399
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
    #else
    object_properties_init(&(intern->std), class_type);
    #endif

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) sapnwrfc_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &sapnwrfc_handlers;
	return retval;
}
/* }}} */


/* {{{ sapnwrfc_object_new */
/* See sapnwrfc_object_new_ex */
static zend_object_value sapnwrfc_object_new(zend_class_entry *class_type TSRMLS_DC) {
	sapnwrfc_object *tmp;
	return sapnwrfc_object_new_ex(class_type, &tmp TSRMLS_CC);
}
/* }}} */


/* {{{ sapnwrfc_open */
/* open a directory resource */
static void sapnwrfc_open(sapnwrfc_object* intern, zval *connection_parameters TSRMLS_DC) {
	RFC_CONNECTION_PARAMETER * loginParams;
	RFC_ERROR_INFO errorInfo;
	int idx, i;
    zval **data;
    HashTable *arr_hash;
    HashPosition pointer;
    char *str_index;
    ulong num_index;
    uint str_length;

    arr_hash = Z_ARRVAL_P(connection_parameters);
    idx = zend_hash_num_elements(arr_hash);

	if (idx == 0) {
		zend_error(E_ERROR, "Method sapnwrfc::__constructor() no connection parameters supplied");
	}

	loginParams = malloc(idx*sizeof(RFC_CONNECTION_PARAMETER));
	memset(loginParams, 0,idx*sizeof(RFC_CONNECTION_PARAMETER));

	i = 0;
	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {

		if (zend_hash_get_current_key_ex (arr_hash, &str_index, &str_length, &num_index, 0, &pointer) != HASH_KEY_IS_STRING) {
			zend_error(E_ERROR, "Method sapnwrfc::__constructor() all connection parameter keys must be strings");
		}

		convert_to_string_ex(data);
		loginParams[i].name = (SAP_UC *) u8to16c(str_index);
		loginParams[i].value = (SAP_UC *) u8to16(*data);
//	            PHPWRITE(Z_STRVAL_PP(data), Z_STRLEN_PP(data));
//	            php_printf(" ");
		 i++;
	}
	intern->handle = RfcOpenConnection(loginParams, idx, &errorInfo);

	if (intern->handle == NULL) {
		for (i = 0; i < idx; i++) {
		   free((char *) loginParams[i].name);
		   free((char *) loginParams[i].value);
		}
		free(loginParams);
		SAPNW_rfc_conn_error("RFC connection open failed ",
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
	}
	else {
		intern->connection_parameters = connection_parameters;
	}
}
/* }}} */



/* {{{ sapnwrfc_object_clone */
/* Local zend_object_value creation (on stack)
   Load the 'other' object
   Create a new empty object (See sapnwrfc_object_new_ex)
   Open the directory
   Clone other members (properties)
 */
static zend_object_value sapnwrfc_object_clone(zval *zobject TSRMLS_DC) {
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	sapnwrfc_object *intern;

	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = sapnwrfc_object_new_ex(old_object->ce, &intern TSRMLS_CC);
	new_object = &intern->std;

	sapnwrfc_open(intern, ((sapnwrfc_object*)old_object)->connection_parameters TSRMLS_CC);

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}
/* }}} */

/* {{{ sapnwrfc_get_ce */
#if MBO_0
static zend_class_entry *sapnwrfc_get_ce(zval *object TSRMLS_DC)
{
	return sapnwrfc_ce;
}
#endif
/* }}} */


/* {{{ proto void sapnwrfc::__construct(string path)
 Cronstructs a new RFC connection object. */
/* zend_replace_error_handling() is used to throw exceptions in case
   the constructor fails. Here we use this to ensure the object
   has a valid directory resource.

   When the constructor gets called the object is already created
   by the engine, so we must only call 'additional' initializations.
 */
PHP_METHOD(sapnwrfc, __construct) {
	zval *object = getThis();
	sapnwrfc_object *intern;
	zval *connection_parameters;
	long len;

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &connection_parameters, &len) == FAILURE) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	// must be an array
	if (Z_TYPE_P(connection_parameters) != IS_ARRAY) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	intern = (sapnwrfc_object*)zend_object_store_get_object(object TSRMLS_CC);
	sapnwrfc_open(intern, connection_parameters TSRMLS_CC);

	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
}
/* }}} */


/* {{{ proto string sapnwrfc::connection_attributes()
   Return connection attributes of the current connection */
PHP_METHOD(sapnwrfc, connection_attributes) {
	zval *object = getThis();
	sapnwrfc_object *intern = (sapnwrfc_object*)zend_object_store_get_object(object TSRMLS_CC);

	RFC_ATTRIBUTES attribs;
	RFC_ERROR_INFO errorInfo;
	RFC_RC rc = RFC_OK;

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	rc = RfcGetConnectionAttributes(intern->handle, &attribs, &errorInfo);
	/* bail on a bad return code */
	if (rc != RFC_OK) {
		SAPNW_rfc_conn_error("getting connection attributes ",
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
		RETURN_NULL();
	}

	/* else return a hash of connection attributes */
	array_init(return_value);
	add_assoc_zval(return_value, "dest", u16to8(attribs.dest));
	add_assoc_zval(return_value, "host", u16to8(attribs.host));
	add_assoc_zval(return_value, "partnerHost", u16to8(attribs.partnerHost));
	add_assoc_zval(return_value, "sysNumber", u16to8(attribs.sysNumber));
	add_assoc_zval(return_value, "sysId", u16to8(attribs.sysId));
	add_assoc_zval(return_value, "client", u16to8(attribs.client));
	add_assoc_zval(return_value, "user", u16to8(attribs.user));
	add_assoc_zval(return_value, "language", u16to8(attribs.language));
	add_assoc_zval(return_value, "trace", u16to8(attribs.trace));
	add_assoc_zval(return_value, "isoLanguage", u16to8(attribs.isoLanguage));
	add_assoc_zval(return_value, "codepage", u16to8(attribs.codepage));
	add_assoc_zval(return_value, "partnerCodepage", u16to8(attribs.partnerCodepage));
	add_assoc_zval(return_value, "rfcRole", u16to8(attribs.rfcRole));
	add_assoc_zval(return_value, "type", u16to8(attribs.type));
	add_assoc_zval(return_value, "rel", u16to8(attribs.rel));
	add_assoc_zval(return_value, "partnerType", u16to8(attribs.partnerType));
	add_assoc_zval(return_value, "partnerRel", u16to8(attribs.partnerRel));
	add_assoc_zval(return_value, "kernelRel", u16to8(attribs.kernelRel));
	add_assoc_zval(return_value, "cpicConvId", u16to8(attribs.cpicConvId));
	add_assoc_zval(return_value, "progName", u16to8(attribs.progName));

	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
}
/* }}} */


/* {{{ proto string sapnwrfc::close()
   Close the current connection */
PHP_METHOD(sapnwrfc, close) {
	zval *object = getThis();
	sapnwrfc_object *intern = (sapnwrfc_object*)zend_object_store_get_object(object TSRMLS_CC);

	RFC_ERROR_INFO errorInfo;
	RFC_RC rc = RFC_OK;

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	if (intern->handle != NULL) {
		rc = RfcCloseConnection(intern->handle, &errorInfo);
		intern->handle = NULL;
		if (rc != RFC_OK) {
		  SAPNW_rfc_conn_error("Problem closing RFC connection handle",
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		  RETURN_NULL();
		}
	}

	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string sapnwrfc::ping()
   Ping the current connection */
PHP_METHOD(sapnwrfc, ping) {
	zval *object = getThis();
	sapnwrfc_object *intern = (sapnwrfc_object*)zend_object_store_get_object(object TSRMLS_CC);

	RFC_ERROR_INFO errorInfo;
	RFC_RC rc = RFC_OK;

	if (intern->handle != NULL) {
		rc = RfcPing(intern->handle, &errorInfo);
		if (rc != RFC_OK) {
		  RETURN_NULL();
		}
	}
	else {
		  RETURN_NULL();
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string sapnwrfc::get_sso_ticket()
   get an SSO Ticket from the current partner connection */
PHP_METHOD(sapnwrfc, get_sso_ticket) {
	zval *object = getThis();
	sapnwrfc_object *intern = (sapnwrfc_object*)zend_object_store_get_object(object TSRMLS_CC);

	RFC_ERROR_INFO errorInfo;
	RFC_RC rc = RFC_OK;
	SAP_UC * ssoTicket = NULL;
	unsigned length;

	sapnwrfc_global_error = false;

	if (intern->handle != NULL) {
		 //DECL_EXP RFC_RC SAP_API RfcGetPartnerSSOTicket(RFC_CONNECTION_HANDLE rfcHandle, SAP_UC *ssoTicket, unsigned *length, RFC_ERROR_INFO* errorInfo);

		rc = RfcGetPartnerSSOTicket(intern->handle, ssoTicket, &length, &errorInfo);
		if (rc != RFC_OK) {
			SAPNW_rfc_conn_error("Problem in RfcGetPartnerSSOTicket: ",
							   errorInfo.code,
							   u16to8(errorInfo.key),
							   u16to8(errorInfo.message));
			RETURN_NULL();
		}
	}
	else {
		  RETURN_NULL();
	}
	RETURN_STRING(Z_STRVAL_P(u16to8c(ssoTicket, length)), 1);
}
/* }}} */


/* Get the Metadata description of a Function Module */

/* {{{ proto string sapnwrfc::function_lookup()
   discover an interface definition for an RFC */
PHP_METHOD(sapnwrfc, function_lookup) {
	zval *object = getThis();
	sapnwrfc_function_object *func_obj;
	char *function_name;
	long len;
	RFC_ERROR_INFO errorInfo;
	RFC_RC rc = RFC_OK;
	SAP_UC * fname;
	RFC_FUNCTION_DESC_HANDLE func_desc_handle;
	RFC_PARAMETER_DESC parm_desc;
	unsigned parm_count;
	int i;
	zval *array;

	sapnwrfc_object *intern = (sapnwrfc_object*)zend_object_store_get_object(object TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &function_name, &len) == FAILURE) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	func_desc_handle = RfcGetFunctionDesc(intern->handle, (fname = u8to16c(function_name)), &errorInfo);
	free((char *)fname);

	/* bail on a bad lookup */
	if (func_desc_handle == NULL) {
		SAPNW_rfc_conn_error(my_concatc2c("Problem looking up RFC: ", function_name),
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
		RETURN_NULL();
	}

	/* wrap in SAPNW::RFC::FunctionDescriptor  Object */
	Z_TYPE_P(return_value) = IS_OBJECT;
	return_value->value.obj = sapnwrfc_function_object_new_ex(sapnwrfc_function_ce, &func_obj TSRMLS_CC);
	func_obj->handle = func_desc_handle;
	func_obj->conn_handle = intern->handle;
	func_obj->name = make_stringc(function_name);
	add_property_string(return_value, "name", function_name, TRUE);

	/* Get the parameter details */
	rc = RfcGetParameterCount(func_obj->handle, &parm_count, &errorInfo);

	/* bail on a bad RfcGetParameterCount */
	if (rc != RFC_OK) {
		SAPNW_rfc_conn_error(my_concatc2c("Problem in RfcGetParameterCount: ", function_name),
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
		RETURN_NULL();
	}

	for (i = 0; i < parm_count; i++) {
		rc = RfcGetParameterDescByIndex(func_obj->handle, i, &parm_desc, &errorInfo);
		/* bail on a bad RfcGetParameterDescByIndex */
		if (rc != RFC_OK) {
			SAPNW_rfc_conn_error(my_concatc2c("Problem in RfcGetParameterDescByIndex: ", function_name),
							   errorInfo.code,
							   u16to8(errorInfo.key),
							   u16to8(errorInfo.message));
			RETURN_NULL();
		}
		MAKE_STD_ZVAL(array);
		array_init(array);
		add_assoc_string(array,"type",Z_STRVAL_P(u16to8((SAP_UC *) RfcGetTypeAsString(parm_desc.type))),TRUE);
		add_assoc_string(array,"direction",Z_STRVAL_P(u16to8((SAP_UC *) RfcGetDirectionAsString(parm_desc.direction))),TRUE);
		add_assoc_string(array,"description",Z_STRVAL_P(u16to8(parm_desc.parameterText)),TRUE);
		add_assoc_bool(array,"optional",parm_desc.optional);
		add_assoc_string(array,"defaultValue",Z_STRVAL_P(u16to8(parm_desc.defaultValue)),TRUE);
		add_property_zval(return_value,Z_STRVAL_P(u16to8(parm_desc.name)),array);
	}

	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
}
/* }}} */



/* {{{ proto void sapnwrfc_function::__construct(string path)
 Cronstructs a new RFC connection object. */
/* zend_replace_error_handling() is used to throw exceptions in case
   the constructor fails. Here we use this to ensure the object
   has a valid directory resource.

   When the constructor gets called the object is already created
   by the engine, so we must only call 'additional' initializations.
 */
PHP_METHOD(sapnwrfc_function, __construct) {
}
/* }}} */



/* Get the Metadata description of a Function Module */
/* {{{ proto string sapnwrfc_function_call::invoke()
   discover an interface definition for an RFC */
PHP_METHOD(sapnwrfc_function, invoke) {
	zval *object = getThis();
	zval *function_parameters;
    zval **data, **row;
    HashTable *parameters, *rows;
    HashPosition pointer, row_pointer;
    char *name;
    ulong num_index;
    uint str_length;
    RFC_RC rc = RFC_OK;
    RFC_ERROR_INFO errorInfo;
	RFC_TABLE_HANDLE tableHandle;
	RFC_STRUCTURE_HANDLE line;
	RFC_FUNCTION_HANDLE func_handle;
	RFC_PARAMETER_DESC paramDesc;
	zval *value;
	zval *parm_name;
	SAP_UC *p_name;
	int idx, r, isActive, len;
	unsigned tabLen;
	unsigned parm_count;

	sapnwrfc_function_object *intern = (sapnwrfc_function_object*)zend_object_store_get_object(object TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	sapnwrfc_global_error = false;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &function_parameters, &len) == FAILURE) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	// must be an array
	if (Z_TYPE_P(function_parameters) != IS_ARRAY) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	// construct a new function call handle
	func_handle = RfcCreateFunction(intern->handle, &errorInfo);

	/* bail on a bad lookup */
	if (func_handle == NULL) {
		SAPNW_rfc_call_error(my_concatc2c("Problem Creating Function Data Container RFC: ", Z_STRVAL_P(intern->name)),
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
		RETURN_NULL();
	}

	/* set the active/inactive parameters */
	rc = RfcGetParameterCount(intern->handle, &parm_count, &errorInfo);

	/* bail on a bad RfcGetParameterCount */
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem in RfcGetParameterCount: ", Z_STRVAL_P(intern->name)),
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
		RETURN_NULL();
	}

	for (idx = 0; idx < parm_count; idx++) {
		rc = RfcGetParameterDescByIndex(intern->handle, idx, &paramDesc, &errorInfo);
		/* bail on a bad RfcGetParameterDescByIndex */
		if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("Problem in RfcGetParameterDescByIndex: ", Z_STRVAL_P(intern->name)),
							   errorInfo.code,
							   u16to8(errorInfo.key),
							   u16to8(errorInfo.message));
			RETURN_NULL();
		}
		parm_name = u16to8(paramDesc.name);
		if (zend_is_true(zend_read_property(sapnwrfc_function_ce, object, Z_STRVAL_P(parm_name), Z_STRLEN_P(parm_name), TRUE TSRMLS_CC))) {
			rc = RfcSetParameterActive(func_handle, paramDesc.name, TRUE, &errorInfo);
		}
		else {
			rc = RfcSetParameterActive(func_handle, paramDesc.name, FALSE, &errorInfo);
		}
	}

	// un pick all the function parameters passed in
	parameters = Z_ARRVAL_P(function_parameters);
    idx = zend_hash_num_elements(parameters);

	/* loop through all Input/Changing/tables parameters and set the values in the call */
	for(zend_hash_internal_pointer_reset_ex(parameters, &pointer); zend_hash_get_current_data_ex(parameters, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(parameters, &pointer)) {

		if (zend_hash_get_current_key_ex (parameters, &name, &str_length, &num_index, 0, &pointer) != HASH_KEY_IS_STRING) {
		zend_error(E_ERROR, "Method sapnwrfc::__constructor() all connection parameter keys must be strings");
		}

		/* get the parameter description */
		rc = RfcGetParameterDescByName(intern->handle, (p_name = u8to16c(name)), &paramDesc, &errorInfo);

		/* bail on a bad call for parameter description */
		if (rc != RFC_OK) {
			free(p_name);
			SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetParameterDescByName: ", name),
							   errorInfo.code,
							   u16to8(errorInfo.key),
							   u16to8(errorInfo.message));
			rc = RfcDestroyFunction(func_handle, &errorInfo);
			RETURN_NULL();
		}

		value = *data;
		switch(paramDesc.direction) {
			case RFC_EXPORT:
				break;
			case RFC_IMPORT:
			case RFC_CHANGING:
				set_parameter_value(func_handle, intern->handle, name, value);
				break;
			case RFC_TABLES:
				if (my_is_empty(value))
					continue;

				if (Z_TYPE_P(value) != IS_ARRAY)
					SAPNW_rfc_call_error1("RFC_TABLES requires LIST:", name);

				rc = RfcGetTable(func_handle, (p_name = u8to16c(name)), &tableHandle, &errorInfo);
				free(p_name);
				if (rc != RFC_OK) {
					SAPNW_rfc_call_error(my_concatc2c("(set)Problem with RfcGetTable: %s", name),
											errorInfo.code,
											u16to8(errorInfo.key),
											u16to8(errorInfo.message));
					rc = RfcDestroyFunction(func_handle, &errorInfo);
					RETURN_NULL();
				}
				rows = Z_ARRVAL_P(value);
			    r = 0;
				for(zend_hash_internal_pointer_reset_ex(rows, &row_pointer); zend_hash_get_current_data_ex(rows, (void**) &row, &row_pointer) == SUCCESS; zend_hash_move_forward_ex(rows, &row_pointer)) {
					r++;
					line = RfcAppendNewRow(tableHandle, &errorInfo);
					if (line == NULL) {
						SAPNW_rfc_call_error(my_concatc2c("Problem with RfcAppendNewRow: %s", name),
												errorInfo.code,
												u16to8(errorInfo.key),
												u16to8(errorInfo.message));
						rc = RfcDestroyFunction(func_handle, &errorInfo);
						RETURN_NULL();
					}
					set_table_line(line, *row);
				}
				break;
			default:
				fprintf(stderr, "should NOT get here!\n");
				exit(1);
			break;
		}
		if (sapnwrfc_global_error) {
			rc = RfcDestroyFunction(func_handle, &errorInfo);
			RETURN_NULL();
		}
	}

	rc = RfcInvoke(intern->conn_handle, func_handle, &errorInfo);

  /* bail on a bad RFC Call */
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem Invoking RFC: %s",
								Z_STRVAL_P(intern->name)),
								errorInfo.code,
								u16to8(errorInfo.key),
								u16to8(errorInfo.message));
		rc = RfcDestroyFunction(func_handle, &errorInfo);
		RETURN_NULL();
	}

	array_init(return_value);
	 /* Get the parameter details */
	rc = RfcGetParameterCount(intern->handle, &parm_count, &errorInfo);

	/* bail on a bad RfcGetParameterCount */
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem in RfcGetParameterCount: ", Z_STRVAL_P(intern->name)),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
		rc = RfcDestroyFunction(func_handle, &errorInfo);
		RETURN_NULL();
	}

	for (idx = 0; idx < parm_count; idx++) {
		rc = RfcGetParameterDescByIndex(intern->handle, idx, &paramDesc, &errorInfo);
		/* bail on a bad RfcGetParameterDescByIndex */
		if (rc != RFC_OK) {
			SAPNW_rfc_call_error(my_concatc2c("Problem in RfcGetParameterDescByIndex: ", Z_STRVAL_P(intern->name)),
									errorInfo.code,
									u16to8(errorInfo.key),
									u16to8(errorInfo.message));
			rc = RfcDestroyFunction(func_handle, &errorInfo);
			RETURN_NULL();
		}
		RfcIsParameterActive(func_handle, paramDesc.name, &isActive, &errorInfo);
		if (!isActive)
			continue;

		switch(paramDesc.direction) {
			case RFC_IMPORT:
				break;
			case RFC_EXPORT:
			case RFC_CHANGING:
				value = get_parameter_value(u16to8(paramDesc.name), func_handle, intern->handle);
				add_assoc_zval(return_value, Z_STRVAL_P(u16to8(paramDesc.name)), value);
				break;
			case RFC_TABLES:
				rc = RfcGetTable(func_handle, paramDesc.name, &tableHandle, &errorInfo);
				if (rc != RFC_OK) {
					SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetTable: ",
											Z_STRVAL_P(u16to8(paramDesc.name))),
											errorInfo.code,
											u16to8(errorInfo.key),
											u16to8(errorInfo.message));
				}
				rc = RfcGetRowCount(tableHandle, &tabLen, NULL);
				if (rc != RFC_OK) {
					SAPNW_rfc_call_error(my_concatc2c("Problem with RfcGetRowCount: ",
											Z_STRVAL_P(u16to8(paramDesc.name))),
											errorInfo.code,
											u16to8(errorInfo.key),
											u16to8(errorInfo.message));
				}
				ALLOC_INIT_ZVAL(value);
				array_init(value);
				for (r = 0; r < tabLen; r++){
					RfcMoveTo(tableHandle, r, NULL);
					line = RfcGetCurrentRow(tableHandle, NULL);
					add_next_index_zval(value, get_table_line(line));
				}
				add_assoc_zval(return_value, Z_STRVAL_P(u16to8(paramDesc.name)), value);
				break;
		}
		if (sapnwrfc_global_error) {
			rc = RfcDestroyFunction(func_handle, &errorInfo);
			RETURN_NULL();
		}
	}

	rc = RfcDestroyFunction(func_handle, &errorInfo);
	if (rc != RFC_OK) {
//		fprintfU(stderr, cU("RfcDestroyFunction: %d - %s - %s\n"),
//							 errorInfo.code,
//							 u16to8(errorInfo.key),
//							 u16to8(errorInfo.message));
		SAPNW_rfc_call_error("Problem destroying RfcDestroyFunction handle",
							errorInfo.code,
							u16to8(errorInfo.key),
							u16to8(errorInfo.message));
		RETURN_NULL();
	}

	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
}
/* }}} */


PHP_METHOD(sapnwrfc_function, activate) {
	zval *object = getThis();
	char *parm_name;
    RFC_RC rc = RFC_OK;
    RFC_ERROR_INFO errorInfo;
	RFC_PARAMETER_DESC paramDesc;
	SAP_UC *pname;
	int len;
  sapnwrfc_function_object *intern;

	sapnwrfc_global_error = false;

	intern = (sapnwrfc_function_object*)zend_object_store_get_object(object TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &parm_name, &len) == FAILURE) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	/* get the parameter description */
	rc = RfcGetParameterDescByName(intern->handle, (pname = u8to16c(parm_name)), &paramDesc, &errorInfo);
	free((char *)pname);

	/* bail on a bad lookup */
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem looking up RFC parameter: ", parm_name),
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
		RETURN_NULL();
	}

	zend_update_property_bool(sapnwrfc_function_ce, object, parm_name, strlen(parm_name), TRUE TSRMLS_CC);

	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);

	RETURN_TRUE;
}


PHP_METHOD(sapnwrfc_function, deactivate) {
	zval *object = getThis();
	char *parm_name;
    RFC_RC rc = RFC_OK;
    RFC_ERROR_INFO errorInfo;
	RFC_PARAMETER_DESC paramDesc;
	SAP_UC *pname;
	int len;
  sapnwrfc_function_object *intern;

	sapnwrfc_global_error = false;

	intern = (sapnwrfc_function_object*)zend_object_store_get_object(object TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &parm_name, &len) == FAILURE) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	/* get the parameter description */
	rc = RfcGetParameterDescByName(intern->handle, (pname = u8to16c(parm_name)), &paramDesc, &errorInfo);
	free((char *)pname);

	/* bail on a bad lookup */
	if (rc != RFC_OK) {
		SAPNW_rfc_call_error(my_concatc2c("Problem looking up RFC parameter: ", parm_name),
						   errorInfo.code,
						   u16to8(errorInfo.key),
						   u16to8(errorInfo.message));
		RETURN_NULL();
	}

	zend_update_property_bool(sapnwrfc_function_ce, object, parm_name, strlen(parm_name), FALSE TSRMLS_CC);

	zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);

	RETURN_TRUE;
}


#define SAPNWRFC_VERSION_MAJOR	0
#define SAPNWRFC_VERSION_MINOR	11

/* {{{ proto bool sapnwrfc_version()
 Retrieve sapnwrfc version */
PHP_FUNCTION(sapnwrfc_version) {
	char * ver;
	int len;

	len = spprintf(&ver, 0, "%d.%02d",
		SAPNWRFC_VERSION_MAJOR, SAPNWRFC_VERSION_MINOR);

	RETURN_STRINGL(ver, len, 0);
}
/* }}} */

/* {{{ proto bool sapnwrfc_version_array()
 Retrieve yourext version as array */
PHP_FUNCTION(sapnwrfc_version_array) {
	char *ver;
	int len = spprintf(&ver, 0, "%d.%d",
		SAPNWRFC_VERSION_MAJOR, SAPNWRFC_VERSION_MINOR);

	array_init(return_value);
	add_assoc_long(return_value, "major", SAPNWRFC_VERSION_MAJOR);
	add_assoc_long(return_value, "minor", SAPNWRFC_VERSION_MINOR);
	add_assoc_string(return_value, "cvs", "$Id: $", 1);
	add_assoc_stringl(return_value, "ver", ver, len, 0);
}
/* }}} */

/* {{{ proto bool sapnwrfc_version()
 Retrieve sapnwrfc version */
PHP_FUNCTION(sapnwrfc_rfcversion) {
	char * ver;
	int len;
	unsigned majorVersion, minorVersion, patchLevel;

	RfcGetVersion(&majorVersion, &minorVersion, &patchLevel);

	len = spprintf(&ver, 0, "major: %d minor: %d patch: %d",
			majorVersion, minorVersion, patchLevel);

	RETURN_STRINGL(ver, len, 0);
}
/* }}} */

/* {{{ proto bool sapnwrfc_setinipath()
 set the ini file path for the RFC SDK */
PHP_FUNCTION(sapnwrfc_setinipath) {
	char *ini_path;
	long len;
	RFC_ERROR_INFO errorInfo;
	SAP_UC * pname;
	RFC_RC rc;

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ini_path, &len) == FAILURE) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}

	rc = RfcSetIniPath((pname = u8to16c(ini_path)), &errorInfo);
	free((char *)pname);
	if (rc != RFC_OK) {
	  SAPNW_rfc_conn_error("Problem setting the INI path ",
							errorInfo.code,
							u16to8(errorInfo.key),
							u16to8(errorInfo.message));
	  RETURN_NULL();
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool sapnwrfc_reloadinifile()
 Reload the INI file for the NW RFC SDK */
PHP_FUNCTION(sapnwrfc_reloadinifile) {
	RFC_ERROR_INFO errorInfo;
	RFC_RC rc;

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	rc = RfcReloadIniFile(&errorInfo);
	if (rc != RFC_OK) {
	  SAPNW_rfc_conn_error("Problem reloading the INI path ",
							errorInfo.code,
							u16to8(errorInfo.key),
							u16to8(errorInfo.message));
	  RETURN_NULL();
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool sapnwrfc_removefunction()
 remove a function description from the cache RFC SDK */
PHP_FUNCTION(sapnwrfc_removefunction) {
	char *sysid;
	char *name;
	long len_sysid, len_name;
	RFC_ERROR_INFO errorInfo;
	SAP_UC * psysid;
	SAP_UC * pname;
	RFC_RC rc;

	zend_replace_error_handling(EH_THROW, zend_exception_get_default(EXTSRM), NULL TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &sysid, &len_sysid, &name, &len_name) == FAILURE) {
		zend_replace_error_handling(EH_NORMAL, NULL, NULL TSRMLS_CC);
		return;
	}
	if (len_name < 1) {
	  SAPNW_rfc_call_error1("Incorrect parameters supplied for remove_function ", "");
    }
	rc = RfcRemoveFunctionDesc(NULL, (pname = u8to16c(name)), &errorInfo);
    if (len_sysid > 0) {
	  rc = RfcRemoveFunctionDesc((psysid = u8to16c(sysid)), pname, &errorInfo);
	  fprintfU(stderr, cU("repId: [%s] function: [%s]\n"), psysid, pname);
	  free((char *)psysid);
	}
	free((char *)pname);
	if (rc != RFC_OK) {
	  SAPNW_rfc_call_error("Problem removing function description from the cache ",
							errorInfo.code,
							u16to8(errorInfo.key),
							u16to8(errorInfo.message));
	  RETURN_NULL();
	}

	RETURN_TRUE;
}
/* }}} */


/* If you declare any globals in php_sapnwrfc.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(sapnwrfc)
*/

/* {{{ sapnwrfc_functions[]
 *
 * Every user visible function must have an entry in sapnwrfc_functions[].
 */
zend_function_entry sapnwrfc_functions[] = {
	PHP_FE(sapnwrfc_version,       NULL)
	PHP_FE(sapnwrfc_version_array, NULL)
	PHP_FE(sapnwrfc_rfcversion,    NULL)
	PHP_FE(sapnwrfc_setinipath,    NULL)
	PHP_FE(sapnwrfc_reloadinifile,    NULL)
    PHP_FE(sapnwrfc_removefunction,   NULL)
	{NULL, NULL, NULL}	/* Must be the last line in sapnwrfc_functions[] */
};
/* }}} */

/* {{{ sapnwrfc_module_entry
 */
zend_module_entry sapnwrfc_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"sapnwrfc",
	sapnwrfc_functions,
	PHP_MINIT(sapnwrfc),
	PHP_MSHUTDOWN(sapnwrfc),
	PHP_RINIT(sapnwrfc),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(sapnwrfc),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(sapnwrfc),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SAPNWRFC
ZEND_GET_MODULE(sapnwrfc)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("sapnwrfc.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_sapnwrfc_globals, sapnwrfc_globals)
    STD_PHP_INI_ENTRY("sapnwrfc.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_sapnwrfc_globals, sapnwrfc_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_sapnwrfc_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_sapnwrfc_init_globals(zend_sapnwrfc_globals *sapnwrfc_globals)
{
	sapnwrfc_globals->global_value = 0;
	sapnwrfc_globals->global_string = NULL;
}
*/
/* }}} */


PHP_MINIT_FUNCTION(sapnwrfc) {
	zend_class_entry ce;
	zend_class_entry ce_func;
	zend_class_entry cex_conn;
	zend_class_entry cex_call;

	INIT_CLASS_ENTRY(ce, "sapnwrfc", sapnwrfc_class_functions);
	sapnwrfc_ce = zend_register_internal_class(&ce TSRMLS_CC);
	sapnwrfc_ce->create_object = sapnwrfc_object_new;
	memcpy(&sapnwrfc_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	sapnwrfc_handlers.clone_obj = sapnwrfc_object_clone;

	INIT_CLASS_ENTRY(ce_func, "sapnwrfc_function", sapnwrfc_function_class_functions);
	sapnwrfc_function_ce = zend_register_internal_class(&ce_func TSRMLS_CC);
	sapnwrfc_function_ce->create_object = sapnwrfc_function_object_new;
	memcpy(&sapnwrfc_function_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	sapnwrfc_function_handlers.clone_obj = sapnwrfc_function_object_clone;

	INIT_CLASS_ENTRY(cex_conn, "sapnwrfcConnectionException", sapnwrfc_connection_exception_class_functions);
#ifdef HAVE_SPL
	sapnwrfc_connection_exception_ce = zend_register_internal_class_ex(&cex_conn, spl_ce_RuntimeException, NULL TSRMLS_CC);
#else
	sapnwrfc_connection_exception_ce = zend_register_internal_class_ex(&cex_conn, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
#endif
	sapnwrfc_connection_exception_ce->ce_flags |= ZEND_ACC_FINAL;
    zend_declare_property_long(sapnwrfc_connection_exception_ce, "code", sizeof("code")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_string(sapnwrfc_connection_exception_ce, "key", sizeof("key")-1, "00000", ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(cex_call, "sapnwrfcCallException", sapnwrfc_call_exception_class_functions);
#ifdef HAVE_SPL
	sapnwrfc_call_exception_ce = zend_register_internal_class_ex(&cex_call, spl_ce_RuntimeException, NULL TSRMLS_CC);
#else
	sapnwrfc_call_exception_ce = zend_register_internal_class_ex(&cex_call, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
#endif
	sapnwrfc_call_exception_ce->ce_flags |= ZEND_ACC_FINAL;
    zend_declare_property_long(sapnwrfc_call_exception_ce, "code", sizeof("code")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_string(sapnwrfc_call_exception_ce, "key", sizeof("key")-1, "00000", ZEND_ACC_PUBLIC TSRMLS_CC);

	/* If you have INI entries, uncomment these lines
	ZEND_INIT_MODULE_GLOBALS(sapnwrfc, php_sapnwrfc_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(sapnwrfc) {
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(sapnwrfc) {
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(sapnwrfc) {
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sapnwrfc) {
	php_info_print_table_start();
	php_info_print_table_header(2, "sapnwrfc support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
