#!/bin/sh
cd unit_tests
phpunit "ConnTest"
##../valgrind_phpunit.sh "FuncTest"
phpunit "FuncDescTest"
phpunit "FuncTest"
phpunit "FuncDataTest"

