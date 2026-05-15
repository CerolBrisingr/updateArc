set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
qt_add_executable(test_downloader tst_downloader.cpp)

target_link_libraries(test_downloader 
    PRIVATE 
        UpdateArc_Core 
        Qt6::Test
)

deploy_qt_target(test_downloader)
add_test(NAME test_downloader COMMAND test_downloader)