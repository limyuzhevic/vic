#!/bin/bash

# NLM Build Automation Script
# This script provides automated build and distribution capabilities

set -e

echo "=== NLM Build Automation ==="

# Function to print colored output
print_info() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

print_success() {
    echo -e "\033[1;32m[SUCCESS]\033[0m $1"
}

print_error() {
    echo -e "\033[1;31m[ERROR]\033[0m $1"
}

# Parse command line arguments
BUILD_TYPE="Release"
BUILD_DIR="build"
GENERATOR="Ninja"
INSTALL_PREFIX=""
PACKAGE_TYPE=""
ENABLE_COVERAGE=""
ENABLE_SANITIZERS=""
ENABLE_PROFILING=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --generator)
            GENERATOR="$2"
            shift 2
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --package)
            PACKAGE_TYPE="$2"
            shift 2
            ;;
        --coverage)
            ENABLE_COVERAGE="ON"
            shift
            ;;
        --sanitizers)
            ENABLE_SANITIZERS="ON"
            shift
            ;;
        --profiling)
            ENABLE_PROFILING="ON"
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --type TYPE          Build type (Release, Debug, RelWithDebInfo)"
            echo "  --build-dir DIR      Build directory"
            echo "  --generator GEN      CMake generator (Ninja, Unix Makefiles, etc.)"
            echo "  --prefix PREFIX      Installation prefix"
            echo "  --package TYPE       Package type (TGZ, DEB, RPM)"
            echo "  --coverage           Enable code coverage"
            echo "  --sanitizers         Enable sanitizers"
            echo "  --profiling          Enable profiling"
            echo "  --help               Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create build directory
create_build_directory() {
    print_info "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
}

# Configure CMake
configure_cmake() {
    print_info "Configuring CMake in $BUILD_DIR with type: $BUILD_TYPE"
    
    local cmake_args="-S . -B $BUILD_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE -G $GENERATOR"
    
    if [ -n "$ENABLE_COVERAGE" ]; then
        cmake_args="$cmake_args -DNLM_ENABLE_COVERAGE=ON"
    fi
    
    if [ -n "$ENABLE_SANITIZERS" ]; then
        cmake_args="$cmake_args -DNLM_ENABLE_SANITIZERS=ON"
    fi
    
    if [ -n "$ENABLE_PROFILING" ]; then
        cmake_args="$cmake_args -DNLM_ENABLE_PROFILING=ON"
    fi
    
    eval "cmake $cmake_args"
    print_success "CMake configured in $BUILD_DIR"
}

# Build the project
build_project() {
    print_info "Building project..."
    cmake --build "$BUILD_DIR" --parallel
    print_success "Build completed"
}

# Run tests
run_tests() {
    print_info "Running tests..."
    cd "$BUILD_DIR" && ctest --output-on-failure
    cd ..
    print_success "Tests completed"
}

# Install the project
install_project() {
    if [ -z "$INSTALL_PREFIX" ]; then
        print_error "Installation prefix not specified. Use --prefix option."
        exit 1
    fi
    
    print_info "Installing to $INSTALL_PREFIX..."
    cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX"
    print_success "Installation completed"
}

# Package the project
package_project() {
    if [ -z "$PACKAGE_TYPE" ]; then
        print_error "Package type not specified. Use --package option."
        exit 1
    fi
    
    print_info "Packaging project as $PACKAGE_TYPE..."
    
    case $PACKAGE_TYPE in
        TGZ)
            cmake --build "$BUILD_DIR" --target package_source
            ;;
        DEB)
            cmake --build "$BUILD_DIR" --target package
            ;;
        RPM)
            cmake --build "$BUILD_DIR" --target package
            ;;
        *)
            print_error "Unsupported package type: $PACKAGE_TYPE"
            exit 1
            ;;
    esac
    
    print_success "Package created"
}

# Generate documentation
generate_documentation() {
    print_info "Generating documentation..."
    cmake --build "$BUILD_DIR" --target docs
    print_success "Documentation generated"
}

# Show build information
show_build_info() {
    print_info "Build Information:"
    if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
        echo "Build type: $BUILD_TYPE"
        echo "Build directory: $BUILD_DIR"
        echo "Generator: $GENERATOR"
        
        grep -E "CMAKE_CXX_COMPILER|CMAKE_BUILD_TYPE|NLM_VERSION|BUILD_TESTS|BUILD_DOCS" "$BUILD_DIR/CMakeCache.txt" | head -10
    fi
}

# Main function
main() {
    create_build_directory
    configure_cmake
    build_project
    
    # Run tests by default unless specified otherwise
    if [ -z "$PACKAGE_TYPE" ] && [ -z "$INSTALL_PREFIX" ]; then
        run_tests
    fi
    
    if [ -n "$PACKAGE_TYPE" ]; then
        package_project
    elif [ -n "$INSTALL_PREFIX" ]; then
        install_project
    fi
    
    show_build_info
    
    if [ -n "$PACKAGE_TYPE" ] || [ -n "$INSTALL_PREFIX" ]; then
        print_info "Build and packaging completed successfully!"
    else
        print_info "Build and tests completed successfully!"
        print_info "\nNext steps:"
        print_info "  - Use './scripts/build.sh --package TGZ' to create source package"
        print_info "  - Use './scripts/build.sh --package DEB' to create DEB package"
        print_info "  - Use './scripts/build.sh --prefix /usr/local install' to install"
        print_info "  - Run tests with 'cd $BUILD_DIR && ctest --output-on-failure'"
    fi
}

# Execute main function
main