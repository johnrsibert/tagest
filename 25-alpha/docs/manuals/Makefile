TagestUserMan.pdf: TagestUserMan.tex TagestUserManText.tex iparname.tex
	pdflatex TagestUserMan
	pdflatex TagestUserMan

iparname.tex: ../../src/iparname.cpp
	awk -f iparname.awk ../../src/iparname.cpp
clean:
	@rm -rfv TagestUserMan.log
	@rm -rfv TagestUserMan.out
	@rm -rfv TagestUserMan.aux
	@rm -rfv TagestUserMan.pdf
	@rm -rfv iparname.tex

