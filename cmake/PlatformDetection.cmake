# Platform Detection and Configuration Module
# Provides platform-specific configurations and optimizations for NLM build system.

# Detect and configure for different operating systems
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(NLM_PLATFORM_LINUX 1)
    message(STATUS "Platform: Linux")
    
    # Linux-specific configurations
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)  # Force PIC for shared libraries
    
    # Use standard Linux file paths
    set(NLM_SYSTEM_LIB_DIR "lib")
    set(NLM_SYSTEM_BIN_DIR "bin")
    
    # Linux compiler optimizations
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_REENTRANT -fstack-protector-strong")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
    endif()
    
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(NLM_PLATFORM_WINDOWS 1)
    message(STATUS "Platform: Windows")
    
    # Windows-specific configurations
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_WIN32_LEAN_AND_MEAN)
    set(NLM_SYSTEM_LIB_DIR "lib")
    set(NLM_SYSTEM_BIN_DIR ".")
    
    # Windows compiler optimizations
    if(MSVC)
        # Windows-specific optimizations
        add_compile_options(/guard:cf)  # Control Flow Guard
        add_link_options(/NXCOMPAT)     # NX Compatibility
    endif()
    
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(NLM_PLATFORM_MACOS 1)
    message(STATUS "Platform: macOS")
    
    # macOS-specific configurations
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib @loader_path/../lib")
    
    # macOS compiler optimizations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-dead_strip_dylibs")
    
else()
    message(WARNING "Unknown platform: ${CMAKE_SYSTEM_NAME}")
    set(NLM_PLATFORM_UNKNOWN 1)
endif()

# Architecture detection
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(NLM_ARCH_X64 1)
    message(STATUS "Architecture: x64")
else()
    set(NLM_ARCH_X86 1)
    message(STATUS "Architecture: x86")
endif()

# CPU features detection
include(CheckCXXCompilerFlag)

# SSE optimizations
check_cxx_compiler_flag(-msse4.1 HAS_SSE4_1)
if(HAS_SSE4_1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
    add_definitions(-DNLM_SSE4_AVAILABLE)
endif()

# AVX optimizations (if supported)
check_cxx_compiler_flag(-mavx HAS_AVX)
if(HAS_AVX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
    add_definitions(-DNLM_AVX_AVAILABLE)
endif()

# Threading configuration
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Memory model configuration
if(NLM_ARCH_X64)
    add_definitions(-DNLM_64BIT)
else()
    add_definitions(-DNLM_32BIT)
endif()

# Performance tuning options
option(NLM_OPTIMIZE_FOR_PERFORMANCE "Optimize build for maximum performance" ON)
option(NLM_OPTIMIZE_FOR_SIZE "Optimize build for minimal size" OFF)

if(NLM_OPTIMIZE_FOR_PERFORMANCE)
    message(STATUS "Performance optimizations enabled")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=native -flto -fomit-frame-pointer -fno-inline-functions-called-once)
        add_link_options(-flto)
    endif()
endif()

if(NLM_OPTIMIZE_FOR_SIZE)
    message(STATUS "Size optimizations enabled")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Os -ffunction-sections -fdata-sections)
        add_link_options(--gc-sections)
    endif()
endif()

# RPATH configuration for shared libraries
if(NLM_PLATFORM_MACOS)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
elseif(NLM_PLATFORM_LINUX)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

# System library search paths
set(SYSTEM_LIBRARY_PATH)

if(NLM_PLATFORM_LINUX)
    list(APPEND SYSTEM_LIBRARY_PATH /usr/lib/x86_64-linux-gnu)
elseif(NLM_PLATFORM_MACOS)
    list(APPEND SYSTEM_LIBRARY_PATH /usr/lib)
elseif(NLM_PLATFORM_WINDOWS)
    # Windows system libraries
endif()

# Export platform configuration
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/PlatformConfig.cmake.in
    ${CMAKE_BINARY_DIR}/nlm/PlatformConfig.cmake
    @ONLY
)

message(STATUS "Platform detection complete")
