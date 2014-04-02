cd ../../result/log
head -q -n 1 *_nv03*.log | awk 'BEGIN {i = 3;}{print i " " $1 " " $2 " " $3;i++;}' > ../estimation_error_nv3.txt
head -q -n 1 *_nv04*.log | awk 'BEGIN {i = 4;}{print i " " $1 " " $2 " " $3;i++;}' > ../estimation_error_nv4.txt
head -q -n 1 *_nv05*.log | awk 'BEGIN {i = 5;}{print i " " $1 " " $2 " " $3;i++;}' > ../estimation_error_nv5.txt
