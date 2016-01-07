<?php
require_once 'PHPUnit/Framework.php';
require_once '../sap_config.php';
require_once 'spyc.php';
dl("sapnwrfc.so");
global $SAP_CONFIG;

class FuncDataTest extends PHPUnit_Framework_TestCase
{

    protected function setUp()
    {
        global $SAP_CONFIG;
        $this->config = Spyc::YAMLLoad($SAP_CONFIG);
        echo "sapnwrfc version: ".sapnwrfc_version()."\n";
        echo "nw rfc sdk version: ".sapnwrfc_rfcversion()."\n";
    }
    
    
    public static function str_hex($string)
    {
        $hex='';
        for ($i=0; $i < strlen($string); $i++) {
            $hex .= dechex(ord($string[$i]));
        }
        return $hex;
    }


    public static function hex_str($hex)
    {
        $string='';
        for ($i=0; $i < strlen($hex)-1; $i+=2) {
            $string .= chr(hexdec($hex[$i].$hex[$i+1]));
        }
        return $string;
    }

    public function testFuncDataCall1()
    {
        echo "testFuncDataCall1\n";
        try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            $func = $conn->function_lookup("Z_TEST_DATA");
            $this->assertEquals($func->name, "Z_TEST_DATA");
            for ($i=0; $i<100; $i++) {
                $parms = array(
                                'CHAR' => "German: öäüÖÄÜß",
                                'INT1' => 123,
                                'INT2' => 1234,
                                'INT4' => 123456,
                                'FLOAT' => 123456.00,
                                'NUMC' => "12345",
                                'DATE' => "20060709",
                                'TIME' => "200607",
                                'BCD' =>  '200607.123',
                                'ISTRUCT' => array( 'ZCHAR' => "German: öäüÖÄÜß", 'ZINT1' => 54, 'ZINT2' => 134, 'ZIT4' => 123456, 'ZFLT' => 123456.00, 'ZNUMC' => '12345', 'ZDATE' => '20060709', 'ZTIME' => '200607', 'ZBCD' => '200607.123'),
                                'DATA' => array(
                                    array( 'ZCHAR' => "German: öäüÖÄÜß", 'ZINT1' => 54, 'ZINT2' => 134, 'ZIT4' => 123456, 'ZFLT' => 123456.00, 'ZNUMC' => '12345', 'ZDATE' => '20060709', 'ZTIME' => '200607', 'ZBCD' => '200607.123'),
                                    array( 'ZCHAR' => "Slovenian: čćšČĆŠ", 'ZINT1' => 54, 'ZINT2' => 134, 'ZIT4' => 123456, 'ZFLT' => 123456.00, 'ZNUMC' => '12345', 'ZDATE' => '20060709', 'ZTIME' => '200607', 'ZBCD' => '200607.123'),
//                                    array( 'ZCHAR' => "German: öäüÖÄÜß", 'ZINT1' => 54, 'ZINT2' => 134, 'ZIT4' => 123456, 'ZFLT' => 123456.00, 'ZNUMC' => '12345', 'ZDATE' => '20060709', 'ZTIME' => '200607', 'ZBCD' => '200607.123'),
//                                    array( 'ZCHAR' => "German: öäüÖÄÜß", 'ZINT1' => 54, 'ZINT2' => 134, 'ZIT4' => 123456, 'ZFLT' => 123456.00, 'ZNUMC' => '12345', 'ZDATE' => '20060709', 'ZTIME' => '200607', 'ZBCD' => '200607.123'),
//                                    array( 'ZCHAR' => "German: öäüÖÄÜß", 'ZINT1' => 54, 'ZINT2' => 134, 'ZIT4' => 123456, 'ZFLT' => 123456.00, 'ZNUMC' => '12345', 'ZDATE' => '20060709', 'ZTIME' => '200607', 'ZBCD' => '200607.123')
                                    )
                               );
                $results = $func->invoke($parms);
//                var_dump($results);
//                echo "ECHAR: ", $results['ECHAR'];
                $this->assertEquals(trim($results['ECHAR']), $parms['CHAR']);
                //echo "INT1: ", $results['EINT1'];
                $this->assertEquals($results['EINT1'], $parms['INT1']);
                //echo "INT2: ", $results['EINT2'];
                $this->assertEquals($results['EINT2'], $parms['INT2']);
                //echo "INT4: ", $results['EINT4'];
                $this->assertEquals($results['EINT4'], $parms['INT4']);
                //echo "FLOAT: ", $results['EFLOAT'];
                $this->assertEquals($results['EFLOAT'], $parms['FLOAT']);
                //echo "NUMC: ", $results['ENUMC'];
                $this->assertEquals($results['ENUMC'], $parms['NUMC']);
                //echo "DATE: ", $results['EDATE'];
                $this->assertEquals($results['EDATE'], $parms['DATE']);
                //echo "TIME: ", $results['ETIME'];
                $this->assertEquals($results['ETIME'], $parms['TIME']);
                //echo "BCD: ", $results['EBCD'];
                $this->assertEquals($results['EBCD'], $parms['BCD']);
                //echo "ESTRUCT: ", $results['ESTRUCT'];
                //echo "DATA: ", $results['DATA'];
                //echo "RESULTS: ", $results['RESULT'];
                //echo "['EXPORT_TABLE'][0]['ZCHAR']: '", $results['EXPORT_TABLE'][0]['ZCHAR'], "'\n";
                //echo "['EXPORT_TABLE'][1]['ZCHAR']: '", $results['EXPORT_TABLE'][1]['ZCHAR'], "'\n";
                $this->assertEquals(chop($results['DATA'][0]['ZCHAR']), $parms['DATA'][0]['ZCHAR']);
                $this->assertEquals(chop($results['DATA'][1]['ZCHAR']), $parms['DATA'][1]['ZCHAR']);
            }
            $this->assertEquals($conn->close(), true);
        } catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }

    public function testFuncChanging1()
    {
        echo "testFuncChanging1\n";
        try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            $func = $conn->function_lookup("STFC_CHANGING");
            $this->assertEquals($func->name, "STFC_CHANGING");
            for ($i=0; $i<100; $i++) {
                $cnt = (int) $i;
                $parms = array('START_VALUE' => $cnt,
                               'COUNTER' => $cnt);
//                echo "start: $cnt count: $cnt \n";
                $results = $func->invoke($parms);
//                var_dump($results);
                $this->assertEquals((int)$results['RESULT'], ($cnt + $cnt));
                $this->assertEquals((int)$results['COUNTER'], ($cnt + 1));
            }
            $this->assertEquals($conn->close(), true);
        } catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
}
