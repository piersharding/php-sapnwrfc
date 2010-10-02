<?php
dl("sapnwrfc.so");
echo "sapnwrfc version: ".sapnwrfc_version()."\n";
echo "nw rfc sdk version: ".sapnwrfc_rfcversion()."\n";
$config = array('ashost' => 'gecko.local.net',
                'sysnr' => "01",
                'client' => "001",
                'user' => 'developer',
                'passwd' => 'developer',
                'lang' => 'EN',
                'trace' => '1' );

// we must have a valid connection
try {
    $conn = new sapnwrfc($config);
    $fds = $conn->function_lookup("STFC_DEEP_STRUCTURE");
    $fdt = $conn->function_lookup("STFC_DEEP_TABLE");
    $parms = array('IMPORTSTRUCT' => array('I' => 123, 'C' => 'AbCdEf', 'STR' => 'The quick brown fox ...'));
    $results = $fds->invoke($parms);
    var_dump($results);
    $parms = array('IMPORT_TAB' => array(array('I' => 123, 'C' => 'AbCdEf', 'STR' => 'The quick brown fox ...')));
    $results = $fdt->invoke($parms);
    var_dump($results);
    $conn->close();
}
catch (Exception $e) {
    //var_dump($e);
    echo "Exception message: ".$e->getMessage();
    throw new Exception('Assertion failed.');
}
