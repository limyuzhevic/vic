# NLM Optimization Module
# Provides build optimization features and configurations

# Parallel build configuration
if(NLM_ENABLE_DISTRIBUTED_BUILD)
    set(CMAKE_JOB_POOLS "compile=4;link=2")
    set(CMAKE_JOB_POOL_COMPILE "compile")
    set(CMAKE_JOB_POOL_LINK "link")
    set(CMAKE_BUILD_PARALLEL_LEVEL 4)
else()
    set(CMAKE_BUILD_PARALLEL_LEVEL 2)
endif()

# Link-time optimization
if(NLM_ENABLE_LTO AND NOT MSVC)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_CREATE_STATIC_LIBRARY TRUE)
endif()

# Code coverage support
if(NLM_ENABLE_COVERAGE AND NOT MSVC)
    if(MSVC)
        target_compile_options(nlm_core PRIVATE /coverage)
        target_link_libraries(nlm_core PRIVATE /coverage)
    else()
        target_compile_options(nlm_core PRIVATE --coverage)
        target_link_libraries(nlm_core PRIVATE --coverage)
    endif()
endif()

# Memory leak detection
if(NLM_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(nlm_core PRIVATE -fsanitize=address,undefined)
    target_link_libraries(nlm_core PRIVATE -fsanitize=address,undefined)
endif()

# Cross-platform optimization
if(NLM_PLATFORM STREQUAL "Windows")
    target_compile_definitions(nlm_core PRIVATE -D_CRT_SECURE_NO_WARNINGS)
elseif(NLM_PLATFORM STREQUAL "macOS")
    target_compile_options(nlm_core PRIVATE -stdlib=libc++)
elseif(NLM_PLATFORM STREQUAL "Linux")
    target_compile_options(nlm_core PRIVATE -pthread)
endif()