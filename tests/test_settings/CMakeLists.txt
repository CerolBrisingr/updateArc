set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
qt_add_executable(test_settings
    tst_settings.cpp
    mock_window.cpp
    mock_window.h
)

target_link_libraries(test_settings 
    PRIVATE 
        UpdateArc_Core 
        Qt6::Test
)

deploy_qt_target(test_settings)
add_test(NAME test_settings COMMAND test_settings)