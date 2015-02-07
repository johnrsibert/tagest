all:
	/usr/local/gcc-3.2.3/bin/g++ admodel.cpp -Wno-deprecated -DOPT_LIB -D__GNUDOS__ -I/usr/local/admodel/include -L/usr/local/admodel/lib -lado -ladt -o admodel
