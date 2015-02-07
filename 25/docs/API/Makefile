.PHONY: default

default: api

all: movemod.html api


movemod.html: movemod.xml movemod.css
#	xsltproc --stringparam html.stylesheet movemod.css /usr/local/docbook/html/docbook.xsl movemod.xml > $@
	xsltproc --stringparam html.stylesheet movemod.css /usr/share/xml/docbook/stylesheet/docbook-xsl/html/docbook.xsl movemod.xml > $@

api: Doxyfile
	(cat Doxyfile ; echo -n 'PROJECT_NUMBER = ';svnversion .. ) | doxygen -
	firefox ./dox/html/index.html&


clean:
	@rm -vrf dox
	@rm -vf movemod.html
