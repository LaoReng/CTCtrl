CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(CTCtrl)

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
add_compile_options(-Wno-format-contains-nul)
# 禁用 -Wformat 警告
add_compile_options(-Wno-format)

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
