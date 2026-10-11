# NLM Development Documentation

## Overview

This document provides comprehensive guidance for developing, testing, building, and contributing to the NLM (Neural Learning Machine) project. It covers coding standards, testing methodologies, build procedures, and contribution guidelines.

## Development Environment Setup

### Prerequisites

**Required Software:**
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: make, ninja, or equivalent
- **Package Manager**: git, pip (for Python dependencies)

**Optional Tools:**
- **ClangFormat**: For code formatting
- **ClangTidy**: For static analysis
- **Valgrind**: For memory debugging
- **Doxygen**: For documentation generation

### Setting Up the Development Environment

```bash
# Clone the repository
mkdir -p ~/dev/nlm
cd ~/dev/nlm
git clone https://github.com/your-repo/nlm.git

# Navigate to project directory
cd nlm

# Configure development build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_DEVELOPER_MODE=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_COVERAGE=ON \
    -DNLM_ENABLE_STATIC_ANALYSIS=ON

# Install development dependencies
pip install -r requirements-dev.txt

# Initialize git hooks (if any)
if [ -f .githooks/pre-commit ]; then
    git config core.hooksPath .githooks
fi
```

### IDE Configuration

#### Visual Studio Code

Create `.vscode/settings.json`:

```json
{
    "cmake.generator": "Ninja",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.cCompileFlags": ["-std=c++20", "-Wall", "-Wextra", "-Wpedantic"],
    "cmake.cppCompileFlags": ["-std=c++20", "-Wall", "-Wextra", "-Wpedantic", "-Werror"],
    "editor.formatOnSave": true,
    "editor.tabSize": 4,
    "files.insertFinalNewline": true,
    "search.exclude": {
        "build/": true,
        "**/*.pyc": true,
        "**/__pycache__/": true,
        "**/.git/": true
    },
    "C_Cpp.clangd.path": "clangd"
}
```

#### CLion

Create `cmake-toolchain.cmake` for custom toolchain:

```cmake
# CMake toolchain file for CLion
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Developer-friendly settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Optimization flags for development
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
    -Wformat-security
    -Wconversion
    -Wlogical-op
    -Wnull-dereference
    -Wdouble-promotion
    -Wdate-time
    -Wcast-align
)

# Performance flags
add_compile_options(
    -O2
    -DNDEBUG
    -march=native
    -fomit-frame-pointer
    -ftree-vectorize
    -fPIC
)
```

## Coding Standards and Conventions

### 1. C++ Coding Standards

#### File Naming

```cpp
// Headers
brain/Brain.hpp
brain/NeuralPopulation.hpp
memory/NeuralWorkingMemory.hpp
prediction/PredictionSystem.hpp

// Source files
brain/Brain.cpp
brain/NeuralPopulation.cpp
memory/NeuralWorkingMemory.cpp
prediction/PredictionSystem.cpp
```

#### Directory Structure

```
src/
├── core/              # Core utilities and infrastructure
│   ├── Config/       # Configuration system
│   ├── Logger/       # Logging system
│   ├── ErrorHandling/ # Error handling infrastructure
│   ├── Random/       # Random number generation
│   ├── SimulationClock/ # Time management
│   └── Types/        # Type definitions
│
├── brain/             # Neural components
│   ├── Brain.hpp/cpp  # Main brain coordinator
│   ├── NeuralPopulation.hpp/cpp # Neuron groups
│   ├── NeuralRegion.hpp/cpp   # Brain regions
│   ├── Neuron.hpp/cpp   # Individual neurons
│   └── Synapse.hpp/cpp  # Synaptic connections
│
├── memory/            # Memory systems (Phase 4)
│   ├── NeuralWorkingMemory.hpp/cpp
│   ├── NeuralEpisodicMemory.hpp/cpp
│   ├── NeuralAssociativeMemory.hpp/cpp
│   ├── ConceptFormation.hpp/cpp
│   └── SpatialRepresentation.hpp/cpp
│
├── plasticity/         # Plasticity rules
│   ├── STDP.hpp/cpp
│   ├── Hebbian.hpp/cpp
│   ├── StructuralPlasticity.hpp/cpp
│   └── SpikeSystem.hpp/cpp
│
├── neuromodulation/     # Neuromodulators
│   ├── Dopamine.hpp/cpp
│   ├── Curiosity.hpp/cpp
│   ├── Novelty.hpp/cpp
│   └── PredictionError.hpp/cpp
│
├── development/        # Developmental system
│   ├── DevelopmentSystem.hpp/cpp
│   ├── Synaptogenesis.hpp/cpp
│   ├── Pruning.hpp/cpp
│   └── Maturation.hpp/cpp
│
├── prediction/         # Prediction systems
│   ├── NeuralPrediction.hpp/cpp
│   ├── PredictionSystem.hpp/cpp
│   └── ActionConsequencePredictor.hpp/cpp
│
├── cognition/          # Cognitive systems
│   ├── AttentionalSelection.hpp/cpp
│   ├── NeuralPlanner.hpp/cpp
│   ├── SelfModel.hpp/cpp
│   └── SocialLearning.hpp/cpp
│
├── sensory/            # Sensory processing
│   ├── SensoryInput.hpp/cpp
│   ├── Vision.hpp/cpp
│   └── Audio.hpp/cpp
│
├── motor/              # Motor system
│   ├── Action.hpp/cpp
│   └── MotorSystem.hpp/cpp
│
├── environment/        # Environment interface
│   ├── Environment.hpp/cpp
│   ├── Observation.hpp/cpp
│   └── Action.hpp/cpp
│
├── experiments/        # Experiment framework
│   ├── Experiment.hpp/cpp
│   ├── Phase3Experiment.hpp/cpp
│   ├── Phase4Experiment.hpp/cpp
│   └── Phase6IntegratedExperiment.hpp/cpp
│
└── visualization/      # Visualization
    └── VisualizationInterface.hpp/cpp
```

#### Naming Conventions

**Variable Names:**
- snake_case for local variables
- camelCase for function parameters
- UPPER_CASE for constants and macros

**Function Names:**
- camelCase for public methods
- snake_case for private methods
- get_*, set_*, is_*, has_*, can_* for accessors

**Class Names:**
- PascalCase (CamelCase) for classes and structs
- Prefix with namespace (nlm::)

**File Names:**
- header files: ClassName.hpp
- source files: ClassName.cpp
- utility files: utility_name.cpp

#### Code Style

```cpp
// ✅ GOOD: Clear and readable
void processSensoryInput(const SensoryPercept& percept) {
    if (!percept.isValid()) {
        throw ValidationError("Invalid sensory percept", "processSensoryInput");
    }
    
    // Extract sensory features
    auto features = percept.extractFeatures();
    
    // Inject into neural system
    brain_->receiveSensoryInput(features);
}

// ❌ BAD: Cryptic and unreadable
void process(const SensoryPercept& p) {
    if(!p.valid) throw Val("Inv");
    auto f=per->exFeatures();
    b->recSensoryIn(f);
}
```

#### Comments

```cpp
// Good: Explanatory comments
/**
 * Initialize brain with configuration and error handling
 * @param config Shared pointer to configuration object
 * @throws BrainInitializationError if configuration is invalid
 */
Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>()) {
    try {
        if (!config || !validateConfiguration(*config)) {
            throw BrainInitializationError(
                "Invalid configuration for brain initialization",
                "Brain::Brain",
                BrainInitializationError::Code::InvalidConfiguration
            );
        }
        // ... rest of initialization
    } catch (const ConfigError& e) {
        // Re-throw as brain initialization error
        throw BrainInitializationError(
            std::string("Configuration error during brain initialization: ") + e.what(),
            "Brain::Brain",
            BrainInitializationError::Code::InvalidConfiguration
        );
    }
}

// Inline comments
auto neuronCount = brain->getTotalNeuronCount(); // Total neurons in the brain
auto firingRate = brain->getAverageFiringRate(); // Average firing rate (Hz)
```

#### Documentation

Use Doxygen-style comments for public API:

```cpp
/**
 * @class Brain
 * @brief The central coordinator of the neural system
 * 
 * The Brain class implements real spiking neural computation with event-driven
 * dynamics and integrated memory, prediction, cognition, and neuromodulation
 * systems with comprehensive error handling and robustness features.
 * 
 * @see NeuralSystem, MemorySystem, PredictionSystem, CognitiveSystem
 * @note This is the main interface for neural computation in NLM
 * @ingroup neural_components
 */
class Brain {
public:
    /**
     * @brief Initialize brain with configuration and validation
     * 
     * @param config Configuration object containing simulation parameters
     * @return true if initialization successful, false otherwise
     * @throws BrainInitializationError if configuration is invalid
     * @see validateConfiguration()
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with error handling
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time
     * @note This is the main computation loop for the neural system
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ... more methods
};
```

### 2. Python Bindings Coding Standards

#### Python API Design

```python
import pynlm
from typing import List, Optional, Dict, Any

class Brain:
    """Main interface for neural computation in NLM.
    
    The Brain class provides the primary API for creating and controlling
    spiking neural networks. It supports comprehensive error handling,
    configuration management, and integration with world environments.
    
    Examples:
        >>> import pynlm
        >>> config = pynlm.create_default_config()
        >>> brain = pynlm.create_brain(config)
        >>> brain.initialize()
        >>> print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    
    Attributes:
        config: Configuration object for brain parameters
        total_neurons: Total number of neurons in the brain
        total_synapses: Total number of synaptic connections
    
    Methods:
        initialize(): Initialize the brain with configuration
        step(step_number): Run one simulation step
        receive_sensory_input(input): Inject sensory data into brain
        produce_action(): Generate motor action from brain activity
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """Initialize brain with configuration.
        
        Args:
            config: Configuration dictionary. If None, uses default configuration.
        
        Raises:
            NLMError: If configuration is invalid or initialization fails.
        """
        pass
    
    def initialize(self) -> bool:
        """Initialize the brain with configuration.
        
        Returns:
            bool: True if initialization successful, False otherwise.
        
        Raises:
            NLMError: If initialization fails due to invalid configuration
                      or missing dependencies.
        """
        pass
    
    def step(self, step_number: int) -> None:
        """Run one simulation step.
        
        Args:
            step_number: Current simulation step number.
        
        Raises:
            NLMError: If simulation step fails due to invalid state or
                     computational errors.
        """
        pass
```

#### Python Naming Conventions

```python
# ✅ GOOD: Pythonic naming
class NeuralWorkingMemory:
    def __init__(self, capacity: int = 1000):
        self.capacity = capacity
        self.stored_experiences = []
        self.current_activity = 0.0
    
    def store_experience(self, experience: Dict[str, Any]) -> bool:
        """Store an experience in working memory.
        
        Args:
            experience: Experience data to store.
        
        Returns:
            bool: True if stored successfully, False if memory full.
        """
        if len(self.stored_experiences) >= self.capacity:
            return False
        self.stored_experiences.append(experience)
        return True

# ❌ BAD: Non-Pythonic naming
class Neural_Working_Memory:
    def __init__(self, capacity_int = 1000):
        self.capacity_int = capacity_int
        self.stored_experiences_list = []
        self.current_activity_float = 0.0
    
    def store_experience_dict(self, experience_dict = {}):
        if len(self.stored_experiences_list) >= self.capacity_int:
            return False
        self.stored_experiences_list.append(experience_dict)
        return True
```

### 3. File Organization Best Practices

#### Configuration Management

Keep configuration separate from source code:

```bash
# Configuration files
configs/
├── default.cfg          # Default configuration
├── config.json          # JSON format
├── config.yaml          # YAML format
├── phase6_integration/  # Phase 6 specific configs
└── benchmarks/          # Benchmark configurations
```

#### Build Artifacts

Exclude build artifacts from version control:

```bash
# Exclude these files/directories from git
build/
dist/
*.pyc
__pycache__/
*.so
*.dylib
*.dll
*.exe
*.o
*.obj
*.log
*.prof
```

#### Documentation

Keep documentation in the docs/ directory:

```bash
docs/
├── Architecture.md          # System architecture
├── Configuration.md          # Configuration system
├── ErrorHandling.md          # Error handling guide
├── Performance.md            # Performance optimization guide
├── Phase6Guide.md            # Phase 6 integration guide
├── Development.md            # Development guidelines
├── APIReference.md           # Complete API reference
├── QuickStart.md             # Quick start guide
└── ...                      # Other documentation
```

## Testing Methodologies

### 1. Testing Strategy

#### Unit Tests

Test individual components in isolation:

```cpp
// TestBrain.cpp
class TestBrain : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = createTestConfiguration();
        brain_ = std::make_unique<Brain>(config_);
        brain_->initialize();
    }
    
    void TearDown() override {
        brain_.reset();
        config_.reset();
    }
    
    std::shared_ptr<Config> config_;
    std::unique_ptr<Brain> brain_;
};

TEST_F(TestBrain, Initialization) {
    ASSERT_TRUE(brain_->initialize());
    EXPECT_EQ(brain_->getTotalNeuronCount(), 1000);
}

TEST_F(TestBrain, StepExecution) {
    brain_->step(1, 0.1);
    // Verify state after step
    EXPECT_GT(brain_->getTotalSpikeCount(), 0);
}
```

#### Integration Tests

Test component interactions:

```cpp
// TestIntegration.cpp
class TestIntegration : public ::testing::Test {
protected:
    void SetUp() override {
        // Create full integrated system
        config_ = createIntegrationConfig();
        brain_ = createBrain(config_);
        agent_ = createAgentBrain(brain_);
        world_ = createSimpleWorld();
        
        // Initialize all components
        brain_->initialize();
        agent_->initialize(world_);
        world_->initialize();
    }
    
    void TearDown() override {
        // Clean up in reverse order
        world_.reset();
        agent_.reset();
        brain_.reset();
        config_.reset();
    }
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
};

TEST_F(TestIntegration, FullSystemIntegration) {
    // Run full simulation loop
    for (int step = 0; step < 100; ++step) {
        world_->update(0.1f);
        agent_->processSensoryInput(world_->getSensoryPercept());
        brain_->step(step);
        world_->applyAction(agent_->decodeMotorCommand(), step * 0.1f);
    }
    
    // Verify integration
    EXPECT_TRUE(brain_->getWorkingMemory()->isFunctional());
    EXPECT_TRUE(brain_->getEpisodicMemory()->hasStorage());
    EXPECT_TRUE(brain_->getPredictionSystem()->isTrained());
}
```

#### Performance Tests

Benchmark performance:

```bash
// Run performance benchmarks
./test/nlm_benchmark --neurons=10000 --steps=10000 --output=perf_results.json

// Compare with baseline
./test/nlm_benchmark --compare-baseline --neurons=10000 --steps=10000
```

#### Stress Tests

Test system under extreme conditions:

```bash
// Stress test with many neurons
./test/nlm_stress --neurons=1000000 --steps=1000

// Memory stress test
./test/nlm_memory_stress --memory-limit=1GB

// Time stress test
./test/nlm_time_stress --timeout=300
```

### 2. Testing Framework

#### Google Test Framework

```cpp
// tests/CMakeLists.txt
add_executable(test_brain TestBrain.cpp)
target_link_libraries(test_brain brain_core)

add_executable(test_integration TestIntegration.cpp)
target_link_libraries(test_integration nlm_agent nlm_world)

add_executable(test_benchmark TestBenchmark.cpp)
target_link_libraries(test_benchmark benchmark_framework)

# Run all tests
add_custom_target(run_all_tests
    COMMAND ./test_brain
    COMMAND ./test_integration
    COMMAND ./test_benchmark
    DEPENDS test_brain test_integration test_benchmark
)
```

#### Python Testing

```python
# tests/test_python_api.py
import pytest
import pynlm

class TestPythonAPI:
    """Test Python API functionality."""
    
    def setup_method(self):
        """Set up test environment."""
        self.config = pynlm.create_default_config()
        self.brain = pynlm.create_brain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Clean up test environment."""
        del self.brain
        del self.config
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        assert self.brain is not None
        assert self.brain.get_total_neuron_count() > 0
    
    def test_configuration(self):
        """Test configuration management."""
        config = self.brain.get_config()
        assert config.has("brain.neuron_count")
        assert config.get_or("brain.neuron_count", 0) > 0
    
    def test_simulation_step(self):
        """Test simulation step execution."""
        initial_spikes = self.brain.get_total_spike_count()
        self.brain.step(1)
        assert self.brain.get_total_spike_count() >= initial_spikes
```

#### Python Test Runner

```bash
# Run Python tests
pytest tests/ -v --tb=short --cov=pynlm

# Run with coverage report
pytest tests/ --cov=pynlm --cov-report=html --cov-report=term
```

## Build and Deployment Procedures

### 1. Building the Project

#### Standard Build

```bash
# Create build directory
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -C build -j$(nproc)

# Run tests
ctest --output-on-failure
```

#### Development Build

```bash
# Build with development flags
cmake -S . -B build-dev -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_DEVELOPER_MODE=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_COVERAGE=ON \
    -DNLM_ENABLE_STATIC_ANALYSIS=ON

# Build with performance optimizations
cmake -S . -B build-opt -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PERFORMANCE_ENABLED=ON \
    -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_PARALLEL=ON
```

#### Python Bindings Build

```bash
# Install Python development dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

### 2. Build Scripts

#### Build Script

```bash
#!/bin/bash
# build.sh - Build script for NLM

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_TYPE="${1:-Release}"
BUILD_DIR="${2:-build}"

# Configure build
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Build
make -C "$BUILD_DIR" -j$(nproc)

# Run tests
if [[ "$BUILD_TYPE" == "Debug" || "$BUILD_TYPE" == "RelWithDebInfo" ]]; then
    echo "Running tests..."
    ctest --output-on-failure -C "$BUILD_TYPE"
fi

echo "Build completed successfully!"
```

#### Installation Script

```bash
#!/bin/bash
# install.sh - Installation script for NLM

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${1:-build}"
INSTALL_PREFIX="${2:-/usr/local}"

# Build if not already built
if [[ ! -f "$BUILD_DIR/bin/nlm" ]]; then
    echo "Building NLM..."
    cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
    make -C "$BUILD_DIR" -j$(nproc)
fi

# Install
make -C "$BUILD_DIR" install

# Install Python bindings
pip install "$SCRIPT_DIR"

echo "Installation completed successfully!"
```

### 3. Deployment Procedures

#### Deployment for Development

```bash
# Deploy development version
deploy.sh build-dev /opt/nlm-dev

# Set up environment
export NLM_HOME=/opt/nlm-dev
source $NLM_HOME/setup_env.sh

# Start development services
./$NLM_HOME/bin/nlm_phase4_demo &
./$NLM_HOME/bin/nlm_test &
```

#### Deployment for Production

```bash
# Deploy production version
deploy.sh build-opt /usr/local

# Create systemd service
 sudo tee /etc/systemd/system/nlm.service > /dev/null <<EOF
[Unit]
Description=NLM (Neural Learning Machine)
After=network.target

[Service]
Type=simple
User=nlm
Group=nlm
WorkingDirectory=/usr/local
ExecStart=/usr/local/bin/nlm --daemon
Restart=on-failure
RestartSec=10
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=nlm

[Install]
WantedBy=multi-user.target
EOF

# Enable and start service
sudo systemctl daemon-reload
sudo systemctl enable nlm
sudo systemctl start nlm
```

### 4. Continuous Integration

#### GitHub Actions Workflow

Create `.github/workflows/ci.yml`:

```yaml
name: CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up C++ environment
      uses: egor-tensin/setup-cpp@v1
      with:
        compiler: gcc
        cmake: true
    
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: '3.10'
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y python3-dev python3-pip
        pip install pybind11 scikit-build-core pytest numpy
    
    - name: Configure CMake
      run: |
        mkdir -p build
        cmake -S . -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: |
        cmake --build build --config ${{ matrix.build_type }} -- -j$(nproc)
    
    - name: Run Tests
      run: |
        cd build
        ctest --output-on-failure -C ${{ matrix.build_type }}
    
    - name: Python Tests
      run: |
        pytest tests/ -v --tb=short
    
    - name: Performance Tests
      run: |
        ./build/test/nlm_benchmark --neurons=10000 --steps=10000
    
    - name: Integration Tests
      run: |
        ./build/test/nlm_integration_test
    
    - name: Upload Artifacts
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: test-results-${{ matrix.build_type }}
        path: |
          build/test/
          test_results.json
```

## Contribution Guidelines

### 1. Contributing Code

#### Fork and Branch

1. Fork the repository on GitHub
2. Create a feature branch from develop
3. Keep branch names descriptive:
   - `feature/component-integration`
   - `bugfix/error-handling-improvement`
   - `docs/architecture-update`

#### Commit Messages

Use conventional commit messages:

```
<type>[optional scope]: <description>

[optional body]

[optional footer]
```

Types:
- **feat**: New feature
- **fix**: Bug fix
- **docs**: Documentation only changes
- **style**: Code style changes (formatting, etc.)
- **refactor**: Code refactoring
- **perf**: Performance improvements
- **test**: Adding tests
- **chore**: Maintenance tasks

Examples:
```
feat: integrate working memory with neural processing
fix: null pointer dereference in Brain::step()
docs: update configuration documentation for Phase 6
style: format code using clang-format
refactor: simplify memory management in ErrorHandling
perf: optimize spike processing for large networks
test: add integration tests for memory systems
chore: update GitHub Actions workflow
```

#### Pull Requests

1. Push feature branch to your fork
2. Create pull request to develop branch
3. Provide detailed description:
   - Problem statement
   - Solution approach
   - Testing performed
   - Known limitations
4. Request review from appropriate maintainers
5. Address feedback and update PR

### 2. Code Review Process

#### Review Checklist

- [ ] Code follows project coding standards
- [ ] All tests pass
- [ ] Performance impact analyzed
- [ ] Documentation updated
- [ ] Error handling reviewed
- [ ] Integration with existing code verified
- [ ] Memory management correct
- [ ] Thread safety ensured
- [ ] Backward compatibility maintained

#### Review Comments Format

```markdown
## Review Comments

### ✅ Approved

### ❌ Changes Required

1. **Code Style**: Code needs formatting with clang-format
2. **Error Handling**: Add missing null pointer checks
3. **Testing**: Add unit tests for new functionality
4. **Documentation**: Update API reference

### 📝 Questions

1. Why this approach instead of alternative X?
2. How will this affect performance?
3. Are there any edge cases not handled?
```

### 3. Code Review Tools

#### Static Analysis

```bash
# Run clang-tidy
clang-tidy -p build src/ -checks='*' -- -I include -std=c++20

# Run clang-format check
clang-format -style=file --dry-run src/ include/
```

#### Code Coverage

```bash
# Generate coverage report
pytest --cov=pynlm --cov-report=html

# Check coverage thresholds
coverage threshold --include=pynlm --fail-under=80
```

#### Performance Analysis

```bash
# Profile performance
perf record -g ./build/bin/nlm_benchmark --neurons=10000
perf report

# Memory profiling
valgrind --tool=massif ./build/bin/nlm_benchmark --neurons=10000
```

### 4. Documentation Contribution

#### Documentation Standards

- Use Doxygen-style comments for C++ API
- Use Google-style docstrings for Python API
- Include examples in documentation
- Keep documentation consistent with project style
- Update API reference when adding/modifying APIs

#### Documentation Workflow

1. Write/ update documentation
2. Ensure documentation builds correctly
3. Run documentation validation
4. Submit for review with code changes

### 5. Testing Contribution

#### Test Development Guidelines

- Write tests that are isolated, repeatable, and maintainable
- Use test frameworks consistently
- Cover edge cases and error conditions
- Include performance benchmarks
- Document test assumptions and limitations

#### Test Integration

- Add tests to existing test suites
- Follow existing test patterns
- Update CI/CD pipeline if needed
- Ensure tests run on all platforms

### 6. Release Process

#### Version Management

- Use semantic versioning (MAJOR.MINOR.PATCH)
- Update version in configuration files
- Update version in documentation
- Update version in build scripts

#### Release Checklist

```markdown
## Pre-Release Checklist

### Code Quality
- [ ] All tests pass
- [ ] Code style enforced
- [ ] Static analysis clean
- [ ] Performance benchmarks meet targets
- [ ] Memory usage acceptable

### Documentation
- [ ] API documentation generated
- [ ] User guides updated
- [ ] Examples tested and working
- [ ] Release notes written

### Build and Distribution
- [ ] Build scripts tested
- [ ] Distribution packages created
- [ ] Installation procedures verified
- [ ] Documentation packages built

### Final Review
- [ ] Security review completed
- [ ] Performance validation complete
- [ ] Integration testing passed
- [ ] Documentation quality reviewed
```

### 7. Team Collaboration

#### Code Review Collaboration

1. Provide constructive feedback
2. Focus on code quality and maintainability
3. Suggest alternatives when appropriate
4. Document decisions and rationale
5. Maintain respectful and professional communication

#### Technical Collaboration

1. Share knowledge and expertise
2. Mentor junior developers
3. Collaborate on complex problems
4. Participate in code reviews
5. Contribute to design discussions

## Development Tools and Utilities

### Development Tools

#### IDE Integration

- **Visual Studio Code**: Full support with CMake integration
- **CLion**: Professional C++ IDE with CMake support
- **VS Code Insiders**: Enhanced development experience

#### Code Quality Tools

```bash
# Install development tools
sudo apt-get install -y clang-format clang-tidy gcovr doxygen

# Configure tools
cat > .clang-format << 'EOF'
BasedOnStyle: LLVM
IndentWidth: 4
BreakBeforeBraces: All
IndentCaseLabels: false
AllowShortIfStatementsOnOneLine: false
AllowShortLoopsOnOneLine: false
EOF
```

#### Profiling Tools

```bash
# Install profiling tools
sudo apt-get install -y perf valgrind gprof

# Profile with different tools
perf record -g ./build/bin/nlm --benchmark
valgrind --tool=massif ./build/bin/nlm
```

### Development Scripts

#### Generate Documentation

```bash
# Generate Doxygen documentation
doxygen Doxyfile

# Generate Python API documentation
sphinx-build -b html docs/ html/

# Generate complete documentation
make docs
```

#### Validate Configuration

```bash
#!/bin/bash
# validate_config.sh - Validate configuration files

set -e

CONFIG_FILE="${1:-configs/default.cfg}"
FORMAT="${2:-simple}"

# Validate based on format
if [[ "$FORMAT" == "json" ]]; then
    python3 -m json.tool "$CONFIG_FILE" > /dev/null
    echo "✓ JSON configuration valid"
elif [[ "$FORMAT" == "yaml" ]]; then
    python3 -c "import yaml; yaml.safe_load(open('$CONFIG_FILE'))" > /dev/null
    echo "✓ YAML configuration valid"
else
    # Simple format validation
    while IFS='=' read -r key value; do
        if [[ -n "$key" && "$key" != "#"* ]]; then
            echo "✓ Found configuration: $key=$value"
        fi
    done < "$CONFIG_FILE"
fi

echo "Configuration validation completed successfully"
```

#### Run All Tests

```bash
#!/bin/bash
# run_tests.sh - Run all tests and generate reports

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${1:-build}"
TEST_REPORT_DIR="${2:-test_reports}"

mkdir -p "$TEST_REPORT_DIR"

# Run C++ tests
echo "Running C++ tests..."
if [[ -f "$BUILD_DIR/test/nlm_test" ]]; then
    "$BUILD_DIR/test/nlm_test" --gtest_output="xml:$TEST_REPORT_DIR/cpp_tests.xml"
fi

# Run integration tests
echo "Running integration tests..."
if [[ -f "$BUILD_DIR/test/nlm_integration_test" ]]; then
    "$BUILD_DIR/test/nlm_integration_test" --gtest_output="xml:$TEST_REPORT_DIR/integration_tests.xml"
fi

# Run Python tests
echo "Running Python tests..."
pytest tests/ --junitxml="$TEST_REPORT_DIR/python_tests.xml" --cov=pynlm --cov-report=xml:"$TEST_REPORT_DIR/python_coverage.xml"

# Run performance tests
echo "Running performance tests..."
if [[ -f "$BUILD_DIR/test/nlm_benchmark" ]]; then
    "$BUILD_DIR/test/nlm_benchmark" --output="$TEST_REPORT_DIR/performance_results.json"
fi

# Generate combined report
echo "Generating combined test report..."
python3 "$SCRIPT_DIR/scripts/generate_test_report.py" "$TEST_REPORT_DIR"

echo "All tests completed. Reports available in $TEST_REPORT_DIR"
```

## Conclusion

This Development Documentation provides comprehensive guidance for working with the NLM project. It covers:

1. **Development Environment Setup**: Prerequisites and tooling
2. **Coding Standards**: Code style, conventions, and best practices
3. **Testing Methodologies**: Unit tests, integration tests, performance tests
4. **Build Procedures**: Standard and development builds
5. **Deployment Procedures**: Development and production deployment
6. **Contribution Guidelines**: Code review, testing, and release process
7. **Development Tools**: Scripts and utilities for development

Key principles:

1. **Quality First**: Code quality, testing, and documentation are essential
2. **Performance Aware**: Development considers performance impact
3. **Consistent**: Follows established patterns and conventions
4. **Collaborative**: Encourages code review and knowledge sharing
5. **Documented**: Comprehensive documentation for all components
6. **Tested**: Thorough testing at all levels

The NLM project is a collaborative research effort where high-quality code, comprehensive testing, and thorough documentation are essential for scientific progress and reproducibility.