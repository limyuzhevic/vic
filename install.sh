#!/bin/bash
# NLM Installation Script
# Comprehensive installation script for NLM with support for different installation modes

# Configuration
set -euo pipefail

# Default configuration
NLM_VERSION="0.1.0"
INSTALL_PREFIX="/usr/local"
BUILD_TYPE="Release"
GENERATOR="Ninja"
BUILD_DIR="build"
COMPONENTS="all"
VERBOSE=false
FORCE=false

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging
exec 1> >(tee -a "install.log")
exec 2>&1

# Print functions
print_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

print_debug() {
    if $VERBOSE; then
        echo -e "${BLUE}[DEBUG]${NC} $*"
    fi
}

# Print help
print_help() {
    cat << EOF
NLM Installation Script

Usage: $0 [OPTIONS]

Options:
  --prefix PREFIX             Installation prefix (default: /usr/local)
  --build-dir DIR             Build directory (default: build)
  --build-type TYPE            Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)
  --generator GEN              CMake generator (default: Ninja if available, otherwise Unix Makefiles)
  --components MODE            Components mode: all, core, dev, python, cli, tests, docs (default: all)
  --verbose                    Enable verbose output
  --force                      Force installation (overwrite existing)
  --dry-run                    Show what would be installed without actually installing
  --help                       Show this help message

Installation Modes:
  1. All components (default): Core libraries, executables, Python bindings, CLI, tests, docs
  2. Core only: Only essential core libraries
  3. Development: Core libraries with headers and documentation
  4. Python bindings: Only Python bindings
  5. Custom: Specific component selection

Examples:
  $0                                    # Install all components
  $0 --prefix /opt/nlm                   # Install to /opt/nlm
  $0 --components core                    # Install only core components
  $0 --build-type Debug --dev             # Debug development build
  $0 --dry-run                           # Show what would be installed
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --generator)
            GENERATOR="$2"
            shift 2
            ;;
        --components)
            COMPONENTS="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --force)
            FORCE=true
            shift
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        --help)
            print_help
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            print_help
            exit 1
            ;;
    esac
done

# Validate build type
valid_build_types="Debug Release RelWithDebInfo MinSizeRel"
if [[ ! " $valid_build_types " =~ " $BUILD_TYPE " ]]; then
    print_error "Invalid build type: $BUILD_TYPE"
    print_error "Valid build types: $valid_build_types"
    exit 1
fi

# Check for dry run
dry_run="${DRY_RUN:-false}"

# Detect CMake generator
if [[ "$GENERATOR" == "Ninja" ]] && ! command -v ninja &> /dev/null; then
    print_warn "Ninja not found, using Unix Makefiles"
    GENERATOR="Unix Makefiles"
fi

# Check if Ninja is available and prefer it
if command -v ninja &> /dev/null; then
    GENERATOR="Ninja"
fi

# Function to create build directory
create_build_dir() {
    local build_dir="$1"
    if [[ ! -d "$build_dir" ]]; then
        print_info "Creating build directory: $build_dir"
        mkdir -p "$build_dir"
    else
        if $FORCE; then
            print_warn "Force rebuilding in existing directory: $build_dir"
        else
            print_info "Using existing build directory: $build_dir"
        fi
    fi
}

# Function to configure CMake
configure_cmake() {
    local build_dir="$1"
    local build_type="$2"
    local generator="$3"
    
    print_info "Configuring CMake in $build_dir..."
    print_debug "Generator: $generator, Build type: $build_type"
    
    cd "$build_dir"
    
    local cmake_args=(
        "-S" ".."
        "-B" "."
        "-G" "$generator"
        "-DCMAKE_BUILD_TYPE=$build_type"
        "-DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
    )
    
    # Component-specific CMake options
    case "$COMPONENTS" in
        "core")
            cmake_args+=(
                "-DNLM_BUILD_TESTS=OFF"
                "-DNLM_BUILD_PYTHON_BINDINGS=OFF"
                "-DNLM_BUILD_CLI=OFF"
                "-DNLM_BUILD_DEMOS=OFF"
                "-DNLM_BUILD_MINIMAL=ON"
            )
            ;;
        "dev")
            cmake_args+=(
                "-DNLM_BUILD_TESTS=ON"
                "-DNLM_BUILD_PYTHON_BINDINGS=ON"
                "-DNLM_BUILD_CLI=ON"
                "-DNLM_BUILD_DEMOS=ON"
                "-DNLM_BUILD_DEV=ON"
            )
            ;;
        "python")
            cmake_args+=(
                "-DNLM_BUILD_TESTS=OFF"
                "-DNLM_BUILD_PYTHON_BINDINGS=ON"
                "-DNLM_BUILD_CLI=OFF"
                "-DNLM_BUILD_DEMOS=OFF"
            )
            ;;
        "cli")
            cmake_args+=(
                "-DNLM_BUILD_TESTS=OFF"
                "-DNLM_BUILD_PYTHON_BINDINGS=OFF"
                "-DNLM_BUILD_CLI=ON"
                "-DNLM_BUILD_DEMOS=OFF"
            )
            ;;
        "tests")
            cmake_args+=(
                "-DNLM_BUILD_TESTS=ON"
                "-DNLM_BUILD_PYTHON_BINDINGS=OFF"
                "-DNLM_BUILD_CLI=OFF"
                "-DNLM_BUILD_DEMOS=OFF"
            )
            ;;
        "docs")
            cmake_args+=(
                "-DNLM_BUILD_TESTS=OFF"
                "-DNLM_BUILD_PYTHON_BINDINGS=OFF"
                "-DNLM_BUILD_CLI=OFF"
                "-DNLM_BUILD_DEMOS=OFF"
                "-DNLM_BUILD_DOC=ON"
            )
            ;;
        "all" | *)
            cmake_args+=(
                "-DNLM_BUILD_TESTS=ON"
                "-DNLM_BUILD_PYTHON_BINDINGS=ON"
                "-DNLM_BUILD_CLI=ON"
                "-DNLM_BUILD_DEMOS=ON"
                "-DNLM_BUILD_FULL=ON"
            )
            ;;
    esac
    
    # Run CMake
    if $dry_run; then
        print_info "[DRY RUN] Would run: cmake ${cmake_args[*]}"
    else
        if cmake "${cmake_args[@]}"; then
            print_info "CMake configuration completed successfully"
        else
            print_error "CMake configuration failed"
            exit 1
        fi
    fi
}

# Function to build project
build_project() {
    local build_dir="$1"
    
    print_info "Building project in $build_dir..."
    
    cd "$build_dir"
    
    local build_args=("cmake" "--build" "." "--" "-j$(nproc)")
    
    if $dry_run; then
        print_info "[DRY RUN] Would run: ${build_args[*]}"
    else
        if cmake "${build_args[@]}"; then
            print_info "Build completed successfully"
        else
            print_error "Build failed"
            exit 1
        fi
    fi
}

# Function to install project
install_project() {
    local build_dir="$1"
    local install_prefix="$2"
    
    print_info "Installing project to $install_prefix..."
    
    cd "$build_dir"
    
    local install_args=("cmake" "--install" "." "--prefix" "$install_prefix")
    
    if $dry_run; then
        print_info "[DRY RUN] Would run: ${install_args[*]}"
        print_info "Would install to: $install_prefix"
        
        # Show what would be installed
        if [[ -d "$install_prefix/bin" ]]; then
            echo "Binaries would be installed to: $install_prefix/bin"
            ls -la "$install_prefix/bin/" 2>/dev/null || echo "No binaries found"
        fi
        
        if [[ -d "$install_prefix/lib" ]]; then
            echo "Libraries would be installed to: $install_prefix/lib"
            ls -la "$install_prefix/lib/" 2>/dev/null || echo "No libraries found"
        fi
        
        if [[ -d "$install_prefix/include" ]]; then
            echo "Headers would be installed to: $install_prefix/include"
            ls -la "$install_prefix/include/" 2>/dev/null || echo "No headers found"
        fi
        
        return 0
    fi
    
    if cmake "${install_args[@]}"; then
        print_info "Installation completed successfully"
    else
        print_error "Installation failed"
        exit 1
    fi
}

# Function to run tests
run_tests() {
    local build_dir="$1"
    
    if [[ "$COMPONENTS" == "tests" || "$COMPONENTS" == "all" || "$COMPONENTS" == "dev" ]]; then
        print_info "Running tests..."
        
        cd "$build_dir"
        
        local test_args=("ctest" "--output-on-failure")
        
        if $dry_run; then
            print_info "[DRY RUN] Would run: ${test_args[*]}"
        else
            if ctest "${test_args[@]}"; then
                print_info "Tests completed successfully"
            else
                print_warn "Tests failed (continuing with installation)"
            fi
        fi
    else
        print_info "Skipping tests (not in components mode)"
    fi
}

# Function to create package
create_package() {
    local build_dir="$1"
    
    if [[ -f "$build_dir/CPackConfig.cmake" ]] || $dry_run; then
        print_info "Creating package..."
        
        cd "$build_dir"
        
        local package_args=("cpack" "--config" "CPackConfig.cmake")
        
        if $dry_run; then
            print_info "[DRY RUN] Would run: ${package_args[*]}"
            print_info "Package would be created in: $(pwd)/package"
        else
            if cpack "${package_args[@]}"; then
                print_info "Package created successfully"
                print_info "Package location: $(pwd)/package"
            else
                print_warn "Package creation failed (continuing with installation)"
            fi
        fi
    else
        print_info "Skipping packaging (CPack not enabled)"
    fi
}

# Function to show installation summary
show_summary() {
    local install_prefix="$1"
    local build_dir="$2"
    
    echo ""
    print_info "Installation Summary"
    echo "==================="
    print_info "Installation prefix: $install_prefix"
    print_info "Build type: $BUILD_TYPE"
    print_info "Components: $COMPONENTS"
    print_info "Build directory: $build_dir"
    echo ""
    
    if [[ -d "$install_prefix/bin" ]]; then
        echo "Binaries:"
        find "$install_prefix/bin" -type f -executable 2>/dev/null | head -10 | while read -r file; do
            echo "  $file"
        done
    fi
    
    if [[ -d "$install_prefix/lib" ]]; then
        echo "Libraries:"
        find "$install_prefix/lib" -name "*.so" -o -name "*.a" -o -name "*.dylib" 2>/dev/null | head -10 | while read -r lib; do
            echo "  $lib"
        done
    fi
    
    if [[ -d "$install_prefix/include" ]]; then
        echo "Headers:"
        find "$install_prefix/include" -name "*.hpp" -o -name "*.h" 2>/dev/null | head -10 | while read -r header; do
            echo "  $header"
        done
    fi
    
    echo ""
    print_info "To add NLM to your PATH, run:"
    echo "  export PATH=\"$install_prefix/bin:\$PATH\""
    echo ""
    print_info "To compile with NLM, run:"
    echo "  export CPLUS_INCLUDE_PATH=\"$install_prefix/include:\$CPLUS_INCLUDE_PATH\""
    echo "  export LD_LIBRARY_PATH=\"$install_prefix/lib:\$LD_LIBRARY_PATH\""
    echo ""
}

# Main installation process
main() {
    print_info "Starting NLM installation"
    print_info "Version: $NLM_VERSION"
    print_info "System: $(uname -s) $(uname -m)"
    print_info "Components: $COMPONENTS"
    print_info "Build directory: $BUILD_DIR"
    print_info "Generator: $GENERATOR"
    
    # Check for dry run
    if $dry_run; then
        print_warn "DRY RUN: No changes will be made"
    fi
    
    # Create build directory
    create_build_dir "$BUILD_DIR"
    
    # Configure CMake
    configure_cmake "$BUILD_DIR" "$BUILD_TYPE" "$GENERATOR"
    
    # Build project
    build_project "$BUILD_DIR"
    
    # Run tests
    run_tests "$BUILD_DIR"
    
    # Install project
    if $dry_run; then
        install_project "$BUILD_DIR" "$INSTALL_PREFIX"
    else
        install_project "$BUILD_DIR" "$INSTALL_PREFIX"
    fi
    
    # Create package
    create_package "$BUILD_DIR"
    
    # Show summary
    show_summary "$INSTALL_PREFIX" "$BUILD_DIR"
    
    print_info "Installation completed successfully"
}

# Run main function
main
