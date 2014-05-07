#!/bin/sh
make clean
phpize --clean
phpize
rm -f *.i *.ii
#./configure --with-sapnwrfc=/home/piers/code/sap/nwrfcsdk  && make && sudo cp ./modules/sapnwrfc.so /usr/lib/php5/20090626/ &&  php -r 'dl("sapnwrfc.so"); echo sapnwrfc_rfcversion()."\n".sapnwrfc_version()."\n";'
./configure --with-sapnwrfc=/home/piers/code/sap/nwrfcsdk  && make && sudo cp ./modules/sapnwrfc.so /usr/lib/php5/20121212/sapnwrfc.so  &&  php -r 'dl("sapnwrfc.so"); echo sapnwrfc_rfcversion()."\n".sapnwrfc_version()."\n";'
