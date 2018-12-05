#!/usr/bin/env bash
cmd_path=`pwd`
current_path=`basename ${cmd_path}`
CPU_TYPE=arm
CPU_TYPE_LIST=`ls ${cmd_path}/arch/`
CONFIG_FILE=goldfish_defconfig
CROSS_COMPILE_PATH=/home/liushengdong/release/mv300h_20181011/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/
CONFIG_FILE_LIST=`ls ${cmd_path}/arch/`
CROSS_COMPILE=arm-eabi-
THREAD_NUM=8

while getopts "t:c:f:j:p:h" opts
do
    case $opts in
        t)
        CPU_TYPE=""
        for type in ${CPU_TYPE_LIST[@]}
        do
            if [ "${type}" == "$OPTARG" ];then
                CPU_TYPE=$OPTARG
            fi
        done
        ;;

        c) 
        CROSS_COMPILE=""

        CROSS_COMPILE=$OPTARG ;;

        f)
        CONFIG_FILE=""
        CONFIG_FILE_LIST=`ls /home/liushengdong/kernel/arch/${CPU_TYPE}/configs/`
        for config in ${CONFIG_FILE_LIST[@]}
        do
            if [ "${config}" == "$OPTARG"  ];then
                CONFIG_FILE=$OPTARG
            fi
        done
        ;;

        p)
        CROSS_COMPILE_PATH=""
        if [ -d $OPTARG ];then
            CROSS_COMPILE_PATH=$OPTARG
        fi
        ;;

        h)
cat << EOF
Usage: `basename $0` [-t cpu type] [-c cross compile] [-f config file] [-p CROSS_COMPILE path] [-h help]
EOF
         exit 0
        ;;

        j)
        THREAD_NUM=$OPTARG
        ;;

        \?) 
        echo "Unknown option -$OPTARG"
        exit 1
        ;;

        :)
        echo "Option -$OPTARG requires an argument"
        exit 1
        ;;
    esac
done

echo -e "=========\nCPU_TYPE=${CPU_TYPE} \nCONFIG_FILE=${CONFIG_FILE} \nCROSS_COMPILE=${CROSS_COMPILE} \nTHREAD_NUM=${THREAD_NUM}\nCROSS_COMPILE_PATH=${CROSS_COMPILE_PATH}\n========="
if [[ "${CPU_TYPE}" == "" || "${CONFIG_FILE}" == "" || "${CROSS_COMPILE}" == "" || "${THREAD_NUM}" == "" || "${CROSS_COMPILE_PATH}" == "" ]];then
    echo "configure error!Please check!"
    exit 1
fi


if [ "${current_path}" == "kernel" ];then
    export PATH=$PATH:/home/liushengdong/release/mv300h_20181011/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/
    pushd ${cmd_path}
       if [ -f ./Makefile ];then
            echo "Makefile exist!"
            make ARCH=${CPU_TYPE} CROSS_COMPILE=${CROSS_COMPILE}
            make ${CONFIG_FILE} -j${THREAD_NUM}
       else
            echo "Makefile isn't exist!"
       fi
    popd
else
    echo "It isn't kernel path!"
fi
