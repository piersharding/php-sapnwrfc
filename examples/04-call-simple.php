<?php
/**
 * Get some system informations
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

$conn = new sapnwrfc($config);

$func = $conn->function_lookup('RFC_SYSTEM_INFO');

$result = $func->invoke([]);

// is an empty array
if (is_array($result)) {
    var_dump($result);
} else {
    echo 'Could not ping the SAP system';
}

$conn->close();
