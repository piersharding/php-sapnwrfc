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

Please see the INSTALL file

For Windows, some precompiled binaries have been supplied by chemik3 (https://github.com/chemik3 - thanks!) available at: https://sourceforge.net/projects/saprfcsapnwrfc/files/?source=navbar

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

//when using a message server
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

/**
 * ping the server
 * @return boolean
 */
$conn->ping();

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

