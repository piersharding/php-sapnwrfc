<?php
namespace SapNwRfc\Offline;

use PHPUnit_Framework_TestCase;
use sapnwrfc;

class SapNwRfcTest extends PHPUnit_Framework_TestCase
{

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc::__construct() expects exactly 1 parameter, 0 given
     */
    public function testConstructNoParameterException()
    {
        $conn = new sapnwrfc();
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc::__construct() expects parameter 1 to be array, string given
     */
    public function testConstructWrongParameterTypeException()
    {
        $conn = new sapnwrfc('test');
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage Parameter ASHOST, GWHOST or MSHOST is missing
     */
    public function testConstructWrongParameterArrayException()
    {
        $conn = new sapnwrfc([
            'something' => 123
        ]);
    }
}
