set terminal postscript eps enhanced color size 3,2.5 "NimbusSanL-Regu,17" fontfile "/usr/share/texmf/fonts/type1/urw/helvetic/uhvr8a.pfb"
set key at 42,52 samplen 2
set out 'RBS.eps'
set title "Forced WC-fragility* Search"
set xlabel "Input size (N)"
set ylabel "WC-fragility*"
set xrange [-2:55]
set yrange [-2:55]
plot "data.dat" using 1:2 title "Full Bubble Sort" w l lw 4, \
"data.dat" using 1:3 title "Merge Sort" w l lw 4, \
"data.dat" using 1:4 title "Comparison Counting Sort" w l lw 4
