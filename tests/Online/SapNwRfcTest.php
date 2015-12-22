<?php
namespace SapNwRfc\Online;

include_once 'AbstractOnlineTestCase.php';

use sapnwrfc;
use sapnwrfc_function;
use sapnwrfcConnectionException;

class SapNwRfcTest extends AbstractOnlineTestCase
{

    public function testConstruct()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        $this->assertInstanceOf('sapnwrfc', $conn);
    }

    /**
     * @expectedException \sapnwrfcConnectionException
     * @expectedExceptionCode 2
     * @expectedExceptionMessage Name or password is incorrect (repeat logon)
     */
    public function testInvalidUsernameException()
    {
        $config = $this->getConfig();
        if (! isset($config['user'])) {
            $this->markTestSkipped('Only available with "user" connection parameter');
        }
        $config['user'] = 'bla';
        
        $conn = new sapnwrfc($config);
        
        $this->assertInstanceOf('sapnwrfc', $conn);
    }

    /**
     * @expectedException \sapnwrfcConnectionException
     * @expectedExceptionCode 2
     * @expectedExceptionMessage Name or password is incorrect (repeat logon)
     */
    public function testInvalidPasswordException()
    {
        $config = $this->getConfig();
        if (! isset($config['passwd'])) {
            $this->markTestSkipped('Only available with "passwd" connection parameter');
        }
        
        $config['passwd'] = 'bla';
        
        $conn = new sapnwrfc($config);
        
        $this->assertInstanceOf('sapnwrfc', $conn);
    }

    /**
     * @expectedException \sapnwrfcConnectionException
     * @expectedExceptionCode 2
     * @expectedExceptionMessage Client bla is not available in this system
     */
    public function testInvalidClientException()
    {
        $config = $this->getConfig();
        if (! isset($config['CLIENT'])) {
            $this->markTestSkipped('Only available with "CLIENT" connection parameter');
        }
        
        $config['CLIENT'] = 'bla';
        
        $conn = new sapnwrfc($config);
        
        $this->assertInstanceOf('sapnwrfc', $conn);
    }

    /**
     * @expectedException \sapnwrfcConnectionException
     * @expectedExceptionCode 1
     */
    public function testInvalidR3nameException()
    {
        $config = $this->getConfig();
        if (! isset($config['R3NAME'])) {
            $this->markTestSkipped('Only available with "R3NAME" connection parameter');
        }
        
        $config['R3NAME'] = 'bla';
        
        try {
            $conn = new sapnwrfc($config);
        } catch (sapnwrfcConnectionException $ex) {
            $this->assertContains('ERROR', $ex->getMessage());
            $this->assertContains('service \'?\' unknown', $ex->getMessage());
            
            throw $ex;
        }
    }

    /**
     * @expectedException \sapnwrfcConnectionException
     * @expectedExceptionCode 1
     */
    public function testInvalidGroupException()
    {
        $config = $this->getConfig();
        if (! isset($config['GROUP'])) {
            $this->markTestSkipped('Only available with "GROUP" connection parameter');
        }
        
        $config['GROUP'] = 'bla';
        
        try {
            $conn = new sapnwrfc($config);
        } catch (sapnwrfcConnectionException $ex) {
            $this->assertContains('ERROR', $ex->getMessage());
            $this->assertContains('Group bla not found', $ex->getMessage());
            
            throw $ex;
        }
    }

    public function testConnectionAttributes()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        $actual = $conn->connection_attributes();
        
        $this->assertInternalType('array', $actual);
        
        $this->assertArrayHasKey('dest', $actual);
        $this->assertArrayHasKey('host', $actual);
        $this->assertArrayHasKey('partnerHost', $actual);
        $this->assertArrayHasKey('sysNumber', $actual);
        $this->assertArrayHasKey('sysId', $actual);
        $this->assertArrayHasKey('client', $actual);
        $this->assertArrayHasKey('user', $actual);
        $this->assertArrayHasKey('language', $actual);
        $this->assertArrayHasKey('trace', $actual);
        $this->assertArrayHasKey('isoLanguage', $actual);
        $this->assertArrayHasKey('codepage', $actual);
        $this->assertArrayHasKey('partnerCodepage', $actual);
        $this->assertArrayHasKey('rfcRole', $actual);
        $this->assertArrayHasKey('type', $actual);
        $this->assertArrayHasKey('rel', $actual);
        $this->assertArrayHasKey('partnerType', $actual);
        $this->assertArrayHasKey('partnerRel', $actual);
        $this->assertArrayHasKey('kernelRel', $actual);
        $this->assertArrayHasKey('cpicConvId', $actual);
        $this->assertArrayHasKey('progName', $actual);
    }

    public function testClose()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        $actual = $conn->close();
        
        $this->assertTrue($actual);
    }

    public function testPing()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        $actual = $conn->ping();
        
        $this->assertTrue($actual);
    }

    /**
     * @expectedException \sapnwrfcConnectionException
     * @expectedExceptionCode 20
     * @expectedExceptionMessage Invalid parameter 'SAP_UC*' was passed to the API call
     */
    public function testGetSsoTicketException()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        $actual = $conn->get_sso_ticket();
    }

    public function testFunctionLookup()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        $actual = $conn->function_lookup('RFC_PING');
        
        $this->assertInstanceOf('sapnwrfc_function', $actual);
        
        $this->assertEquals('RFC_PING', $actual->name);
    }

    /**
     * @expectedException \sapnwrfcConnectionException
     * @expectedExceptionCode 5
     * @expectedExceptionMessage ID:FL Type:E Number:046 RFC_NOT_EXISTENT
     */
    public function testFunctionLookupNotAvailableException()
    {
        $conn = new sapnwrfc($this->getConfig());
        
        $actual = $conn->function_lookup('RFC_NOT_EXISTENT');
    }
}
