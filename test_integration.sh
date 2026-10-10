#!/usr/bin/env bash

# Integration Verifier Test Script
# This script demonstrates how to use the IntegrationVerifier framework

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "==============================================================================="
echo "NLM Integration Verifier Test Script"
echo "==============================================================================="
echo ""
echo "This script demonstrates the Integration Verification Framework for NLM brain integration."
echo ""
echo "The framework validates the following integration points:"
echo "  1. Brain-Memory integration"
echo "  2. Neuromodulation-Plasticity integration"
echo "  3. Development-Plasticity integration"
echo "  4. Prediction-Memory integration"
echo "  5. Cognition-Memory integration"
echo "  6. Sensory-Memory integration"
echo "  7. Motor-Cognition integration"
echo "  8. World-Environment integration"
echo "  9. Development-Brain integration"
echo "  10. Neuromodulation-Brain integration"
echo "  11. System connectivity"
echo "  12. Plasticity consistency"
echo "  13. Neuromodulation balance"
echo "  14. Memory capacity"
echo ""
echo "Usage: ./test_integration.sh [build|run|all]"
echo ""
echo "Options:"
echo "  build   - Build the integration verifier library"
echo "  run     - Build and run integration verification tests"
echo "  all     - Build, run, and generate detailed report"
echo ""

# Check if we need to build
if [[ "$1" == "build" || "$1" == "all" ]]; then
    echo -e "${YELLOW}Building integration verifier...${NC}"
    cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d3548bef-e4dd-44dc-a82b-f96d4c3d7aff
    
    # Check if CMakeLists.txt has been updated
    if ! grep -q "nlm_integration" CMakeLists.txt; then
        echo -e "${RED}ERROR: CMakeLists.txt not updated properly${NC}"
        exit 1
    fi
    
    # Try to build
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    
    if [[ $? -ne 0 ]]; then
        echo -e "${RED}Build failed!${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}Build successful!${NC}"
    echo "The integration verifier library (nlm_integration) has been built."
fi

# Check if we need to run
if [[ "$1" == "run" || "$1" == "all" ]]; then
    echo -e "${YELLOW}Running integration verification tests...${NC}"
    cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d3548bef-e4dd-44dc-a82b-f96d4c3d7aff/build
    
    # Check if the executable exists
    if [[ ! -f "nlm" ]]; then
        echo -e "${YELLOW}Building executable first...${NC}"
        cd .. && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)
        
        if [[ $? -ne 0 ]]; then
            echo -e "${RED}Build failed!${NC}"
            exit 1
        fi
    fi
    
    # Run the integration verification executable
    echo ""
    echo "Running NLM integration verification..."
    echo "This will test all NLM brain subsystems and report any integration issues."
    echo ""
    
    ./nlm 2>&1 | grep -A 50 "=== Test 4: Integration Verification Framework ==="
    
    if [[ $? -eq 0 ]]; then
        echo ""
        echo -e "${GREEN}Integration verification completed!${NC}"
        echo "Check the output above for detailed integration results."
    else
        echo -e "${RED}Integration verification failed or was not executed.${NC}"
        exit 1
    fi
fi

echo ""
echo "==============================================================================="
echo "Integration verification script completed."
echo "==============================================================================="
