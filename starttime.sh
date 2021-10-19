#!/bin/bash
readFile="sensor-json.txt"
writeFile="log-start-time.txt"

#printf "로그 파일이 저장된 디렉토리를 입력하세요: "
#read logDir
#echo ${logDir}
logDir="log-stride"

for i in ${logDir}/*; do
    j="${i}/${readFile}"
    echo $j
    grep -h '^\[' $j | head -1 >> $writeFile
done
