<?php
require_once 'PHPUnit/Framework.php';
require_once '../sap_config.php';
require_once 'spyc.php';
dl("sapnwrfc.so");
global $SAP_CONFIG;

class FuncTest extends PHPUnit_Framework_TestCase
{

    protected function setUp() {
        global $SAP_CONFIG;
        $this->config = Spyc::YAMLLoad($SAP_CONFIG);
        echo "sapnwrfc version: ".sapnwrfc_version()."\n";
        echo "nw rfc sdk version: ".sapnwrfc_rfcversion()."\n";
    }
    
    public function testFuncCall1() {
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);

            $func = $conn->function_lookup("RFC_READ_TABLE");
            //var_dump($fd);
            $this->assertEquals($func->name, "RFC_READ_TABLE");
            $parms = array('QUERY_TABLE' => "TRDIR",
                           'ROWCOUNT' => 50,
                            'OPTIONS' => array(array('TEXT' => "NAME LIKE 'RS%'")));
            $results = $func->invoke($parms);
            //var_dump($results);
          $this->assertEquals(count($results['DATA']), 50);
          $this->assertEquals($conn->close(), true);
            
       }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    
    public function testFuncCall2() {
       try {
            for ($i=0; $i<100; $i++) {
                //echo "iter: $i\n";
                $conn = new sapnwrfc($this->config);
                // we must have a valid connection
                $this->assertNotNull($conn);
                $func = $conn->function_lookup("RFC_READ_TABLE");
                $parms = array('QUERY_TABLE' => "TRDIR",
                               'ROWCOUNT' => 50,
                                'OPTIONS' => array(array('TEXT' => "NAME LIKE 'RS%'")));
                $results = $func->invoke($parms);
                $this->assertEquals(count($results['DATA']), 50);
                $this->assertEquals($conn->close(), true);
            }
            
       }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    
    public function testFuncCall3() {
       echo "testFuncCall3\n";
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            for ($i=0; $i<100; $i++) {
                //echo "iter: $i\n";
                $func = $conn->function_lookup("RFC_READ_TABLE");
                $parms = array('QUERY_TABLE' => "TRDIR",
                               'ROWCOUNT' => 50,
                                'OPTIONS' => array(array('TEXT' => "NAME LIKE 'RS%'")));
                $results = $func->invoke($parms);
                $this->assertEquals(count($results['DATA']), 50);
            }
            $this->assertEquals($conn->close(), true);
            
       }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    
    public function testFuncCall4() {
       echo "testFuncCall4\n";
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            $func = $conn->function_lookup("RFC_READ_TABLE");
            for ($i=0; $i<100; $i++) {
                $parms = array('QUERY_TABLE' => "TRDIR",
                               'ROWCOUNT' => 50,
                                'OPTIONS' => array(array('TEXT' => "NAME LIKE 'RS%'")));
                $results = $func->invoke($parms);
                $this->assertEquals(count($results['DATA']), 50);
            }
            $this->assertEquals($conn->close(), true);
            
       }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }

    static function str_hex($string){
        $hex='';
        for ($i=0; $i < strlen($string); $i++){
            $hex .= dechex(ord($string[$i]));
        }
        return $hex;
    }


    static function hex_str($hex){
        $string='';
        for ($i=0; $i < strlen($hex)-1; $i+=2){
            $string .= chr(hexdec($hex[$i].$hex[$i+1]));
        }
        return $string;
    }

    public function testFuncDeepCall() {
       echo "testFuncDeepCall\n";
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            $fds = $conn->function_lookup("STFC_DEEP_STRUCTURE");
            $this->assertEquals($fds->name, "STFC_DEEP_STRUCTURE");
            $fdt = $conn->function_lookup("STFC_DEEP_TABLE");
            $this->assertEquals($fdt->name, "STFC_DEEP_TABLE");
            for ($i=0; $i<100; $i++) {
                $parms = array('IMPORTSTRUCT' => array('I' => 123, 'C' => 'AbCdEf', 'STR' => 'The quick brown fox ...', 'XSTR' => FuncTest::str_hex("deadbeef")));
                $results = $fds->invoke($parms);
//                var_dump($results);
                $echostruct = $results['ECHOSTRUCT'];
//                echo "I is ".$echostruct['I']."\n";
                $this->assertEquals($echostruct['I'], 123);
                $this->assertEquals(trim($echostruct['C']), 'AbCdEf');
                $this->assertEquals($echostruct['STR'], 'The quick brown fox ...');
                $this->assertEquals(FuncTest::hex_str($echostruct['XSTR']), 'deadbeef');
                $parms = array('IMPORT_TAB' => array(array('I' => 123, 'C' => 'AbCdEf', 'STR' => 'The quick brown fox ...', 'XSTR' => FuncTest::str_hex("deadbeef"))));
                $results = $fdt->invoke($parms);
//                var_dump($results);
                $import_tab = $results['EXPORT_TAB'];
                foreach ($import_tab as $r) {
                    if (trim($r['C']) == 'Appended') {
                        continue;
                    }
                    $this->assertEquals($r['I'], 123);
                    $this->assertEquals(trim($r['C']), 'AbCdEf');
                    $this->assertEquals($r['STR'], 'The quick brown fox ...');
                    $this->assertEquals(FuncTest::hex_str($r['XSTR']), 'deadbeef');
                }
            }
            $this->assertEquals($conn->close(), true);
       }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
}
