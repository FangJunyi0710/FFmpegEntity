cmake_minimum_required(VERSION 3.16)

project(VideoGenerate VERSION 0.1 LANGUAGES CXX)

# 设置Qt自动处理
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
add_compile_options(-Wall -Werror -Wextra -Ofast -DNDEBUG -g -DLOG)
include_directories(${CMAKE_SOURCE_DIR})

# 查找Qt (支持Qt5和Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui)

# 添加FFmpegEntity子目录
add_subdirectory(FFmpegEntity)

# 定义添加可执行文件的函数
function(add_executable_with_qt NAME_ SOURCE_ EXE_OPTIONS)
    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(${NAME_}
            ${EXE_OPTIONS}
            MANUAL_FINALIZATION
            ${SOURCE_}
        )
    else()
        add_executable(${NAME_} ${EXE_OPTIONS} ${SOURCE_})
    endif()

    target_link_libraries(${NAME_} PRIVATE 
        FFmpegEntity
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Gui
    )

    # 设置目标属性
    set_target_properties(${NAME_} PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
    )

    # Qt6特定的最终处理
    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(${NAME_})
    endif()
endfunction()

# 设置源文件
set(SOURCES
    main.cpp
    # 如果有其他源文件，可以在这里添加
)

# 如果有Qt资源文件，可以在这里添加
# set(RESOURCES
#     resources.qrc
# )

add_executable_with_qt(VideoGenerate "${SOURCES}" "")

# 添加tools目录下的单源代码文件
file(GLOB TOOLS_SOURCES "tools/*.cpp")
foreach(TOOL_SOURCE ${TOOLS_SOURCES})
    get_filename_component(TOOL_NAME ${TOOL_SOURCE} NAME_WE)
    add_executable_with_qt(${TOOL_NAME} ${TOOL_SOURCE} EXCLUDE_FROM_ALL)
endforeach()