set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

qt_standard_project_setup()

qt_add_executable(UpdateArc)

target_link_libraries(UpdateArc
    PUBLIC
        project_warnings
        Qt6::Gui
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
)

target_include_directories(UpdateArc
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/includes)

set_target_properties(UpdateArc PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

if (WIN32)
    # Find windeployqt via location of qmake
    get_target_property(WINDEPLOYQT_EXECUTABLE Qt6::windeployqt IMPORTED_LOCATION)

    # Run windeployqt after build
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET UpdateArc POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --no-compiler-runtime
                    --verbose 1
                    "$<TARGET_FILE:UpdateArc>"
            COMMENT "Running windeployqt from ${WINDEPLOYQT_EXECUTABLE}"
        )
    else()
        message(WARNING "windeployqt was not found. Will skip deployment of plugins and DLLs")
    endif()
endif()

target_sources(UpdateArc
    PRIVATE 
        ${VERSION_FILES}

        sources/main.cpp
	    sources/helper/downloader.cpp
		sources/helper/fileinteractions.cpp
		sources/helper/logger.cpp
		sources/helper/settings.cpp
		sources/helper/simple_json.cpp
		sources/helper/version.cpp
        sources/config/githup_config.cpp
		sources/installer/install_blishhud.cpp
		sources/installer/install_boontable.cpp
		sources/installer/install_kp.cpp
		sources/installer/installer.cpp
		sources/ui/form.cpp
        sources/ui/form.ui
		sources/ui/mainwindow.cpp
        sources/ui/mainwindow.ui
		sources/updater/arcupdater.cpp
		sources/updater/baseupdater.cpp
		sources/updater/githupdater.cpp
		sources/updater/updater.cpp

        includes/helper/downloader.h
	    includes/helper/logger.h
	    includes/helper/settings.h
	    includes/ui/form.h
	    includes/ui/mainwindow.h
	    includes/updater/arcupdater.h
	    includes/updater/baseupdater.h
	    includes/updater/githupdater.h
	    includes/updater/updater.h
)

install(TARGETS UpdateArc
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET UpdateArc
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_COMPILER_RUNTIME
    DEPLOY_TOOL_OPTIONS "--no-opengl-sw" "--no-system-d3d-compiler"
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})

install(CODE "
    message(STATUS \"Removing Shader-Compiler excess...\")
    file(REMOVE 
        \"\${CMAKE_INSTALL_PREFIX}/bin/dxcompiler.dll\"
        \"\${CMAKE_INSTALL_PREFIX}/bin/dxil.dll\"
    )
")
