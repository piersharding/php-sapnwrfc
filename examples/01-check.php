<?php
/**
 * Check if you have successfully installed this extension
 */
if (! extension_loaded('sapnwrfc')) {
    throw new \Exception('Extension "sapnwrfc" not loaded. Please see https://github.com/piersharding/php-sapnwrfc#installation');
}

echo 'Your SapNwRfc extension version: ' . sapnwrfc_version() . PHP_EOL;
echo 'Your SapNwRfc version: ' . sapnwrfc_rfcversion() . PHP_EOL;

echo 'Congratulation! Your installation looks good.' . PHP_EOL;
