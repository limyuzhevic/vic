# NLM Project - Advanced Guide and Examples

This document provides advanced documentation for the NLM project, including detailed examples, parameter tuning, and best practices for power users and researchers.

## Advanced Configuration Examples

### Example 1: Advanced STDP Configuration

This example demonstrates how to configure advanced STDP parameters for specific learning behaviors:

```cpp
// Advanced STDP configuration for research
auto config = std::make_shared<nlm::Config>();

// Set up basic parameters
config->set("random_seed", 12345);
config->set("neuron_count", 2000);
config->set("simulation_timestep", 0.0005);

// Configure asymmetric STDP for specific learning patterns
config->set("stdp_ltp_weight", 0.015);      // Stronger potentiation
config->set("stdp_ltd_weight", 0.010);      // Weaker depression
config->set("stdp_time_constant", 25.0);    // Longer time constant

// Add eligibility traces for reward-modulated STDP
config->set("eligibility_decay", 0.03);     // Slower decay

// Configure structural plasticity
config->set("synaptogenesis_rate", 0.0005);  // Higher synaptogenesis
config->set("pruning_rate", 0.00005);         // Lower pruning rate

// Configure development
config->set("development_synaptogenesis_rate", 0.001);
config->set("development_pruning_rate", 0.0001);

// Configure neuromodulation
config->set("dopamine_baseline", 0.05);
config->set("reward_discount_factor", 0.95);

// Load advanced configuration
brain->initialize();
```

### Example 2: Real-time Learning with Neuromodulation

This example demonstrates real-time learning with various neuromodulation systems:

```cpp
void realTimeLearningExperiment() {
    // Create brain with advanced configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1500);
    config->set("simulation_timestep", 0.001);
    config->set("enable_reward_modulation", true);
    config->set("enable_curiosity", true);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(30, 30, 12, 12);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Configure neuromodulation systems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    
    std::cout << "=== Real-time Learning Experiment ===" << std::endl;
    std::cout << "Duration: 60 seconds" << std::endl;
    std::cout << "Neuron count: " << brain->getTotalNeuronCount() << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    int step = 0;
    
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime).count() < 60) {
        
        // Update world
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        // Get and execute action
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Calculate and apply rewards
        float reward = calculateLearningReward(world, agent);
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        // Update development system
        agent->updateDevelopment(0.1);
        
        // Log progress every 10 seconds
        if (step % 100 == 0) {
            auto status = brain->getStatistics();
            std::cout << "Step " << step << ": "
                      << "Firing=" << status.firingNeurons << " "
                      << "Spikes=" << status.totalSpikes << " "
                      << "Rate=" << status.firingRate << " Hz "
                      << "Dopamine=" << agent->getNeuromodulationLevel() << " "
                      << "Curiosity=" << agent->getCuriosityLevel() << " "
                      << "Development=" << agent->getDevelopmentalStage() << std::endl;
        }
        
        step++;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Export trained model
    brain->save("real_time_learning_model.json");
    std::cout << "Model exported to: real_time_learning_model.json" << std::endl;
}
```

### Example 3: Batch Processing with Parameter Sweeps

This example demonstrates batch processing with parameter sweeps:

```cpp
void batchParameterSweep() {
    std::vector<std::string> configs = {
        "config_low_ltp.json",
        "config_high_ltp.json",
        "config_fast_learning.json",
        "config_slow_learning.json",
        "config_no_structural.json",
        "config_max_structural.json"
    };
    
    std::cout << "=== Batch Parameter Sweep ===" << std::endl;
    std::cout << "Running " << configs.size() << " configurations..." << std::endl;
    
    std::vector<BatchResult> results;
    
    for (const auto& configFile : configs) {
        std::cout << "\nRunning: " << configFile << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        if (config->loadFromFile(configFile)) {
            auto brain = std::make_shared<nlm::Brain>(config);
            if (brain->initialize()) {
                // Run simulation with logging
                BatchResult result;
                result.configFile = configFile;
                result.startTime = std::chrono::high_resolution_clock::now();
                
                for (int step = 0; step < 5000; ++step) {
                    brain->step(step);
                    if (step % 1000 == 0) {
                        auto status = brain->getStatistics();
                        std::cout << "  Step " << step << ": "
                                  << "Firing=" << status.firingNeurons << " "
                                  << "Rate=" << status.firingRate << " Hz" << std::endl;
                    }
                }
                
                result.endTime = std::chrono::high_resolution_clock::now();
                result.totalSpikes = brain->getTotalSpikeCount();
                result.finalFiringRate = brain->getAverageFiringRate();
                
                // Save results
                std::string resultFile = "result_" + 
                    std::filesystem::path(configFile).stem().string() + ".json";
                brain->save(resultFile);
                result.outputFile = resultFile;
                
                results.push_back(result);
                
                std::cout << "  Completed in " 
                          << std::chrono::duration_cast<std::chrono::seconds>(
                              result.endTime - result.startTime).count() 
                          << " seconds" << std::endl;
                std::cout << "  Total spikes: " << result.totalSpikes << std::endl;
            }
        }
    }
    
    // Generate summary report
    generateBatchReport(results);
    
    // Create comparison plot
    generateComparisonPlot(results);
}
```

### Example 4: Performance Benchmarking

This example demonstrates performance benchmarking:

```cpp
void performanceBenchmark() {
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    
    struct BenchmarkConfig {
        std::string name;
        int neuronCount;
        int maxSteps;
        double timestep;
        bool enablePlasticity;
        bool enableDevelopment;
    };
    
    std::vector<BenchmarkConfig> benchmarks = {
        {"Small network", 100, 1000, 0.001, false, false},
        {"Medium network", 1000, 5000, 0.001, true, false},
        {"Large network", 5000, 10000, 0.0005, true, true},
        {"High plasticity", 2000, 8000, 0.001, true, true},
        {"Development", 3000, 12000, 0.0005, true, true}
    };
    
    std::vector<BenchmarkResult> results;
    
    for (const auto& bench : benchmarks) {
        std::cout << "\nRunning benchmark: " << bench.name << std::endl;
        std::cout << "  Neurons: " << bench.neuronCount << " "
                  << "Steps: " << bench.maxSteps << " "
                  << "Timestep: " << bench.timestep << " "
                  << "Plasticity: " << (bench.enablePlasticity ? "ON" : "OFF") << " "
                  << "Development: " << (bench.enableDevelopment ? "ON" : "OFF") << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", bench.neuronCount);
        config->set("simulation_timestep", bench.timestep);
        config->set("plasticity_learning_rate", 0.01);
        config->set("synaptogenesis_rate", 0.0001);
        config->set("pruning_rate", 0.00001);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        BenchmarkResult result;
        result.config = bench;
        result.startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < bench.maxSteps; ++step) {
            brain->step(step);
        }
        
        result.endTime = std::chrono::high_resolution_clock::now();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.finalFiringRate = brain->getAverageFiringRate();
        
        double duration = std::chrono::duration_cast<std::chrono::seconds>(
            result.endTime - result.startTime).count();
        double spikesPerSecond = result.totalSpikes / std::max(1.0, duration);
        double neuronsPerSecond = bench.neuronCount / duration;
        
        result.spikesPerSecond = spikesPerSecond;
        result.neuronsPerSecond = neuronsPerSecond;
        
        results.push_back(result);
        
        std::cout << "  Time: " << duration << " seconds" << std::endl;
        std::cout << "  Total spikes: " << result.totalSpikes << std::endl;
        std::cout << "  Spikes/sec: " << spikesPerSecond << " "
                  << "Neurons/sec: " << neuronsPerSecond << std::endl;
    }
    
    // Generate performance report
    generatePerformanceReport(results);
}
```

### Example 5: Visualization Script

This example demonstrates how to generate visualizations:

```cpp
void generateVisualizations() {
    std::cout << "=== NLM Visualization Generation ===" << std::endl;
    
    // Load trained model
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>());
    if (brain->load("trained_brain.json")) {
        std::cout << "Model loaded successfully" << std::endl;
        
        // Generate connectivity matrix
        generateConnectivityMatrix(brain);
        
        // Generate spiking activity plot
        generateSpikingActivityPlot(brain);
        
        // Generate plasticity evolution
        generatePlasticityEvolution(brain);
        
        // Generate performance charts
        generatePerformanceCharts(brain);
        
        // Generate network topology
        generateNetworkTopology(brain);
        
        // Generate activity heatmap
        generateActivityHeatmap(brain);
        
        // Generate time series plots
        generateTimeSeriesPlots(brain);
        
        std::cout << "All visualizations generated successfully" << std::endl;
    } else {
        std::cerr << "Failed to load model" << std::endl;
    }
}
```

## Neural Dynamics Configuration Examples

### Example 1: LIF Neuron Parameters

```cpp
// Configure LIF neuron dynamics for different behaviors
struct LIFConfig {
    float membraneTimeConstant;    // τ in ms
    float membraneResistance;       // R in MOhm
    float threshold;                // V_th in mV
    float resetPotential;          // V_reset in mV
    float restingPotential;        // E_rest in mV
    bool adaptiveThreshold;         // Adaptive threshold
    float adaptationTimeConstant;   // τ_ada in ms
    float refractoryPeriod;         // Refractory period in ms
};

LIFConfig createFastLIF() {
    LIFConfig config;
    config.membraneTimeConstant = 10.0f;    // Fast membrane
    config.membraneResistance = 5.0f;        // Low resistance
    config.threshold = 15.0f;                // Lower threshold
    config.resetPotential = -70.0f;          // Reset voltage
    config.restingPotential = -65.0f;        // Resting potential
    config.adaptiveThreshold = true;          // Adaptive threshold
    config.adaptationTimeConstant = 50.0f;    // Fast adaptation
    config.refractoryPeriod = 2.0f;           // Short refractory
    return config;
}

LIFConfig createSlowLIF() {
    LIFConfig config;
    config.membraneTimeConstant = 30.0f;    // Slow membrane
    config.membraneResistance = 20.0f;        // High resistance
    config.threshold = 25.0f;                // Higher threshold
    config.resetPotential = -60.0f;          // Higher reset voltage
    config.restingPotential = -65.0f;        // Resting potential
    config.adaptiveThreshold = false;         // No adaptation
    config.adaptationTimeConstant = 200.0f;   // Slow adaptation
    config.refractoryPeriod = 5.0f;           // Longer refractory
    return config;
}
```

### Example 2: Synaptic Dynamics

```cpp
// Configure synaptic dynamics for different connection types
struct SynapticConfig {
    std::string type;           // "AMPA", "NMDA", "GABA", "GABAA", "GABAB"
    float weight;               // Synaptic weight
    float delay;                // Transmission delay in ms
    float reversalPotential;    // E_rev in mV
    float timeConstant;         // τ for conductance dynamics
    float riseTime;             // α for rise dynamics
    bool shortTermPlasticity;   // Enable STP
    float facilitation;        // Facilitation factor
    float depression;           // Depression factor
};

SynapticConfig createExcitatorySynapse() {
    SynapticConfig config;
    config.type = "AMPA";
    config.weight = 0.5f;
    config.delay = 1.0f;
    config.reversalPotential = 0.0f;
    config.timeConstant = 5.0f;
    config.riseTime = 1.0f;
    config.shortTermPlasticity = true;
    config.facilitation = 1.2f;
    config.depression = 0.95f;
    return config;
}

SynapticConfig createInhibitorySynapse() {
    SynapticConfig config;
    config.type = "GABAA";
    config.weight = -0.8f;
    config.delay = 0.5f;
    config.reversalPotential = -70.0f;
    config.timeConstant = 10.0f;
    config.riseTime = 0.5f;
    config.shortTermPlasticity = true;
    config.facilitation = 1.1f;
    config.depression = 0.98f;
    return config;
}
```

## Advanced Neural Networks

### Example 1: Recurrent Network Configuration

```cpp
void configureRecurrentNetwork() {
    // Create brain for recurrent network
    auto config = std::make_shared<nlm::Config>();
    
    // Configuration for recurrent network
    config->set("neuron_count", 3000);                    // Many neurons
    config->set("region_count", 3);                       // Multiple regions
    config->set("simulation_timestep", 0.0005);           // Fast timestep
    config->set("connection_probability", 0.25);          // High connectivity
    config->set("random_seed", 42);                       // Reproducible
    
    // STDP parameters for recurrent learning
    config->set("stdp_ltp_weight", 0.02);
    config->set("stdp_ltd_weight", 0.02);
    config->set("stdp_time_constant", 30.0);
    
    // Plasticity for recurrent networks
    config->set("plasticity_learning_rate", 0.015);
    config->set("synaptogenesis_rate", 0.001);
    config->set("pruning_rate", 0.0001);
    
    // Development for recurrent networks
    config->set("development_synaptogenesis_rate", 0.002);
    config->set("development_pruning_rate", 0.0002);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Add recurrent connections between regions
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i != j) {
                brain->addInterRegionConnection(i, j, 0.1f, 5);
            }
        }
    }
    
    // Create world for recurrent network
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(25, 25, 10, 10);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable all learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    
    // Run recurrent network simulation
    for (int step = 0; step < 10000; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward for learning
        float reward = calculateNetworkReward(world, agent);
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        if (step % 1000 == 0) {
            auto status = brain->getStatistics();
            std::cout << "Step " << step << ": "
                      << "Firing=" << status.firingNeurons << " "
                      << "Rate=" << status.firingRate << " Hz "
                      << "Memory=" << status.memoryUsage << " MB" << std::endl;
        }
    }
}
```

### Example 2: Feedforward Network Configuration

```cpp
void configureFeedforwardNetwork() {
    // Create brain for feedforward network
    auto config = std::make_shared<nlm::Config>();
    
    // Configuration for feedforward network
    config->set("neuron_count", 2000);                    // Many neurons
    config->set("region_count", 2);                       // Two regions
    config->set("simulation_timestep", 0.001);           // Normal timestep
    config->set("connection_probability", 0.1);          // Moderate connectivity
    
    // STDP parameters for feedforward learning
    config->set("stdp_ltp_weight", 0.01);
    config->set("stdp_ltd_weight", 0.01);
    config->set("stdp_time_constant", 20.0);
    
    // Limited plasticity for feedforward
    config->set("plasticity_learning_rate", 0.005);
    config->set("synaptogenesis_rate", 0.0005);
    config->set("pruning_rate", 0.00001);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Add feedforward connections from region 0 to region 1
    brain->addInterRegionConnection(0, 1, 0.5f, 2);
    
    // Create world for feedforward network
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Limited learning for feedforward
    agent->enableRewardModulation(true);     // Reward-based learning
    agent->enableCuriosity(false);          // No curiosity exploration
    agent->enableStructuralPlasticity(false); // No structural changes
    agent->enableDevelopment(false);          // No development
    
    // Run feedforward network simulation
    for (int step = 0; step < 5000; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Simple reward based on action
        float reward = (action == nlm::MotorCommand::Interact) ? 1.0f : 0.0f;
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        if (step % 1000 == 0) {
            auto status = brain->getStatistics();
            std::cout << "Step " << step << ": "
                      << "Firing=" << status.firingNeurons << " "
                      << "Rate=" << status.firingRate << " Hz" << std::endl;
        }
    }
}
```

## Best Practices and Guidelines

### 1. Configuration Best Practices

#### Do's
- Always set a random seed for reproducible experiments
- Use appropriate neuron counts for your system resources
- Choose simulation timesteps that balance accuracy and performance
- Enable plasticity early in training for better learning
- Monitor memory usage during long simulations

#### Don'ts
- Don't use extremely high neuron counts without sufficient memory
- Don't skip configuration validation
- Don't disable logging during debugging
- Don't enable all plasticity rules simultaneously
- Don't run large networks without monitoring

### 2. Performance Optimization

#### CPU Optimization
- Use larger timesteps when accuracy is not critical
- Disable unnecessary plasticity rules
- Use optimized configuration files
- Monitor CPU usage during long simulations

#### Memory Optimization
- Use appropriate neuron and synapse counts
- Monitor memory usage with `--profile-memory`
- Use efficient data structures
- Consider memory-efficient configurations

#### I/O Optimization
- Use disk I/O only when necessary
- Cache frequently accessed data
- Use efficient file formats
- Limit log file sizes

### 3. Simulation Best Practices

#### Common Pitfalls

1. **Overfitting**: Network memorizes training data instead of generalizing
   - Solution: Use validation data and regularization

2. **Runaway Excitatory Activity**: Network becomes unstable
   - Solution: Add inhibitory connections and balance E/I

3. **Memory Exhaustion**: Simulation runs out of memory
   - Solution: Reduce network size and complexity

4. **Slow Learning**: Network learns too slowly
   - Solution: Adjust learning rates and plasticity parameters

5. **Poor Generalization**: Network fails on new data
   - Solution: Use diverse training data and regularization

#### Mitigation Strategies

1. **Network Architecture**: Use balanced excitation and inhibition
2. **Learning Parameters**: Tune plasticity rules and learning rates
3. **Simulation Parameters**: Adjust timesteps and network sizes
4. **Regularization**: Use dropout and weight decay
5. **Monitoring**: Monitor performance during training

### 4. Debugging Guidelines

#### Common Issues and Solutions

| Issue | Symptoms | Solution |
|-------|----------|----------|
| Brain initialization fails | No output, hangs | Check configuration, check memory |
| No learning occurs | Weights don't change | Enable plasticity, check firing |
| High memory usage | OOM error | Reduce network size |
| Slow performance | Long execution time | Increase timestep, reduce neurons |
| Visualization fails | No output files | Check dependencies, check paths |

#### Debugging Commands

```bash
# Enable verbose logging
./nlm --debug-level verbose

# Profile memory usage
./nlm --profile-memory

# Trace spike activity
./nlm --trace-spikes

# Monitor performance in real-time
./nlm --monitor

# Validate configuration
./nlm --validate-config config.json

# List all configuration options
./nlm --list-config-options
```

### 5. Advanced Tips

#### Network Design

1. **Layered Architecture**: Use multiple regions for hierarchical processing
2. **Recurrent Connections**: Add feedback loops for temporal processing
3. **Balanced Networks**: Ensure proper excitation/inhibition balance
4. **Plasticity Control**: Balance stability and learning capacity

#### Learning Strategies

1. **Curriculum Learning**: Start with simple tasks, progress to complex
2. **Metalearning**: Learn how to learn
3. **Transfer Learning**: Apply learned knowledge to new tasks
4. **Continual Learning**: Learn continuously without forgetting

#### Performance Monitoring

1. **Metrics Collection**: Track firing rates, memory usage, performance
2. **Real-time Visualization**: Monitor network activity in real-time
3. **Performance Profiling**: Identify bottlenecks and optimize
4. **Comparison Analysis**: Compare different configurations

## Configuration Templates

### Template 1: Basic Research

```cfg
# NLM Basic Research Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
plasticity_learning_rate = 0.01
environment_name = GridWorld
environment_width = 20
environment_height = 20
log_level = INFO
```

### Template 2: Advanced Learning

```cfg
# NLM Advanced Learning Configuration
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 3000
region_count = 3
connection_probability = 0.25
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.015
stdp_time_constant = 30.0
plasticity_learning_rate = 0.02
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
dopamine_baseline = 0.05
reward_discount_factor = 0.95
environment_name = ComplexWorld
environment_width = 30
environment_height = 30
log_level = DEBUG
```

### Template 3: Performance Testing

```cfg
# NLM Performance Testing Configuration
random_seed = 99999
simulation_timestep = 0.002
neuron_count = 5000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.01
stdp_time_constant = 15.0
plasticity_learning_rate = 0.005
synaptogenesis_rate = 0.0002
pruning_rate = 0.00001
dopamine_baseline = 0.01
reward_discount_factor = 0.99
environment_name = SimpleWorld
environment_width = 10
environment_height = 10
log_level = WARN
```

## Conclusion

The NLM project provides comprehensive tools for advanced neural network simulation and research. With over 109 advanced features, extensive documentation, and thorough testing, NLM is ready for both research and educational applications.

The examples and guidelines in this document should help you get started with NLM and make the most of its capabilities. For the most up-to-date information, visit the NLM documentation website or check the release notes for new features and improvements.

Remember that neural network simulation requires careful parameter tuning and monitoring. Use the examples as starting points and adjust parameters based on your specific requirements and system resources.

Happy experimenting with NLM! 🚀