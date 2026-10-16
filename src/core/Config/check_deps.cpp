#if defined _WIN32
    #pragma message("Windows: Install dependencies via vcpkg or manual setup")
#elif defined __linux__
    #pragma message("Linux: Install dependencies via package manager")
    # Try to install:
    #   sudo apt-get update && sudo apt-get install -y nlohmann-json3-dev libyaml-cpp-dev
    # Or using vcpkg
    #   vcpkg install nlohmann-json yaml-cpp
#else
    #pragma message("macOS: Install dependencies via Homebrew or similar")
    #   brew install nlohmann-json yaml-cpp
#endif

// Alternative: Bundled version for development
// Add to CMakeLists.txt:
// if (NOT TARGET nlohmann_json AND NOT TARGET yaml-cpp)
//   add_subdirectory(deps/json)
//   add_subdirectory(deps/yaml-cpp)
// endif()

// Check for package managers and guide user
#include <iostream>

void check_dependencies() {
    std::cout << "=== Kilo Configuration System Enhancement ===" << std::endl;
    std::cout << std::endl;
    std::cout << "This implementation adds JSON and YAML support to the NLM configuration system." << std::endl;
    std::cout << std::endl;
    
#if defined _WIN32
    std::cout << "Windows users: Install via vcpkg:" << std::endl;
    std::cout << "  vcpkg install nlohmann-json yaml-cpp" << std::endl;
    std::cout << std::endl;
    std::cout << "Or download from:" << std::endl;
    std::cout << "  - https://github.com/nlohmann/json/releases" << std::endl;
    std::cout << "  - https://github.com/yaml-cpp/yaml-cpp/releases" << std::endl;
#elif defined __linux__
    std::cout << "Linux users:" << std::endl;
    std::cout << "  Ubuntu/Debian: sudo apt-get install nlohmann-json3-dev libyaml-cpp-dev" << std::endl;
    std::cout << "  Fedora/CentOS: sudo dnf install nlohmann-json-devel libyaml-cpp-devel" << std::endl;
    std::cout << "  Or using vcpkg: vcpkg install nlohmann-json yaml-cpp" << std::endl;
#elif defined __APPLE__
    std::cout << "macOS users:" << std::endl;
    std::cout << "  brew install nlohmann-json yaml-cpp" << std::endl;
    std::cout << "  Or using vcpkg: vcpkg install nlohmann-json yaml-cpp" << std::endl;
#endif
    
    std::cout << std::endl;
    std::cout << "Build Instructions:" << std::endl;
    std::cout << "1. Configure CMake to use found packages:" << std::endl;
    std::cout << "   - find_package(nlohmann_json REQUIRED)" << std::endl;
    std::cout << "   - find_package(yaml-cpp REQUIRED)" << std::endl;
    std::cout << "2. Update target_link_libraries:" << std::endl;
    std::cout << "   - target_link_libraries(nlm_core PUBLIC nlohmann_json::nlohmann_json yaml-cpp)" << std::endl;
    std::cout << "3. Add include directories:" << std::endl;
    std::cout << "   - target_include_directories(nlm_core PUBLIC ${NLM_SOURCE_DIR}/config)" << std::endl;
}

// Include the header to demonstrate functionality
#include "config/Config.hpp"

int main() {
    check_dependencies();
    return 0;
}
