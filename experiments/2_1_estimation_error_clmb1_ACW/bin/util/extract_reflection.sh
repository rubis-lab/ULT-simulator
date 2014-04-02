cd ../../result/log/



for ((i=3;i<=30;i++)); do
	nb=`printf %02d $i`

	if [ $i -eq  3 ]; then
		nv3=`cat *nb${nb}*nv03* | grep D01 | awk '{print $3}'`
		printf "3\t%f\n" ${nv3} #> ../reflection.txt
	fi
	if [ $i -eq 4 ]; then
		nv3=`cat *nb${nb}*nv03* | grep D01 | awk '{print $3}'`
		nv4=`cat *nb${nb}*nv04* | grep D01 | awk '{print $3}'`
		printf "4\t%f\t%f\n" ${nv3} ${nv4} #>> ../reflection.txt
	fi
	if [ $i -gt 4 ]; then
		nv3=`cat *nb${nb}*nv03* | grep D01 | awk '{print $3}'`
		nv4=`cat *nb${nb}*nv04* | grep D01 | awk '{print $3}'`
		nv5=`cat *nb${nb}*nv05* | grep D01 | awk '{print $3}'`
		printf "$i\t%f\t%f\t%f\n" ${nv3} ${nv4} ${nv5} #>> ../reflection.txt
	fi
done
		

