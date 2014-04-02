cur_path=`pwd`
for i in $(ls -d ../../experiments/m*|cut -b 19-);do
	cd $cur_path
	cp ../simulator ../../experiments/$i/
	cd ../../experiments/$i/
	./simulator ../conf/conf$i &
done

