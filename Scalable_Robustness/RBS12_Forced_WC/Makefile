DESTDIR:=../../paper/images

all:	RBS.pdf

export:	$(DESTDIR)/RBS.pdf

$(DESTDIR)/RBS.pdf:	RBS.pdf
	cp -f RBS.pdf $(DESTDIR)/RBS.pdf

RBS.pdf:	data.dat RBS.plt Makefile
	gnuplot RBS.plt
	epstopdf RBS.eps

data.dat:	EG
	./EG >data.dat

EG:	ErrGain.cpp
	g++ ErrGain.cpp -o EG

clean: 
	rm -f EG data.dat RBS.eps
