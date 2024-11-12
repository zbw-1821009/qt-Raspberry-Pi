cmake_minimum_required(VERSION 3.16)

# 项目信息
project(RaspberryPiControllerAndMediaPlayer VERSION 0.1 LANGUAGES CXX)

# C++ 标准版本
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 MOC、UIC 和 RCC 的自动调用
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找所需的 Qt5 模块
find_package(Qt5 COMPONENTS Core Gui Widgets MultimediaWidgets Network REQUIRED)

# 添加 libevdev 头文件目录
include_directories("/usr/include/libevdev-1.0/libevdev")

# 定义 RaspberryPiController 项目的可执行文件（包含 videobuttons）
add_executable(RaspberryPiController
    main.cpp
    mainwindow.cpp
    joystick_control.cpp
    bluetooth_control.cpp
    streaming.cpp
    videobuttons.cpp    # 添加 videobuttons.cpp
    joystick_control.h
    mainwindow.h
    streaming.h
    videobuttons.h      # 添加 videobuttons.h
)

# 将 RaspberryPiController 可执行文件链接到必要的库
target_link_libraries(RaspberryPiController
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::MultimediaWidgets
    Qt5::Network
    Qt5::Multimedia
    wiringPi
    evdev
)

# 安装设置
include(GNUInstallDirs)

# 安装 RaspberryPiController 可执行文件
install(TARGETS RaspberryPiController
    RUNTIME DESTINATION /usr/local/bin
)
