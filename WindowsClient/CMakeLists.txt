project(WindowsClient)

add_executable(WindowsClient 
    main.cpp
    Windows/WindowsPlatformWindow.cpp
    Input/RawInputHandler.cpp
)

target_include_directories(WindowsClient PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(WindowsClient PRIVATE Engine ClientShared)
target_compile_definitions(WindowsClient PRIVATE GP_STATIC CLIENT_STATIC)


# Copy the Engine DLL and Shaders to the executable directory after build
if(WIN32)
    add_custom_command(TARGET WindowsClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE_DIR:WindowsClient>
    )
    add_custom_command(TARGET WindowsClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_BINARY_DIR}/Shaders
        $<TARGET_FILE_DIR:WindowsClient>/Shaders
    )
endif()
