# NLM Development Guide

## Overview

The NLM development guide provides comprehensive information for developers who want to contribute to the project, extend its capabilities, and understand the development workflow. It covers project structure, build processes, contribution guidelines, and extension development patterns.

## Project Structure

### Directory Layout

```
NLM/
├── CMakeLists.txt                    # Top-level CMake configuration
├── README.md                         # Project overview and installation
├── pyproject.toml                    # Python package configuration
├── docs/                            # Documentation files
│   ├── ARCHITECTURE.md              # System architecture
│   ├── CONFIGURATION.md             # Configuration system
│   ├── CLI-Reference.md             # Command line interface
│   ├── EXPERIMENTS.md               # Experiment descriptions
│   ├── API-Guide.md                 # Programming interface
│   ├── PERFORMANCE-Guide.md         # Performance optimization
│   ├── DEVELOPMENT-Guide.md         # This file
│   ├── Troubleshooting.md            # Common issues
│   ├── SCIENCE.md                   # Scientific background
│   ├── PHASE6_ARCHITECTURE.md       # Phase 6 integration details
│   ├── PHASE6_FINAL_AUDIT.md        # Phase 6 audit results
│   └── ...                          # Additional documentation
├── src/                             # Source code
│   ├── core/                        # Core utilities
│   │   ├── Config.cpp/hpp           # Configuration management
│   │   ├── Logger.cpp/hpp           # Logging system
│   │   ├── Random.cpp/hpp           # Random number generation
│   │   └── SimulationClock.cpp/hpp  # Time management
│   ├── brain/                       # Neural components
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity rules
│   ├── development/                 # Developmental system
│   ├── neuromodulation/              # Neuromodulators
│   ├── memory/                      # Memory systems
│   ├── prediction/                   # Prediction systems
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                 # Environment interface
│   ├── experiments/                 # Experiment framework
│   └── visualization/                # Visualization
│       └── Visualizer.cpp/hpp       # Visualization interface
├── tests/                           # Test suite
│   ├── test_main.cpp               # Main test framework
│   ├── test_brain.cpp              # Brain component tests
│   ├── test_config.cpp             # Configuration tests
│   ├── test_json_config.cpp        # JSON config tests
│   ├── test_synapse.cpp            # Synapse tests
│   ├── test_types.cpp              # Type system tests
│   ├── test_random.cpp             # Random tests
│   ├── test_stdp.cpp               # STDP tests
│   ├── test_neuron.cpp             # Neuron tests
│   ├── test_clock.cpp              # Clock tests
│   └── test_performance.cpp        # Performance tests
├── build/                           # Build outputs
├── examples/                        # Working examples
│   ├── simple_brain.py             # Basic brain usage
│   ├── agent_simulation.py         # Complete agent example
│   └── experiment_configs/          # Experiment configurations
└── scripts/                         # Development scripts
    ├── validate_config.py           # Configuration validation
    ├── generate_docs.py             # Documentation generation
    ├── benchmark.py                 # Benchmarking suite
    └── performance_test.py          # Performance testing
```

### Key Directories

#### Core Components (`src/core/`)
- **Config**: Configuration management system
- **Logger**: Structured logging implementation
- **Random**: Deterministic random number generator
- **Clock**: Simulation time management

#### Neural Components (`src/brain/`)
- **Neuron**: LIF neuron implementation
- **Synapse**: Synaptic connections
- **NeuralPopulation**: Groups of neurons with shared properties
- **NeuralRegion**: Brain regions with multiple populations
- **Brain**: Central coordinator

#### Development Pipeline (`src/development/`)
- **DevelopmentSystem**: Orchestrates developmental stages
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

#### Memory Systems (`src/memory/`)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels

#### Performance Layer (`src/performance/`)
- **MemoryPool**: Custom memory allocation
- **EventQueue**: Event-driven spike processing
- **CheckpointSystem**: State persistence

## Build Process

### Prerequisites

**Software Requirements:**
```bash
# C++ Compiler
g++ 10+ or clang 12+ or MSVC 2019+

# Build Tools
cmake 3.16+ or ninja
cmake 3.16+ or ninja
python 3.8+ (for Python bindings)

# Dependencies
pybind11 2.11.0+
nlohmann_json 3.11.2+
CLI11 2.4.0+
```

**Hardware Requirements:**
- **Small Scale (1000 neurons):** Minimum 4 GB RAM
- **Medium Scale (100,000 neurons):** 8-16 GB RAM
- **Large Scale (1,000,000 neurons):** 32+ GB RAM
- **Multi-core:** At least 4 CPU cores recommended

### Building with CMake

**Standard Build:**
```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets with parallel compilation
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

**Debug Build:**
```bash
mkdir -p build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0 -DDEBUG"
make -j$(nproc)
```

**Performance Build:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_SSE=1 \
         -DNLM_ENABLE_AVX=1 \
         -DNLM_ENABLE_OPENMP=1 \
         -DNLM_ENABLE_CUDA=0
```

### Building with Python Bindings

**Using pyproject.toml (Modern Python Packaging):**
```bash
# Install build dependencies
pip install scikit-build-core pybind11 nlohmann_json CLI11 pytest numpy

# Build and install
pip install .

# Or install in development mode (editable)
pip install -e .

# Build only (without installing)
pip install build
python -m build
```

**Using setup.py (Legacy):**
```bash
# Install setup.py dependencies
pip install pybind11 numpy

# Build and install
python setup.py install
```

### Build Outputs

| Target | Description | Location |
|--------|-------------|----------|
| `nlm` | Main simulation executable | `build/` |
| `nlm_core` | Static library (core brain components) | `build/` |
| `nlm_agent` | Static library (agent system) | `build/` |
| `nlm_world` | Static library (world simulation) | `build/` |
| `nlm_phase3_demo` | Phase 3 demonstration | `build/` |
| `nlm_phase4_demo` | Phase 4 demonstration | `build/` |
| `nlm_test` | Unit test suite | `build/` |
| `pynlm` | Python bindings | `dist/` |

### Build Options

**CMake Configuration Options:**
```bash
# Enable optimizations
cmake .. -DNLM_ENABLE_SSE=1
cmake .. -DNLM_ENABLE_AVX=2
cmake .. -DNLM_ENABLE_FMA=1

# Enable multi-threading
cmake .. -DNLM_ENABLE_OPENMP=1

# Enable GPU acceleration (if CUDA available)
cmake .. -DNLM_ENABLE_CUDA=1

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=g++
```

**Python Configuration Options:**
```bash
# Build with specific options
pip install . --config-settings=-DNLM_ENABLE_SSE=1

# Build in specific directory
pip install . --prefix ./python_install
```

## Contributing Code

### Code Style Guidelines

**C++ Code Style:**
```cpp
// File organization
// Top: Includes
// Middle: Namespace open
// Bottom: Function definitions, tests, private implementation

// Naming conventions
class ClassName {                    // CamelCase for classes
    public:
        static const int kConstant = 42;  // Constants in caps
        void publicMethod();             // public methods
    
    private:
        NeuronID neuronId_;            // Member variables with underscore
        std::string name_;             // Private implementation
};

// Indentation
namespace nlm {
    class Brain {
        void veryLongMethodName(
            const ParameterType& param1,
            double param2) const {
            // Code here
        }
    };
}

// Documentation/**
 * This class represents a neural brain simulation.
 * 
 * @param config Configuration for the brain
 * @param id Unique identifier for the brain
 * @return New brain instance
 */
Brain createBrain(const Config& config, BrainID id);
```

**Python Code Style:**
```python
"""Module docstring with brief description.

This module provides functionality for X, Y, and Z.

Attributes:
    DEFAULT_CONFIG_PATH (str): Default path for configuration files.
    NEURON_TYPES (list): Available neuron types.

Functions:
    create_brain(config): Create and initialize a brain.
    create_agent(brain, world): Create an agent-brain interface.
"""

import pynlm

# Constants
DEFAULT_CONFIG_PATH = "config.json"
NEURON_TYPES = ["excitatory", "inhibitory", "sensory", "motor"]

# Functions
def create_brain(config=None):
    """Create and initialize a brain.
    
    Args:
        config (dict, optional): Brain configuration. 
            If None, uses default configuration.
    
    Returns:
        Brain: Initialized brain instance.
    
    Raises:
        ValueError: If configuration is invalid.
    
    Examples:
        >>> brain = create_brain()
        >>> print(f"Brain has {brain.get_total_neurons()} neurons")
    """
    if config is None:
        config = pynlm.create_default_config()
    
    return pynlm.create_brain(config)
```

**Documentation Style:**
- Use Doxygen for C++ (JavaDoc style comments)
- Use Markdown with Google style for Python
- Maintain consistent formatting across all files
- Include examples and usage notes in documentation

### Coding Standards

**Quality Standards:**
```cpp
// Use RAII for resource management
class NeuralRegion {
public:
    NeuralRegion() {
        std::cout << "NeuralRegion created" << std::endl;
    }
    
    ~NeuralRegion() {
        std::cout << "NeuralRegion destroyed" << std::endl;
    }
    
    // Prevent copying
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    // Allow moving
    NeuralRegion(NeuralRegion&& other) noexcept {
        // Move implementation
    }
    
    NeuralRegion& operator=(NeuralRegion&& other) noexcept {
        // Move assignment implementation
    }
};

// Use smart pointers for dynamic allocation
class Brain {
private:
    std::unique_ptr<NeuralRegion[]> regions_;
    std::shared_ptr<PlasticityRule> plasticity_;
};

// Use constexpr for constant values
namespace Constants {
    constexpr double kDefaultThreshold = -50.0;
    constexpr double kDefaultRestPotential = -70.0;
    constexpr double kDefaultMembraneTimeConstant = 20.0;
    constexpr double kDefaultRefractoryPeriod = 2.0;
}
```

### Testing Guidelines

**Unit Testing Standards:**
```cpp
// Test file structure
// test_filename.cpp

#include "gtest/gtest.h"
#include "your_class.hpp"

class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
        object_ = YourClass(
            TestConfig(),
            AnotherClass("test")
        );
    }
    
    void TearDown() override {
        // Cleanup code
        object_ = YourClass();
    }
    
    YourClass object_;
};

// Test cases
TEST_F(YourClassTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(object_.getProperty(), "expected_value");
    EXPECT_TRUE(object_.isValid());
}

TEST_F(YourClassTest, MethodDoesSomething) {
    // Arrange
    GivenState state = GivenState::WithValue(10);
    
    // Act
    Result result = object_.doSomething(state);
    
    // Assert
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.value, 20);
}

TEST(YourClassStandalone, StaticMethodTest) {
    EXPECT_EQ(YourClass::staticMethod(), "static_result");
}
```

**Python Testing Standards:**
```python
import pytest
import pynlm

class TestBrain:
    """Test class for Brain functionality."""
    
    def setup_method(self):
        """Setup before each test method."""
        self.config = pynlm.create_default_config()
        self.brain = pynlm.create_brain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Cleanup after each test method."""
        self.brain = None
    
    def test_brain_creation(self):
        """Test that brain is created with correct neuron count."""
        assert self.brain is not None
        assert self.brain.get_total_neurons() == self.config.get("brain.neuron_count")
    
    def test_brain_initialization(self):
        """Test that brain initializes correctly."""
        self.brain.initialize()
        assert self.brain.get_firing_neuron_count() >= 0
    
    @pytest.mark.parametrize("step,count", [
        (0, 0),
        (100, 10),
        (1000, 50),
    ])
    def test_brain_step(self, step, expected_min_count):
        """Test brain step with different step counts.
        
        Args:
            step (int): Number of steps to run.
            expected_min_count (int): Minimum expected firing neurons.
        """
        for i in range(step):
            self.brain.step(i)
        
        assert self.brain.get_firing_neuron_count() >= expected_min_count
```

**Integration Testing:**
```bash
# Run unit tests
cmake --build build --target nlm_test

# Run integration tests
./nlm_integration_test --test-dir tests/integration

# Run performance tests
./nlm_benchmark --performance-mode full
```

### Git Workflow

**Branching Strategy:**
```
main                    # Main development branch
feature/xxx             # Feature branches
bugfix/xxx              # Bug fix branches
release/1.2.0           # Release branches
feature/experiments     # Experimental features
```

**Commit Message Guidelines:**
```
# Format: <type>(<scope>): <description>

# Examples
feat(brain): add new neural region type
fix(config): resolve loading issue for YAML files
docs(api): update parameter documentation
refactor(plasticity): simplify STDP implementation
perf(neuron): optimize LIF dynamics with SIMD
style: fix linting issues in new files

# Commit body (optional but recommended):
# - Description of changes
# - Rationale for changes
# - Testing performed
# - Known issues
# - Related issues
```

**Pull Request Process:**
1. **Pre-commit Checklist:**
   ```bash
   git status                    # Check working directory
   git diff --staged             # Review staged changes
   clang-format --style=file -i src/yourfile.cpp  # Format C++
   black yourfile.py            # Format Python
   flake8 yourfile.py           # Check Python style
   ./scripts/lint.sh            # Run project linter
   ```

2. **Commit and Push:**
   ```bash
   git add src/ tests/ docs/ yourfile.py
   git commit -m "feat(your_feature): add new functionality"
   git push origin feature/your-branch-name
   ```

3. **Create Pull Request:**
   - Fill out PR template
   - Add relevant labels
   - Request reviews from relevant maintainers
   - Run CI checks

4. **Code Review:**
   - Check code quality and style
   - Verify tests pass
   - Review documentation
   - Check for performance impact

5. **Merge:**
   - Squash commits if needed
   - Update CHANGELOG
   - Tag release if appropriate

### Code Review Process

**Review Checklist:**

1. **Code Quality:**
   - [ ] Code follows project style guidelines
   - [ ] No unused imports or variables
   - [ ] Appropriate use of RAII and smart pointers
   - [ ] Proper error handling
   - [ ] Thread safety where needed

2. **Tests:**
   - [ ] Unit tests cover new functionality
   - [ ] Integration tests verify system compatibility
   - [ ] Performance tests document speed impact
   - [ ] Edge cases are tested

3. **Documentation:**
   - [ ] API documentation updated
   - [ ] Usage examples provided
   - [ ] Code comments explain complex logic
   - [ ] External documentation updated

4. **Integration:**
   - [ ] Compatible with existing API
   - [ ] Doesn't break existing functionality
   - [ ] Follows established patterns
   - [ ] Uses appropriate abstractions

## Extension Development

### Adding New Plasticity Rules

**Example: Custom Plasticity Rule:**
```cpp
// plasticity/CustomPlasticity.hpp
#ifndef NLM_CUSTOM_PLASTICITY_HPP
#define NLM_CUSTOM_PLASTICITY_HPP

#include "plasticity/PlasticityRule.hpp"

namespace nlm {

class CustomPlasticity : public PlasticityRule {
public:
    CustomPlasticity() : PlasticityRule("CustomPlasticity") {}
    
    void apply(Neuron* pre, Neuron* post, double dt) override {
        double weightChange = calculateWeightChange(pre, post, dt);
        pre->adjustSynapticWeight(post, weightChange);
    }
    
    std::string getName() const override {
        return "CustomPlasticity";
    }
    
private:
    double calculateWeightChange(Neuron* pre, Neuron* post, double dt) {
        // Custom plasticity rule implementation
        double preSpike = pre->getLastSpikeTime();
        double postSpike = post->getLastSpikeTime();
        
        if (preSpike > 0 && postSpike > 0) {
            double timeDiff = std::abs(preSpike - postSpike);
            return customLearningRate * std::exp(-timeDiff / timeConstant);
        }
        
        return 0.0;
    }
    
    static constexpr double customLearningRate = 0.001;
    static constexpr double timeConstant = 10.0;
};

} // namespace nlm

#endif // NLM_CUSTOM_PLASTICITY_HPP
```

**Registration:**
```cpp
// plasticity/CustomPlasticity.cpp
#include "plasticity/CustomPlasticity.hpp"
#include "plasticity/PlasticityRuleFactory.hpp"

namespace nlm {

void registerCustomPlasticity() {
    PlasticityRuleFactory::registerRule(
        "CustomPlasticity",
        []() { return new CustomPlasticity(); }
    );
}

} // namespace nlm

// Call in initialization
void PlasticityRuleFactory::initialize() {
    registerStandardPlasticityRules();
    registerCustomPlasticity();  // Your custom registration
}
```

### Adding New Memory Systems

**Example: Custom Memory System:**
```cpp
// memory/CustomMemory.hpp
#ifndef NLM_CUSTOM_MEMORY_HPP
#define NLM_CUSTOM_MEMORY_HPP

#include "memory/Memory.hpp"

namespace nlm {

class CustomMemory : public Memory {
public:
    CustomMemory(size_t capacity) : Memory(capacity) {}
    
    void store(const Experience& experience) override {
        // Store experience in custom data structure
        customStorage_.push_back(processExperience(experience));
    }
    
    Experience recall(const Query& query) const override {
        // Retrieve relevant experience
        auto bestMatch = findBestMatch(query);
        return reconstructExperience(bestMatch);
    }
    
    void update(double dt) override {
        // Update memory traces
        for (auto& trace : customStorage_) {
            trace.decay(dt);
        }
        consolidateWeakTraces();
    }
    
    std::string getType() const override {
        return "CustomMemory";
    }
    
private:
    struct MemoryTrace {
        Experience experience;
        double strength;
        double age;
        double decayRate;
        
        void decay(double dt) {
            age += dt;
            strength *= std::exp(-decayRate * dt);
        }
    };
    
    std::vector<MemoryTrace> customStorage_;
};

} // namespace nlm

#endif // NLM_CUSTOM_MEMORY_HPP
```

### Adding New Neuron Types

**Example: Custom Neuron Type:**
```cpp
// neurons/CustomNeuron.hpp
#ifndef NLM_CUSTOM_NEURON_HPP
#define NLM_CUSTOM_NEURON_HPP

#include "brain/neurons/Neuron.hpp"

namespace nlm {

class CustomNeuron : public Neuron {
public:
    CustomNeuron() : Neuron() {
        setType(NeuronType::Internal);  // Or create new type
    }
    
    void step(double dt) override {
        // Custom LIF dynamics
        updateMembranePotential(dt);
        
        if (getMembranePotential() >= getThreshold()) {
            spike();
            resetMembranePotential();
        }
    }
    
    void setCustomParameter(double value) {
        customParameter_ = value;
    }
    
    double getCustomParameter() const {
        return customParameter_;
    }
    
protected:
    void updateMembranePotential(double dt) override {
        // Custom dynamics implementation
        double alpha = dt / getMembraneTimeConstant();
        double I_total = getTotalSynapticInput() + getExternalInput();
        
        double v_new = getMembranePotential() + 
                      alpha * (I_total - (getMembranePotential() - getRestPotential()))
                      + customParameter_ * alpha;  // Custom term
        
        setMembranePotential(v_new);
        
        // Update refractory if needed
        if (isRefractory()) {
            updateRefractoryPeriod(dt);
        }
    }
    
private:
    double customParameter_ = 0.0;
};

} // namespace nlm

#endif // NLM_CUSTOM_NEURON_HPP
```

### Adding New Environment Types

**Example: Custom Environment:**
```cpp
// environment/CustomEnvironment.hpp
#ifndef NLM_CUSTOM_ENVIRONMENT_HPP
#define NLM_CUSTOM_ENVIRONMENT_HPP

#include "environment/World.hpp"

namespace nlm {

class CustomEnvironment : public World {
public:
    CustomEnvironment() : World() {}
    
    void configure(const WorldConfig& config) override {
        World::configure(config);
        
        // Custom environment setup
        setupCustomFeatures(config);
    }
    
    void update(double dt) override {
        World::update(dt);
        
        // Custom update logic
        applyCustomDynamics(dt);
    }
    
    SensoryInput getSensoryPercept() const override {
        SensoryInput percept = World::getSensoryPercept();
        
        // Add custom sensory features
        addCustomSensoryFeatures(pert percept);
        
        return percept;
    }
    
    void applyMotorCommand(const Action& action, double timestep) override {
        World::applyMotorCommand(action, timestep);
        
        // Custom action effects
        applyCustomActionEffects(action, timestep);
    }
    
protected:
    virtual void setupCustomFeatures(const WorldConfig& config) = 0;
    virtual void applyCustomDynamics(double dt) = 0;
    virtual void addCustomSensoryFeatures(SensoryInput& percept) = 0;
    virtual void applyCustomActionEffects(const Action& action, double timestep) = 0;
};

} // namespace nlm

#endif // NLM_CUSTOM_ENVIRONMENT_HPP
```

## Development Tools

### Build Tools

**CMake Configuration:**
```bash
# Configure with specific options
cmake .. -DNLM_BUILD_EXAMPLES=ON \
          -DNLM_BUILD_TESTS=ON \
          -DNLM_ENABLE_SSE=ON \
          -DNLM_ENABLE_AVX=ON \
          -DNLM_ENABLE_OPENMP=ON

# Build with custom compiler
cmake .. -DCMAKE_CXX_COMPILER=clang++

# Generate documentation
cmake .. -DNLM_DOCUMENTATION=ON
```

**Python Development:**
```bash
# Install in development mode
pip install -e .[dev]

# Install with specific dependencies
pip install -e .[cuda,openmp,tests]
```

### Testing Tools

**Run Test Suite:**
```bash
# Run all tests
cmake --build build --target test

# Run specific test
./nlm_test --gtest_filter="NeuronTest.*"

# Run with verbose output
./nlm_test --gtest_list_tests
```

**Test Coverage:**
```bash
# Install coverage tools
pip install coverage pytest-cov

# Run tests with coverage
python -m pytest --cov=src --cov-report=html

# Generate coverage report
open htmlcov/index.html
```

### Performance Tools

**Benchmark Suite:**
```bash
# Run performance benchmarks
./nlm_benchmark --benchmark-file benchmark_results.json

# Profile with GNU time
/usr/bin/time -v ./nlm_simulation --steps 10000

# Profile with Intel VTune (if available)
amplxe-cl -collect hotspots --result-dir vtune_results ./nlm_simulation
```

### Documentation Tools

**Generate Documentation:**
```bash
# Generate Doxygen documentation
make doc

# Generate Python documentation
pdoc --output-dir docs/api src/nlm

# Generate markdown documentation
python scripts/generate_docs.py --format markdown
```

### Code Quality Tools

**Static Analysis:**
```bash
# Run clang-tidy for C++
clang-tidy src/ --config-file .clang-tidy

# Run clang-format for C++
find src/ -name "*.cpp" -o -name "*.hpp" -o -name "*.h" |
  xargs clang-format --style=file -i

# Run cppcheck for additional checks
cppcheck --enable=all --suppress=missingIncludeSystem src/
```

**Python Code Quality:**
```bash
# Run flake8 for style checking
flake8 src/python/

# Run mypy for type checking
mypy src/python/

# Run black for formatting
black src/python/ --check
```

### Development Scripts

**Common Development Scripts:**
```bash
# scripts/lint.sh - Run all linters
source scripts/lint.sh

# scripts/test.sh - Run all tests
source scripts/test.sh

# scripts/benchmark.sh - Run performance benchmarks
source scripts/benchmark.sh

# scripts/docs.sh - Generate documentation
source scripts/docs.sh

# scripts/validate_config.sh - Validate configuration
source scripts/validate_config.sh
```

**Configuration Examples:**
```bash
# scripts/validate_config.sh
#!/bin/bash

# Validate configuration file
CONFIG_FILE="$1"

if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Configuration file '$CONFIG_FILE' not found"
    exit 1
fi

# Validate JSON configuration
if [[ "$CONFIG_FILE" == *.json ]]; then
    python3 -m json.tool "$CONFIG_FILE" > /dev/null
    if [ $? -ne 0 ]; then
        echo "Error: Invalid JSON in '$CONFIG_FILE'"
        exit 1
    fi
fi

# Validate YAML configuration
if [[ "$CONFIG_FILE" == *.yaml || "$CONFIG_FILE" == *.yml ]]; then
    python3 -c "import yaml; yaml.safe_load(open('$CONFIG_FILE'))" > /dev/null
    if [ $? -ne 0 ]; then
        echo "Error: Invalid YAML in '$CONFIG_FILE'"
        exit 1
    fi
fi

# Validate INI configuration
if [[ "$CONFIG_FILE" == *.ini || "$CONFIG_FILE" == *.cfg ]]; then
    # Basic validation for INI format
    if ! grep -q "^\[" "$CONFIG_FILE" > /dev/null; then
        echo "Error: Invalid INI format in '$CONFIG_FILE'"
        exit 1
    fi
fi

echo "Configuration file '$CONFIG_FILE' is valid"
```

## Development Environment

### Docker Development Environment

**Dockerfile for Development:**
```dockerfile
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    gcc \
    git \
    python3 \
    python3-pip \
    python3-venv \
    build-essential \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
COPY pyproject.toml .
RUN pip3 install -e .[dev]

# Copy source code
COPY . /workspace/nlm
WORKDIR /workspace/nlm

# Build
RUN mkdir -p build && cd build && cmake .. && make -j4

# Run tests
CMD ["ctest", "--output-on-failure"]
```

**Docker Compose for Development:**
```yaml
docker-compose.yml
docker-compose.dev.yml
version: '3.8'

services:
  nlm-dev:
    build: .
    volumes:
      - .:/workspace/nlm
      - ./build:/workspace/nlm/build
    working_dir: /workspace/nlm
    command: >
      bash -c "
        mkdir -p build && 
        cd build && 
        cmake .. -DCMAKE_BUILD_TYPE=Debug && 
        make -j4
      "
    environment:
      - NLM_CONFIG_PATH=./config.json
      - NLM_LOG_LEVEL=DEBUG
    ports:
      - "8080:8080"
```

### Cloud Development Setup

**AWS Cloud Development:**
```bash
# Install AWS CLI
apt-get install -y awscli

# Configure AWS
cd /workspace/nlm
source scripts/aws_setup.sh

# Create EC2 instance for development
aws ec2 run-instances \
    --image-id ami-0c02fb55956c7d316 \
    --instance-type t3.large \
    --key-name nlm-dev \
    --security-group-ids sg-xxxxxxxxx \
    --subnet-id subnet-xxxxxxxxx
```

### Version Control

**Git Configuration:**
```bash
# Set up git user
git config user.name "NLM Developer"
git config user.email "dev@nlm-project.org"
git config core.editor "vim"

# Configure git hooks
git config core.hooksPath .githooks
```

**Git Hooks Example (`.githooks/pre-commit`):**
```bash
#!/bin/bash
# Pre-commit hook for NLM development

# Run linting
make lint || exit 1

# Run unit tests
make test || exit 1

# Check formatting
make format || exit 1

# Validate documentation
make docs || exit 1

echo "Pre-commit checks passed"
```

## Learning and Resources

### Learning Resources

**Books and Papers:**
- "Computational Neuroscience" - Dayan & Abbott
- "Theoretical Neuroscience" - Dayan & Abbott
- "Neural Engineering: Computation, Representation, and Dynamics in the Brain" - Bokil & Gray
- "How to Think Like a Neuron" - R. P. Niven

**Online Courses:**
- Coursera: Computational Neuroscience (University of Washington)
- edX: Neural Networks and Deep Learning (MIT)
- Udacity: Introduction to Autonomous Mobile Robots

**Documentation:**
- [Doxygen Documentation](docs/api/html/index.html)
- [Python API Reference](docs/api/python)
- [Performance Optimization Guide](docs/Performance-Guide.md)
- [Configuration Reference](docs/Configuration.md)

### Research Projects

**Started as Experiments:**
- SimpleWorld Environment
- Basic AgentBrain Interface
- Phase 3 World Interaction
- Phase 4 Cognitive Experiments

**Community Contributions:**
- [NLM Research Repository](https://github.com/nlm-project/research)
- [NLM Extensions](https://github.com/nlm-project/extensions)
- [NLM Examples](https://github.com/nlm-project/examples)

### Development Forums

**Community Support:**
- **Slack:** #nlm-development
- **Discourse:** forums.nlm-project.org
- **Stack Overflow:** Use #nlm-tag
- **GitHub Issues:** [Project Issues](https://github.com/nlm-project/nlm/issues)

**Contact Information:**
- **Development Team:** dev@nlm-project.org
- **Bug Reports:** bugs@nlm-project.org
- **Feature Requests:** features@nlm-project.org
- **Documentation:** docs@nlm-project.org

## Development Roadmap

### Next Development Phase

**Immediate Goals (Next 3 Months):**
1. Complete Phase 6 integration
2. Implement full memory system integration
3. Add advanced cognitive capabilities
4. Improve performance benchmarks
5. Enhance documentation

**Medium-term Goals (6-12 Months):**
1. Add GPU acceleration support
2. Implement distributed computing
3. Add reinforcement learning integration
4. Create web interface for simulation control
5. Establish performance benchmarks

**Long-term Goals (1-2 Years):**
1. Scale to million-neuron simulations
2. Add neuroscientific validation frameworks
3. Create educational tools
4. Establish research collaborations
5. Develop commercial applications

## Development Acknowledgments

**Key Contributors:**
- NLM Core Team
- Open Source Contributors
- Research Collaborators
- Funding Agencies

**Funding Acknowledgments:**
- National Science Foundation (NSF)
- Defense Advanced Research Projects Agency (DARPA)
- National Institutes of Health (NIH)
- Private Research Foundations

**License Acknowledgments:**
- This project uses components from:
  - Boost Libraries (BSL-1.0)
  - pybind11 (BSD 3-Clause)
  - nlohmann_json (MIT)
  - CLI11 (BSD 3-Clause)

## Related Documentation

- See [ARCHITECTURE.md](ARCHITECTURE.md) for system architecture details
- See [SCIENCE.md](SCIENCE.md) for scientific background
- See [EXPERIMENTS.md](EXPERIMENTS.md) for experiment descriptions
- See [Configuration.md](Configuration.md) for configuration system
- See [API-Guide.md](API-Guide.md) for programming interface
- See [Performance-Guide.md](Performance-Guide.md) for optimization techniques
- See [Troubleshooting.md](Troubleshooting.md) for common issues

## Development Contact

**For development questions or assistance:**
- **GitHub Issues:** [Submit Issue](https://github.com/nlm-project/nlm/issues)
- **Development Slack:** #nlm-development
- **Email:** dev@nlm-project.org
- **Website:** [NLM Documentation](https://docs.nlm-project.org)

**Development Tools Support:**
- **CMake Support:** cmake-help@nlm-project.org
- **Python Support:** python-help@nlm-project.org
- **Performance Support:** perf-help@nlm-project.org

## End of Development Guide

This guide provides comprehensive information for NLM developers. Please contribute to its improvement by submitting documentation updates, adding examples, or suggesting new sections.

_This document is continuously updated as the project evolves._