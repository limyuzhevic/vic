// Enhanced documentation with practical implementation notes and troubleshooting
g// We have several key areas where documentation can be improved:

// 1. DETAILED IMPLEMENTATION NOTES: Add practical implementation details for complex algorithms
// 2. API DOCUMENTATION: Add comprehensive documentation for key headers
// 3. TROUBLESHOOTING GUIDE: Add practical troubleshooting steps
// 4. BEST PRACTICES: Add coding guidelines and common pitfalls

// Let me focus on enhancing the existing documentation with practical implementation notes

## Enhanced Implementation Notes for Key Algorithms

### LIF Neuron Dynamics (src/brain/Neuron.cpp)

The Leaky Integrate-and-Fire (LIF) neuron implementation in Neuron.cpp follows this core mathematical model:

```cpp
// LIF equation: dV/dt = (V_rest - V)/tau + I/C
// Discrete approximation: V_new = V + dt * 1000 * (leak + synaptic)

// Key components:
// 1. Refractory period handling (lines 194-203)
// 2. LIF integration (lines 205-224)
// 3. Spike-frequency adaptation (lines 225-229)
// 4. Spike reset and refractory entry (lines 234-249)
```

#### Common Implementation Issues:

1. **Numerical Stability**: The membrane potential clamp (line 232) prevents runaway excitation
2. **Adaptation Variable**: Should decay toward zero (line 228) for biological realism
3. **Refractory Period**: Prevent multiple spikes during refractory period (lines 194-203)

#### Optimization Notes:
- The synaptic input clearing (line 257) assumes one input per timestep
- For multi-synapse inputs, sum all inputs before integration
- Consider using exponential Euler for better accuracy (already implemented)

### STDP Implementation (src/plasticity/STDP.cpp)

Spike-Timing-Dependent Plasticity (STDP) is implemented as a sliding window process:

```cpp
// STDP window function (exponential decay)
// LTP decays with tau_plus (20ms typical)
// LTD decays with tau_minus (20ms typical)
```

#### Implementation Details:
- **Pre-before-post pairing**: LTP strengthening
- **Post-before-pre pairing**: LTD weakening  
- **Time window**: ~60ms effective window for plasticity
- **Weight constraints**: Prevent zero/overflow weights

### AgentBrain Neuromodulation (src/agent/AgentBrain.cpp)

The AgentBrain integrates multiple neuromodulators for realistic behavior:

```cpp
// Key neuromodulators:
// 1. Dopamine: Prediction error signal (line 248)
// 2. Novelty: Changes from previous sensory input (line 128)
// 3. Curiosity: Novelty + prediction error (line 146)
// 4. Development: Age-dependent plasticity (lines 289-301)
```

#### Implementation Notes:
- **Curiosity-driven exploration**: SelectWithCuriosity (lines 212-236)
- **Development stages**: Initial → CriticalPeriod → Maturation → Adult
- **Plasticity modulation**: Dopamine affects STDP weights (lines 274-279)

## Enhanced API Documentation

### Brain.hpp Core Methods

#### Brain::initialize() - Critical Setup

```cpp
// Phase 2+ requirement: Must be called before any simulation steps
bool Brain::initialize();

// Implementation requirements:
// 1. Seed random number generator
// 2. Create neuron populations according to config
// 3. Establish initial synaptic connections
// 4. Configure neuromodulation systems
// 5. Set developmental stage (Initial)
```

#### Brain::step(SimulationStep, Timestamp) - Main Loop

```cpp
// The core simulation loop that advances the entire brain state
void Brain::step(SimulationStep currentStep);
void Brain::step(SimulationStep currentStep, Timestamp currentTime);

// Execution order (Phase 6 complete loop):
// 1. Receive sensory input (if any)
// 2. Process spikes through spike system
// 3. Apply neuromodulatory signals
// 4. Update plasticities (STDP, Hebbian, reward-modulated)
// 5. Apply developmental changes
// 6. Update working memory
// 7. Check for prediction updates
// 8. Generate motor action
// 9. Log statistics
```

#### Memory System Access Methods

```cpp
// Working Memory - persistent activity for temporary storage
NeuralWorkingMemory* Brain::getWorkingMemory();

// Episodic Memory - experience encoding in neural patterns
NeuralEpisodicMemory* Brain::getEpisodicMemory();

// Associative Memory - pattern associations
NeuralAssociativeMemory* Brain::getAssociativeMemory();
```

### Neuron.hpp State Management

#### NeuronState Structure

```cpp
struct NeuronState {
    // Membrane potential (mV)
    MembranePotential membranePotential;
    MembranePotential restingPotential;
    MembranePotential threshold;
    MembranePotential resetPotential;
    MembranePotential leakConductance;
    MembranePotential synapseConductance;
    
    // Dynamics
    FiringRate firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    
    // Adaptation and history
    float adaptationVariable;
    float lastSpikeTime;
    // ... (see full definition)
};
```

#### State Access Patterns

```cpp
// Const access (recommended for read-only operations)
const NeuronState& Neuron::getState() const;

// Mutable access (required for updates)
NeuronState& Neuron::getState();

// Individual state variable access
MembranePotential Neuron::getMembranePotential() const;
void Neuron::setMembranePotential(MembranePotential potential);
```

### Config.hpp Configuration Patterns

#### Configuration Loading

```cpp
// Load from file (ignore if not found for robustness)
if (config->loadFromFile(configFile)) {
    NLM_LOG_INFO("Loaded configuration from: " + configFile);
} else {
    NLM_LOG_INFO("Using default configuration.");
}

// Override with command line arguments
config->loadFromArgs(argc, argv);

// Set defaults for specific phases
config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
config->set("simulation_timestep", 0.001, ConfigSource::Default);
```

#### Configuration Validation

```cpp
// Validate critical configuration parameters
bool validateConfig(const std::shared_ptr<Config>& config) {
    bool valid = true;
    
    // Neuron count must be positive and reasonable
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (!ConfigValidator::validateNeuronCount(neuronCount)) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
        valid = false;
    }
    
    // Connection probability must be [0,1]
    float connProb = config->getOr<float>("connection_probability", -1.0f);
    if (!ConfigValidator::validateConnectionProbability(connProb)) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connProb));
        valid = false;
    }
    
    return valid;
}
```

## Comprehensive Troubleshooting Guide

### Common Issues and Solutions

#### Issue 1: Simulation Does Not Advance (Steps Processed but No Changes)

**Symptoms:**
- `getTotalSpikeCount()` remains at 0
- `getFiringNeuronCount()` remains at 0
- Program appears stuck or slow

**Root Causes:**
1. **Neurons not initialized properly**
2. **Too much inhibition/excitatory balance**
3. **Incorrect current injection**
4. **Refractory period too long**

**Solutions:**
```cpp
// 1. Check neuron initialization
if (!brain->initialize()) {
    NLM_LOG_ERROR("Brain initialization failed");
    return 1;
}

// 2. Check current injection (main.cpp lines 180-183)
brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);

// 3. Check neuron parameters
for (const auto& neuron : region->getAllNeurons()) {
    if (neuron->getThreshold() > 50.0f) {  // Threshold too high
        neuron->setThreshold(-55.0f);
    }
}
```

#### Issue 2: Memory Leaks and Resource Exhaustion

**Symptoms:**
- Program crashes after prolonged execution
- Memory usage grows indefinitely
- `valgrind` or similar tools show leaks

**Root Causes:**
1. **Circular references in smart pointers**
2. **Missing explicit destructors**
3. **Improper pImpl usage**

**Solutions:**
```cpp
// 1. Check for circular references
// Use weak_ptr for bidirectional relationships

// 2. Ensure proper destructors
Neuron::~Neuron() {
    // Clean up any resources if needed
    delete pImpl;
}

// 3. Review pImpl implementation
// Ensure proper ownership transfer
```

#### Issue 3: Configuration Errors

**Symptoms:**
- Invalid parameter values
- Runtime crashes due to uninitialized variables
- Unpredictable behavior

**Root Causes:**
1. **Invalid file format**
2. **Missing required parameters**
3. **Type mismatches**
4. **Out-of-range values**

**Solutions:**
```cpp
// 1. Validate configuration file format
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!ConfigValidator::validateKey(entry.key)) {
            return false;
        }
        
        if (!validateValue(entry.key, entry.value)) {
            return false;
        }
    }
    return true;
}

// 2. Use configuration validation helper
void setupConfig(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(configFile, true)) { // Validate = true
        NLM_LOG_ERROR("Failed to load or validate configuration");
        return;
    }
    
    if (!config->validate()) {
        NLM_LOG_ERROR("Configuration validation failed");
        return;
    }
    
    // Use validated configuration...
}
```

#### Issue 4: Performance Problems

**Symptoms:**
- Simulation runs too slowly
- Memory usage spikes
- Real-time requirements not met

**Root Causes:**
1. **Inefficient algorithms**
2. **Excessive object creation**
3. **Poor memory locality**
4. **Inefficient data structures**

**Solutions:**
```cpp
// 1. Use performance monitoring
PerformanceDebugger debug;
debug.startPerformanceMonitoring();

// 2. Profile critical sections
auto timer = debug.getProfiler()->time("critical_section");

// 3. Check memory usage
auto& memoryMonitor = debug.getMemoryMonitor();
if (memoryMonitor.detectLeaks()) {
    auto leaks = memoryMonitor.getLeaks();
    for (const auto& leak : leaks) {
        NLM_LOG_WARN("Memory leak detected: " + leak);
    }
}

// 4. Optimize data structures
// Use std::vector instead of std::list for sequential access
// Use pools for object allocation
```

#### Issue 5: Spike Propagation Problems

**Symptoms:**
- No spikes despite strong input
- Excessive spiking (explosion)
- Spikes not propagating to target neurons

**Root Causes:**
1. **Incorrect synaptic delay configuration**
2. **Synapse weight too low/high**
3. **Neuron threshold too high/low**
4. **Spike system not updated**

**Solutions:**
```cpp
// 1. Check synaptic delays
for (auto* syn : region->getSynapses()) {
    syn->setDelay(1); // Standard 1-step delay
}

// 2. Check synapse weights
for (auto* syn : region->getSynapses()) {
    float weight = syn->getWeight();
    if (weight < 0.01f) {
        syn->setWeight(0.01f); // Minimum weight
    }
}

// 3. Check neuron thresholds
for (Neuron* neuron : region->getAllNeurons()) {
    if (neuron->getThreshold() < -60.0f) {
        neuron->setThreshold(-55.0f); // More reasonable threshold
    }
}

// 4. Check spike system update
brain->getSpikeSystem()->updateAll();
```

### Phase-Specific Troubleshooting

#### Phase 2: Neural Computation

**Problem:** "No learning experiment detected"

**Diagnosis:** Check PlasticityRule::enablePlasticity() calls

**Solution:**
```cpp
// Enable plasticity on synapses (main.cpp lines 214-219)
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
    }
}
```

#### Phase 3: World Interaction

**Problem:** "Agent cannot decode motor commands"

**Diagnosis:** Check motor neuron group distribution

**Solution:**
```cpp
// Verify motor neuron groups in AgentBrain::initialize()
// Ensure motorForward_, motorBackward_, etc. are populated
for (const auto& group : {motorForward_, motorBackward_, motorTurnLeft_,
                         motorTurnRight_, motorInteract_, motorWait_}) {
    if (group.empty()) {
        NLM_LOG_WARN("Motor group is empty - agent may not act");
    }
}
```

#### Phase 4: Cognitive Functions

**Problem:** "Concept formation not working"

**Diagnosis:** Check concept formation initialization

**Solution:**
```cpp
// Ensure concept formation is enabled
auto* conceptForm = brain->getConceptFormation();
if (conceptForm) {
    conceptForm->enableLearning(true);
}
```

### Best Practices and Common Pitfalls

#### 1. Memory Management Pitfalls

**Common Mistake:** Using raw pointers instead of smart pointers

**Correct Pattern:**
```cpp
// Wrong (manual memory management)
Neuron* neuron = new Neuron(id);
// ... use neuron ...
delete neuron;

// Correct (RAII with smart pointers)
auto neuron = std::make_shared<Neuron>(id);
// ... use neuron (automatically managed)
```

#### 2. Thread Safety Issues

**Common Mistake:** Concurrent access to shared data structures

**Correct Pattern:**
```cpp
// Wrong - data race
for (auto& neuron : neurons) {
    neuron->setFiringState(FiringState::Active);
}

// Correct - thread-safe with mutex
std::lock_guard<std::mutex> lock(mutex_);
for (auto& neuron : neurons) {
    neuron->setFiringState(FiringState::Active);
}
```

#### 3. Configuration Validation

**Common Mistake:** Assuming configuration values are valid

**Correct Pattern:**
```cpp
// Wrong - assuming values exist
float value = config->get<float>("some.key").value();

// Correct - validate and handle missing values
std::optional<float> value = config->get<float>("some.key");
if (!value.has_value()) {
    NLM_LOG_ERROR("Required configuration parameter missing");
    return false;
}
```

#### 4. Performance Optimization

**Common Mistake:** Profiling too much code

**Correct Pattern:**
```cpp
// Profile only critical paths
{
    auto timer = profiler.time("simulation_step");
    
    // Core simulation logic here
    for (step = 0; step < maxSteps; ++step) {
        brain->step(step);
    }
    
    // Check if performance is acceptable
    if (timer.getDuration() > 1.0) { // 1 second threshold
        NLM_LOG_WARN("Step took too long: " + std::to_string(timer.getDuration()) + "s");
    }
}
```

#### 5. Error Handling

**Common Mistake:** Ignoring return codes

**Correct Pattern:**
```cpp
// Wrong - ignoring return codes
brain->step(step);

// Correct - check return codes
if (!brain->initialize()) {
    NLM_LOG_ERROR("Failed to initialize brain");
    return 1;
}

if (!brain->save("checkpoint.bin")) {
    NLM_LOG_ERROR("Failed to save checkpoint");
    return 1;
}
```

## Enhanced Phase-Specific Examples

### Phase 2 Enhanced Learning Experiment

```cpp
void enhancedLearningExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Enhanced Learning Experiment ===");
    
    // 1. Record initial state (same as original)
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    // 2. Enhanced plasticity configuration
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            // Enable all plasticity mechanisms
            syn->enablePlasticity(true, true, true); // Hebbian, STDP, reward-modulated
            
            // Set learning parameters
            syn->setWeight(0.5f); // Start with moderate weight
        }
    }
    
    // 3. More sophisticated input pattern
    NLM_LOG_INFO("Applying sophisticated input patterns (5000 steps)...");
    
    for (SimulationStep step = 0; step < 5000; ++step) {
        // Create complex input pattern - simulate sensory input
        for (size_t i = 0; i < 50 && i < brain->getTotalNeuronCount() / 8; ++i) {
            brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), 
                                         50.0f * std::sin(step * 0.01f)); // Oscillating input
        }
        
        brain->step(step, step * 0.001);
        
        // Log detailed progress
        if (step % 500 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        }
    }
    
    // 4. Enhanced final state analysis
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // 5. Calculate learning metrics
    NLM_LOG_INFO("=== Learning Metrics ===");
    float initialMean = calculateMean(experiment.initialWeights);
    float finalMean = calculateMean(experiment.finalWeights);
    float learningRate = (finalMean - initialMean) / initialMean;
    
    NLM_LOG_INFO("Learning rate: " + std::to_string(learningRate));
    NLM_LOG_INFO("Convergence: " + std::to_string(learningRate > 0.001f ? "Yes" : "No"));
}
```

### Phase 4 Cognitive Integration Example

```cpp
void cognitiveIntegrationExperiment(std::shared_ptr<Brain> brain, const SimpleWorld& world) {
    NLM_LOG_INFO("=== Cognitive Integration Experiment ===");
    
    // 1. Initialize all cognitive systems
    auto* attention = brain->getAttention();
    auto* planner = brain->getPlanner();
    auto* conceptForm = brain->getConceptFormation();
    auto* workingMem = brain->getWorkingMemory();
    auto* episodicMem = brain->getEpisodicMem();
    
    // Enable learning for all cognitive systems
    attention->enableLearning(true);
    planner->enableLearning(true);
    conceptForm->enableLearning(true);
    workingMem->enableConsolidation(true);
    episodicMem->enableReplay(true);
    
    // 2. Create complex task environment
    NLM_LOG_INFO("Creating complex cognitive task environment...");
    
    // Reset world for controlled experiment
    // ... (world setup code) ...
    
    // 3. Run cognitive task
    NLM_LOG_INFO("Running cognitive task for 2000 steps...");
    
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Update world
        // ... (world update code) ...
        
        // Get sensory input
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // Process through all cognitive systems
        brain->receiveSensoryInput(performAttention(attention, percept));
        brain->step(step);
        
        // Get action through planning system
        auto action = planner->selectAction();
        
        // Apply action
        // ... (action application) ...
        
        // Log cognitive metrics every 200 steps
        if (step % 200 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ":");
            NLM_LOG_INFO("  Attention: " + std::to_string(attention->getSelectionStrength()));
            NLM_LOG_INFO("  Planning confidence: " + std::to_string(planner->getConfidence()));
            NLM_LOG_INFO("  Concept stability: " + std::to_string(conceptForm->getStability()));
            NLM_LOG_INFO("  Working memory load: " + std::to_string(workingMem->getLoad()));
            NLM_LOG_INFO("  Episodic memory size: " + std::to_string(episodicMem->getSize()));
        }
    }
    
    // 4. Analyze cognitive integration
    NLM_LOG_INFO("=== Cognitive Integration Analysis ===");
    
    double attentionQuality = attention->getSelectionQuality();
    double planningEfficiency = planner->getEfficiency();
    double conceptCoherence = conceptForm->getCoherence();
    double memoryRetention = episodicMem->getRetentionRate();
    
    NLM_LOG_INFO("Attention quality: " + std::to_string(attentionQuality));
    NLM_LOG_INFO("Planning efficiency: " + std::to_string(planningEfficiency));
    NLM_LOG_INFO("Concept coherence: " + std::to_string(conceptCoherence));
    NLM_LOG_INFO("Memory retention: " + std::to_string(memoryRetention));
    
    NLM_LOG_INFO("Integration success: " + 
                std::to_string(attentionQuality > 0.7 && planningEfficiency > 0.7 && 
                              conceptCoherence > 0.7 && memoryRetention > 0.7 ? "Yes" : "No"));
}
```

### Performance Optimization Examples

#### Memory Pool Example

```cpp
// Memory pool for efficient object allocation
class NeuronPool {
public:
    static NeuronPool& getInstance() {
        static NeuronPool instance;
        return instance;
    }
    
    Neuron* allocate(NeuronId id) {
        std::lock_guard<std::mutex> lock(mutex_);
        Neuron* neuron = pool_.allocate();
        new (neuron) Neuron(id);
        return neuron;
    }
    
    void deallocate(Neuron* neuron) {
        std::lock_guard<std::mutex> lock(mutex_);
        neuron->~Neuron();
        pool_.deallocate(neuron);
    }
    
private:
    NeuronPool() : pool_(64) {} // Pool of 64 neurons
    std::mutex mutex_;
    std::vector<Neuron*> pool_;
};

// Usage in Brain::addNeuron()
NeuronId Brain::addNeuron() {
    NeuronId id = generateNeuronId();
    Neuron* neuron = NeuronPool::getInstance().allocate(id);
    // ... initialize neuron ...
    return id;
}
```

#### SIMD Vectorization Example

```cpp
// SIMD-optimized neuron stepping
void VectorizedNeuronStepping(const std::vector<Neuron*>& neurons, 
                            const std::vector<MembranePotential>& inputs,
                            size_t count) {
    // Use SIMD for parallel processing of neuron dynamics
    #ifdef __SSE4_1__
    // Process 4 neurons at once using SSE
    for (size_t i = 0; i < count; i += 4) {
        // Load 4 neurons into SIMD registers
        // Process 4 membrane potential updates in parallel
        // Store results back
    }
    #endif
    
    // Fallback to scalar for remaining neurons
    for (size_t i = count - (count % 4); i < count; ++i) {
        neurons[i]->stepLIF(currentTime, dt);
    }
}
```

### Advanced Configuration Examples

#### Environment-Specific Configuration

```cpp
// Configuration for different simulation environments
class EnvironmentConfig {
public:
    static std::shared_ptr<Config> getDevelopmentConfig() {
        auto config = std::make_shared<Config>();
        
        // Development configuration (small scale, detailed logging)
        config->set("random_seed", 12345ll);
        config->set("simulation_timestep", 0.0001);
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.3);
        config->set("log_level", "DEBUG");
        config->set("log_to_file", true);
        config->set("log_filename", "nlm_dev.log");
        config->set("max_simulation_steps", 10000);
        
        return config;
    }
    
    static std::shared_ptr<Config> getProductionConfig() {
        auto config = std::make_shared<Config>();
        
        // Production configuration (larger scale, optimized)
        config->set("random_seed", 42ll);
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 10000);
        config->set("region_count", 4);
        config->set("connection_probability", 0.1);
        config->set("log_level", "INFO");
        config->set("log_to_file", true);
        config->set("log_filename", "nlm_prod.log");
        config->set("max_simulation_steps", 100000);
        
        return config;
    }
    
    static std::shared_ptr<Config> getResearchConfig() {
        auto config = std::make_shared<Config>();
        
        // Research configuration (custom parameters for specific studies)
        config->set("random_seed", 99999ll);
        config->set("simulation_timestep", 0.01);
        config->set("neuron_count", 5000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15);
        config->set("plasticity_learning_rate", 0.001);
        config->set("stdp_ltp_weight", 0.005);
        config->set("stdp_ltd_weight", 0.012);
        config->set("development_synaptogenesis_rate", 0.0001);
        config->set("development_pruning_rate", 0.00001);
        config->set("log_level", "INFO");
        config->set("log_to_file", true);
        config->set("log_filename", "nlm_research.log");
        config->set("max_simulation_steps", 50000);
        
        return config;
    }
};
```

#### Dynamic Configuration Adjustment

```cpp
// Example: Adjusting configuration during simulation
class AdaptiveConfig {
public:
    AdaptiveConfig(std::shared_ptr<Config> config) : config_(config) {}
    
    void adjustBasedOnPerformance(double avgStepTime, size_t memoryUsage) {
        if (avgStepTime > 0.1) { // Steps taking too long
            // Reduce simulation complexity
            int64_t currentNeurons = config_->getOr<int64_t>("neuron_count", 1000);
            config_->set("neuron_count", currentNeurons * 0.8);
            NLM_LOG_WARN("Reducing neuron count to improve performance");
        } else if (memoryUsage > 1024 * 1024 * 1024) { // > 1GB memory
            // Reduce memory footprint
            int64_t currentNeurons = config_->getOr<int64_t>("neuron_count", 1000);
            config_->set("neuron_count", currentNeurons * 0.9);
            NLM_LOG_WARN("Reducing neuron count to reduce memory usage");
        } else if (avgStepTime < 0.001 && memoryUsage < 100 * 1024 * 1024) {
            // Performance is good, can increase complexity
            int64_t currentNeurons = config_->getOr<int64_t>("neuron_count", 1000);
            config_->set("neuron_count", std::min(currentNeurons * 1.2, 10000ll));
            NLM_LOG_INFO("Increasing neuron count for better performance");
        }
    }
    
private:
    std::shared_ptr<Config> config_;
};
```

## Summary

This enhanced documentation provides:

1. **Detailed Implementation Notes**: Practical insights for complex algorithms
2. **API Documentation**: Comprehensive method descriptions and usage patterns
3. **Troubleshooting Guide**: Systematic approach to common problems
4. **Best Practices**: Coding guidelines and common pitfalls
5. **Advanced Examples**: Phase-specific and optimization examples
6. **Configuration Management**: Environment-specific and dynamic configuration

This documentation transforms the existing technical documentation into a comprehensive practical guide that helps developers understand, debug, and extend the NLM system effectively.