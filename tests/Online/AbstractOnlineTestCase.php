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
        
        $this->config = file_get_contents('tests/connection.config.php');
    }

    protected function getConfig()
    {
        return $this->config;
    }
}
