<?php
require_once 'PHPUnit/Framework.php';
require_once '../sap_config.php';
require_once 'spyc.php';
dl("sapnwrfc.so");
global $SAP_CONFIG;

class SSOTest extends PHPUnit_Framework_TestCase
{

    protected function setUp() {
        global $SAP_CONFIG;
        $this->config = Spyc::YAMLLoad($SAP_CONFIG);
        echo "sapnwrfc version: ".sapnwrfc_version()."\n";
        echo "nw rfc sdk version: ".sapnwrfc_rfcversion()."\n";
    }
    
    public function testConnection() {
       try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
        }
        catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    public function testConnectionSSO() {
       try {
            unset($this->config['user']);
            unset($this->config['passwd']);
           $this->config['x509cert'] = "MIIB0TCCAToCByAJAgYAGBYwDQYJKoZIhvcNAQEFBQAwLjELMAkGA1UEBhMCTloxETAPBgNVBAoTCEtFUklLRVJJMQwwCgYDVQQDEwNSRkMwHhcNMDkwMjA2MDAxODE2WhcNMzgwMTAxMDAwMDAxWjAuMQswCQYDVQQGEwJOWjERMA8GA1UEChMIS0VSSUtFUkkxDDAKBgNVBAMTA1JGQzCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA/+55h8dAKzW9YNwV2Bv4sf1aLjOdyQNFLOXJneTrmT9ALbBh5QDnAH/A1YBwpMmlmmEzmrSSd3wpdqz8d82hWECPd1t3Co3EkSZVvBMWeTk6vvQSYnXYeMOZqDzl2YZ/mARLBrou/nKQJHEG+ImjlPo4VUltJULNv3C/9sBvBcsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQD4WB1CEY9nCYTozVnVRH0+jIQA4tqLvLUq6X83boOrhPqTHYlpWMABe+azsf0UT22UsZLgJQzO+d+1pQE55ZfVr5kaJnc79riD7dvlUm8goeaFnFS65p6fjQ/DWHE+60+ET99ibKP7tOuIq757AV1YwB+AhwEmE6597ylDKLexfg";
           $this->config['snc_mode'] = 1;
           $this->config['snc_qop'] = 3;
           $this->config['snc_myname'] = "p:CN=RFC, O=KERIKERI, C=NZ";
           $this->config['snc_partnername'] = "p:CN=N4S, O=KERIKERI, C=NZ";
           $this->config['snc_lib'] = "/home/piers/code/sec/libsapcrypto.so";
           // var_dump($this->config);
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            $attr = $conn->connection_attributes();
            $this->assertEquals($attr['partnerHost'], 'gecko');
            $fd = $conn->function_lookup("SUSR_CHECK_LOGON_DATA");
            //$res = $fd->invoke(array( 'AUTH_METHOD' => "E", 'AUTH_DATA' => "p:ompka\\piers", 'EXTID_TYPE' => "NT"));
            $res = $fd->invoke(array( 'AUTH_METHOD' => "E", 'AUTH_DATA' => "SME\\PIERS", 'EXTID_TYPE' => "NT"));
            var_dump($res);
            $ticket = $res['TICKET'];
            unset($this->config['x509cert']);
            unset($this->config['snc_mode']);
            unset($this->config['snc_qop']);
            unset($this->config['snc_myname']);
            unset($this->config['snc_partnername']);
            unset($this->config['snc_lib']);
            $this->config['mysapsso2'] = $ticket;
            //var_dump($this->config);
            $conn2 = new sapnwrfc($this->config);
        }
        catch (sapnwrfcConnectionException $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            $this->assertTrue(TRUE);
        }
        catch (Exception $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
    
}
