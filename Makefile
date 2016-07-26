SRC= $(wildcard *.tex)

TARGPDF=$(SRC:.tex=.pdf)

all: $(TARGPDF)

%.pdf: %.tex
	pdflatex $<
	pdflatex $<

clean:
	rm -f *.out *.aux *.log $(TARGPDF) *.dvi *~

