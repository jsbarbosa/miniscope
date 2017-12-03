useless = *.aux *.log *.out *.xml *.gz *.tdo *.thm *.toc

pdf : master.tex
	pdflatex $<
	rm $(useless)

clean :
	rm $(useless)
