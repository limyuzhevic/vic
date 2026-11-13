#!/usr/bin/env bash

# NLM (熙然) - Neural Learning Machine Build and Configuration Script
# Phase 2: Real Neural Computation

# Usage:
#   ./nlm-build.sh [options]
#   ./nlm-build.sh help      # Show available options
#   ./nlm-build.sh config     # Show configuration options
#   ./nlm-build.sh run        # Run the demo with default settings
#   ./nlm-build.sh benchmark   # Run benchmark tests

# Colors for output
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
NC="\033[0m" # No Color

# Default configuration values
DEFAULT_NEURON_COUNT=500
DEFAULT_REGION_COUNT=1
DEFAULT_CONNECTION_PROBABILITY=0.15
DEFAULT_STDP_LTP_WEIGHT=0.02
DEFAULT_STDP_LTD_WEIGHT=0.015
DEFAULT_STDP_TAU=20.0
DEFAULT_SIMULATION_TIMESTEP=0.001
DEFAULT_RANDOM_SEED=42

echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║     NLM Build and Configuration Script                    │${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir -p build
fi

# Navigate to build directory
mkdir -p build
cd build

# Configure CMake with options
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF \
    -DNLM_BUILD_PYTHON_BINDINGS=OFF \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF

# Build the project with parallel compilation
echo -e "${GREEN}Building NLM (Phase 2 neural system)...${NC}"
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Build successful!${NC}"
    echo -e "${GREEN}Available binaries:${NC}"
    echo -e "  ./nlm           # Phase 2 demo (basic neural computation)"
    echo -e "  ./nlm_phase3_demo # Phase 3 world interaction demo"
    echo -e "  ./nlm_benchmark  # Performance benchmark"
    echo -e "  ./nlm_phase6_demo # Phase 6 integration test"
else
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi

# Create configuration help function
generate_help() {
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                 NLM Configuration Help               │${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${YELLOW}Basic Usage:${NC}"
    echo "  ./nlm                 # Run Phase 2 demo"
    echo "  ./nlm --help          # Show help"
    echo "  ./nlm --config my.cfg # Load custom config"
    echo ""
    echo -e "${YELLOW}Command Line Options:${NC}"
    echo "  --config=file      # Load specific configuration file"
    echo "  --config file       # Alternative format"
    echo "  --debug            # Enable debug logging"
    echo "  --verbose           # Enable verbose output"
    echo "  --quiet            # Suppress output"
    echo "  --log-file=file    # Log to file"
    echo "  --no-colors        # Disable colored output"
    echo "  --neurons=N        # Override neuron count"
    echo "  --regions=N        # Override region count"
    echo "  --connection=N     # Override connection probability"
    echo "  --stdp-ltp=N       # Override STDP LTP weight"
    echo "  --stdp-ltd=N       # Override STDP LTD weight"
    echo "  --simulation=N     # Override simulation timestep"
    echo ""
    echo -e "${YELLOW}Configuration File Format:${NC}"
    echo "  key=value          # Simple key=value pairs"
    echo "  # or use JSON/XML format for complex configurations"
    echo ""
    echo -e "${YELLOW}Sample Configuration File:${NC}"
    echo "  random_seed=42"
    echo "  simulation_timestep=0.001"
    echo "  neuron_count=500"
    echo "  region_count=1"
    echo "  connection_probability=0.15"
    echo "  stdp_ltp_weight=0.02"
    echo "  stdp_ltd_weight=0.015"
    echo "  synaptogenesis_rate=0.0001"
    echo "  pruning_rate=0.00001"
    echo ""
    echo -e "${YELLOW}Performance Tips:${NC}"
    echo "  For faster demo runs: neuron_count=100, max_steps=1000"
    echo "  For realistic simulation: neuron_count=1000, max_steps=10000"
    echo "  Use --debug for troubleshooting"
    echo ""
}

# Show configuration help
if [ "$1" == "help" ] || [ "$1" == "--help" ]; then
    generate_help
    exit 0
fi

# Show configuration options
if [ "$1" == "config" ] || [ "$1" == "--config" ]; then
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                 NLM Configuration                     │${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${YELLOW}Default Configuration Values:${NC}"
    echo "  random_seed: ${DEFAULT_RANDOM_SEED}"
    echo "  simulation_timestep: ${DEFAULT_SIMULATION_TIMESTEP}s"
    echo "  neuron_count: ${DEFAULT_NEURON_COUNT}"
    echo "  region_count: ${DEFAULT_REGION_COUNT}"
    echo "  connection_probability: ${DEFAULT_CONNECTION_PROBABILITY}"
    echo "  stdp_ltp_weight: ${DEFAULT_STDP_LTP_WEIGHT}"
    hadder_stdp_ltd_weight: ${DEFAULT_STDP_LTD_WEIGHT}"
    echo "  synaptogenesis_rate: 0.0001"
    echo "  pruning_rate: 0.00001"
    echo ""
    echo -e "${YELLOW}Environment Variables (override defaults):${NC}"
    echo "  NLM_RANDOM_SEED"
    echo "  NLM_SIMULATION_TIMESTEP"
    echo "  NLM_NEURON_COUNT"
    echo "  NLM_REGION_COUNT"
    echo "  NLM_CONNECTION_PROBABILITY"
    echo ""
    echo -e "${YELLOW}Configuration File Location:${NC}"
    echo "  Config files should be placed in the 'configs/' directory"
    echo "  Example: configs/default.cfg, configs/custom.cfg"
    exit 0
fi

# Run benchmark mode
if [ "$1" == "benchmark" ] || [ "$1" == "--benchmark" ]; then
    echo -e "${YELLOW}Running Performance Benchmark...${NC}"
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                 NLM Benchmark Test                     │${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo "Running benchmark with default settings..."
    echo "Note: This may take several minutes depending on system resources"
    echo ""
    
    # Run benchmark with optimized settings for testing
    ./nlm_benchmark --neuron-count=100 --max-steps=500 --runs=3
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Benchmark completed successfully!${NC}"
        echo -e "${GREEN}Benchmark results saved to benchmark_results.txt${NC}"
    else
        echo -e "${RED}✗ Benchmark failed${NC}"
        exit 1
    fi
    exit 0
fi

# Run demo mode
echo -e "${YELLOW}Running NLM Phase 2 Demo...${NC}"
echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                 NLM Phase 2 Demo                        │${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo "This demo demonstrates Phase 2 of the NLM (熙然) project:"
    echo "  - Real LIF neuron dynamics"
    echo "  - Event-driven spike propagation with delays"
    echo "  - STDP and Hebbian plasticity rules"
    echo "  - Structural plasticity (synaptogenesis/pruning)"
    echo ""
    echo "The demo runs three sequential tests:"
    echo "  1. Basic connectivity test"
    echo "  2. Plasticity learning experiment"
    echo "  3. STDP verification test"
    echo ""
    echo -e "${YELLOW}Press Ctrl+C to interrupt${NC}"
    echo ""
    
    # Run the demo executable
    ./nlm "$@"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Phase 2 Demo completed successfully!${NC}"
        echo -e "${GREEN}The NLM brain is now functioning as an artificial neural substrate${NC}"
        echo -e "${GREEN}capable of changing its synaptic connections through experience.${NC}"
    else
        echo -e "${RED}✗ Phase 2 Demo failed${NC}"
        exit 1
    fi
fi
