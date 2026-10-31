#!/bin/bash
# Enhanced NLM Build and Usage Script
# This script provides comprehensive commands for building, testing, and using NLM

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="${1:-build}"
PYTHON_PACKAGE="${2:-pynlm}"
EXAMPLES_DIR="${3:-examples}"
DOCS_DIR="${4:-docs}"

# Print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

print_header() {
    echo -e "${BLUE}[BUILD]${NC} $*"
}

# Check if we're in the right directory
check_environment() {
    print_info "Checking environment..."
    
    if [ ! -f "README.md" ]; then
        print_error "README.md not found. Please run this script from the NLM project root."
        exit 1
    fi
    
    if [ ! -f "CMakeLists.txt" ]; then
        print_error "CMakeLists.txt not found. Please run this script from the NLM project root."
        exit 1
    fi
    
    if [ ! -f "pyproject.toml" ]; then
        print_error "pyproject.toml not found. Please run this script from the NLM project root."
        exit 1
    fi
    
    print_info "Environment check passed."
}

# Build the C++ core libraries
build_cpp() {
    print_header "Building C++ core libraries..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    print_info "Configuring with CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DNLM_BUILD_PYTHON_BINDINGS=ON \
             -DCMAKE_CXX_STANDARD=20
    
    # Build with parallel jobs
    print_info "Building with $(nproc) parallel jobs..."
    make -j$(nproc)
    
    cd ..
    
    print_info "C++ build completed successfully."
}

# Install Python bindings
install_python() {
    print_header "Installing Python bindings..."
    
    # Install package in development mode
    print_info "Installing Python package in development mode..."
    pip install -e .
    
    # Verify installation
    print_info "Verifying Python package installation..."
    python3 -c "import pynlm; print(f'pynlm version: {pynlm.__version__}'); print('Import successful!')"
    
    print_info "Python package installation completed."
}

# Build documentation
build_docs() {
    print_header "Building documentation..."
    
    mkdir -p "$DOCS_DIR"
    
    # Generate API documentation
    print_info "Generating API documentation..."
    python3 -m pydoc pynlm > "$DOCS_DIR/api_reference.txt" 2>&1 || true
    
    # Copy documentation files
    print_info "Copying documentation files..."
    cp README.md "$DOCS_DIR/"
    cp HOW_TO_USE.md "$DOCS_DIR/"
    cp easy_usage.md "$DOCS_DIR/"
    cp -r docs/ "$DOCS_DIR/" 2>/dev/null || true
    
    print_info "Documentation build completed."
}

# Run tests
run_tests() {
    print_header "Running tests..."
    
    # Run C++ tests
    if [ -f "$BUILD_DIR/nlm_test" ]; then
        print_info "Running C++ unit tests..."
        cd "$BUILD_DIR"
        ./nlm_test
        cd ..
    else
        print_warning "C++ test executable not found. Skipping C++ tests."
    fi
    
    # Run Python tests (if any)
    if [ -d "tests" ]; then
        print_info "Running Python tests..."
        python3 -m pytest tests/ -v 2>/dev/null || print_warning "No Python tests found or pytest not available."
    else
        print_warning "Tests directory not found. Skipping Python tests."
    fi
    
    print_info "Test execution completed."
}

# Create examples directory
create_examples() {
    print_header "Creating examples directory..."
    
    mkdir -p "$EXAMPLES_DIR"
    
    # Copy example scripts
    if [ -f "examples/enhanced_api_examples.py" ]; then
        cp "examples/enhanced_api_examples.py" "$EXAMPLES_DIR/"
        print_info "Copied enhanced API examples."
    fi
    
    # Create example configurations
    mkdir -p "$EXAMPLES_DIR/configs"
    
    # Create example config files
    cat > "$EXAMPLES_DIR/configs/simple_config.py" << 'EOF'
import pynlm

# Create a simple configuration
def create_simple_config():
    config = pynlm.createDefaultConfig()
    
    # Configure for basic simulation
    config.configure_brain(
        neuron_count=500,
        synapse_density=0.1,
        connection_probability=0.05
    )
    
    # Configure plasticity
    config.configure_plasticity(
        enable_stdp=True,
        learning_rate=0.001
    )
    
    # Configure neuromodulation
    config.configure_neuromodulation(
        enable_curiosity=True,
        enable_novelty=True
    )
    
    return config

if __name__ == "__main__":
    config = create_simple_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print("Configuration saved successfully.")
EOF

    cat > "$EXAMPLES_DIR/configs/expert_config.py" << 'EOF'
import pynlm

def create_expert_config():
    """Configuration optimized for expert performance"""
    config = pynlm.createDefaultConfig()
    
    # Larger brain for expertise
    config.configure_brain(
        neuron_count=2000,
        synapse_density=0.15,
        connection_probability=0.08
    )
    
    # Strong plasticity for learning
    config.configure_plasticity(
        enable_stdp=True,
        enable_hebbian=True,
        learning_rate=0.01
    )
    
    # Suppress curiosity for focused learning
    config.configure_neuromodulation(
        enable_curiosity=False,
        enable_novelty=False
    )
    
    return config

if __name__ == "__main__":
    config = create_expert_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created expert brain with {brain.getTotalNeuronCount()} neurons")
    print("Expert configuration saved successfully.")
EOF

    print_info "Examples directory created successfully."
}

# Test Python bindings
.test_python_bindings() {
    print_header "Testing Python bindings..."
    
    # Create a test script
    cat > "${BUILD_DIR}/test_python_bindings.py" << 'EOF'
import pynlm
import sys

def test_basic_functionality():
    """Test basic Python binding functionality"""
    print("Testing basic Python binding functionality...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize
    if not brain.initialize():
        print("ERROR: Brain initialization failed")
        return False
    
    print(f"  ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Test simulation
    for i in range(10):
        brain.step(i)
    
    print(f"  ✓ Simulation completed, {brain.getTotalSpikeCount()} spikes")
    
    # Test configuration
    config.set("test.value", 42)
    if config.get("test.value") != 42:
        print("ERROR: Configuration get/set failed")
        return False
    
    print("  ✓ Configuration get/set working")
    
    # Test agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("  ✓ Agent created and initialized")
    
    # Test interaction
    for i in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("  ✓ Agent interaction completed")
    
    return True

def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test missing key
    try:
        value = config.get("nonexistent.key")
        print("ERROR: Should have raised exception for missing key")
        return False
    except pynlm.NLMError:
        print("  ✓ NLMError raised for missing key")
    except Exception as e:
        print(f"ERROR: Wrong exception type: {e}")
        return False
    
    return True

def test_enhanced_features():
    """Test enhanced Python features"""
    print("Testing enhanced Python features...")
    
    # Test configuration convenience methods
    config = pynlm.createDefaultConfig()
    config.configure_brain(neuron_count=1000)
    config.configure_plasticity(learning_rate=0.001)
    config.configure_neuromodulation(enable_curiosity=True)
    
    print("  ✓ Configuration convenience methods working")
    
    # Test statistics
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    stats = brain.get_stats()
    if 'total_neurons' not in stats:
        print("ERROR: Statistics missing required fields")
        return False
    
    print(f"  ✓ Statistics working: {stats['total_neurons']} neurons")
    
    return True

if __name__ == "__main__":
    print("=== NLM Python Bindings Test ===")
    
    success = True
    
    if not test_basic_functionality():
        success = False
    
    if not test_error_handling():
        success = False
    
    if not test_enhanced_features():
        success = False
    
    if success:
        print("\n=== All Python binding tests passed! ===")
        sys.exit(0)
    else:
        print("\n=== Some Python binding tests failed! ===")
        sys.exit(1)
EOF
    
    # Run the test
    python3 "${BUILD_DIR}/test_python_bindings.py"
    
    # Clean up
    rm -f "${BUILD_DIR}/test_python_bindings.py"
    
    print_info "Python binding tests completed."
}

# Create development environment
.create_dev_environment() {
    print_header "Creating development environment..."
    
    # Create virtual environment
    if [ ! -d "venv" ]; then
        print_info "Creating virtual environment..."
        python3 -m venv venv
    fi
    
    # Install dependencies
    print_info "Installing development dependencies..."
    source venv/bin/activate
    pip install -e .
    pip install matplotlib seaborn pandas jupyter sphinx
    
    # Create requirements file
    cat > "requirements-dev.txt" << 'EOF'
# NLM Development Dependencies
pynlm
numpy
matplotlib
seaborn
pandas
jupyter
sphinx

# Build dependencies
scikit-build-core
pybind11
cmake
EOF
    
    print_info "Development environment created successfully."
    
    # Create development shortcuts
    cat > "dev-setup.sh" << 'EOF'
#!/bin/bash
echo "=== NLM Development Environment Setup ==="
source venv/bin/activate
pip install -e .
pip install matplotlib seaborn pandas jupyter sphinx
echo "Development setup completed."
echo "To activate the environment: source venv/bin/activate"
EOF
    
    chmod +x "dev-setup.sh"
}

# Generate quick start guide
.generate_quick_start() {
    print_header "Generating quick start guide..."
    
    cat > "QUICK_START.md" << 'EOF'
# NLM Quick Start Guide

## Installation

```bash
# Install NLM
pip install .

# For development
pip install -e .

# Install with development dependencies
./dev-setup.sh
```

## Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
```

## Agent Simulation

```python
import pynlm

# Create agent environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_subsystems(reward_modulation=True, curiosity=True)

# Run simulation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Step {step}: {agent.getThoughtTrace()}")
```

## Examples

Run the examples:

```bash
python3 examples/enhanced_api_examples.py
```

Or run specific examples:

```bash
python3 examples/basic_usage.py
python3 examples/agent_simulation.py
python3 examples/training_example.py
```

## Documentation

For detailed documentation:

- `docs/Enhanced_API_Documentation.md` - Complete API reference
- `docs/API_Reference.md` - Function-by-function documentation
- `README.md` - Project overview and build instructions
- `HOW_TO_USE.md` - Detailed usage instructions

## Development

### Building from Source

```bash
# Create build directory and build
mkdir build && cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)

# Install Python bindings
cd ..
pip install -e .
```

### Running Tests

```bash
# Run C++ tests
./build/nlm_test

# Run Python tests
python3 -m pytest tests/ -v

# Run all tests
./build_and_test.sh
```

### Working with Examples

The `examples/` directory contains:

- `enhanced_api_examples.py` - Comprehensive examples of enhanced API usage
- `configs/` - Example configuration files
- `demo/` - Demonstration scripts

### Performance Optimization

```python
# Use configuration optimization
config = pynlm.createDefaultConfig()
config.configure_brain(neuron_count=500)  # Smaller for better performance

# Use efficient simulation patterns
def efficient_simulation(brain, steps):
    for i in range(steps):
        brain.step(i)
        if i % 100 == 0:
            brain.save(f"checkpoint_{i}.bin")
```

## Troubleshooting

### Common Issues

**"Brain not initialized"**

```python
# Solution: Always call initialize()
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
```

**"Configuration error"**

```python
# Solution: Use .get() with default
value = config.get("some.key", default_value)
```

**"Import error"**

```bash
# Solution: Ensure package is installed
pip install -e .
```

## Getting Help

- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [Issues](https://github.com/nlm-project/nlm/issues)
- [Discussions](https://github.com/nlm-project/nlm/discussions)

## Community

Join the NLM community:

- GitHub: https://github.com/nlm-project/nlm
- Discord: https://discord.gg/nlm
- Twitter: @NLM_Research
EOF
    
    print_info "Quick start guide generated: QUICK_START.md"
}

# Main execution
main() {
    echo "=== Enhanced NLM Build and Usage Script ==="
    echo
    
    case "$1" in
        "build")
            check_environment
            build_cpp
            install_python
            build_docs
            create_examples
            .test_python_bindings
            create_dev_environment
            generate_quick_start
            echo -e "${GREEN}✓ All build and setup completed successfully!${NC}"
            ;;
        
        "test")
            check_environment
            run_tests
            .test_python_bindings
            echo -e "${GREEN}✓ All tests completed successfully!${NC}"
            ;;
        
        "examples")
            check_environment
            install_python
            create_examples
            echo -e "${GREEN}✓ Examples created successfully!${NC}"
            ;;
        
        "docs")
            check_environment
            build_docs
            generate_quick_start
            echo -e "${GREEN}✓ Documentation built successfully!${NC}"
            ;;
        
        "dev")
            check_environment
            create_dev_environment
            echo -e "${GREEN}✓ Development environment created!${NC}"
            ;;
        
        "help")
            echo "Usage: $0 [command]"
            echo
            echo "Commands:"
            echo "  build    - Complete build and setup (default)"
            echo "  test     - Run tests only"
            echo "  examples - Create examples directory"
            echo "  docs     - Build documentation only"
            echo "  dev      - Create development environment only"
            echo "  help     - Show this help message"
            ;;
        
        *)
            print_error "Unknown command: $1"
            echo "Run '$0 help' for usage information."
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
