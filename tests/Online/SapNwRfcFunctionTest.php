<?php
namespace SapNwRfc\Online;

require_once 'AbstractOnlineTestCase.php';

use sapnwrfc;
use sapnwrfc_function;
use sapnwrfcConnectionException;

class SapNwRfcFunctionTest extends AbstractOnlineTestCase
{

    private $conn;

    protected function setUp()
    {
        parent::setUp();
        
        try {
            $this->conn = new sapnwrfc($this->getConfig());
        } catch (\Exception $ex) {
            $this->markTestSkipped('Valid connection is required');
        }
    }

    /**
     *
     * @return sapnwrfc
     */
    protected function getConnection()
    {
        return $this->conn;
    }

    public function testInvoke()
    {
        $conn = $this->getConnection();
        
        $func = $conn->function_lookup('RFC_PING');
        
        $actual = $func->invoke([]);
        
        $this->assertInternalType('array', $actual);
        $this->assertCount(0, $actual);
    }

    /**
     * @expectedException \sapnwrfcCallException
     * @expectedExceptionCode 20
     * @expectedExceptionMessage field 'something' not found
     */
    public function testInvokeUnknownParameterException()
    {
        $conn = $this->getConnection();
        
        $func = $conn->function_lookup('RFC_PING');
        
        $actual = $func->invoke([
            'something' => 123
        ]);
    }

    /**
     * @expectedException \sapnwrfcCallException
     * @expectedExceptionCode 20
     * @expectedExceptionMessage field 'asdf' not found
     */
    public function testActivateUnknownParameterException()
    {
        $conn = $this->getConnection();
        
        $func = $conn->function_lookup('RFC_PING');
        
        $actual = $func->activate('asdf');
    }

    /**
     * @expectedException \sapnwrfcCallException
     * @expectedExceptionCode 20
     * @expectedExceptionMessage field 'asdf2' not found
     */
    public function testDeactivateUnknownParameterException()
    {
        $conn = $this->getConnection();
        
        $func = $conn->function_lookup('RFC_PING');
        
        $actual = $func->deactivate('asdf2');
    }
}
