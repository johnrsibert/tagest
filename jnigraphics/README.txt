Installation Procedure

Prerequistes
  * ADMB installed
  * Sun JAVA (version 1.5 or recent) installed

1. Build the jnigraphics library.

   jnigraphics$ ADMB_HOME=admb-home-path make

   If the build was successful, a jnigraphics folder will have been created.

2. Copy the jnigraphics folder to /usr/local

   jnigraphics$ sudo cp -R jnigraphics /usr/local



changes:
	20040525:
		* fixed Makefiles again to make clean everything
		* added jni-invoke-cygwin for howto setup cygwin invoke
		todo:
			[] port to vc++
			[] port to bcc32
	20040524:
		* add cygwin support
		* fixed the compiler errors
		* cleaned up example Makefile and jnigraphics3 Makefile
		todo:
			[20040525] test example for cygwin
	20040503:
		* fixed compiler errors/warnings with gcc-3.2.*
		* cleanout unessary stuff bcc55 and unused
		* moved jcalist.cpp into jcalist.h for better portablity
		* made a patch for logging differences and upgrade

			#!/bin/bash
			#to generate patch
			$ diff -wrc ../jnigraphics3-20030810/ . > jnigraphics3-20030810.patch
			#go to old directory
			$ cd jnigraphics3-20030810
			#patch it
			$ patch -p0 < jnigraphics3-20030810.patch

		todo:
			[] fix gmt2cst

	20040504:
		* fixed makefile for different os compiles
			- must be careful not to reassgin variables
			eq
				JAVA_HOME=/usr/local/java2 
				ifeq ($(strip $(JAVA_HOME)),)
				JAVA_HOME=/usr/local/java <---produces errors because variable gets reassigned.
				endif
		* removed outputs for R
		* redid patch form 20030810 instead from 20040503 since small changes occurred
		* DON'T FORGET to dos2unix files before applying patches
