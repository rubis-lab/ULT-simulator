function make_conf(){
	cp ./conf.base ./conf$1_$2;
	echo "sim_n_beacon=$1" >> conf$1_$2;
	echo "env_n_cut=$2" >> conf$1_$2;
	mkdir ../../experiments/$1_$2;
	mv ./conf$1_$2 ../../experiments/conf/;
}

rm -rf ../../experiments
mkdir -p ../../experiments/conf 
for ((i=3;i<=5;i++)); do
	for ((j=i;j<=30;j++)); do
		nb=`printf %02d $j`
		nv=`printf %02d $i`
		make_conf $nb $nv;
	done
done

