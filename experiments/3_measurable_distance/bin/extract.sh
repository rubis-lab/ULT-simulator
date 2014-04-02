rm ../result/* -rf
mkdir -p ../result/log
cp ../experiments/*/*.log ../result/log
cd ../result/log 
#./extract_log.sh
#./extract_reflection.sh
#./extract_map.sh
#./extract_fail.sh


function extract_log 
{
rm -f $2
for ((i=1;i<=20;i++)); do
	meas=`printf %02d $i`
	
	val=`cat *md${meas}* | grep $1 | awk '{print $3}'`
	printf "%d00\t%f\n" $i $val >> $2
done
}


extract_log G01 ../error.txt
extract_log G02 ../error_ref.txt
extract_log H03 ../fail.txt
