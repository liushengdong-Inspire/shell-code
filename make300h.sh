#!/usr/bin/env bash
RED=$(tput setaf 1)
NORMAL=$(tput sgr0)
GREEN=$(tput setaf 2)
DATE=`date "+%Y%m%d"`
LOG_FILE_DIR="/home/liushengdong/log/$DATE"
DATE=`date "+%Y%m%d-%H%M%S"`
LOG_FILE_PATH="${LOG_FILE_DIR}/operation_${DATE}_make.log"
cmd_path=`pwd`
TYPE=`ls /home/liushengdong/release/`
MAKE_TYPE="env"
thread_num=8
MAKE_UPDATE_API=no
MAKE_RM_FILE=no
HELP=no
MAKE_RELEASE_SUCCESS=1

function note_log
{
    if [ ! -d $LOG_FILE_DIR ]
    then
        mkdir -p $LOG_FILE_DIR
    fi

    if [ ! -f $LOG_FILE_PATH ]
    then
        touch $LOG_FILE_PATH
        chmod 0644 $LOG_FILE_PATH
    fi
    DATE=`date "+%Y%m%d-%H%M%S"`
    echo "$DATE($$): ${1}" >> $LOG_FILE_PATH
}

function red()
{
    note_log "${1}"
    echo -e "${RED}${1} ${NORMAL}"
}

function green()
{
    note_log "${1}"
    echo -e "${GREEN}${1} ${NORMAL}"
}

function begin_to_make_program
{
    if [ "$1" == "" ]
    then
        red "$1 unexpect,exit!"
        MAKE_RELEASE_SUCCESS=0
        return
    fi
    
    lunch_name=""
    cd ${cmd_path}/
    pushd ${cmd_path}

        ### 删除当前目录下所有文件，并且重新同步
        if [ "$MAKE_RM_FILE" == "yes" ]
        then
            rm ./* -rf
            if [ $? -ne 0 ]
            then
                red "rm ./* -rf"
                MAKE_RELEASE_SUCCESS=0
                popd
                return
            else
                green "rm ./* -rf success!"
            fi

            repo sync
            if [ $? -ne 0 ]
            then
                red "repo sync failed!"
                popd
                MAKE_RELEASE_SUCCESS=0
                return
            else
                green "repo sync success!"
            fi
        fi

        ### 确认编译大系统时，将所有修改回退，并且clean 一下
        if [[ "$MAKE_TYPE" == "system" || "$MAKE_TYPE" == "all" ]]
        then
            repo forall -c "git clean -df && git checkout . "
            if [ $? -ne 0 ]
            then
                red "repo forall error!"
                MAKE_RELEASE_SUCCESS=0
                popd
                return
            else
                green "repo forall success!"
            fi
 
        fi

        ## 根据不同大系统，选择不同的 lunch
        if [ "$1" == "android_source" ]
        then
            green "android_source"
        elif [ "$1" == "mv200_kk" ]
        then
            lunch_name="Hi3798MV200-eng"
        elif [ "$1" == "Hisi3798mv100" ]
        then
            lunch_name="Hi3798MV100-eng"
        elif [ "$1" == "mv300_spc030_fj" ]
        then
            lunch_name="Hi3798MV300-eng"
        elif [ "$1" == "mv310_20180410" ]
        then
            lunch_name="Hi3798MV310-eng"
        elif [ "$1" == "mv300h_20181011" ]
        then
            lunch_name="Hi3798MV300H-eng"
        fi

        #300及以上芯片使用宏定义，以及环境变量
        if [[ "$1" == "mv300h_20181011" || "$1" == "mv300_spc030_fj" || "$1" == "mv310_20180410" ]]
        then
            ### 300、310和300H 共用变量
            export PRODUCT_TARGET=telecom
            if [ $? -ne 0 ]
            then
                red "export PRODUCT_TARGET failed!"
                popd
                MAKE_RELEASE_SUCCESS=0
                return
            else
                green "export PRODUCT_TARGET=$PRODUCT_TARGET success!"
            fi

            ### 300H 私有变量
            if [ "$1" == "mv300h_20181011" ]
            then
                export HISILICON_SECURITY_L2_FLAG=true
                if [ $? -ne 0 ]
                then
                    red "export HISILICON_SECURITY_L2_FLAG failed!"
                    popd
                    MAKE_RELEASE_SUCCESS=0
                    return
                else
                    green "export HISILICON_SECURITY_L2_FLAG=$HISILICON_SECURITY_L2_FLAG success!"
                fi

                source build/swfastbootenv/fastboot-hi3798mv31dms1-M8273-L2.env
                if [ $? -ne 0 ]
                then
                    red "source build/swfastbootenv/fastboot-hi3798mv31dms1-M8273-L2.env failed!"
                    MAKE_RELEASE_SUCCESS=0
                    popd
                    return
                else
                    green "source build/swfastbootenv/fastboot-hi3798mv31dms1-M8273-L2.env success!"
                fi
                
                ### 判断为编译大系统的时候
                if [[ "$MAKE_TYPE" == "system" || "$MAKE_TYPE" == "all" ]]
                then
                    source build/envsetup.sh
                    if [ $? -ne 0 ]
                    then
                        red "source build/envsetup.sh failed!"
                        popd
                        MAKE_RELEASE_SUCCESS=0
                        return
                    else
                        green "source build/envsetup.sh success!"
                    fi

                    lunch Hi3798MV300H-eng
                    if [ $? -ne 0 ]
                    then
                        red "lunch Hi3798MV300H-eng failed!"
                        MAKE_RELEASE_SUCCESS=0
                        popd
                        return
                    else
                        green "lunch Hi3798MV300H-eng success!"
                    fi

                    make clean
                    if [ $? -ne 0 ]
                    then
                        red "make clean error!"
                        popd
                        MAKE_RELEASE_SUCCESS=0
                        return
                    else
                        green "make clean success!"
                    fi

                    make bigfish common_pkg=y -j$thread_num
                    if [ $? -ne 0 ]
                    then
                        red "make bigfish common_pkg=y -j$thread_num failed!"
                        popd
                        MAKE_RELEASE_SUCCESS=0
                        return
                    else
                        green "make bigfish common_pkg=y -j$thread_num success!"
                    fi
                fi

                source build/swfastbootenv/fastboot-hi3798mv2dmc-M8238-L2.env
                if [ $? -ne 0 ]
                then
                    red "source build/swfastbootenv/fastboot-hi3798mv2dmc-M8238-L2.env failed!"
                    popd
                    MAKE_RELEASE_SUCCESS=0
                    return
                else
                    green "source build/swfastbootenv/fastboot-hi3798mv2dmc-M8238-L2.env success!"
                fi

                ### 判断为编译大系统的时候
                if [[ "$MAKE_TYPE" == "system" || "$MAKE_TYPE" == "all" ]]
                then
                    source build/envsetup.sh
                    if [ $? -ne 0 ]
                    then
                        red "source build/envsetup.sh failed!"
                        popd
                        MAKE_RELEASE_SUCCESS=0
                        return
                    else
                        green "source build/envsetup.sh success!"
                    fi

                    lunch Hi3798MV300-eng
                    if [ $? -ne 0 ]
                    then
                        red "lunch Hi3798MV300-eng failed!"
                        popd
                        MAKE_RELEASE_SUCCESS=0
                        return
                    else 
                        green "lunch Hi3798MV300-eng success!"
                    fi

                    make bigfish diff=y -j$thread_num
                    if [ $? -ne 0 ]
                    then
                        red "make bigfish diff=y -j$thread_num failed!"
                        popd
                        MAKE_RELEASE_SUCCESS=0
                        return
                    else 
                        green "make bigfish diff=y -j$thread_num success!"
                    fi

                fi

            fi
        fi
        
        ###编译完成后，需要编译其他文件的时候，使用到的命令
        if [ "$1" == "mv300h_20181011" ]
        then
            source build/swfastbootenv/fastboot-hi3798mv31dms1-M8273-L2.env
            if [ $? -ne 0 ]
            then
                red "source build/swfastbootenv/fastboot-hi3798mv31dms1-M8273-L2.env failed!"
                MAKE_RELEASE_SUCCESS=0
                popd
                return
            else
                green "source build/swfastbootenv/fastboot-hi3798mv31dms1-M8273-L2.env success!"
            fi
        fi

        source build/envsetup.sh
        if [ $? -ne 0 ]
        then
            red "source build/envsetup.sh failed!"
            popd
            MAKE_RELEASE_SUCCESS=0
            return
        else
            green "source build/envsetup.sh success!"
        fi

        green "lunch_name:$lunch_name"
        lunch $lunch_name
        if [ $? -ne 0 ]
        then
            red "lunch $lunch_name failed!"
            MAKE_RELEASE_SUCCESS=0
            popd
            return
        else
            green "lunch $lunch_name success!"
        fi
        
        if [ "$MAKE_UPDATE_API" == "yes" ]
        then
            make update-api
            if [ $? -ne 0 ]
            then
                red "make update-api failed!"
                popd
                MAKE_RELEASE_SUCCESS=0
                return
            else
                green "make update-api success!"
            fi
        fi

        if [[ "$MAKE_TYPE" == "system" || "$MAKE_TYPE" == "all" ]]
        then
            if [ "$1" != "mv300h_20181011" ]
            then
                make clean
                if [ $? -ne 0 ]
                then
                    red "make clean error!"
                    popd
                    MAKE_RELEASE_SUCCESS=0
                    return
                else
                    green "make clean success!"
                fi
                make -j$thread_num
                if [ $? -ne 0 ]
                then
                    red "make -j$thread_num failed!"
                    popd
                    MAKE_RELEASE_SUCCESS=0
                    return
                else
                    green "make -j$thread_num success!"
                fi
            fi
        fi
    popd
}

function show_help_info
{
    echo "`basename $0` help information:"
    echo "-all     编译 /home/liushengdong/release/ 目录下所有大系统"
    echo "-system  编译大系统，包括 git clean df 和 git checkout "
    echo "-j       配置线程数，默认使用 8 条线程"
    echo "-u       使用更新api命令，make update-api，默认不更新"
    echo "-rm      删除当前目录下所有文件，并且 repo sync"
    echo "-h       使用帮助"
    echo "         默认只导入编译环境、不编译大系统，使用 8 条线程，不更新 api，不删除当前文件夹"
}

###################################################
## 使用的命令集:默认使用8线程，导入环境变量
## 1.全编译 -all
## 2.编译对应系统  -system
## 3.自定义编译使用线程 -j，默认 8 线程
## 4.加条判断，是否 make update-api，默认更新 -u
## 5.删除编译目录下所有文件，并更新文件 -rm
## 6.帮助 -h
###################################################
if [ $# -ge 1 ]
then
    cmd_array=("$@")
    match_flag=0
    for ((i=0;i<=$#;i++))
    do
        if [ "${cmd_array[$i]}" == "-all" ]
        then
            MAKE_TYPE="all"
            match_flag=1
        elif [ "${cmd_array[$i]}" == "-env" ]
        then
            MAKE_TYPE="env"
            match_flag=1
        elif [ "${cmd_array[$i]}" == "-system" ]
        then
            MAKE_TYPE="system"
            match_flag=1
        elif [ "${cmd_array[$i]}" == "-u" ]
        then
            MAKE_UPDATE_API="yes"
            match_flag=1
        elif [ "${cmd_array[$i]}" == "-rm" ]
        then
            MAKE_RM_FILE="yes"
            match_flag=1
        elif [ "${cmd_array[$i]}" == "-h" ]
        then
            HELP="yes"
            match_flag=1
        elif [ "${cmd_array[$i]}" == "-j" ]
        then
            if [[ ${cmd_array[$(($i+1))]} -lt 0 || ${cmd_array[$(($i+1))]} -gt 256 || ${cmd_array[$(($i+1))]} == "" ]]
            then
                red "cmd error with empty thread number!"
                exit 0
            fi
            thread_num=${cmd_array[$(($i+1))]}
            match_flag=1
        fi
    done

    if [ $match_flag -eq 0 ]
    then
        red "commond some error!use default!"
    fi

    if [ "$HELP" == "no" ]
    then
        green "MAKE_TYPE=$MAKE_TYPE , thread_num=$thread_num MAKE_UPDATE_API=$MAKE_UPDATE_API"
    fi
fi

### 主函数
function main_function
{
    ### 帮助信息
    if [ "$HELP" == "yes" ]
    then
        show_help_info
        exit 0
    ### 判断编译类型是否为 all,system,env，默认为 env
    elif [[ "$MAKE_TYPE" == "all" || "$MAKE_TYPE" == "system" || "$MAKE_TYPE" == "env" ]]
    then

        ### 查找 /home/liushengdong/release/ 目录下所有大系统的路径
        ### 如果是 all，不判断编译路径是否正确
        ### 其他命令下，需要判断执行这条命令的路径是否是所有大系统路径之一，如果不是，不进行处理，防止出现问题

        for type in ${TYPE[@]}
        do
            ### 找到了对应路径的标记
            ### all 命令默认为找到，即编译所有大系统版本，可以不管执行命令的路径
            ### -system 命令，必须执行命令的路径在大系统路径中，否则不进行编译
            type=/home/liushengdong/release/${type}
            find_path_flag="false"
            if [ "$MAKE_TYPE" != "all" ]
            then
                ### 执行命令的路径存在大系统路径中
                if [ ${type} == ${cmd_path} ]
                then
                    find_path_flag="true"
                fi
            ### all 命令，默认编译所有大系统中的大系统，不对路径进行判断
            elif [ "$MAKE_TYPE" == "all"  ]
            then
                find_path_flag="true"
            fi

            ### 判断路径有找到
            if [ "$find_path_flag" == "true" ]
            then
                cmd_path=${type}
                current_path=`basename ${cmd_path}`
                green "begin to make $MAKE_TYPE $current_path version sdk!"
                MAKE_RELEASE_SUCCESS=1
                begin_to_make_program $current_path $thread_num
                if [ $MAKE_RELEASE_SUCCESS == "1" ]
                then
                    green "success to make $current_path !"
                elif [ $MAKE_RELEASE_SUCCESS == "0" ]
                then
                    red "make $current_path failed!"
                fi
                green " "
                echo "make $current_path log:"
                cat $LOG_FILE_PATH
                if [ "$MAKE_TYPE" != "all" ]
                then
                    exit 0
                fi
            fi
        done

        if [ "$find_path_flag" == "true" ]
        then
            exit 0
        fi
    fi
    red "program doesn't match!exit!"
    exit 0
}

main_function
