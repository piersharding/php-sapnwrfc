<?php
require_once 'PHPUnit/Framework.php';
require_once '../sap_config.php';
require_once 'spyc.php';
global $SAP_CONFIG;
dl("sapnwrfc.so");

class FuncDescTest extends PHPUnit_Framework_TestCase
{

    protected function setUp() {
        global $SAP_CONFIG;
        $this->config = Spyc::YAMLLoad($SAP_CONFIG);
        echo "sapnwrfc version: ".sapnwrfc_version()."\n";
        echo "nw rfc sdk version: ".sapnwrfc_rfcversion()."\n";
    }
    
    public function testFuncDesc1() {
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            $func = $conn->function_lookup("RFC_READ_REPORT");
            $this->assertEquals($func->name, "RFC_READ_REPORT");
            $this->assertNotNull(sapnwrfc_removefunction("", "RFC_READ_REPORT"));
            $this->assertNotNull(sapnwrfc_removefunction("N4S", "RFC_READ_REPORT"));
        }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    public function testFuncDesc2() {
       try {
            foreach (array("STFC_CHANGING", "STFC_XSTRING", "RFC_READ_TABLE", "RFC_READ_REPORT", "RPY_PROGRAM_READ", "RFC_PING", "RFC_SYSTEM_INFO") as $i) {
                $conn = new sapnwrfc($this->config);
                // we must have a valid connection
                $this->assertNotNull($conn);
                $func = $conn->function_lookup($i);
                $this->assertEquals($func->name, $i);
            }
        }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    public function testFuncDesc3() {
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            foreach (array("STFC_CHANGING", "STFC_XSTRING", "RFC_READ_TABLE", "RFC_READ_REPORT", "RPY_PROGRAM_READ", "RFC_PING", "RFC_SYSTEM_INFO") as $f) {
                $func = $conn->function_lookup($f);
                $this->assertEquals($func->name, $f);
            }
        }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    public function testFuncDesc4() {
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            for ($i=0; $i<100; $i++) {
            //echo "iter: $i\n";
                foreach (array("STFC_CHANGING", "STFC_XSTRING", "RFC_READ_TABLE", "RFC_READ_REPORT", "RPY_PROGRAM_READ", "RFC_PING", "RFC_SYSTEM_INFO") as $f) {
                    $func = $conn->function_lookup($f);
                    $this->assertEquals($func->name, $f);
                }
            }
        }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
}
