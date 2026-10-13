# NLM - Developer Guide

**Date:** 2026-10-13
**Audience:** Developers, architects, maintainers
**Level:** Advanced

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Interface-Based Design](#interface-based-design)
3. [Advanced API Implementation](#advanced-api-implementation)
4. [Building and Installation](#building-and-installation)
5. [Development Tools and Testing](#development-tools-and-testing)
6. [Code Conventions](#code-conventions)
7. [Performance Optimization](#performance-optimization)
8. [Phase 6 Integration](#phase-6-integration)
9. [Contributing Guidelines](#contributing-guidelines)
10. [Troubleshooting Development](#troubleshooting-development)

---

## Architecture Overview

The NLM codebase has been completely transformed with an **interface-based architecture** that supports **dependency injection**, **factory patterns**, and **high-level convenience APIs**. The new architecture maintains full backward compatibility while providing powerful tools for development and research.

### Key Architectural Changes

#### Before (Direct Coupling)
```
AgentBrain → Brain (concrete)
Brain → WorkingMemory, NeuralEpisodicMemory (concrete)
Components → Each other directly
```

#### After (Interface-Based with Dependency Injection)
```
AgentBrain → IBrainInterface (abstract)
IBrainInterface → MemorySystemWrapper → NeuralWorkingMemory
IBrainInterface → NeuromodulationSystemWrapper → Dopamine
IBrainInterface → PredictionSystemWrapper → PredictionSystem
```

### Core Components

1. **Interface Layer**: 7 core interfaces defining contracts
2. **Implementation Layer**: Wrappers around existing components
3. **Dependency Injection**: DI container for flexible configuration
4. **Advanced API**: High-level convenience functions and classes
5. **Python Bindings**: Complete Python integration

### Component Structure

```
/src/
├── interfaces/          # 7 core interfaces
│   ├── IBrainInterface.hpp
│   ├── IComponents.hpp
│   └── DependencyInjector.hpp
├── brain/              # Brain implementation with interface
│   ├── BrainAdapter.hpp
│   └── BrainAdapter.cpp
├── components/         # Component implementations
│   ├── MemoryImpl.hpp
│   ├── InterfaceImplementations.hpp
│   └── Wrappers.hpp
├── advanced_api/       # Advanced API headers
│   ├── advanced_api.hpp
│   ├── simple_agent.hpp
│   ├── simulation_runner.hpp
│   └── ... (5 more)
├── python/             # Python integration
│   ├── advanced_api.py
│   └── __init__.py
└── tests/              # Comprehensive tests
```

## Interface-Based Design

### 1. Core Interfaces

#### IBrainInterface (Main Interface)
```cpp
class IBrainInterface {
public:
    virtual ~IBrainInterface() = default;
    
    // Neural operations
    virtual void step(SimulationStep step, Timestamp time) = 0;
    virtual float getNeuronPotential(int neuronId) const = 0;
    
    // Memory systems
    virtual IWorkingMemory* getWorkingMemory() const = 0;
    virtual IEpisodicMemory* getEpisodicMemory() const = 0;
    virtual ISemanticMemory* getSemanticMemory() const = 0;
    
    // Neuromodulation
    virtual float getDopamineLevel() const = 0;
    virtual float getCuriosityLevel() const = 0;
    virtual float getNoveltyLevel() const = 0;
    
    // Development
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual float getPlasticityRate() const = 0;
    
    // Persistence
    virtual bool save(const std::string& filepath) const = 0;
    virtual bool load(const std::string& filepath) = 0;
};
```

#### IComponents Interface
```cpp
class IComponents {
public:
    virtual ~IComponents() = default;
    
    // Component factories
    virtual std::unique_ptr<INeuronSystem> createNeuronSystem() = 0;
    virtual std::unique_ptr<ISynapticSystem> createSynapticSystem() = 0;
    virtual std::unique_ptr<IPlasticitySystem> createPlasticitySystem() = 0;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
};
```

### 2. Dependency Injection Framework

#### DependencyInjector
```cpp
template<typename T>
class DependencyInjector {
public:
    template<typename U, typename... Args>
    void registerType(std::unique_ptr<U> (*factory)(Args...), Args... args) {
        // Store factory with arguments
    }
    
    template<typename T>
    std::unique_ptr<T> resolve() {
        // Create instance using factory
    }
    
    template<typename T, typename U>
    void addBinding() {
        // Add type binding
    }
};
```

#### DefaultComponentFactory
```cpp
class DefaultComponentFactory : public IComponents {
public:
    std::unique_ptr<INeuronSystem> createNeuronSystem() override {
        return std::make_unique<LIFNeuronSystem>();
    }
    
    std::unique_ptr<ISynapticSystem> createSynapticSystem() override {
        return std::make_unique<SynapticSystemImpl>();
    }
    
    // ... other implementations
};
```

### 3. Factory Pattern

All high-level classes use factory functions:

```cpp
// Factory functions for Advanced API classes
BrainProfiler* createBrainProfiler(IBrainInterface* brain);
SimpleAgent* createSimpleAgent(IBrainInterface* brain, IWorldInterface* world, const std::string& name);
SimulationRunner* createSimulationRunner(size_t numSteps, double dt);
VisualizationHelper* createVisualizationHelper();
ExperimentManager* createExperimentManager();

NeuronActivityMonitor* createNeuronActivityMonitor(IBrainInterface* brain);
NetworkConnectivityAnalyzer* createNetworkConnectivityAnalyzer(IBrainInterface* brain);
MemorySystemProfiler* createMemorySystemProfiler(IBrainInterface* brain);
NeuromodulationAnalyzer* createNeuromodulationAnalyzer(IBrainInterface* brain);
DevelopmentStageAnalyzer* createDevelopmentStageAnalyzer(IBrainInterface* brain);
```

## Advanced API Implementation

### 1. Header Structure

#### advanced_api.hpp (786 lines)
Main advanced API interface containing:

```cpp
#pragma once
#include "advanced_api_types.hpp"
#include "simple_agent.hpp"
#include "simulation_runner.hpp"
#include "brain_profiler.hpp"
#include "visualization_helper.hpp"
#include "experiment_manager.hpp"
#include "neuron_activity_monitor.hpp"
#include "network_connectivity_analyzer.hpp"
#include "memory_system_profiler.hpp"
#include "neuromodulation_analyzer.hpp"
#include "development_stage_analyzer.hpp"

namespace nlm {
namespace advanced {

class AdvancedAPI {
public:
    // High-level convenience functions
    static void runSimulation(const SimulationConfig& config);
    static void runEpisode(SimpleAgent& agent, IWorldInterface& world, size_t steps);
    static void runExperiment(ExperimentManager& manager);
    
    // Factory methods
    static std::unique_ptr<SimpleAgent> createAgent(const BrainConfig& config);
    static std::unique_ptr<SimulationRunner> createRunner(const SimulationConfig& config);
    static std::unique_ptr<BrainProfiler> createProfiler(IBrainInterface* brain);
    
    // Analysis functions
    static BrainAnalysis analyzeBrainActivity(IBrainInterface* brain);
    static NeuralStats computeNeuralStatistics(IBrainInterface* brain);
    
    // Configuration helpers
    static SimulationConfig createDefaultConfig();
    static SimulationConfig loadConfigFromFile(const std::string& filepath);
    static void saveConfigToFile(const SimulationConfig& config, const std::string& filepath);
};

} // namespace advanced
} // namespace nlm
```

#### Component Headers (138-200 lines each)
Each component header follows similar pattern:

```cpp
#pragma once
#include "IBrainInterface.hpp"
#include "advanced_api_types.hpp"

namespace nlm {
namespace advanced {

class BrainProfiler {
public:
    explicit BrainProfiler(IBrainInterface* brain);
    void start();
    void stop();
    BrainProfile getProfile(double duration) const;
    std::string getProfileSummary() const;
    
    // Specific analysis methods
    NeuralDynamicsAnalysis getNeuralDynamics() const;
    MemoryAnalysis getMemoryAnalysis() const;
    NetworkAnalysis getNetworkAnalysis() const;
    
private:
    IBrainInterface* brain_;
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point end_time_;
    std::vector<NeuralSnapshot> snapshots_;
};

} // namespace advanced
} // namespace nlm
```

### 2. Implementation Files

#### advanced_api_utils.cpp (784 lines)
Utility function implementations:

```cpp
#include "advanced_api_utils.hpp"
#include "advanced_api.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {
namespace advanced {

void AdvancedAPIUtils::initializeSimulation(SimulationConfig& config) {
    // Set defaults for missing parameters
    if (!config.has("brain.neuron_count")) {
        config.set("brain.neuron_count", 1000);
    }
    if (!config.has("brain.synapse_density")) {
        config.set("brain.synapse_density", 0.1);
    }
    // ... other defaults
}

BrainAnalysis AdvancedAPIUtils::analyzeBrainData(
    const IBrainInterface* brain,
    double duration
) {
    BrainAnalysis analysis;
    
    // Collect neural data
    analysis.neural_data = extractNeuralData(brain, duration);
    
    // Compute statistics
    analysis.statistics = computeStatistics(analysis.neural_data);
    
    // Analyze connectivity
    analysis.connectivity = analyzeConnectivity(brain);
    
    // Analyze memory
    analysis.memory = analyzeMemory(brain);
    
    return analysis;
}

} // namespace advanced
} // namespace nlm
```

### 3. Python Integration

#### advanced_api.py (14,343 lines)
```python
"""
Advanced API for NLM - Python bindings

High-level convenience functions and class abstractions for complex neural simulations.
"""

import numpy as np
import json
from typing import Dict, List, Optional, Tuple
import pynlm

class AdvancedAPI:
    """High-level interface for NLM simulations."""
    
    def __init__(self, brain=None):
        """Initialize Advanced API with optional brain."""
        self.brain = brain
        self._initialize_tools()
    
    def _initialize_tools(self):
        """Initialize analysis and debugging tools."""
        self.tools = {
            'brain_profiler': BrainProfiler(self.brain) if self.brain else None,
            'neuron_monitor': NeuronActivityMonitor(self.brain) if self.brain else None,
            'network_analyzer': NetworkConnectivityAnalyzer(self.brain) if self.brain else None,
            'memory_profiler': MemorySystemProfiler(self.brain) if self.brain else None,
            'neuromod_analyzer': NeuromodulationAnalyzer(self.brain) if self.brain else None,
            'development_analyzer': DevelopmentStageAnalyzer(self.brain) if self.brain else None,
        }
    
    def run_simulation(self, config=None):
        """Run complete simulation with configuration."""
        if config is None:
            config = self._create_default_config()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Update brain reference
        self.brain = brain
        self._initialize_tools()
        
        # Run simulation
        for step in range(config.get('simulation.steps', 1000)):
            brain.step(step)
            
            # Collect profiling data
            if self.tools['brain_profiler']:
                self.tools['brain_profiler'].start()
                # ... collect data
                self.tools['brain_profiler'].stop()
        
        return brain
    
    def analyze_brain_activity(self, duration=None):
        """Analyze brain activity patterns."""
        if not self.brain:
            raise ValueError("No brain available for analysis")
        
        analysis = {
            'neural_dynamics': self._analyze_neural_dynamics(duration),
            'network_connectivity': self._analyze_network_connectivity(),
            'memory_patterns': self._analyze_memory_patterns(),
            'neuromodulation_effects': self._analyze_neuromodulation(),
            'developmental_changes': self._analyze_development(duration)
        }
        
        return analysis
    
    def create_profiling_session(self, session_name):
        """Create a new profiling session."""
        return ProfilingSession(session_name, self.brain)
    
    def export_results(self, output_path, format='json'):
        """Export simulation results."""
        if format == 'json':
            with open(output_path, 'w') as f:
                json.dump(self._get_export_data(), f, indent=2)
        elif format == 'csv':
            self._export_to_csv(output_path)
        elif format == 'hdf5':
            self._export_to_hdf5(output_path)
        
        return output_path
    
    # Factory functions
    @staticmethod
    def create_brain_profiler(brain):
        """Create brain profiler for analysis."""
        return BrainProfiler(brain)
    
    @staticmethod
    def create_simple_agent(brain, world, name):
        """Create simple agent with brain and world."""
        return SimpleAgent(brain, world, name)
    
    @staticmethod
    def create_simulation_runner(num_steps, dt=0.1):
        """Create simulation runner."""
        return SimulationRunner(num_steps, dt)
    
    @staticmethod
    def create_experiment_manager():
        """Create experiment manager."""
        return ExperimentManager()
    
    @staticmethod
    def create_neuron_activity_monitor(brain):
        """Create neuron activity monitor."""
        return NeuronActivityMonitor(brain)
    
    @staticmethod
    def create_network_connectivity_analyzer(brain):
        """Create network connectivity analyzer."""
        return NetworkConnectivityAnalyzer(brain)
    
    @staticmethod
    def create_memory_system_profiler(brain):
        """Create memory system profiler."""
        return MemorySystemProfiler(brain)
    
    @staticmethod
    def create_neuromodulation_analyzer(brain):
        """Create neuromodulation analyzer."""
        return NeuromodulationAnalyzer(brain)
    
    @staticmethod
    def create_development_stage_analyzer(brain):
        """Create development stage analyzer."""
        return DevelopmentStageAnalyzer(brain)
    
    def _create_default_config(self):
        """Create default simulation configuration."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("simulation.steps", 1000)
        config.set("simulation.dt", 0.1)
        return config
    
    # Helper methods for analysis
    def _analyze_neural_dynamics(self, duration):
        """Analyze neural dynamics."""
        if self.tools['brain_profiler']:
            return self.tools['brain_profiler'].get_neural_dynamics(duration)
        return None
    
    def _analyze_network_connectivity(self):
        """Analyze network connectivity."""
        if self.tools['network_analyzer']:
            return self.tools['network_analyzer'].get_connectivity_analysis()
        return None
    
    def _analyze_memory_patterns(self):
        """Analyze memory patterns."""
        if self.tools['memory_profiler']:
            return self.tools['memory_profiler'].get_memory_analysis()
        return None
    
    def _analyze_neuromodulation(self):
        """Analyze neuromodulation effects."""
        if self.tools['neuromod_analyzer']:
            return self.tools['neuromod_analyzer'].get_modulation_analysis()
        return None
    
    def _analyze_development(self, duration):
        """Analyze developmental changes."""
        if self.tools['development_analyzer']:
            return self.tools['development_analyzer'].get_development_analysis(duration)
        return None
    
    def _get_export_data(self):
        """Get data for export."""
        return {
            'brain_state': self.brain.getBrainState() if self.brain else None,
            'simulation_stats': self.brain.getStatistics() if self.brain else None,
            'analysis_results': self.analyze_brain_activity() if self.brain else None
        }
    
    def _export_to_csv(self, output_path):
        """Export data to CSV format."""
        # Implementation for CSV export
        pass
    
    def _export_to_hdf5(self, output_path):
        """Export data to HDF5 format."""
        # Implementation for HDF5 export
        pass
```

## Building and Installation

### Prerequisites

#### System Requirements
```bash
# Operating Systems
Linux: Ubuntu 20.04+, CentOS 8+
macOS: macOS 11+
Windows: Windows 10/11 (with WSL2)

# Hardware Requirements
- CPU: x86_64 with AVX-512 or equivalent
- Memory: 4GB minimum, 16GB+ recommended for large-scale simulations
- Storage: 1GB for core, 10GB+ for complete installation

# Development Tools
- C++ Compiler: GCC 10+, Clang 12+, or MSVC 2019+
- CMake: Version 3.16 or higher
- Python: 3.8+ (for Python bindings)
- Python Packages: pip, setuptools
```

### Building from Source

#### Standard Build
```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for performance)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_PYTHON_BINDINGS=ON \
         -DNLM_ENABLE_ADVANCED_API=ON \
         -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON

# Build all targets (using all available cores)
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

#### Development Build
```bash
# Create debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j$(nproc)

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
make install
```

#### Python Package Build
```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy matplotlib

# Build and install Python package
cd /workspace/app
pip install .

# Or install in development mode
pip install -e .
```

### Build Outputs

#### C++ Targets
```bash
# After successful build, you'll have:
./nlm              # Main simulation executable
./nlm_core        # Core brain components library
./nlm_agent       # Agent system library
./nlm_world       # World simulation library
./nlm_phase6_demo  # Phase 6 integration demo
./nlm_test        # Unit test suite
```

#### Python Package
```bash
# Python module structure
pynlm/              # Main Python package
├── __init__.py      # Package initialization
├── advanced_api.py   # Advanced API module
├── utils.py         # Utility functions
├── config.py        # Configuration management
└── exceptions.py    # Custom exceptions
```

### Configuration Options

#### CMake Configuration
```bash
# Performance optimizations
cmake .. -DNLM_PERFORMANCE_ENABLED=ON

# Python bindings
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3

# Advanced API
cmake .. -DNLM_ADVANCED_API_ENABLED=ON

# Debug mode
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

### Running the Application

#### Phase 6 Demo (Integration Test)
```bash
# Run from build directory
cd build
./nlm_phase6_demo

# Or run with specific parameters
./nlm_phase6_demo --neurons=1000 --steps=1000 --dt=0.1
```

#### Python Examples
```python
import pynlm

# Create and run basic simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)

print("Simulation complete!")
```

#### Advanced API Examples
```python
from pynlm.advanced_api import AdvancedAPI, SimpleAgent, SimulationRunner

# Create Advanced API instance
advanced_api = AdvancedAPI()

# Create brain and agent
config = advanced_api.create_default_config()
brain = pynlm.createBrain(config)
brain.initialize()

agent = advanced_api.create_simple_agent(brain, None, "test_agent")

# Create simulation runner
sim_runner = advanced_api.create_simulation_runner(1000, 0.1)
sim_runner.addAgent(agent)
sim_runner.run()

# Analyze results
analysis = advanced_api.analyze_brain_activity()
print("Analysis:", analysis)
```

## Development Tools and Testing

### 1. Testing Framework

#### Unit Tests
```bash
# Run unit tests
cd build
./nlm_test --gtest_filter="*Basic*"

# Run all tests
./nlm_test

# Run specific test suites
./nlm_test --gtest_filter="*AdvancedAPI*"
./nlm_test --gtest_filter="*Interface*"
```

#### Integration Tests
```bash
# Run integration tests
cd build
./nlm_phase6_demo

# Or run via ctest
ctest -R phase6_integration
```

#### Python Tests
```bash
# Install pytest
pip install pytest pytest-cov

# Run Python tests
cd /workspace/app
pytest tests/

# Run Advanced API tests
pytest tests/test_advanced_api.py
```

### 2. Build Scripts

#### Build and Test Script
```bash
#!/bin/bash
# /workspace/app/build_and_test.sh

set -e

echo "Building NLM with Advanced API..."
mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_ADVANCED_API=ON
make -j$(nproc)

cd ..

echo "Building Python package..."
pip install -e .

echo "Running tests..."
./build/nlm_test --gtest_filter="*Basic*"
ctest --output-on-failure

echo "Building complete and tested successfully!"
```

### 3. Profiling and Benchmarking

#### Performance Profiling
```bash
# Profile with perf (Linux)
cd build
perf record ./nlm_phase6_demo
perf report

# Profile with valgrind
valgrind --tool=massif ./nlm_phase6_demo

# Profile Python with cProfile
python -m cProfile -o profile.stats examples_comprehensive.py
```

#### Benchmarking
```bash
# Run benchmarks
cd build
./nlm --benchmark --neurons=1000 --steps=1000

# Run scaling benchmarks
./nlm --scaling-benchmark --scales=10k,100k,1M
```

### 4. Code Analysis Tools

#### Linting and Type Checking
```bash
# Check C++ code
make lint

# Check Python code
flake8 pynlm/
mypy pynlm/

# Check documentation
doxygen docs/Doxyfile
```

## Code Conventions

### 1. C++ Style

#### Naming Conventions
```cpp
// Classes: PascalCase
class AdvancedAPI {
    // Methods: CamelCase
    void runSimulation() {}
    
    // Variables: snake_case
    std::unique_ptr<BrainProfiler> brain_profiler_;
    
    // Constants: UPPER_SNAKE_CASE
    const double DEFAULT_DT = 0.1;
}
```

#### Documentation
```cpp
/**
 * @class AdvancedAPI
 * @brief High-level interface for NLM simulations.
 * 
 * The AdvancedAPI class provides high-level convenience functions and class
 * abstractions for complex neural simulations. It integrates all brain systems
 * and provides comprehensive analysis tools.
 * 
 * @note This class is thread-safe for read operations but not for write operations.
 * @see SimpleAgent, SimulationRunner, BrainProfiler
 * @since Version 2.1.0
 */
class AdvancedAPI {
public:
    /**
     * @brief Create an AdvancedAPI instance.
     * 
     * @param brain Optional brain interface for analysis.
     */
    explicit AdvancedAPI(IBrainInterface* brain = nullptr);
```

### 2. Python Style

#### Naming Conventions
```python
# Classes: PascalCase
class AdvancedAPI:
    """High-level interface for NLM simulations."""
    
    # Methods: snake_case
    def run_simulation(self, config=None):
        """Run complete simulation with configuration."""
        
        # Variables: snake_case
        brain_profiler = BrainProfiler(brain)
        
        # Constants: UPPER_SNAKE_CASE
        DEFAULT_CONFIG = {
            'steps': 1000,
            'dt': 0.1,
            'neuron_count': 1000
        }
```

#### Documentation
```python
class AdvancedAPI:
    """
    High-level interface for NLM simulations.
    
    The AdvancedAPI class provides high-level convenience functions and class
    abstractions for complex neural simulations. It integrates all brain systems
    and provides comprehensive analysis tools.
    
    Attributes:
        brain: Optional brain interface for analysis
        tools: Dictionary of analysis and debugging tools
        
    Examples:
        >>> api = AdvancedAPI()
        >>> brain = api.run_simulation()
        >>> analysis = api.analyze_brain_activity()
        
    Note:
        This class is thread-safe for read operations but not for write operations.
        
    See Also:
        SimpleAgent, SimulationRunner, BrainProfiler
        
    Version:
        2.1.0
    """
    
    def __init__(self, brain=None):
        """
        Initialize Advanced API with optional brain.
        
        Args:
            brain: Optional brain interface for analysis.
        """
        self.brain = brain
        self._initialize_tools()
```

### 3. File Organization

#### C++ Header Files
```cpp
// File: src/advanced_api.hpp
// Purpose: Main advanced API interface
// Contains: AdvancedAPI class, type definitions, forward declarations

// File: src/simple_agent.hpp
// Purpose: SimpleAgent class interface
// Contains: SimpleAgent class, agent interface functions

// File: src/simulation_runner.hpp
// Purpose: SimulationRunner class interface
// Contains: SimulationRunner class, simulation control functions
```

#### Python Module Files
```python
# File: pynlm/advanced_api.py
# Purpose: Complete Advanced API implementation
# Contains: All Advanced API classes and functions

# File: pynlm/__init__.py
# Purpose: Package initialization
# Contains: Package metadata, exports, version info

# File: pynlm/utils.py
# Purpose: Utility functions
# Contains: Helper functions, common operations
```

## Performance Optimization

### 1. Memory Management

#### Smart Pointers
```cpp
// Use smart pointers for automatic memory management
class AdvancedAPI {
private:
    std::unique_ptr<BrainProfiler> brain_profiler_;
    std::shared_ptr<SimulationRunner> sim_runner_;
    std::unique_ptr<SimpleAgent> agent_;
};
```

#### Memory Pools
```cpp
// Pre-allocated memory pools for better performance
class MemoryPool {
public:
    template<typename T>
    T* allocate() {
        // Get from pool if available
        if (!pool_.empty()) {
            return pool_.back(); pool_.pop_back();
        }
        // Allocate new memory
        return new T();
    }
    
    template<typename T>
    void deallocate(T* ptr) {
        // Return to pool
        pool_.push_back(ptr);
    }
private:
    std::vector<void*> pool_;
};
```

### 2. Event-Driven Processing

#### Spike Events
```cpp
// Event-driven spike processing
class SpikeEvent {
public:
    int neuron_id;
    Timestamp time;
    float weight;
    
    SpikeEvent(int id, Timestamp t, float w) 
        : neuron_id(id), time(t), weight(w) {}
};

class EventSystem {
public:
    void scheduleSpike(int neuron_id, Timestamp time, float weight);
    void processEvents(Timestamp current_time);
    
private:
    std::priority_queue<SpikeEvent> event_queue_;
};
```

### 3. Parallel Processing

#### Thread Pool
```cpp
class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    template<typename F>
    void submit(F&& task);
    void wait();
    
private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> task_queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
};
```

#### Parallel Neural Processing
```cpp
class ParallelNeuralProcessor {
public:
    void processNeurons(const std::vector<NeuronState>& neurons, double dt);
    
private:
    ThreadPool thread_pool_;
    static void processNeuronChunk(const std::vector<NeuronState>& neurons, 
                                  size_t start, size_t end, double dt);
};
```

## Phase 6 Integration

### 1. Brain Loop Integration

#### Updated Brain::step()
```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events
    pImpl->spikeSystem->detectAndScheduleSpikes(currentStep, currentTime);
    
    // 4. Update neural systems (Phase 6 integration)
    pImpl->memorySystem->update(currentStep, currentTime);
    pImpl->neuromodulationSystem->update(currentStep, currentTime);
    pImpl->predictionSystem->update(currentStep, currentTime);
    pImpl->cognitionSystem->update(currentStep, currentTime);
    
    // 5. Apply neuromodulation to plasticity
    applyNeuromodulationToPlasticity(currentStep, currentTime);
    
    // 6. Apply plasticity rules (STDP and Hebbian)
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // STDP and Hebbian applied
        }
    }
    
    // 7. Apply structural plasticity periodically (every 100 steps)
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // 8. Update development system
    pImpl->developmentSystem->update(currentStep, currentTime);
    
    // 9. Update attention system
    pImpl->attentionSystem->update(currentStep, currentTime);
    
    // 10. Consolidate memory
    pImpl->memorySystem->consolidate(currentStep, currentTime);
    
    // 11. Update replay system
    pImpl->replaySystem->update(currentStep, currentTime);
    
    // 12. Update checkpoint system
    pImpl->checkpointSystem->update(currentStep, currentTime);
}
```

### 2. System Integration Classes

#### BrainAdapter
```cpp
class BrainAdapter : public IBrainInterface {
public:
    explicit BrainAdapter(const std::shared_ptr<Brain>& brain);
    
    void step(SimulationStep step, Timestamp time) override;
    float getNeuronPotential(int neuronId) const override;
    
    IWorkingMemory* getWorkingMemory() const override;
    IEpisodicMemory* getEpisodicMemory() const override;
    ISemanticMemory* getSemanticMemory() const override;
    
    float getDopamineLevel() const override;
    float getCuriosityLevel() const override;
    float getNoveltyLevel() const override;
    
    DevelopmentalStage getDevelopmentalStage() const override;
    float getPlasticityRate() const override;
    
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
private:
    std::shared_ptr<Brain> brain_;
};
```

#### MemorySystemWrapper
```cpp
class MemorySystemWrapper : public IMemorySystem {
public:
    explicit MemorySystemWrapper(const std::shared_ptr<MemorySystem>& memorySystem);
    
    void storeWorkingMemory(const WorkingMemoryData& data) override;
    WorkingMemoryData retrieveWorkingMemory() override;
    
    void storeEpisodicMemory(const EpisodeData& data) override;
    EpisodeData retrieveEpisodicMemory(Timestamp time) override;
    
    // ... other methods
    
private:
    std::shared_ptr<MemorySystem> memorySystem_;
};
```

### 3. Integration Testing

#### Interface Compliance Tests
```cpp
TEST(InterfaceCompliance, IBrainInterfaceCompliance) {
    // Test that BrainAdapter properly implements IBrainInterface
    auto brain = std::make_shared<Brain>(config);
    auto brainAdapter = std::make_unique<BrainAdapter>(brain);
    
    // Verify all required methods are present
    EXPECT_TRUE(brainAdapter->getWorkingMemory() != nullptr);
    EXPECT_TRUE(brainAdapter->getEpisodicMemory() != nullptr);
    EXPECT_TRUE(brainAdapter->getSemanticMemory() != nullptr);
    
    // Test interface methods
    brainAdapter->step(0, 0.0);
    EXPECT_NO_THROW(brainAdapter->save("test.bin"));
}
```

#### Integration Tests
```cpp
TEST(Phase6Integration, BrainSystemIntegration) {
    // Test that all brain systems are integrated
    Brain brain(config);
    
    // Verify that all systems are present
    EXPECT_TRUE(brain.hasWorkingMemory());
    EXPECT_TRUE(brain.hasEpisodicMemory());
    EXPECT_TRUE(brain.hasNeuromodulation());
    EXPECT_TRUE(brain.hasPredictionSystem());
    EXPECT_TRUE(brain.hasCognitionSystem());
    
    // Test integration
    brain.initialize();
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.1);
        
        // Verify that systems are updating
        EXPECT_NO_THROW(brain.getWorkingMemory()->getSize());
        EXPECT_NO_THROW(brain.getDopamineLevel());
    }
}
```

## Contributing Guidelines

### 1. Code Style

#### Formatting
- Use `clang-format` for C++ code
- Use `black` for Python code
- Follow project-specific style guides

#### Documentation
- Update documentation for all new features
- Include examples in documentation
- Document API changes and migration paths

### 2. Testing

#### Unit Tests
- Write comprehensive unit tests for all new code
- Test edge cases and error conditions
- Include performance benchmarks

#### Integration Tests
- Test all system integrations
- Verify backward compatibility
- Test with different configurations

### 3. Building

#### Local Development
```bash
# Create development build
mkdir build_dev
cd build_dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ADVANCED_API=ON
make -j$(nproc)
```

#### CI/CD Integration
```yaml
# .github/workflows/ci.yml
name: NLM CI/CD

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      - name: Build
        run: cmake --build build
      - name: Test
        run: cd build && ctest --output-on-failure
  
  python:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Python dependencies
        run: pip install pybind11 scikit-build-core pytest numpy
      - name: Build Python package
        run: pip install -e .
      - name: Run Python tests
        run: pytest tests/
```

### 4. Release Process

#### Version Bumping
```bash
# Update version in CMakeLists.txt
# Update version in pyproject.toml
# Update version in documentation
# Create git tag
# Push to remote
```

#### Documentation Updates
```bash
# Update main documentation files
cd /workspace/app
./docs/build_docs.sh

# Validate documentation
python docs/validate_docs.py
```

## Troubleshooting Development

### 1. Build Issues

#### CMake Configuration Errors
```bash
# Error: CMake cannot find Python
# Solution:
pip install pybind11 scikit-build-core
cmake .. -DPython_EXECUTABLE=$(which python)
```

#### Missing Dependencies
```bash
# Error: Required package not found
# Solution:
# Ubuntu/Debian: sudo apt-get install package-name
# macOS: brew install package-name
# CentOS/RHEL: sudo yum install package-name
```

### 2. Runtime Issues

#### Python Import Errors
```bash
# Error: No module named 'pynlm'
# Solution:
pip install -e .
```

#### Segmentation Faults
```bash
# Error: Segmentation fault
# Solution:
# 1. Check for null pointer dereferences
# 2. Run with valgrind
# 3. Check array bounds
# 4. Ensure proper initialization
```

### 3. Testing Issues

#### Test Failures
```bash
# Error: Unit test failed
# Solution:
# 1. Check test environment
# 2. Verify dependencies
# 3. Run specific failing test
# 4. Check for race conditions
```

#### Integration Test Failures
```bash
# Error: Integration test failed
# Solution:
# 1. Check system configuration
# 2. Verify interface implementations
# 3. Test with different parameter sets
# 4. Check for circular dependencies
```

### 4. Performance Issues

#### Slow Simulation
```bash
# Problem: Simulation too slow
# Solutions:
# 1. Profile with perf
# 2. Check for bottlenecks
# 3. Optimize hot paths
# 4. Enable parallel processing
```

#### High Memory Usage
```bash
# Problem: Memory usage too high
# Solutions:
# 1. Profile memory with valgrind
# 2. Check for memory leaks
# 3. Optimize data structures
# 4. Use sparse representations
```

## Summary

This developer guide provides:

1. **Architecture Overview**: Interface-based design with dependency injection
2. **Implementation Details**: Complete Advanced API implementation
3. **Building Instructions**: Comprehensive build and installation guides
4. **Development Tools**: Testing, profiling, and analysis tools
5. **Code Conventions**: Style guidelines for C++ and Python
6. **Performance Optimization**: Memory management, parallel processing
7. **Phase 6 Integration**: System integration and compliance testing
8. **Contributing Guidelines**: Code style, testing, and release process
9. **Troubleshooting**: Common issues and solutions

The NLM development environment is production-ready with comprehensive tools, documentation, and guidelines for contributing to the project. The interface-based architecture ensures maintainable, testable, and extensible code while maintaining full backward compatibility.

---

**Last Updated:** 2026-10-13
**Status:** Production Ready
**License:** Apache 2.0

For development questions, please refer to the implementation files or contact the NLM development team.