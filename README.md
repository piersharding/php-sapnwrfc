```
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2010 Piers Harding                                     |
  +----------------------------------------------------------------------+
  | This package is released under the same terms as PHP itself.         |
  +----------------------------------------------------------------------+
  | Author: Piers Harding <piers@ompka.net>                              |
  +----------------------------------------------------------------------+
```

# Summary

Welcome to the sapnwrfc PHP package.  This package is intended to facilitate RFC calls to an SAP R/3 system of release NW2004x and above.  It may work for earlier versions but it hasn't been tested.
The fundamental purpose of the production of this package, is to provide a clean object oriented interface to RFC calls from within PHP.  This will hopefully have a number of effects:

*  make it really easy to do RFC calls to SAP from PHP in an object oriented fashion (Doh!)
*  promote PHP as the interface/scripting/glue language of choice for interaction with SAP R/3.
*  make the combination of Linux, Apache, and PHP the killer app for internet connectivity with SAP.
*  Establish a small fun open source project that people are more than welcome to contribute to, if they so wish.

# Installation

Please refer [INSTALL.md](INSTALL.md)

For Windows, some precompiled binaries have been supplied by chemik3 (https://github.com/chemik3 - thanks!) available at 
- https://sourceforge.net/projects/saprfcsapnwrfc/files/?source=navbar


## Check installation

should list the module after installation
```cli
php -m 
```

Display the installed versions
```php
var_dump(sapnwrfc_version());
var_dump(sapnwrfc_rfcversion());
```

# Getting started

```php
use sapnwrfc;
use sapnwrfcConnectionException;
use sapnwrfcCallException;

// see parameters here: http://help.sap.com/saphelp_nwpi711/helpdata/en/48/c7bb09da5e31ebe10000000a42189b/content.htm
// example when using a message server
$config = [
    'MSHOST' => '...',
    
    'CLIENT' => '...',
    
    'R3NAME' => '...',
    'GROUP' => '...',
    
    'CODEPAGE' => '...',
    
    'LANG' => 'en',
    
    'LCHECK' => true,
    'TRACE' => true,
    
    'user' => '...',
    'passwd' => '...'
];

try {
    $conn = new sapnwrfc($config);
} catch(sapnwrfcConnectionException $ex){
    // do something!
}

if($conn->ping() === true){
    //call a real method
    try {
        $func = $conn->function_lookup('RFC_PING');
        
        /**
         * Call the function and return the result
         * @return array
         */
        $result = $func->invoke([]);
    } catch (sapnwrfcCallException $ex){
        // do something
    }
}
```

# Documentation

## Hint: change log directory
The sapnwrfc will create a logfile, if you enable `TRACE` 
It will by default use the current working dir. You can view your with `getcwd()`

If you want to log it in the directory where you have your logfiles, just change the directory when you connect
```php
$cwd = getcwd();
chdir('../your/log/folder');
try {
    $conn = new sapnwrfc($config);
} catch(sapnwrfcConnectionException $ex){
    // do something! e.g. log it
}
chdir($cwd); //change the cwd back to the previous value
```

## Functions and classes
A list of all available functions/methods and their parameters + return values

```php
<?php

/**
 * Get the version of this module
 * 
 * @return string
 */
function sapnwrfc_version()
{}

/**
 * Get the version of this module
 * Array keys: major, minor, cvs, ver
 *
 * @return array
 */
function sapnwrfc_version_array()
{}

/**
 * Get the sapnwrfc version
 *
 * @return string
 */
function sapnwrfc_rfcversion()
{}

/**
 * Set the ini file path for the RFC SDK
 *
 * @param string $path            
 * @return boolean
 */
function sapnwrfc_setinipath($path)
{}

/**
 * Reload the INI file for the NW RFC SDK
 *
 * @return boolean
 */
function sapnwrfc_reloadinifile()
{}

/**
 * Remove a function description from the cache RFC SDK
 *
 * @param string $systemId            
 * @param string $functionName            
 *
 * @return boolean
 */
function sapnwrfc_removefunction($systemId = '', $functionName)
{}

/**
 * Connect exception
 */
class sapnwrfcConnectionException extends Exception
{
    // @ todo define the error codes here
}

/**
 * Call exception
 */
class sapnwrfcCallException extends Exception
{
    // @ todo define the error codes here
}

/**
 * Connection class
 */
class sapnwrfc
{

    /**
     * Create a connection with SAP NW RFC
     *
     * @see http://help.sap.com/saphelp_nwpi711/helpdata/en/48/c7bb09da5e31ebe10000000a42189b/content.htm
     * @param array $config            
     * @throws sapnwrfcConnectionException
     */
    public function __construct(array $config);

    /**
     * Get the used connection parameters
     *
     * @return array
     */
    public function connection_attributes();

    /**
     *
     * @return boolean
     */
    public function close();

    /**
     * Is the server around?
     *
     * @return boolean
     */
    public function ping();

    /**
     * Get the SSO ticket
     *
     * @return string
     */
    public function get_sso_ticket();

    /**
     *
     * @param string $functionName            
     * @throws sapnwrfcConnectionException
     * @throws sapnwrfcCallException
     *
     * @return sapnwrfc_function
     */
    public function function_lookup($functionName);
}

/**
 * Function call class
 */
class sapnwrfc_function
{
    public function __construct();
    
    /**
     * 
     * @param array $parameters
     * @return array
     */
    public function invoke(array $parameters);
    
    public function activate($parameterName);
    
    public function deactivate($parameterName);
    
}
```
