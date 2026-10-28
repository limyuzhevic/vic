#!/usr/bin/env bash
"""
Setup script for NLM Enhanced Python API
Helps users install and configure Phase 7 advanced features
"""

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function print_header() {
    echo -e "${GREEN}========================================================${NC}"
    echo -e "${GREEN}NLM Enhanced Python API Setup Script${NC}"
    echo -e "${GREEN}========================================================${NC}"
}

function print_step() {
    echo -e "${YELLOW}[STEP] $1${NC}"
}

function print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

function print_error() {
    echo -e "${RED}✗ $1${NC}"
}

function check_python() {
    print_step "Checking Python environment..."
    
    if command -v python3 &> /dev/null; then
        PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
        print_success "Python $PYTHON_VERSION found"
        
        if [[ $(python3 -c "import sys; exit(0 if sys.version_info >= (3, 8) else 1)") ]] != "0" ]]; then
            print_error "Python 3.8+ required, found $PYTHON_VERSION"
            exit 1
        fi
        return 0
    else
        print_error "Python 3.8+ not found"
        exit 1
    fi
}

function check_pip() {
    print_step "Checking pip installation..."
    
    if command -v pip3 &> /dev/null; then
        print_success "pip3 found"
        return 0
    elif command -v pip &> /dev/null; then
        print_success "pip found"
        PIP_CMD="pip"
        return 0
    else
        print_error "pip not found. Please install pip."
        exit 1
    fi
}

function install_dependencies() {
    print_step "Installing Python dependencies..."
    
    # Install required dependencies
    $PIP_CMD install --upgrade pip
    $PIP_CMD install scikit-build-core>=0.5.0 pybind11>=2.11.0 numpy>=1.20 pytest>=7.0
    
    print_success "Core dependencies installed"
}

function build_basic_api() {
    print_step "Building Basic Python API..."
    
    if [ -d "build_basic" ]; then
        print_success "Build directory already exists"
    else
        mkdir -p build_basic
        cd build_basic
        cmake .. -DCMAKE_BUILD_TYPE=Release -DENHANCED_PYTHON_BINDINGS=OFF
        make -j4
        cd ..
    fi
    
    print_success "Basic Python API built successfully"
}

function build_enhanced_api() {
    print_step "Building Enhanced Python API..."
    
    if [ -d "build_enhanced" ]; then
        print_success "Enhanced build directory already exists"
    else
        mkdir -p build_enhanced
        cd build_enhanced
        cmake .. -DCMAKE_BUILD_TYPE=Release -DENHANCED_PYTHON_BINDINGS=ON
        make -j4
        cd ..
    fi
    
    print_success "Enhanced Python API built successfully"
}

function install_python_module() {
    print_step "Installing Python module..."
    
    # Install basic API
    pip install -e .
    print_success "Basic Python API installed"
    
    # Install enhanced API if requested
    if [ "$1" = "--enhanced" ]; then
        pip install -e .[tools]
        print_success "Enhanced Python API installed with optional dependencies"
    fi
    
    print_success "Python module installation completed"
}

function create_environment() {
    print_step "Creating development environment..."
    
    # Create virtual environment if not exists
    if [ ! -d "venv" ]; then
        python3 -m venv venv
        print_success "Virtual environment created"
        
        # Activate virtual environment
        source venv/bin/activate
        
        # Install dependencies in virtual environment
        pip install --upgrade pip
        pip install scikit-build-core>=0.5.0 pybind11>=2.11.0 numpy>=1.20 pytest>=7.0
        pip install -e .[tools]
        
        print_success "Development environment setup completed"
    else
        echo "Virtual environment already exists, skipping creation"
    fi
}

function setup_examples() {
    print_step "Setting up examples..."
    
    if [ ! -d "examples" ]; then
        mkdir -p examples
        echo "Created examples directory"
    fi
    
    # Copy example files if they exist
    if [ -f "examples/basic_simulation.py" ]; then
        print_success "Example files found"
    else
        echo "Note: Example files may need to be added manually"
    fi
    
    print_success "Examples setup completed"
}

function run_tests() {
    print_step "Running quick tests..."
    
    if [ -f "test_enhanced_api.py" ]; then
        python3 test_enhanced_api.py
        if [ $? -eq 0 ]; then
            print_success "Tests completed successfully"
        else
            print_error "Tests failed. Please check output above."
        fi
    else
        echo "Note: Comprehensive test file not found. Run tests manually after installation."
    fi
}

function show_summary() {
    print_step "Setup Summary"
    echo ""
    echo "NLM Enhanced Python API has been successfully configured!"
    echo ""
    echo "Available commands:"
    echo "  python3 -c 'import pynlm; print(pynlm.__version__)'"
    echo "  python3 -c 'import pynlm; print(pynlm.API_VERSION)'"
    echo ""
    echo "Usage examples:"
    echo "  python3 examples/basic_simulation.py"
    echo "  python3 examples/advanced_simple.py"
    echo ""
    echo "Testing:"
    echo "  python3 test_enhanced_api.py"
    echo ""
    echo "For more information, see:
    - README.md
    - ADVANCED_USAGE.md
    - python/README.md"
    echo ""
}

function main() {
    print_header
    
    # Check if enhanced mode is requested
    ENHANCED_MODE="$1"
    
    check_python
    check_pip
    
    # Determine pip command
    if command -v pip3 &> /dev/null; then
        PIP_CMD="pip3"
    else
        PIP_CMD="pip"
    fi
    
    install_dependencies
    
    if [ "$ENHANCED_MODE" = "--enhanced" ]; then
        echo -e "${YELLOW}Enhanced mode requested. Building with advanced features...${NC}"
        build_enhanced_api
        install_python_module "$ENHANCED_MODE"
    else
        echo -e "${YELLOW}Basic mode requested. Building with core features...${NC}"
        build_basic_api
        install_python_module "$ENHANCED_MODE"
    fi
    
    create_environment
    setup_examples
    
    if [ "$ENHANCED_MODE" = "--enhanced" ]; then
        echo -e "${YELLOW}Enhanced mode testing...${NC}"
        run_tests
    fi
    
    show_summary
    
    echo -e "${GREEN}Setup completed successfully!${NC}"
}

# Run main function with all arguments
main "$@"