FUNCTION Z_TEST_DATA.
*"----------------------------------------------------------------------
*"*"Local Interface:
*"  IMPORTING
*"     VALUE(INT1) LIKE  ZTABLE-ZINT1 OPTIONAL
*"     VALUE(INT2) LIKE  ZTABLE-ZINT2 OPTIONAL
*"     VALUE(INT4) LIKE  ZTABLE-ZINT4 OPTIONAL
*"     VALUE(FLOAT) LIKE  ZTABLE-ZFLT OPTIONAL
*"     VALUE(CHAR) LIKE  ZTABLE-ZCHAR OPTIONAL
*"     VALUE(DATE) LIKE  ZTABLE-ZDATE OPTIONAL
*"     VALUE(TIME) LIKE  ZTABLE-ZTIME OPTIONAL
*"     VALUE(NUMC) LIKE  ZTABLE-ZNUMC OPTIONAL
*"     VALUE(BCD) LIKE  ZTABLE-ZBCD OPTIONAL
*"     VALUE(ISTRUCT) LIKE  ZTABLE STRUCTURE  ZTABLE OPTIONAL
*"     VALUE(IMPORT_TABLE) TYPE  ZTABLET OPTIONAL
*"  EXPORTING
*"     VALUE(EINT1) LIKE  ZTABLE-ZINT1
*"     VALUE(EINT2) LIKE  ZTABLE-ZINT2
*"     VALUE(EINT4) LIKE  ZTABLE-ZINT4
*"     VALUE(EFLOAT) LIKE  ZTABLE-ZFLT
*"     VALUE(ECHAR) LIKE  ZTABLE-ZCHAR
*"     VALUE(EDATE) LIKE  ZTABLE-ZDATE
*"     VALUE(ETIME) LIKE  ZTABLE-ZTIME
*"     VALUE(ENUMC) LIKE  ZTABLE-ZNUMC
*"     VALUE(EBCD) LIKE  ZTABLE-ZBCD
*"     VALUE(ESTRUCT) LIKE  ZTABLE STRUCTURE  ZTABLE
*"     VALUE(EXPORT_TABLE) TYPE  ZTABLET
*"  TABLES
*"      DATA STRUCTURE  ZTABLE OPTIONAL
*"      RESULT STRUCTURE  TAB512 OPTIONAL
*"----------------------------------------------------------------------


data: val(50) type c.

data: begin of out,
   fld(20),
   val(256),
      end of out.


refresh result.

if int1 is not initial.
   write int1 to out-val left-justified.
   out-fld = 'INT1'.
   append out to result.
   eint1 = int1.
endif.

if int2 is not initial.
   write int2 to out-val left-justified.
   out-fld = 'INT2'.
   append out to result.
   eint2 = int2.
endif.

if int4 is not initial.
   write int4 to out-val left-justified.
   out-fld = 'INT4'.
   append out to result.
   eint4 = int4.
endif.

if float is not initial.
   write float to out-val left-justified.
   out-fld = 'FLOAT'.
   append out to result.
   efloat = float.
endif.


if char is not initial.
   write char to out-val.
   out-fld = 'CHAR'.
   append out to result.
   echar = char.
endif.

if date is not initial.
   write date to out-val using edit mask '__/__/____'.
   out-fld = 'DATE'.
   append out to result.
   edate = date.
endif.

if time is not initial.
   write time to out-val using edit mask '__:__:__'.
   out-fld = 'TIME'.
   append out to result.
   etime = time.
endif.

if numc is not initial.
   write numc to out-val left-justified.
   out-fld = 'NUMC'.
   append out to result.
   enumc = numc.
endif.

if bcd is not initial.
   write bcd to out-val left-justified.
   out-fld = 'BCD'.
   append out to result.
   ebcd = bcd.
endif.

result = ''.
append result.
result = 'START-OF-ISTRUCT-DATA'.
append result.
  if istruct-zint1 is not initial.
     write istruct-zint1 to out-val left-justified.
     out-fld = 'INT1'.
     append out to result.
  endif.

  if istruct-zint2 is not initial.
     write istruct-zint2 to out-val left-justified.
     out-fld = 'INT2'.
     append out to result.
  endif.

  if istruct-zint4 is not initial.
     write istruct-zint4 to out-val left-justified.
     out-fld = 'INT4'.
     append out to result.
  endif.

  if istruct-zflt is not initial.
     write istruct-zflt to out-val left-justified.
     out-fld = 'FLOAT'.
     append out to result.
  endif.


  if istruct-zchar is not initial.
     write istruct-zchar to out-val.
     out-fld = 'CHAR'.
     append out to result.
  endif.

  if istruct-zdate is not initial.
     write istruct-zdate to out-val using edit mask '__/__/____'.
     out-fld = 'DATE'.
     append out to result.
  endif.

  if istruct-ztime is not initial.
     write istruct-ztime to out-val using edit mask '__:__:__'.
     out-fld = 'TIME'.
     append out to result.
  endif.

  if istruct-znumc is not initial.
     write istruct-znumc to out-val left-justified.
     out-fld = 'NUMC'.
     append out to result.
  endif.

  if istruct-zbcd is not initial.
     write istruct-zbcd to out-val left-justified.
     out-fld = 'BCD'.
     append out to result.
  endif.
  result = 'end-of-istruct-line'.
  append result.

result = ''.
append result.
result = 'START-OF-TABLE-DATA'.
append result.

loop at data.
  if data-zint1 is not initial.
     write data-zint1 to out-val left-justified.
     out-fld = 'INT1'.
     append out to result.
  endif.

  if data-zint2 is not initial.
     write data-zint2 to out-val left-justified.
     out-fld = 'INT2'.
     append out to result.
  endif.

  if data-zint4 is not initial.
     write data-zint4 to out-val left-justified.
     out-fld = 'INT4'.
     append out to result.
  endif.

  if data-zflt is not initial.
     write data-zflt to out-val left-justified.
     out-fld = 'FLOAT'.
     append out to result.
  endif.


  if data-zchar is not initial.
     write data-zchar to out-val.
     out-fld = 'CHAR'.
     append out to result.
  endif.

  if data-zdate is not initial.
     write data-zdate to out-val using edit mask '__/__/____'.
     out-fld = 'DATE'.
     append out to result.
  endif.

  if data-ztime is not initial.
     write data-ztime to out-val using edit mask '__:__:__'.
     out-fld = 'TIME'.
     append out to result.
  endif.

  if data-znumc is not initial.
     write data-znumc to out-val left-justified.
     out-fld = 'NUMC'.
     append out to result.
  endif.

  if data-zbcd is not initial.
     write data-zbcd to out-val left-justified.
     out-fld = 'BCD'.
     append out to result.
  endif.
  result = 'end-of-one-line'.
  append result.
endloop.

clear data.
data-zchar = 'another line'.
append data.

export_table[] = import_table[].

move-corresponding: istruct to estruct.


ENDFUNCTION.