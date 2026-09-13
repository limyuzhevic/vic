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

### Basic Build (Unix-like systems)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Advanced Build Options

The project supports multiple build configurations. Choose from these options:

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with tests enabled
cmake .. -DBUILD_TESTS=ON

# Build with Python bindings (enabled by default)
cmake .. -DBUILD_PYTHON_BINDINGS=ON

# Build shared libraries (instead of static)
cmake .. -DBUILD_SHARED_LIBS=ON

# Enable visualization support
cmake .. -DNLM_ENABLE_VISUALIZATION=ON

# Enable coverage reporting
cmake .. -DNLM_COVERAGE=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Multi-Configuration Build (CMake 3.19+)

For better control over build configurations:

```bash
mkdir build
cd build
cmake ..
# Then run:
cmake --build . --config Release
cmake --build . --config Debug
```

## Installation

### System Installation

```bash
# Configure build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=/usr/local

# Build and install
make -j4
make install
```

### Python Package Installation

The Python bindings are built using scikit-build:

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build
```

### Installation Verification

After installation, verify that NLM is working:

```bash
# Test C++ executable
/usr/local/bin/nlm

# Test Python bindings
python -c "import pynlm; print(pynlm.__version__)"
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

### Phase 2 Demo (Basic Neural Computation)
```bash
./nlm
```

### Phase 3 Demo (Experiment Framework)
```bash
./nlm_phase3_demo
```

### Phase 4 Demo (Advanced Neural Processing)
```bash
./nlm_phase4_demo
```

## Development Setup

### Prerequisites

- **Linux/macOS**: GCC 10+, Clang 12+, or MSVC 2019+
- **Windows**: Visual Studio 2019+ with C++ support
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pip**: Python package installer

### Docker Development Environment

Build a development container:

```bash
# Create and run container
cd /path/to/nlm
docker build -t nlm-dev .
docker run -it --rm -v $(pwd):/workspace/nlm nlm-dev
```

### Conda Development Environment

Create an isolated environment:

```bash
# Create conda environment
conda create -n nlm python=3.8
conda activate nlm

# Install Python dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build the project
mkdir build && cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python)
make -j4
```

### Development Build

For development builds with extra debugging features:

```bash
# Configure development build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DBUILD_TESTS=ON \
         -DBUILD_PYTHON_BINDINGS=ON \
         -DNLM_COVERAGE=ON

# Build with verbose output
make VERBOSE=1
```

## Troubleshooting

### Common Issues

**"CMake cannot find Python"**
```bash
pip install scikit-build-core pybind11
 cmake .. -DPython_EXECUTABLE=$(which python)
```

**"Compilation errors"**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
- Install build dependencies: `build-essential`, `cmake`, `libpython3-dev`

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Use debug build: `cmake .. -DCMAKE_BUILD_TYPE=Debug`

**"Library not found errors"**
- On Linux: `export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH`
- On macOS: `export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH`
- Install development packages for dependencies

### Platform-Specific Issues

**Windows**
```bash
# Using Visual Studio Generator
cmake .. -G "Visual Studio 16 2019" -A x64

# Build with MSBuild
msbuild ALL_BUILD.vcxproj
```

**macOS**
```bash
# Ensure Xcode Command Line Tools are installed
xcode-select --install

# Build with proper permissions
make install
```

**Linux**
```bash
# Install required packages (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential cmake libpython3-dev python3-pip python3-dev

# Install required packages (Fedora/CentOS)
sudo dnf install gcc gcc-c++ cmake python3-devel python3-pip
```

## Continuous Integration

### GitHub Actions Configuration

The project includes GitHub Actions workflows for CI/CD:

- **build.yml**: Build and test the C++ core
- **python.yml**: Test Python bindings
- **coverage.yml**: Generate coverage reports

To use GitHub Actions, create a `.github/workflows/` directory:

```bash
mkdir -p .github/workflows
cat > .github/workflows/build.yml << 'EOF'
name: Build

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        build_type: [Release, Debug]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up CMake
      uses: action/cache@v2
      with:
        path: ~/.cache/cmake
        key: ${{ runner.os }}-cmake-${{ hashFiles('CMakeLists.txt') }}
    
    - name: Build
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
        cmake --build . --config ${{ matrix.build_type }}
```

### Pre-commit Hooks

Set up pre-commit hooks for code quality:

```bash
# Install pre-commit
curl https://pre-commit.com/install.py | python
pre-commit install

# Install git hooks
pre-commit install-hooks
```

### Code Quality Checks

Run static analysis:

```bash
# Code linting
clang-format --style=LLVM src/**/*.cpp src/**/*.hpp

# Type checking (if available)
cmake --build . --target check-types

# Memory leak detection
valgrind --leak-check=full ./nlm
```

## Performance Tuning

### Compiler Optimizations

For maximum performance:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_OPTIMIZE_FOR_PERFORMANCE=ON \
         -DBUILD_SHARED_LIBS=ON
```

### Runtime Optimization

Environment variables for performance tuning:

- `NLM_MAX_THREADS`: Limit number of CPU threads
- `NLM_SIMULATION_STEPS`: Maximum simulation steps
- `NLM_PLACEMENT_THRESHOLD`: Memory allocation threshold

Example configuration:

```bash
export NLM_MAX_THREADS=4
export NLM_SIMULATION_STEPS=100000
```

## Testing

Run the full test suite:

```bash
# Build with tests
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
cmake --build . --target nlm_test

# Run tests
./nlm_test

# Or use CTest
ctest --output-on-failure
```

### Python Tests

```bash
# Install pytest and dependencies
pip install pytest pytest-cov

# Run Python tests
pytest tests/

# Generate coverage report
pytest --cov=pynlm --cov-report=html
```

## Performance Testing

Benchmark the performance of different components:

```bash
# Run benchmark suite
cd build
./benchmark

# Profile performance
gprof ./nlm gmon.out
```

## Development Guidelines

### Code Style

- Use C++20 features
- Follow Google C++ Style Guide
- Use consistent naming conventions
- Add comprehensive comments

### Documentation

- Keep README.md updated
- Document new functions and classes
- Add examples for new features
- Update version in CMakeLists.txt

### Contributing

1. Fork the repository
2. Create a feature branch
3. Commit with clear messages
4. Add tests for new functionality
5. Update documentation
6. Run all tests before merging

## Resources

- **Documentation**: See `docs/` directory for detailed information
- **Architecture**: `docs/ARCHITECTURE.md`
- **Science**: `docs/SCIENCE.md`
- **Examples**: `python/examples.py`
- **Python API**: `easy_usage.md` and `HOW_TO_USE.md`

## License

MIT

## Authors

Research project — See docs for scientific background.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
