.PHONY: test clean

SRC_DIR=src

ifndef ADMB_HOME
$(error "Error: System variable ADMB_HOME is not defined.")
endif
ifndef JAVA_HOME
$(error "Error: System variable JAVA_HOME is not defined.")
endif

DISTRIBUTION=jnigraphics

CC=g++

vpath %.cpp $(SRC_DIR)

ifeq ($(TERM),cygwin)
  OS=cygwin
  CFLAGS=-v -Wall -D__int64="long long" -Dlinux -D__GNUDOS__ -DOPT_LIB -I$(ADMB_HOME)/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/win32 -I$(SRC_DIR) -Wno-deprecated 
#CHECKJAVA=$(shell find $(JAVA_HOME) -follow -name java.exe -mindepth 1)
  TARGET=jvm.dll ppm.dll coastlines.dll libjnigraphics3-cygwin.a jnigraphics3.jar
else #defaults linux
  TARGET2=libppm.so libjnigraphics3-$(OS).a jnigraphics3.jar

  UNAME_S=$(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    TARGET=$(TARGET2) libcoastlines.dylib
    OS=darwin
  else
    TARGET=$(TARGET2) libcoastlines.so
    OS=linux
  endif

  UNAME_M=$(shell uname -m)
  ifeq ($(UNAME_M),x86_64)
    UNAME_M=amd64
  else
    UNAME_M=i386
  endif

  CFLAGS=-Wall -Dlinux -D__GNUDOS__ -DOPT_LIB -I$(ADMB_HOME)/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(OS) -I$(SRC_DIR) -Wno-deprecated
endif

OBJS=jnigraphics3.o fvar.o worms.o

all: build $(DISTRIBUTION) $(TARGET) gmt2cst
	@rm -vf *.o
	@rm -vf *.class
	@cp -vf lib/*.jar jnigraphics
	@echo "Done: cat \"export LD_LIBRARY_PATH=$(JAVA_HOME)/jre/lib/$(UNAME_M)/client:$(JAVA_HOME)/jre/lib/$(UNAME_M):$$LD_LIBRARY_PATH\" >> $(HOME)/.bashrc "

build:
	@test -d $@ || mkdir -v $@

$(DISTRIBUTION):
	@test -d $@ || mkdir -v $@

libppm.so:$(SRC_DIR)/ppm.cpp
	$(CC) $(CFLAGS) -shared -fpic $^ $(PASSLDFLAGS) -o $(DISTRIBUTION)/$@

ppm.dll: $(SRC_DIR)/ppm.cpp
	$(CC) -mno-cygwin $(CFLAGS) -shared $^ -Wl,--add-stdcall-alias -o $(DISTRIBUTION)/$@

libcoastlines.so:$(SRC_DIR)/coastlines.cpp
	$(CC) $(CFLAGS) -shared -fpic $^ $(PASSLDFLAGS) -o $(DISTRIBUTION)/$@

libcoastlines.dylib:$(SRC_DIR)/coastlines.cpp
	$(CC) -dynamiclib -undefined suppress -flat_namespace $(CFLAGS) $(PASSLDFLAGS) $^ -o $(DISTRIBUTION)/$@

coastlines.dll:$(SRC_DIR)/coastlines.cpp
	#solution found at
	#http://forum.java.sun.com/thread.jsp?forum=52&thread=471668
	$(CC) -mno-cygwin $(CFLAGS) -shared $^ -Wl,--add-stdcall-alias -o $(DISTRIBUTION)/$@

libjnigraphics3-$(OS).a: $(OBJS)
	@ar -r $(DISTRIBUTION)/$@ $^
	@cd $(DISTRIBUTION); ln -vfs libjnigraphics3-$(OS).a libjnigraphics3.a
	@cp $(SRC_DIR)/jcalist.h $(DISTRIBUTION)
	@cp $(SRC_DIR)/jnigraphics2.h $(DISTRIBUTION)
	@cp $(SRC_DIR)/jnigraphics3.h $(DISTRIBUTION)
	@cp $(SRC_DIR)/worms.h $(DISTRIBUTION)

jvm.dll:
	echo EXPORTS > jvm.def
	echo "	JNI_CreateJavaVM@12" >> jvm.def
	echo "	fontmanager!_JNI_OnLoad@8" >> jvm.def
	dlltool -k --def jvm.def --dllname jvm.dll  --output-lib libjnigraphics3-cygwin.a
	@rm -vf jvm.def

jnigraphics3.jar:
	CLASSPATH=lib/jai_codec.jar:lib/jai_core.jar:$(CLASSPATH) $(JAVA_HOME)/bin/javac -Xlint:unchecked -Xlint:deprecation -sourcepath $(SRC_DIR)/edu.hawaii.soest.pfrp -d . $(SRC_DIR)/edu.hawaii.soest.pfrp/jnigraphics3.java
	CLASSPATH=lib/jai_codec.jar:lib/jai_core.jar:$(CLASSPATH) $(JAVA_HOME)/bin/javac -sourcepath $(SRC_DIR)/edu.hawaii.soest.pfrp -d . $(SRC_DIR)/edu.hawaii.soest.pfrp/ppm.java
	$(JAVA_HOME)/bin/jar cvf $(DISTRIBUTION)/jnigraphics3.jar *.class

gmt2cst:
	JNIGRAPHICS_PATH=../../$(DISTRIBUTION) UNAME_S=$(UNAME_S) UNAME_M=$(UNAME_M) make --directory src/gmt2cst
	cp src/gmt2cst/gmt2cst $(DISTRIBUTION)

test:
	JNIGRAPHICS_PATH=../../$(DISTRIBUTION) UNAME_S=$(UNAME_S) UNAME_M=$(UNAME_M) make --directory test/example all run-macos

clean:
	@rm -rvf build
	@rm -vf .gmtcommands4
	@rm -rvf $(DISTRIBUTION)
	@make --directory src/gmt2cst clean
	@make --directory test/example clean

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@
