cp avg_ref_cnt.awk ../../result/trace/
cd ../../result/trace/
printf "3\t%f\n" \
`awk -f avg_ref_cnt.awk *nb03_nc03*.trace` \
> reflection_cnt.txt

printf "4\t%f\t%f\n" \
`awk -f avg_ref_cnt.awk *nb04_nc03*.trace` \
`awk -f avg_ref_cnt.awk *nb04_nc04*.trace` \
>> reflection_cnt.txt

for ((i=5;i<=30;i++)); do
	nb=`printf %02d $i`
	printf "$i\t%f\t%f\t%f\n" \
	`awk -f avg_ref_cnt.awk *nb${nb}_nc03*.trace` \
	`awk -f avg_ref_cnt.awk *nb${nb}_nc04*.trace` \
	`awk -f avg_ref_cnt.awk *nb${nb}_nc05*.trace` \
	>> reflection_cnt.txt;
done
