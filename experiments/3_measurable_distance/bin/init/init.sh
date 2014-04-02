function make_conf(){
	filename=m$1;
	cp ./conf.base ./conf$filename;
	echo "sim_max_dist=$1" >> conf$filename;
	mkdir ../../experiments/$filename;
	mv ./conf$filename ../../experiments/conf/;
}

rm -rf ../../experiments
mkdir -p ../../experiments/conf 
for ((i=100;i<=2000;i+=100)); do
	meas=`printf %04d $i`;
	make_conf $meas;
done

mv ../simulator ../simulator.old
cp ../../../../bin/simulator ../
