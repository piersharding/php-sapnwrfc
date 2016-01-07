
# Install sapnwrfc

Below are the install instructions for Unix and Windows


## Unix

### Requirements
- PHP sources (http://www.php.net/downloads.php)
- php-devel package for installation sapnwrfc as dynamic module
- GNU tools (autoconf, automake, flex, libtool, gcc, m4, make)
- Non-Unicode SAP RFCSDK 6.20 for your platform (see below)

If you are SAP customer you can download it from http://service.sap.com/swdc under 
<Support Packages and Patches - My Company's Application Components
      SAP WEB AS - SAP WEB AS 6.20 - SAP RFC SDK>)


### Install the SAP NW RFC SDK

You must obtain and install the NW RFC SDK from SAP for this extension to work.
[http://service.sap.com](http://service.sap.com)

Please follow OSS note 1056472, 1058327, 1236530

See also https://help.sap.com/saphelp_46c/helpdata/de/22/042887488911d189490000e829fbbd/content.htm?frameset=/de/22/04287a488911d189490000e829fbbd/frameset.htm&current_toc=/de/22/0438e7488911d189490000e829fbbd/plain.htm&node_id=7


### Build static into PHP

Extract source tarball to PHP source tree (under ext/ directory)  and rebuild PHP with commands 
```
rm ./configure
./buildconf --force
./configure --with-sapnwrfc=<path to nwrfcsdk> (+ your configuration directives)
make
```


### As a dynamic extension

Extract source tarball to some directory and build dynamic module with commands  
```
cd <sapnwrfc dir>
phpize
./configure
./configure --with-sapnwrfc=<path to nwrfcsdk>
make 
make install
```

Enable sapnwrfc extension editing your php.ini 
```
extension = sapnwrfc.so
```

see also the example [build.sh](build.sh)


## Windows


### Install the SAP NW RFC SDK

- Get the `SAP NW RFC SDK` from http://service.sap.com.
- Extract the SDK archive with SAPCAR
- Copy the `*.dll` files from the `lib` folder to (just use the same like your PHP installation)
  - x86 (32bit): C:\Windows\system32
  - x64 (64bit): C:\Windows\SysWOW64


### Install the precompiled PHP extension

- copy the right compiled version `php_sapnwrfc_*.dll` from [builds](builds) to your `php/ext` folder
- active it in your `php.ini` (extension = php_sapnwrfc_*.dll)


### Build it on your own

NOTE: This is harder than you think :-)

- download SAP NW RFC SDK (i used 7.2.xxx)
  - extract with SAPCAR.exe to C:\SAP\sapcar_x86\nwrfcsdk (for x64 C:\SAP\sapcar_x64\nwrfcsdk)
- get PHP "Development package (SDK to develop PHP extension)" http://windows.php.net/snapshots/
  - install to C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86
- install VS2012 (can be express for free)
  - create a new project (Visual C++ -> Win32 Project -> choose DLL -> choose empty project)
  - add the files from `php-sapnwrfc` to the project (i think `php_sapnwrfc.h` and `sapnwrfc.c` are enough, but i copied all)
    - also add them not just to folder, but to the header files!
  - add c++ paths for *.h files
    - C:\SAP\sapcar_x86\nwrfcsdk\include;
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include;
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\Zend;
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\win32;
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\TSRM;
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\sapi;
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\main;
  - Declare c++ preprocessors: 
    - PHP_WIN32
    - ZEND_WIN32
    - ZEND_DEBUG=0
    - COMPILE_DL_SAPNWRFC
    - _UNICODE
    - UNICODE
    - SAPwithUNICODE
  - Linker add paths: 
    - C:\SAP\sapcar_x86\nwrfcsdk\lib
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\lib
  - Linker add: 
    - libsapucum.lib
    - sapnwrfc.lib
    - php5.lib
    
#### Helping articles

- https://wiki.php.net/internals/windows/stepbystepbuild
- http://blog.slickedit.com/2007/09/creating-a-php-5-extension-with-visual-c-2005/
- https://jojokahanding.wordpress.com/2014/05/26/writing-php-extensions-for-windowsbuilding-a-compile-environment/
