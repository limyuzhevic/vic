# NLM (Neural Learning Machine) - Contributing Guide

This document provides guidelines for contributing to the NLM (Neural Learning Machine) project. We welcome contributions from everyone, regardless of experience level.

## Getting Started

### Prerequisites

Before contributing, please ensure you have:

- **Git**: Version control system
- **C++ compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Development tools**: Text editor or IDE

### Setting Up the Environment

1. **Clone the repository**

   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. **Create and activate a virtual environment** (for Python development)

   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

3. **Install development dependencies**

   ```bash
   pip install -e .[dev]
   ```

4. **Build the project**

   ```bash
   mkdir -p build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make -j$(nproc)
   ```

## Understanding the Project

### Project Structure

```
NLM/
├── src/              # Source code (C++ and Python bindings)
│   ├── core/        # Core utilities and types
│   ├── brain/       # Neural processing components
│   ├── memory/      # Memory systems
│   ├── neuromodulation/ # Neuromodulation systems
│   ├── cognition/   # Cognitive mechanisms
│   ├── sensory/     # Sensory processing
│   ├── motor/       # Motor system
│   ├── environment/ # Environment interface
│   └── visualization/ # Visualization
│
├── python/           # Python bindings
│   ├── bindings.cpp # C++/pybind11 bindings
│   └── CMakeLists.txt # Python bindings build
│
├── tests/           # Test suite
│   ├── unit/       # Unit tests
│   └── integration/ # Integration tests
│
├── docs/            # Documentation
├── configs/         # Configuration files
└── examples/        # Example scripts
```

### Development Phases

NLM development follows a phased approach:

1. **Phase 1**: Core components and utilities
2. **Phase 2**: Basic learning mechanisms
3. **Phase 3**: World interaction and environment
4. **Phase 4**: Advanced cognition and planning
5. **Phase 5**: Performance optimizations
6. **Phase 6**: Final integration (current phase)

Each phase adds new capabilities while maintaining backward compatibility.

## Contributing Code

### Coding Standards

#### File Structure

- **Header files (.hpp)**: Declaration of classes, functions, and types
- **Source files (.cpp)**: Implementation of class methods and functions
- **Documentation**: Comments in Doxygen format for C++, docstrings for Python

#### Naming Conventions

| Component | Convention | Example |
|-----------|------------|---------|
| Classes | PascalCase | `NeuralWorkingMemory` |
| Functions | camelCase | `storeEpisodicMemory()` |
| Constants | UPPER_SNAKE_CASE | `MAX_NEURONS = 10000` |
| Types | CamelCase | `NeuronId`, `SimulationStep` |
| Variables | snake_case | `simulation_step`, `neuron_id` |

#### Code Style

- **Indentation**: 4 spaces (no tabs)
- **Line length**: Maximum 120 characters for source files, 100 for documentation
- **Brackets**: One per line with K&R style
- **Whitespace**: Use meaningful whitespace for readability
- **Comments**: Doxygen format for C++:

  ```cpp
  /**
   * @class NeuralWorkingMemory
   * @brief Neural-enhanced working memory system
   *
   * @details This class provides neural integration with working memory,
   *          implementing both maintenance connections and competition dynamics.
   *
   * @note Phase 6: Enhanced neural working memory with attentional selection
   */
  ```

### Pull Request Process

#### Step 1: Fork and Clone

1. Fork the repository on GitHub
2. Clone your fork locally
3. Create a feature branch

   ```bash
   git checkout -b feature/your-name/feature-name
   ```

#### Step 2: Make Your Changes

1. **Fix bugs**: If fixing a bug, create a test case first
2. **Add features**: Ensure backward compatibility
3. **Update documentation**: Add or update relevant documentation
4. **Add tests**: Write tests for new functionality

#### Step 3: Commit Your Changes

```bash
# Stage your changes
git add path/to/file.cpp

# Commit with descriptive message
git commit -m "Add feature: [brief description]

Co-authored-by: Your Name <your.email@example.com>"
```

#### Step 4: Push and Create Pull Request

```bash
git push origin feature/your-name/feature-name

# Create pull request on GitHub
```

#### Step 5: Respond to Reviews

- Address feedback from reviewers
- Make necessary changes
- Request new reviews if needed

### Branch Management

#### Feature Branches

- **Naming**: `feature/<username>/<short-descriptive-name>`
- **Duration**: Features should be developed in branches for 1-2 weeks
- **Merging**: Merge into `main` branch when ready

#### Hotfix Branches

- **Naming**: `hotfix/<issue-number>/<short-description>`
- **Purpose**: Emergency fixes for production issues
- **Merging**: Merge immediately into `main` branch

#### Release Branches

- **Naming**: `release/<version-number>`
- **Purpose**: Prepare for new releases
- **Duration**: Usually 1-2 weeks

## Testing

### Running Tests

#### Unit Tests

```bash
# Run unit tests
./build/nlm_test

# Or use CTest
ctest --output-on-failure
```

#### Python Tests

```bash
# Run Python tests
pytest tests/

# Or run with verbose output
pytest tests/ -v
```

### Writing Tests

#### Unit Test Guidelines

1. **Test one thing at a time**: Each test should focus on a single functionality
2. **Use descriptive names**: Test names should clearly indicate what is being tested
3. **Test edge cases**: Include boundary conditions and error cases
4. **Use fixtures**: Create setup/teardown fixtures for common test scenarios

#### Test Structure Example

```cpp
#include <gtest/gtest.h>
#include "Brain.hpp"
#include "Config.hpp"

class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = Config::createDefault();
        brain = std::make_unique<Brain>(config);
        brain->initialize();
    }

    void TearDown() override {
        brain.reset();
    }

    Config config;
    std::unique_ptr<Brain> brain;
};

TEST_F(BrainTest, InitializationSetsNeuronCount) {
    EXPECT_GT(brain->getTotalNeuronCount(), 0);
    EXPECT_EQ(brain->getFiringNeuronCount(), 0);
}

TEST_F(BrainTest, StepUpdatesBrainState) {
    brain->step(0);
    // Add specific assertions based on expected behavior
}
```

#### Python Test Guidelines

```python
def test_brain_initialization():
    """Test that brain initializes with correct neuron count."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    assert brain.getFiringNeuronCount() == 0
    
def test_brain_step():
    """Test that brain step updates brain state."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(0)
    
    # Add specific assertions based on expected behavior
    assert brain.getTotalSpikeCount() >= initial_spikes
```

### Testing Coverage

We aim for high test coverage:

- **Core functionality**: 95%+ coverage
- **Python bindings**: 90%+ coverage
- **Edge cases**: Full coverage
- **Error handling**: 100% coverage

Use tools like `gcov` and `coverage.py` to measure test coverage.

## Documentation

### Writing Documentation

#### Code Documentation

- **C++**: Use Doxygen format in header files
- **Python**: Use Google or NumPy style docstrings

Example docstring:

```cpp
/**
 * @brief Store an episodic memory in the memory system.
 *
 * @param episode The episodic memory item to store.
 * @param timestamp The timestamp when the episode occurred.
 *
 * @details This method handles storage of episodic memories, including
 *          capacity management and age tracking.
 *
 * @note Memory consolidation happens during off-peak periods.
 */
void storeEpisode(const EpisodicMemoryItem& episode, SimulationStep timestamp);
```

Python example:

```python
"""Store an episodic memory in the memory system.

Args:
    episode: The episodic memory item to store.
    timestamp: The timestamp when the episode occurred.

Returns:
    bool: True if the episode was stored successfully, False otherwise.

Raises:
    ValueError: If the episode is invalid.

Note:
    Memory consolidation happens during off-peak periods.
"""
def storeEpisodicMemory(episode, timestamp):
    pass
```

#### Markdown Documentation

- Use the same formatting throughout
- Include code examples in code blocks
- Use tables for comparing options or results
- Include diagrams where helpful

### Updating Documentation

Always update documentation when:

- Adding new public APIs
- Changing existing APIs
- Fixing bugs that affect user behavior
- Adding new features

## Code Reviews

### Review Process

1. **Initial Review**: Automated checks (linting, formatting, tests)
2. **Peer Review**: Code review by team members
3. **Final Approval**: Merge after all requirements are met

### Review Checklist

#### Code Quality
- [ ] Code follows project style guide
- [ ] No syntax or compilation errors
- [ ] Performance implications are documented
- [ ] Memory management is correct

#### Testing
- [ ] All existing tests pass
- [ ] New tests cover new functionality
- [ ] Edge cases are tested
- [ ] Error handling is tested

#### Documentation
- [ ] Code is documented
- [ ] Examples are provided
- [ ] API documentation is updated
- [ ] README is updated if needed

#### Compatibility
- [ ] Backward compatibility is maintained
- [ ] Dependencies are properly specified
- [ ] Build configuration is updated

## Building and Testing Locally

### Prerequisites

```bash
# Install system dependencies
# Ubuntu/Debian:
apt-get update
apt-get install build-essential cmake git python3-venv python3-pip

# macOS:
brew install cmake git python3

# Windows:
# Use Chocolatey or manual installation
choco install cmake git python
```

### Building

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Running Python Tests

```bash
# Activate virtual environment
source venv/bin/activate

# Install development dependencies
pip install -e .[dev]

# Run all tests
pytest

# Run specific test module
pytest tests/unit/
pytest tests/integration/
```

### Performance Testing

For performance-critical components:

1. **Benchmarking**: Use `benchmark` framework for microbenchmarks
2. **Profiling**: Use `gprof` or `perf` for profiling
3. **Stress testing**: Test with maximum configurations

```bash
# Build with benchmarks
mkdir benchmark-build
cd benchmark-build
cmake .. -DBUILD_BENCHMARKS=ON
cmake --build . --target benchmark

# Run benchmarks
./benchmarks
```

## Contributing to Documentation

### Writing Documentation

1. **Markdown files**: Use `.md` extension
2. **Documentation structure**: Follow the existing structure in `docs/`
3. **Code examples**: Include working code examples
4. **Cross-references**: Use relative paths for linking

### Documentation Projects

We maintain the following documentation projects:

- **API Reference**: `API_REFERENCE.md` - Complete API documentation
- **User Guide**: `HOW_TO_USE.md` - Step-by-step usage instructions
- **Easy Usage**: `easy_usage.md` - Beginner-friendly guide
- **Architecture**: `docs/ARCHITECTURE.md` - System architecture
- **Science**: `docs/SCIENCE.md` - Scientific background
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment descriptions

## Security Considerations

### Vulnerability Disclosure

If you discover a security vulnerability:

1. **Do not disclose** publicly
2. **Contact** the project maintainers privately
3. **Provide** detailed information about the vulnerability
4. **Work** with maintainers to fix the issue

### Code Security

When contributing code:

- **Validate inputs**: Always validate user inputs
- **Use safe functions**: Prefer secure functions over unsafe ones
- **Check bounds**: Always check array and buffer bounds
- **Handle errors**: Properly handle and propagate errors
- **Use memory safety**: Use smart pointers and RAII

## Legal and License

### License Compliance

- **License**: MIT License
- **Copyright**: Project authors
- **Acknowledgments**: Due credit to all contributors

### Attribution

When using NLM in your project:

```markdown
## Acknowledgments

This project is based on the Neural Learning Machine (NLM) framework,
developed as an experimental artificial developmental brain project.

NLM is not:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics
```

## Getting Help

### Community Resources

1. **GitHub Issues**: Report bugs and request features
2. **Discussions**: Ask questions and discuss ideas
3. **Documentation**: Check the comprehensive documentation
4. **Examples**: Study the example scripts

### Project Resources

- **Website**: https://nlm.readthedocs.io
- **Repository**: https://github.com/nlm-project/nlm
- **Documentation**: See `docs/` directory
- **Examples**: See `examples/` directory

## Code of Conduct

We follow a code of conduct that promotes respectful collaboration:

- **Be inclusive**: Welcome contributors from all backgrounds
- **Be respectful**: Listen to others' perspectives
- **Be collaborative**: Work together to solve problems
- **Be professional**: Maintain professional standards in all interactions

## Thank You

Thank you for contributing to NLM! Your work helps advance the field of computational neuroscience and artificial developmental systems.

Every contribution, no matter how small, makes a difference in our mission to create brain-inspired artificial intelligence systems.
