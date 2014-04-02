
m100=`grep '00	' map1_nPMS1.txt | awk '{print $2}'`
m101=`grep '01	' map1_nPMS1.txt | awk '{print $2}'`
m105=`grep '05	' map1_nPMS1.txt | awk '{print $2}'`
m107=`grep '07	' map1_nPMS2.txt | awk '{print $2}'`

m200=`grep '00	' map2_nPMS1.txt | awk '{print $2}'`
m201=`grep '01	' map2_nPMS1.txt | awk '{print $2}'`
m205=`grep '05	' map2_nPMS1.txt | awk '{print $2}'`
m207=`grep '07	' map2_nPMS2.txt | awk '{print $2}'`

m300=`grep '00	' map3_nPMS1.txt | awk '{print $2}'`
m301=`grep '01	' map3_nPMS1.txt | awk '{print $2}'`
m305=`grep '05	' map3_nPMS1.txt | awk '{print $2}'`
m307=`grep '07	' map3_nPMS2.txt | awk '{print $2}'`

m100r=`awk "BEGIN {print $m100 / $m100}"`;
m101r=`awk "BEGIN {print $m101 / $m100}"`;
m105r=`awk "BEGIN {print $m105 / $m100}"`;
m107r=`awk "BEGIN {print $m107 / $m100}"`;

m200r=`awk "BEGIN {print $m200 / $m200}"`;
m201r=`awk "BEGIN {print $m201 / $m200}"`;
m205r=`awk "BEGIN {print $m205 / $m200}"`;
m207r=`awk "BEGIN {print $m207 / $m200}"`;


m300r=`awk "BEGIN {print $m300 / $m300}"`;
m301r=`awk "BEGIN {print $m301 / $m300}"`;
m305r=`awk "BEGIN {print $m305 / $m300}"`;
m307r=`awk "BEGIN {print $m307 / $m300}"`;

outname="nPMS.txt"

echo "map1	$m100	$m101	$m105	$m107" > $outname
echo "map2	$m200	$m201	$m205	$m207" >> $outname
echo "map3	$m300	$m301	$m305	$m307" >> $outname


outname="nPMS_ratio.txt"

echo "map1	$m100r	$m101r	$m105r	$m107r" > $outname
echo "map2	$m200r	$m201r	$m205r	$m207r" >> $outname
echo "map3	$m300r	$m301r	$m305r	$m307r" >> $outname
