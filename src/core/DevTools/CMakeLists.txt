# DevTools CMakeLists.txt

# NLM DevTools library
add_library(nlm_devtools STATIC
    core/DevTools/NLMDevTools.cpp
)

target_link_libraries(nlm_devtools PUBLIC nlm_core)

# Add dependencies for DevTools-specific functionality
target_include_directories(nlm_devtools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Install DevTools library (future)
install(TARGETS nlm_devtools
    ARCHIVE DESTINATION lib
)
