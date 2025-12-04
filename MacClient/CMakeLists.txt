project(MacClient)

add_executable(MacClient 
    main.cpp
    Mac/MacPlatformWindow.mm
    Input/MacInputHandler.mm
)

target_include_directories(MacClient PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MacClient PRIVATE Engine ClientShared)

# Link macOS frameworks
if(APPLE)
    target_link_libraries(MacClient PRIVATE
        "-framework Cocoa"
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
    )
endif()

# Set bundle properties for macOS app
set_target_properties(MacClient PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.gameplatform.macclient
    MACOSX_BUNDLE_BUNDLE_NAME "MacClient"
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
)

# Generate Xcode schemes
set_target_properties(MacClient PROPERTIES
    XCODE_GENERATE_SCHEME ON
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
