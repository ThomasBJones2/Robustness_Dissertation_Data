all:	EG RBS.plt
	gnuplot RBS.plt
	epstopdf RBS.eps

run:	EG
	./EG >data.dat

RBS:
	gnuplot RBS.plt
	epstopdf RBS.eps

EG:	ErrGain.cpp
	g++ ErrGain.cpp -o EG


clean: 
	rm EG
