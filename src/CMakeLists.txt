#########################
#  Thread Pool Sources  #
#########################

if (${ENABLE_TESTS})
    add_subdirectory(test)
    set(SRC_TEST ${SRC_TEST}
        PARENT_SCOPE
        )
    set(HDRS_TEST ${HDRS_TEST}
        PARENT_SCOPE
        )
endif()

set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/threadpool.cpp
    PARENT_SCOPE)

set(HEADERS ${HEADERS}
    PARENT_SCOPE
    )
