cmake_minimum_required(VERSION 2.8)
project(Depth2XYX )

# Prevent compilation in-source
if( ${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
  Message( " " )
  Message( FATAL_ERROR "Source and build  directories are the same.
 Create an empty build directory,
 change into it and re-invoke cmake")
endif()

# To use PKG_CHECK_MODULES to find some optional packages
FIND_PACKAGE(PkgConfig)

#    Tell CMake where to find the source files and where to leave executables
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
SET(SOURCE_FILE_DIR ${PROJECT_SOURCE_DIR}/src/)

IF(APPLE)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/macosx/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/macosx/)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/macosx/)
ELSEIF(UNIX)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/linux/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/linux/)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/linux/)
ELSEIF(WIN32)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/win32/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/win32/)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/win32/)
ENDIF()



#Enable C++11 support
SET(ENABLE_C11 ON CACHE BOOL "Compile with C++11")
MESSAGE("WARNING!! C++11 support has been enabled!!!")

if(ENABLE_C11)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-std=c++11")
else()
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

# Check for OpenCV like core, photo, xphoto, etc.
FIND_PACKAGE(OpenCV)
if (OpenCV_FOUND)
  message("OpenCV Found and the version is --  ${OpenCV_VERSION}")
  #SET(CIMG_CFLAGS "${CIMG_CFLAGS} ${CIMG_OPENCV_CCFLAGS}")
  include_directories(${OpenCV_INCLUDE_DIRS})
  link_directories(${OpenCV_LIB_DIRS})
  SET( SYSTEM_LIBS  ${SYSTEM_LIBS}  ${OpenCV_LIBS} )
endif()

# Add source files to a variable
SET(SRC ${SOURCE_FILE_DIR}/Depth2XYX.cpp
    ${SOURCE_FILE_DIR}/PlyFunctions.cpp
    ${SOURCE_FILE_DIR}/Transformation.cpp
)
add_executable( Depth2XYX  ${SRC})
target_link_libraries( Depth2XYX  ${SYSTEM_LIBS} )
