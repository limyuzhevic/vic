# Vcpkg integration for NLM

# Check if vcpkg is available
if(NLM_HAS_VCPKG AND NOT DEFINED ENV{VCPKG_ROOT})
    # Try to find vcpkg in common locations
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(VCPKG_ROOT "C:/vcpkg" CACHE PATH "VCPKG root directory")
    else()
        set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg" CACHE PATH "VCPKG root directory")
        execute_process(
            COMMAND ${VCPKG_EXECUTABLE} install --triplet=x64-linux
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE VCPKG_INIT_RESULT
        )
        if(NOT VCPKG_INIT_RESULT EQUAL 0)
            message(WARNING "Vcpkg initialization failed. Continuing without dependencies.")
        endif()
    endif()
endif()

if(DEFINED ENV{VCPKG_ROOT} OR VCPKG_ROOT)
    # Add vcpkg-generated toolchain file
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()
