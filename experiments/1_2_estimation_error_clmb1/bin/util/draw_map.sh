cp ./../map.plt ../../experiments/$1/
cd ../../experiments/$1/
gnuplot map.plt
cp map.eps ../../result/map/map$1.eps
