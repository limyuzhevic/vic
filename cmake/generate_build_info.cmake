# Build info generation script
# This script generates build_info.json with project configuration

# Parse build options and generate JSON configuration
function(generate_build_info)
    set(BUILD_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_info.json")
    file(WRITE ${BUILD_INFO_FILE} "{")
    file(APPEND ${BUILD_INFO_FILE} "\\\"project\\\":\\\"NLM\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"version\\\":\\\"${PROJECT_VERSION}\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_type\\\":\\\"${CMAKE_BUILD_TYPE}\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"optimization_level\\\":\\\"${OPTIMIZATION_LEVEL}\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_python_bindings\\\":${BUILD_PYTHON_BINDINGS},")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_tests\\\":${BUILD_TESTS},")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_experiments\\\":${BUILD_EXPERIMENTS},")
    file(APPEND ${BUILD_INFO_FILE} "\\\"enable_sanitizers\\\":${ENABLE_SANITIZERS}")
    file(APPEND ${BUILD_INFO_FILE} "}")
endfunction()

generate_build_info()
