cmake_minimum_required (VERSION 3.22)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
	CACHE STRING "Vcpkg toolchain file")

project (msgq CXX)
if (${CMAKE_SOURCE_DIR} STREQUAL {CMAKE_BINARY_DIR})
	message (FATAL_ERROR "In-source build is not allowed, please create a separate build folder.")
endif ()

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message (FATAL_ERROR "Cannot compile this project on 32-bit architecture")
endif()

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
	message (STATUS "Enabling extra debug info ...")
	set (ENABLE_EXTRA_DEBUG ON)
endif ()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message (STATUS "Setting build type to 'Debug' as none was specified.")
	set (CMAKE_BUILD_TYPE Debug
		CACHE STRING "Choose the type of build" FORCE)

	set_property(CACHE CMAKE_BUILD_TYPE
		PROPERTY STRINGS
		"Debug"
		"Release"
		"RelWithDebInfo")
endif ()

# The following comment serves zero purpose, and other than making the cmake script "easy" to read
############################################################
## print info
message (STATUS "build info:")
message (STATUS "  Host System: ${CMAKE_HOST_SYSTEM}")
message (STATUS "  Host System Name: ${CMAKE_HOST_SYSTEM_NAME}")
message (STATUS "  Host System Processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")


############################################################
# build options
option (ENABLE_ASAN "Turn on ASAN" OFF)
option (ENABLE_UBSAN "Turn on UBSan" OFF)
option (CLANG_TIDY "Enable clang-tidy" OFF)


############################################################
# compile commands
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)


############################################################
# ccache
find_program (CCACHE_FOUND ccache)
if (CCACHE_FOUND)
	message (STATUS "Using ccache")
	set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif ()


############################################################
## Compiler
set (ENABLE_DEBUG_MODE 0)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set (ENABLE_DEBUG_MODE 1)
endif ()

if (CMAKE_LTO)
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif ()

if (${ENABLE_EXTRA_DEBUG})
	set (CMAKE_CXX_FLAGS_DEBUG
		"${CMAKE_CXX_FLAGS_DEBUG} -Og -DDEBUG -Wall -Wnarrowing -Wconversion -pedantic -fPIC -Wno-vla")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
endif ()

if (CLANG_TIDY)
	set (CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

if (ENABLE_UBSAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-sanitize-recover=undefined")
endif ()

if (ENABLE_ASAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif ()

############################################################
# Configure output directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


############################################################
## check required libraries
find_package (czmq CONFIG REQUIRED)
find_package (OpenCL REQUIRED)

# TODO: Remove Python support. Python-bridge implementation can be found in https://github.com/dpyte/tunnel.git.
find_package (Python3 REQUIRED COMPONENTS Interpreter Development NumPy)

add_subdirectory (msgq)
