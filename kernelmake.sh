#!/usr/bin/env bash
function show_help
{
cat << EOF
Usage: under kernel path
`basename $0` [-t cpu type] [-c cross compile] [-f config file] [-p CROSS_COMPILE path] [-h help] [-l make clean]
EOF
exit 0
}

cmd_path=`pwd`
current_path=`basename ${cmd_path}`
if [ "${current_path}" != "kernel" ];then
    show_help
fi

RED=$(tput setaf 1)
NORMAL=$(tput sgr0)
GREEN=$(tput setaf 2)
CPU_TYPE=arm
CPU_TYPE_LIST=`ls ${cmd_path}/arch/`
CONFIG_FILE=goldfish_armv7_defconfig
CROSS_COMPILE_PATH=/home/liushengdong/release/mv300h_20181011/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/
CONFIG_FILE_LIST=`ls ${cmd_path}/arch/`
CROSS_COMPILE=arm-eabi-
THREAD_NUM=8
MAKE_CLEAN_FLAG=0

function red()
{
    echo -e "${1} ${RED}${2} ${NORMAL}"
}

function green()
{
    echo -e "${1} ${GREEN}${2} ${NORMAL}"
}

### 命令行取值
while getopts "t:c:f:j:p:hl" opts
do
    case $opts in
        t) CPU_TYPE=$OPTARG ;;

        c) CROSS_COMPILE=$OPTARG ;;

        f) CONFIG_FILE=$OPTARG ;;

        p) CROSS_COMPILE_PATH=$OPTARG ;;

        h) show_help ;;

        l) MAKE_CLEAN_FLAG=1 ;;

        j) THREAD_NUM=$OPTARG ;;

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

### 命令行取值反馈
EXIST_RECODE=(0 0 0 0 0)
STRING_TYPE=('CPU_TYPE          ' 'CONFIG_FILE       ' 'CROSS_COMPILE_PATH' 'CROSS_COMPILE     ' 'THREAD_NUM        ')
VALUE_ARRAY=(${CPU_TYPE} ${CONFIG_FILE} ${CROSS_COMPILE_PATH} ${CROSS_COMPILE} ${THREAD_NUM})
### 判断 CPU 类型文件夹是否存在
if [ -d "${cmd_path}/arch/${CPU_TYPE}" ]
then
    EXIST_RECODE[0]=1
fi

### 判断配置文件是否存在
if [ -f "${cmd_path}/arch/${CPU_TYPE}/configs/${CONFIG_FILE}" ]
then
    EXIST_RECODE[1]=1
fi

### 判断交叉工具路径是否存在
if [ -d ${CROSS_COMPILE_PATH} ]
then
    EXIST_RECODE[2]=1
fi

### 判断交叉工具前缀是否存在
if [ -f "${CROSS_COMPILE_PATH}/${CROSS_COMPILE}gcc" ]
then
    EXIST_RECODE[3]=1
fi

### 判断线程数是否在 64 条以内
if [[ $THREAD_NUM -ge 0 && $THREAD_NUM -le 128 ]]
then
    EXIST_RECODE[4]=1
fi

exit_flag=0
MAX_STRING_LENGTH=0
show_string=""

### 获取所有参数名称中的最长值
for string_name in ${STRING_TYPE[@]}
do
    string_length=${#string_name}
    if [ $string_length -ge $MAX_STRING_LENGTH ]
    then
        MAX_STRING_LENGTH=$[string_length+3]
    fi
done

### 显示总共 MAX_STRING_LENGTH 个 ‘=’
for ((i=0;i<${MAX_STRING_LENGTH};i++))
do
    show_string=${show_string}=
done

### 显示信息
echo -e "\n${show_string}"
for num in {0..4}
do
    if [ ${EXIST_RECODE[num]} -eq 0 ]
    then
        exit_flag=1
        red "${STRING_TYPE[num]}  =" "${VALUE_ARRAY[num]}"
    else
        green "${STRING_TYPE[num]}  =" "${VALUE_ARRAY[num]}"
    fi
done
green "MAKE_CLEAN_FLAG     =" "${MAKE_CLEAN_FLAG}"
echo "${show_string}"

if [ $exit_flag -eq 1 ]
then
    echo -e "configure error!Please check!\n"
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
