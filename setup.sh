#!/usr/bin/env bash
# NLM Development Environment Setup Script

# This script sets up the development environment for NLM framework

echo "=== NLM Development Environment Setup ==="
echo
echo

# Check for required tools
check_command() {
    if ! command -v $1 &> /dev/null; then
        echo "Error: $1 is required but not installed."
        echo "Please install $1 and try again."
        exit 1
    fi
    echo "✓ $1 found"
}

# Check for essential tools
echo "Checking for required tools..."
check_command "g++"
check_command "cmake"
check_command "git"
check_command "make"

# Check for optional tools
echo
enecho "Checking for optional tools..."
if command -v ninja &> /dev/null; then
    echo "✓ Ninja build system found (recommended)"
else
    echo "⚠ Ninja not found (cmake will use make)"
fi

# Check for C++ compiler version
echo
echo "Checking C++ compiler..."
g++ --version | head -1 | grep -q "g++\|clang++" || {
    echo "Warning: Could not detect modern C++ compiler"
    echo "Please ensure you have C++17 support"
}

# Check for OpenMP support
echo
echo "Checking for OpenMP support..."
g++ -fopenmp --version &> /dev/null && echo "✓ OpenMP support found" || echo "⚠ OpenMP not detected (single-threaded mode)"

# Check for CUDA (optional)
echo
echo "Checking for CUDA support..."
if command -v nvcc &> /dev/null; then
    echo "✓ CUDA compiler found (GPU acceleration available)"
else
    echo "⚠ CUDA not found (CPU-only build)"
fi

# Set up build directory
echo
enecho "Setting up build directory..."
if [[ ! -d "build" ]]; then
    mkdir -p build
    echo "✓ Created build directory"
else
    echo "⚠ Build directory already exists"
fi

# Create directory structure for artifacts
echo
enecho "Creating artifact directories..."
mkdir -p build/logs
mkdir -p build/checkpoints
mkdir -p build/results
mkdir -p build/docs
mkdir -p examples
mkdir -p configs/examples
mkdir -p scripts

echo "✓ Created directory structure"

# Check for third-party dependencies (recommendations)
echo
enecho "Checking third-party dependencies..."

echo "Recommended dependencies:"
echo "  - Eigen3 (linear algebra): apt-get install libeigen3-dev"
echo "  - nlohmann/json (JSON parsing): apt-get install nlohmann-json3-dev"
echo "  - Boost (C++ libraries): apt-get install libboost-all-dev"
echo "  - OpenBLAS (BLAS/LAPACK): apt-get install libopenblas-dev"
echo "  - Python (for analysis scripts): apt-get install python3 python3-pip"
echo

echo "Optional dependencies:"
echo "  - CUDA toolkit (GPU acceleration)"
echo "  - MKL (Intel Math Kernel Library)"
echo "  - BLIS (BLAS-like library)"

# Generate CMake options documentation
echo
enecho "Creating CMake configuration guide..."
cat > build/CMAKE_GUIDE.md << 'EOF'
# CMake Configuration Guide for NLM

## Standard Configuration
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Configuration Options

### Core Options
- `USE_OPENMP=ON/OFF` - Enable OpenMP parallelization (default: ON)
- `USE_CUDA=ON/OFF` - Enable CUDA GPU acceleration (default: OFF)
- `USE_MPI=ON/OFF` - Enable MPI distributed computing (default: OFF)
- `BUILD_TESTS=ON/OFF` - Build unit tests (default: OFF)
- `BUILD_EXAMPLES=ON/OFF` - Build example programs (default: ON)
- `BUILD_DOCUMENTATION=ON/OFF` - Build documentation (default: OFF)

### Performance Options
- `SIMD_LEVEL=NONE/SSSE3/SSE4.1/SSE4.2/AVX/AVX2/AVX512` - SIMD optimization level
- `OPTIMIZATION_LEVEL=Debug/Release/RelWithDebInfo/MinSizeRel` - Build type
- `CMAKE_BUILD_TYPE` - Alias for OPTIMIZATION_LEVEL

### Feature Options
- `ENABLE_VISUALIZATION=ON/OFF` - Enable visualization features (default: OFF)
- `ENABLE_PROFILING=ON/OFF` - Enable performance profiling (default: OFF)
- `ENABLE_MONITORING=ON/OFF` - Enable real-time monitoring (default: OFF)
- `ENABLE_PLUGINS=ON/OFF` - Enable plugin architecture (default: OFF)

## Example Configurations

### Development Build
```bash
mkdir build-dev && cd build-dev
cmake -DUSE_OPENMP=ON -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Production Build
```bash
mkdir build-prod && cd build-prod
cmake -DUSE_OPENMP=ON -DUSE_CUDA=OFF -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### GPU-Accelerated Build
```bash
mkdir build-gpu && cd build-gpu
cmake -DUSE_OPENMP=ON -DUSE_CUDA=ON -DSIMD_LEVEL=AVX512 ..
make -j$(nproc)
```

## Building with External Libraries

### With Eigen3
```bash
mkdir build && cd build
cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON ..
```

### With nlohmann/json
```bash
# Check if it's installed on your system
cmake -DBUILD_WITH_JSON=ON ..
```

### With Boost
```bash
mkdir build && cd build
cmake -DBUILD_WITH_BOOST=ON ..
```

## Running Tests

### Unit Tests
```bash
# Build with tests
mkdir build-tests && cd build-tests
cmake -DBUILD_TESTS=ON ..
make -j$(nproc)

# Run all tests
./nlm_tests

# Or run specific test suites
ctest --output-on-failure
```

### Integration Tests
```bash
# Run integration tests (if enabled)
./nlm --experiment basic-connectivity --steps 100
./nlm --experiment plasticity-learning --steps 500
```

## Performance Testing

### Benchmarks
```bash
# Run built-in benchmarks
./nlm --run-benchmarks

# Export benchmark results
./nlm --run-benchmarks --export-results ./benchmark_results
```

### Profiling
```bash
# Build with profiling
mkdir build-profile && cd build-profile
cmake -DENABLE_PROFILING=ON ..
make -j$(nproc)

# Run with profiling
./nlm --experiment plasticity-learning --steps 1000

# Generate performance reports
./profile_analyzer generate-reports
```

## Installation

### System Installation
```bash
# Build
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make

# Install (requires sudo)
sudo make install
```

### Development Installation
```bash
# Build with development options
mkdir build-dev && cd build-dev
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make install

# Install without sudo (for development)
make install DESTDIR=./install
```

## Troubleshooting

### Common Issues

#### OpenMP not found
```bash
# Install OpenMP
# Ubuntu/Debian: sudo apt-get install libomp-dev
# CentOS/RHEL: sudo yum install omp
# macOS: brew install libomp

# Rebuild
make clean && make -j$(nproc)
```

#### CUDA not found
```bash
# Install CUDA toolkit
# See: https://developer.nvidia.com/cuda-toolkit-archive

# Set CUDA paths
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

# Rebuild with CUDA
make clean && make -j$(nproc)
```

#### Build errors
```bash
# Check compiler and dependencies
make VERBOSE=1

# Or check CMake output
make -j$(nproc) 2>&1 | grep -i "error\|warning"
```

## Advanced Configuration

### Custom CMake Options
```bash
mkdir custom-build && cd custom-build
cmake \
    -DUSE_OPENMP=ON \
    -DUSE_CUDA=OFF \
    -DBUILD_TESTS=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native" \
    ..
```

### Cross-compilation
```bash
mkdir cross-build && cd cross-build
cmake \
    -DCMAKE_TOOLCHAIN_FILE=~/toolchains/arm-linux-gnueabihf.toolchain.cmake \
    -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
    ..
```

### Distcc for distributed compilation
```bash
# Install distcc on multiple machines
# Configure and rebuild with distcc support
```

## Clean Build
```bash
# Clean everything
make clean
rm -rf build/*

# Or remove build directory entirely
rm -rf build
```

## Build Statistics
After building, you can check build statistics:
```bash
# Show disk usage
cd build && du -sh .

# Show cache configuration
cd build && cpack --verbose
```
EOF

echo "✓ Created CMake guide"

# Create basic C++ project structure for contributions
echo
echo "Creating contribution guide..."
cat > CONTRIBUTING.md << 'EOF'
# Contributing to NLM

## Getting Started

### First Contributions
1. **Find an issue**: Browse GitHub issues and look for "good first issue" tags
2. **Set up development environment**: Follow the setup script above
3. **Fork and clone**: Fork the repository and clone locally
4. **Make changes**: Implement your changes following the code style
5. **Test**: Ensure all tests pass
6. **Create pull request**: Submit your changes

### Development Workflow
```bash
# Create feature branch
 feature/awesome-new-feature

# Make changes
# ... your code changes ...

# Test changes
make test

# Commit changes
# ... commit with proper message ...

# Push to branch
git push origin feature/awesome-new-feature

# Create pull request
# ... via GitHub interface ...
```

## Code Style

### C++ Style Guide
- **Indentation**: 4 spaces, no tabs
- **Line length**: Maximum 100 characters
- **Naming**: camelCase for variables/methods, PascalCase for classes
- **Comments**: Use Doxygen for documentation
- **Error handling**: Proper exception handling

### CMake Style Guide
- **Naming**: Upper case with underscores
- **Structure**: Flat structure with dependencies
- **Versions**: Semantic versioning

## Testing

### Unit Tests
- **Framework**: Catch2 or Google Test
- **Coverage**: Aim for >80% code coverage
- **Continuous integration**: Run on every commit

### Integration Tests
- **End-to-end testing**: Test complete workflows
- **Performance testing**: Verify performance characteristics
- **Regression testing**: Ensure no functionality is broken

### Test Commands
```bash
# Build tests
mkdir build-tests && cd build-tests
cmake -DBUILD_TESTS=ON ..
make -j$(nproc)

# Run all tests
./nlm_tests

# Run specific test suite
ctest --output-on-failure

# Run performance tests
./nlm --run-benchmarks
```

## Documentation

### Documentation Style
- **Format**: Markdown with proper headers
- **Examples**: Include working code examples
- **API documentation**: Generate from source comments

### Documentation Commands
```bash
# Build documentation
mkdir build-docs && cd build-docs
cmake -DBUILD_DOCUMENTATION=ON ..
make documentation

# View documentation
# ./docs/index.html (if generated)
```

## Performance Considerations

### Memory Management
- Use smart pointers (unique_ptr, shared_ptr)
- Avoid memory leaks
- Profile memory usage for large networks

### CPU Optimization
- Use SIMD instructions where appropriate
- Consider OpenMP for parallel processing
- Optimize hot paths

### Network Scaling
- **Small networks** (<100 neurons): No special optimization needed
- **Medium networks** (100-1000 neurons): Basic optimizations
- **Large networks** (>1000 neurons): Advanced optimizations required

## Debugging

### Debug Mode
```bash
# Enable debug logging
./nlm --experiment plasticity-learning --debug --steps 100

# Enable memory debugging
./nlm --experiment plasticity-learning --trace-memory-usage

# Enable performance profiling
./nlm --experiment plasticity-learning --profile
```

### Common Debugging Commands
```bash
# Check build configuration
make help

# Rebuild with different options
make clean && cmake -DNEW_OPTION=ON ..

# Run with valgrind
valgrind --leak-check=full ./nlm
```

## Version Control

### Commit Message Guidelines
- **Subject line**: 50 characters maximum, imperative mood
- **Body**: Explain what and why, not how
- **Co-authored-by**: Add when working with others

Example:
```
Fix neural dynamics instability

The LIF neuron dynamics had a numerical stability issue
when membrane potential exceeded bounds. Added clamping
and improved convergence.

Co-authored-by: OpenHands <openhands@all-hands.dev>
```

## License

All contributions must be under the MIT license.

## Acknowledgments

### Contributors
- Please add your name here when making your first contribution!

### Third-party Libraries
- All third-party libraries must be properly licensed
- All dependencies must be documented

## Questions and Support

### Getting Help
- **GitHub Issues**: Report bugs and ask questions
- **Discord/Slack**: Chat with other developers
- **Documentation**: Check the docs directory

### Getting Involved
- **Code reviews**: Review pull requests
- **Testing**: Test new features
- **Documentation**: Improve documentation
- **Community**: Help new contributors

## Code Quality Checklist

Before submitting changes:
- [ ] Code passes all tests
- [ ] No linting errors
- [ ] Code coverage maintained
- [ ] Documentation updated
- [ ] Examples added/updated
- [ ] Commit message follows guidelines
- [ ] Branch name is descriptive
- [ ] Pull request title and description are clear

## Merging Process

### Reviewing Pull Requests
1. **Review code**: Check for quality, style, and correctness
2. **Run tests**: Ensure all tests pass
3. **Check documentation**: Verify documentation is updated
4. **Ask questions**: Request clarification if needed
5. **Merge or comment**: Merge if ready, or provide feedback

### Merge Conflicts
- Resolve conflicts promptly
- Prioritize main branch stability
- Document conflict resolution

## Future Development

### Research Areas
- Neuromorphic computing
- Cognitive architectures
- Neuro-inspired learning
- Brain-inspired AI

### Technology Trends
- Event-driven simulation
- Multi-scale modeling
- GPU acceleration
- Distributed computing

### Community Growth
- Expand user base
- Improve documentation
- Increase contributions
- Foster mentorship

---

For questions about contributing, please contact the project maintainers.
EOF

echo "✓ Created contribution guide"

# Create basic scripting examples
echo
echo "Creating scripting examples..."
mkdir -p scripts/examples

cat > scripts/examples/basic_simulation.sh << 'EOF'
#!/bin/bash
# Basic NLM Simulation Example
# This script shows how to run a basic NLM simulation

echo "=== Basic NLM Simulation Example ==="
echo

# Check if NLM exists
if [[ ! -f "./nlm" ]]; then
    echo "Error: NLM binary not found"
    echo "Please build the project first"
    exit 1
fi

# Create output directory
mkdir -p results

# Run basic simulation
echo "Running basic simulation..."
./nlm --steps 500 --neurons 100 --regions 1 --seed 42 --output results

echo
echo "Simulation complete! Results in: results/"
EOF

cat > scripts/examples/plasticity_experiment.sh << 'EOF'
#!/bin/bash
# Plasticity Learning Experiment Example
# This script demonstrates plasticity learning in NLM

echo "=== Plasticity Learning Experiment ==="
echo

if [[ ! -f "./nlm" ]]; then
    echo "Error: NLM binary not found"
    exit 1
fi

mkdir -p results/plasticity

echo "Running plasticity learning experiment..."
echo "This will demonstrate STDP and Hebbian learning mechanisms"
echo

./nlm --experiment plasticity-learning \
     --steps 2000 \
     --neurons 500 \
     --regions 2 \
     --seed 123 \
     --output results/plasticity \
     --plasticity stdp \
     --plasticity-rate 0.8

echo
echo "Plasticity experiment complete!"
echo "Results in: results/plasticity/"
echo "Check the exported brain_state.checkpoint for learned weights"
EOF

cat > scripts/examples/memory_experiment.sh << 'EOF'
#!/bin/bash
# Memory Formation Experiment Example
# This script demonstrates memory systems in NLM

echo "=== Memory Formation Experiment ==="
echo

if [[ ! -f "./nlm" ]]; then
    echo "Error: NLM binary not found"
    exit 1
fi

mkdir -p results/memory

echo "Running memory formation experiment..."
echo "This will demonstrate working memory and episodic memory"
echo

./nlm --experiment memory-formation \
     --steps 3000 \
     --neurons 1000 \
     --regions 3 \
     --seed 456 \
     --output results/memory \
     --working-memory-capacity 200 \
     --episodic-memory-capacity 2000

echo
echo "Memory experiment complete!"
echo "Results in: results/memory/"
echo "Check memory traces and episodic episodes in exported results"
EOF

# Make scripts executable
chmod +x scripts/examples/*.sh

# Create configuration examples
echo
echo "Creating configuration examples..."

cat > configs/examples/basic_simulation.cfg << 'EOF'
# Basic Simulation Configuration
# Used for initial testing and validation

neuron_count = 100
region_count = 1
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001
max_steps = 500

# Neuron parameters
neuron_model = "lif"
threshold = -55.0
resting_potential = -70.0
refractory_period = 5

# Plasticity (disabled for basic test)
plasticity_mode = "none"

# Output
output_frequency = 50
log_level = "info"
EOF

cat > configs/examples/learning_simulation.cfg << 'EOF'
# Learning Simulation Configuration
# Used for plasticity and learning experiments

neuron_count = 500
region_count = 2
connection_probability = 0.15
random_seed = 123
simulation_timestep = 0.001
max_steps = 2000

# Neuron parameters
neuron_model = "lif"
threshold = -55.0
resting_potential = -70.0
refractory_period = 10

# Plasticity (enabled for learning)
plasticity_mode = "both"
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
hebbian_learning_rate = 0.01

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory systems
working_memory_capacity = 100
e episodic_memory_capacity = 500
replay_interval = 100

# Output
output_frequency = 100
log_level = "info"
EOF

cat > configs/examples/large_scale.cfg << 'EOF'
# Large Scale Network Configuration
# Used for testing scalability and performance

neuron_count = 10000
region_count = 5
connection_probability = 0.05
random_seed = 789
simulation_timestep = 0.002
max_steps = 1000

# Neuron parameters
neuron_model = "lif"
threshold = -50.0
resting_potential = -70.0
refractory_period = 8

# Plasticity (balanced)
plasticity_mode = "stdp"
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Structural plasticity (minimal)
synaptogenesis_rate = 0.00001
pruning_rate = 0.000005

# Memory systems (scaled)
working_memory_capacity = 500
e episodic_memory_capacity = 2000
replay_interval = 200

# Output (reduced frequency for performance)
output_frequency = 200
log_level = "warning"

# Performance optimization
enable_profiling = true
enable_monitoring = true
```
EOF

# Create example scripts for quick testing
echo
echo "Creating quick test scripts..."

cat > run-tests.sh << 'EOF'
#!/bin/bash
# Quick test script for NLM

echo "=== NLM Quick Test ==="
echo

if [[ ! -f "./nlm" ]]; then
    echo "Building NLM first..."
    mkdir -p build && cd build
    cmake ..
    make -j$(nproc)
    cd ..
fi

# Test 1: Basic connectivity

echo "Test 1: Basic Connectivity"
./nlm --experiment basic-connectivity --steps 100 --output ./test_results/basic

echo

# Test 2: Simple simulation
echo "Test 2: Simple Simulation"
./nlm --steps 50 --neurons 50 --output ./test_results/simple

echo

echo "All tests completed! Results in: ./test_results/"
EOF

chmod +x run-tests.sh

# Create a summary file
echo
echo "=== Development Environment Setup Complete ==="
echo
echo "Created files and directories:"
echo "  scripts/examples/              - Example scripts"
echo "  configs/examples/             - Configuration examples"
echo "  build/CMAKE_GUIDE.md          - CMake configuration guide"
echo "  CONTRIBUTING.md               - Contribution guidelines"
echo "  run-tests.sh                  - Quick test script"
echo
echo "Next steps:"
echo "  1. Build the project:"
echo "     mkdir build && cd build && cmake .. && make -j$(nproc)"
echo "  2. Run tests:"
echo "     ./run-tests.sh"
echo "  3. Try examples:"
echo "     ./scripts/examples/basic_simulation.sh"
echo "  4. Review configuration files:"
echo "     cat configs/examples/basic_simulation.cfg"
echo
echo "For help, see README.md or run './nlm --help'"
