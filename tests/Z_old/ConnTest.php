<?php
#require_once 'PHPUnit/Framework.php';
require_once '../sap_config.php';
require_once 'spyc.php';
dl("sapnwrfc.so");
global $SAP_CONFIG;

class ConnTest extends PHPUnit_Framework_TestCase
{

    protected function setUp()
    {
        global $SAP_CONFIG;
        $this->config = Spyc::YAMLLoad($SAP_CONFIG);
        echo "sapnwrfc version: ".sapnwrfc_version()."\n";
        echo "nw rfc sdk version: ".sapnwrfc_rfcversion()."\n";
    }
    
    public function testConnection()
    {
        try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
        } catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Assertion failed.');
        }
    }
    
    public function testConnectionAttr()
    {
        try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            $attr = $conn->connection_attributes();
            $this->assertEquals($attr['partnerHost'], 'gecko');
        } catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
    
    public function testConnectionAttrLots()
    {
        try {
            $conn = new sapnwrfc($this->config);
            // we must have a valid connection
            $this->assertNotNull($conn);
            for ($i=0; $i<100; $i++) {
                $attr = $conn->connection_attributes();
                $this->assertEquals($attr['partnerHost'], 'gecko');
            }
        } catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
    
    public function testConnectionAttrLots2()
    {
        try {
            for ($i=0; $i<100; $i++) {
                $conn = new sapnwrfc($this->config);
                // we must have a valid connection
                $this->assertNotNull($conn);
                $attr = $conn->connection_attributes();
                $this->assertEquals($attr['partnerHost'], 'gecko');
                $this->assertNotNull($conn->close());
            }
        } catch (Exception $e) {
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
    
    public function testConnectionThrow()
    {
        try {
            $config = $this->config;
            $config['sysnr'] = sprintf('%02d', ((int) $config['sysnr']) + 1);
            $conn = new sapnwrfc($config);
        } catch (sapnwrfcConnectionException $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            $this->assertTrue(true);
        } catch (Exception $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
    
    public function testConnectionThrow2()
    {
        try {
            $conn = new sapnwrfc($this->config);
            $h = $conn->function_lookup('RFC_READ_TABLE');
            $h->invoke(array('DOES_NOT_EXIST' => 1));
        } catch (sapnwrfcCallException $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            $this->assertTrue(true);
        } catch (Exception $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
    
    public function testConnectionThrow3()
    {
        try {
            $conn = new sapnwrfc($this->config);
            $h = $conn->function_lookup('RFC_READ_TABLE');
            $h->invoke(array('DATA' => array(array('NOT_THERE' => 1))));
        } catch (sapnwrfcCallException $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            $this->assertTrue(true);
        } catch (Exception $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
   
    public function testConnectionCert()
    {
        try {
            $conn = new sapnwrfc($this->config);
            $ticket = $conn->get_sso_ticket();
            echo "ticket: $ticket \n";
        } catch (Exception $e) {
            echo "Exception type: ".$e."\n";
            echo "Exception key: ".$e->key."\n";
            echo "Exception code: ".$e->code."\n";
            echo "Exception message: ".$e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
}
