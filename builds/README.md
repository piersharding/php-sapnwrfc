# Windows builds

Here are some precompiled .dll for windows.

# Installation of the precompiled *.dll

## Install the SAP NW RFC SDK
- Get the `SAP NW RFC SDK` from http://service.sap.com.
- Extract the SDK archive with SAPCAR
- Copy the `*.dll` files from the `lib` folder to (just use the same like your PHP installation)
  - x86 (32bit): C:\Windows\system32
  - x64 (64bit): C:\Windows\SysWOW64

## Install the PHP extension
- copy the right version `php_sapnwrfc_*.dll` to your `php/ext` folder
- active it in your `php.ini`

# How to compile from scratch
- download SAP NW RFC SDK (i used 7.2.xxx)
  - extract with SAPCAR.exe to C:\SAP\sapcar_x86\nwrfcsdk (for x64 C:\SAP\sapcar_x64\nwrfcsdk)
- get PHP "Development package (SDK to develop PHP extension)" http://windows.php.net/snapshots/
  - install to C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86
- install VS2012 (can be express for free)
  - create a new project (Visual C++ -> Win32 Project -> choose DLL -> choose empty project)
  - add the files from `php-sapnwrfc` to the project (i think `php_sapnwrfc.h` and `sapnwrfc.c` are enough, but i copied all)
    - also add them not just to folder, but to the header files!
  - add c++ paths for *.h files -> C:\SAP\sapcar_x86\nwrfcsdk\include;C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include;C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\Zend;C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\win32;C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\TSRM;C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\sapi;C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\include\main;
  - Declare c++ preprocessors: PHP_WIN32
ZEND_WIN32
ZEND_DEBUG=0
COMPILE_DL_SAPNWRFC
_UNICODE
UNICODE
SAPwithUNICODE
  - Linker add paths: 
    - C:\SAP\sapcar_x86\nwrfcsdk\lib
    - C:\php-sdk\phpdev\vc11\x86\php-5.6.9-dev-devel-VC11-x86\lib
  - Linker add: 
    - libsapucum.lib
    - sapnwrfc.lib
    - php5.lib
  - 

# Helping articles
http://blog.slickedit.com/2007/09/creating-a-php-5-extension-with-visual-c-2005/
https://wiki.php.net/internals/windows/stepbystepbuild
https://jojokahanding.wordpress.com/2014/05/26/writing-php-extensions-for-windowsbuilding-a-compile-environment/
