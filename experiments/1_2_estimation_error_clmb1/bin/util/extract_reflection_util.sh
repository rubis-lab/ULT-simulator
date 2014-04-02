cp avg_ref_cnt.awk ../../result/trace/
cd ../../result/trace/

printf "3\t%f\n" \
`echo \`awk -f avg_ref_cnt.awk *nb03_nc03*.trace\` / 3 | bc -l` \
> reflection_util.txt

printf "4\t%f\t%f\n" \
`echo \`awk -f avg_ref_cnt.awk *nb04_nc03*.trace\` / 3 | bc -l` \
`echo \`awk -f avg_ref_cnt.awk *nb04_nc04*.trace\` / 4 | bc -l` \
>> reflection_util.txt

for ((i=5;i<=30;i++)); do
	nb=`printf %02d $i`
	printf "5\t%f\t%f\t%f\n" \
	`echo \`awk -f avg_ref_cnt.awk *nb${nb}_nc03*.trace\` / 3 | bc -l` \
	`echo \`awk -f avg_ref_cnt.awk *nb${nb}_nc04*.trace\` / 4 | bc -l` \
	`echo \`awk -f avg_ref_cnt.awk *nb${nb}_nc05*.trace\` / 5 | bc -l` \
	>> reflection_util.txt;
done;
