ifndef ADMB_HOME
$(error "Error: System variable ADMB_HOME is not defined.")
endif
ifndef JAVA_HOME
$(error "Error: System variable JAVA_HOME is not defined.")
endif

all:
	g++ example.cpp -Wno-deprecated -I../../jnigraphics -L../../jnigraphics -ljnigraphics3 -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(UNAME_S) -L$(JAVA_HOME)/jre/lib/$(UNAME_M)/server -ljvm -D__int64="long long" -Dlinux -DOPT_LIB -D__GNUDOS__ -I$(ADMB_HOME)/include -L$(ADMB_HOME)/lib -lado -ladt -o example

run:
	-JNIGRAPHICS_PATH=../../jnigraphics LD_LIBRARY_PATH=$(JAVA_HOME)/jre/lib/amd64/server:$(LD_LIBRARY_PATH) ./example

run-macos:
	rm -f test.png
	-JNIGRAPHICS_PATH=/Users/johnoel/Projects/jnigraphics/jnigraphics DYLD_LIBRARY_PATH=$(JAVA_HOME)/jre/lib/amd64:$(JAVA_HOME)/jre/lib/amd64/server:$(DYLD_LIBRARY_PATH) ./example

clean:
	@rm -vf test.png
	@rm -vf example
