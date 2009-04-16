#!/bin/sh
BASE=/home/piers/workspace/sapnwrfc
MAJ=`grep 'define SAPNWRFC_VERSION_MAJOR' sapnwrfc.c | awk '{print $3}'`
MIN=`grep 'define SAPNWRFC_VERSION_MINOR' sapnwrfc.c | perl -ne '@x=split(/\s+/, $_);print sprintf("%02d",pop(@x));'`
VERS=$MAJ.$MIN
echo "building version $VERS"
DIST=php-sapnwrfc-$VERS
BALL=$DIST.tar.gz
ZIP=$DIST.zip

./clean.sh

if [ -d $DIST ]; then
  echo "removing: $DIST ..."
  rm -rf $DIST
fi

if [ -f $BALL ]; then
  echo "removing: $BALL ..."
  rm -f $BALL
fi

if [ -f $ZIP ]; then
  echo "removing: $ZIP ..."
  rm -f $ZIP
fi

echo "setting up the distribution foot print $DIST ..."
mkdir -p $DIST
for i in build.sh clean.sh CREDITS INSTALL php_sapnwrfc.h sapnwrfc.c tools/* config.m4 examples/* sap_config.php tests.sh unit_tests/* ChangeLog
do
  DIR=`perl -e '$ARGV[0] =~ s/^(.*)\/.*?$/$1/; print $ARGV[0]' $i`
  if [ -d $DIR ]; then
    echo "making dir: $DIR"
    mkdir -p $DIST/$DIR
  fi
  echo "copy $BASE/$i to $DIST/$i ..."
	if [ -f $BASE/$i ]; then
	  echo "$BASE/$i exists..."
	else
	  if [ -d $BASE/$i ]; then
	    echo "$BASE/$i exists..."
		else
	    echo "$BASE/$i IS MISSING !!!"
		  exit 1
		fi
	fi
    #echo "copying $i to $DIST/$i"
  cp -a $i $DIST/$i
done

echo "make tar ball: $BALL"
tar -czvf $BALL $DIST
ls -l $BALL

echo "make zip: $ZIP"
zip -r $ZIP $DIST
ls -l $ZIP

if [ -d $DIST ]; then
  echo "removing: $DIST ..."
  rm -rf $DIST
fi
echo "Done."

echo "Copy up distribution"
scp $BALL ompka.net:www/download/php/sapnwrfc/
scp $ZIP ompka.net:www/download/php/sapnwrfc/
