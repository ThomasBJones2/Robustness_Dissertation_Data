set terminal postscript eps color "Helvetica" 20
set out 'RBS.eps'
set title "Controlled Fault Modified Computation WC-Fragility* Search"
set xlabel "Input Size"
set ylabel "WC-Fragility*"
plot "data.dat" using 1:2 title "Full Bubble Sort" w l, \
"data.dat" using 1:3 title "Merge Sort" w l, \
"data.dat" using 1:4 title "Comparison Counting Sort" w l
