#!/usr/bin/env bash

# Simple build script for NLM Python bindings
# This script shows how to build and test the Python bindings

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${GREEN}[INFO] Building NLM Python bindings...${NC}"

# Check if scikit-build-core is available
if python3 -c "import scikit_build_core" 2>/dev/null; then
    echo -e "${GREEN}[INFO] scikit-build-core is available${NC}"
    echo -e "${GREEN}[INFO] Building with scikit-build-core...${NC}"
    python3 -m pip install --upgrade pip setuptools wheel
    python3 -m pip install .
    
    echo -e "${GREEN}[INFO] Build completed. Testing import...${NC}"
    python3 -c "
import sys
try:
    import pynlm
    print(f'Successfully imported pynlm version {pynlm.__version__}')
    print(f'Description: {pynlm.__doc__.strip() if pynlm.__doc__ else \"NLM Python Bindings\"}')
    
    # Test basic functionality
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f'✓ Brain created: {brain.getTotalNeuronCount()} neurons')
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    print(f'✓ World created: {world.getWidth()} x {world.getHeight()}')
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print(f'✓ Agent created: curiosity enabled = {agent.isCuriosityEnabled()}')
    
    print('')
    print('✓ All basic functionality tests passed!')
    print('✓ NLM Python bindings are working correctly!')
    
except ImportError as e:
    print(f'✗ Failed to import pynlm: {e}')
    sys.exit(1)
except Exception as e:
    print(f'✗ Error during testing: {e}')
    sys.exit(1)
    "
else
    echo -e "${RED}[ERROR] scikit-build-core is not available${NC}"
    echo -e "${YELLOW}[WARNING] Please install scikit-build-core:${NC}"
    echo "  pip install scikit-build-core pybind11"
    echo ""
    echo -e "${YELLOW}[WARNING] After installing, run:${NC}"
    echo "  ./build_python.sh"
    exit 1
fi

echo -e "${GREEN}[INFO] Build and test completed successfully!${NC}"
echo -e "${GREEN}[INFO] You can now import pynlm in Python!${NC}"