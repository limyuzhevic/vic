# NLM Developer Guide

## Introduction

This guide provides comprehensive documentation for developers working on the NLM (Neural Learning Machine) project. It covers the development process, coding standards, contribution guidelines, and advanced topics for extending and maintaining the NLM codebase.

## Development Environment Setup

### Prerequisites

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++17 compatible compiler (g++ 9+, clang 12+)
- **Build Tools**: CMake 3.15+
- **Package Manager**: Git, make, pip
- **Text Editor**: VS Code, CLion, or any IDE with C++ support

### Setting Up the Development Environment

#### 1. Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Configure git for contributions
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Set up remote for pull requests
git remote add upstream https://github.com/your-org/nlm.git
```

#### 2. Install Dependencies

```bash
# Install C++ dependencies
sudo apt-get update
sudo apt-get install build-essential cmake git

# Install Python dependencies
pip install numpy matplotlib pyopengl
pip install pynlm -e python/

# Install development tools
pip install pre-commit black flake8 mypy
```

#### 3. Create Build Environment

```bash
# Create and enter build directory
mkdir build
cd build

# Configure with all optional features
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DNLM_ENABLE_PYTHON=ON \
         -DNLM_ENABLE_VISUALIZATION=ON \
         -DNLM_ENABLE_PERFORMANCE_MONITORING=ON \
         -DNLM_ENABLE_PLUGINS=ON \
         -DNLM_ENABLE_TESTS=ON

# Build the project
make -j$(nproc)
```

#### 4. Development Python Environment

```bash
# Install Python development tools
cd python
pip install -e .
pip install pytest pytest-cov black flake8 mypy
pip install scikit-build-core

# Install additional Python dependencies
cd ..
```

## Coding Standards

### C++ Coding Standards

#### File Structure

```cpp
// Header files (.hpp)
// - Class declarations
// - Public API
// - Type definitions
// - Constants
// - Inline functions

// Source files (.cpp)
// - Function definitions
// - Implementation details
// - Helper functions
// - Static member definitions
```

#### Naming Conventions

```cpp
// Classes and Structs
class NeuralNetwork { ... }  // PascalCase
struct NeuronState { ... }   // PascalCase

// Functions and Methods
void initialize() { ... }    // camelCase
int getNeuronCount() const { ... }  // camelCase

// Variables
int neuron_count = 100;      // snake_case for constants
int neuronId = 42;            // camelCase for variables

// Constants
static constexpr float PI = 3.14159f;  // UPPER_SNAKE_CASE
```

#### Comment Style

```cpp
// Block comments for class/function documentation
/**
 * Class representing a neural network with advanced features.
 * 
 * This class provides a comprehensive interface for neural network simulation,
 * including support for plasticity, neuromodulation, and visualization.
 * 
 * @note Thread-safe implementation with mutex protection.
 * @warning Experimental implementation subject to change.
 */
class NeuralNetwork {
private:
    /** Neural network state and configuration */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /** Mutex for thread safety */
    mutable std::mutex mutex_;
public:
    /**
     * Initialize the neural network with given configuration.
     * 
     * @param config Configuration parameters for network initialization.
     * @throws ConfigurationError if configuration is invalid.
     * @complexity O(1) for basic initialization, O(n) for complex networks.
     */
    void initialize(const Config& config);
    
    /** Check if the network is currently initialized. */
    bool isInitialized() const noexcept;
    
    // ... other methods
};
```

### Python Coding Standards

#### PEP 8 Compliance

```python
# Use black for code formatting
black --check .
black --diff .

# Use flake8 for linting
flake8 .

# Use mypy for type checking
mypy .
```

#### Docstring Standards

```python
"""Module docstring (single line if possible, otherwise multi-line).

This module provides comprehensive functionality for neural network simulation.

Typical usage:
    >>> brain = Brain()
    >>> brain.initialize()
    >>> brain.step(0)

Attributes:
    DEFAULT_CONFIG (AdvancedConfig): Default configuration for new simulations.
    FEATURE_DETECTOR (FeatureDetector): Global feature detection instance.

See Also:
    nlm_py: Core C++ bindings module
    nlm_utils: Additional Python utilities

Raises:
    ConfigurationError: If configuration is invalid.
    SimulationError: If simulation fails.
"""

class Brain:
    """Neural network brain with advanced simulation capabilities.
    
    This class provides a high-level interface for neural network simulation,
    including support for plasticity, neuromodulation, and visualization.
    
    Args:
        config (AdvancedConfig): Configuration parameters for the brain.
            If None, uses default configuration.
        
    Attributes:
        is_initialized (bool): Whether the brain is initialized.
        performance_monitor (PerformanceMonitor): Performance monitoring instance.
        statistics_collector (StatisticsCollector): Statistics collection instance.
    
    Examples:
        >>> brain = Brain()
        >>> brain.initialize()
        >>> brain.step(0)
        >>> print(brain.getAverageFiringRate())
        
        >>> config = AdvancedConfig()
        >>> config.max_steps = 1000
        >>> brain = Brain(config)
        >>> brain.initialize()
    
    Note:
        This class is thread-safe for concurrent access.
        
    See Also:
        nlm_py.AdvancedBrain: Enhanced brain interface with additional features.
        ConfigBuilder: Utility class for creating common configurations.
    
    Warning:
        Experimental implementation subject to change.
    """
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        """Initialize the brain with configuration.
        
        Args:
            config: Configuration parameters. Uses default if None.
            
        Raises:
            ConfigurationError: If configuration is invalid.
        """
        pass
    
    def initialize(self) -> bool:
        """Initialize the brain.
        
        Returns:
            bool: True if initialization succeeded, False otherwise.
            
        Raises:
            ConfigurationError: If configuration is invalid.
            SimulationError: If initialization fails.
        """
        pass
```

## Build System

### CMake Configuration

#### Standard Build Options

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### Optional Features

```bash
# Enable Python bindings
cmake .. -DNLM_ENABLE_PYTHON=ON

# Enable visualization
cmake .. -DNLM_ENABLE_VISUALIZATION=ON

# Enable performance monitoring
cmake .. -DNLM_ENABLE_PERFORMANCE_MONITORING=ON

# Enable plugin system
cmake .. -DNLM_ENABLE_PLUGINS=ON

# Enable tests
cmake .. -DNLM_ENABLE_TESTS=ON

# Custom build type
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

#### Build Targets

```bash
# Build all targets
make all

# Build specific target
make nlm_phase6_demo

# Build Python bindings
make python_bindings

# Install to system
make install

# Run tests
ctest

# Run all tests with verbose output
ctest -V
```

### Python Build

#### Building Python Extensions

```bash
# Build Python extensions
cd python
scikit-build-core setup.py build_ext --inplace

# Install as editable package
pip install -e .

# Test installation
python -c "import nlm_py; print(nlm_py.__version__)"
```

#### Python Development Setup

```bash
# Create virtual environment
cd nlm
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r python/requirements.txt
pip install -e python/

# Install development tools
pip install pytest pytest-cov black flake8 mypy pre-commit
```

## Testing

### Running Tests

#### C++ Tests

```bash
# Run C++ tests
cd build
ctest

# Run specific test
test_name

# Run all tests with verbose output
ctest -V

# Run tests with coverage
ctest --output-on-failure --coverage
```

#### Python Tests

```bash
# Run Python tests
cd python
pytest

# Run specific test file
pytest test_neuron.py

# Run tests with coverage
pytest --cov=nlm_py --cov-report=html

# Run tests with verbose output
pytest -v

# Run tests in specific directory
pytest tests/
```

### Test Structure

#### C++ Test Structure

```cpp
// tests/test_neuron.cpp
#include "../src/brain/Neuron.hpp"
#include <gtest/gtest.h>

class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = std::make_unique<nlm::Neuron>(1);
    }
    
    void TearDown() override {
        neuron.reset();
    }
    
    std::unique_ptr<nlm::Neuron> neuron;
};

TEST_F(NeuronTest, InitialState) {
    EXPECT_EQ(neuron->getId(), 1);
    EXPECT_EQ(neuron->getType(), nlm::NeuronType::Internal);
}

TEST_F(NeuronTest, StepLIF) {
    neuron->initializeRandom(random_generator);
    bool fired = neuron->stepLIF(0.0, 0.001f);
    // Verify results
}
```

#### Python Test Structure

```python
# python/tests/test_neuron.py
import pytest
import nlm_py
from nlm_py import NeuronId, createNeuronId

class TestNeuron:
    def setup_method(self):
        self.neuron_id = createNeuronId(42)
        # Initialize brain if needed
        self.brain = nlm_py.createBrain()
    
    def teardown_method(self):
        self.brain = None
    
    def test_neuron_creation(self):
        assert self.neuron_id.index() == 42
    
    def test_neuron_properties(self):
        # Test neuron properties
        pass
    
    def test_neuron_step(self):
        # Test neuron stepping
        pass
```

### Performance Testing

#### Performance Benchmarking

```cpp
// src/benchmark.cpp
#include <benchmark/benchmark.h>
#include "brain/Neuron.hpp"

static void NeuronStepBenchmark(benchmark::State& state) {
    nlm::Neuron neuron(1);
    neuron.initializeRandom(random_generator);
    
    for (auto _ : state) {
        neuron.stepLIF(0.0, 0.001f);
    }
}

BENCHMARK(NeuronStepBenchmark);

BENCHMARK_MAIN();
```

#### Python Performance Profiling

```python
# python/performance_test.py
import time
import nlm_py
from nlm_py import Profiler

def performance_test():
    brain = nlm_py.createBrain()
    brain.initialize()
    
    # Profile performance
    with Profiler("simulation") as profiler:
        for step in range(1000):
            brain.step(step)
            profiler.record_frame()
    
    # Get performance metrics
    metrics = brain.getPerformanceMonitor().getMetrics()
    print(f"FPS: {metrics.fps}")
    print(f"Memory usage: {metrics.memory_usage} MB")
    
    # Print profiling results
    profiler.printResults()
    profiler.exportToFile("performance_results.json")

if __name__ == "__main__":
    performance_test()
```

## Extending NLM

### Creating Plugins

#### Plugin Architecture

```cpp
// plugins/novelty_plugin.hpp
#pragma once
#include "brain/Brain.hpp"

class NoveltyPlugin {
public:
    virtual ~NoveltyPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool load(Brain* brain) = 0;
    virtual bool initialize() = 0;
    virtual void step(int step) = 0;
    virtual void shutdown() = 0;
    virtual std::unordered_map<std::string, std::string> getStatistics() const = 0;
};

// Implementation example
class NoveltyPluginImpl : public NoveltyPlugin {
public:
    std::string getName() const override { return "NoveltyPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    bool load(Brain* brain) override { /* Load plugin */ }
    // ... other methods
};
```

#### Python Plugin Interface

```python
# python/plugins/novelty_plugin.py
class NoveltyPlugin:
    """Plugin for novelty detection and surprise calculation."""
    
    def __init__(self):
        self.threshold = 0.5
        self.sensitivity = 1.0
        self.observations = []
    
    def get_name(self):
        """Get plugin name."""
        return "NoveltyPlugin"
    
    def get_version(self):
        """Get plugin version."""
        return "1.0.0"
    
    def load(self, brain):
        """Load plugin with brain reference."""
        self.brain = brain
        return True
    
    def initialize(self, config):
        """Initialize plugin with configuration."""
        self.threshold = config.get("threshold", 0.5)
        self.sensitivity = config.get("sensitivity", 1.0)
        return True
    
    def step(self, step):
        """Process step with plugin logic."""
        # Calculate novelty
        novelty = self.calculate_novelty(step)
        self.observations.append((step, novelty))
        
        # Trigger if threshold exceeded
        if novelty > self.threshold:
            self.on_novelty_detected(novelty, step)
    
    def shutdown(self):
        """Shutdown plugin."""
        self.observations.clear()
        self.brain = None
    
    def get_statistics(self):
        """Get plugin statistics."""
        stats = {
            "name": self.get_name(),
            "version": self.get_version(),
            "observations_count": len(self.observations),
            "average_novelty": self.calculate_average_novelty(),
            "max_novelty": self.calculate_max_novelty(),
            "threshold": self.threshold,
            "sensitivity": self.sensitivity
        }
        return stats
    
    def calculate_novelty(self, step):
        """Calculate novelty for given step."""
        # Implementation-specific novelty calculation
        return 0.0
    
    def calculate_average_novelty(self):
        """Calculate average novelty."""
        if not self.observations:
            return 0.0
        total = sum(obs[1] for obs in self.observations)
        return total / len(self.observations)
    
    def calculate_max_novelty(self):
        """Calculate maximum novelty."""
        if not self.observations:
            return 0.0
        return max(obs[1] for obs in self.observations)
    
    def on_novelty_detected(self, novelty, step):
        """Handle novelty detection event."""
        # Implementation-specific handling
        pass
```

### Creating Custom Brain Classes

#### Extending AdvancedBrain

```cpp
// src/custom_brain.hpp
#pragma once
#include "nlm_py.cpp"

class CustomBrain : public nlm::AdvancedBrain {
public:
    CustomBrain(const nlm::AdvancedConfig& config)
        : AdvancedBrain(nullptr, config) {}  // Will be initialized with proper brain
    
    // Custom methods
    void customInitialization() {
        // Custom initialization logic
    }
    
    void customProcessing(int step) {
        // Custom processing logic
        AdvancedBrain::step(step);
        // Additional custom processing
    }
    
protected:
    // Override virtual methods for custom behavior
    bool onStep(int step) override {
        // Custom pre-processing
        customProcessing(step);
        return true;
    }
};
```

#### Python Brain Extension

```python
# python/custom_brain.py
import nlm_py
from nlm_py import AdvancedBrain, AdvancedConfig

class CustomBrain(AdvancedBrain):
    """Custom brain class with extended functionality."""
    
    def __init__(self, config=None):
        """Initialize custom brain."""
        if config is None:
            config = AdvancedConfig()
        super().__init__(config)
        self.custom_metrics = {}
    
    def custom_initialization(self):
        """Custom initialization logic."""
        # Additional initialization steps
        pass
    
    def custom_processing(self, step):
        """Custom processing logic."""
        # Custom processing before parent step
        self.custom_metrics[f'step_{step}'] = step
        
        # Call parent processing
        super().step(step)
        
        # Custom processing after parent step
        self.update_custom_metrics(step)
    
    def step(self, step):
        """Override step method with custom logic."""
        if not self.isInitialized():
            raise Exception("Brain not initialized")
        
        try:
            self.custom_processing(step)
            
            # Get performance metrics
            perf = self.getPerformanceMonitor().getMetrics()
            
            # Check for custom conditions
            if self.should_custom_action(step, perf):
                self.perform_custom_action(step)
            
        except Exception as e:
            self.handle_custom_error(e, step)
            raise
    
    def update_custom_metrics(self, step):
        """Update custom metrics."""
        metrics = self.getPerformanceMetrics()
        self.custom_metrics[f'step_{step}_fps'] = metrics.fps
        self.custom_metrics[f'step_{step}_memory'] = metrics.memory_usage
    
    def should_custom_action(self, step, metrics):
        """Determine if custom action should be performed."""
        # Custom logic for determining actions
        return False
    
    def perform_custom_action(self, step):
        """Perform custom action."""
        # Custom action implementation
        pass
    
    def handle_custom_error(self, error, step):
        """Handle custom error."""
        # Custom error handling
        pass
    
    def get_custom_statistics(self):
        """Get custom statistics."""
        stats = super().getBrainStatus()
        stats['custom_metrics'] = self.custom_metrics
        return stats
```

### Creating Custom Configurations

#### Configuration Factory

```cpp
// src/config_factory.hpp
#pragma once
#include "core/Config/Config.hpp"

class ConfigFactory {
public:
    static nlm::Config createDefaultConfig() {
        nlm::Config config;
        config.set("simulation.max_steps", 10000);
        config.set("simulation.timestep", 0.1);
        config.set("brain.global_connectivity", 0.1);
        config.set("neuron_count", 100);
        config.set("region_count", 10);
        config.set("plasticity.stdp.enabled", true);
        config.set("plasticity.hebbian.enabled", true);
        config.set("plasticity.structural.enabled", true);
        return config;
    }
    
    static nlm::Config createResearchConfig() {
        nlm::Config config;
        config.set("simulation.max_steps", 50000);
        config.set("simulation.timestep", 0.01);
        config.set("brain.global_connectivity", 0.05);
        config.set("neuron_count", 1000);
        config.set("region_count", 50);
        config.set("plasticity.stdp.enabled", true);
        config.set("plasticity.hebbian.enabled", true);
        config.set("plasticity.structural.enabled", true);
        config.set("plasticity.stdp.learning_rate", 0.005);
        config.set("plasticity.hebbian.learning_rate", 0.002);
        return config;
    }
    
    static nlm::Config createDevelopmentConfig() {
        nlm::Config config;
        config.set("simulation.max_steps", 100);
        config.set("simulation.timestep", 0.05);
        config.set("brain.global_connectivity", 0.2);
        config.set("neuron_count", 10);
        config.set("region_count", 2);
        config.set("plasticity.stdp.enabled", true);
        config.set("plasticity.hebbian.enabled", true);
        config.set("plasticity.structural.enabled", false);
        return config;
    }
};
```

#### Python Configuration Builder

```python
# python/config_builder.py
import nlm_py
from nlm_py import AdvancedConfig, VisualizationConfig, PerformanceMetrics

class ConfigBuilder:
    """Helper class for building NLM configurations."""
    
    @staticmethod
    def create_default_config():
        """Create a default advanced configuration."""
        return AdvancedConfig()
    
    @staticmethod
    def create_research_config():
        """Create a configuration optimized for research."""
        config = AdvancedConfig()
        config.max_steps = 50000
        config.timestep = 0.01
        config.temporal_decay = 0.99
        config.spatial_decay = 0.9
        config.global_connectivity = 0.05
        config.enable_stdp = True
        config.enable_hebbian = True
        config.enable_structural = True
        config.stdp_learning_rate = 0.005
        config.hebbian_learning_rate = 0.002
        config.enable_reward_modulation = True
        config.enable_curiosity = True
        config.enable_prediction_error = True
        config.enable_development = True
        config.development_rate = 0.0005
        config.enable_working_memory = True
        config.enable_episodic_memory = True
        config.memory_capacity = 5000
        config.enable_performance_monitoring = True
        config.enable_profiling = True
        config.profile_interval = 0.5
        config.enable_visualization = False
        return config
    
    @staticmethod
    def create_development_config():
        """Create a configuration optimized for development/testing."""
        config = AdvancedConfig()
        config.max_steps = 1000
        config.timestep = 0.05
        config.temporal_decay = 0.9
        config.spatial_decay = 0.7
        config.global_connectivity = 0.2
        config.enable_stdp = True
        config.enable_hebbian = True
        config.enable_structural = False
        config.stdp_learning_rate = 0.01
        config.hebbian_learning_rate = 0.005
        config.enable_reward_modulation = False
        config.enable_curiosity = True
        config.enable_prediction_error = False
        config.enable_development = False
        config.enable_working_memory = False
        config.enable_episodic_memory = False
        config.memory_capacity = 100
        config.enable_performance_monitoring = False
        config.enable_profiling = False
        config.enable_visualization = False
        return config
    
    @staticmethod
    def create_visualization_config(window_width=1920, window_height=1080):
        """Create a configuration for visualization."""
        config = AdvancedConfig()
        config.enable_visualization = True
        config.visualization_config.window_width = window_width
        config.visualization_config.window_height = window_height
        config.visualization_config.enable_3d = True
        config.visualization_config.show_neurons = True
        config.visualization_config.show_synapses = True
        config.visualization_config.show_world = True
        config.visualization_config.refresh_rate = 60.0
        return config
    
    @staticmethod
    def create_config_from_preset(preset):
        """Create configuration from preset name."""
        presets = {
            'default': ConfigBuilder.create_default_config,
            'research': ConfigBuilder.create_research_config,
            'development': ConfigBuilder.create_development_config,
            'visualization': ConfigBuilder.create_visualization_config,
        }
        
        if preset not in presets:
            raise ValueError(f"Unknown preset: {preset}")
        
        return presets[preset]()
```

## Code Quality and Maintenance

### Linting and Formatting

#### Pre-commit Hooks

```bash
# Install pre-commit hooks
pre-commit install

# Run all pre-commit checks
pre-commit run --all-files

# Fix formatting issues
black .

# Check code style
flake8 .

# Check type hints
mypy .
```

#### Git Hooks

```bash
# Create pre-commit configuration
.cat > .pre-commit-config.yaml << EOF
cmds:
  - repo: local
    hooks:
      - id: black
        name: black
        entry: black
        language: system
        files: \.py$
        types: [python]
      
      - id: flake8
        name: flake8
        entry: flake8
        language: system
        files: \.py$
        types: [python]
      
      - id: mypy
        name: mypy
        entry: mypy
        language: system
        files: \.py$
        types: [python]
      
      - id: cmake-format
        name: cmake-format
        entry: cmake-format
        language: system
        files: CMakeLists\.txt$
        types: [file]
EOF

# Install git hooks
pre-commit install
```

### Documentation Generation

#### Doxygen Documentation

```cpp
/**
 * @mainpage NLM Documentation
 * 
 * @section intro Introduction
 * NLM (Neural Learning Machine) is an experimental artificial brain project.
 * 
 * @section files Files
 * - src/brain/Neuron.cpp: Neuron implementation
 * - src/memory/NeuralWorkingMemory.cpp: Memory system implementation
 * - src/core/Config/Config.cpp: Configuration system implementation
 * 
 * @section examples Examples
 * - examples/basic_simulation.cpp: Basic simulation example
 * - examples/plugin_example.cpp: Plugin development example
 * 
 * @section api API Reference
 * - @ref nlm namespace
 * - @ref Neuron class
 * - @ref Config class
 * 
 * @section tutorial Tutorial
 * 1. [Getting Started](docs/GETTING_STARTED.md)
 * 2. [Building from Source](docs/BUILDING.md)
 * 3. [Running Simulations](docs/USING.md)
 * 4. [Extending NLM](docs/EXTENDING.md)
 */
```

#### Python Documentation

```python
# python/docs/conf.py
sphinx.ext.autodoc = True
sphinx.ext.viewcode = True
sphinx.ext.napoleon = True

# -- Project information ----------------------------------------------------
project = 'NLM Python Bindings'
copyright = '2024, NLM Team'
author = 'NLM Team'

# -- General configuration ---------------------------------------------------
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.viewcode',
    'sphinx.ext.napoleon',
    'sphinx.ext.mathjax',
]

# -- Options for HTML output -------------------------------------------------
html_theme = 'sphinx_rtd_theme'
```

### Version Control

#### Git Workflow

```bash
# Clone the repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Create feature branch
git checkout -b feature/new-plugin

# Make changes
# ... edit files, commit changes

git add new_file.cpp
 git commit -m "Add new plugin implementation"

# Push to remote branch
git push origin feature/new-plugin

# Create pull request
# ... describe changes, request review
```

#### Commit Message Guidelines

```
<type>(<scope>): <description>

[optional body]

[optional footer]

Types:
- feat: New feature
- fix: Bug fix
- docs: Documentation changes
- style: Code style changes (formatting, etc.)
- refactor: Code refactoring
- perf: Performance improvements
- test: Test additions or fixes
- build: Build system changes
- ci: CI/CD changes
- chore: Maintenance tasks

Scopes:
- neuron: Neuron-related changes
- memory: Memory system changes
- config: Configuration system changes
- python: Python bindings changes
- plugin: Plugin system changes
- visualization: Visualization system changes
- performance: Performance monitoring changes
```

## Advanced Topics

### Concurrency and Threading

#### Thread-Safe Operations

```cpp
class ThreadSafeBrain {
private:
    std::mutex mutex_;
    nlm::Brain brain_;
    
public:
    void safeInitialize(const nlm::Config& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        brain_.initialize(config);
    }
    
    void safeStep(int step) {
        std::lock_guard<std::mutex> lock(mutex_);
        brain_.step(step);
    }
    
    nlm::Config safeGetConfig() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return brain_.getConfig();
    }
};
```

#### C++ Concurrency Features

```cpp
// Thread pool for parallel processing
class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
    
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex_);
                        this->condition_.wait(lock, [this] { 
                            return this->stop_ || !this->tasks_.empty(); 
                        });
                        
                        if (this->stop_ && this->tasks_.empty()) {
                            return;
                        }
                        
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task();
                }
            });
        }
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread &worker : workers_) {
            worker.join();
        }
    }
    
    template<class F>
    void enqueue(F &&f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace(std::forward<F>(f));
        }
        condition_.notify_one();
    }
};
```

### Memory Management

#### Smart Pointer Usage

```cpp
// Memory management with smart pointers
class MemoryEfficientBrain {
private:
    // Use unique_ptr for exclusive ownership
    std::unique_ptr<nlm::Brain> brain_ptr_;
    
    // Use shared_ptr for shared ownership
    std::shared_ptr<nlm::Config> config_ptr_;
    
    // Use weak_ptr for observer references
    std::weak_ptr<nlm::Brain> weak_brain_ptr_;
    
public:
    MemoryEfficientBrain(const nlm::Config& config) {
        // Use make_unique for automatic memory management
        brain_ptr_ = std::make_unique<nlm::Brain>(config);
        config_ptr_ = std::make_shared<nlm::Config>(config);
        weak_brain_ptr_ = brain_ptr_;  // Shared ownership
    }
    
    void step(int step) {
        // Check if brain is still valid
        if (auto brain = weak_brain_ptr_.lock()) {
            brain->step(step);
        } else {
            throw std::runtime_error("Brain has been destroyed");
        }
    }
    
    // Use custom deleter for complex cleanup
    void customCleanup() {
        // Custom cleanup logic
        brain_ptr_.reset();
    }
};
```

### Performance Optimization

#### SIMD Vectorization

```cpp
// SIMD-optimized neuron computation
class SIMDNeuronProcessor {
public:
    static void processNeuronsSIMD(
        std::vector<float>& membrane_potentials,
        const std::vector<float>& synaptic_inputs,
        float timestep
    ) {
        // Use SIMD for parallel processing
        size_t simd_width = 8;  // AVX2
        size_t simd_size = membrane_potentials.size() / simd_width * simd_width;
        
        for (size_t i = 0; i < simd_size; i += simd_width) {
            // Load values into SIMD registers
            __m256 V = _mm256_loadu_ps(&membrane_potentials[i]);
            __m256 I = _mm256_loadu_ps(&synaptic_inputs[i]);
            __m256 dt = _mm256_set1_ps(timestep);
            
            // Compute with SIMD
            // V = V + dt * (I/C - V/tau)
            __m256 result = V + dt * (I / Impl::MEMBRANE_CAPACITANCE - V / Impl::TIME_CONSTANT);
            
            // Store result back
            _mm256_storeu_ps(&membrane_potentials[i], result);
        }
        
        // Process remaining elements with scalar code
        for (size_t i = simd_size; i < membrane_potentials.size(); ++i) {
            membrane_potentials[i] += timestep * (synaptic_inputs[i] / Impl::MEMBRANE_CAPACITANCE - 
                                                 membrane_potentials[i] / Impl::TIME_CONSTANT);
        }
    }
};
```

### Error Handling and Recovery

#### Exception Safety

```cpp
class FaultTolerantBrain {
private:
    nlm::Brain brain_;
    bool is_healthy_;
    
public:
    void initialize(const nlm::Config& config) {
        try {
            brain_.initialize(config);
            is_healthy_ = true;
        } catch (const std::exception& e) {
            is_healthy_ = false;
            throw;
        }
    }
    
    void step(int step) {
        if (!is_healthy_) {
            throw std::runtime_error("Brain is not healthy");
        }
        
        try {
            brain_.step(step);
        } catch (const std::exception& e) {
            // Attempt recovery
            if (!recoverFromError(e)) {
                throw;
            }
        }
    }
    
    bool recoverFromError(const std::exception& e) {
        // Recovery logic
        std::string error_msg = e.what();
        
        if (error_msg.find("memory") != std::string::npos) {
            // Memory error - attempt cleanup
            cleanupMemory();
            return true;
        } else if (error_msg.find("file") != std::string::npos) {
            // File error - retry with backup
            return retryWithBackup();
        }
        
        return false;
    }
    
    void cleanupMemory() {
        // Cleanup memory allocations
        brain_.reset();
    }
    
    bool retryWithBackup() {
        // Retry with backup configuration
        nlm::Config backup_config = getBackupConfig();
        try {
            brain_.initialize(backup_config);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    nlm::Config getBackupConfig() const {
        // Return backup configuration
        nlm::Config config;
        config.set("simulation.max_steps", 100);
        config.set("brain.global_connectivity", 0.1);
        return config;
    }
};
```

### Debugging and Testing

#### Debugging with GDB

```bash
# Start GDB with core files
gdb ./nlm_phase6_demo
coredump limit unlimited
run

# Examine variable values
(pdb) print brain.getTotalNeuronCount()
(pdb) print brain.getAverageFiringRate()

# Set breakpoints
(pdb) break Neuron::stepLIF
(pdb) run

# Continue execution
(pdb) continue

# Examine memory
(pdb) info locals
(pdb) x/20x &neuron_state
```

#### Testing with Valgrind

```bash
# Run with Valgrind for memory leaks
valgrind --leak-check=full --track-origins=yes ./nlm_phase6_demo

# Memory profiling
valgrind --tool=massif --stacks=yes ./nlm_phase6_demo
```

#### Code Coverage

```bash
# Generate coverage report
cd build
ctest --coverage

# Generate HTML coverage report
mkdir -p coverage
coverage analyze --output coverage
open coverage/index.html
```

## Contribution Guidelines

### How to Contribute

#### 1. Report Issues

- Use GitHub Issues to report bugs
- Provide reproduction steps
- Include relevant logs and error messages
- Describe expected vs. actual behavior

#### 2. Fix Bugs

- Fork the repository
- Create a feature branch
- Fix the bug
- Add tests for the fix
- Submit a pull request

#### 3. Add Features

- Fork the repository
- Create a feature branch
- Implement the feature
- Add comprehensive tests
- Update documentation
- Submit a pull request

#### 4. Improve Documentation

- Fork the repository
- Create a feature branch
- Update documentation files
- Add examples
- Submit a pull request

#### 5. Performance Improvements

- Fork the repository
- Create a feature branch
- Implement performance improvements
- Add performance benchmarks
- Update documentation
- Submit a pull request

### Pull Request Process

#### 1. Create Pull Request

```bash
git add modified_files
git commit -m "feat: add new feature with comprehensive description"
git push origin feature-branch

# Open pull request on GitHub
# Add detailed description
# Request review from maintainers
# Wait for CI/CD tests to pass
```

#### 2. Review Process

- **Code Review**: Check code quality and standards
- **Testing**: Ensure all tests pass
- **Documentation**: Verify documentation is complete
- **Performance**: Check for performance regressions
- **Compatibility**: Ensure backward compatibility

#### 3. Merge Process

```bash
# Rebase onto main branch
git checkout main
git pull upstream main
git checkout feature-branch
git rebase main

# Resolve any conflicts
# Run tests again

# Merge with pull request
# Add Co-authored-by: openhands <openhands@all-hands.dev>
```

## Support and Community

### Getting Help

#### Community Forums

- **GitHub Discussions**: Ask questions and share ideas
- **Stack Overflow**: Use `nlm` tag for questions
- **Discord Server**: Join the NLM community
- **Mailing List**: Subscribe for project updates

#### Support Channels

| Channel | Description | Link |
|---------|-------------|------|
| GitHub Issues | Bug reports and feature requests | https://github.com/your-org/nlm/issues |
| Discord | Real-time community chat | https://discord.gg/nlm |
| Slack | Project-specific discussions | https://nlm.slack.com |
| Email | Support email | support@nlm.org |
| Documentation | Complete documentation | docs/ |

### Contributing to Documentation

#### Writing Documentation

```markdown
# Documentation Guidelines

## Structure

1. **Title**: Clear, descriptive title
2. **Overview**: Brief summary of the topic
3. **Prerequisites**: Required knowledge or setup
4. **Installation**: Step-by-step instructions
5. **Usage**: Examples and code snippets
6. **Configuration**: Configuration options
7. **Troubleshooting**: Common issues and solutions
8. **References**: Related documentation

## Code Examples

```cpp
// Use code blocks for examples
class Example {
public:
    void demonstrate() {
        // Example code
    }
};
```

## Image and Diagram Guidelines

- Use SVG format for diagrams
- Include alt text for accessibility
- Keep images under 1MB
- Label all axes and components
```

### Contributing to Code

#### Code Review Checklist

- [ ] Code follows coding standards
- [ ] Tests are comprehensive and pass
- [ ] Documentation is complete
- [ ] Performance is optimized
- [ ] Error handling is robust
- [ ] Thread safety is maintained
- [ ] Memory management is efficient
- [ ] Dependencies are up-to-date

## Conclusion

The NLM project provides a comprehensive framework for neural network simulation with advanced features for research and development. This developer guide provides the tools and knowledge needed to extend, maintain, and contribute to the NLM codebase.

By following these guidelines, developers can:

- Create high-quality, maintainable code
- Add new features efficiently
- Ensure backward compatibility
- Maintain code quality
- Collaborate effectively with the community
- Document their work comprehensively

The NLM project welcomes contributions from all developers interested in creating advanced neural network simulation systems.

## References

- [NLM Documentation](docs/) - Complete project documentation
- [API Reference](https://github.com/your-org/nlm/tree/main/docs/api) - API documentation
- [Code Examples](https://github.com/your-org/nlm/tree/main/examples) - Working examples
- [Contributing Guidelines](CONTRIBUTING.md) - Detailed contribution guidelines
- [Code of Conduct](CODE_OF_CONDUCT.md) - Project code of conduct
- [License](LICENSE) - Project license information
- [Acknowledgments](ACKNOWLEDGMENTS.md) - Project acknowledgments
