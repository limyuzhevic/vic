#!/usr/bin/env bash

# NLM Development Setup Script
# This script sets up the development environment and creates useful tools

set -e

echo "Setting up NLM development environment..."

# Check if we're in the right directory
if [ ! -f "pyproject.toml" ]; then
    echo "Error: pyproject.toml not found. Please run from project root."
    exit 1
fi

# Build the Python bindings
if [ ! -d "python" ]; then
    echo "Error: python directory not found."
    exit 1
fi

# Try to build using pip
if command -v pip &> /dev/null; then
    echo "Building pynlm Python bindings..."
    pip install -e .
else
    echo "pip not found. Please install dependencies manually:"
    echo "  pip install scikit-build-core pybind11>=2.11.0"
    echo "  pip install -e ."
    exit 1
fi

# Create development shortcuts
cat > "dev.sh" << 'EOF'
#!/bin/bash
# Development shortcuts

cd "$(dirname "$0")"

function nlm-dev() {
    echo "NLM Development Mode"
    echo "===================="
    echo "Available commands:"
    echo "  nlm          - Main CLI tool"
    echo "  nlm-example  - Run built-in examples"
    echo "  nlm-bench    - Run benchmarks"
    echo "  nlm-config   - Configuration management"
    echo ""
    echo "Getting started: nlm run --steps 100"
}

cat > "examples.sh" << 'ENDOFFILE'
#!/bin/bash
# Run NLM examples

echo "NLM Examples"
echo "============"
echo ""
echo "Running simple brain example..."
python3 -c "
import sys
sys.path.insert(0, 'python')
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(50):
    brain.step(i)

print(f'Success! Brain has {brain.getTotalNeuronCount()} neurons and {brain.getTotalSpikeCount()} spikes')
"

echo ""
echo "Running world simulation example..."
python3 -c "
import sys
sys.path.insert(0, 'python')
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(30):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f'Simulation complete!')
print(f'  Firing neurons: {brain.getFiringNeuronCount()}')
print(f'  Total spikes: {brain.getTotalSpikeCount()}')
"

ENDOFFILE

chmod +x examples.sh

# Create performance tools
cat > "profile.sh" << 'EOF'
#!/bin/bash
# Performance profiling tools

echo "NLM Performance Profiling"
echo "========================="
echo ""
echo "To profile a simulation, use:"
echo "  python3 -m cProfile -o profile.stats nlm run --steps 1000"
echo ""
echo "To analyze the profile:"
echo "  python3 -c \"import pstats; p = pstats.Stats('profile.stats'); p.sort_stats('cumulative').print_stats(50)\""

echo ""
echo "To run a benchmark:"
echo "  nlm benchmark --steps 500 --repeats 10"

echo ""
echo "To check system resources:"
echo "  nmltop"
EOF

chmod +x profile.sh

# Create project documentation
cat > "PROJECT_SETUP.md" << 'EOF'
# NLM Project Setup

## Quick Start

1. Ensure Python 3.8+ is installed
2. Build the project:
   ```bash
   pip install -e .
   ```

3. Test the installation:
   ```bash
   nlm run --steps 100
   ```

## Development Workflow

### 1. Build Changes
```bash
# Build the project
pip install -e .

# Run tests
pytest tests/

# Build with specific features
export NLM_BUILD_FEATURES="performance,memory"
pip install -e .
```

### 2. Run Simulations
```bash
# Simple brain simulation
nlm run --steps 1000

# World simulation
nlm run --steps 500 --world-config "width=20 height=20"

# Agent with learning
nlm run --steps 1000 --agent-config "reward_modulation=true curiosity=true"
```

### 3. Configuration
```bash
# List available options
nlm config list

# Create custom config
nlm config create --parameters "neuron_count=1000 learning_rate=0.01"

# Save and load brain states
nlm brain save --file brain_state.json
nlm brain load --file brain_state.json
```

### 4. Performance
```bash
# Run benchmark
nlm benchmark --steps 500 --repeats 5

# Profile performance
python3 -m cProfile -o profile.stats nlm run --steps 1000
```

### 5. Examples
```bash
# Run built-in examples
./examples.sh

# Run specific example
nlm example simple
```

## Development Tools

- `dev.sh` - Development mode shortcuts
- `profile.sh` - Performance profiling
- `examples.sh` - Run examples

## Building from Source

### Prerequisites
```bash
apt-get update
apt-get install -y build-essential cmake python3-dev
pip install scikit-build-core pybind11>=2.11.0
```

### Build Process
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e .
```

## Configuration Files

- `pyproject.toml` - Python package configuration
- `CMakeLists.txt` - C++ build system
- `python/CMakeLists.txt` - Python bindings
- `configs/` - Sample configuration files

## Next Steps

1. **Choose a project**: Select a feature to implement
2. **Set up development environment**: Use the setup script
3. **Test changes**: Run existing tests
4. **Create examples**: Add to examples.sh
5. **Profile performance**: Use profile.sh

## Common Issues

### pynlm not available
```bash
# Rebuild the bindings
pip install --force-reinstall -e .
```

### Build errors
```bash
# Clean build
rm -rf build dist *.egg-info
pip install --force-reinstall -e .
```

### Performance issues
```bash
# Use optimized build
export NLM_BUILD_TYPE=Release
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Support

For issues, refer to the documentation in the `docs/` directory or join the NLM community.
EOF

chmod +x dev.sh
chmod +x profile.sh

# Create .gitignore if it doesn't exist
cat > ".gitignore" << 'EOF'
# Build artifacts
build/
dist/
*.egg-info/
__pycache__/
*.pyc
*.so
*.pyd

# Python environment
.venv/
env/
venv/

# Configuration files
*.json
*.yaml
*.yml

# Test results
*.xml

# IDE files
.vscode/
.idea/
*.swp
*.swo

# OS files
.DS_Store
Thumbs.db

# Documentation
*.md
!README.md
!CONTRIBUTING.md
!LICENSE

# Temporary files
*.tmp
*.temp

# Debug files
core
.valgrind*</EOF

echo "Setup complete!"
echo ""
echo "Created:"
echo "  - nlm (main CLI tool)"
echo "  - dev.sh (development shortcuts)"
echo "  - profile.sh (performance profiling)"
echo "  - examples.sh (example runner)"
echo "  - PROJECT_SETUP.md (comprehensive documentation)"
echo "  - .gitignore (standard git ignore)"
echo ""
echo "To get started:"
echo "  1. Run: chmod +x nlm"
echo "  2. Run: ./dev.sh"
echo "  3. Try: nlm run --steps 100"
