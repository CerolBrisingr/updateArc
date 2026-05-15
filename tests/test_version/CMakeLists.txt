set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
qt_add_executable(test_version
    tst_version.cpp
)

target_link_libraries(test_version 
    PRIVATE 
        UpdateArc_Core 
        Qt6::Test
)

deploy_qt_target(test_version)
add_test(NAME test_version COMMAND test_version)