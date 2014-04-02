set terminal post eps;


set style histogram clustered gap 5 
set xlabel "xlabel"
set ylabel "ylabel"
set y2label "y2label"
set bmargin 4 
set key box spacing 5
set key width 6
set y2range[0:500];
set yrange[0:1.1];
set xrange[0:1600];
set y2tics
set ytics nomirror

set output "dist_limit.eps";

plot "../result/fail.txt" u 1:2 w boxes fs solid 0.25 lt 1 axes x1y1 not, \
	 "../result/error_ref.txt" u 1:2 w lp axes x1y2 t 'prev', \
	 "../result/error.txt" u 1:2 w lp axes x1y2 t 'ours'



