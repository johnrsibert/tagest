.PHONY: test example.gmt.cst example.gmt gmt2cst

CC=g++

default: all

all: gmt2cst

gmt2cst: src/gmt2cst.cpp src/main.cpp
	$(CC) $^ -Wno-deprecated -I$(JNIGRAPHICS_PATH) -L$(JNIGRAPHICS_PATH) -ljnigraphics3 -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(UNAME_S) -L$(JAVA_HOME)/jre/lib/$(UNAME_M)/server -ljvm -D__int64="long long" -Dlinux -DOPT_LIB -D__GNUDOS__ -I$(ADMB_HOME)/include -L$(ADMB_HOME)/lib -lado -ladt -o gmt2cst

example.gmt:
	PATH=/usr/local/gmt/bin:$(PATH) pscoast -JM5 -R115/195/-20/15 -Dc -W -M > $@

example.gmt.cst: gmt2cst example.gmt
	./gmt2cst example.gmt 115 -20 80 25

test: example.gmt.cst

clean:
	@rm -vf gmt2cst .gmtcommands4 example.gmt .gmtdefaults4 example.gmt.combined example.gmt.cst
