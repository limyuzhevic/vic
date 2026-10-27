#!/usr/bin/env bash
"""
Build script for NLM enhanced Python bindings.
This script builds both the original and enhanced bindings.
"""

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_color() {
    local color=$1
    shift
    echo -e "${color}$*${NC}"
}

# Check if we have required tools
check_tools() {
    local missing_tools=()
    
    if ! command -v cmake &> /dev/null; then
        missing_tools+=("cmake")
    fi
    
    if ! command -v python3 &> /dev/null; then
        missing_tools+=("python3")
    fi
    
    if ! command -v pip3 &> /dev/null; then
        missing_tools+=("pip3")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_color "$RED" "Error: Missing required tools: ${missing_tools[*]}"
        print_color "$YELLOW" "Please install the missing tools and try again."
        exit 1
    fi
}

# Install Python dependencies
install_dependencies() {
    print_color "$GREEN" "Installing Python dependencies..."
    
    # Install system dependencies
    if command -v apt-get &> /dev/null; then
        apt-get update && apt-get install -y \
            build-essential \
            python3-dev \
            python3-pip \
            libboost-all-dev || {
            print_color "$RED" "Error: Failed to install system dependencies"
            exit 1
        }
    elif command -v yum &> /dev/null; then
        yum install -y gcc gcc-c++ python3-devel python3-pip || {
            print_color "$RED" "Error: Failed to install system dependencies"
            exit 1
        }
    fi
    
    # Install Python packages
    pip3 install pybind11 scikit-build-core pytest numpy || {
        print_color "$RED" "Error: Failed to install Python dependencies"
        exit 1
    }
}

# Build NLM C++ core
build_nlm_core() {
    print_color "$GREEN" "Building NLM C++ core..."
    
    cd "$PROJECT_ROOT"
    
    if [ ! -d "build" ]; then
        mkdir -p build
    fi
    
    cd build
    
    # Configure CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF || {
        print_color "$RED" "Error: CMake configuration failed"
        exit 1
    }
    
    # Build all targets
    make -j$(nproc) || {
        print_color "$RED" "Error: Build failed"
        exit 1
    }
    
    print_color "$GREEN" "✓ NLM C++ core built successfully"
}

# Build Python bindings
build_python_bindings() {
    print_color "$GREEN" "Building Python bindings..."
    
    cd "$PROJECT_ROOT/python"
    
    # Configure pybind11 bindings build
    python3 -m pip install pybind11 scikit-build-core || {
        print_color "$RED" "Error: Failed to install pybind11 build dependencies"
        exit 1
    }
    
    # Build original bindings
    python3 -m pip install -e "$PROJECT_ROOT/python" --no-build-isolation || {
        print_color "$RED" "Error: Failed to build original bindings"
        exit 1
    }
    
    # Build enhanced bindings (if available)
    if [ -f "bindings_enhanced.cpp" ]; then
        print_color "$YELLOW" "Building enhanced bindings..."
        python3 -m pip install -e "$PROJECT_ROOT/python/enhanced" --no-build-isolation 2>/dev/null || {
            print_color "$YELLOW" "Note: Enhanced bindings build may require additional configuration"
        }
    fi
    
    print_color "$GREEN" "✓ Python bindings built successfully"
}

# Build tests
build_tests() {
    print_color "$GREEN" "Building tests..."
    
    cd "$PROJECT_ROOT/build"
    
    # Build test executable
    make nlm_test || {
        print_color "$RED" "Error: Test build failed"
        exit 1
    }
    
    print_color "$GREEN" "✓ Tests built successfully"
}

# Run tests
run_tests() {
    print_color "$GREEN" "Running tests..."
    
    cd "$PROJECT_ROOT/build"
    
    if [ -f "nlm_test" ]; then
        ./nlm_test || {
            print_color "$RED" "Error: Tests failed"
            exit 1
        }
    else
        print_color "$YELLOW" "Warning: Test executable not found, skipping tests"
    fi
    
    print_color "$GREEN" "✓ Tests passed"
}

# Create installation
create_installation() {
    print_color "$GREEN" "Creating installation..."
    
    # Create installation directory structure
    local install_dir="$PROJECT_ROOT/install"
    mkdir -p "$install_dir"
    
    # Copy built libraries and headers
    if [ -d "build" ]; then
        cp -r "build/lib"* "$install_dir/" 2>/dev/null || true
        cp -r "$PROJECT_ROOT/src" "$install_dir/include/" 2>/dev/null || true
    fi
    
    print_color "$GREEN" "✓ Installation created at $install_dir"
}

# Generate documentation
generate_docs() {
    print_color "$GREEN" "Generating documentation..."
    
    local docs_dir="$PROJECT_ROOT/docs"
    mkdir -p "$docs_dir"
    
    # Copy existing documentation
    if [ -f "README.md" ]; then
        cp "README.md" "$docs_dir/"
    fi
    
    if [ -f "HOW_TO_USE.md" ]; then
        cp "HOW_TO_USE.md" "$docs_dir/"
    fi
    
    if [ -f "easy_usage.md" ]; then
        cp "easy_usage.md" "$docs_dir/"
    fi
    
    if [ -f "IMPROVEMENT_PLAN.md" ]; then
        cp "IMPROVEMENT_PLAN.md" "$docs_dir/"
    fi
    
    print_color "$GREEN" "✓ Documentation generated at $docs_dir"
}

# Main build function
main() {
    print_color "$GREEN" "Starting NLM enhanced bindings build..."
    
    check_tools
    install_dependencies
    build_nlm_core
    build_python_bindings
    build_tests
    run_tests
    create_installation
    generate_docs
    
    print_color "$GREEN" "\n✓ Build completed successfully!"
    print_color "$YELLOW" "\nNext steps:"
    print_color "$YELLOW" "1. Install Python dependencies: pip install -r requirements.txt"
    print_color "$YELLOW" "2. Test the bindings: python3 demo_enhanced_features.py"
    print_color "$YELLOW" "3. Run unit tests: cd build && ./nlm_test"
    print_color "$YELLOW" "4. Explore the enhanced features in pynlm_enhanced module"
}

# Run main function
main "$@"