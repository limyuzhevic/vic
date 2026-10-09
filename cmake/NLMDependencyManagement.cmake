# NLM Dependency Management Module
# Provides automated dependency detection and management

# Function to find required dependencies
function(nlm_find_dependencies)
    # Find Python
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    
    # Find pybind11
    find_package(pybind11 CONFIG REQUIRED)
    
    # Platform-specific dependencies
    if(NLM_PLATFORM STREQUAL "Windows")
        # Windows-specific dependencies
        find_package(Boost REQUIRED COMPONENTS system thread)
        # Add Windows-specific library paths
        link_libraries(ws2_32 iphlpapi)
    elseif(NLM_PLATFORM STREQUAL "macOS")
        # macOS-specific dependencies
        find_package(Boost REQUIRED COMPONENTS system thread)
        # Accelerate framework for math operations
        find_library(ACCELERATE_LIBRARY Accelerate)
        if(ACCELERATE_LIBRARY)
            target_link_libraries(nlm_core PRIVATE ${ACCELERATE_LIBRARY})
        endif()
    elseif(NLM_PLATFORM STREQUAL "Linux")
        # Linux-specific dependencies
        find_package(Boost REQUIRED COMPONENTS system thread)
        # X11 for some graphical components
        find_package(X11 REQUIRED)
        target_link_libraries(nlm_core PRIVATE ${X11_LIBRARIES})
    endif()
    
    # Find package for configuration files
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        # Check for FFmpeg but don't fail if not found
        pkg_check_modules(FFMPEG libavcodec libavformat libavutil libswscale)
        if(FFMPEG_FOUND)
            target_link_libraries(nlm_core PRIVATE ${FFMPEG_LIBRARIES})
            target_include_directories(nlm_core PRIVATE ${FFMPEG_INCLUDE_DIRS})
        endif()
    endif()
endfunction()

# Call the dependency function
nlm_find_dependencies()