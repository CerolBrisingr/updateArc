function(deploy_qt_target TARGET_NAME)
    if (WIN32)
        # 1. Pfad zu windeployqt auflösen
        get_target_property(WINDEPLOYQT_EXECUTABLE Qt6::windeployqt IMPORTED_LOCATION)

        if(WINDEPLOYQT_EXECUTABLE)
            # 2. Deployment-Befehl hinzufügen
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                        --no-compiler-runtime
                        --no-opengl-sw
                        --verbose 1
                        "$<TARGET_FILE:${TARGET_NAME}>"
                COMMENT "Deploying Qt dependencies for ${TARGET_NAME} using windeployqt..."
            )
        else()
            message(WARNING "windeployqt not found for target ${TARGET_NAME}. Skipping deployment.")
        endif()
    endif()
endfunction()