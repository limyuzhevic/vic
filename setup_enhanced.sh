#!/usr/bin/env bash
"""
Setup script for NLM Enhanced Features

This script helps users quickly get started with the enhanced NLM features:
1. Build the project with improved Python bindings
2. Run basic tests to verify enhancements
3. Show examples of new features
"""

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_usage() {
    cat << EOF
NLM Enhanced Features Setup Script

Usage: $0 [OPTIONS]

Options:
  --build-only      Only build the project, don't run tests
  --test-only       Only run tests, don't build
  --help            Show this help message

Examples:
  $0                    # Full setup: build and test
  $0 --build-only       # Only build
  $0 --test-only        # Only run tests
EOF
}

print_step() {
    echo -e "${GREEN}[STEP]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Parse arguments
BUILD_ONLY=false
TEST_ONLY=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --build-only)
            BUILD_ONLY=true
            shift
            ;;
        --test-only)
            TEST_ONLY=true
            shift
            ;;
        --help)
            print_usage
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            print_usage
            exit 1
            ;;
    esac
done

# Project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_ROOT"

# Check if Python is available
print_step "Checking Python installation..."
if command -v python3 &> /dev/null; then
    PYTHON_AVAILABLE=true
    PYTHON_VERSION=$(python3 --version 2>&1)
    print_success "Python found: $PYTHON_VERSION"
else
    print_error "Python3 is required but not found. Please install Python 3.8+"
    exit 1
fi

# Check if pip is available
if command -v pip3 &> /dev/null; then
    PIP_AVAILABLE=true
    print_success "pip3 found"
else
    print_warning "pip3 not found. Trying pip..."
    if command -v pip &> /dev/null; then
        PIP_AVAILABLE=true
        print_success "pip found"
    else
        print_error "pip is required but not found. Please install pip"
        exit 1
    fi
fi

# Check if CMake is available
print_step "Checking CMake installation..."
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1)
    print_success "CMake found: $CMAKE_VERSION"
else
    print_error "CMake is required but not found. Please install CMake 3.16+"
    exit 1
fi

# Check if make is available
print_step "Checking make installation..."
if command -v make &> /dev/null; then
    MAKE_VERSION=$(make --version | head -n1)
    print_success "Make found: $MAKE_VERSION"
else
    print_error "make is required but not found. Please install build tools"
    exit 1
fi

# Install Python dependencies
print_step "Installing Python dependencies..."
if [ "$PYTHON_AVAILABLE" = true ]; then
    # Check if we can install dependencies
    pip install --quiet scikit-build-core pybind11 pytest numpy 2>&1 | head -20
    print_success "Python dependencies installed"
else
    print_error "Cannot install Python dependencies due to Python not being found"
    exit 1
fi

# Build the project
if [ "$TEST_ONLY" = false ]; then
    print_step "Building NLM project..."
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -20
    print_success "CMake configuration complete"
    
    # Build the project
    make -j$(nproc) 2>&1 | tail -30
    print_success "NLM project built successfully"
    
    # Go back to project root
    cd "$PROJECT_ROOT"
else
    print_step "Skipping build (--test-only mode)..."
fi

# Run tests
if [ "$BUILD_ONLY" = false ]; then
    print_step "Running NLM enhanced features tests..."
    
    # Check if Python test script exists
    if [ -f "test_enhanced_features.py" ]; then
        # Run the test script
        python3 test_enhanced_features.py 2>&1
        
        if [ $? -eq 0 ]; then
            print_success "All tests passed!"
        else
            print_error "Some tests failed. See output above for details."
            exit 1
        fi
    else
        print_warning "Test script not found. Skipping tests."
    fi
else
    print_step "Skipping tests (--build-only mode)..."
fi

# Show examples
print_step "Showing enhanced features examples..."

cat << EOF

╔══════════════════════════════════════════════════════════════╗
║               NLM Enhanced Features Examples               ║
╚═════════════════════════════════════════════════════════════╝

1. Basic Python Usage (enhanced API):
   python3 -c "
   import pynlm
   config = pynlm.createDefaultConfig()
   config.set('brain.neuron_count', 1000)
   brain = pynlm.createBrain(config)
   brain.initialize()
   brain.step(0)
   print(f'Created brain with {brain.getTotalNeuronCount()} neurons')
   "

2. Configuration Management (new features):
   python3 -c "
   import pynlm
   config = pynlm.createDefaultConfig()
   # Type-safe getters
   neurons = config.getInt('brain.neuron_count', 1000)
   learning_rate = config.getFloat('plasticity.learning_rate', 0.01)
   enabled = config.getBool('plasticity.stdp.enable', True)
   print(f'Neurons: {neurons}, Learning Rate: {learning_rate}, STDP: {enabled}')
   "

3. Complete Agent Simulation (new convenience method):
   python3 << 'EOF'
   import pynlm
   
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   
   world = pynlm.createSimpleWorld()
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   world.reset()
   
   agent = pynlm.createAgentWithDefaults(brain, world)
   agent.initialize(world)
   
   # Run simulation
   for step in range(50):
       world.update(0.1)
       agent.processSensoryInput(world.getSensoryPercept())
       brain.step(step)
       world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
   
   print('Agent simulation complete!')
   EOF

4. Command-Line Interface (new options):
   ./nlm --experiment-mode=plasticity --save --output=checkpoints/bin

5. Enhanced Help:
   ./nlm --help

╚═════════════════════════════════════════════════════════════╝

🎉 NLM Enhanced Features Setup Complete!

For more information, see:
- enhanced_docs.md (comprehensive documentation)
- easy_usage.md (beginner guide)
- HOW_TO_USE.md (detailed instructions)
EOF

print_success "Setup complete! Your NLM enhanced features are ready to use."
echo ""
echo "Next steps:"
echo "1. Run 'python3 test_enhanced_features.py' to verify the enhancements"
echo "2. Start using the new Python API features"
echo "3. Explore the enhanced documentation in enhanced_docs.md"
echo "4. Try the new command-line options"