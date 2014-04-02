cd ../../result/log/



for ((i=3;i<=30;i++)); do
	nb=`printf %02d $i`

	if [ $i -eq  3 ]; then
		nv3=`cat *nb${nb}*nv03* | grep F01 | awk '{print $3}'`
		printf "3\t%f\n" ${nv3} > ../fail.txt
	fi
	if [ $i -eq 4 ]; then
		nv3=`cat *nb${nb}*nv03* | grep F01 | awk '{print $3}'`
		nv4=`cat *nb${nb}*nv04* | grep F01 | awk '{print $3}'`
		printf "4\t%f\t%f\n" ${nv3} ${nv4} >> ../fail.txt
	fi
	if [ $i -gt 4 ]; then
		nv3=`cat *nb${nb}*nv03* | grep F01 | awk '{print $3}'`
		nv4=`cat *nb${nb}*nv04* | grep F01 | awk '{print $3}'`
		nv5=`cat *nb${nb}*nv05* | grep F01 | awk '{print $3}'`
		printf "$i\t%f\t%f\t%f\n" ${nv3} ${nv4} ${nv5} >> ../fail.txt
	fi
done
		

