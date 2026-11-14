#!/bin/bash

# NLM Setup Script
# This script provides an easy way to install, build, and test the NLM project

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== NLM (Neural Learning Machine) Setup Script ===${NC}"

# Detect package manager
if command -v apt-get &> /dev/null; then
    PKG_MANAGER="apt-get"
elif command -v yum &> /dev/null; then
    PKG_MANAGER="yum"
elif command -v dnf &> /dev/null; then
    PKG_MANAGER="dnf"
elif command -v pacman &> /dev/null; then
    PKG_MANAGER="pacman"
else
    PKG_MANAGER="unknown"
fi

# Function to install dependencies
install_dependencies() {
    echo -e "${YELLOW}Installing dependencies...${NC}"
    
    case $PKG_MANAGER in
        apt-get)
            sudo apt-get update
            sudo apt-get install -y build-essential cmake python3 python3-pip python3-dev
            ;;
        yum|dnf)
            sudo $PKG_MANAGER install -y gcc gcc-c++ cmake python3 python3-devel
            ;;
        pacman)
            sudo pacman -Sy --noconfirm
            sudo pacman -S --noconfirm base-devel cmake python
            ;;
        *)
            echo -e "${RED}Unsupported package manager. You may need to install dependencies manually.${NC}"
            ;;
    esac
}

# Function to build the project
build_project() {
    echo -e "${YELLOW}Building NLM project...${NC}"
    
    mkdir -p "$PROJECT_ROOT/build"
    cd "$PROJECT_ROOT/build"
    
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    
    cd "$PROJECT_ROOT"
}

# Function to install Python bindings
install_python_bindings() {
    echo -e "${YELLOW}Installing Python bindings...${NC}"
    
    cd "$PROJECT_ROOT"
    
    # Check if virtual environment is needed
    if python3 -c "import sys; print('System Python' if sys.version_info.major >= 3 else 'Virtual Environment')" 2>/dev(sig); then
        echo -e "${YELLOW}Would recommend creating a virtual environment for Python packages...${NC}"
    fi
    
    # Install dependencies
    pip3 install pybind11 scikit-build-core pytest numpy
    
    # Build and install
    pip3 install .
}

# Function to run tests
test_project() {
    echo -e "${YELLOW}Running tests...${NC}"
    
    if [ -f "$PROJECT_ROOT/build/nlm_test" ]; then
        cd "$PROJECT_ROOT/build"
        ./nlm_test
        echo -e "${GREEN}✓ C++ tests passed!${NC}"
    else
        echo -e "${RED}Error: nlm_test executable not found. Build the project first.${NC}"
        exit 1
    fi
}

# Function to create virtual environment
create_venv() {
    echo -e "${YELLOW}Creating virtual environment...${NC}"
    
    if [ -d "$PROJECT_ROOT/venv" ]; then
        echo -e "${YELLOW}Virtual environment already exists.${NC}"
    else
        python3 -m venv "$PROJECT_ROOT/venv"
    fi
    
    echo -e "${GREEN}To activate virtual environment:${NC}"
    echo -e "  source $PROJECT_ROOT/venv/bin/activate"
    echo -e "  Then install dependencies and build with the same script."
}

# Function to show help
function_usage() {
    echo -e "${GREEN}NLM Setup Script${NC}"
    echo -e ""
    echo -e "Usage: $0 <command>"
    echo -e ""
    echo -e "Commands:"
    echo -e "  install-deps    Install system dependencies"
    echo -e "  build           Build the project (requires dependencies)"
    echo -e "  install-python  Install Python bindings"
    echo -e "  test            Run tests"
    echo -e "  venv            Create virtual environment"
    echo -e "  all             Run all setup steps"
    echo -e "  help            Show this help message"
    echo -e ""
    echo -e "Examples:"
    echo -e "  $0 install-deps"
    echo -e "  $0 build"
    echo -e "  $0 install-python"
    echo -e "  $0 test"
}

# Main script logic
case "$1" in
    install-deps)
        install_dependencies
        ;;
    build)
        build_project
        ;;
    install-python)
        install_python_bindings
        ;;
    test)
        test_project
        ;;
    venv)
        create_venv
        ;;
    all)
        install_dependencies
        build_project
        install_python_bindings
        test_project
        ;;
    *|help)
        function_usage
        ;;
    esac