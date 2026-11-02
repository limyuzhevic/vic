# NLM Development Guide

This document provides comprehensive guidance for setting up and working with the NLM (Neural Learning Machine) project.

## Prerequisites

### System Requirements

- **Operating System**: Linux or macOS (Windows requires WSL or equivalent)
- **Memory**: Minimum 8GB RAM, recommended 16GB+ for Phase 6 experiments
- **Storage**: Minimum 50GB free space
- **Compiler**: C++20 compatible compiler (g++ 11+, clang 14+)
- **Python**: Python 3.8+

### Essential Tools

```bash
# System dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    python3-pip \
    python3-dev \
    python3-venv \
    libopenblas-dev \
    liblapack-dev \
    pkg-config

# Additional Python packages for development
pip3 install scikit-build-core pybind11 pytest numpy matplotlib
```

## Initial Setup

### 1. Clone the Repository

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

### 2. Configure Development Environment

```bash
# Create and activate virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install development dependencies
pip install -e .[dev]

# Initialize pre-commit hooks if available
if [ -f .pre-commit-config.yaml ]; then
    pre-commit install
fi
```

### 3. Build the Project

#### Standard C++ Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20

# Build all targets
make -j4
```

#### Build with Python Bindings

```bash
# Build with Python bindings enabled (default)
mkdir -p build_python
cd build_python
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Build All Phases

```bash
# Build specific phase demo (example: Phase 6 integration)
mkdir -p build_phase6
cd build_phase6
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make nlm_phase6_demo
```

## Running Tests

### C++ Tests

```bash
cd build

# Run all C++ tests
./nlm_test_suite

# Run specific test (example: neuron tests)
./nlm_test_suite --neuron

# Run tests with verbose output
./nlm_test_suite -v
```

### Python Tests

```bash
# Activate virtual environment (if not already active)
source venv/bin/activate

# Run Python tests
pytest tests/ -v

# Run Python tests with coverage
pytest tests/ --cov=pynlm --cov-report=html

# Run specific test module
pytest tests/test_bindings.py -v
```

### Integration Testing

```bash
# Run Phase 6 integration demo (comprehensive test)
./build/nlm_phase6_demo

# Run benchmark
time ./build/nlm_benchmark

# Run Phase 5 performance tests
./build/nlm_phase5_demo
```

## Development Workflow

### Daily Workflow

1. **Morning:**
   ```bash
   git status
   git log --oneline -5
   ```

2. **During development:**
   ```bash
   # Create feature branch
   git checkout -b feature/your-feature-name
   
   # Make changes
   # Write tests
   # Test thoroughly
   
   # Commit changes
   git add .
   git commit -m "Add feature description (#issue-number)"
   ```

3. **End of day:**
   ```bash
   git push origin feature/your-feature-name
   # Create draft PR for code review
   ```

### Key Development Commands

| Command | Purpose |
|---------|---------|
| `git status` | Check current changes |
| `git log --oneline -10` | View recent commits |
| `git diff HEAD~1` | View last commit changes |
| `make -j4` | Rebuild after changes |
| `./build/nlm_test_suite` | Run all C++ tests |
| `pytest tests/ -v` | Run all Python tests |
| `cd build && cmake .. -DDEBUG=ON` | Build with debug flags |

## Code Exploration

### Core Components

#### Brain Components

```bash
# Navigate to core brain modules
src/brain/
├── Brain.cpp - Main brain orchestration
├── Neuron.cpp - LIF neuron model
├── Synapse.cpp - Synaptic transmission
└── NeuralPopulation.cpp - Neuron population management
```

#### Memory Systems

```bash
# Memory components in src/memory/
src/memory/
├── Memory.cpp - Base memory interface
├── NeuralWorkingMemory.cpp - Working memory implementation
└── NeuralEpisodicMemory.cpp - Episodic memory
```

#### Neuromodulation

```bash
# Neuromodulators in src/neuromodulation/
src/neuromodulation/
├── Neuromodulator.cpp - Base neuromodulator
├── Reward.cpp - Dopamine reward signaling
├── Curiosity.cpp - Exploration drive
└── Novelty.cpp - Surprise detection
```

### Configuration

#### Core Configuration

```bash
# Configuration files in configs/
ls -la configs/

# Modify core parameters
cat configs/core_config.json
```

#### Environment Setup

```bash
# Environment configurations
src/environment/
├── Environment.cpp - Main environment interface
└── Observation.cpp - Sensor data structure
```

## Debugging and Troubleshooting

### Common Issues

#### 1. Build Failures

```bash
# Check for common build issues
build/CMakeFiles/*.log

# Rebuild with verbose output
cd build
make VERBOSE=1
```

#### 2. Test Failures

```bash
# Run with detailed output
cd build
./nlm_test_suite --debug

# Check specific test modules
./nlm_test_suite --neuron --synapse
```

#### 3. Python Binding Issues

```bash
# Test Python bindings
python -c "import pynlm; print(pynlm.__version__)"

# Check binding errors
python -c "from pynlm import *; print('All imports successful')"
```

### Debugging Techniques

#### GDB Debugging

```bash
# Start debugging
cd build
gdb ./nlm_phase6_demo

# Set breakpoints
set breakpoint pending on
break Brain::advance
run

# Step through code
next
continue
```

#### Valgrind Memory Debugging

```bash
# Run with Valgrind for memory leaks
cd build
valgrind --leak-check=full --track-origins=yes ./nlm_test_suite
```

## Advanced Development

### Performance Profiling

#### C++ Profiling

```bash
# Use gprof
gcd build
cd build
make profiled
./nlm_benchmark

gprof nlm_benchmark gmon.out > profile.txt
```

#### Python Profiling

```bash
# Profile Python code
python -m cProfile -o profile.stats tests/test_bindings.py

# Visualize with snakeviz
pip install snakeviz
snakeviz profile.stats
```

### Running Experiments

#### Phase 6 Integration Experiment

```bash
# Run the full integration demo
cd build
./nlm_phase6_demo

# Run with custom configuration
./nlm_phase6_demo --brain-size=1000 --simulation-steps=5000
```

#### Phase-Specific Demos

```bash
# Run specific phase demos
./build/nlm_phase3_demo      # Phase 3: Basic interaction
./build/nlm_phase4_demo      # Phase 4: Cognitive integration
./build/nlm_phase5_demo      # Phase 5: Performance optimized
./build/nlm_benchmark        # Performance benchmark
```

### Contributing to Documentation

#### Documentation Structure

```
docs/
├── ARCHITECTURE.md      # System architecture
├── SCIENCE.md           # Scientific background
├── ROADMAP.md           # Development roadmap
├── EXPERIMENTS.md       # Experiment descriptions
├── LIMITATIONS.md       # Known limitations
└── PHASE6_FINAL_AUDIT.md # Phase 6 audit report
```

#### Writing Documentation

```bash
# Edit documentation using Markdown
nano docs/ARCHITECTURE.md

# Build documentation (if using Sphinx)
sphinx-build -b html docs/ docs/_build/html
```

## Environment Management

### Virtual Environments

#### Development vs Production

```bash
# Development environment (with debug symbols)
mkdir build_dev
cd build_dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_BUILD_PYTHON_BINDINGS=ON

# Production environment (optimized)
mkdir build_prod
cd build_prod
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
```

### Containerized Development

```bash
# Docker development setup
# Check for Dockerfile in project root
docker build -t nlm-dev .

# Run development container
docker run -it --rm -v $(pwd):/workspace/nlm nlm-dev
```

## Community Guidelines

### Communication

- Use GitHub Issues for bug reports and feature requests
- Ask questions in GitHub Discussions
- Use appropriate labels when creating issues

### Code Review Process

1. Ensure all tests pass
2. Follow code style guidelines
3. Update documentation as needed
4. Request peer review for significant changes

## Project Maintenance

### Version Management

```bash
# Check current version
cat CMakeLists.txt | grep "project(NLM VERSION"

# Update version in CMakeLists.txt
# Update version in pyproject.toml
```

### Release Process

```bash
# Create and push release tag
git tag v0.1.0
git push origin v0.1.0

# Build release artifacts
./scripts/build_release.sh
```

## Troubleshooting Table

| Symptom | Cause | Solution |
|---------|-------|----------|
| Build fails | Missing dependencies | Install required packages |  
| Test crashes | Memory corruption | Run with valgrind |
| Python import fails | Bindings not built | Run 'make pynlm' |
| Performance issues | Inefficient algorithms | Profile with gprof/snakeviz |

## Additional Resources

### Online Documentation

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [NLM Discord Server](https://discord.gg/nlm)

### Research Papers

- "The NLM Architecture: An Experimental Artificial Developmental Brain"
- "Phase 6 Integration: Unifying Cognitive and Neural Systems"

### Related Projects

- [PyTorch Neural Networks](https://pytorch.org)
- [Brain-Compatible AI](https://brain-compatible.ai)

---

**Last Updated:** 2026-10-31
**Version:** 0.1.0
**Maintainers:** NLM Development Team

## Quick Start Summary

```bash
# 1. Setup
python -m venv venv && source venv/bin/activate
pip install -e .[dev]

# 2. Build
mkdir build && cd build
cmake .. && make -j4

# 3. Test
./nlm_test_suite
pytest tests/

# 4. Develop
# Make changes, write tests, commit
```

> "The brain is not something to be found, but something to be created." - NLM Philosophy

---

*This guide is a living document. Contributions to improve it are welcome!*

## Getting Help

If you encounter issues or have questions:

1. **Check existing documentation:** This file, README.md, and the docs/
2. **Search GitHub Issues:** Look for similar issues or questions
3. **Create an Issue:** Provide detailed reproduction steps
4. **Contact maintainers:** Through GitHub or project communication channels

## Development Best Practices

### Code Review Checklist

- [ ] Tests added or updated
- [ ] Documentation updated
- [ ] Code follows style guidelines
- [ ] Commit messages are descriptive
- [ ] Branch name follows naming convention
- [ ] Changes don't break existing functionality

### Testing Best Practices

- **Unit tests:** Test individual components in isolation
- **Integration tests:** Test system components together
- **Performance tests:** Validate performance characteristics
- **Regression tests:** Prevent reintroduction of bugs
- **Edge case tests:** Test boundary conditions and error handling

### Performance Considerations

NLM is performance-sensitive:

- Use `-DNDEBUG` for release builds
- Profile before optimizing
- Use SIMD optimizations where appropriate
- Consider memory access patterns
- Test on target hardware

---

**Need help?** Check our [CONTRIBUTING.md](CONTRIBUTING.md) for additional guidance on the contribution process.

**Ready to contribute?** Fork, branch, and submit a pull request!

---

*This document is maintained by the NLM development team and will be updated regularly.*

## Emergency Procedures

### Project Halted

If the project encounters critical issues:

1. **Immediate Actions:**
   ```bash
   git log --oneline --graph
   git branch -v
   git status
   ```

2. **Assessment:**
   - Check recent commits for problematic changes
   - Verify test suite status
   - Review build logs

3. **Recovery:**
   - Revert problematic changes if necessary
   - Restore from backup if available
   - Coordinate with maintainers

### Data Recovery

For corrupted data or configurations:

1. Check version control history
2. Restore from git
3. Rebuild from source
4. Use checkpointing features (Phase 6+)

---

**Remember:** NLM is a research project. Be patient, thorough, and scientific in your approach.

---

## Migration Guide (for upgrading between versions)

### v0.1.0 to v0.2.0 (Planned)

1. New memory management system
2. Enhanced Python bindings
3. Additional cognitive architectures
4. Improved visualization tools

**Breaking Changes:**
- API changes in core modules
- Updated configuration format
- New test framework

**Migration Steps:**
```bash
# Update CMakeLists.txt
cat CMakeLists.txt | grep "VERSION"

# Check for deprecated APIs in CHANGELOG.md
```

---

**End of Development Guide**

*NLM: Building the future of brain-inspired artificial intelligence, one synapse at a time.*

## Version Control Information

Current working directory: `/workspace/app`

Git status:
```bash
git status
git log --oneline -3
git branch -a
```

This guide will be kept current with project evolution. Contributions to improve this documentation are welcome!