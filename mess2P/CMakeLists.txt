cmake_minimum_required(VERSION 3.5)

project(mess2P LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(OpenSSL REQUIRED)

if(ANDROID)
  add_library(mess2P SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    peer.cpp
    peer.h
  )
else()
  add_executable(mess2P
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    peer.cpp
    peer.h
    AES_ciph.cpp
    AES_ciph.h
  )
endif()

target_link_libraries(mess2P PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(mess2P PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(mess2P PRIVATE Qt${QT_VERSION_MAJOR}::Network)

target_link_libraries(mess2P PRIVATE OpenSSL::Crypto)
target_link_libraries(mess2P PRIVATE OpenSSL::SSL)
