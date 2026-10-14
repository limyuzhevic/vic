# BrainAPI Documentation - Simplified User Interface for NLM

**Version:** 1.0.0
**Date:** October 14, 2026
**Author:** NLM Development Team

## Overview

The **BrainAPI** provides a simplified, user-friendly interface to the complex NLM (Neural Learning Machine) brain system. It makes it easy for users to create, configure, and run brain simulations while maintaining access to advanced features for power users.

## Key Features

### 1. Simplified Brain Creation
- Factory methods for easy brain creation
- Pre-configured default settings
- File-based configuration loading

### 2. Configuration Management
- Configuration profiles (research, performance, learning, minimal)
- Runtime configuration modification
- Configuration validation
- File I/O for configuration

### 3. Enhanced Error Handling
- Comprehensive error checking
- Clear error messages
- Exception safety
- Graceful failure handling

### 4. Advanced Features
- Feature management and monitoring
- Development stage control
- Event callback system
- Statistics and monitoring

### 5. State Management
- Save/load brain state
- Reset functionality
- State validation

## Getting Started

### Quick Start

```cpp
#include "BrainAPI.hpp"

int main() {
    // Create a default brain
    auto brain = BrainAPI::createDefault();
    
    // Initialize the brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    brain->step(1000);
    
    // Get statistics
    auto stats = brain->getStats();
    std::cout << "Simulation completed! " << stats.totalSpikes << " spikes" << std::endl;
    
    return 0;
}
```

## API Reference

### BrainAPI Class

The main class for interacting with NLM brains.

#### Factory Methods

| Method | Description |
|--------|-------------|
| `createDefault()` | Create brain with default configuration |
| `createFromFile(filename)` | Create brain from configuration file |
| `createCustom(config)` | Create brain from custom configuration |

#### Core Methods

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the brain |
| `step(steps)` | Advance simulation by specified steps |
| `step(timeDelta)` | Advance simulation by specified time |
| `getStats()` | Get brain statistics |
| `reset()` | Reset brain to initial state |
| `save(filename)` | Save brain state to file |
| `load(filename)` | Load brain state from file |

#### Configuration Management

| Method | Description |
|--------|-------------|
| `enableFeature(name, enable)` | Enable/disable a feature |
| `isFeatureEnabled(name)` | Check if feature is enabled |
| `loadProfile(name)` | Load configuration profile |
| `getAvailableProfiles()` | Get list of available profiles |
| `validateConfiguration()` | Validate current configuration |

#### Development Control

| Method | Description |
|--------|-------------|
| `setDevelopmentStage(stage)` | Set developmental stage |
| `getDevelopmentStage()` | Get current developmental stage |

#### Event Callbacks

| Method | Description |
|--------|-------------|
| `setStepCallback(callback)` | Set function to call on each step |
| `setSpikeCallback(callback)` | Set function to call on spike events |

#### Accessors

| Method | Description |
|--------|-------------|
| `getConfig()` | Get configuration object |
| `getBrain()` | Get underlying Brain instance (advanced) |

### BrainAPIStats Structure

```cpp
struct BrainAPIStats {
    int totalNeurons;           // Total number of neurons
    int totalSynapses;         // Total number of synapses
    float avgFiringRate;       // Average firing rate
    float eI_Ratio;            // Excitation/inhibition ratio
    int totalSpikes;           // Total spike count
    int workingMemoryTraced;   // Working memory traces
    int memoryEpisodes;        // Memory episodes stored
    float dopamineLevel;       // Dopamine level
    float curiosityLevel;      // Curiosity level
    float noveltyLevel;        // Novelty level
};
```

### Configuration Profiles

NLM provides several predefined configuration profiles:

#### Research Profile
Optimized for research and experimentation:
- 500 neurons
- All plasticity mechanisms enabled
- Full neuromodulation
- Checkpointing enabled
- Development enabled

#### Performance Profile
Optimized for performance and speed:
- 2000 neurons
- Only STDP plasticity
- Limited neuromodulation
- No checkpointing
- Development disabled

#### Learning Profile
Optimized for learning and adaptation:
- 1000 neurons
- All plasticity mechanisms
- Full neuromodulation
- Checkpointing enabled
- Development enabled
- Large memory capacity

#### Minimal Profile
Optimized for minimal resource usage:
- 100 neurons
- No plasticity mechanisms
- No neuromodulation
- No checkpointing
- Development disabled
- Memory disabled

#### Usage Example

```cpp
auto brain = BrainAPI::createDefault();
brain->loadProfile("research");
if (brain->initialize()) {
    // Run research simulation
}
```

### Feature Management

The BrainAPI supports various features that can be enabled/disabled:

#### Available Features

| Feature Name | Description |
|--------------|-------------|
| `reward_modulation` | Enable dopamine-based reward learning |
| `structural_plasticity` | Enable synapse creation/removal |
| `development` | Enable developmental plasticity |
| `curiosity` | Enable curiosity-driven exploration |
| `novelty` | Enable novelty detection |

#### Usage Example

```cpp
auto brain = BrainAPI::createDefault();

// Enable features
brain->enableFeature("reward_modulation", true);
brain->enableFeature("structural_plasticity", true);
brain->enableFeature("development", true);

// Check feature status
if (brain->isFeatureEnabled("reward_modulation")) {
    std::cout << "Reward modulation is enabled!" << std::endl;
}
```

### Development Stages

Brains go through different developmental stages that affect plasticity:

| Stage | Description |
|-------|-------------|
| `Initial` | High plasticity, learning quickly |
| `CriticalPeriod` | Moderate plasticity, sensitive learning |
| `Maturation` | Low plasticity, stable learning |
| `Adult` | Very low plasticity, stable behavior |
| `Aging` | Declining plasticity, slower learning |

#### Usage Example

```cpp
auto brain = BrainAPI::createDefault();
brain->initialize();

// Test different development stages
for (int stage = 0; stage <= 4; ++stage) {
    brain->setDevelopmentStage(static_cast<DevelopmentalStage>(stage));
    brain->reset();
    brain->initialize();
    brain->step(100);
    
    auto stats = brain->getStats();
    std::cout << "Stage " << stage << ": " << stats.totalSpikes << " spikes" << std::endl;
}
```

### Event Callbacks

The BrainAPI supports callbacks for monitoring and reacting to events:

#### Step Callback

```cpp
brain->setStepCallback([](int step) {
    if (step % 100 == 0) {
        std::cout << "Processing step " << step << std::endl;
    }
});
```

#### Spike Callback

```cpp
brain->setSpikeCallback([](const DetailedSpikeEvent& event) {
    std::cout << "Neuron " << event.source_neuron 
              << " spiked at time " << event.timestamp << std::endl;
});
```

### File Operations

#### Save/Load

```cpp
auto brain = BrainAPI::createDefault();
brain->initialize();

// Save brain state
brain->save("my_brain.bin");

// Reset and load
brain->reset();
brain->load("my_brain.bin");
```

## Error Handling

The BrainAPI provides comprehensive error handling:

### Error Types

| Error Type | Description |
|------------|-------------|
| `INITIALIZATION_FAILED` | Failed to initialize brain |
| `INVALID_CONFIGURATION` | Invalid configuration settings |
| `MEMORY_ALLOCATION_FAILED` | Failed to allocate memory |
| `SYSTEM_INITIALIZATION_FAILED` | Failed to initialize a system |
| `FILE_IO_ERROR` | File read/write error |
| `VALIDATION_ERROR` | Configuration validation failed |

### Error Handling Example

```cpp
auto brain = BrainAPI::createDefault();
if (!brain->initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    // Check last error (if available)
    return 1;
}
```

## Advanced Usage

### Custom Configuration

Create a custom configuration:

```cpp
Config config;
config.set("brain.neuron_count", 2000);
config.set("brain.synapse_density", 0.2);
config.set("plasticity.stdp.enable", true);
config.set("plasticity.hebbian.enable", false);
config.set("neuromod.dopamine.scale", 1.5);

auto brain = BrainAPI::createCustom(config);
```

### Configuration File

Create a configuration file (JSON format):

```json
{
    "brain.neuron_count": 1500,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": true,
    "plasticity.hebbian.enable": true,
    "neuromod.dopamine.scale": 2.0,
    "neuromod.curiosity.enable": true,
    "simulation.timestep": 0.001,
    "checkpoint.enable": true,
    "development.enable": true
}
```

### Statistics Monitoring

Monitor brain statistics during simulation:

```cpp
auto brain = BrainAPI::createDefault();
brain->initialize();

for (int step = 0; step < 1000; ++step) {
    brain->step(1);
    
    if (step % 100 == 0) {
        auto stats = brain->getStats();
        std::cout << "Step " << step << ": "
                  << stats.totalSpikes << " spikes, "
                  << "rate: " << stats.avgFiringRate << ", "
                  << "E/I: " << stats.eI_Ratio << std::endl;
    }
}
```

## Performance Tips

### For Better Performance

1. **Use appropriate profile**: Choose the profile that matches your needs
2. **Control timestep**: Smaller timesteps are more accurate but slower
3. **Limit simulation time**: Run only as many steps as needed
4. **Monitor memory usage**: Large simulations require more memory
5. **Use checkpointing**: Save progress periodically for long simulations

### For Research

1. **Use research profile**: Full features for experimentation
2. **Enable all systems**: Memory, neuromodulation, development
3. **Use checkpointing**: Preserve experimental state
4. **Monitor statistics**: Track all relevant metrics

### For Production

1. **Use performance profile**: Optimized for speed
2. **Disable unnecessary features**: Only enable what's needed
3. **Use minimal profile**: For low-resource environments
4. **Regular checkpointing**: Ensure data recovery

## Migration Guide

### From Old API to BrainAPI

If you're using the old NLM API, here's how to migrate to BrainAPI:

#### Old API Example

```cpp
// Old API
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
auto brain = std::make_shared<Brain>(config);
brain->initialize();
brain->step(100);
```

#### New API Equivalent

```cpp
// New API
auto brain = BrainAPI::createCustom(config);
if (brain->initialize()) {
    brain->step(100);
}
```

### Backward Compatibility

The BrainAPI maintains backward compatibility with the original NLM API:

```cpp
// Backward compatibility functions
brainStep(100);           // Same as: brain->step(100)
brainReset();            // Same as: brain->reset()
brainSave("file.bin");   // Same as: brain->save("file.bin")
brainLoad("file.bin");   // Same as: brain->load("file.bin")
getTotalNeurons();       // Same as: brain->getStats().totalNeurons
getTotalSynapses();      // Same as: brain->getStats().totalSynapses
getAverageFiringRate();  // Same as: brain->getStats().avgFiringRate
getExcitationInhibitionRatio();  // Same as: brain->getStats().eI_Ratio
```

## Troubleshooting

### Common Issues

#### "Brain not initialized"
**Solution**: Call `initialize()` before using the brain.

```cpp
auto brain = BrainAPI::createDefault();
brain->initialize();  // <- This line was missing!
```

#### "Unknown feature"
**Solution**: Use one of the supported feature names.

```cpp
// Supported features:
brain->enableFeature("reward_modulation", true);
brain->enableFeature("structural_plasticity", true);
brain->enableFeature("development", true);
brain->enableFeature("curiosity", true);
brain->enableFeature("novelty", true);
```

#### "Configuration validation failed"
**Solution**: Check configuration values or load a valid profile.

```cpp
auto brain = BrainAPI::createDefault();
if (!brain->validateConfiguration()) {
    std::cerr << "Invalid configuration! Loading research profile instead." << std::endl;
    brain->loadProfile("research");
}
```

#### "File not found"
**Solution**: Ensure the file exists or use a relative path.

```cpp
brain->save("./checkpoints/my_brain.bin");  // Use relative path
```

### Error Codes

| Error Code | Description |
|------------|-------------|
| `1` | Brain initialization failed |
| `2` | Invalid configuration |
| `3` | File I/O error |
| `4` | Memory allocation failed |
| `5` | System initialization failed |

## Examples

### Complete Example: Research Simulation

```cpp
#include "BrainAPI.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "=== NLM Research Simulation ===" << std::endl;
    
    // 1. Create research brain
    auto brain = BrainAPI::createDefault();
    brain->loadProfile("research");
    
    // 2. Initialize
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize research brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Research brain initialized successfully!" << std::endl;
    
    // 3. Set up monitoring
    brain->setStepCallback([](int step) {
        if (step % 200 == 0) {
            auto stats = brainAPI.getStats();
            std::cout << "Progress: Step " << step << ", "
                      << "Spikes: " << stats.totalSpikes << std::endl;
        }
    });
    
    // 4. Run simulation
    std::cout << "Running research simulation..." << std::endl;
    brain->step(1000);
    
    // 5. Save results
    brain->save("research_simulation.bin");
    std::cout << "Research simulation completed and saved!" << std::endl;
    
    return 0;
}
```

### Complete Example: Development Study

```cpp
#include "BrainAPI.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== NLM Development Study ===" << std::endl;
    
    // Create minimal brain
    auto brain = BrainAPI::createDefault();
    brain->loadProfile("minimal");
    
    // Study development stages
    std::vector<std::string> stageNames = {
        "Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"
    };
    
    std::vector<float> firingRates;
    
    for (int stage = 0; stage <= 4; ++stage) {
        // Set development stage
        brain->setDevelopmentStage(static_cast<DevelopmentalStage>(stage));
        
        // Reset and initialize
        brain->reset();
        brain->initialize();
        
        // Run simulation
        brain->step(500);
        
        // Get statistics
        auto stats = brain->getStats();
        float rate = stats.avgFiringRate;
        firingRates.push_back(rate);
        
        std::cout << "Stage: " << stageNames[stage] << 
                  << " | Firing Rate: " << rate << 
                  << " | Spikes: " << stats.totalSpikes << std::endl;
    }
    
    // Analyze results
    std::cout << "\nDevelopment Study Results:" << std::endl;
    for (size_t i = 0; i < stageNames.size(); ++i) {
        std::cout << stageNames[i] << ": " << firingRates[i] << std::endl;
    }
    
    std::cout << "Development study completed successfully!" << std::endl;
    
    return 0;
}
```

## Best Practices

### 1. Always Initialize Before Use
```cpp
// ✓ Correct
auto brain = BrainAPI::createDefault();
if (!brain->initialize()) {
    // Handle error
}
brain->step(100);

// ✗ Incorrect
auto brain = BrainAPI::createDefault();
brain->step(100);  // Crashes!
```

### 2. Use Appropriate Profiles
```cpp
// For research: full features
brain->loadProfile("research");

// For production: optimized performance
brain->loadProfile("performance");

// For learning: adaptive behavior
brain->loadProfile("learning");

// For embedded: minimal resources
brain->loadProfile("minimal");
```

### 3. Validate Configuration
```cpp
if (!brain->validateConfiguration()) {
    std::cerr << "Invalid configuration!" << std::endl;
    return 1;
}
```

### 4. Handle Errors Gracefully
```cpp
if (!brain->initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    // Try to load saved state
    if (!brain->load("last_known_good.bin")) {
        std::cerr << "Could not recover!" << std::endl;
        return 1;
    }
}
```

### 5. Monitor and Log
```cpp
// Set up monitoring
brain->setStepCallback([](int step) {
    auto stats = brain->getStats();
    if (stats.totalSpikes > 1000) {
        std::cout << "High activity detected at step " << step << std::endl;
    }
});

// Log final status
brain->logStatus();
```

## Acknowledgments

This BrainAPI implementation is based on the work of the NLM research team and benefits from the contributions of:
- Core NLM developers for the original brain implementation
- Configuration system team for configuration management
- Memory system team for memory management
- Neuromodulation team for dopamine, curiosity, and novelty systems
- Development team for developmental plasticity
- Performance team for optimizations and parallel processing

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
