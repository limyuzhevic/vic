# NLM - Advanced Usage Guide

This guide covers advanced features and usage patterns for NLM beyond the basic introduction.

## Configuration Management

### Advanced Config Operations

#### Configuration Templates
```cpp
void setupAdvancedConfig(nlm::Config& config) {
    // Learning configuration
    config.set("plasticity_learning_rate", 0.05f);
    config.set("stdp_ltp_weight", 0.05f);
    config.set("stdp_ltd_weight", 0.0475f);
    
    // Structural plasticity
    config.set("synaptogenesis_rate", 0.002f);
    config.set("pruning_rate", 0.00005f);
    
    // Development
    config.set("development_stage", 3);  // Phase 3/4
    config.set("development_maturation_rate", 0.1f);
    
    // Neuromodulation
    config.set("dopamine_baseline", 0.1f);
    config.set("curiosity_strength", 0.5f);
    config.set("novelty_threshold", 0.3f);
    
    // Memory systems
    config.set("working_memory_capacity", 100);
    config.set("episodic_memory_span", 50);
    
    // Prediction
    config.set("prediction_horizon", 10);
    config.set("prediction_error_decay", 0.9f);
}
```

#### Configuration Validation
```cpp
bool validateConfig(const nlm::Config& config) {
    auto neuron_count = config.getOr<int64_t>("neuron_count", 100);
    auto region_count = config.getOr<int64_t>("region_count", 1);
    
    // Ensure valid ranges
    if (neuron_count < 1 || neuron_count > 100000) return false;
    if (region_count < 1 || region_count > 1000) return false;
    
    // Check plasticity parameters
    auto ltp_weight = config.getOr<float>("stdp_ltp_weight", 0.01f);
    auto ltd_weight = config.getOr<float>("stdp_ltd_weight", 0.012f);
    
    if (ltp_weight <= 0.0f || ltd_weight <= 0.0f) return false;
    if (std::abs(ltp_weight - ltd_weight) > 0.1f) return false;
    
    return true;
}
```

## Performance Optimization

### Brain Performance Tuning

#### Reduce Memory Footprint
```cpp
cpp
// Use sparse connectivity for large networks
config.set("connection_probability", 0.01f);  // 1% connectivity

// Enable memory pool optimizations
config.set("use_memory_pool", true);
config.set("memory_pool_size", 10000);

// Enable sparse connectivity
config.set("use_sparse_connectivity", true);
config.set("connectivity_threshold", 0.05f);
```

#### Enable Parallel Processing
```cpp
// For multi-core systems
config.set("enable_parallel_processing", true);
config.set("max_threads", std::thread::hardware_concurrency());

// Configure parallel regions
config.set("parallel_region_count", 4);
config.set("region_parallel_threshold", 1000);
```

#### Performance Monitoring
```cpp
void benchmarkBrainPerformance(const nlm::Config& config) {
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Warm-up
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    // Benchmark learning
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 100; i < 1000; ++i) {
        // Apply learning stimuli
        for (int j = 0; j < 10; ++j) {
            brain->injectCurrentToNeurons(
                static_cast<nlm::NeuronType>(j % 3), 10.0f
            );
        }
        
        brain->step(i);
        
        // Log progress
        if (i % 100 == 0) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - start_time).count();
            std::cout << "Step " << i << ", "
                     << "Spikes: " << brain->getTotalSpikeCount() << ", "
                     << "Time: " << duration << "ms, "
                     << "Rate: " << (i * 1000.0 / duration) << " steps/s"
                     << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time).count();
    
    std::cout << "Benchmark complete: "
             << total_duration << "ms for 900 steps, "
             << (900 * 1000.0 / total_duration) << " steps/sec"
             << std::endl;
}
```

## Advanced Features

### Neuromodulation Configuration

#### Reward-Modulated Plasticity
```cpp
void setupRewardModulatedLearning(nlm::Brain& brain) {
    // Enable reward-modulated STDP
    brain.getSTDP()->enableRewardModulation(true);
    brain.getSTDP()->setRewardPredictionError(true);
    
    // Configure dopamine signals
    auto neuromodulator = brain.getDopamine();
    neuromodulator->setBaseline(0.1f);
    neuromodulator->setLearningRate(0.05f);
    neuromodulator->enableCuriosity(0.3f);
    
    // Enable novelty detection
    auto novelty = brain.getNovelty();
    novelty->setSensitivity(0.5f);
    novelty->setAdaptationRate(0.01f);
}
```

### Development System Control

#### Phase-Based Development
```cpp
void controlDevelopment(nlm::Brain& brain) {
    // Set developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Phase3);
    
    // Configure development rates
    auto dev_system = brain.getDevelopmentSystem();
    dev_system->setSynaptogenesisRate(0.001f);
    dev_system->setPruningRate(0.0001f);
    dev_system->setMaturationRate(0.05f);
    
    // Enable developmental milestones
    dev_system->enablePlasticityMaturation(true);
    dev_system->enableCircuitRefinement(true);
    dev_system->enableBehavioralSpecialization(true);
}
```

### Memory System Configuration

#### Working Memory Optimization
```cpp
void optimizeWorkingMemory(nlm::Brain& brain) {
    auto working_memory = brain.getWorkingMemory();
    working_memory->setCapacity(200);
    working_memory->setDecayRate(0.95f);
    working_memory->setCompetitionStrength(0.8f);
    
    // Enable persistent activity
    working_memory->enablePersistentActivity(true);
    working_memory->setMaintenanceThreshold(0.7f);
}

void optimizeEpisodicMemory(nlm::Brain& brain) {
    auto episodic_memory = brain.getEpisodicMemory();
    episodic_memory->setEncodingStrength(0.8f);
    episodic_memory->setStorageCapacity(1000);
    episodic_memory->setReconstructionAccuracy(0.9f);
    
    // Enable replay mechanism
    episodic_memory->enableReplayMechanism(true);
    episodic_memory->setReplayFrequency(0.1f);
}
```

## Multi-Agent System

### Running Multiple Brains

#### Parallel Agent Execution
```cpp
void runMultiAgentSimulation(const std::vector<nlm::Config>& configs) {
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    
    // Create all brains
    for (const auto& config : configs) {
        auto brain = std::make_shared<nlm::Brain>(config);
        if (brain->initialize()) {
            brains.push_back(brain);
        }
    }
    
    std::cout << "Initialized " << brains.size() << " brains" << std::endl;
    
    // Synchronized simulation steps
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Apply stimuli to each brain
        for (size_t i = 0; i < brains.size(); ++i) {
            // Different patterns for each agent
            brains[i]->injectCurrentToNeurons(
                static_cast<nlm::NeuronType>(i % 3), 
                10.0f * (1.0f + 0.1f * std::sin(step * 0.01f))
            );
        }
        
        // Step all brains simultaneously
        for (auto& brain : brains) {
            brain->step(step);
        }
        
        // Log progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": Agents active: "
                     << std::count_if(brains.begin(), brains.end(),
                           [](const auto& brain) {
                               return brain->getFiringNeuronCount() > 0;
                           })
                     << "/" << brains.size()
                     << std::endl;
        }
    }
}
```

## CLI Interface (Future Enhancement)

### Command Line Options
```bash
# Basic usage
nlm --config configs/default.cfg --steps 1000

# Advanced configuration
nlm --config configs/advanced.cfg \
    --neurons 5000 \
    --regions 3 \
    --plasticity \
    --development \
    --neuromodulation

# Performance mode
nlm --performance \
    --threads 8 \
    --memory-pool \
    --benchmark

# Learning experiment
nlm --experiment learning \
    --stimulus-pattern correlated \
    --duration 5000 \
    --record-behavior
```

### Configuration File Format (JSON)
```json
{
    "nlm": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "plasticity": {
            "learning_rate": 0.01,
            "stdp": {
                "ltp_weight": 0.02,
                "ltd_weight": 0.015
            },
            "structural": {
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        },
        "development": {
            "stage": "Phase3",
            "maturation_rate": 0.1
        }
    }
}
```

## Debugging and Troubleshooting

### Common Issues and Solutions

#### 1. No Learning Detected
```cpp
void debugNoLearning(nlm::Brain& brain) {
    // Check plasticity is enabled
    bool hebbian = brain.getHebbian()->isEnabled();
    bool stdp = brain.getSTDP()->isEnabled();
    bool reward = brain.getDopamine()->isRewardModulated();
    
    std::cout << "Plasticity enabled: Hebbian=" << hebbian 
             << ", STDP=" << stdp 
             << ", Reward=" << reward << std::endl;
    
    // Check neuron activity
    std::cout << "Active neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    // Check configuration
    auto ltp_weight = brain.getConfig()->getOr<float>("stdp_ltp_weight", 0.01f);
    std::cout << "STDP LTP weight: " << ltp_weight << std::endl;
}
```

#### 2. High Memory Usage
```cpp
void debugMemoryUsage(nlm::Brain& brain) {
    auto regions = brain.getRegions();
    size_t total_neurons = 0;
    size_t total_synapses = 0;
    
    for (const auto& region : regions) {
        total_neurons += region->getNeuronCount();
        total_synapses += region->getSynapseCount();
    }
    
    std::cout << "Memory usage estimate:" << std::endl;
    std::cout << "  Neurons: " << total_neurons * sizeof(nlm::Neuron) / 1024 << " KB" << std::endl;
    std::cout << "  Synapses: " << total_synapses * sizeof(nlm::Synapse) / 1024 << " KB" << std::endl;
    std::cout << "  Total: " << (total_neurons * sizeof(nlm::Neuron) + total_synapses * sizeof(nlm::Synapse)) / 1024 / 1024 << " MB" << std::endl;
}
```

## Advanced Experimentation

### Custom Learning Protocols
```cpp
cpp
void customLearningProtocol(nlm::Brain& brain) {
    enum class LearningPhase {
        InitialState,
        StimulationPhase,
        ConsolidationPhase,
        TestingPhase
    };
    
    LearningPhase phase = LearningPhase::InitialState;
    size_t phase_steps = 0;
    
    for (nlm::SimulationStep step = 0; step < 5000; ++step) {
        switch (phase) {
            case LearningPhase::InitialState:
                // Record initial state
                if (step == 0) {
                    brain.recordInitialState();
                }
                // Apply structured stimulation
                if (step >= 100 && step < 300) {
                    brain.injectCurrentToNeurons(
                        nlm::NeuronType::Sensory, 
                        20.0f * std::sin(step * 0.02f)
                    );
                }
                break;
                
            case LearningPhase::StimulationPhase:
                // High-frequency stimulation for LTP
                brain.injectCurrentToNeurons(
                    nlm::NeuronType::Excitatory, 
                    50.0f
                );
                break;
                
            case LearningPhase::ConsolidationPhase:
                // Low-frequency stimulation for LTD
                brain.injectCurrentToNeurons(
                    nlm::NeuronType::Inhibitory, 
                    30.0f
                );
                break;
                
            case LearningPhase::TestingPhase:
                // Test with novel stimuli
                brain.injectCurrentToNeurons(
                    static_cast<nlm::NeuronType>(step % 3), 
                    15.0f
                );
                break;
        }
        
        brain.step(step);
        phase_steps++;
        
        // Phase transitions
        if (phase_steps >= 200) {
            phase = static_cast<LearningPhase>((static_cast<int>(phase) + 1) % 4);
            phase_steps = 0;
        }
    }
    
    brain.recordFinalState();
    brain.computeStatistics();
}
```

This guide provides comprehensive coverage of NLM's advanced features, helping users get the most out of this powerful neural simulation platform.