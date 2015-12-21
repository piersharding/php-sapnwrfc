<?php
namespace SapNwRfc\Online;

require 'AbstractOnlineTestCase.php';

class ConnectionTest extends AbstractOnlineTestCase
{

    public function testConnection()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        // we must have a valid connection
        $this->assertNotNull($conn);
    }

    public function testConnectionAttr()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        // we must have a valid connection
        $this->assertNotNull($conn);
        
        $attr = $conn->connection_attributes();
        
        $this->assertEquals($attr['partnerHost'], 'gecko');
    }

    public function testConnectionAttrLots()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        // we must have a valid connection
        $this->assertNotNull($conn);
        
        for ($i = 0; $i < 100; $i ++) {
            $attr = $conn->connection_attributes();
            $this->assertEquals($attr['partnerHost'], 'gecko');
        }
    }

    public function testConnectionAttrLots2()
    {
        for ($i = 0; $i < 100; $i ++) {
            $conn = new sapnwrfc($this->getConfig());
            
            // we must have a valid connection
            $this->assertNotNull($conn);
            $attr = $conn->connection_attributes();
            
            $this->assertEquals($attr['partnerHost'], 'gecko');
            $this->assertNotNull($conn->close());
        }
    }

    public function testConnectionThrow()
    {
        try {
            $config = $this->getConfig();
            $config['sysnr'] = sprintf('%02d', ((int) $config['sysnr']) + 1);
            $conn = new sapnwrfc($config);
        } catch (sapnwrfcConnectionException $e) {
            echo "Exception type: " . $e . "\n";
            echo "Exception key: " . $e->key . "\n";
            echo "Exception code: " . $e->code . "\n";
            echo "Exception message: " . $e->getMessage();
            $this->assertTrue(TRUE);
        } catch (Exception $e) {
            echo "Exception type: " . $e . "\n";
            echo "Exception key: " . $e->key . "\n";
            echo "Exception code: " . $e->code . "\n";
            echo "Exception message: " . $e->getMessage();
            throw new Exception('Connection failed.');
        }
    }

    public function testConnectionThrow2()
    {
        try {
            $conn = new sapnwrfc($this->getConfig());
            $h = $conn->function_lookup('RFC_READ_TABLE');
            $h->invoke(array(
                'DOES_NOT_EXIST' => 1
            ));
        } catch (sapnwrfcCallException $e) {
            echo "Exception type: " . $e . "\n";
            echo "Exception key: " . $e->key . "\n";
            echo "Exception code: " . $e->code . "\n";
            echo "Exception message: " . $e->getMessage();
            $this->assertTrue(TRUE);
        } catch (Exception $e) {
            echo "Exception type: " . $e . "\n";
            echo "Exception key: " . $e->key . "\n";
            echo "Exception code: " . $e->code . "\n";
            echo "Exception message: " . $e->getMessage();
            throw new Exception('Connection failed.');
        }
    }

    public function testConnectionThrow3()
    {
        try {
            $conn = new sapnwrfc($this->getConfig());
            $h = $conn->function_lookup('RFC_READ_TABLE');
            $h->invoke(array(
                'DATA' => array(
                    array(
                        'NOT_THERE' => 1
                    )
                )
            ));
        } catch (sapnwrfcCallException $e) {
            echo "Exception type: " . $e . "\n";
            echo "Exception key: " . $e->key . "\n";
            echo "Exception code: " . $e->code . "\n";
            echo "Exception message: " . $e->getMessage();
            $this->assertTrue(TRUE);
        } catch (Exception $e) {
            echo "Exception type: " . $e . "\n";
            echo "Exception key: " . $e->key . "\n";
            echo "Exception code: " . $e->code . "\n";
            echo "Exception message: " . $e->getMessage();
            throw new Exception('Connection failed.');
        }
    }

    public function testConnectionCert()
    {
        try {
            $conn = new sapnwrfc($this->getConfig());
            $ticket = $conn->get_sso_ticket();
            echo "ticket: $ticket \n";
        } catch (Exception $e) {
            echo "Exception type: " . $e . "\n";
            echo "Exception key: " . $e->key . "\n";
            echo "Exception code: " . $e->code . "\n";
            echo "Exception message: " . $e->getMessage();
            throw new Exception('Connection failed.');
        }
    }
}
