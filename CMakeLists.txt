cmake_minimum_required(VERSION 2.8.12)

PROJECT(SpringMassClothSim)

file(GLOB H_FILES *.h)
file(GLOB CPP_FILES *.cpp)

add_executable(SpringMassClothSim MACOSX_BUNDLE ${CPP_FILES} ${H_FILES})

find_path(FREEGLUT FreeGlut)
target_include_directories(SpringMassClothSim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${FREEGLUT}/include)
target_link_libraries(SpringMassClothSim ${FREEGLUT}/build/lib/Debug/freeglutd.lib)

if(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
	# Remove console in release
    set_target_properties(SpringMassClothSim PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
  endif()
endif()