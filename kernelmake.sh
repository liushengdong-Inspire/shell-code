#!/usr/bin/env bash
cmd_path=`pwd`
current_path=`basename ${cmd_path}`
CPU_TYPE=arm
CPU_TYPE_LIST=`ls ${cmd_path}/arch/`
CONFIG_FILE=goldfish_armv7_defconfig
CROSS_COMPILE_PATH=/home/liushengdong/release/mv300h_20181011/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/
CONFIG_FILE_LIST=`ls ${cmd_path}/arch/`
CROSS_COMPILE=arm-eabi-
THREAD_NUM=8
MAKE_CLEAN_FLAG=0

function show_help
{
cat << EOF
Usage: `basename $0` [-t cpu type] [-c cross compile] [-f config file] [-p CROSS_COMPILE path] [-h help] [-l make clean]
EOF
exit 0
}

if [ "${current_path}" != "kernel" ];then
    show_help
fi

while getopts "t:c:f:j:p:hl" opts
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
        show_help
        ;;

        l)
        MAKE_CLEAN_FLAG=1
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

echo "=============================================="
echo "CPU_TYPE            = ${CPU_TYPE}"
echo "CONFIG_FILE         = ${CONFIG_FILE}"
echo "CROSS_COMPILE       = ${CROSS_COMPILE}"
echo "THREAD_NUM          = ${THREAD_NUM}"
echo "CROSS_COMPILE_PATH  = ${CROSS_COMPILE_PATH}"
echo "MAKE_CLEAN_FLAG     = ${MAKE_CLEAN_FLAG}"
echo "=============================================="

if [[ "${CPU_TYPE}" == "" || "${CONFIG_FILE}" == "" || "${CROSS_COMPILE}" == "" || "${THREAD_NUM}" == "" || "${CROSS_COMPILE_PATH}" == "" ]];then
    echo "configure error!Please check!"
    exit 1
fi

export PATH=$PATH:${CROSS_COMPILE_PATH}
pushd ${cmd_path}
   if [ -f ./Makefile ];then
        export ARCH=${CPU_TYPE}
        export CROSS_COMPILE=${CROSS_COMPILE}
        if [ $MAKE_CLEAN_FLAG -eq 1 ];then
            make clean
            rm -rf .config
        fi
        make ${CONFIG_FILE}
        make -j${THREAD_NUM}
   else
        echo "Makefile isn't exist!"
   fi
popd
