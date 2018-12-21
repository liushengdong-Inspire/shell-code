#!/usr/bin/env bash
PWD=`pwd`
echo "current path=$PWD"
FILE_ARRAY=$(ls ${PWD})
echo ${FILE_ARRAY[@]}
sourceString=$1
modifyString=$2
fileExecute=`basename ${PWD}`
for file in ${FILE_ARRAY[@]}
do
    if [[ "${file}" == "${fileExecute}" ]]
    then
        continue
    fi
    sed -i "s/${sourceString}/${modifyString}/g" ${file}
done
#sed -i "s/#4=SPDIF/4=SPDIF/g"
