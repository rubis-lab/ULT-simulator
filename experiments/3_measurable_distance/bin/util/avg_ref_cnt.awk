BEGIN {sum = 0; cnt = 0;}
{
	sum += $3;
	cnt ++;
}
END {print sum/cnt;}
