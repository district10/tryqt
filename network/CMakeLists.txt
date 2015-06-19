cmake_minimum_required( VERSION 2.6 )
project( LMS_READER )

add_executable( ${PROJECT_NAME} main.cpp lmsreader.cpp)
include_directories( ${CMAKE_SOURCE_DIR} )

find_package(Qt4 REQUIRED)
include( ${QT_USE_FILE} )
SET(QT_USE_QTNETWORK TRUE)
include_directories( "C:/Qt/qt-4.8.6-x64-msvc2010/qt-4.8.6-x64-msvc2010/include/QtNetwork" )

find_package(Boost REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )
set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)   # force dynamic linking for all libraries

target_link_libraries( ${PROJECT_NAME} ${QT_LIBRARIES} )
target_link_libraries( ${PROJECT_NAME} ${Boost_LIBRARIES} )