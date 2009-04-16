dnl config.m4 for extension sapnwrfc
dnl don't forget to call PHP_EXTENSION(sapnwrfc)

dnl
dnl PHP_ADD_SOURCES_X(source-path, sources[, special-flags[, target-var[, shared[, special-post-flags]]]])
dnl
dnl Additional to PHP_ADD_SOURCES (see above), this lets you set the
dnl name of the array target-var directly, as well as whether
dnl shared objects will be built from the sources.
dnl
dnl Should not be used directly.
dnl 
AC_DEFUN([PHP_ADD_SOURCES_X],[
dnl relative to source- or build-directory?
dnl ac_srcdir/ac_bdir include trailing slash


  case $1 in
  ""[)] ac_srcdir="$abs_srcdir/"; unset ac_bdir; ac_inc="-I. -I$abs_srcdir" ;;
  /*[)] ac_srcdir=`echo "$1"|cut -c 2-`"/"; ac_bdir=$ac_srcdir; ac_inc="-I$ac_bdir -I$abs_srcdir/$ac_bdir" ;;
  *[)] ac_srcdir="$abs_srcdir/$1/"; ac_bdir="$1/"; ac_inc="-I$ac_bdir -I$ac_srcdir" ;;
  esac
  
dnl how to build .. shared or static?
  ifelse($5,yes,_PHP_ASSIGN_BUILD_VARS(shared),_PHP_ASSIGN_BUILD_VARS(php))

dnl iterate over the sources
  old_IFS=[$]IFS
  for ac_src in $2; do
  
dnl remove the suffix
      IFS=.
      set $ac_src
      ac_obj=[$]1
      IFS=$old_IFS
      
dnl append to the array which has been dynamically chosen at m4 time
      $4="[$]$4 [$]ac_bdir[$]ac_obj.lo"

      dnl sort out the unix flag
      my_extra_flags=" -D_LARGEFILE_SOURCE -mno-3dnow -fno-strict-aliasing -pipe -fexceptions -funsigned-char -Wall -Wno-uninitialized -Wno-long-long -Wcast-align -DSAPwithUNICODE -DSAPonUNIX -D__NO_MATH_INLINES -fPIC -DSAPwithTHREADS"
      case "$host" in
        *-linux-*)
            my_extra_flags=" -D_LARGEFILE_SOURCE -mno-3dnow -fno-strict-aliasing -pipe -fexceptions -funsigned-char -Wall -Wno-uninitialized -Wno-long-long -Wcast-align -DSAPwithUNICODE -DSAPonUNIX -D__NO_MATH_INLINES -fPIC -DSAPwithTHREADS"
            ;;
        *mswin32*)
            my_extra_flags=" -DWIN32  -D_CONSOLE  -DUNICODE  -D_UNICODE -DSAPwithUNICODE  -DSAPonNT  -DSAP_PLATFORM_MAKENAME=ntintel"
            ;;
        *mingw32*)
            my_extra_flags=" -DWIN32  -D_CONSOLE  -DUNICODE  -D_UNICODE -DSAPwithUNICODE  -DSAPonNT  -DSAP_PLATFORM_MAKENAME=ntintel"
            ;;
      esac	

      my_ac_comp="$b_c_pre $3 $ac_inc $b_c_meta $my_extra_flags -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $6$b_c_post"
      AC_CANONICAL_HOST
      case "$host" in
        *-linux-*)
            shared_c_pre_1='rm -f $<.ii $<.i && $(CC) -E '
            shared_c_pre_2=' $< -o $<.ii && perl ./tools/u16lit.pl -le $<.ii && $(LIBTOOL) --mode=compile $(CC) '
            my_ac_comp="$shared_c_pre_1 $3 $ac_inc $b_c_meta $my_extra_flags $shared_c_pre_2 $3 $ac_inc $b_c_meta $my_extra_flags -c $ac_srcdir$ac_src.i -o $ac_bdir$ac_obj.$b_lo $6$b_c_post"
            ;;
      esac	
dnl choose the right compiler/flags/etc. for the source-file
dnl        *.c[)] ac_comp="$my_ac_comp" ;;
dnl        *.c[)] ac_comp="$b_c_pre $3 $ac_inc $b_c_meta -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $6$b_c_post" ;;
      case $ac_src in
        *.c[)] ac_comp="$my_ac_comp" ;;
        *.s[)] ac_comp="$b_c_pre $3 $ac_inc $b_c_meta -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $6$b_c_post" ;;
        *.S[)] ac_comp="$b_c_pre $3 $ac_inc $b_c_meta -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $6$b_c_post" ;;
        *.cpp|*.cc|*.cxx[)] ac_comp="$b_cxx_pre $3 $ac_inc $b_cxx_meta -c $ac_srcdir$ac_src -o $ac_bdir$ac_obj.$b_lo $6$b_cxx_post" ;;
      esac

dnl create a rule for the object/source combo
    cat >>Makefile.objects<<EOF
$ac_bdir[$]ac_obj.lo: $ac_srcdir[$]ac_src
	$ac_comp
EOF
  done
])



PHP_ARG_WITH(sapnwrfc, for sapnwrfc support,
[  --with-sapnwrfc[=DIR]     Include sapnwrfc support. DIR is the SAP NWRFCSDK 
                          install directory.])


if test "$PHP_SAPNWRFC" != "no"; then
  for i in /usr/sap/nwrfcsdk /usr/local/nwrfcsdk /opt/nwrfcsdk $PHP_SAPNWRFC; do
    if test -f $i/include/sapnwrfc.h; then
      SAPNWRFC_DIR=$i
    fi
  done
  
  if test -z "$SAPNWRFC_DIR"; then
     AC_MSG_ERROR(Please install SAP NWRFCSDK - I cannot find sapnwrfc.h)
  fi      

  PHP_ADD_INCLUDE($SAPNWRFC_DIR/include)
  if test ! -f $SAPNWRFC_DIR/lib/libsapnwrfc.so; then
     AC_MSG_ERROR(Shared RFC Non-Unicode library (file libsapnwrfc.so) missing)
  fi   
  PHP_ADD_LIBRARY_WITH_PATH(sapnwrfc, $SAPNWRFC_DIR/lib, SAPNWRFC_SHARED_LIBADD)

  PHP_ADD_LIBRARY_WITH_PATH(sapucum, $SAPNWRFC_DIR/lib, SAPNWRFC_SHARED_LIBADD)

  PHP_BUILD_SHARED()
  AC_CANONICAL_HOST
  case "$host" in
    *-hp-*)
        PHP_ADD_LIBRARY_WITH_PATH(cl, $SAPNWRFC_DIR/lib, SAPNWRFC_SHARED_LIBADD)
        ;;
  esac	

  PHP_SUBST(SAPNWRFC_SHARED_LIBADD)
  
  dnl PHP_NEW_EXTENSION(sapnwrfc,sapnwrfc.c,$ext_shared)
  PHP_NEW_EXTENSION(sapnwrfc,sapnwrfc.c,$ext_shared)

  PHP_ADD_BUILD_DIR($SAPNWRFC_DIR/lib)
fi
