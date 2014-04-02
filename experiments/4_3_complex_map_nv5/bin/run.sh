cp run_local.sh ../maps
cd ../maps

for dir in $(ls -d map*); do
	cd $dir
	../run_local.sh&
	cd ..
done
		
