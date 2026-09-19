# NLM (熙然) - Neural Learning Machine

## Documentation Index

### Core Documentation
- [README.md](README.md) - Project overview and architecture
- [HOW_TO_USE.md](HOW_TO_USE.md) - Installation and basic usage guide
- [easy_usage.md](easy_usage.md) - Beginner-friendly usage guide

### Technical Documentation
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture and design
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [docs/ROADMAP.md](docs/ROADMAP.md) - Project roadmap and future plans
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results
- [docs/LIMITATIONS.md](docs/LIMITATIONS.md) - Known limitations and research focus
- [docs/PERFORMANCE.md](docs/PERFORMANCE.md) - Performance characteristics and optimizations
- [docs/PHASE6_ARCHITECTURE.md](docs/PHASE6_ARCHITECTURE.md) - Phase 6 integration architecture
- [docs/PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md) - Phase 6 final results
- [docs/FINAL_LIMITATIONS.md](docs/FINAL_LIMITATIONS.md) - Final limitations and constraints
- [docs/FAILURES.md](docs/FAILURES.md) - Project failures and lessons learned
- [docs/SCALING.md](docs/SCALING.md) - Scaling considerations and optimizations
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Detailed experiment documentation
- [docs/RESEARCH_METHODS.md](docs/RESEARCH_METHODS.md) - Research methodologies
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit documentation
- [docs/BIOLOGICAL_FIDELITY.md](docs/BIOLOGICAL_FIDELITY.md) - Biological fidelity analysis

### Python API Documentation
- [pynlm_api_docs.md](pynlm_api_docs.md) - Complete Python bindings API reference
- [python/bindings.cpp](python/bindings.cpp) - Python binding implementation

### Development Documentation
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines (to be created)
- [DEVELOPMENT.md](DEVELOPMENT.md) - Development workflow and setup (to be created)
- [API_GUIDE.md](API_GUIDE.md) - Core API development guide (to be created)

## Project Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Experimental Architecture**: Brain-inspired spiking neural network simulator
- **Multi-Stage Development**: Implemented in phases, with Phase 6 being the final integration
- **Full Lifecycle Simulation**: Developmental stages from initial to aging
- **Memory Integration**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error systems
- **Structural Plasticity**: Synaptogenesis and pruning for connection growth
- **Embodied Interaction**: Integration with simulated environments
- **Learning Capabilities**: STDP, Hebbian learning, and reward-modulated plasticity

## Project Structure

```
NLM/
├── CMakeLists.txt                    # CMake build configuration
├── README.md                         # Main project documentation
├── HOW_TO_USE.md                     # Installation and usage guide
├── easy_usage.md                     # Quick start guide
├── pyproject.toml                    # Python bindings configuration
├── python/                           # Python bindings
│   └── bindings.cpp                  # Pybind11 bindings implementation
├── src/                              # C++ source code
│   ├── core/                        # Core utilities
│   │   ├── Config/                  # Configuration management
│   │   ├── Random/                  # Random number generation
│   │   ├── Logger/                  # Logging system
│   │   └── SimulationClock/         # Timing system
│   ├── brain/                       # Neural components
│   │   ├── Neuron.cpp                # LIF neuron implementation
│   │   ├── Synapse.cpp               # Synaptic connection model
│   │   ├── NeuralPopulation.cpp      # Neuron population management
│   │   ├── NeuralRegion.cpp          # Brain region organization
│   │   ├── Brain.cpp                 # Central brain controller
│   │   └── Neuron.hpp                # Neuron header (in .hpp files)
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity mechanisms
│   ├── development/                 # Developmental system
│   ├── neuromodulation/              # Neuromodulator systems
│   ├── memory/                      # Memory systems
│   ├── prediction/                  # Prediction systems
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor system
│   ├── environment/                 # Environment interface
│   ├── experiments/                 # Experiment framework
│   └── visualization/               # Visualization tools
├── tests/                           # Test suite
│   ├── test_*.cpp                   # C++ unit tests
│   └── test_*.py                    # Python tests
├── configs/                         # Configuration files
│   └── default.cfg                  # Default configuration
└── docs/                            # Documentation
    ├── *.md                         # Documentation files
    └── *.png                        # Diagrams and illustrations
```

## Getting Started

### Quick Installation

```bash
# Install Python bindings
pip install pynlm

# Or build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Basic Usage

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

## Documentation Navigation

### For Users

- **HOW_TO_USE.md**: Complete installation and usage guide
- **easy_usage.md**: Beginner-friendly step-by-step instructions
- **pynlm_api_docs.md**: Complete Python API reference

### For Developers

- **docs/ARCHITECTURE.md**: System architecture and component relationships
- **docs/SCIENCE.md**: Scientific background and theoretical foundations
- **docs/ROADMAP.md**: Project roadmap and development phases

### For Researchers

- **docs/EXPERIMENTS.md**: Detailed experiment descriptions and results
- **docs/LIMITATIONS.md**: Scientific limitations and future research directions
- **docs/RESEARCH_METHODS.md**: Research methodologies and experimental design

## Version Information

### Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Build System

The project uses CMake with modern C++20 features and scikit-build for Python bindings integration.

### Python Bindings

The Python bindings provide access to the full NLM simulation capabilities through an intuitive Python API. All C++ classes and functionality are exposed through pybind11.

## Support and Community

### Getting Help

- **Documentation**: All documentation is available in the `docs/` directory
- **Python API**: Complete API reference in `pynlm_api_docs.md`
- **Installation**: Step-by-step instructions in `HOW_TO_USE.md`

### Contributing

For information on contributing to the project, please refer to the contribution guidelines (to be created).

### Issues and Bug Reports

For bug reports and feature requests, please create issues in the project repository.

## Copyright and License

Copyright (c) NLM Project

MIT License

```
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
