CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(CTCtrl)

# 检查该系统是否安装了MySQL开发环境
# 系统类型检测
IF(UNIX AND NOT APPLE)
    EXECUTE_PROCESS(
        COMMAND lsb_release -is 
        OUTPUT_VARIABLE 
        SYSTEM_TYPE 
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # MESSAGE(STATUS "SYSTEM_TYPE: ${SYSTEM_TYPE}")
    # MESSAGE(STATUS "OUTPUT_VARIABLE: ${OUTPUT_VARIABLE}")
    # MESSAGE(STATUS "OUTPUT_STRIP_TRAILING_WHITESPACE: ${OUTPUT_STRIP_TRAILING_WHITESPACE}")
ELSE()
    MESSAGE(STATUS "不是UNIX系统, 可能为Windows或macOS系统, 请手动安装并指定库文件及头文件所在位置")
ENDIF()

# 查找MySQL开发环境头文件所在位置
FIND_PATH(
    MYSQL_INCLUDE_DIR 
    NAMES mysql.h 
    PATHS /usr/include /usr/include/mysql /usr/local/include /usr/local/include/mysql /opt/local/include /opt/local/include/mysql
)
# 查找MySQL开发环境库文件所在位置
FIND_LIBRARY(
    MYSQL_LIBRARIES 
    NAMES mysqlclient 
    PATHS /usr/lib /usr/lib/mysql /usr/local/lib /usr/local/lib/mysql /opt/local/lib /opt/local/lib/mysql
)

# 判断MySQL开发环境是否安装
IF(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARIES)
    MESSAGE(STATUS "已找到MySQL的开发环境")
    MESSAGE(STATUS "头文件所在位置: ${MYSQL_INCLUDE_DIR}")
    MESSAGE(STATUS "库文件所在位置: ${MYSQL_LIBRARIES}")
ELSE()
    # 如果没有判断它是什么系统，如果是Ubuntu系统提供命令给他安装
    MESSAGE(SEND_ERROR "未找到MySQL的开发环境")
    IF(SYSTEM_TYPE STREQUAL "Ubuntu")
        MESSAGE(STATUS "检测到您为Ubuntu发行版系统, 为您提供MySQL开发环境安装命令")
        MESSAGE(STATUS "安装命令: sudo apt-get update && sudo apt install mysql-server mysql-client libmysqlclient-dev")
    ELSE()
        MESSAGE(STATUS "非Ubuntu发行版 SYSTEM_TYPE: ${SYSTEM_TYPE}")
    ENDIF()
ENDIF()

# 首先需要把jsoncpp和logger编译成静态库，然后链接到项目中
# 所以需要在这两个目录中分别创建CMake文件

# 需要指定生成的两个静态库的名字
SET(JSONCPP_LIBNAME jsoncpp)     # 指定jsoncpp库文件的名字
SET(LOG_LIBNAME log)             # 指定日志库文件的名字
SET(EXE_NAME CrossTime.out)      # 指定可执行程序的名字
SET(LIBOUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib) # 指定生成库文件的路径
SET(OUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)        # 指定程序的生成路径
SET(LOG_PATH ${OUT_PATH}/loggers)                    # 设置日志文件夹路径
SET(EXECUTABLE_OUTPUT_PATH ${OUT_PATH})              # 设置可执行程序的生成位置
SET(JSONCPP_HEAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp) # 指定jsoncpp头文件的路径
SET(LOGGER_HEAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/logger)   # 指定日志库头文件的路径
SET(CMAKE_CXX_STANDARD 17)                                 # 指定C++的标准

# 搜索源文件
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

# 禁用 -Wformat-contains-nul 警告
ADD_COMPILE_OPTIONS(-Wno-format-contains-nul)
# 禁用 -Wformat 警告
ADD_COMPILE_OPTIONS(-Wno-format)

# MESSAGE(WARNING ${CMAKE_CURRENT_SOURCE_DIR})

# 指定头文件所在位置
INCLUDE_DIRECTORIES(
    ${JSONCPP_HEAD_PATH}  # 指定jsoncpp头文件所在位置
    ${LOGGER_HEAD_PATH}   # 指定logger头文件所在位置
    crypto
    db
    epoll
    websocket
    process
    public
    http
    thread
    fileserver
    webserver
)

# 添加子cmake文件
ADD_SUBDIRECTORY(jsoncpp)   # 包含jsoncpp子目录下面的CMake文件
ADD_SUBDIRECTORY(logger)    # 包含logger子目录下面的CMake文件
ADD_SUBDIRECTORY(db)
ADD_SUBDIRECTORY(fileserver)
ADD_SUBDIRECTORY(process)
ADD_SUBDIRECTORY(thread)
ADD_SUBDIRECTORY(websocket)
ADD_SUBDIRECTORY(crypto)
ADD_SUBDIRECTORY(epoll)
ADD_SUBDIRECTORY(public)
ADD_SUBDIRECTORY(http)
ADD_SUBDIRECTORY(webserver)

# 指定库文件所在的路径
LINK_DIRECTORIES(${LIBOUT_PATH})

# MESSAGE(WARNING ${SRC_LIST})

ADD_EXECUTABLE(${EXE_NAME} ${SRC_LIST})

# 链接库文件
TARGET_LINK_LIBRARIES(
    ${EXE_NAME}
    ${JSONCPP_LIBNAME}
    ${LOG_LIBNAME} 
    pthread 
    curl 
    mysqlclient 
    crypto
)

# 使用file命令来创建文件夹
FILE(MAKE_DIRECTORY ${LOG_PATH})
