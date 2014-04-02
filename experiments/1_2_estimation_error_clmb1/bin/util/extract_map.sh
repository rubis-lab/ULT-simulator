rm -rf ../../result/map
mkdir -p ../../result/map
for i in $(ls -d ../../experiments/*_03|cut -b 19-);do
	./draw_map.sh $i 
done

for i in $(ls -d ../../experiments/*_04|cut -b 19-);do
	./draw_map.sh $i 
done

for i in $(ls -d ../../experiments/*_05|cut -b 19-);do
	./draw_map.sh $i 
done
