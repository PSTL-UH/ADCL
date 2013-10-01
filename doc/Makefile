
all: adcl-spec adcl-installation-and-usage

LATEX = pdflatex


adcl-spec: adcl-spec.tex title.tex introduction.tex abstract.tex attribute.tex function.tex environment.tex request.tex timer.tex topology.tex vector.tex highlevel.tex 
	$(LATEX) adcl-spec.tex	
	$(LATEX) adcl-spec.tex
	$(LATEX) adcl-spec.tex	


adcl-installation-and-usage: adcl-installation-and-usage.tex installation.tex configuration.tex gui.tex
	$(LATEX) adcl-installation-and-usage.tex
	$(LATEX) adcl-installation-and-usage.tex
	$(LATEX) adcl-installation-and-usage.tex

clean: 
	rm -rf *.aux *.blg *.log *.toc *~ 