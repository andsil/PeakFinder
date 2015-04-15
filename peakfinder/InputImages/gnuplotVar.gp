set terminal png transparent nocrop enhanced size 1920,800 font "arial,8" 
# set output 'a.png'
set style fill   solid 1.00 border lt -1
set datafile separator ";"
set autoscale xfix
set xtics auto
set ytics auto
set title "Fourier transform intensities peaks" 
set xrange [ 0.00000 : ]
set xlabel "Distance (px)"
set yrange [ 0.00000 : 7.000 ] noreverse nowriteback
set ylabel "Module sum"
plot "radialVarHisto.csv" every ::1::724 using 5 with boxes