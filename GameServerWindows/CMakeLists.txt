project(GameServerWindows)

add_executable(GameServerWindows main.cpp)

target_link_libraries(GameServerWindows PRIVATE Engine)
target_compile_definitions(GameServerWindows PRIVATE GP_STATIC)


# Copy the Engine DLL to the executable directory after build
if(WIN32)
    add_custom_command(TARGET GameServerWindows POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE_DIR:GameServerWindows>
    )
endif()
