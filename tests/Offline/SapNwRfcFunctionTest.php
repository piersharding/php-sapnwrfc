<?php
namespace SapNwRfc\Offline;

use PHPUnit_Framework_TestCase;
use sapnwrfc_function;

class SapNwRfcFunctionTest extends PHPUnit_Framework_TestCase
{

    public function testConstruct()
    {
        $func = new sapnwrfc_function();
        
        $this->assertInstanceOf('sapnwrfc_function', $func);
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc_function::invoke() expects exactly 1 parameter, 0 given
     */
    public function testInvokeNoParameterException()
    {
        $func = new sapnwrfc_function();
        
        $func->invoke();
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc_function::invoke() expects parameter 1 to be array, string given
     */
    public function testInvokeWrongParameterTypeException()
    {
        $func = new sapnwrfc_function();
        
        $func->invoke('something');
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc_function::activate() expects exactly 1 parameter, 0 given
     */
    public function testActivateNoParameterException()
    {
        $func = new sapnwrfc_function();
        
        $func->activate();
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc_function::activate() expects parameter 1 to be string, array given
     */
    public function testActivateWrongParameterTypeException()
    {
        $func = new sapnwrfc_function();
        
        $func->activate([]);
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc_function::deactivate() expects exactly 1 parameter, 0 given
     */
    public function testDeactivateNoParameterException()
    {
        $func = new sapnwrfc_function();
        
        $func->deactivate();
    }

    /**
     * @expectedException \Exception
     * @expectedExceptionMessage sapnwrfc_function::deactivate() expects parameter 1 to be string, array given
     */
    public function testDeactivateWrongParameterTypeException()
    {
        $func = new sapnwrfc_function();
        
        $func->deactivate([]);
    }
}
