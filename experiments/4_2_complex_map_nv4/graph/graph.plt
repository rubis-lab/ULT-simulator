set terminal post eps monochrome solid;

#set xlabel 'xlabel'
set ylabel 'ylabel'
set key box spacing 5
set key width 6
set bmargin 4
#set size ratio 1.0
set yrange[0:]


#set output "map1_error.eps"
#plot "../result/map1_error.txt" using 2:xticlabel(1) w hist fs

#set output "map2_error.eps"
#plot "../result/map2_error.txt" using 2:xticlabel(1) w hist fs

#set output "map3_error.eps"
#plot "../result/map3_error.txt" using 2:xticlabel(1) w hist fs

#set output "map1_PMS1.eps"
#plot "../result/map1_nPMS1.txt" using 2:xticlabel(1) w hist fs

#set output "map2_PMS1.eps"
#plot "../result/map2_nPMS1.txt" using 2:xticlabel(1) w hist fs

#set output "map3_PMS1.eps"
#plot "../result/map3_nPMS1.txt" using 2:xticlabel(1) w hist fs

#set output "map1_PMS2.eps"
#plot "../result/map1_nPMS2.txt" using 2:xticlabel(1) w hist fs

#set output "map2_PMS2.eps"
#plot "../result/map2_nPMS2.txt" using 2:xticlabel(1) w hist fs

#set output "map3_PMS2.eps"
#plot "../result/map3_nPMS2.txt" using 2:xticlabel(1) w hist fs

set output "nPMS.eps"
plot "../result/nPMS.txt" using 2:xticlabel(1) w hist fs p 0 t "no_opt", \
'' using 3:xticlabel(1) w hist fs p 4 t "opt1", \
'' using 4:xticlabel(1) w hist fs p 6 t "opt1_opt2", \
'' using 5:xticlabel(1) w hist fs p 3 t "opt1_opt2_opt3"

set output "nPMS_ratio.eps"
plot "../result/nPMS_ratio.txt" using 2:xticlabel(1) w hist fs p 0 t "no_opt", \
'' using 3:xticlabel(1) w hist fs p 4 t "opt1", \
'' using 4:xticlabel(1) w hist fs p 6 t "opt1_opt2", \
'' using 5:xticlabel(1) w hist fs p 3 t "opt1_opt2_opt3"


#######################################
# Draw Map
#

reset 
set size ratio -1
set xtic 100
set ytic 100
set xlabel "xlabel"
set ylabel "ylabel"

do for [dir in system('ls -d ../maps/map*')] {
	name=system('echo '.dir.'|cut -b 9-')
	plane=dir."/plane.conf"
	beacon=dir."/beacon.conf"
	set output name.".eps"

	awk_plane="<gawk -F '[,#O()]+' '{print $2\" \"$3;print $5\" \"$6;print $8\" \"$9;print $11\" \"$12;print $2\" \"$3;print \" \"}' ".plane
	awk_beacon="<gawk -F '[,#\t ]+' '{print $2\" \"$3}' ".beacon

	plot awk_plane u 1:2 w l lw 10 not,\
	awk_beacon w p pt 6 not
}



