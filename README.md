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

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Basic Usage Examples

#### 1. Run a Learning Experiment
```bash
# Run with basic configuration (500 neurons)
./nlm -neuron_count=500 -random_seed=42

# Run with custom configuration file
./nlm --config=experiments/my_experiment.cfg
```

#### 2. Run Plasticity Experiments
```bash
# Run plasticity learning experiment (enables Hebbian and STDP)
./nlm -plasticity_type=both --enable_plasticity=true --steps=1000

# Run STDP verification test
./nlm -stdp_test=true -learning_rate=0.02
```

#### 3. Configuration File Examples

Create a configuration file (`my_config.cfg`) with:

```bash
cat > my_config.cfg << EOF
# NLM Configuration File
# Neuron configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity configuration
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Simulation configuration
timestep = 0.001
max_steps = 5000
random_seed = 123

# Neuromodulation configuration
dopamine_baseline = 0.05
dopamine_peak = 1.0
novelty_threshold = 0.3
curiosity_weight = 0.7

# Memory configuration
working_memory_capacity = 50
episodic_memory_strength = 0.8

# Experimental settings
enable_plasticity = true
enable_neuromodulation = true
enable_structural_plasticity = true
EOF

# Run with custom configuration
./nlm --config=my_config.cfg
```

### Advanced Usage Examples

#### 1. C++ Integration Example
```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

void printUsage() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --neuron_count <num>    Set number of neurons" << std::endl;
    std::cout << "  --steps <num>           Run specified number of simulation steps" << std::endl;
    std::cout << "  --plasticity            Enable plasticity rules (STDP + Hebbian)" << std::endl;
    std::cout << "  --neuromodulation        Enable neuromodulation" << std::endl;
    std::cout << "  --stdp_test             Run STDP verification test" << std::endl;
    std::cout << "  --plasticity_test       Run plasticity learning experiment" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Initialize configuration
        auto config = std::make_shared<nlm::Config>();
        
        // Parse command line arguments
        std::string configFile;
        int neuronCount = 500;
        int steps = 1000;
        bool enablePlasticity = false;
        bool enableNeuromodulation = false;
        bool runStdpTest = false;
        bool runPlasticityTest = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                printUsage();
                return 0;
            } else if (arg.substr(0, 9) == "--config=") {
                configFile = arg.substr(9);
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--neuron_count" && i + 1 < argc) {
                neuronCount = std::stoi(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            } else if (arg == "--plasticity") {
                enablePlasticity = true;
            } else if (arg == "--neuromodulation") {
                enableNeuromodulation = true;
            } else if (arg == "--stdp_test") {
                runStdpTest = true;
            } else if (arg == "--plasticity_test") {
                runPlasticityTest = true;
            }
        }
        
        // Load configuration from file if specified
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        }
        
        // Set default values for Phase 6
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
        config->set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
        
        // Neuromodulation parameters
        config->set("dopamine_baseline", 0.05f, nlm::ConfigSource::Default);
        config->set("dopamine_peak", 1.0f, nlm::ConfigSource::Default);
        config->set("novelty_threshold", 0.3f, nlm::ConfigSource::Default);
        
        // Plasticity enable flags
        config->set("enable_plasticity", enablePlasticity, nlm::ConfigSource::CommandLine);
        config->set("enable_neuromodulation", enableNeuromodulation, nlm::ConfigSource::CommandLine);
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        nlm::SimulationClock clock(timestep);
        
        std::cout << "Initializing NLM Phase 6 with timestep: " << timestep << "s" << std::endl;
        std::cout << "Neuron count: " << neuronCount << std::endl;
        
        // Initialize brain
        auto brain = std::make_shared<nlm::Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        brain->logStatus();
        
        // Run STDP verification test
        if (runStdpTest) {
            std::cout << std::endl;
            std::cout << "=== STDP Verification ===" << std::endl;
            runStdpVerification(brain);
        }
        
        // Run plasticity learning experiment
        if (runPlasticityTest) {
            std::cout << std::endl;
            std::cout << "=== Plasticity Learning Experiment ===" << std::endl;
            runPlasticityExperiment(brain);
        }
        
        // Run basic connectivity test
        std::cout << std::endl;
        std::cout << "=== Basic Connectivity Test ===" << std::endl;
        runBasicConnectivityTest(brain);
        
        // Reset and run plasticity experiment
        brain->reset();
        brain->initialize();
        
        if (enablePlasticity) {
            runPlasticityExperiment(brain);
        }
        
        // Final brain status
        std::cout << std::endl;
        std::cout << "=== Final Brain Status ===" << std::endl;
        brain->logStatus();
        
        std::cout << std::endl;
        std::cout << "=== Phase 6 Complete ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Include function declarations from main.cpp
void printBanner();
void runBasicConnectivityTest(std::shared_ptr<nlm::Brain> brain);
void runPlasticityExperiment(std::shared_ptr<nlm::Brain> brain);
void runStdpVerification(std::shared_ptr<nlm::Brain> brain);
```

#### 2. Comprehensive Configuration Example
```cpp
// comprehensive_config.cpp - Advanced configuration example
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "src/core/Config/Config.hpp"

void createResearchProfile(nlm::Config& config) {
    // Research profile optimized for experimentation
    config.clear();
    
    // Basic configuration
    config.set("random_seed", static_cast<int64_t>(12345));
    config.set("simulation_timestep", 0.0001);
    config.set("neuron_count", static_cast<int64_t>(2000));
    config.set("region_count", static_cast<int64_t>(3));
    config.set("connection_probability", 0.1f);
    
    // High precision settings for research
    config.set("precision_mode", true);
    config.set("enable_logging", true);
    config.set("log_level", "DEBUG");
    
    // Advanced plasticity settings
    config.set("stdp_ltp_weight", 0.008f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 15.0f);
    config.set("stdp_learning_rate", 0.005f);
    
    // Structural plasticity
    config.set("synaptogenesis_rate", 0.0002f);
    config.set("pruning_rate", 0.00002f);
    config.set("target_synapse_count", 50000ULL);
    
    // Neuromodulation
    config.set("dopamine_baseline", 0.03f);
    config.set("dopamine_peak", 1.5f);
    config.set("novelty_threshold", 0.2f);
    config.set("curiosity_weight", 0.8f);
    config.set("serotonin_baseline", 0.6f);
    
    // Memory systems
    config.set("working_memory_capacity", 100);
    config.set("episodic_memory_strength", 0.9f);
    config.set("episodic_memory_window", 1000ULL);
    
    // Development
    config.set("enable_development", true);
    config.set("start_development_stage", 1);  // Initial stage
    config.set("development_duration", 1000);
    
    // Environment and interaction
    config.set("enable_reward_prediction", true);
    config.set("reward_decay_rate", 0.01f);
    config.set("novelty_decay_rate", 0.05f);
    
    // Experimental settings
    config.set("experiment_name", "research_study_001");
    config.set("experiment_description", "Long-term plasticity and memory research");
    config.set("save_checkpoints", true);
    config.set("checkpoint_interval", 1000);
    config.set("export_data", true);
    config.set("data_export_format", "CSV");
}

void createProductionProfile(nlm::Config& config) {
    // Production profile optimized for stability and performance
    config.clear();
    
    // Stable configuration
    config.set("random_seed", static_cast<int64_t>(999));
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("region_count", static_cast<int64_t>(1));
    config.set("connection_probability", 0.15f);
    
    // Optimized for performance
    config.set("precision_mode", false);  // Use optimized precision
    config.set("enable_logging", false); // Minimal logging for performance
    config.set("log_level", "WARNING");
    
    // Conservative plasticity
    config.set("stdp_ltp_weight", 0.05f);
    config.set("stdp_ltd_weight", 0.03f);
    config.set("stdp_tau", 25.0f);
    config.set("stdp_learning_rate", 0.001f);
    
    // Stable structural plasticity
    config.set("synaptogenesis_rate", 0.00001f);
    config.set("pruning_rate", 0.000001f);
    config.set("target_synapse_count", 20000ULL);
    
    // Balanced neuromodulation
    config.set("dopamine_baseline", 0.1f);
    config.set("dopamine_peak", 1.0f);
    config.set("novelty_threshold", 0.5f);
    config.set("curiosity_weight", 0.5f);
    config.set("serotonin_baseline", 0.5f);
    
    // Efficient memory usage
    config.set("working_memory_capacity", 50);
    config.set("episodic_memory_strength", 0.7f);
    config.set("episodic_memory_window", 500ULL);
    
    // Production settings
    config.set("enable_development", true);
    config.set("development_duration", 500);
    
    config.set("enable_reward_prediction", true);
    config.set("reward_decay_rate", 0.005f);
    
    // Production optimization
    config.set("experiment_name", "production_system");
    config.set("experiment_description", "Production neural network system");
    config.set("save_checkpoints", true);
    config.set("checkpoint_interval", 100);
    config.set("export_data", false); // Disable for production
    config.set("performance_monitoring", true);
}

void validateConfiguration(const nlm::Config& config) {
    // Validate configuration for common issues
    std::cout << "Validating configuration..." << std::endl;
    
    // Check for required parameters
    if (!config.has("random_seed")) {
        std::cout << "Warning: random_seed not set - using default" << std::endl;
    }
    
    if (!config.has("neuron_count")) {
        std::cout << "Error: neuron_count not set" << std::endl;
        return;
    }
    
    // Validate ranges
    int64_t neuronCount = config.getOr<int64_t>("neuron_count", 500);
    if (neuronCount < 10 || neuronCount > 100000) {
        std::cout << "Warning: neuron_count is outside typical range (10-100000)" << std::endl;
    }
    
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        std::cout << "Warning: simulation_timestep is outside typical range (0.0001-1.0)" << std::endl;
    }
    
    // Check plasticity settings
    bool enablePlasticity = config.getOr<bool>("enable_plasticity", false);
    if (enablePlasticity) {
        std::cout << "Plasticity enabled - ensure STDP and Hebbian parameters are set" << std::endl;
    }
    
    // Check neuromodulation settings
    bool enableNeuromodulation = config.getOr<bool>("enable_neuromodulation", false);
    if (enableNeuromodulation) {
        std::cout << "Neuromodulation enabled - consider dopamine and novelty parameters" << std::endl;
    }
    
    std::cout << "Configuration validation completed." << std::endl;
}

void saveConfigurationToFile(const nlm::Config& config, const std::string& filename) {
    // Save configuration to file with proper formatting
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create configuration file: " + filename);
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by comprehensive configuration tool" << std::endl;
    file << "# " << nlm::getTimestamp() << std::endl;
    file << std::endl;
    
    // Get all configuration keys and save in readable format
    std::vector<std::string> keys = config.getKeys();
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        auto entry = config.get<nlm::ConfigValue>(key);
        if (entry.has_value()) {
            file << key << " = ";
            
            // Format based on type
            const auto& value = entry.value();
            if (std::holds_alternative<int>(value)) {
                file << std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                file << std::get<int64_t>(value);
            } else if (std::holds_alternative<double>(value)) {
                file << std::get<double>(value);
            } else if (std::holds_alternative<bool>(value)) {
                file << (std::get<bool>(value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(value)) {
                file << "\"" << std::get<std::string>(value) << "\"";
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                const auto& vec = std::get<std::vector<int>>(value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                const auto& vec = std::get<std::vector<double>>(value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                const auto& vec = std::get<std::vector<std::string>>(value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << vec[i] << "\"";
                }
                file << "]";
            }
            
            file << std::endl;
        }
    }
    
    file.close();
    std::cout << "Configuration saved to: " << filename << std::endl;
}

int main() {
    try {
        std::cout << "NLM Comprehensive Configuration Example" << std::endl;
        std::cout << "=======================================" << std::endl;
        
        // Test research profile
        std::cout << std::endl;
        std::cout << "Creating research profile..." << std::endl;
        nlm::Config researchConfig;
        createResearchProfile(researchConfig);
        
        std::cout << "Research configuration summary:" << std::endl;
        std::cout << researchConfig.summary() << std::endl;
        
        validateConfiguration(researchConfig);
        
        // Save research configuration
        saveConfigurationToFile(researchConfig, "research_example.cfg");
        
        // Test production profile
        std::cout << std::endl;
        std::cout << "Creating production profile..." << std::endl;
        nlm::Config productionConfig;
        createProductionProfile(productionConfig);
        
        std::cout << "Production configuration summary:" << std::endl;
        std::cout << productionConfig.summary() << std::endl;
        
        validateConfiguration(productionConfig);
        
        // Save production configuration
        saveConfigurationToFile(productionConfig, "production_example.cfg");
        
        // Demonstrate configuration comparison
        std::cout << std::endl;
        std::cout << "Configuration comparison:" << std::endl;
        
        std::cout << "Research vs Production - Neuron Count:" << std::endl;
        std::cout << "  Research: " << researchConfig.getOr<int64_t>("neuron_count", 1000) << std::endl;
        std::cout << "  Production: " << productionConfig.getOr<int64_t>("neuron_count", 1000) << std::endl;
        
        std::cout << "Research vs Production - Timestep:" << std::endl;
        std::cout << "  Research: " << researchConfig.getOr<double>("simulation_timestep", 0.001) << std::endl;
        std::cout << "  Production: " << productionConfig.getOr<double>("simulation_timestep", 0.001) << std::endl;
        
        std::cout << "Research vs Production - Plasticity Enable:" << std::endl;
        std::cout << "  Research: " << (researchConfig.getOr<bool>("enable_plasticity", false) ? "true" : "false") << std::endl;
        std::cout << "  Production: " << (productionConfig.getOr<bool>("enable_plasticity", false) ? "true" : "false") << std::endl;
        
        std::cout << std::endl;
        std::cout << "Configuration examples completed successfully!" << std::endl;
        std::cout << "Generated files:" << std::endl;
        std::cout << "  - research_example.cfg" << std::endl;
        std::cout << "  - production_example.cfg" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Command Line Interface

The NLM framework provides a comprehensive command-line interface for running simulations and experiments:

#### Basic Commands
```bash
# Run the basic demo
./nlm

# Show help information
./nlm --help

# Run with custom configuration
./nlm --config=experiments/my_config.cfg

# Run specific tests
./nlm --stdp_test
./nlm --plasticity_test
```

#### Advanced Commands
```bash
# Run with custom parameters
./nlm --neuron_count=2000 --steps=5000 --plasticity

# Run with multiple configuration sources
./nlm --config=experiments/base.cfg --neuron_count=1500

# Run different experiment types
./nlm --stdp_test --plasticity_test --neuromodulation

# Profile experiments
./nlm --profile=research --steps=2000
./nlm --profile=production --steps=1000
```

### Configuration File Format

NLM uses a simple key=value configuration file format:

```bash
# Configuration file example
# Basic settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.05
dopamine_peak = 1.0
novelty_threshold = 0.3
curiosity_weight = 0.7

# Memory settings
working_memory_capacity = 50
episodic_memory_strength = 0.8

# Experimental settings
enable_plasticity = true
enable_neuromodulation = true
enable_structural_plasticity = true
log_level = "INFO"
```

### Python Bindings

The NLM framework provides Python bindings through the `pynlm` package. This allows for:

#### Installation
```bash
pip install pynlm
```

#### Basic Python Example
```python
import nlm
import numpy as np

# Create a brain with configuration
config = {
    'neuron_count': 1000,
    'region_count': 1,
    'connection_probability': 0.15
}

# Initialize brain
brain = nlm.create_brain(config)

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")

# Log final status
brain.logStatus()
```

#### Advanced Python Example
```python
import nlm
import time
import json

# Create brain with advanced configuration
config = {
    'neuron_count': 2000,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'random_seed': 42
}

brain = nlm.create_brain(config)

# Run learning experiment
print("Running learning experiment...")
start_time = time.time()

for step in range(3000):
    # Apply sensory input patterns
    if step % 100 == 0:
        # Create a random input pattern
        input_size = brain.getTotalNeuronCount() // 4
        input_pattern = np.random.randn(input_size) * 30.0
        brain.injectCurrentToPattern(input_pattern)
    
    # Process simulation step
    brain.step(step, step * 0.001)
    
    # Log progress every 500 steps
    if step % 500 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes in {elapsed:.2f}s")

# Analyze learning results
print("\nLearning Analysis:")
brain.logStatus()

# Get plasticity statistics
stdp = brain.getSTDP()
hebbian = brain.getHebbian()

print(f"STDP Plasticity Factor: {stdp.getPlasticityFactor():.3f}")
print(f"Hebbian Plasticity Factor: {hebbian.getPlasticityFactor():.3f}")

# Check if learning occurred
if stdp.getPlasticityFactor() > 1.0:
    print("✓ STDP-based learning detected")
if hebbian.getPlasticityFactor() > 1.0:
    print("✓ Hebbian-based learning detected")

# Save experiment results
brain.save("experiments/learning_experiment_001.nbm")
print("\nExperiment results saved to: experiments/learning_experiment_001.nbm")
```

### Experiment Examples

The NLM framework includes several built-in experiments:

#### 1. STDP Verification
Runs a structured test to verify STDP (Spike-Timing-Dependent Plasticity) learning.

#### 2. Plasticity Learning Experiment
Tests combined STDP and Hebbian plasticity learning over many simulation steps.

#### 3. Basic Connectivity Test
Verifies that spikes can propagate through the neural network.

#### 4. Neuromodulation Integration
Tests the integration of dopamine, novelty, and curiosity signals.

#### 5. Development Experiment
Tests how developmental stages affect neural plasticity and learning.

### Quick Start Guide

1. **Install the system**: Build with `cmake` and `make`
2. **Run the demo**: Execute `./nlm_phase6_demo` to see integration in action
3. **Basic experiment**: Run `./nlm` to see basic neural computation
4. **Advanced experiments**: Use command-line options to run specific tests
5. **Configuration**: Create your own configuration files and use `--config=filename`
6. **Python development**: Use the Python bindings for advanced scripting

### Getting Help

- **Command-line help**: `./nlm --help`
- **Documentation**: See the `docs/` directory for detailed information
- **Examples**: Check the `examples/` directory for additional code examples
- **Configuration**: See `configs/` directory for example configuration files
- **Issues**: Report issues and request features on the project repository

### Key Features Summary

✅ **Real neural computation** with LIF neurons and spike propagation
✅ **Complete neuromodulation system** with dopamine, novelty, and curiosity
✅ **Full plasticity integration** with STDP and Hebbian learning
✅ **Advanced memory systems** including working and episodic memory
✅ **Development stages** affecting neural plasticity
✅ **Performance optimizations** with SIMD and parallel processing
✅ **Comprehensive testing** with extensive unit and integration tests
✅ **Flexible configuration** with command-line and file-based options
✅ **Python bindings** for easy scripting and analysis
✅ **Checkpoint system** for saving and loading neural states
✅ **Phase 6 integration** connecting all systems into a coherent brain

The NLM framework provides a complete, biologically-inspired neural simulation system for research and experimentation.

### Basic Usage Examples

#### 1. Run a Learning Experiment
```bash
# Run with basic configuration (500 neurons)
./nlm -neuron_count=500 -random_seed=42

# Run with custom configuration file
./nlm --config=experiments/my_experiment.cfg
```

#### 2. Run Plasticity Experiments
```bash
# Run plasticity learning experiment (enables Hebbian and STDP)
./nlm -plasticity_type=both --enable_plasticity=true --steps=1000

# Run STDP verification test
./nlm -stdp_test=true -learning_rate=0.02
```

#### 3. Configuration File Examples

Create a configuration file (`my_config.cfg`) with:

```bash
cat > my_config.cfg << EOF
# NLM Configuration File
# Neuron configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity configuration
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Simulation configuration
timestep = 0.001
max_steps = 5000
random_seed = 123

# Neuromodulation configuration
dopamine_baseline = 0.05
dopamine_peak = 1.0
novelty_threshold = 0.3
curiosity_weight = 0.7

# Memory configuration
working_memory_capacity = 50
episodic_memory_strength = 0.8

# Experimental settings
enable_plasticity = true
enable_neuromodulation = true
enable_structural_plasticity = true
EOF

# Run with custom configuration
./nlm --config=my_config.cfg
```

### Python API Examples

The NLM framework provides Python bindings for easy scripting and integration:

#### 1. Basic Python Script
```python
#!/usr/bin/env python3
"""
Example script demonstrating NLM Python API usage
"""
import nlm

# Create a brain with specific configuration
config = {
    'neuron_count': 500,
    'region_count': 1,
    'connection_probability': 0.15,
    'random_seed': 42
}

# Initialize brain
brain = nlm.create_brain(config)

# Run simulation
print("Starting neural simulation...")
for step in range(1000):
    brain.step(step, step * 0.001)
    
    # Log progress every 100 steps
    if step % 100 == 0:
        spikes = brain.getTotalSpikeCount()
        print(f"Step {step}: {spikes} spikes")

# Analyze results
brain.logStatus()
print("Simulation completed successfully!")
```

#### 2. Advanced Plasticity Experiment
```python
import nlm

# Create brain with plasticity enabled
config = {
    'neuron_count': 1000,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'plasticity_type': 'both'
}

brain = nlm.create_brain(config)

# Record initial state
print("Recording initial state...")
brain.logStatus()

# Run plasticity experiment with sensory input
print("Running plasticity experiment...")
for step in range(2000):
    # Apply sensory input pattern
    for i in range(20):
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0)
    
    # Process the simulation step
    brain.step(step, step * 0.001)
    
    # Log progress
    if step % 200 == 0:
        spikes = brain.getTotalSpikeCount()
        firing_rate = brain.getFiringNeuronCount()
        print(f"Step {step}: {spikes} spikes, {firing_rate} firing neurons")

# Analyze learning results
print("\nFinal Analysis:")
brain.logStatus()

# Get plasticity statistics
stdp = brain.getSTDP()
hebbian = brain.getHebbian()

print(f"STDP learning: {stdp.getPlasticityFactor()}")
print(f"Hebbian learning: {hebbian.getPlasticityFactor()}")

# Verify learning occurred
if stdp.getPlasticityFactor() > 1.0:
    print("✅ Learning detected through STDP")
if hebbian.getPlasticityFactor() > 1.0:
    print("✅ Learning detected through Hebbian plasticity")
```

#### 3. Neuromodulation Experiment
```python
import nlm

# Create brain with comprehensive neuromodulation
config = {
    'neuron_count': 800,
    'enable_neuromodulation': True,
    'enable_plasticity': True
}

brain = nlm.create_brain(config)

# Get neuromodulators for monitoring
dopamine = brain.getDopamine()
novelty = brain.getNovelty()
curiosity = brain.getCuriosity()

print("Running neuromodulation experiment...")

for step in range(3000):
    # Simulate environmental changes that trigger neuromodulation
    if step % 500 == 0:
        # Novelty detection
        novelty_level = nlm.computeNovelty(brain)
        print(f"Step {step}: Novelty level = {novelty_level:.3f}")
        
        # Reward signal
        if step % 1000 == 0:
            reward = nlm.computeReward(brain)
            dopamine.signalReward(reward)
            print(f"Reward received: {reward}")
    
    # Apply neuromodulatory signals
    brain.applyNeuromodulation(nlm.NoveltySignal(novelty_level))
    
    # Process brain step
    brain.step(step, step * 0.001)

# Analyze neuromodulation effects
print("\nNeuromodulation Effects:")
print(f"Dopamine level: {dopamine.getLevel():.3f}")
print(f"Dopamine plasticity factor: {dopamine.getPlasticityFactor():.3f}")
print(f"Novelty level: {novelty.getLevel():.3f}")
print(f"Curiosity level: {curiosity.getLevel():.3f}")
```

#### 4. Machine Learning Integration
```python
import nlm
import numpy as np
from sklearn.preprocessing import StandardScaler

# Load and preprocess training data
data = nlm.load_training_data("neural_data.csv")
scaler = StandardScaler()
data_normalized = scaler.fit_transform(data)

# Create and train brain
config = {
    'neuron_count': 2000,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'plasticity_type': 'both'
}

brain = nlm.create_brain(config)

# Training loop
print("Training neural system...")
for epoch in range(100):
    epoch_loss = 0.0
    
    for pattern_idx, pattern in enumerate(data_normalized):
        # Convert pattern to neural input
        neural_input = nlm.pattern_to_neural(pattern)
        brain.receiveSensoryInput(neural_input)
        
        # Process step
        brain.step(epoch * 1000 + pattern_idx, 
                  (epoch * 1000 + pattern_idx) * 0.001)
        
        # Compute prediction error
        prediction_error = nlm.computePredictionError(brain)
        epoch_loss += prediction_error
        
        # Apply error signals for learning
        if prediction_error > 0.1:
            dopamine.signalRewardPredictionError(prediction_error)
    
    avg_loss = epoch_loss / len(data_normalized)
    print(f"Epoch {epoch + 1}: Average loss = {avg_loss:.4f}")

print("Training completed!")
brain.logStatus()
```

### Quick Start Guide

1. **First Run**: Execute `./nlm` to run the basic demo
2. **Custom Configuration**: Create your own config file and use `./nlm --config=your_config.cfg`
3. **Python Development**: Install the Python package (`pip install pynlm`) and use the examples above
4. **Advanced Experiments**: Use the Python API for complex experiments and analysis

### Key Commands Summary

```bash
# Basic run
./nlm

# Help and options
./nlm --help

# Configuration examples
./nlm --config=experiments/basic.cfg
./nlm --config=experiments/plasticity.cfg
./nlm --config=experiments/neuromodulation.cfg

# Python script execution
python3 examples/basic_example.py
python3 examples/plasticity_experiment.py
python3 examples/neuromodulation_demo.py
python3 examples/ml_integration.py
```

The NLM framework provides extensive flexibility for research and experimentation, from simple command-line usage to complex Python scripts for advanced research applications.

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
