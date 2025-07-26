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
add_compile_options(-Wall -Werror -Wextra -Ofast -DNDEBUG)

# 查找Qt (支持Qt5和Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui)

# 添加FFmpegEntity子目录
add_subdirectory(FFmpegEntity)

# 设置源文件
set(SOURCES
    main.cpp
    # 如果有其他源文件，可以在这里添加
)

# 如果有Qt资源文件，可以在这里添加
# set(RESOURCES
#     resources.qrc
# )

# 根据Qt版本创建可执行文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VideoGenerate
        MANUAL_FINALIZATION
        ${SOURCES}
        # ${RESOURCES}  # 如果有资源文件
    )
else()
    add_executable(VideoGenerate
        ${SOURCES}
        # ${RESOURCES}  # 如果有资源文件
    )
endif()

# 链接库
target_link_libraries(VideoGenerate PRIVATE 
    FFmpegEntity
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

# 设置目标属性
set_target_properties(VideoGenerate PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# 安装规则
include(GNUInstallDirs)
install(TARGETS VideoGenerate
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt6特定的最终处理
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VideoGenerate)
endif()