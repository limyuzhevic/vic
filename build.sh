#!/bin/bash
# NLM Build and Verification Script
# Provides comprehensive build instructions and verification for the NLM project

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== NLM (熙然) Build and Verification Script ===${NC}"
echo

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -f "pyproject.toml" ]; then
    echo -e "${RED}Error: Please run this script from the NLM project root directory.${NC}"
    echo "Expected files: CMakeLists.txt, pyproject.toml"
    exit 1
fi

# Detect platform and Python version
OS="$(uname -s)"
PYTHON_VERSION="$(python3 --version 2>&1)"

if [[ "$OS" == "Linux" ]]; then
    PLATFORM="Linux"
elif [[ "$OS" == "Darwin" ]]; then
    PLATFORM="macOS"
elif [[ "$OS" =~ MINGW || "$OS" =~ MSYS || "$OS" =~ CYGWIN ]]; then
    PLATFORM="Windows"
else
    PLATFORM="Unknown"
fi

echo -e "${GREEN}Platform:${NC} $PLATFORM"
echo -e "${GREEN}Python:${NC} $PYTHON_VERSION"
echo

# Check for required tools
check_command() {
    if ! command -v $1 &> /dev/null; then
        echo -e "${RED}Error: $1 is required but not installed.${NC}"
        return 1
    else
        echo -e "${GREEN}✓ $1 found:${NC} $(which $1)"
    fi
}

check_python_package() {
    if python3 -c "import $1" 2>/dev/null; then
        echo -e "${GREEN}✓ Python package $1:${NC} found"
    else
        echo -e "${RED}Error: Python package $1 is required but not installed.${NC}"
        return 1
    fi
}

echo -e "${YELLOW}Checking required tools...${NC}"
check_command "cmake"
check_command "make"
check_command "python3"
check_python_package "pybind11"
check_python_package "numpy"
check_python_package "pytest"
echo

# Build directory setup
BUILD_DIR="build"
PYTHON_BUILD_DIR="python_build"

echo -e "${YELLOW}Setting up build directories...${NC}"
mkdir -p "$BUILD_DIR"
mkdir -p "$PYTHON_BUILD_DIR"
echo

# Configure CMake
echo -e "${YELLOW}Configuring CMake...${NC}"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20 \
    -DPYTHON_EXECUTABLE=$(which python3)

# Build the C++ library
echo -e "${YELLOW}Building NLM C++ library...${NC}"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo "4")

if [ ! -f "libnlm_core.a" ]; then
    echo -e "${RED}Error: Build failed - libnlm_core.a not found${NC}"
    exit 1
fi

echo -e "${GREEN}✓ C++ library built successfully${NC}"
echo

# Build Python bindings
echo -e "${YELLOW}Building Python bindings...${NC}"
cd "$PYTHON_BUILD_DIR"
pip install -e . --no-deps

if [ ! -f "$(python3 -c "import site; print(site.getsitepackages()[0])")/pynlm*.so" ]; then
    echo -e "${RED}Error: Python bindings build failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Python bindings built successfully${NC}"
echo

# Run basic verification tests
echo -e "${YELLOW}Running verification tests...${NC}"

# Create a simple test script
test_script="test_verification.py"
cat > "$test_script" << 'EOF'
import sys
import pynlm

print("Testing NLM Python API...")

# Test 1: Default config creation
print("Test 1: Creating default config...")
config = pynlm.createDefaultConfig()
print(f"  Config created: {type(config)}")

# Test 2: Brain creation
print("Test 2: Creating brain...")
brain = pynlm.createBrain(config)
print(f"  Brain created: {brain.getTotalNeuronCount()} neurons")

# Test 3: Initialize brain
print("Test 3: Initializing brain...")
brain.initialize()
print("  Brain initialized successfully")

# Test 4: Step brain
print("Test 4: Running simulation steps...")
for i in range(10):
    brain.step(i)
print("  Steps completed")

# Test 5: Get statistics
print("Test 5: Getting brain statistics...")
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "avg_firing_rate": brain.getAverageFiringRate()
}
print(f"  Statistics: {stats}")

# Test 6: Agent brain
print("Test 6: Creating agent brain...")
agent = pynlm.createAgentBrain(brain)
print("  Agent brain created")

# Test 7: Simple world
print("Test 7: Creating simple world...")
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
print("  World created and configured")

# Test 8: Agent initialization
print("Test 8: Initializing agent...")
agent.initialize(world)
print("  Agent initialized")

print("\n✓ All verification tests passed!")
EOF

# Run the test script
python3 "$test_script"
result=$?

if [ $result -eq 0 ]; then
    echo -e "${GREEN}✓ All verification tests passed successfully!${NC}"
else
    echo -e "${RED}Error: Verification tests failed${NC}"
    rm -f "$test_script"
    exit 1
fi

rm -f "$test_script"

# Generate build summary
echo

echo -e "${YELLOW}=== Build Summary ===${NC}"
echo -e "Build directory: $BUILD_DIR"
echo -e "Python build directory: $PYTHON_BUILD_DIR"
echo -e "Platform: $PLATFORM"
echo -e "Python version: $PYTHON_VERSION"

echo

echo -e "${GREEN}=== Next Steps ===${NC}"
echo "1. To run full test suite:"
echo "   cd $BUILD_DIR"
echo "   ctest --output-on-failure"
echo

echo "2. To use NLM in Python:"
echo "   import pynlm"
echo "   # See easy_usage.md or HOW_TO_USE.md for examples"
echo

echo "3. For Phase 6 integration demo:"
echo "   ./nlm_phase6_demo"
echo

echo -e "${GREEN}=== Build completed successfully! ===${NC}"