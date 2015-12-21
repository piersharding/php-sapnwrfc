#!/bin/sh
valgrind \
   --tool=memcheck \
   --leak-check=yes \
   --error-limit=no \
   --suppressions=/usr/lib/valgrind/python.supp \
   --num-callers=10 \
   -v \
   phpunit $@
