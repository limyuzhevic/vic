#!/usr/bin/env bash

# NLM Development Setup Script
# Sets up development environment for NLM project

set -e

BUILD_DIR="build"
VENV_DIR="venv"
PYPROJECT_TOML="pyproject.toml"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}NLM Development Setup${NC}"
echo "======================="

# Check if we're in the right directory
if [ ! -f "$PYPROJECT_TOML" ]; then
    echo -e "${RED}Error: pyproject.toml not found. Run this script from the project root.${NC}"
    exit 1
fi

# Option 1: Setup virtual environment and install dependencies
if command -v python3 &> /dev/null && command -v pip3 &> /dev/null; then
    echo -e "${YELLOW}Setting up Python virtual environment...${NC}"
    
    if [ ! -d "$VENV_DIR" ]; then
        python3 -m venv "$VENV_DIR"
        echo -e "${GREEN}Virtual environment created${NC}"
    else
        echo -e "${YELLOW}Virtual environment already exists${NC}"
    fi
    
    # Activate virtual environment
    source "$VENV_DIR/bin/activate"
    
    # Upgrade pip
    pip install --upgrade pip
    
    # Install project in development mode
    echo -e "${YELLOW}Installing NLM in development mode...${NC}"
    pip install -e .
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Python package installed successfully${NC}"
    else
        echo -e "${RED}Failed to install Python package${NC}"
        exit 1
    fi
else
    echo -e "${YELLOW}Skipping virtual environment setup (python3/pip3 not available)${NC}"
fi

# Option 2: Build C++ extension if not already built
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Build directory not found, building C++ extension...${NC}"
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}Error: CMake not found. Please install CMake to build C++ extension.${NC}"
        echo -e "${YELLOW}You can install CMake via:${NC}"
        echo "  Ubuntu/Debian: sudo apt-get install cmake"
        echo "  macOS: brew install cmake"
        echo "  CentOS/RHEL: sudo yum install cmake"
        exit 1
    fi
    
    # Check for build tools
    if ! command -v make &> /dev/null && ! command -v ninja &> /dev/null; then
        echo -e "${RED}Error: Neither 'make' nor 'ninja' found.${NC}"
        echo -e "${YELLOW}Please install a build system:${NC}"
        echo "  Ubuntu/Debian: sudo apt-get install build-essential"
        echo "  macOS: brew install make"
        exit 1
    fi
    
    # Run the main build script
    echo -e "${YELLOW}Running build script...${NC}"
    ./build.sh
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}Build failed${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}Build directory already exists, skipping C++ build${NC}"
fi

# Option 3: Setup testing environment
if [ -f "$PYPROJECT_TOML" ]; then
    echo -e "${YELLOW}Checking test dependencies...${NC}"
    
    # Activate virtual environment if it exists
    if [ -f "$VENV_DIR/bin/activate" ]; then
        source "$VENV_DIR/bin/activate"
    fi
    
    # Check if pytest is available for running tests
    if command -v pytest &> /dev/null; then
        echo -e "${GREEN}Pytest is available for testing${NC}"
    else
        echo -e "${YELLOW}Pytest not available, you may want to install dev dependencies:${NC}"
        echo "  pip install -e .[dev]"
    fi
fi

echo ""
echo -e "${GREEN}Development setup complete!${NC}"
echo ""
echo "Usage:"
echo "  To run the main simulation:"
if [ -f "$BUILD_DIR/nlm" ]; then
    echo "    $BUILD_DIR/nlm"
elif [ -f "venv/bin/nlm" ]; then
    echo "    venv/bin/nlm (after activating venv)"
else
    echo "    Build the project first: ./build.sh"
fi
echo ""
echo "  To run Phase 6 integration test:"
if [ -f "$BUILD_DIR/nlm_phase6_demo" ]; then
    echo "    $BUILD_DIR/nlm_phase6_demo"
elif [ -f "venv/bin/nlm_phase6_demo" ]; then
    echo "    venv/bin/nlm_phase6_demo (after activating venv)"
else
    echo "    Build the project first: ./build.sh"
fi
echo ""
echo "  To run Python examples or tests:"
if [ -d "$VENV_DIR" ]; then
    echo "    source venv/bin/activate && python -m pytest tests/"
elif command -v pytest &> /dev/null; then
    echo "    python -m pytest tests/"
else
    echo "    Install development dependencies: pip install -e .[dev]"
fi
