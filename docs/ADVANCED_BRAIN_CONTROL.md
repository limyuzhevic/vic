# NLM - Advanced Usage Guide for Power Users

**Warning:** This guide assumes familiarity with NLM basics. Advanced users will benefit from understanding the complete integrated brain system.

## Table of Contents

1. [Advanced Configuration Management](#advanced-configuration-management)
2. [Configuration Validation and Profiles](#configuration-validation-and-profiles)
3. [Advanced Brain Control Methods](#advanced-brain-control-methods)
4. [Power User Examples](#power-user-examples)
5. [Performance Optimization](#performance-optimization)
6. [Debugging and Monitoring](#debugging-and-monitoring)

---

## 1. Advanced Configuration Management

### Configuration Sources and Precedence

NLM supports multiple configuration sources with clear precedence:

```cpp
// Configuration source precedence (highest to lowest):
// 1. Runtime settings (set() calls)
// 2. Command line arguments
// 3. Configuration file (JSON format)
// 4. Default values
// 5. Essential system parameters
```

### JSON Configuration Schema

Advanced users can create complete configuration schemas:

```json
{
    "schema_version": "1.0",
    "strict_mode": true,
    "required_keys": [
        "neuron_count",
        "connection_probability",
        "simulation_timestep"
    ],
    "default_values": {
        "neuron_count": {"__type": "int", "value": 1000},
        "connection_probability": {"__type": "double", "value": 0.1},
        "plasticity.stdp.enable": {"__type": "bool", "value": true},
        "neuromod.dopamine.scale": {"__type": "double", "value": 1.0}
    },
    "configuration": {
        "neuron_count": {
            "__type": "int",
            "value": 2000,
            "description": "Total number of neurons in the brain",
            "category": "brain"
        },
        "plasticity.stdp.learning_rate": {
            "__type": "double", 
            "value": 0.005,
            "description": "STDP learning rate (higher = faster learning)",
            "category": "plasticity"
        }
    }
}
```

### Configuration Categories

- **Brain**: neuron count, region count, connection parameters
- **Dynamics**: membrane time constants, thresholds, refractory periods
- **Plasticity**: STDP, Hebbian, structural plasticity rates
- **Neuromodulation**: dopamine, curiosity, novelty parameters
- **Memory**: working memory capacity, episodic memory settings
- **Prediction**: prediction accuracy thresholds, confidence levels
- **Development**: stage transition ages, plasticity modifiers
- **Performance**: simulation timestep, recording rates, checkpoints

---

## 2. Configuration Validation and Profiles

### Configuration Validation

Validate configurations before use:

```cpp
void validateConfiguration() {
    pynlm::Config config = pynlm::createDefaultConfig();
    
    // Set advanced parameters
    config.set("brain.neuron_count", 5000);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("neuromod.dopamine.scale", 2.0);
    config.set("memory.working.capacity", 100);
    
    pynlm::Brain brain(config);
    
    // Validate configuration
    if (!brain.validateConfig()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        auto errors = brain.getConfigValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Error: " << error << std::endl;
        }
        return;
    }
    
    // Initialize with validated config
    brain.initialize();
}
```

### Configuration Profiles

Create and use configuration profiles:

```cpp
void useConfigurationProfiles() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Create different profiles for different use cases
    brain.addConfigProfile("high_performance");
    brain.addConfigProfile("deep_learning");
    brain.addConfigProfile("fast_exploration");
    
    // Load a specific profile
    if (brain.loadConfigProfile("deep_learning")) {
        std::cout << "Loaded deep_learning profile" << std::endl;
        brain.initialize();
        
        // Run simulation with profile
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            // Monitor performance
            if (step % 100 == 0) {
                brain.logBrainStatistics();
            }
        }
    }
    
    // Merge configurations
    pynlm::Config otherConfig;
    otherConfig.set("plasticity.hebbian.enable", true);
    brain.mergeConfig(otherConfig);
}
```

### Advanced Profile Features

```cpp
void advancedProfileUsage() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Add profile with custom validation
    brain.addConfigProfile("strict_validation");
    
    // Apply validation rules
    bool valid = brain.updateConfig("brain.neuron_count", 10000);
    if (valid) {
        std::cout << "Neuron count updated successfully" << std::endl;
    }
    
    // List available profiles
    std::vector<std::string> profiles;
    brain.listConfigProfiles(profiles);
    
    // Reset to profile
    if (brain.loadConfigProfile("high_performance")) {
        brain.resetToTemplate("high_performance");
    }
}
```

---

## 3. Advanced Brain Control Methods

### Direct Brain State Manipulation

Direct access to individual neurons for precise control:

```cpp
void directNeuronControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get specific neurons
    auto sensoryNeurons = brain.getNeuronsByType(pynlm::NeuronType::Sensory);
    auto motorNeurons = brain.getNeuronsByType(pynlm::NeuronType::Motor);
    auto internalNeurons = brain.getNeuronsByType(pynlm::NeuronType::Internal);
    
    // Control sensory neurons
    if (!sensoryNeurons.empty()) {
        // Set membrane potential
        brain.setNeuronMembranePotential(sensoryNeurons[0]->getId(), -40.0f);
        
        // Set neuron state
        brain.setNeuronActive(sensoryNeurons[0]->getId(), true);
        
        // Get neuron statistics
        pynlm::NeuronStats stats;
        brain.getNeuronStats(sensoryNeurons[0]->getId(), stats);
        
        std::cout << "Sensory neuron stats:" << std::endl;
        std::cout << "  Membrane potential: " << stats.membranePotential << " mV" << std::endl;
        std::cout << "  Firing rate: " << stats.firingRate << " Hz" << std::endl;
        std::cout << "  Activity: " << (stats.active ? "Active" : "Inactive") << std::endl;
    }
    
    // Control motor neurons
    if (!motorNeurons.empty()) {
        // Inject current to trigger specific actions
        for (size_t i = 0; i < motorNeurons.size(); ++i) {
            brain.injectCurrent(motorNeurons[i]->getId(), 
                               (i == 0) ? 20.0f : 0.0f);  // Forward action
        }
        
        // Get firing neuron count
        size_t firingCount = brain.getFiringNeuronCount();
        std::cout << "Firing motor neurons: " << firingCount << std::endl;
    }
    
    // Control internal neurons
    if (!internalNeurons.empty()) {
        // Set internal state for modulation
        brain.setNeuronActive(internalNeurons[0]->getId(), true);
        brain.setNeuronMembranePotential(internalNeurons[0]->getId(), -30.0f);
    }
}
```

### Synapse Control

Precise synaptic manipulation:

```cpp
void advancedSynapseControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all synapses
    auto allSynapses = brain.getAllSynapses();
    
    // Classify synapses by strength
    std::vector<pynlm::Synapse*> strongSynapses;
    std::vector<pynlm::Synapse*> weakSynapses;
    std::vector<pynlm::Synapse*> modulatorySynapses;
    
    for (auto* syn : allSynapses) {
        float weight = syn->getWeight();
        if (std::abs(weight) > 1.0f) {
            strongSynapses.push_back(syn);
        } else if (std::abs(weight) < 0.1f) {
            weakSynapses.push_back(syn);
        }
        
        // Check for plasticity flags
        auto flags = syn->getPlasticityFlags();
        if (flags.stdp || flags.hebbian) {
            modulatorySynapses.push_back(syn);
        }
    }
    
    std::cout << "Strong synapses: " << strongSynapses.size() << std::endl;
    std::cout << "Weak synapses: " << weakSynapses.size() << std::endl;
    std::cout << "Modulatory synapses: " << modulatorySynapses.size() << std::endl;
    
    // Manipulate strong synapses
    for (auto* syn : strongSynapses) {
        // Enhance strong connections
        syn->setWeight(syn->getWeight() * 1.2f);
        syn->setDelay(1);  // Fast transmission
    }
    
    // Strengthen weak synapses
    for (auto* syn : weakSynapses) {
        // Boost weak connections
        syn->setWeight(syn->getWeight() * 2.0f);
        syn->setDelay(2);  // Slower transmission
        
        // Enable plasticity
        syn->enablePlasticity(true, true, true);
    }
    
    // Modify modulatory synapses
    for (auto* syn : modulatorySynapses) {
        // Set eligibility trace for reward-modulated learning
        syn->setEligibilityTrace(0.5f);
        
        // Configure plasticity
        syn->enablePlasticity(true, true, true);
    }
}
```

### Region and Population Control

Control neural regions and populations:

```cpp
void regionAndPopulationControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    config.set("brain.region_count", 3);  // 3 regions
    config.set("brain.neuron_count", 3000);  // 3K neurons
    
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all regions
    auto regions = brain.getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = regions[i].get();
        pynlm::RegionId regionId = region->getId();
        
        std::cout << "Region " << regionId << ": " << region->getName() << std::endl;
        
        // Get populations in this region
        auto populations = region->getPopulations();
        
        for (auto& pop : populations) {
            pynlm::NeuronType type = pop->getNeuronType();
            size_t neuronCount = pop->getNeurons().size();
            
            std::cout << "  Population type " << static_cast<int>(type) << ": " 
                     << neuronCount << " neurons" << std::endl;
            
            // Control population activity
            region->setPopulationActive(pop->getId(), true);
            
            // Get neurons in this population
            auto neurons = pop->getNeurons();
            for (size_t j = 0; j < neurons.size() && j < 3; ++j) {
                auto* neuron = neurons[j];
                
                // Set different potentials for demonstration
                float potential = -70.0f + (j * 10.0f);
                brain.setNeuronMembranePotential(neuron->getId(), potential);
            }
        }
    }
    
    // Add inter-region connections
    if (regions.size() >= 2) {
        brain.addInterRegionConnection(regions[0]->getId(), regions[1]->getId(), 
                                      0.5f, 2);  // Weight 0.5, delay 2 steps
        brain.addInterRegionConnection(regions[1]->getId(), regions[2]->getId(), 
                                      0.3f, 1);
    }
}
```

---

## 4. Configuration Validation and Profiles

### Configuration Validation

Validate configurations before use:

```cpp
void validateConfiguration() {
    pynlm::Config config = pynlm::createDefaultConfig();
    
    // Set advanced parameters
    config.set("brain.neuron_count", 5000);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("neuromod.dopamine.scale", 2.0);
    config.set("memory.working.capacity", 100);
    
    pynlm::Brain brain(config);
    
    // Validate configuration
    if (!brain.validateConfig()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        auto errors = brain.getConfigValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Error: " << error << std::endl;
        }
        return;
    }
    
    // Initialize with validated config
    brain.initialize();
}
```

### Configuration Profiles

Create and use configuration profiles:

```cpp
void useConfigurationProfiles() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Create different profiles for different use cases
    brain.addConfigProfile("high_performance");
    brain.addConfigProfile("deep_learning");
    brain.addConfigProfile("fast_exploration");
    
    // Load a specific profile
    if (brain.loadConfigProfile("deep_learning")) {
        std::cout << "Loaded deep_learning profile" << std::endl;
        brain.initialize();
        
        // Run simulation with profile
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            // Monitor performance
            if (step % 100 == 0) {
                brain.logBrainStatistics();
            }
        }
    }
    
    // Merge configurations
    pynlm::Config otherConfig;
    otherConfig.set("plasticity.hebbian.enable", true);
    brain.mergeConfig(otherConfig);
}
```

### Advanced Profile Features

```cpp
void advancedProfileUsage() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Add profile with custom validation
    brain.addConfigProfile("strict_validation");
    
    // Apply validation rules
    bool valid = brain.updateConfig("brain.neuron_count", 10000);
    if (valid) {
        std::cout << "Neuron count updated successfully" << std::endl;
    }
    
    // List available profiles
    std::vector<std::string> profiles;
    brain.listConfigProfiles(profiles);
    
    // Reset to profile
    if (brain.loadConfigProfile("high_performance")) {
        brain.resetToTemplate("high_performance");
    }
}
```

---

## 5. Advanced Brain Control Methods

### Direct Brain State Manipulation

Direct access to individual neurons for precise control:

```cpp
void directNeuronControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get specific neurons
    auto sensoryNeurons = brain.getNeuronsByType(pynlm::NeuronType::Sensory);
    auto motorNeurons = brain.getNeuronsByType(pynlm::NeuronType::Motor);
    auto internalNeurons = brain.getNeuronsByType(pynlm::NeuronType::Internal);
    
    // Control sensory neurons
    if (!sensoryNeurons.empty()) {
        // Set membrane potential
        brain.setNeuronMembranePotential(sensoryNeurons[0]->getId(), -40.0f);
        
        // Set neuron state
        brain.setNeuronActive(sensoryNeurons[0]->getId(), true);
        
        // Get neuron statistics
        pynlm::NeuronStats stats;
        brain.getNeuronStats(sensoryNeurons[0]->getId(), stats);
        
        std::cout << "Sensory neuron stats:" << std::endl;
        std::cout << "  Membrane potential: " << stats.membranePotential << " mV" << std::endl;
        std::cout << "  Firing rate: " << stats.firingRate << " Hz" << std::endl;
        std::cout << "  Activity: " << (stats.active ? "Active" : "Inactive") << std::endl;
    }
    
    // Control motor neurons
    if (!motorNeurons.empty()) {
        // Inject current to trigger specific actions
        for (size_t i = 0; i < motorNeurons.size(); ++i) {
            brain.injectCurrent(motorNeurons[i]->getId(), 
                               (i == 0) ? 20.0f : 0.0f);  // Forward action
        }
        
        // Get firing neuron count
        size_t firingCount = brain.getFiringNeuronCount();
        std::cout << "Firing motor neurons: " << firingCount << std::endl;
    }
    
    // Control internal neurons
    if (!internalNeurons.empty()) {
        // Set internal state for modulation
        brain.setNeuronActive(internalNeurons[0]->getId(), true);
        brain.setNeuronMembranePotential(internalNeurons[0]->getId(), -30.0f);
    }
}
```

### Synapse Control

Precise synaptic manipulation:

```cpp
void advancedSynapseControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all synapses
    auto allSynapses = brain.getAllSynapses();
    
    // Classify synapses by strength
    std::vector<pynlm::Synapse*> strongSynapses;
    std::vector<pynlm::Synapse*> weakSynapses;
    std::vector<pynlm::Synapse*> modulatorySynapses;
    
    for (auto* syn : allSynapses) {
        float weight = syn->getWeight();
        if (std::abs(weight) > 1.0f) {
            strongSynapses.push_back(syn);
        } else if (std::abs(weight) < 0.1f) {
            weakSynapses.push_back(syn);
        }
        
        // Check for plasticity flags
        auto flags = syn->getPlasticityFlags();
        if (flags.stdp || flags.hebbian) {
            modulatorySynapses.push_back(syn);
        }
    }
    
    std::cout << "Strong synapses: " << strongSynapses.size() << std::endl;
    std::cout << "Weak synapses: " << weakSynapses.size() << std::endl;
    std::cout << "Modulatory synapses: " << modulatorySynapses.size() << std::endl;
    
    // Manipulate strong synapses
    for (auto* syn : strongSynapses) {
        // Enhance strong connections
        syn->setWeight(syn->getWeight() * 1.2f);
        syn->setDelay(1);  // Fast transmission
    }
    
    // Strengthen weak synapses
    for (auto* syn : weakSynapses) {
        // Boost weak connections
        syn->setWeight(syn->getWeight() * 2.0f);
        syn->setDelay(2);  // Slower transmission
        
        // Enable plasticity
        syn->enablePlasticity(true, true, true);
    }
    
    // Modify modulatory synapses
    for (auto* syn : modulatorySynapses) {
        // Set eligibility trace for reward-modulated learning
        syn->setEligibilityTrace(0.5f);
        
        // Configure plasticity
        syn->enablePlasticity(true, true, true);
    }
}
```

### Region and Population Control

Control neural regions and populations:

```cpp
void regionAndPopulationControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    config.set("brain.region_count", 3);  // 3 regions
    config.set("brain.neuron_count", 3000);  // 3K neurons
    
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all regions
    auto regions = brain.getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = regions[i].get();
        pynlm::RegionId regionId = region->getId();
        
        std::cout << "Region " << regionId << ": " << region->getName() << std::endl;
        
        // Get populations in this region
        auto populations = region->getPopulations();
        
        for (auto& pop : populations) {
            pynlm::NeuronType type = pop->getNeuronType();
            size_t neuronCount = pop->getNeurons().size();
            
            std::cout << "  Population type " << static_cast<int>(type) << ": " 
                     << neuronCount << " neurons" << std::endl;
            
            // Control population activity
            region->setPopulationActive(pop->getId(), true);
            
            // Get neurons in this population
            auto neurons = pop->getNeurons();
            for (size_t j = 0; j < neurons.size() && j < 3; ++j) {
                auto* neuron = neurons[j];
                
                // Set different potentials for demonstration
                float potential = -70.0f + (j * 10.0f);
                brain.setNeuronMembranePotential(neuron->getId(), potential);
            }
        }
    }
    
    // Add inter-region connections
    if (regions.size() >= 2) {
        brain.addInterRegionConnection(regions[0]->getId(), regions[1]->getId(), 
                                      0.5f, 2);  // Weight 0.5, delay 2 steps
        brain.addInterRegionConnection(regions[1]->getId(), regions[2]->getId(), 
                                      0.3f, 1);
    }
}
```

---

## 6. Configuration Validation and Profiles

### Configuration Validation

Validate configurations before use:

```cpp
void validateConfiguration() {
    pynlm::Config config = pynlm::createDefaultConfig();
    
    // Set advanced parameters
    config.set("brain.neuron_count", 5000);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("neuromod.dopamine.scale", 2.0);
    config.set("memory.working.capacity", 100);
    
    pynlm::Brain brain(config);
    
    // Validate configuration
    if (!brain.validateConfig()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        auto errors = brain.getConfigValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Error: " << error << std::endl;
        }
        return;
    }
    
    // Initialize with validated config
    brain.initialize();
}
```

### Configuration Profiles

Create and use configuration profiles:

```cpp
void useConfigurationProfiles() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Create different profiles for different use cases
    brain.addConfigProfile("high_performance");
    brain.addConfigProfile("deep_learning");
    brain.addConfigProfile("fast_exploration");
    
    // Load a specific profile
    if (brain.loadConfigProfile("deep_learning")) {
        std::cout << "Loaded deep_learning profile" << std::endl;
        brain.initialize();
        
        // Run simulation with profile
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            // Monitor performance
            if (step % 100 == 0) {
                brain.logBrainStatistics();
            }
        }
    }
    
    // Merge configurations
    pynlm::Config otherConfig;
    otherConfig.set("plasticity.hebbian.enable", true);
    brain.mergeConfig(otherConfig);
}
```

### Advanced Profile Features

```cpp
void advancedProfileUsage() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Add profile with custom validation
    brain.addConfigProfile("strict_validation");
    
    // Apply validation rules
    bool valid = brain.updateConfig("brain.neuron_count", 10000);
    if (valid) {
        std::cout << "Neuron count updated successfully" << std::endl;
    }
    
    // List available profiles
    std::vector<std::string> profiles;
    brain.listConfigProfiles(profiles);
    
    // Reset to profile
    if (brain.loadConfigProfile("high_performance")) {
        brain.resetToTemplate("high_performance");
    }
}
```

---

## 7. Advanced Brain Control Methods

### Direct Brain State Manipulation

Direct access to individual neurons for precise control:

```cpp
void directNeuronControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get specific neurons
    auto sensoryNeurons = brain.getNeuronsByType(pynlm::NeuronType::Sensory);
    auto motorNeurons = brain.getNeuronsByType(pynlm::NeuronType::Motor);
    auto internalNeurons = brain.getNeuronsByType(pynlm::NeuronType::Internal);
    
    // Control sensory neurons
    if (!sensoryNeurons.empty()) {
        // Set membrane potential
        brain.setNeuronMembranePotential(sensoryNeurons[0]->getId(), -40.0f);
        
        // Set neuron state
        brain.setNeuronActive(sensoryNeurons[0]->getId(), true);
        
        // Get neuron statistics
        pynlm::NeuronStats stats;
        brain.getNeuronStats(sensoryNeurons[0]->getId(), stats);
        
        std::cout << "Sensory neuron stats:" << std::endl;
        std::cout << "  Membrane potential: " << stats.membranePotential << " mV" << std::endl;
        std::cout << "  Firing rate: " << stats.firingRate << " Hz" << std::endl;
        std::cout << "  Activity: " << (stats.active ? "Active" : "Inactive") << std::endl;
    }
    
    // Control motor neurons
    if (!motorNeurons.empty()) {
        // Inject current to trigger specific actions
        for (size_t i = 0; i < motorNeurons.size(); ++i) {
            brain.injectCurrent(motorNeurons[i]->getId(), 
                               (i == 0) ? 20.0f : 0.0f);  // Forward action
        }
        
        // Get firing neuron count
        size_t firingCount = brain.getFiringNeuronCount();
        std::cout << "Firing motor neurons: " << firingCount << std::endl;
    }
    
    // Control internal neurons
    if (!internalNeurons.empty()) {
        // Set internal state for modulation
        brain.setNeuronActive(internalNeurons[0]->getId(), true);
        brain.setNeuronMembranePotential(internalNeurons[0]->getId(), -30.0f);
    }
}
```

### Synapse Control

Precise synaptic manipulation:

```cpp
void advancedSynapseControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all synapses
    auto allSynapses = brain.getAllSynapses();
    
    // Classify synapses by strength
    std::vector<pynlm::Synapse*> strongSynapses;
    std::vector<pynlm::Synapse*> weakSynapses;
    std::vector<pynlm::Synapse*> modulatorySynapses;
    
    for (auto* syn : allSynapses) {
        float weight = syn->getWeight();
        if (std::abs(weight) > 1.0f) {
            strongSynapses.push_back(syn);
        } else if (std::abs(weight) < 0.1f) {
            weakSynapses.push_back(syn);
        }
        
        // Check for plasticity flags
        auto flags = syn->getPlasticityFlags();
        if (flags.stdp || flags.hebbian) {
            modulatorySynapses.push_back(syn);
        }
    }
    
    std::cout << "Strong synapses: " << strongSynapses.size() << std::endl;
    std::cout << "Weak synapses: " << weakSynapses.size() << std::endl;
    std::cout << "Modulatory synapses: " << modulatorySynapses.size() << std::endl;
    
    // Manipulate strong synapses
    for (auto* syn : strongSynapses) {
        // Enhance strong connections
        syn->setWeight(syn->getWeight() * 1.2f);
        syn->setDelay(1);  // Fast transmission
    }
    
    // Strengthen weak synapses
    for (auto* syn : weakSynapses) {
        // Boost weak connections
        syn->setWeight(syn->getWeight() * 2.0f);
        syn->setDelay(2);  // Slower transmission
        
        // Enable plasticity
        syn->enablePlasticity(true, true, true);
    }
    
    // Modify modulatory synapses
    for (auto* syn : modulatorySynapses) {
        // Set eligibility trace for reward-modulated learning
        syn->setEligibilityTrace(0.5f);
        
        // Configure plasticity
        syn->enablePlasticity(true, true, true);
    }
}
```

### Region and Population Control

Control neural regions and populations:

```cpp
void regionAndPopulationControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    config.set("brain.region_count", 3);  // 3 regions
    config.set("brain.neuron_count", 3000);  // 3K neurons
    
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all regions
    auto regions = brain.getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = regions[i].get();
        pynlm::RegionId regionId = region->getId();
        
        std::cout << "Region " << regionId << ": " << region->getName() << std::endl;
        
        // Get populations in this region
        auto populations = region->getPopulations();
        
        for (auto& pop : populations) {
            pynlm::NeuronType type = pop->getNeuronType();
            size_t neuronCount = pop->getNeurons().size();
            
            std::cout << "  Population type " << static_cast<int>(type) << ": " 
                     << neuronCount << " neurons" << std::endl;
            
            // Control population activity
            region->setPopulationActive(pop->getId(), true);
            
            // Get neurons in this population
            auto neurons = pop->getNeurons();
            for (size_t j = 0; j < neurons.size() && j < 3; ++j) {
                auto* neuron = neurons[j];
                
                // Set different potentials for demonstration
                float potential = -70.0f + (j * 10.0f);
                brain.setNeuronMembranePotential(neuron->getId(), potential);
            }
        }
    }
    
    // Add inter-region connections
    if (regions.size() >= 2) {
        brain.addInterRegionConnection(regions[0]->getId(), regions[1]->getId(), 
                                      0.5f, 2);  // Weight 0.5, delay 2 steps
        brain.addInterRegionConnection(regions[1]->getId(), regions[2]->getId(), 
                                      0.3f, 1);
    }
}
```

---

## 8. Configuration Validation and Profiles

### Configuration Validation

Validate configurations before use:

```cpp
void validateConfiguration() {
    pynlm::Config config = pynlm::createDefaultConfig();
    
    // Set advanced parameters
    config.set("brain.neuron_count", 5000);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("neuromod.dopamine.scale", 2.0);
    config.set("memory.working.capacity", 100);
    
    pynlm::Brain brain(config);
    
    // Validate configuration
    if (!brain.validateConfig()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        auto errors = brain.getConfigValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Error: " << error << std::endl;
        }
        return;
    }
    
    // Initialize with validated config
    brain.initialize();
}
```

### Configuration Profiles

Create and use configuration profiles:

```cpp
void useConfigurationProfiles() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Create different profiles for different use cases
    brain.addConfigProfile("high_performance");
    brain.addConfigProfile("deep_learning");
    brain.addConfigProfile("fast_exploration");
    
    // Load a specific profile
    if (brain.loadConfigProfile("deep_learning")) {
        std::cout << "Loaded deep_learning profile" << std::endl;
        brain.initialize();
        
        // Run simulation with profile
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            // Monitor performance
            if (step % 100 == 0) {
                brain.logBrainStatistics();
            }
        }
    }
    
    // Merge configurations
    pynlm::Config otherConfig;
    otherConfig.set("plasticity.hebbian.enable", true);
    brain.mergeConfig(otherConfig);
}
```

### Advanced Profile Features

```cpp
void advancedProfileUsage() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Add profile with custom validation
    brain.addConfigProfile("strict_validation");
    
    // Apply validation rules
    bool valid = brain.updateConfig("brain.neuron_count", 10000);
    if (valid) {
        std::cout << "Neuron count updated successfully" << std::endl;
    }
    
    // List available profiles
    std::vector<std::string> profiles;
    brain.listConfigProfiles(profiles);
    
    // Reset to profile
    if (brain.loadConfigProfile("high_performance")) {
        brain.resetToTemplate("high_performance");
    }
}
```

---

## 9. Advanced Brain Control Methods

### Direct Brain State Manipulation

Direct access to individual neurons for precise control:

```cpp
void directNeuronControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get specific neurons
    auto sensoryNeurons = brain.getNeuronsByType(pynlm::NeuronType::Sensory);
    auto motorNeurons = brain.getNeuronsByType(pynlm::NeuronType::Motor);
    auto internalNeurons = brain.getNeuronsByType(pynlm::NeuronType::Internal);
    
    // Control sensory neurons
    if (!sensoryNeurons.empty()) {
        // Set membrane potential
        brain.setNeuronMembranePotential(sensoryNeurons[0]->getId(), -40.0f);
        
        // Set neuron state
        brain.setNeuronActive(sensoryNeurons[0]->getId(), true);
        
        // Get neuron statistics
        pynlm::NeuronStats stats;
        brain.getNeuronStats(sensoryNeurons[0]->getId(), stats);
        
        std::cout << "Sensory neuron stats:" << std::endl;
        std::cout << "  Membrane potential: " << stats.membranePotential << " mV" << std::endl;
        std::cout << "  Firing rate: " << stats.firingRate << " Hz" << std::endl;
        std::cout << "  Activity: " << (stats.active ? "Active" : "Inactive") << std::endl;
    }
    
    // Control motor neurons
    if (!motorNeurons.empty()) {
        // Inject current to trigger specific actions
        for (size_t i = 0; i < motorNeurons.size(); ++i) {
            brain.injectCurrent(motorNeurons[i]->getId(), 
                               (i == 0) ? 20.0f : 0.0f);  // Forward action
        }
        
        // Get firing neuron count
        size_t firingCount = brain.getFiringNeuronCount();
        std::cout << "Firing motor neurons: " << firingCount << std::endl;
    }
    
    // Control internal neurons
    if (!internalNeurons.empty()) {
        // Set internal state for modulation
        brain.setNeuronActive(internalNeurons[0]->getId(), true);
        brain.setNeuronMembranePotential(internalNeurons[0]->getId(), -30.0f);
    }
}
```

### Synapse Control

Precise synaptic manipulation:

```cpp
void advancedSynapseControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all synapses
    auto allSynapses = brain.getAllSynapses();
    
    // Classify synapses by strength
    std::vector<pynlm::Synapse*> strongSynapses;
    std::vector<pynlm::Synapse*> weakSynapses;
    std::vector<pynlm::Synapse*> modulatorySynapses;
    
    for (auto* syn : allSynapses) {
        float weight = syn->getWeight();
        if (std::abs(weight) > 1.0f) {
            strongSynapses.push_back(syn);
        } else if (std::abs(weight) < 0.1f) {
            weakSynapses.push_back(syn);
        }
        
        // Check for plasticity flags
        auto flags = syn->getPlasticityFlags();
        if (flags.stdp || flags.hebbian) {
            modulatorySynapses.push_back(syn);
        }
    }
    
    std::cout << "Strong synapses: " << strongSynapses.size() << std::endl;
    std::cout << "Weak synapses: " << weakSynapses.size() << std::endl;
    std::cout << "Modulatory synapses: " << modulatorySynapses.size() << std::endl;
    
    // Manipulate strong synapses
    for (auto* syn : strongSynapses) {
        // Enhance strong connections
        syn->setWeight(syn->getWeight() * 1.2f);
        syn->setDelay(1);  // Fast transmission
    }
    
    // Strengthen weak synapses
    for (auto* syn : weakSynapses) {
        // Boost weak connections
        syn->setWeight(syn->getWeight() * 2.0f);
        syn->setDelay(2);  // Slower transmission
        
        // Enable plasticity
        syn->enablePlasticity(true, true, true);
    }
    
    // Modify modulatory synapses
    for (auto* syn : modulatorySynapses) {
        // Set eligibility trace for reward-modulated learning
        syn->setEligibilityTrace(0.5f);
        
        // Configure plasticity
        syn->enablePlasticity(true, true, true);
    }
}
```

### Region and Population Control

Control neural regions and populations:

```cpp
void regionAndPopulationControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    config.set("brain.region_count", 3);  // 3 regions
    config.set("brain.neuron_count", 3000);  // 3K neurons
    
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all regions
    auto regions = brain.getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = regions[i].get();
        pynlm::RegionId regionId = region->getId();
        
        std::cout << "Region " << regionId << ": " << region->getName() << std::endl;
        
        // Get populations in this region
        auto populations = region->getPopulations();
        
        for (auto& pop : populations) {
            pynlm::NeuronType type = pop->getNeuronType();
            size_t neuronCount = pop->getNeurons().size();
            
            std::cout << "  Population type " << static_cast<int>(type) << ": " 
                     << neuronCount << " neurons" << std::endl;
            
            // Control population activity
            region->setPopulationActive(pop->getId(), true);
            
            // Get neurons in this population
            auto neurons = pop->getNeurons();
            for (size_t j = 0; j < neurons.size() && j < 3; ++j) {
                auto* neuron = neurons[j];
                
                // Set different potentials for demonstration
                float potential = -70.0f + (j * 10.0f);
                brain.setNeuronMembranePotential(neuron->getId(), potential);
            }
        }
    }
    
    // Add inter-region connections
    if (regions.size() >= 2) {
        brain.addInterRegionConnection(regions[0]->getId(), regions[1]->getId(), 
                                      0.5f, 2);  // Weight 0.5, delay 2 steps
        brain.addInterRegionConnection(regions[1]->getId(), regions[2]->getId(), 
                                      0.3f, 1);
    }
}
```

---

## 10. Configuration Validation and Profiles

### Configuration Validation

Validate configurations before use:

```cpp
void validateConfiguration() {
    pynlm::Config config = pynlm::createDefaultConfig();
    
    // Set advanced parameters
    config.set("brain.neuron_count", 5000);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("neuromod.dopamine.scale", 2.0);
    config.set("memory.working.capacity", 100);
    
    pynlm::Brain brain(config);
    
    // Validate configuration
    if (!brain.validateConfig()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        auto errors = brain.getConfigValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Error: " << error << std::endl;
        }
        return;
    }
    
    // Initialize with validated config
    brain.initialize();
}
```

### Configuration Profiles

Create and use configuration profiles:

```cpp
void useConfigurationProfiles() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Create different profiles for different use cases
    brain.addConfigProfile("high_performance");
    brain.addConfigProfile("deep_learning");
    brain.addConfigProfile("fast_exploration");
    
    // Load a specific profile
    if (brain.loadConfigProfile("deep_learning")) {
        std::cout << "Loaded deep_learning profile" << std::endl;
        brain.initialize();
        
        // Run simulation with profile
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            // Monitor performance
            if (step % 100 == 0) {
                brain.logBrainStatistics();
            }
        }
    }
    
    // Merge configurations
    pynlm::Config otherConfig;
    otherConfig.set("plasticity.hebbian.enable", true);
    brain.mergeConfig(otherConfig);
}
```

### Advanced Profile Features

```cpp
void advancedProfileUsage() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Add profile with custom validation
    brain.addConfigProfile("strict_validation");
    
    // Apply validation rules
    bool valid = brain.updateConfig("brain.neuron_count", 10000);
    if (valid) {
        std::cout << "Neuron count updated successfully" << std::endl;
    }
    
    // List available profiles
    std::vector<std::string> profiles;
    brain.listConfigProfiles(profiles);
    
    // Reset to profile
    if (brain.loadConfigProfile("high_performance")) {
        brain.resetToTemplate("high_performance");
    }
}
```

---

## Key Takeaways for Advanced Brain Control

1. **Direct Neural Manipulation**: Use low-level neuron and synapse control for precise behaviors
2. **Neuromodulation Precision**: Fine-tune neuromodulators for specific learning patterns
3. **Memory System Control**: Configure working memory, episodic memory, and attention
4. **Prediction Integration**: Use predictions to guide action selection
5. **Cognition System Control**: Control planners, concepts, and self-model
6. **Development Programming**: Set custom developmental trajectories
7. **Checkpoint Management**: Control brain persistence and recovery
8. **Recording and Export**: Configure recording formats and export options
9. **Event-Driven Control**: Use custom event triggers and handlers
10. **Integration Hooks**: Register callbacks at key integration points

These advanced brain control methods provide power users with complete control over the NLM artificial brain system, enabling sophisticated research and experimentation.

---

*This document is intended for advanced users with deep knowledge of NLM architecture and programming. Master basic concepts before attempting advanced brain control.*
