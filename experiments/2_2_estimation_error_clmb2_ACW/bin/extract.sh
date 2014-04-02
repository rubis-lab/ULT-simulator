rm ../result/* -rf
mkdir -p ../result/log
cp ../experiments/*/*.log ../result/log
cd util
./extract_log.sh
./extract_reflection.sh
./extract_map.sh
./extract_fail.sh

