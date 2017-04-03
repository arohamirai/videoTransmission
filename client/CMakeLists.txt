cmake_minimum_required(VERSION 2.8)

project(client)

include(CheckCXXCompilerFlag)  
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)  
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)  
if(COMPILER_SUPPORTS_CXX11)  
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")  
elseif(COMPILER_SUPPORTS_CXX0X)  
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")  
else()  
     message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")  
endif() 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")


find_package(OpenCV REQUIRED)
find_package(cnettran REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CNETTRAN_INCLUDE_DIR})



add_executable(client client.cpp
)

target_link_libraries(client
	${CNETTRAN_LIBRARY}
	${OpenCV_LIBS}
)
