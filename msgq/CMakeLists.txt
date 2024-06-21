include_directories (logger)
include_directories (visionipc)
include_directories (
		${CMAKE_CURRENT_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}
		${Python3_INCLUDE_DIRS}
		${Python3_NumPy_INCLUDE_DIRS}
	)

############################################################
## Msgq
set (Msgq_SOURCES
	ipc.cc ipc.h
	event.cc event.h
	impl_zmq.cc impl_zmq.h
	impl_fake.cc impl_fake.h
	msgq.cc msgq.h)

add_library (msgq ${Msgq_SOURCES})
target_link_libraries (msgq PRIVATE czmq czmq-static OpenCL::OpenCL)
target_link_directories (msgq PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

############################################################
## VisionIPC
set (VisionIPC_SOURCES
	visionipc/visionipc.cc visionipc/visionipc.h
	visionipc/visionipc_server.cc visionipc/visionipc_server.h
	visionipc/visionipc_client.cc visionipc/visionipc_client.h
	visionipc/visionbuf.cc visionipc/visionbuf.h
		visionipc/visionbuf_cl.cc
	)

add_library (visionipc ${VisionIPC_SOURCES})
target_link_libraries (visionipc STATIC msgq)
target_link_directories (visionipc
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/visionipc/logger
	)

############################################################
## Cython files
set (CYTHON_FILES
	ipc_pyx.pyx
	visionipc/visionipc_pyx.pyx
)

foreach (file ${CYTHON_FILE})
	get_filename_component (name ${file} NAME_WE)
	add_custom_command (
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp
			COMMAND ${Python3_EXECUTABLE} -m cython ${file} -3 --cplus -o ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp
			DEPENDS ${file}
			COMMENT "Generating C++ source from ${file}"
	)

	add_library (${name}_pyx MODULE ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp)
	set_target_properties (${name}_pyx PROPERTIES PREFIX "" SUFFIX ".so")
	target_link_libraries (${name}_pyx PRIVATE ${Python3_LIBRARIES} ${Python3_NumPy_LIBRARIES}
			visionipc msgq czmq czmq-static OpenCL::OpenCL)
	if (APPLE)
		set_target_properties (${name}_pyx PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	else()
		target_link_libraries (${name}_pyx PRIVATE pthread)
	endif()
endforeach ()

# Add Python tests
enable_testing ()
add_test (NAME PythonMsgqTest
		COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test_msgq.py
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})