rm ../../maps -rf
mkdir ../../maps
cp ../../../../bin/simulator ../../maps

function make_opt() {
	opt=`printf "%02d" $1`
	mkdir opt$opt
	cp conf.base opt$opt/conf.txt
	cp beacon.conf plane.conf ../simulator opt$opt/
	echo "env_optimization=$opt" >> opt$opt/conf.txt
}

for dir in $(ls -d map*); do
	cp -R $dir ../../maps/
	cd ../../maps/$dir
	make_opt 0
	make_opt 1
	make_opt 2
	make_opt 3
	make_opt 4
	make_opt 5
	make_opt 6
	make_opt 7
	cd - > /dev/null
	
done


