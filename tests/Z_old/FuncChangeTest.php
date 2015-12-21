<?php
namespace SapNwRfc\Online;

require 'AbstractOnlineTestCase.php';

class FuncChangeTest extends AbstractOnlineTestCase
{

    static function str_hex($string)
    {
        $hex = '';
        for ($i = 0; $i < strlen($string); $i ++) {
            $hex .= dechex(ord($string[$i]));
        }
        return $hex;
    }

    static function hex_str($hex)
    {
        $string = '';
        for ($i = 0; $i < strlen($hex) - 1; $i += 2) {
            $string .= chr(hexdec($hex[$i] . $hex[$i + 1]));
        }
        return $string;
    }

    public function testFuncDataCall1()
    {
        try {
            $conn = new sapnwrfc($this->getConfig());
            $attr = $conn->connection_attributes();
            // we must have a valid connection
            $this->assertNotNull($conn);
            
            $func = $conn->function_lookup("Z_TEST_DATA");
            $this->assertEquals($func->name, "Z_TEST_DATA");
            $parms = array(
                'CHAR' => "German: öäüÖÄÜß",
                'INT1' => 123,
                'INT2' => 1234,
                'INT4' => 123456,
                'FLOAT' => 123456.00,
                'NUMC' => "12345",
                'DATE' => "20060709",
                'TIME' => "200607",
                'BCD' => '200607.123',
                'ISTRUCT' => array(
                    'ZCHAR' => "German: öäüÖÄÜß",
                    'ZINT1' => 54,
                    'ZINT2' => 134,
                    'ZIT4' => 123456,
                    'ZFLT' => 123456.00,
                    'ZNUMC' => '12345',
                    'ZDATE' => '20060709',
                    'ZTIME' => '200607',
                    'ZBCD' => '200607.123'
                ),
                'DATA' => array(
                    array(
                        'ZCHAR' => "German: öäüÖÄÜß",
                        'ZINT1' => 54,
                        'ZINT2' => 134,
                        'ZIT4' => 123456,
                        'ZFLT' => 123456.00,
                        'ZNUMC' => '12345',
                        'ZDATE' => '20060709',
                        'ZTIME' => '200607',
                        'ZBCD' => '200607.123'
                    ),
                    array(
                        'ZCHAR' => "Slovenian: čćšČĆŠ",
                        'ZINT1' => 54,
                        'ZINT2' => 134,
                        'ZIT4' => 123456,
                        'ZFLT' => 123456.00,
                        'ZNUMC' => '12345',
                        'ZDATE' => '20060709',
                        'ZTIME' => '200607',
                        'ZBCD' => '200607.123'
                    )
                )
            );
            $results = $func->invoke($parms);
            var_dump($results);
            $this->assertEquals(trim($results['ECHAR']), $parms['CHAR']);
            $this->assertEquals($results['EINT1'], $parms['INT1']);
            $this->assertEquals($results['EINT2'], $parms['INT2']);
            $this->assertEquals($results['EINT4'], $parms['INT4']);
            $this->assertEquals($results['EFLOAT'], $parms['FLOAT']);
            $this->assertEquals($results['ENUMC'], $parms['NUMC']);
            $this->assertEquals($results['EDATE'], $parms['DATE']);
            $this->assertEquals($results['ETIME'], $parms['TIME']);
            $this->assertEquals($results['EBCD'], $parms['BCD']);
            $this->assertEquals(chop($results['DATA'][0]['ZCHAR']), $parms['DATA'][0]['ZCHAR']);
            $this->assertEquals(chop($results['DATA'][1]['ZCHAR']), $parms['DATA'][1]['ZCHAR']);
            sleep(10);
            sapnwrfc_removefunction($attr['sysId'], 'Z_TEST_DATA');
            $func = $conn->function_lookup("Z_TEST_DATA");
            $parms = array(
                'ICHAR' => "German: öäüÖÄÜß",
                'INT1' => 123,
                'INT2' => 1234,
                'INT4' => 123456,
                'FLOAT' => 123456.00,
                'NUMC' => "12345",
                'DATE' => "20060709",
                'TIME' => "200607",
                'BCD' => '200607.123',
                'ISTRUCT' => array(
                    'ZCHAR' => "German: öäüÖÄÜß",
                    'ZINT1' => 54,
                    'ZINT2' => 134,
                    'ZIT4' => 123456,
                    'ZFLT' => 123456.00,
                    'ZNUMC' => '12345',
                    'ZDATE' => '20060709',
                    'ZTIME' => '200607',
                    'ZBCD' => '200607.123'
                ),
                'DATA' => array(
                    array(
                        'ZCHAR' => "German: öäüÖÄÜß",
                        'ZINT1' => 54,
                        'ZINT2' => 134,
                        'ZIT4' => 123456,
                        'ZFLT' => 123456.00,
                        'ZNUMC' => '12345',
                        'ZDATE' => '20060709',
                        'ZTIME' => '200607',
                        'ZBCD' => '200607.123'
                    ),
                    array(
                        'ZCHAR' => "Slovenian: čćšČĆŠ",
                        'ZINT1' => 54,
                        'ZINT2' => 134,
                        'ZIT4' => 123456,
                        'ZFLT' => 123456.00,
                        'ZNUMC' => '12345',
                        'ZDATE' => '20060709',
                        'ZTIME' => '200607',
                        'ZBCD' => '200607.123'
                    )
                )
            );
            $results = $func->invoke($parms);
            var_dump($results);
            $this->assertEquals(trim($results['ECHAR']), $parms['CHAR']);
            $this->assertEquals($results['EINT1'], $parms['INT1']);
            $this->assertEquals($results['EINT2'], $parms['INT2']);
            $this->assertEquals($results['EINT4'], $parms['INT4']);
            $this->assertEquals($results['EFLOAT'], $parms['FLOAT']);
            $this->assertEquals($results['ENUMC'], $parms['NUMC']);
            $this->assertEquals($results['EDATE'], $parms['DATE']);
            $this->assertEquals($results['ETIME'], $parms['TIME']);
            $this->assertEquals($results['EBCD'], $parms['BCD']);
            $this->assertEquals(chop($results['DATA'][0]['ZCHAR']), $parms['DATA'][0]['ZCHAR']);
            $this->assertEquals(chop($results['DATA'][1]['ZCHAR']), $parms['DATA'][1]['ZCHAR']);
            $this->assertEquals($conn->close(), true);
        } catch (Exception $e) {
            echo "Exception message: " . $e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
}

?>
