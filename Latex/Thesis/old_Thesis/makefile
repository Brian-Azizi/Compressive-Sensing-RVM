X = thesis

mini.pdf : $(X).tex referencesRVM.bib
	pdflatex $(X).tex
	bibtex $(X).aux
	pdflatex $(X).tex
	pdflatex $(X).tex

clean : 
	rm $(X).aux $(X).bbl $(X).blg $(X).log $(X).toc chapter*.aux