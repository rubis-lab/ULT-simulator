function extract_log
{
	rm -f $2
	for ((i=0;i<=7;i++)); do
		opt=`printf %02d $i`

		val=`cat *opt${opt}* | grep $1 | awk '{print $3}'`
		printf "%02d\t%f\n" $i $val >> $2
	done
}

mkdir -p ../result
rm -rf ../result/*
cp ./extract_pms.sh ../result
cd ../maps

idx=0
for map_dir in $(ls -d map*); do
	idx=`expr $idx + 1`
	cd $map_dir
	rm -rf result
	mkdir result
	cp opt*/*.log result/
	cd result

	result_path=../../../result

	extract_log B01 $result_path/map${idx}_nPMS1.txt
	extract_log B03 $result_path/map${idx}_nPMS2.txt
	extract_log G01 $result_path/map${idx}_error.txt
	extract_log H03 $result_path/map${idx}_fail2.txt

	cd ../..
done

cd ../result
./extract_pms.sh

