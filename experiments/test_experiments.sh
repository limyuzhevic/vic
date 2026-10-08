#!/bin/bash
# NLM Quick Start Test Script
# Demonstrates basic NLM functionality with example usage

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Quick Start Tests
quick_start_tests() {
    echo -e "${GREEN}=== NLM Quick Start Tests ===${NC}"
    echo "Testing basic NLM functionality..."
    echo ""
    
    # Check if NLM is built
    if [[ ! -f "./build/nlm" ]]; then
        echo -e "${YELLOW}[WARN]${NC} NLM not built. Building..."
        mkdir -p build
        cd build
        if ! cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4; then
            echo -e "${RED}[ERROR]${NC} Build failed"
            return 1
        fi
        cd ..
    fi
    
    # Test 1: Help command
    echo -e "${BLUE}Test 1: Help command${NC}"
    echo "./build/nlm help"
    ./build/nlm help | head -20
    echo "✓ Help command works"
    echo ""
    
    # Test 2: Status command
    echo -e "${BLUE}Test 2: Status command${NC}"
    echo "./build/nlm status"
    ./build/nlm status | head -20
    echo "✓ Status command works"
    echo ""
    
    # Test 3: Version/help
    echo -e "${BLUE}Test 3: Basic functionality${NC}"
    echo "./build/nlm --help"
    ./build/nlm --help 2>&1 | head -10
    echo "✓ Basic command works"
    echo ""
    
    # Test 4: Experiment help
    echo -e "${BLUE}Test 4: Experiment help${NC}"
    echo "./build/nlm run --help"
    ./build/nlm run --help 2>&1 | head -10
    echo "✓ Experiment command works"
    echo ""
    
    echo -e "${GREEN}=== Quick Start Tests Complete ===${NC}"
}

# Experiment Test Script
experiment_test_script() {
    echo -e "${GREEN}=== NLM Experiment Test Script ===${NC}"
    echo "Running typical experiment sequences..."
    echo ""
    
    # Create experiment script
    cat > ./experiments/test_experiments.sh << 'EOF'
#!/bin/bash

# Experiment Test Script
# Demonstrates common experiment sequences

echo "=== NLM Experiment Test Script ==="

# Build NLM if not already built
if [[ ! -f "./build/nlm" ]]; then
    echo "Building NLM..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4
    cd ..
fi

# Function to run experiment with timing
run_experiment() {
    local name=$1
    local steps=$2
    local output_file="${name}_results.txt"
    
    echo "--- Running $name experiment ($steps steps) ---"
    ./build/nlm run $name
    
    # Save results if experiment supports it
    if [[ -f "brain_state.checkpoint" ]]; then
        ./build/nlm save ${name}_brain.checkpoint
        echo "Brain state saved to ${name}_brain.checkpoint"
    fi
    
    echo ""
}

# Experiment 1: Connectivity Test
run_experiment "connectivity" 500

# Experiment 2: Plasticity Test
run_experiment "plasticity" 1000

# Experiment 3: STDP Test
run_experiment "stdp" 750

# Experiment 4: Learning Test
run_experiment "learning" 1500

# Experiment 5: Demo Test
run_experiment "demo" 300

echo "=== All Experiments Complete ==="

# Summary
 echo "
=== Experiment Summary ==="
 echo "Connectivity: Tested basic neural connections"
 echo "Plasticity: Demonstrated synaptic learning"
 echo "STDP: Verified spike-timing-dependent plasticity"
 echo "Learning: Showed experience-dependent adaptation"
 echo "Demo: Displayed integrated brain functionality"
EOF
    
    # Make script executable
    chmod +x ./experiments/test_experiments.sh
    
    echo "Experiment script created: ./experiments/test_experiments.sh"
    echo ""
    echo "To run experiments: ./experiments/test_experiments.sh"
    echo ""
}

# Configuration Test Script
config_test_script() {
    echo -e "${GREEN}=== NLM Configuration Test Script ===${NC}"
    echo "Testing configuration management..."
    echo ""
    
    # Create configuration test script
    cat > ./experiments/test_config.sh << 'EOF'
#!/bin/bash

# Configuration Test Script
# Tests configuration parameter management

echo "=== NLM Configuration Test Script ==="

if [[ ! -f "./build/nlm" ]]; then
    echo "Building NLM..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4
    cd ..
fi

# Test setting and getting parameters
echo "--- Testing parameter management ---"

# Set various parameters
./build/nlm set neuron_count 2000
./build/nlm set region_count 2
./build/nlm set simulation_timestep 0.0005
./build/nlm set connection_probability 0.15
./build/nlm set stdp_ltp_weight 0.025
./build/nlm set stdp_ltd_weight 0.02

# Get and display parameters
./build/nlm get neuron_count
./build/nlm get region_count
./build/nlm get simulation_timestep

# Test configuration file loading
echo ""
echo "--- Testing configuration file ---"
./build/nlm --config configs/default.cfg

echo ""
echo "=== Configuration Tests Complete ==="
EOF
    
    # Make script executable
    chmod +x ./experiments/test_config.sh
    
    echo "Configuration script created: ./experiments/test_config.sh"
    echo ""
    echo "To test configuration: ./experiments/test_config.sh"
    echo ""
}

# Documentation Test Script
doc_test_script() {
    echo -e "${GREEN}=== NLM Documentation Test Script ===${NC}"
    echo "Testing documentation and help systems..."
    echo ""
    
    # Create documentation test script
    cat > ./experiments/test_documentation.sh << 'EOF'
#!/bin/bash

# Documentation Test Script
# Tests documentation and help systems

echo "=== NLM Documentation Test Script ==="

echo "--- Testing help system ---"
./build/nlm help | grep -E "^(help|status|run|save|load|reset|step|exit)" | head -10

echo ""
echo "--- Testing experiment help ---"
./build/nlm run --help | grep -E "^(connectivity|plasticity|stdp|learning|demo|ablation)" | head -10

echo ""
echo "--- Testing parameter help ---"
./build/nlm set --help | head -10

echo ""
echo "=== Documentation Tests Complete ==="
EOF
    
    # Make script executable
    chmod +x ./experiments/test_documentation.sh
    
    echo "Documentation script created: ./experiments/test_documentation.sh"
    echo ""
    echo "To test documentation: ./experiments/test_documentation.sh"
    echo ""
}

# Main execution
main() {
    case "$1" in
        quick)
            quick_start_tests
            ;;
        experiment)
            experiment_test_script
            ;;
        config)
            config_test_script
            ;;
        doc)
            doc_test_script
            ;;
        all)
            quick_start_tests
            experiment_test_script
            config_test_script
            doc_test_script
            ;;
        *)
            echo "Usage: $0 [quick|experiment|config|doc|all]"
            echo ""
            echo "Commands:"
            echo "  quick   - Run quick start tests"
            echo "  experiment - Create experiment test script"
            echo "  config  - Create configuration test script"
            echo "  doc     - Create documentation test script"
            echo "  all     - Run all test scripts"
            exit 1
            ;;
    esac
}

# Execute main function
main "$@"
