<?php
/**
 * Alternative ping the SAP system
 */
if (! extension_loaded('sapnwrfc')) {
    throw new \Exception('Extension "sapnwrfc" not loaded. Please see https://github.com/piersharding/php-sapnwrfc#installation');
}

// @see the available connection paraemters here
// http://help.sap.com/saphelp_nwpi711/helpdata/en/48/c7bb09da5e31ebe10000000a42189b/content.htm
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

// change the directory for logging
$cwd = getcwd();
chdir('my/log/dir');

try {
    $conn = new sapnwrfc($config);
} catch (Exception $ex) {
    throw $ex;
} finally {
    // change the cwd always back to the previous value
    chdir($cwd);
}

$func = $conn->function_lookup('RFC_PING');

$result = $func->invoke([]);

// is an empty array
if (is_array($result)) {
    echo 'Successfully pinged the SAP system';
} else {
    echo 'Could not ping the SAP system';
}

$conn->close();
