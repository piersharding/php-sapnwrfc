<?php
/**
 * Call a RFC function
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

$fds = $conn->function_lookup('STFC_DEEP_STRUCTURE');
$fdt = $conn->function_lookup('STFC_DEEP_TABLE');

$parms = [
    'IMPORTSTRUCT' => [
        'I' => 123,
        'C' => 'AbCdEf',
        'STR' => 'The quick brown fox ...'
    ]
];
$results = $fds->invoke($parms);

var_dump($results);

$parms = [
    'IMPORT_TAB' => [
        [
            'I' => 123,
            'C' => 'AbCdEf',
            'STR' => 'The quick brown fox ...'
        ]
    ]
];
$results = $fdt->invoke($parms);

var_dump($results);

$conn->close();
