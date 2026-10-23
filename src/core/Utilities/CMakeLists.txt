add_library(core_utils STATIC
    CoreUtils.cpp
)

target_include_directories(core_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link core_utils with logger
find_package(nlm_core REQUIRED)
target_link_libraries(core_utils PUBLIC nlm_core)
