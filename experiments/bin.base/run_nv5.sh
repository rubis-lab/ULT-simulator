cd util
for i in $(ls -d ../../experiments/*_05|cut -b 19-);do
	./run_local.sh $i &
done

