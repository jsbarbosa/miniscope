useless = *.aux *.log *.out *.xml *.gz *.tdo *.thm *.toc *.bbl *.blg *.bib

pdf : master.tex
	pdflatex $<

clean :
	rm $(useless)
