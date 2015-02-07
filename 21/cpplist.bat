cpp32 -P- -I. -DOPT_LIB  %1.cpp
sed -f cpp.sed %1.i > %1.ii
c:\apps2\cdoc\cdocw32 @clist.cfg -L3 -s2 -z1 -k1 -~k0 -~i0 %1.ii
