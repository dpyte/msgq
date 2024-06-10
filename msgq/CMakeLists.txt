include_directories (logger)
include_directories (visionipc)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${PROJECT_SOURCE_DIR})

############################################################
## Msgq
set (Msgq_SOURCES
	ipc.cc ipc.h
	event.cc event.h
	impl_zmq.cc impl_zmq.h
	impl_fake.cc impl_fake.h
	msgq.cc msgq.h)

add_library (msgq ${Msgq_SOURCES})
target_link_libraries (msgq PRIVATE czmq czmq-static)
target_link_directories (msgq PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

############################################################
## VisionIPC
set (VisionIPC_SOURCES
	visionipc/visionipc.cc visionipc/visionipc.h
	visionipc/visionipc_server.cc visionipc/visionipc_server.h
	visionipc/visionipc_client.cc visionipc/visionipc_client.h
	visionipc/visionbuf.cc visionipc/visionbuf.h
	)

add_library (visionipc ${VisionIPC_SOURCES})
target_link_libraries (visionipc STATIC msgq)
target_link_directories (visionipc
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/visionipc/logger
)
