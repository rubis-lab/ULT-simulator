cd ../../result/log/


function extract_log 
{
for ((i=3;i<=30;i++)); do
	nb=`printf %02d $i`

	if [ $i -eq  3 ]; then
		nv3=`cat *nb${nb}*nv03* | grep $1 | awk '{print $3}'`
		printf "3\t%f\n" ${nv3} > $2
	fi
	if [ $i -eq 4 ]; then
		nv3=`cat *nb${nb}*nv03* | grep $1 | awk '{print $3}'`
		nv4=`cat *nb${nb}*nv04* | grep $1 | awk '{print $3}'`
		printf "4\t%f\t%f\n" ${nv3} ${nv4} >> $2
	fi
	if [ $i -gt 4 ]; then
		nv3=`cat *nb${nb}*nv03* | grep $1 | awk '{print $3}'`
		nv4=`cat *nb${nb}*nv04* | grep $1 | awk '{print $3}'`
		nv5=`cat *nb${nb}*nv05* | grep $1 | awk '{print $3}'`
		printf "$i\t%f\t%f\t%f\n" ${nv3} ${nv4} ${nv5} >> $2
	fi
done
}

extract_log F01 ../fail.txt
extract_log F02 ../fail_ref.txt
extract_log H01 ../fail2.txt
extract_log H02 ../fail2_ref.txt
extract_log H03 ../fail3.txt
extract_log H04 ../fail3_ref.txt
extract_log G01 ../error.txt
extract_log G02 ../error_ref.txt
extract_log G03 ../error2.txt
extract_log G04 ../error2_ref.txt
extract_log G05 ../error3.txt
extract_log G06 ../error3_ref.txt
