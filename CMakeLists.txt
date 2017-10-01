cmake_minimum_required(VERSION 2.8.12)
 
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

cmake_policy(SET CMP0053 OLD)
 
PROJECT(HW)

file(GLOB H_FILES *.h)
file(GLOB CPP_FILES *.cpp)

add_executable(HW MACOSX_BUNDLE ${CPP_FILES} ${H_FILES})

find_path(FREEGLUT FreeGlut)
target_include_directories(HW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${FREEGLUT}/include)
target_link_libraries(HW ${FREEGLUT}/build/lib/Debug/freeglutd.lib)

if(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    set_target_properties(HW PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
  endif()
endif()