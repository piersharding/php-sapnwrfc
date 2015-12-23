<?php
namespace SapNwRfc\Online;

use PHPUnit_Framework_TestCase;

class AbstractOnlineTestCase extends PHPUnit_Framework_TestCase
{

    private $config;

    protected function setUp()
    {
        if (! file_exists('tests/connection.config.php')) {
            $this->markTestSkipped('Configuration file for a SAP testsystem required "tests/connection.config.php"');
        }
        
        $this->config = include 'tests/connection.config.php';
        
        if (! is_array($this->config)) {
            throw new \Exception('config needs to be an array!');
        }
    }

    protected function getConfig()
    {
        return $this->config;
    }
}
