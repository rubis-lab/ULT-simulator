set term post eps enhanced;

set output "map.eps";
set size ratio -1
set xtic 100
set ytic 100


awk_plane="<gawk -F '[,#O()]+' '{print $2\" \"$3;print $5\" \"$6;print $8\" \"$9;print $11\" \"$12;print $2\" \"$3;print \" \"}' plane.auto.conf"
awk_beacon="<gawk -F '[,#\t;]+' '{print $2\" \"$3}' beacon.auto.conf

plot awk_plane u 1:2 w l lw 10 not,\
awk_beacon w p pt 6 not
