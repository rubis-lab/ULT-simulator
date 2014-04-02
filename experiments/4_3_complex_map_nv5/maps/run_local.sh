for dir in $(ls -d opt*); do
	cd $dir
	./simulator &
	cd ..
done
