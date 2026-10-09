project(NLMDocs LANGUAGES CXX)

# Doxygen documentation
find_package(Doxygen REQUIRED)

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)

# Doxygen configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

# Add custom target for documentation generation
add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)