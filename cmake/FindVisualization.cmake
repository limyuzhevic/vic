# Find OpenGL libraries
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(glm REQUIRED)

# Include nlohmann/json for JSON serialization
find_package(nlohmann_json REQUIRED)

# Check if we have the required libraries
if (NOT OPENGL_FOUND)
    message(WARNING "OpenGL not found - visualization features will be limited")
endif()

if (NOT GLFW_FOUND)
    message(WARNING "GLFW not found - windowing features will be limited")
endif()

# Visualization system configuration
if (OPENGL_FOUND AND GLFW_FOUND)
    message(STATUS "Advanced visualization system enabled with OpenGL and GLFW")
else()
    message(STATUS "Basic visualization mode - limited OpenGL/GLFW support")
endif()

# Set visualization-related compile definitions
if (OPENGL_FOUND)
    add_definitions(-DHAVE_OPENGL)
endif()

if (GLFW_FOUND)
    add_definitions(-DHAVE_GLFW)
endif()

add_definitions(-DHAVE_ADVANCED_VISUALIZATION)
