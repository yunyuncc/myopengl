CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
FIND_PACKAGE(doctest REQUIRED)
set(test_progs string_test error_test)
message("--------------")
FOREACH(test_prog ${test_progs})
	ADD_EXECUTABLE(${test_prog} ${CMAKE_CURRENT_LIST_DIR}/${test_prog}.cpp)
	TARGET_LINK_LIBRARIES(${test_prog} PRIVATE doctest::doctest)
	TARGET_LINK_LIBRARIES(${test_prog} PRIVATE myopengl)
	set(UNIT_TEST ${test_prog})
        add_custom_command(
             TARGET ${UNIT_TEST}
             COMMENT "Run tests"
             POST_BUILD
             COMMAND ${UNIT_TEST}
        )
ENDFOREACH()

