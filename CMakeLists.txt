cmake_minimum_required(VERSION 3.10)
project(EnhancedLogger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Logger library
add_library(logger
    src/core/Logger/Logger.hpp
    src/core/Logger/Logger.cpp
)

# Test executable
add_executable(test_logger
    test_enhanced_logger.cpp
)

target_link_libraries(test_logger PRIVATE logger)

# Installation
install(TARGETS logger
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES src/core/Logger/Logger.hpp
    DESTINATION include/core/Logger
)

# Enable testing
enable_testing()
add_test(NAME test_enhanced_logger
    COMMAND test_logger
)