# NLM Troubleshooting Guide: Common Issues and Solutions

## Introduction

This guide provides solutions to common problems encountered when using NLM (Neural Learning Machine). It covers:

- **Installation issues**: Problems with building and installing NLM
- **Configuration issues**: Problems with configuration files and settings
- **Simulation issues**: Problems during brain simulation
- **Performance issues**: Slow simulation or memory problems
- **Learning issues**: Problems with learning and plasticity
- **Integration issues**: Problems connecting different components
- **Advanced issues**: Complex problems and debugging techniques

## Installation Issues

### Problem: "Cannot find CMake or CMake version < 3.16"

**Symptoms:**
- CMake error when building from source
- Version requirement not met

**Solution:**

#### Option 1: Install CMake

```bash
# On Ubuntu/Debian
sudo apt-get update
sudo apt-get install cmake

# On macOS
brew install cmake

# On Windows
# Download from https://cmake.org/download/
# Add CMake to PATH during installation
```

#### Option 2: Use Pre-built Binaries

If you don't want to build from source, install the Python package directly:

```bash
pip install pynlm
```

#### Option 3: Use Conda

```bash
conda install -c conda-forge pynlm
```

### Problem: "Python bindings cannot be built"

**Symptoms:**
- Error during pip install
- Missing Python development headers
- pybind11 not found

**Solution:**

```bash
# Install Python development headers
# On Ubuntu/Debian:
sudo apt-get install python3-dev python3-venv python3-pip

# On macOS:
# Python is included, but you may need:
brew install python

# Install required dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install -e .

# Or use system package manager
# On Ubuntu/Debian:
sudo apt-get install python3-pynlm
```

### Problem: "Compiler not supported"

**Symptoms:**
- Compiler error during C++ compilation
- Version not supported

**Solution:**

```bash
# Install modern compiler
# On Ubuntu/Debian:
sudo apt-get install g++

# On macOS:
# Xcode command line tools are usually installed
# If not: xcode-select --install

# Verify compiler version
g++ --version
# Should be 8.0 or newer
```

## Configuration Issues

### Problem: "Configuration file not found"

**Symptoms:**
- Error when loading configuration file
- File path issues
- Wrong filename or extension

**Solution:**

1. **Check file path:**
   ```python
   # Use absolute path
   config->loadFromFile("/path/to/config.cfg");
   
   # Or relative to current directory
   config->loadFromFile("my_config.cfg");
   ```

2. **Verify file exists:**
   ```python
   # Check if file exists before loading
   if (std::filesystem::exists("config.cfg")) {
       config->loadFromFile("config.cfg");
   } else {
       std::cout << "Config file not found, using defaults" << std::endl;
   }
   ```

3. **Check file format:**
   ```python
   # Valid config file format:
   # key = value
   # key: value
   # key value  # space separated
   
   brain.neuron_count = 1000
   brain.synapse_density = 0.1
   plasticity.stdp.enable = true
   ```

### Problem: "Invalid configuration values"

**Symptoms:**
- Wrong data types
- Out-of-range values
- Missing required settings

**Solution:**

1. **Validate configuration values:**
   ```python
   auto config = std::make_shared<Config>();
   
   // Set with validation
   config->set("brain.neuron_count", 1000);
   config->set("brain.synapse_density", 0.1f);
   config->set("plasticity.stdp.enable", true);
   
   // Get with defaults
   int64_t neurons = config->getOr<int64_t>("brain.neuron_count", 1000);
   float density = config->getOr<float>("brain.synapse_density", 0.1f);
   bool stdp = config->getOr<bool>("plasticity.stdp.enable", false);
   ```

2. **Use default configuration:**
   ```python
   // Always use defaults for missing values
   config->set("brain.neuron_count", config->getOr<int64_t>("brain.neuron_count", 1000));
   ```

### Problem: "JSON configuration file syntax error"

**Symptoms:**
- JSON parsing error
- Missing quotes or brackets
- Invalid data types

**Solution:**

1. **Fix JSON syntax:**
   ```json
   // Incorrect:
   "brain.neuron_count": 1000,
   "plasticity.stdp.enable": true
   
   // Correct:
   {
     "brain": {
       "neuron_count": 1000,
       "synapse_density": 0.1
     },
     "plasticity": {
       "stdp": {
         "enable": true,
         "learning_rate": 0.001
       }
     }
   }
   ```

2. **Use proper JSON tools:**
   ```python
   # Validate JSON before loading
   # Using nlohmann/json:
   try {
       nlohmann::json jsonData = nlohmann::json::parse(fileContent);
       // Load into config
   } catch (const nlohmann::json::parse_error& e) {
       std::cout << "JSON parse error: " << e.what() << std::endl;
   }
   ```

## Simulation Issues

### Problem: "Brain initialization failed"

**Symptoms:**
- Brain doesn't start
- Error during initialization
- Segmentation fault

**Solution:**

1. **Check configuration:**
   ```python
   auto config = std::make_shared<Config>();
   
   // Required settings
   config->set("random_seed", 42);
   config->set("simulation_timestep", 0.001);
   config->set("neuron_count", 500);
   config->set("region_count", 1);
   config->set("connection_probability", 0.1f);
   
   // Initialize brain
   auto brain = std::make_shared<Brain>(config);
   if (!brain->initialize()) {
       std::cerr << "Failed to initialize brain!" << std::endl;
       return 1;
   }
   ```

2. **Check system resources:**
   ```bash
   # Check memory usage
   free -h
   
   # Check CPU usage
   top
   
   # Check disk space
   df -h
   ```

3. **Try minimal configuration:**
   ```python
   // Start with minimal viable configuration
   auto config = std::make_shared<Config>();
   config->set("neuron_count", 10);  // Small network
   config->set("region_count", 1);   // Single region
   ```

### Problem: "Simulation step fails"

**Symptoms:**
- Crash during simulation
- Infinite loop
- Memory corruption

**Solution:**

1. **Add error handling:**
   ```python
   for (size_t step = 0; step < maxSteps; ++step) {
       try {
           brain->step(step);
           
           // Check for errors
           if (brain->getTotalSpikeCount() > 1000000) {
               std::cout << "Warning: Very high spike count" << std::endl;
           }
           
       } catch (const std::exception& e) {
           std::cerr << "Error during step " << step << ": " << e.what() << std::endl;
           break;
       }
   }
   ```

2. **Add validation checks:**
   ```python
   if (step % 1000 == 0) {
       size_t spikes = brain->getTotalSpikeCount();
       size_t neurons = brain->getTotalNeuronCount();
       
       if (spikes / neurons > 10.0f) {
           std::cout << "Warning: High firing rate detected" << std::endl;
       }
   }
   ```

### Problem: "Brain reset doesn't work"

**Symptoms:**
- Brain state persists after reset
- Memory not cleared
- Neuron states not reset

**Solution:**

1. **Call reset properly:**
   ```python
   // Reset before re-initialization
   brain->reset();
   brain->initialize();
   ```

2. **Check reset implementation:**
   ```python
   // Verify reset implementation
   void Brain::reset() {
       pImpl->currentStep = 0;
       pImpl->currentTime = 0.0;
       pImpl->totalSpikesThisStep = 0;
       pImpl->totalSpikesTotal = 0;
       
       for (auto& region : pImpl->regions) {
           region->reset();
       }
       
       pImpl->spikeSystem->reset();
       pImpl->developmentalStage = DevelopmentalStage::Initial;
       
       // Reset memory systems
       if (pImpl->workingMemory) pImpl->workingMemory->clear();
       if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
       if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
       
       if (pImpl->attention) pImpl->attention->reset();
   }
   ```

## Performance Issues

### Problem: "Simulation is too slow"

**Symptoms:**
- Long simulation times
- Poor performance
- Resource usage too high

**Solution:**

1. **Optimize configuration:**
   ```python
   // Speed optimizations
   auto config = std::make_shared<Config>();
   config->set("brain.neuron_count", 500);           // Smaller network
   config->set("plasticity.stdp.enable", false);    // Disable costly plasticity
   config->set("neuromod.dopamine.enable", false);  // Disable neuromodulation
   config->set("brain.synapse_density", 0.05);      // Sparse connectivity
   ```

2. **Use efficient data structures:**
   ```python
   // Use optimized brain
   auto brain = std::make_shared<OptimizedBrain>(config);
   ```

3. **Parallel processing:**
   ```python
   // Use parallel simulation
   ParallelBrain parallelBrain(config);
   parallelBrain.initialize();
   
   for (size_t step = 0; step < maxSteps; step += PARALLEL_STEPS) {
       parallelBrain.stepParallel(step, PARALLEL_STEPS);
   }
   ```

4. **Memory optimization:**
   ```python
   // Enable memory pools
   MemoryPool::enable(true);
   
   // Use efficient data structures
   std::vector<float> optimizedData;
   ```

### Problem: "High memory usage"

**Symptoms:**
- Memory exhaustion
- Swap usage
- System instability

**Solution:**

1. **Reduce network size:**
   ```python
   config->set("brain.neuron_count", 100);   // Smaller network
   config->set("brain.region_count", 1);    // Fewer regions
   ```

2. **Use sparse connectivity:**
   ```python
   config->set("brain.synapse_density", 0.01);  // Very sparse
   ```

3. **Enable memory management:**
   ```python
   // Enable memory pool
   MemoryPool::enable(true);
   
   // Optimize memory usage
   brain->optimizeMemoryUsage();
   ```

4. **Use checkpointing:**
   ```python
   // Save and load to reduce memory usage
   brain->save("checkpoint.bin");
   // ... later ...
   brain->load("checkpoint.bin");
   ```

## Learning Issues

### Problem: "Learning is too slow"

**Symptoms:**
- Weights don't change significantly
- Learning plateaus early
- Network doesn't converge

**Solution:**

1. **Adjust learning parameters:**
   ```python
   config->set("plasticity.stdp.learning_rate", 0.01);  // Higher learning rate
   config->set("neuromod.dopamine.scale", 2.0);         // Stronger reward signals
   config->set("neuromod.curiosity.enable", true);     // Enable curiosity
   ```

2. **Increase plasticity:**
   ```python
   config->set("plasticity.stdp.enable", true);
   config->set("plasticity.hebbian.enable", true);
   config->set("plasticity.structural.enable", true);
   ```

3. **Longer training:**
   ```python
   // Train for longer
   for (size_t step = 0; step < 10000; ++step) {
       brain->step(step);
       
       // Log progress
       if (step % 1000 == 0) {
           std::cout << "Step " << step << ": " 
                     << "weight change: " << calculateWeightChange(brain) << std::endl;
       }
   }
   ```

### Problem: "Learning is unstable"

**Symptoms:**
- Weights oscillate
- Network explodes
- Learning becomes erratic

**Solution:**

1. **Reduce learning rates:**
   ```python
   config->set("plasticity.stdp.learning_rate", 0.001);  // Lower learning rate
   config->set("plasticity.hebbian.learning_rate", 0.001);
   ```

2. **Add regularization:**
   ```python
   config->set("plasticity.stdp.ltd_weight", 0.015);    // Stronger decay
   config->set("plasticity.stdp.ltp_weight", 0.01);    // Weaker growth
   ```

3. **Add weight constraints:**
   ```python
   // Add weight normalization
   brain->addWeightConstraint(-1.0f, 1.0f);  // Constrain to [-1, 1]
   ```

### Problem: "Memory systems aren't working"

**Symptoms:**
- Working memory doesn't store patterns
- Episodic memory doesn't record episodes
- Associative memory doesn't complete patterns

**Solution:**

1. **Enable memory systems:**
   ```python
   // Ensure memory systems are enabled
   if (brain->getWorkingMemory()) {
       brain->getWorkingMemory()->setCapacity(100);
   }
   
   if (brain->getEpisodicMemory()) {
       brain->getEpisodicMemory()->setMaxEpisodes(1000);
   }
   
   if (brain->getAssociativeMemory()) {
       brain->getAssociativeMemory()->setCompletionThreshold(0.8f);
   }
   ```

2. **Store data in memory:**
   ```python
   // Store neural activity in working memory
   brain->getWorkingMemory()->storeToNeuron(neuronId, activationLevel);
   
   // Store episodes in episodic memory
   EpisodicMemoryItem episode;
   episode.timestamp = currentStep;
   episode.reward = reward;
   // ... fill other fields ...
   
   brain->getEpisodicMemory()->storeEpisode(episode);
   ```

## Integration Issues

### Problem: "Components not integrating"

**Symptoms:**
- Memory systems disconnected
- Neuromodulation not affecting learning
- Prediction system not working

**Solution:**

1. **Check integration:**
   ```python
   // Verify integration
   auto* workingMemory = brain->getWorkingMemory();
   if (!workingMemory) {
       std::cerr << "Working memory is NOT integrated!" << std::endl;
   } else {
       std::cout << "Working memory is integrated!" << std::endl;
   }
   
   auto* dopamine = brain->getDopamine();
   if (!dopamine) {
       std::cerr << "Dopamine system is NOT integrated!" << std::endl;
   } else {
       std::cout << "Dopamine system is integrated!" << std::endl;
   }
   ```

2. **Enable all systems:**
   ```python
   // Ensure all systems are enabled
   agent->enableRewardModulation(true);
   agent->enableCuriosity(true);
   agent->enableDevelopment(true);
   agent->enablePrediction(true);
   agent->enableAttention(true);
   ```

3. **Check dependencies:**
   ```python
   // Check system dependencies
   if (!brain->getWorkingMemory() || !brain->getEpisodicMemory()) {
       std::cerr << "Memory systems are required for integration!" << std::endl;
       return 1;
   }
   
   if (!brain->getDopamine() || !brain->getCuriosity()) {
       std::cerr << "Neuromodulation systems are required!" << std::endl;
       return 1;
   }
   ```

### Problem: "Sensory input not connected"

**Symptoms:**
- Agent doesn't see environment
- No sensory data
- Motor commands not generated

**Solution:**

1. **Check sensory setup:**
   ```python
   // Ensure sensory input is connected
   auto percept = world->getSensoryPercept();
   if (percept.getData().empty()) {
       std::cout << "Warning: No sensory data available" << std::endl;
   } else {
       std::cout << "Sensory data available: " << percept.getData().size() << " values" << std::endl;
   }
   
   agent->processSensoryInput(percept);
   ```

2. **Check agent initialization:**
   ```python
   // Ensure agent is properly initialized
   auto agent = std::make_shared<AgentBrain>(brain);
   auto world = pynlm::createSimpleWorld();
   world->configure(20, 20, 8, 8);
   world->reset();
   
   agent->initialize(*world);
   
   // Check if agent has sensory processing
   if (!agent->hasSensoryProcessing()) {
       std::cerr << "Agent doesn't have sensory processing!" << std::endl;
       return 1;
   }
   ```

## Advanced Issues

### Problem: "Debugging complex simulation issues"

**Symptoms:**
- Hard to find root cause
- Intermittent failures
- Complex state management

**Solution:**

1. **Add logging:**
   ```python
   // Comprehensive logging
   # Enable different log levels
   logger->setLevel(LogLevel::Debug);
   
   // Add structured logging
   logger->enableStructuredLogging(true);
   
   // Add file logging
   logger->setOutputFile("debug.log");
   ```

2. **Use debugging tools:**
   ```python
   // Use debug mode
   # Define DEBUG macro
   # #define DEBUG 1
   
   // Add debug output
   #ifdef DEBUG
   std::cout << "Debug: Neuron " << neuron->getId() 
             << " membrane potential: " << neuron->getMembranePotential() << std::endl;
   #endif
   ```

3. **Use visualization:**
   ```python
   // Enable visualization for debugging
   auto visualizer = std::make_shared<VisualizationInterface>(brain);
   visualizer->enable(true);
   visualizer->startVisualization();
   ```

### Problem: "Performance bottleneck analysis"

**Symptoms:**
- Certain parts of code are slow
- Memory usage spikes
- System doesn't scale

**Solution:**

1. **Profile performance:**
   ```python
   // Use profiler
   Profiler profiler;
   profiler.start();
   
   // Run simulation
   for (step in range(1000)):
       brain->step(step);
   
   profiler.stop();
   
   // Analyze results
   profiler.printReport();
   profiler.exportProfile("profile.json");
   ```

2. **Identify bottlenecks:**
   ```python
   // Time different operations
   auto start = std::chrono::high_resolution_clock::now();
   brain->initialize();
   auto end = std::chrono::high_resolution_clock::now();
   
   std::cout << "Initialization time: " 
             << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
             << " ms" << std::endl;
   ```

### Problem: "Reproducing experimental results"

**Symptoms:**
- Results vary between runs
- Non-deterministic behavior
- Hard to reproduce issues

**Solution:**

1. **Set random seed:**
   ```python
   config->set("random_seed", 42);  // Fixed seed for reproducibility
   ```

2. **Use deterministic algorithms:**
   ```python
   // Ensure deterministic behavior
   brain->setRandomSeed(42);
   
   // Use consistent initialization
   brain->initializeRandom(rng);
   ```

3. **Document experimental setup:**
   ```python
   // Document experiment configuration
   struct ExperimentSetup {
       std::shared_ptr<Config> config;
       size_t numSteps;
       std::string description;
       
       void saveToFile(const std::string& filename) const;
       static ExperimentSetup loadFromFile(const std::string& filename);
   };
   ```

## Frequently Asked Questions

### Q: How do I create a custom brain configuration?

A: Use the Config class to create and customize configurations:

```python
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("brain.synapse_density", 0.1);
config->set("plasticity.stdp.enable", true);
config->set("neuromod.curiosity.enable", true);

auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

### Q: How do I run multiple experiments?

A: Create experiment configurations and run them sequentially:

```python
struct Experiment {
    std::string name;
    std::shared_ptr<Config> config;
    size_t numSteps;
};

std::vector<Experiment> experiments = {
    {"Small", createSmallConfig(), 1000},
    {"Medium", createMediumConfig(), 500},
    {"Large", createLargeConfig(), 200}
};

for (const auto& exp : experiments) {
    runExperiment(exp);
}
```

### Q: How do I save and load brain states?

A: Use the brain's save/load methods:

```python
// Save brain state
brain->save("my_brain.bin");

// Load brain state
auto newBrain = std::make_shared<Brain>(config);
newBrain->initialize();
newBrain->load("my_brain.bin");
```

### Q: How do I debug simulation issues?

A: Use comprehensive logging and debugging tools:

```python
// Enable debug logging
logger->setLevel(LogLevel::Debug);
logger->setOutputFile("debug.log");

// Add debug output
#ifdef DEBUG
// Add debug statements
#endif
```

### Q: How do I optimize performance?

A: Use configuration options and optimization techniques:

```python
// Speed optimizations
config->set("brain.neuron_count", 500);
config->set("plasticity.stdp.enable", false);
config->set("neuromod.dopamine.enable", false);

// Accuracy optimizations  
config->set("brain.neuron_count", 5000);
config->set("plasticity.stdp.enable", true);
config->set("neuromod.curiosity.enable", true);
```

## Conclusion

This troubleshooting guide provides solutions to common NLM issues. Remember that:

1. **Start simple**: Begin with minimal configurations and gradually add complexity
2. **Use logging**: Enable comprehensive logging for debugging
3. **Check resources**: Monitor memory usage and CPU usage
4. **Validate inputs**: Always validate configuration values
5. **Document experiments**: Document configurations for reproducibility
6. **Profile performance**: Identify and fix performance bottlenecks

For more advanced troubleshooting and debugging techniques, consult the NLM documentation and community resources.

## References

- [NLM Documentation](docs/ARCHITECTURE.md)
- [User Guide](docs/USER_GUIDE.md)
- [API Reference](docs/API_REFERENCE.md)
- [Examples](docs/EXAMPLES.md)
- [NLM Community Forum](https://github.com/nlm-project/nlm/discussions)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
