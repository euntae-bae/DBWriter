#!/bin/bash
readFile="sensor-log-mean.txt"
writeFile="log-gait-speed.txt"

#printf "로그 파일이 저장된 디렉토리를 입력하세요: "
#read logDir
#echo ${logDir}

logDir="log-stride"

for i in ${logDir}/*; do
    j="${i}/${readFile}"
    echo $j
    grep -h '^-> v:' $j >> $writeFile
done
