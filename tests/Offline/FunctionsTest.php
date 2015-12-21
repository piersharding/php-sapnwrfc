<?php
namespace SapNwRfc\Offline;

use PHPUnit_Framework_TestCase;

class FunctionsTest extends PHPUnit_Framework_TestCase
{

    public function testVersion()
    {
        $this->assertInternalType('string', sapnwrfc_version());
    }

    public function testVersionArray()
    {
        $actual = sapnwrfc_version_array();
        
        $this->assertInternalType('array', $actual);
        $this->assertArrayHasKey('major', $actual);
        $this->assertArrayHasKey('minor', $actual);
        $this->assertArrayHasKey('cvs', $actual);
        $this->assertArrayHasKey('ver', $actual);
    }

    public function testRfcVersion()
    {
        $this->assertInternalType('string', sapnwrfc_rfcversion());
    }

    /**
     * Empty ini path
     *
     * @expectedException \Exception
     */
    public function testIniPathException()
    {
        $actual = sapnwrfc_setinipath();
    }

    /**
     * Empty ini path
     */
    public function testIniPath()
    {
        $actual = sapnwrfc_setinipath('.tmp');
        
        $this->assertTrue($actual);
    }

    public function testReloadInifile()
    {
        $actual = sapnwrfc_reloadinifile();
        
        $this->assertTrue($actual);
    }

    /**
     * Missing parameter
     *
     * @expectedException \Exception
     */
    public function testRemoveFunctionNoParameterException()
    {
        $actual = sapnwrfc_removefunction('');
    }

    /**
     * Missing parameter
     *
     * @expectedException \Exception
     */
    public function testRemoveFunctionOnlyOneParameterException()
    {
        $actual = sapnwrfc_removefunction('');
    }

    /**
     * Missing parameter
     */
    public function testRemoveFunction()
    {
        $actual = sapnwrfc_removefunction('', 'RFC_READ_REPORT');
        
        $this->assertTrue($actual);
    }
}
