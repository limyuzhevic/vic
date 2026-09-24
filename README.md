# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

### Modern Build System with Python Bindings

NLM now includes a modern build system with comprehensive Python bindings:

```bash
# Standard C++ build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

```bash
# Python bindings (automatically included)
pip install scikit-build-core pybind11
pip install .
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `pynlm` | Python Module | NLM Python bindings |

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Interface

NLM now includes comprehensive Python bindings:

```python
import pynlm

# Create configuration
config = pynlm.create_default_config()

# Create brain
brain = pynlm.create_brain(config)
brain.initialize()

# Create agent
agent = pynlm.create_agent_brain(brain)

# Create world
world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)
agent.enable_structural_plasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
```

### Command Line Interface

Enhanced CLI with comprehensive options:

```bash
nlm --help              # Show help information
nlm --version           # Show version and build information
nlm --config=myconfig.cfg # Load custom configuration
nlm --neuron_count=2000   # Create larger network
nlm --test=1             # Run specific test (1-3)
nlm --verbose            # Enable verbose output
nlm --quiet              # Quiet mode (errors only)
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Modern CMake configuration
├── pyproject.toml                    # Python packaging configuration
├── README.md                         # This documentation
├── docs/                            # Detailed documentation
│   ├── ARCHITECTURE.md               # System architecture
│   ├── SCIENCE.md                    # Scientific background
│   ├── ROADMAP.md                    # Development roadmap
│   ├── EXPERIMENTS.md                # Experiment descriptions
│   ├── LIMITATIONS.md                # Scientific limitations
│   └── PHASE6_FINAL_AUDIT.md         # Phase 6 audit
├── src/                             # Source code
│   ├── core/                        # Core utilities
│   │   ├── Config/                  # Configuration management
│   │   ├── Logger/                  # Logging system
│   │   ├── Random/                  # Random number generation
│   │   └── SimulationClock/         # Time management
│   ├── brain/                       # Neural components
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity mechanisms
│   ├── development/                 # Development system
│   ├── neuromodulation/              # Neuromodulation
│   ├── memory/                      # Memory systems
│   ├── cognition/                   # Cognitive systems
│   ├── sensory/                      # Sensory processing
│   ├── prediction/                   # Prediction systems
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiments
│   └── visualization/                # Visualization
│   ├── pynlm.cpp                    # Python bindings (NEW)
│   └── core/Constants.hpp            # Constants system (NEW)
├── src/agent/                       # Agent system
├── src/world/                       # World simulation
├── configs/                         # Configuration files
├── tests/                           # Test suite
└── build/                          # Build directory (ignored)
```

## Key Improvements

### 1. **Enhanced Code Organization**
- Refactored monolithic `main.cpp` (449 lines → 319 lines) into modular, focused functions
- Extracted configuration validation logic into dedicated functions
- Improved separation of concerns across all major components

### 2. **Constants System**
- Created comprehensive `Constants.hpp` with biologically-inspired values
- Replaced magic numbers throughout codebase with named constants
- Improved maintainability and documentation

### 3. **Python Bindings**
- Added complete `pynlm` Python module using pybind11
- Provides access to all major NLM functionality
- Includes agent, brain, world, and configuration interfaces
- Follows Python conventions and best practices

### 4. **Enhanced CLI Interface**
- Comprehensive argument parsing with help/version options
- Support for configuration files, test modes, and verbose output
- Better error handling and user guidance
- Additional options for debugging and performance monitoring

### 5. **Modern Build System**
- Updated `CMakeLists.txt` with modern CMake practices
- Added proper installation rules for libraries and executables
- Included Python module integration in build process
- Better compiler warning configurations

### 6. **Error Handling and Validation**
- Improved error messages and validation
- Configuration value validation with helpful warnings
- Better handling of edge cases and invalid inputs
- Enhanced logging for debugging and monitoring

### 7. **Code Quality Improvements**
- Reduced code duplication in sensory processing
- Better modularization of functionality
- Improved maintainability across the entire codebase
- Enhanced documentation and comments

## Dependencies

### Required Components

- **C++ Compiler**: C++20 compliant (GCC 10+, Clang 12+, or MSVC 2019+)
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **scikit-build-core**: Version 0.5.0+ (for Python bindings)

### Optional Components

- **Python Libraries**: pytest, numpy (for testing and data analysis)
- **Visualization Tools**: GLFW, SDL2 (for future visualization features)
- **Documentation Tools**: Doxygen, Sphinx (for API documentation)

## Installation

### From Source (Recommended)

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python module
pip install .

# Or install in development mode
pip install -e .
```

### C++ Only Installation

```bash
# Standard CMake build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install to system or local prefix
make install
```

## Usage Examples

### C++ Example

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("simulation_timestep", 0.001);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Log results
    brain->logStatus();
    return 0;
}
```

### Python Example

```python
import pynlm

# Create and configure brain
config = pynlm.create_default_config()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.05)

brain = pynlm.create_brain(config)
brain.initialize()

# Enable advanced features
brain.get_agent().enable_reward_modulation(True)
brain.get_agent().enable_curiosity(True)
brain.get_agent().enable_development(True)

# Run simulation with statistics
for step in range(500):
    brain.step(step)
    if step % 50 == 0:
        firing = brain.get_firing_neuron_count()
        spikes = brain.get_total_spike_count()
        rate = brain.get_average_firing_rate()
        print(f"Step {step}: {firing} firing, {spikes} total spikes, {rate:.2f} Hz")
```

## Testing

Run the test suite to verify installation and functionality:

```bash
cd tests
./run_tests.sh  # or use your preferred test runner
```

Or run individual tests:

```bash
# Run C++ tests
ctest --output-on-failure

# Run Python tests
pytest tests/ -v
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration values are valid

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report

## License

MIT

## Authors

Research project — See docs for scientific background.

---

**Version**: 0.1.0
**Phase**: 6 - Final Integration
**Build**: Updated with modern CMake practices, Python bindings, and enhanced CLI interface
**Last Updated**: Current improvements include modular code structure, comprehensive Python bindings, and improved user experience.