# Conan integration for NLM

# Check if Conan is available
if(NLM_HAS_CONAN)
    # Configure Conan to use the required profile
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py)
        execute_process(
            COMMAND ${CONAN_EXECUTABLE} install ${CMAKE_CURRENT_SOURCE_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE CONAN_INSTALL_RESULT
        )
        if(NOT CONAN_INSTALL_RESULT EQUAL 0)
            message(WARNING "Conan installation failed. Continuing without dependencies.")
        else()
            # Add Conan-generated files to CMake search path
            set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake ${CMAKE_PREFIX_PATH})
            include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        endif()
    endif()
endif()
