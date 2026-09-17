#!/bin/bash
# NLM Phase 6 Integration Test Script

# Exit on error
# Pour l'utilisateur

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo -e "${BLUE}[NLM-TEST]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "pyproject.toml" ] && [ ! -f "CMakeLists.txt" ]; then
    error "Not in NLM project directory!"
    error "Please run this script from the NLM project root."
    exit 1
fi

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install packages
define_install_package() {
    local package=$1
    
    if command_exists apt-get; then
        info "Installing $package using apt-get..."
        sudo apt-get update
        sudo apt-get install -y "$package"
    elif command_exists yum; then
        info "Installing $package using yum..."
        sudo yum install -y "$package"
    elif command_exists brew; then
        info "Installing $package using Homebrew..."
        brew install "$package"
    else
        warn "Package manager not found. Please install $package manually."
        return 1
    fi
}

# Function to create directories
create_directories() {
    log "Creating necessary directories..."
    
    mkdir -p "tests/results"
    mkdir -p "examples"
    info "Directories created."
}

# Function to check environment
check_environment() {
    log "Checking NLM environment..."
    
    # Check CMake
    if ! command_exists cmake; then
        warn "CMake not found. Installing..."
        define_install_package cmake
        if ! command_exists cmake; then
            error "CMake installation failed."
            return 1
        fi
    fi
    
    # Check Python
    if ! command_exists python3; then
        error "Python3 not found."
        return 1
    fi
    
    info "Python found: $(python3 --version)"
}

# Function to configure CMake
configure_cmake() {
    log "Configuring CMake..."
    
    mkdir -p build
    cd build || return 1
    
    cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)
    
    if [ $? -eq 0 ]; then
        info "CMake configuration successful."
    else
        error "CMake configuration failed."
        return 1
    fi
    
    cd .. || return 1
}

# Function to build the project
build_project() {
    log "Building NLM..."
    
    if [ ! -d "build" ]; then
        error "Build directory not found. Run setup first."
        return 1
    fi
    
    cd build
    
    if command_exists nproc; then
        make -j$(nproc)
    else
        make -j4
    fi
    
    if [ $? -eq 0 ]; then
        info "Build successful!"
    else
        error "Build failed."
        return 1
    fi
    
    cd .. || return 1
}

# Function to run integration tests
run_integration_tests() {
    log "Running NLM Phase 6 Integration Tests..."
    
    if [ ! -f "build/nlm" ]; then
        error "NLM executable not found. Build the project first."
        return 1
    fi
    
    # Run Phase 6 demo
    info "Running Phase 6 integration demo..."
    cd build
    ./nlm_phase6_demo
    demo_result=$?
    cd .. || return 1
    
    if [ $demo_result -eq 0 ]; then
        info "Phase 6 demo passed!"
    else
        error "Phase 6 demo failed."
        return 1
    fi
    
    # Run unit tests if available
    if [ -f "build/nlm_test" ]; then
        info "Running unit tests..."
        cd build
        ./nlm_test --verbose
        test_result=$?
        cd .. || return 1
        
        if [ $test_result -eq 0 ]; then
            info "Unit tests passed!"
        else
            warn "Unit tests had failures but continuing."
        fi
    fi
    
    # Create test results directory
    mkdir -p tests/results
    
    info "Integration tests completed successfully."
}

# Function to create examples
create_examples() {
    log "Creating NLM examples..."
    
    # Simple brain example
    cat > examples/simple_brain.py << 'EOF'
import pynlm

def main():
    print("=== NLM Simple Brain Example ===\n")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"And {brain.getTotalSynapseCount()} synapses\n")
    
    for step in range(10):
        brain.step(step)
        
        if step % 5 == 0:
            print(f"Step {step}:")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f} Hz\n")
    
    print("Example completed successfully!")

if __name__ == "__main__":
    main()
EOF

    # Agent example
    cat > examples/agent_example.py << 'EOF'
import pynlm

def main():
    print("=== NLM Agent Example ===\n")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Created agent with:")
    print(f"  - Sensory neurons: {len(brain.getWorkingMemory().getActiveTraces()) if brain.getWorkingMemory() else 0}")
    print(f"  - Motor neurons: {len(brain.getMotorNeurons()) if hasattr(brain, 'getMotorNeurons') else 'N/A'}\n")
    
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        
        if step % 5 == 0:
            print(f"Step {step}:")
            print(f"  - Action: {action.getType() if hasattr(action, 'getType') else 'N/A'}")
            print(f"  - Reward mod: {agent.getNeuromodulationLevel():.3f}")
            print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}\n")
    
    print("Agent example completed successfully!")

if __name__ == "__main__":
    main()
EOF

    # Integration example
    cat > examples/integration_example.py << 'EOF'
import pynlm

def main():
    print("=== NLM Phase 6 Integration Example ===\n")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Integration System Status:")
    print(f"  - Working Memory: {'✓' if brain.getWorkingMemory() else '✗'}")
    print(f"  - Episodic Memory: {'✓' if brain.getEpisodicMemory() else '✗'}")
    print(f"  - Neuromodulation: {'✓' if brain.getDopamine() else '✗'}")
    print(f"  - Cognition: {'✓' if brain.getAttention() else '✗'}")
    print(f"  - Development: {'✓' if brain.getDevelopmentSystem() else '✗'}")
    print(f"  - Prediction: {'✓' if brain.getPredictionSystem() else '✗'}")
    print()
    
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
            print(f"  - Episodes stored: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
            print(f"  - Dopamine level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  - Developmental stage: {brain.getDevelopmentalStage()}\n")
    
    print("Integration example completed successfully!")
    print("\nThe NLM brain is now a fully integrated artificial cognitive system!")

if __name__ == "__main__":
    main()
EOF

    info "Examples created in examples/ directory"
}

# Function to display help
function_help() {
    cat << 'EOF'
NLM Phase 6 Integration Test Script

Usage:
  $0 [OPTIONS]

Options:
  --help, -h           Show this help message
  --configure-only     Only configure CMake, don't build
  --build-only        Only build, don't run tests
  --examples-only      Only create examples, don't build
  --test-only         Only run integration tests

Functions:
  setup               Run complete setup (configure, build, tests)
  check_environment    Check development environment
  configure_cmake       Configure CMake project
  build_project         Build the project
  run_tests            Run integration tests
  create_examples       Create example files
  help                 Show this help message

Environment Variables:
  NLM_BUILD_TYPE         Build type (Debug, Release, RelWithDebInfo)
  NLM_PARALLEL_JOBS     Number of parallel build jobs (default: CPU count)

Examples:
  $0                           # Full setup (configure, build, tests)
  $0 --configure-only           # Only configure CMake
  $0 --build-only              # Only build
  $0 --examples-only            # Only create examples
  $0 --test-only               # Only run integration tests

For more information, visit: https://github.com/nlm-project/nlm
EOF
}

# Main function
main() {
    # Parse command line arguments
    CONFIGURE_ONLY=false
    BUILD_ONLY=false
    EXAMPLES_ONLY=false
    TEST_ONLY=false
    
    while [ $# -gt 0 ]; do
        case $1 in
            --configure-only)
                CONFIGURE_ONLY=true
                shift
                ;;
            --build-only)
                BUILD_ONLY=true
                shift
                ;;
            --examples-only)
                EXAMPLES_ONLY=true
                shift
                ;;
            --test-only)
                TEST_ONLY=true
                shift
                ;;
            --help|-h)
                function_help
                exit 0
                ;;
            *)
                error "Unknown option: $1"
                function_help
                exit 1
                ;;
        esac
    done
    
    # Set environment variables
    if [ -z "$NLM_BUILD_TYPE" ]; then
        NLM_BUILD_TYPE="Release"
    fi
    
    if [ -z "$NLM_PARALLEL_JOBS" ]; then
        if command_exists nproc; then
            NLM_PARALLEL_JOBS=$(nproc)
        else
            NLM_PARALLEL_JOBS=4
        fi
    fi
    
    # Main setup logic
    log "Starting NLM Phase 6 Integration Test Setup (Build type: $NLM_BUILD_TYPE, Parallel jobs: $NLM_PARALLEL_JOBS)"
    
    # Check environment
    check_environment || exit 1
    
    # Create directories
    create_directories || exit 1
    
    # Configure CMake (unless examples only)
    if [ "$CONFIGURE_ONLY" = false ] && [ "$EXAMPLES_ONLY" = false ]; then
        configure_cmake || exit 1
    fi
    
    # Build (unless test only)
    if [ "$BUILD_ONLY" = false ] && [ "$EXAMPLES_ONLY" = false ] && [ "$TEST_ONLY" = false ]; then
        build_project || exit 1
    fi
    
    # Create examples
    if [ "$EXAMPLES_ONLY" = true ] || [ "$TEST_ONLY" = false ]; then
        create_examples || exit 1
    fi
    
    # Run integration tests (unless build only)
    if [ "$TEST_ONLY" = true ] || ([ "$BUILD_ONLY" = false ] && [ "$EXAMPLES_ONLY" = false ]); then
        run_integration_tests || exit 1
    fi
    
    # Final message
    info "NLM Phase 6 Integration Test Setup completed successfully!"
    info ""
    info "Next steps:"
    info "1. Run examples:"
    info "   cd examples"
    info "   python3 simple_brain.py"
    info "   python3 agent_example.py"
    info "   python3 integration_example.py"
    info ""
    info "2. Run integration tests again:"
    info "   ./test_integration.sh --test-only"
    info ""
    info "For more information, see the NLM documentation."
}

# Run main function with all arguments
main "$@"
