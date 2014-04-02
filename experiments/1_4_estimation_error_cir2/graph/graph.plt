set terminal post eps;

##########################################################
reset
set output "error_.eps";
set bmargin 4
set key box spacing 2
set key width 8


set xlabel "xlabel";
set ylabel "ylabel";
set pointsize 2

#set xrange[3:20];

plot \
				'<head -18 ../result/error_ref.txt' using 1:2 title 'naive nv=3' w lp, \
				'' using 1:3 title 'naive nv=4' w lp, \
				'' using 1:4 title 'naive nv=5' w lp, \
				'<head -18 ../result/error.txt' using 1:2 title 'nv=3' w lp ,\
				'' using 1:3 title 'nv=4' w lp ,\
				'' using 1:4 title 'nv=5' w lp;			

##########################################################
reset
unset size

set output "fail_.eps";

set style histogram clustered gap 1
set ylabel 'Failure rate of measurement reception'
set xlabel 'Number of beacons'
set key box spacing 5
set key width 6
set bmargin 4 

#set xrange[-1:17.8]

plot \
'<head -18 ../result/fail3.txt' using 2:xticlabel(1) t 'nv=3' w histogram fs pattern 3 ls 1,\
'' using 3:xticlabel(1) t 'nv=4' w histogram fs pattern 4 ls 1,\
'' using 4:xticlabel(1) t 'nv=5' w histogram fs pattern 0 ls 1,\
'' using 2:xticlabel(1) notitle w lines ls 4, \
'' using 3:xticlabel(1) notitle w lines ls 4, \
'' using 4:xticlabel(1) notitle w lines ls 4;


##########################################################
reset
set output "error.eps";
set bmargin 4
set key box spacing 2
set key width 8


set xlabel "xlabel";
set ylabel "ylabel";
set pointsize 2

plot \
				'../result/error_ref.txt' using 1:2 title 'naive nv=3' w lp, \
				'' using 1:3 title 'naive nv=4' w lp, \
				'' using 1:4 title 'naive nv=5' w lp, \
				'../result/error.txt' using 1:2 title 'nv=3' w lp ,\
				'' using 1:3 title 'nv=4' w lp ,\
				'' using 1:4 title 'nv=5' w lp;			

##########################################################
reset
set output "error2.eps";
set bmargin 4
set key box spacing 2
set key width 8


set xlabel "xlabel";
set ylabel "ylabel";
set pointsize 2

plot \
				'../result/error2_ref.txt' using 1:2 title 'naive nv=3' w lp, \
				'' using 1:3 title 'naive nv=4' w lp, \
				'' using 1:4 title 'naive nv=5' w lp, \
				'../result/error2.txt' using 1:2 title 'nv=3' w lp ,\
				'' using 1:3 title 'nv=4' w lp ,\
				'' using 1:4 title 'nv=5' w lp;			
##########################################################
reset
set output "error3.eps";
set bmargin 4
set key box spacing 2
set key width 8


set xlabel "xlabel";
set ylabel "ylabel";
set pointsize 2

plot \
				'../result/error3_ref.txt' using 1:2 title 'naive nv=3' w lp, \
				'' using 1:3 title 'naive nv=4' w lp, \
				'' using 1:4 title 'naive nv=5' w lp, \
				'../result/error3.txt' using 1:2 title 'nv=3' w lp ,\
				'' using 1:3 title 'nv=4' w lp ,\
				'' using 1:4 title 'nv=5' w lp;			
##########################################################
#
##########################################################
# integrated graph of ../result/estimation error
#set output "err_nbnv.eps";
#set size noratio 1,1.2
#set bmargin 4
#set key box spacing 2
#set key horizontal
#set key outside left top
#set key width 8
#set key tmargin


#set xrange[0:1200];
#set xlabel "xlabel";
#set ylabel "ylabel";
#set pointsize 2
#set yrange [0:900]
#unset key;

#plot \
#				'../result/estimation_error_nv3.txt' using 1:3 title 'naive nv=3' w lp, \
#				'../result/estimation_error_nv4.txt' using 1:3 title 'naive nv=4' w lp, \
#				'../result/estimation_error_nv5.txt' using 1:3 title 'naive nv=5' w lp, \
#				'../result/estimation_error_nv3.txt' using 1:2 title 'nv=3' w lp ,\
#				'../result/estimation_error_nv4.txt' using 1:2 title 'nv=4' w lp ,\
#				'../result/estimation_error_nv5.txt' using 1:2 title 'nv=5' w lp;			

##########################################################
				
##########################################################
reset

set output "overhead.eps";

#set style histogram clustered gap 2
set ylabel 'ylabel2'
set xlabel 'xlabel'
set key box spacing 5
set key width 6
set bmargin 4

#set xrange[0:9]



plot \
'../result/reflection.txt' using 2:xticlabel(1) t 'nv=3' w histogram fs pattern 0,\
'' using 3:xticlabel(1) t 'nv=4' w histogram fs pattern 1,\
'' using 4:xticlabel(1) t 'nv=5' w histogram fs pattern 2,\
794	t 'critical_point' w l lw 5;


##########################################################
reset

set output "overhead_2.eps";
set multiplot
y1=0.0;y2=2000.0;y3=7600;y4=7800;
size = 0.75;
gap = 0.05;

#set style histogram clustered gap 2
set ylabel 'ylabel2'
set xlabel 'xlabel'
set key box spacing 5
set key width 6
set bmargin 4

#set xrange[0:9]


set yrange[y1:y2]
set tmargin at screen size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3)));

plot \
'../result/reflection.txt' using 2:xticlabel(1) t 'nv=3' w histogram fs pattern 0,\
'../result/reflection.txt' using 3:xticlabel(1) t 'nv=4' w histogram fs pattern 1,\
'../result/reflection.txt' using 4:xticlabel(1) t 'nv=5' w histogram fs pattern 2,\
 216 t 'worst_case nv=3',\
 1296 t 'worst_case nv=4';

unset xtics
unset xlabel
unset key
set yrange [y3:y4]
set bmargin at screen size* (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3))) + gap;
set tmargin at screen size + gap;

plot\
 7776 t 'worst_case nv=5';
unset multiplot

##########################################################
reset

set output "reflection_util.eps";
set xlabel "xlabel"
set ylabel "ylabel"
set key box spacing 5;
set key width 6
set bmargin 4

plot \
'../result/reflection.txt' using 2:xticlabel(1) t 'nv=3' w histo fs pattern 0,\
'../result/reflection.txt' using 3:xticlabel(1) t 'nv=4' w histo fs pattern 1,\
'../result/reflection.txt' using 4:xticlabel(1) t 'nv=5' w histo fs pattern 2;


#set style data histogram
#set style histogram cluster
#set style fill solid 1.0 border lt -1
#plot for [COL=2:3] 'avg_err.txt' using COL:xticlabels(1)
##########################################################
reset

set output "reflection_util2.eps";
set xlabel "xlabel"
set ylabel "ylabel"
set key outside
#set key box spacing 5;
#set key width 6
#set bmargin 4
set style histogram rowstacked

plot \
'../result/reflection.txt' using 2:xticlabel(1) t 'nv=3' w histo fs pattern 0,\
'../result/reflection.txt' using 3:xticlabel(1) t 'nv=4' w histo fs pattern 1,\
'../result/reflection.txt' using 4:xticlabel(1) t 'nv=5' w histo fs pattern 2;


#set style data histogram
#set style histogram cluster
#set style fill solid 1.0 border lt -1
#plot for [COL=2:3] 'avg_err.txt' using COL:xticlabels(1)


##########################################################
reset
unset size

set output "fail.eps";

set style histogram clustered gap 1
set ylabel 'Failure rate of measurement reception'
set xlabel 'Number of beacons'
set key box spacing 5
set key width 6
set bmargin 4 

#set xrange[-1:18]



plot \
'../result/fail3.txt' using 2:xticlabel(1) t 'nv=3' w histogram fs pattern 3 ls 1,\
'' using 3:xticlabel(1) t 'nv=4' w histogram fs pattern 4 ls 1,\
'' using 4:xticlabel(1) t 'nv=5' w histogram fs pattern 0 ls 1,\
'' using 2:xticlabel(1) notitle w lines ls 4, \
'' using 3:xticlabel(1) notitle w lines ls 4, \
'' using 4:xticlabel(1) notitle w lines ls 4;

