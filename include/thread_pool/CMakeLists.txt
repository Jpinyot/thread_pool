###########################
#   Thread Pool headers   #
###########################

set (SOURCES ${SOURCES}
	PARENT_SCOPE
)

set(HEADERS ${HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/threadpool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mdlist.h
	PARENT_SCOPE
)
