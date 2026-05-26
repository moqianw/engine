cmake_minimum_required(VERSION 3.20)

project(gameengine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/VulkanSDK.cmake)

option(VK_ENABLE_QT "Enable Qt platform backend" ON)

if(VK_ENABLE_QT)
    set(QT_ROOT "D:/Qt/6.11.0/msvc2022_64" CACHE PATH "Qt installation path")
    list(APPEND CMAKE_PREFIX_PATH "${QT_ROOT}")

    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

    message(STATUS "Qt root: ${QT_ROOT}")
endif()

add_subdirectory(src)
