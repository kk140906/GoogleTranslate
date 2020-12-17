cmake_minimum_required(VERSION 3.17)
project(GoogleTranslate VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
# 设置Qt前缀路径
set(CMAKE_PREFIX_PATH "D:/Qt/6.0.0/mingw81_64")
#set(CMAKE_PREFIX_PATH "D:/Qt/5.13.1/mingw73_64")
# 自动处理Qt中使用了Q_OBJECT宏的头文件
set(CMAKE_AUTOMOC ON)
# 自动处理Qt的rc资源文件
set(CMAKE_AUTORCC ON)
# 自动处理Qt的ui文件
set(CMAKE_AUTOUIC ON)

# 设置Qt版本
set(QT_VERSION 6)
set(QT_MINOR_VERSION 0)
# 设置项目需要用到的Qt模块
set(REQUIRED_LIBS Core Gui Widgets Network)
set(REQUIRED_LIBS_QUALIFIED Qt${QT_VERSION}::Core Qt${QT_VERSION}::Gui Qt${QT_VERSION}::Widgets Qt${QT_VERSION}::Network)
# 查找Qt的包
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

add_executable(${PROJECT_NAME} WIN32)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

set(MAIN_WINDOW src/mainwindow.ui)
set(RESOURCES src/resource.qrc)
target_sources(${PROJECT_NAME}
        PRIVATE
        ${MAIN_WINDOW}
        ${RESOURCES}
        ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_autogen/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE QT${QT_VERSION})
# 将Qt的库链接到可执行文件
target_link_libraries(${PROJECT_NAME} PRIVATE ${REQUIRED_LIBS_QUALIFIED})

# Windows平台相关的设置，如果是Debug则拷贝的库是带有d标志的dll
if (WIN32)
    set(DEBUG_SUFFIX)
    # 为了解决Qt5.14 MinGW不再提供带d的dll而新增加两个条件限制一下，QT_MINOR_VERSION是在前面自定义的
    if (CMAKE_BUILD_TYPE MATCHES "Debug"
            AND QT_VERSION LESS_EQUAL 5
            AND QT_MINOR_VERSION LESS 14)
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    # 拷贝windows相关的库到目标文件所在的目录
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    # 拷贝每个需要的Qt库到目标文件所在的目录
    foreach (QT_LIB ${REQUIRED_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
