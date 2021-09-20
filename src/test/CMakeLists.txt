######################
#  Thread Pool Test  #
######################

set(SRC_TEST ${SRC_TEST}
    ${CMAKE_CURRENT_SOURCE_DIR}/threadpool_ut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mdlist_ut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/queue_ut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stack_ut.cpp
    PARENT_SCOPE
    )
set(HDRS_TEST ${HDRS_TEST}
    PARENT_SCOPE
    )
