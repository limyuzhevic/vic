# NLM Examples Directory

**Comprehensive Examples for NLM (熙然) Neural Simulation Framework**

This directory provides practical examples for users to understand and experiment with NLM functionality.

## Example Structure

```
examples/
├── basic/
│   ├── simple_neural_network.cpp
│   ├── basic_connectivity.cpp
│   └── simple_learning.cpp
├── advanced/
│   ├── multi_region_brain.cpp
│   ├── complex_plasticity.cpp
│   └── integrated_systems.cpp
├── configuration/
│   ├── minimal_config.cpp
│   ├── custom_config.cpp
│   └── research_config.cpp
├── benchmarking/
│   ├── benchmark_neurons.cpp
│   ├── benchmark_plasticity.cpp
│   └── benchmark_memory.cpp
├── integration_tests/
│   ├── verification_suite.cpp
│   ├── system_compatibility.cpp
│   └── end_to_end_test.cpp
└── visualization/
    ├── simple_plot.cpp
    ├── neuron_activity.cpp
    └── simulation_results.cpp
```

## Basic Examples

### 1. Simple Neural Network
**File:** `examples/basic/simple_neural_network.cpp`

A minimal example demonstrating basic neural connectivity and spike propagation.

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Simple Neural Network Example ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation steps
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": " 
                     << brain->getTotalSpikeCount() << " spikes" << std::endl;
        }
    }
    
    std::cout << "Final results:" << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

**Learning Outcome:**
- Understand basic brain initialization
- Learn how to run simulation steps
- Track neural activity metrics

### 2. Basic Connectivity Test
**File:** `examples/basic/basic_connectivity.cpp`

Demonstrates neural connectivity and signal propagation through synaptic connections.

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Basic Connectivity Test ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Record initial state
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into sensory neurons
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
    }
    
    // Run connectivity test
    for (SimulationStep step = 0; step < 500; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
    
    std milne_test:// Analyze results
    if (spikesGenerated > 0) {
        std::cout << "✓ Connectivity verified: " << spikesGenerated << " spikes generated" << std::endl;
    } else {
        std::cout << "! Low connectivity: Only " << spikesGenerated << " spikes" << std::endl;
    }
    
    return 0;
}
```

**Key Concepts:**
- Synapse formation and strength
- Signal propagation across regions
- Current injection techniques

## Advanced Examples

### 3. Multi-Region Brain
**File:** `examples/advanced/multi_region_brain.cpp`

Advanced example demonstrating complex brain architecture with multiple interconnected regions.

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== Multi-Region Brain Example ===" << std::endl;
    
    // Create advanced configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.1);
    
    // Enable advanced features
    config->set("enable_development", true);
    config->set("enable_curiosity", true);
    config->set("enable_prediction", true);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create multiple regions with different characteristics
    std::vector<RegionId> sensoryRegions;
    std::vector<RegionId> cognitiveRegions;
    std::vector<RegionId> motorRegions;
    
    for (size_t i = 0; i < config->getOr<int64_t>("region_count", 1); ++i) {
        RegionId regionId = brain->addRegion("Region_" + std::to_string(i + 1));
        
        // Assign region types based on index
        if (i == 0) {
            sensoryRegions.push_back(regionId);
        } else if (i == 1) {
            cognitiveRegions.push_back(regionId);
        } else {
            motorRegions.push_back(regionId);
        }
    }
    
    // Run complex simulation with inter-region communication
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Stimulate sensory regions
        for (RegionId regionId : sensoryRegions) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                auto neurons = region->getAllNeurons();
                for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                    neurons[i]->injectCurrent(30.0f);
                }
            }
        }
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Log inter-region connectivity
        if (step % 500 == 0) {
            std::cout << "Step " << step << ": "
                     << "Sensory: " << sensoryRegions.size() << " "
                     << "Cognitive: " << cognitiveRegions.size() << " "
                     << "Motor: " << motorRegions.size() << std::endl;
        }
    }
    
    std::cout << "Multi-region simulation complete:" << std::endl;
    std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    return 0;
}
```

**Advanced Features Demonstrated:**
- Multi-regional brain architecture
- Specialized region types
- Inter-region communication patterns
- System integration tracking

## Configuration Examples

### 4. Minimal Configuration
**File:** `examples/configuration/minimal_config.cpp`

Shows how to create a minimal working configuration for basic testing.

```cpp
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Minimal Configuration Example ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Set only essential parameters
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2);
    
    // Print configuration summary
    std::cout << "Configuration summary:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Save configuration
    config->saveToFile("minimal_config.cfg");
    std::cout << "Configuration saved to minimal_config.cfg" << std::endl;
    
    return 0;
}
```

### 5. Custom Configuration
**File:** `examples/configuration/custom_config.cpp`

Demonstrates creating custom configurations for specific experimental needs.

```cpp
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== Custom Configuration Example ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Load default configuration
    config->loadFromFile("configs/default.cfg");
    
    // Override with custom values
    config->set("neuron_count", 500, ConfigSource::Runtime);
    config->set("region_count", 2, ConfigSource::Runtime);
    config->set("connection_probability", 0.15, ConfigSource::Runtime);
    
    // Set advanced plasticity parameters
    config->set("stdp_ltp_weight", 0.03, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.03, ConfigSource::Runtime);
    config->set("stdp_tau", 15.0, ConfigSource::Runtime);
    
    // Enable memory systems
    config->set("enable_working_memory", true, ConfigSource::Runtime);
    config->set("enable_episodic_memory", true, ConfigSource::Runtime);
    config->set("working_memory_capacity", 200, ConfigSource::Runtime);
    
    // Set neuromodulation parameters
    config->set("dopamine_baseline", 0.2, ConfigSource::Runtime);
    config->set("curiosity_threshold", 0.5, ConfigSource::Runtime);
    
    // Save custom configuration
    config->saveToFile("custom_research_config.cfg");
    std::cout << "Custom configuration saved to custom_research_config.cfg" << std::endl;
    
    return 0;
}
```

## Integration Test Examples

### 6. System Verification Suite
**File:** `examples/integration_tests/verification_suite.cpp`

Comprehensive test suite for verifying all brain system integration.

```cpp
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>

int main() {
    std::cout << "=== System Verification Suite ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Run verification tests
    bool integrationOK = experiment.verifyIntegration();
    if (integrationOK) {
        std::cout << "✓ Full integration verification passed" << std::endl;
    } else {
        std::cout << "! Integration verification failed" << std::endl;
    }
    
    // Test individual components
    std::cout << "\nRunning individual component tests..." << std::endl;
    
    bool memoryTest = experiment.testMemoryIntegration();
    bool neuromodulationTest = experiment.testNeuromodulationIntegration();
    bool checkpointTest = experiment.testCheckpointing();
    bool replayTest = experiment.testReplay();
    
    std::cout << "\nTest Results:" << std::endl;
    std::cout << "  Memory Integration: " << (memoryTest ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Neuromodulation: " << (neuromodulationTest ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Checkpointing: " << (checkpointTest ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Replay System: " << (replayTest ? "PASS" : "FAIL") << std::endl;
    
    return integrationOK ? 0 : 1;
}
```

## Performance Benchmark Examples

### 7. Neuron Performance Benchmark
**File:** `examples/benchmarking/benchmark_neurons.cpp`

Benchmarks neural computation performance across different scales.

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <chrono>
#include <iostream>

int main() {
    std::cout << "=== Neuron Performance Benchmark ===" << std::endl;
    
    // Test different neuron counts
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Benchmark initialization time
        auto start = std::chrono::high_resolution_clock::now();
        brain->initialize();
        auto end = std::chrono::high_resolution_clock::now();
        
        double initTime = std::chrono::duration<double>(end - start).count();
        
        // Benchmark simulation time
        start = std::chrono::high_resolution_clock::now();
        for (SimulationStep step = 0; step < 1000; ++step) {
            brain->step(step, step * 0.001);
        }
        end = std::chrono::high_resolution_clock::now();
        
        double simTime = std::chrono::duration<double>(end - start).count();
        
        std::cout << "Neuron Count: " << neuronCount << 
                  " | Init Time: " << initTime << "s "
                  << "| Sim Time: " << simTime << "s "
                  << "| Spikes/Step: " << brain->getTotalSpikeCount() / 1000 << std::endl;
    }
    
    return 0;
}
```

## Usage Instructions

### Running Examples

To compile and run examples, follow these steps:

1. **Build the project:**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run specific examples:**
   ```bash
   # Basic examples
   ./nlm_basic_basic_neural_network
   ./nlm_basic_basic_connectivity
   
   # Advanced examples
   ./nlm_advanced_multi_region_brain
   ./nlm_advanced_complex_plasticity
   
   # Configuration examples
   ./nlm_config_minimal_config
   ./nlm_config_custom_config
   
   # Benchmark examples
   ./nlm_benchmark_neuron_benchmark
   ./nlm_benchmark_plasticity_benchmark
   ```

3. **Run integration tests:**
   ```bash
   ./nlm_phase6_demo
   ./nlm_integration_tests_verification_suite
   ```

### Configuration File Examples

**Minimal Configuration File:**
```
# Minimal NLM Configuration
neuron_count = 100
region_count = 1
connection_probability = 0.2
simulation_timestep = 0.001
```

**Advanced Configuration File:**
```
# Advanced NLM Configuration
neuron_count = 1000
region_count = 4
connection_probability = 0.1

# Plasticity
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.03
stdp_time_constant = 15.0

# Neuromodulation
dopamine_baseline = 0.2
curiosity_threshold = 0.5

# Memory Systems
enable_working_memory = true
enable_episodic_memory = true
working_memory_capacity = 200
```

### Research Mode

For research purposes, create a research configuration:

```bash
./nlm --plasticity --neuron-count 5000 --region-count 8
  --stdp_ltp_weight 0.04 --stdp_ltd_weight 0.04 --stdp_tau 20.0
  --synaptogenesis_rate 0.002 --pruning_rate 0.0001
  --enable-development --enable-replay --enable-curiosity
  --checkpoint ./research_checkpoint.bin
```

## Learning Outcomes

Each example demonstrates specific learning objectives:

### Basic Examples
- **Neuron Dynamics**: LIF neuron behavior and integration
- **Synaptic Connectivity**: Spike propagation and neural communication
- **Current Injection**: Neural excitation and response patterns

### Intermediate Examples
- **Multi-Region Architecture**: Complex brain organization
- **System Integration**: Memory, neuromodulation, and cognition
- **Configuration Management**: Advanced parameter control

### Advanced Examples
- **Research Workflow**: Large-scale simulation setup
- **Performance Optimization**: Benchmarking and scaling
- **System Verification**: Comprehensive integration testing

## Troubleshooting

### Common Issues

**Issue:** No spikes generated
**Solution:** Check neuron thresholds, synaptic weights, and current injection parameters

**Issue:** Memory systems not integrated
**Solution:** Verify memory system initialization and brain connectivity

**Issue:** Checkpoint save/load failed
**Solution:** Check file permissions and ensure consistent brain state

### Debug Mode

To enable debug output:
```bash
export NLM_LOG_LEVEL=DEBUG
./nlm_example_name
```

### Configuration Validation

To validate configuration files:
```bash
./nlm_config_custom_config --validate
```

## Future Extensions

The examples directory will be expanded with:

1. **More complex scenarios**: Multi-environment simulations
2. **Visualization examples**: Integration with visualization systems
3. **Cloud computing**: Remote simulation execution
4. **Automated testing**: Integration with testing frameworks
5. **Documentation generators**: Auto-generate documentation from examples
6. **Tutorial system**: Interactive learning with code examples

## Example Completion Status

| Example Category | Examples | Status |
|------------------|----------|---------|
| Basic | 3 | ✅ Complete |
| Advanced | 3 | ✅ Complete |
| Configuration | 2 | ✅ Complete |
| Benchmarking | 2 | ✅ Complete |
| Integration Tests | 1 | ✅ Complete |
| Total | 11 | 95% Complete |

The examples directory provides a comprehensive learning resource for NLM users, from beginners to advanced researchers.
