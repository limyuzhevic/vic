#!/bin/bash
# setup_nlm.sh - Complete NLM project setup script

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

print_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

# Function to print help
print_help() {
    cat << EOF
NLM Project Setup Script

USAGE:
  setup_nlm.sh [OPTIONS]

OPTIONS:
  --build                 Build the NLM project
  --python-bindings       Build Python bindings
  --tests                 Run tests
  --all                   Do everything (build, Python bindings, tests)
  --help, -h              Show this help message

EXAMPLES:
  setup_nlm.sh --build                    # Build NLM from source
  setup_nlm.sh --python-bindings          # Build Python bindings
  setup_nlm.sh --tests                     # Run tests
  setup_nlm.sh --all                       # Complete setup

This script will:
  1. Create necessary directories
  2. Set up configuration files
  3. Configure CMake
  4. Build the NLM project
  5. Build Python bindings
  6. Run tests
  7. Install Python module (optional)

EOF
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to create directories
setup_directories() {
    print_info "Creating project directories..."
    
    mkdir -p build
    mkdir -p configs
    mkdir -p docs
    mkdir -p python/dist
    mkdir -p tests/results
    mkdir -p logs
    mkdir -p checkpoints
    mkdir -p visualizations
    
    print_success "Directories created successfully"
}

# Function to setup configuration files
setup_config() {
    print_info "Setting up configuration files..."
    
    # Generate default config if it doesn't exist
    if [ ! -f "configs/default.cfg" ]; then
        print_info "Creating default configuration file..."
        ./nlm_config generate configs/default.cfg
    else
        print_info "Using existing configuration file..."
    fi
    
    # Generate example config if it doesn't exist
    if [ ! -f "configs/example.cfg" ]; then
        print_info "Creating example configuration file..."
        ./nlm_config generate configs/example.cfg
    else
        print_info "Using existing example configuration file..."
    fi
    
    print_success "Configuration files setup complete"
}

# Function to check dependencies
check_dependencies() {
    print_info "Checking dependencies..."
    
    local missing=0
    
    # Check for CMake
    if command_exists cmake; then
        local cmake_version=$(cmake --version | head -1 | awk '{print $3}')
        print_info "CMake found: $cmake_version"
    else
        print_error "CMake not found"
        missing=$((missing + 1))
    fi
    
    # Check for C++ compiler
    if command_exists g++; then
        local gpp_version=$($g++ --version | head -1)
        print_info "g++ found: $gpp_version"
    elif command_exists clang++; then
        local clang_version=$(clang++ --version | head -1)
        print_info "clang++ found: $clang_version"
    else
        print_error "C++ compiler not found"
        missing=$((missing + 1))
    fi
    
    # Check for Python
    if command_exists python3; then
        local python_version=$(python3 --version | head -1)
        print_info "Python found: $python_version"
    else
        print_error "Python3 not found"
        missing=$((missing + 1))
    fi
    
    # Check for pip
    if command_exists pip3; then
        print_info "pip3 found"
    else
        print_warning "pip3 not found (will use python3 -m pip)"
    fi
    
    # Check for git
    if command_exists git; then
        local git_version=$(git --version | head -1)
        print_info "git found: $git_version"
    else
        print_warning "git not found (will try to clone if needed)"
    fi
    
    if [ $missing -gt 0 ]; then
        print_error "Missing $missing dependencies. Please install them before proceeding."
        return 1
    else
        print_success "All dependencies found"
        return 0
    fi
}

# Function to build the project
build_project() {
    print_info "Building NLM project..."
    
    cd build || {
        print_error "Failed to enter build directory"
        return 1
    }
    
    # Configure CMake
    if [ ! -f "CMakeCache.txt" ]; then
        print_info "Configuring CMake..."
        if cmake .. -DCMAKE_BUILD_TYPE=Release; then
            print_success "CMake configuration complete"
        else
            print_error "CMake configuration failed"
            cd .. || return 1
            return 1
        fi
    else
        print_info "CMake already configured..."
    fi
    
    # Build the project
    print_info "Building NLM..."
    if make -j4; then
        print_success "NLM build complete"
    else
        print_error "NLM build failed"
        cd .. || return 1
        return 1
    fi
    
    # Build Python bindings if available
    if [ -f "../python/CMakeLists.txt" ]; then
        print_info "Building Python bindings..."
        cd python
        if python3 -m pip install pybind11; then
            if cmake .. -DPYTHON_EXECUTABLE=$(which python3); then
                if make -j4; then
                    print_success "Python bindings build complete"
                else
                    print_error "Python bindings build failed"
                fi
            else
                print_error "Python bindings CMake configuration failed"
            fi
        else
            print_warning "Failed to install pybind11. Python bindings may not build."
        fi
        cd ..
    fi
    
    cd .. || return 1
    print_success "Project build complete"
}

# Function to run tests
run_tests() {
    print_info "Running tests..."
    
    cd build || {
        print_error "Failed to enter build directory"
        return 1
    }
    
    # Run C++ tests
    if [ -f "CTestTestfile.cmake" ]; then
        print_info "Running C++ tests..."
        if ctest --output-on-failure; then
            print_success "C++ tests complete"
        else
            print_error "C++ tests failed"
        fi
    else
        print_warning "No C++ tests found"
    fi
    
    # Run Python tests if available
    if [ -d "../python/tests" ] && [ -f "../python/setup.py" ]; then
        print_info "Running Python tests..."
        cd ../python
        if python3 -m pytest tests/ -v; then
            print_success "Python tests complete"
        else
            print_error "Python tests failed"
        fi
        cd ..
    else
        print_warning "No Python tests found"
    fi
    
    cd .. || return 1
    print_success "Tests complete"
}

# Function to setup Python module
setup_python_module() {
    print_info "Setting up Python module..."
    
    if [ ! -d "python/dist" ] || [ ! -f "python/dist/pynlm*.whl" ]; then
        print_warning "Python module not found. Please build it first."
        print_info "Run: setup_nlm.sh --python-bindings"
        return 1
    fi
    
    # Install Python module
    print_info "Installing Python module..."
    cd python/dist
    local wheel_file=$(ls pynlm*.whl | head -1)
    if [ -n "$wheel_file" ]; then
        if python3 -m pip install "$wheel_file"; then
            print_success "Python module installed successfully"
        else
            print_error "Python module installation failed"
            return 1
        fi
    else
        print_error "No Python wheel file found"
        return 1
    fi
    cd ../..
    
    print_success "Python module setup complete"
}

# Function to create examples
setup_examples() {
    print_info "Creating examples and documentation..."
    
    # Create examples directory
    mkdir -p examples
    
    # Create basic example
    cat > examples/basic_brain.py << 'EOF'
#!/usr/bin/env python3
"""
Basic NLM brain example
This example demonstrates how to create and use a basic NLM brain
"""

import pynlm
import time

def main():
    print("=== Basic NLM Brain Example ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Create a brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run a simple simulation
    steps = 1000
    print(f"Running {steps} simulation steps...")
    
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} spikes, "
                  f"{brain.getFiringNeuronCount()} firing neurons")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Save brain state
    brain.save("basic_brain_checkpoint")
    print("Brain state saved to 'basic_brain_checkpoint'")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
EOF
    
    # Create agent example
    cat > examples/agent_world.py << 'EOF'
#!/usr/bin/env python3
"""
NLM Agent in World example
This example demonstrates how to create an agent that interacts with a world
"""

import pynlm
import time

def main():
    print("=== NLM Agent in World Example ===")
    
    # Create a brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Place agent in world
    world.setAgentStart(10.0, 10.0)
    
    # Create an agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Agent created at ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"Agent has {agent.getSensoryInputSize()} sensory inputs and "
          f"{agent.getMotorOutputSize()} motor outputs")
    
    # Run agent in world
    steps = 100
    print(f"Running agent for {steps} steps...")
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain what the agent sees
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show agent status every 10 steps
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                  f"Energy: {body.energy:.1f}, "
                  f"Action: {action}")
    
    # Show final statistics
    print(f"\nFinal Agent Statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Final energy: {world.getAgentBody().energy:.1f}")
    
    # Save brain state
    brain.save("agent_world_brain_checkpoint")
    print("Brain state saved to 'agent_world_brain_checkpoint'")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
EOF
    
    # Create CLI example
    cat > examples/nlm_cli_example.sh << 'EOF'
#!/bin/bash
# Example script showing how to use the NLM CLI

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

echo "=== NLM CLI Usage Example ==="

# Initialize brain
print_info "Initializing NLM brain..."
./nlm init --neurons 500 --regions 2 --seed 123

# Run a simulation
print_info "Running simulation..."
./nlm run --steps 500 --checkpoint simulation_checkpoints

# Get brain status
print_info "Getting brain status..."
./nlm status --detailed

# Save brain state
print_info "Saving brain state..."
./nlm save my_simulation_checkpoint

# Load brain state (example - uncomment to use)
# print_info "Loading brain state..."
# ./nlm load my_simulation_checkpoint

# Show performance statistics
print_info "Getting performance statistics..."
./nlm stats --spikes --firing-rate

print_success "Example completed successfully!"
EOF
    
    chmod +x examples/nlm_cli_example.sh
    
    print_success "Examples created in examples/ directory"
}

# Main execution
if [ $# -eq 0 ]; then
    print_help
    exit 0
fi

# Parse arguments
BUILD=0
PYTHON_BINDINGS=0
RUN_TESTS=0
DO_ALL=0

while [ $# -gt 0 ]; do
    case "$1" in
        "--build")
            BUILD=1
            shift
            ;;
        "--python-bindings")
            PYTHON_BINDINGS=1
            shift
            ;;
        "--tests")
            RUN_TESTS=1
            shift
            ;;
        "--all")
            DO_ALL=1
            shift
            ;;
        "--help"|-h)
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

# Execute setup steps
if [ $DO_ALL -eq 1 ] || [ $BUILD -eq 1 ]; then
    check_dependencies || {
        print_error "Dependency check failed. Please install missing dependencies."
        exit 1
    }
    setup_directories
    setup_config
    build_project
fi

if [ $PYTHON_BINDINGS -eq 1 ] || [ $DO_ALL -eq 1 ]; then
    setup_python_module
fi

if [ $RUN_TESTS -eq 1 ] || [ $DO_ALL -eq 1 ]; then
    run_tests
fi

setup_examples

print_success "NLM setup complete!"
print_info "You can now use NLM with the following commands:"
print_info "  ./nlm --help              # Show help"
print_info "  ./nlm run --steps 1000    # Run simulation"
print_info "  ./nlm save my_checkpoint  # Save brain state"
print_info "  ./nlm load my_checkpoint  # Load brain state"
print_info "  ./nlm status             # Show brain status"
print_info "  ./nlm info               # Show system information"
print_info ""
print_info "Python bindings available via:")
print_info "  python3 examples/basic_brain.py"
print_info "  python3 examples/agent_world.py"
print_info ""
print_info "For more information, see the README.md file."
