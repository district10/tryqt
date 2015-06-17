# Qt Console

### g++

```bash
g++ main.cpp \
-IC:\Qt\4.8.4\include \
-LC:\Qt\4.8.4\lib \
-lQtCore4 \
-o main
```


### qmake

hello.pro:
```
CONFIG += qt
QT -= gui
SOURCES += main.cpp
```

generate:
```
$ mkdir qmake && cd qmake && qmake.exe ../hello.pro
```

### cmake

CMakeLists.txt:
```
cmake_minimum_required( VERSION 3.2 )
project( HELLO )
find_package( Qt4 REQUIRED )
set( QT_DONT_USE_QTGUI TRUE )
include( ${QT_USE_FILE} )
add_executable( hello main.cpp)
target_link_libraries( hello ${QT_LIBRARIES} )
```

we can substitute
```
find_package( Qt4 REQUIRED )
set( QT_DONT_USE_QTGUI TRUE )
```
with
```
find_package(Qt4 COMPONENTS QtCore REQUIRED) # use only one component in this package
```

![](/data/qt-console-cmake.png)
![](/data/qt-console-vs.png)

```
1>QtCored4.lib(QtCored4.dll) : fatal error LNK1112: module machine type 'X86' conflicts with target machine type 'x64'
```

