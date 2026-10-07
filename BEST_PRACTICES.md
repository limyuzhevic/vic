# NLM Best Practices Guide

## Overview

This document provides comprehensive guidelines for developing and using NLM brains effectively. It covers best practices, design patterns, and development recommendations based on the NLM architecture and Phase 6 integration goals.

## Code Development Best Practices

### 1. Component Integration

#### DOs
- **Integrate with Brain Loop**: Ensure all new components participate in the main brain loop
- **Use Standard Interfaces**: Implement existing interfaces (e.g., `PlasticityRule`, `Neuromodulator`)
- **Follow Event-Driven Pattern**: Use spike-based communication where appropriate
- **Maintain Memory Efficiency**: Use memory pools and SoA layouts for large networks

#### DON'Ts
- **Don't create standalone systems**: All systems should integrate with the main brain loop
- **Don't duplicate functionality**: Leverage existing components where possible
- **Don't bypass validation**: Always validate inputs and outputs
- **Don't assume constant performance**: Profile and optimize as needed

#### Examples

**Good Integration (Working System)**:
```cpp
// Neuromodulation properly integrated
class Dopamine : public Neuromodulator {
public:
    void apply(ModulatedNeuron& neuron, float reward_prediction_error) {
        // Modulates synaptic plasticity based on reward
        neuron.modulatePlasticity(reward_prediction_error * scale);
    }
};

// Used in brain loop
void Brain::step(int step) {
    // ... neural processing ...
    // Apply neuromodulation
    if (dopamine_enabled_) {
        float rpe = calculateRewardPredictionError();
        applyNeuromodulation(Dopamine::Type::kDopamine, rpe);
    }
}
```

**Bad Integration (Disconnected System)**:
```cpp
// System created but never used
class NeuralPlanner {
    // Full implementation with neural mechanisms
    // But Brain::getNeuralPlanner() returns nullptr
};

// Brain loop ignores it entirely
void Brain::step(int step) {
    // Process basic neurons, plasticity
    // Never mentions NeuralPlanner
}
```

### 2. Memory System Integration

#### Integration Pattern for Working Memory
```cpp
// In NeuralWorkingMemory.hpp
class NeuralWorkingMemory {
public:
    void integrateWithBrain(Brain* brain) {
        // Connect to brain's attention system
        brain->setWorkingMemoryCallback(
            [this](const NeuralActivity& activity) {
                updateWorkingMemory(activity);
            }
        );
    }
    
    void updateWorkingMemory(const NeuralActivity& activity) {
        // Maintain persistent activity through recurrent connections
        // Use competition to select important information
        updateRecurrentConnections(activity);
        decayUnreinforcedActivity();
    }
};

// In Brain.hpp - integration point
class Brain {
private:
    NeuralWorkingMemory* working_memory_ = nullptr;
    
public:
    void step(int step) {
        // ... neural processing ...
        
        // Update working memory with current activity
        if (working_memory_) {
            working_memory_->updateWorkingMemory(current_activity_);
        }
    }
    
    NeuralWorkingMemory* getWorkingMemory() override {
        return working_memory_;
    }
    
    void setWorkingMemory(NeuralWorkingMemory* memory) {
        working_memory_ = memory;
    }
};
```

#### Integration Pattern for Episodic Memory
```cpp
// In NeuralEpisodicMemory.hpp
class NeuralEpisodicMemory {
public:
    void recordExperience(const Experience& experience) {
        // Encode experience as neural pattern
        encodeExperiencePattern(experience);
        
        // Store in temporal and spatial indexes
        addTemporalIndex(experience.timestamp_);
        addSpatialIndex(experience.location_);
    }
    
    void replayDuringRest(float replay_probability = 0.1f) {
        // Reactivate important patterns during rest
        identifyImportantExperiences();
        for (const auto& pattern : important_patterns_) {
            if (random_uniform() < replay_probability) {
                reactivatePattern(pattern);
            }
        }
    }
};

// Integration in Brain.cpp
void Brain::step(int step) {
    // ... normal processing ...
    
    // Check for consolidation opportunities
    if (step % CONSOLIDATION_INTERVAL == 0) {
        if (episodic_memory_) {
            episodic_memory_->replayDuringRest(0.05f);
        }
    }
    
    // Record significant experiences
    if (isSignificantExperience(current_state_)) {
        if (episodic_memory_) {
            episodic_memory_->recordExperience(current_state_);
        }
    }
}
```

### 3. Plasticity Rule Design

#### Standard Plasticity Rule Interface
```cpp
// PlasticityRule.hpp - Base interface
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    virtual void applyPlasticity(
        Synapse& synapse,
        const NeuralEvent& pre_event,
        const NeuralEvent& post_event,
        float neuromodulation_factor = 1.0f
    ) = 0;
    
    virtual std::string getName() const = 0;
    virtual PlasticityType getType() const = 0;
};
```

#### STDP Implementation
```cpp
// STDP.hpp
class STDP : public PlasticityRule {
public:
    STDP(float learning_rate = 0.001f, float tau_plus = 20.0f, float tau_minus = 20.0f)
        : learning_rate_(learning_rate), tau_plus_(tau_plus), tau_minus_(tau_minus) {}
    
    void applyPlasticity(Synapse& synapse,
                        const NeuralEvent& pre_event,
                        const NeuralEvent& post_event,
                        float neuromodulation_factor = 1.0f) override {
        float delta_t = post_event.time_ - pre_event.time_;
        
        if (delta_t > 0) {
            // Post before pre: Long-Term Depression
            float weight_change = learning_rate_ * std::exp(-delta_t / tau_minus_);
            synapse.weight_ += weight_change;
            synapse.addTrace(-weight_change);  // Eligibility trace
        } else {
            // Pre before post: Long-Term Potentiation
            float weight_change = learning_rate_ * std::exp(delta_t / tau_plus_);
            synapse.weight_ += weight_change;
            synapse.addTrace(weight_change);  // Eligibility trace
        }
        
        // Apply neuromodulation
        if (neuromodulation_factor != 1.0f) {
            synapse.weight_ *= neuromodulation_factor;
        }
    }
    
    std::string getName() const override { return "STDP"; }
    PlasticityType getType() const override { return PlasticityType::kSTDP; }
};
```

#### Reward-Modulated STDP
```cpp
// RewardModulatedSTDP.hpp - Enhanced STDP with reward modulation
class RewardModulatedSTDP : public STDP {
public:
    RewardModulatedSTDP(float base_learning_rate = 0.001f, 
                       float neuromodulation_strength = 2.0f)
        : STDP(base_learning_rate), neuromodulation_strength_(neuromodulation_strength) {}
    
    void applyPlasticity(Synapse& synapse,
                        const NeuralEvent& pre_event,
                        const NeuralEvent& post_event,
                        float neuromodulation_factor = 1.0f) override {
        // Apply base STDP
        float original_weight = synapse.weight_;
        STDP::applyPlasticity(synapse, pre_event, post_event, 1.0f);
        
        // Apply reward modulation based on eligibility traces
        float reward_modulation = calculateRewardModulation(synapse, neuromodulation_factor);
        
        // Modify based on reward prediction error
        if (reward_modulation != 0.0f) {
            float weight_change = original_weight - synapse.weight_;
            synapse.weight_ += weight_change * reward_modulation;
            synapse.addTrace(weight_change * (reward_modulation - 1.0f));
        }
    }
    
private:
    float calculateRewardModulation(const Synapse& synapse, float reward_factor) {
        // Calculate modulation based on eligibility traces and reward signals
        float trace = synapse.getEligibilityTrace();
        return 1.0f + (reward_factor - 1.0f) * trace * neuromodulation_strength_;
    }
};
```

### 4. Performance Optimization Best Practices

#### Memory Management
```cpp
// Good: Use memory pools
class NeuronPool {
public:
    static Neuron* allocate() {
        static ThreadLocalBuffer buffer;
        return buffer.allocate();
    }
    
    static void deallocate(Neuron* neuron) {
        static ThreadLocalBuffer buffer;
        buffer.deallocate( neuron);
    }
};

// Use in brain construction
void Brain::initialize() {
    neurons_.reserve(config_.neuron_count_);
    for (size_t i = 0; i < config_.neuron_count_; ++i) {
        neurons_.push_back(NeuronPool::allocate());
    }
}
```

#### Event-Driven Processing
```cpp
// Good: Use event queues for efficiency
class EventQueue {
public:
    void pushSpike(SpikeEvent event) {
        // Lock-free SPSC queue for single producer, multiple consumer
        queue_.push(std::move(event));
    }
    
    bool tryPop(SpikeEvent& event) {
        return queue_.try_pop(event);
    }
};

// Integrate with neural processing
void Brain::processSpikes() {
    SpikeEvent event;
    while (event_queue_.tryPop(event)) {
        processSpike(event.neuron_id_, event.time_);
    }
}
```

#### Parallel Processing
```cpp
// Good: Parallel neural processing
void Brain::parallelProcessNeurons() {
    int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, i]() {
            processNeuronBatch(i);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

void Brain::processNeuronBatch(int thread_id) {
    // Thread-local processing for cache efficiency
    int start = thread_id * neurons_per_thread_;
    int end = (thread_id + 1) * neurons_per_thread_;
    
    for (int i = start; i < end && i < neuron_count_; ++i) {
        processNeuron(i);
    }
}
```

### 5. Development System Best Practices

#### Developmental Stages
```cpp
// Well-structured developmental system
class DevelopmentSystem {
public:
    enum class Stage {
        kInitial,
        kCriticalPeriod,
        kMaturation,
        kAdult,
        kAging
    };
    
    void updateStage(float time) {
        current_stage_ = calculateStage(time);
        
        switch (current_stage_) {
            case Stage::kCriticalPeriod:
                enableHighPlasticity();
                increaseSynaptogenesisRate();
                break;
                
            case Stage::kMaturation:
                decreasePlasticityRate();
                increasePruning();
                break;
                
            case Stage::kAdult:
                stabilizeConnectivity();
                maintainPerformance();
                break;
                
            default:
                // Default stable state
                break;
        }
    }
    
    float getPlasticityRate() const {
        switch (current_stage_) {
            case Stage::kCriticalPeriod: return high_plasticity_rate_;
            case Stage::kMaturation: return medium_plasticity_rate_;
            case Stage::kAdult: return low_plasticity_rate_;
            default: return baseline_plasticity_rate_;
        }
    }
};
```

#### Synaptogenesis and Pruning
```cpp
// Coordinated development processes
void DevelopmentSystem::updateConnectivity(float time) {
    // Age-dependent synaptogenesis
    if (time < CRITICAL_PERIOD_END) {
        // Rapid synaptogenesis during critical period
        float synaptogenesis_rate = calculateSynaptogenesisRate(time);
        createNewSynapses(synaptogenesis_rate * CONNECTION_PROBABILITY);
    }
    
    // Experience-dependent pruning
    if (time > MATURATION_START) {
        // Prune weak connections based on use
        pruneUnusedConnections();
        
        // Strengthen frequently used connections
        strengthenUsedConnections();
    }
}
```

### 6. Neuromodulation Best Practices

#### Dopamine System Integration
```cpp
// Proper dopamine integration with eligibility traces
class DopamineNeuromodulator : public Neuromodulator {
public:
    void apply(ModulatedNeuron& neuron, float reward_prediction_error) override {
        // Apply to plasticity eligibility traces
        neuron.applyDopamineModulation(reward_prediction_error, time_since_last_spike_);
        
        // Also affect neural excitability
        neuron.modulateExcitability(reward_prediction_error * excitability_effect_);
        
        // And modulate working memory
        neuron.modulateWorkingMemory(reward_prediction_error);
    }
};

// Integration in brain loop
void Brain::step(int step) {
    // ... normal processing ...
    
    // Calculate reward prediction error
    float rpe = calculateRewardPredictionError();
    
    // Apply all neuromodulators
    if (dopamine_enabled_) {
        applyNeuromodulation(Dopamine::Type::kDopamine, rpe);
    }
    
    if (curiosity_enabled_) {
        applyNeuromodulation(Curiosity::Type::kCuriosity, rpe);
    }
}
```

#### Novelty Detection
```cpp
// Well-designed novelty detector
class NoveltyDetector {
public:
    float calculateNovelty(const SensoryPercept& percept, 
                         const NeuralPattern& current_pattern) {
        // Compare with prediction
        NeuralPattern predicted_pattern = predictSensoryInput(percept);
        
        // Calculate prediction error as novelty measure
        float novelty = calculatePatternDifference(current_pattern, predicted_pattern);
        
        // Apply thresholding to ignore small variations
        if (novelty < novelty_threshold_) {
            return 0.0f;
        }
        
        // Apply scaling for appropriate magnitude
        return std::min(novelty * novelty_scaling_, 1.0f);
    }
    
private:
    float novelty_threshold_ = 0.1f;      // Ignore small variations
    float novelty_scaling_ = 10.0f;       // Scale to [0,1] range
};
```

### 7. C++ Coding Standards

#### File Structure
```cpp
// Recommended file structure
brain/
  ├── Brain.hpp           // Public interface
  ├── Brain.cpp           // Implementation
  ├── NeuralPopulation.hpp // Population management
  ├── NeuralRegion.hpp    // Region organization
  └── Neuron.hpp          // Individual neuron

neuromodulation/
  ├── Neuromodulator.hpp   // Base class
  ├── Dopamine.hpp        // Dopamine implementation
  ├── Serotonin.hpp       // Serotonin implementation
  └── NoveltyDetector.hpp // Novelty detection

performance/
  ├── EventQueue.hpp      // Spike event handling
  ├── MemoryPool.hpp      // Memory management
  ├── SIMDVectorization.hpp // Vectorized operations
  └── ParallelProcessing.hpp // Multi-threading
```

#### Naming Conventions
```cpp
// Classes: PascalCase
class NeuralWorkingMemory;
class SpikeEvent;
class RewardModulatedSTDP;

// Functions: camelCase
void updateWorkingMemory();
float calculatePlasticityChange();

// Constants: UPPER_SNAKE_CASE
const float CRITICAL_PERIOD_END = 100.0f;
const int MAX_SYNAPSES_PER_NEURON = 1000;

// Types: CamelCase or typedef
using NeuronId = uint32_t;
using SynapseWeight = float;
using Timestamp = uint64_t;
```

#### Documentation Standards
```cpp
// Doxygen-style comments
/**
 * @class NeuralWorkingMemory
 * @brief Maintains persistent neural activity for short-term storage.
 * 
 * The working memory system uses recurrent connections to maintain
 * information over seconds to minutes. It implements competitive
 * selection to retain the most important information.
 * 
 * @note Integration with brain's attention system is required
 *       for optimal functionality.
 * 
 * @see NeuralEpisodicMemory
 * @see NeuralPlanner
 */
class NeuralWorkingMemory {
    /** ... */
};
```

### 8. Testing and Validation

#### Unit Testing Best Practices
```cpp
// Test framework integration
class NeuralWorkingMemoryTest {
public:
    void testPersistentActivity() {
        // Setup
        NeuralWorkingMemory memory;
        memory.initialize(1000);
        
        // Exercise
        NeuralActivity activity({1.0f, 0.5f, 0.0f});
        memory.update(activity);
        
        // Verify
        ASSERT_TRUE(memory.isActive(500));  // 500ms persistence
    }
    
    void testCompetitiveSelection() {
        // Test winner-take-more dynamics
        NeuralActivity activities[] = {
            {0.9f, 0.1f, 0.0f},
            {0.3f, 0.6f, 0.1f},
            {0.2f, 0.2f, 0.6f}
        };
        
        for (const auto& activity : activities) {
            memory.update(activity);
        }
        
        // Verify only strongest signal persists
        ASSERT_GT(memory.getActiveStrength(0), 0.8f);
        ASSERT_LT(memory.getActiveStrength(1), 0.2f);
    }
};
```

#### Performance Testing
```cpp
// Performance benchmark
class PerformanceBenchmark {
public:
    void benchmarkEventQueue() {
        EventQueue queue;
        
        // Setup
        std::vector<SpikeEvent> events;
        for (int i = 0; i < 100000; ++i) {
            events.push_back(SpikeEvent{i, i * 0.1f});
        }
        
        // Benchmark push operations
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& event : events) {
            queue.pushSpike(event);
        }
        auto push_time = std::chrono::high_resolution_clock::now() - start;
        
        // Benchmark pop operations
        start = std::chrono::high_resolution_clock::now();
        SpikeEvent event;
        for (const auto& event : events) {
            queue.pushSpike(event);
        }
        for (int i = 0; i < 100000; ++i) {
            queue.tryPop(event);
        }
        auto pop_time = std::chrono::high_resolution_clock::now() - start;
        
        std::cout << "Push: " << push_time.count() << " ns/spike" << std::endl;
        std::cout << "Pop: " << pop_time.count() << " ns/spike" << std::endl;
    }
};
```

## Documentation Best Practices

### Writing Clear Documentation

#### API Documentation
```cpp
/**
 * @brief Creates a new brain instance with the given configuration.
 * 
 * This function allocates and initializes a brain according to the specified
 * configuration parameters. The brain will be ready for simulation after
 * calling initialize().
 * 
 * @param config Configuration object defining brain parameters. Use
 *               createDefaultConfig() for sensible defaults.
 * @return nlm.Brain A new brain instance
 * 
 * @note The returned brain is in an uninitialized state and requires
 *       a call to initialize() before use.
 * 
 * @throws std::invalid_argument if config is invalid
 * 
 * @example
 * @code
 * import pynlm
 * 
 * config = pynlm.createDefaultConfig()
 * config.set("brain.neuron_count", 1000)
 * brain = pynlm.createBrain(config)
 * brain.initialize()
 * @endcode
 */
Brain createBrain(const Config& config);
```

#### Code Comments
```cpp
// Good: Explain the "why" not the "what"
void processSpikes() {
    // Process all incoming spikes efficiently using event-driven approach
    // Each spike updates target neuron potentials and triggers synaptic transmission
    SpikeEvent event;
    while (spike_queue_.try_pop(event)) {
        // Convert spike time to voltage change based on synapse strength
        float voltage_change = event.synaptic_weight_ * 10.0f;  // Approximate conversion
        neurons_[event.target_id_].addVoltage(voltage_change);
        
        // Check if threshold exceeded and schedule post-synaptic spike
        if (neurons_[event.target_id_].getVoltage() >= neurons_[event.target_id_].getThreshold()) {
            scheduleSpike(event.target_id_, event.time_);
        }
    }
}

// Bad: Comments that add no value
void processSpikes() {
    // Process spikes in the queue
    SpikeEvent event;
    while (spike_queue_.try_pop(event)) {
        // Update neuron voltage
        float voltage_change = event.synaptic_weight_ * 10.0f;
        neurons_[event.target_id_].addVoltage(voltage_change);
        
        // Check threshold
        if (neurons_[event.target_id_].getVoltage() >= neurons_[event.target_id_].getThreshold()) {
            // Schedule spike
            scheduleSpike(event.target_id_, event.time_);
        }
    }
}
```

### Documentation Structure

#### Level 1: Quick Start Guide
- 2 pages maximum
- Installation and basic usage
- Common use cases
- Next steps

#### Level 2: User Guide
- 20-50 pages
- Detailed API reference
- Examples and tutorials
- Configuration and customization
- Troubleshooting

#### Level 3: Developer Guide
- 50-200 pages
- Architecture and design patterns
- Code examples and best practices
- Performance optimization
- Advanced features

#### Level 4: Reference Manual
- 200+ pages
- Complete API documentation
- Technical specifications
- Detailed algorithms
- Research methods

### Documentation Generation

#### Doxygen Configuration
```bash
# Generate documentation
doxygen Doxyfile

# Open generated documentation
open docs/html/index.html
```

#### Doxyfile Configuration
```
GENERATE_LATEX = NO
GENERATE_RTF = NO
GENERATE_XML = YES
GENERATE_HTML = YES
GENERATE_CHI = NO

INPUT = src/
FILE_PATTERNS = *.hpp *.cpp *.md

PROJECT_NAME = "NLM/熙然"
PROJECT_NUMBER = "Phase 6.0"
PROJECT_BRIEF = "An Experimental Artificial Developmental Brain"

EXCLUDE = tests/ build/ doc/
```

### Documentation Deployment

#### Static Site Generation
```bash
# For GitHub Pages
git clone https://github.com/nlm-project/nlm.git
mkdocs build
mkdocs gh-deploy

# For ReadTheDocs
pip install mkdocs-material
set -e
cat > requirements.txt << EOF
mkdocs>=1.5.0
mkdocs-material>=9.0.0
EOF
mkdocs build
sphinx-build -b html docs/ _build/html
```

#### Continuous Integration
```yaml
# .github/workflows/docs.yml
name: Documentation

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.10'
    
    - name: Install dependencies
      run: |
        pip install mkdocs mkdocs-material
    
    - name: Build documentation
      run: |
        mkdocs build --clean
    
    - name: Upload documentation
      uses: actions/upload-artifact@v3
      with:
        name: documentation
        path: site/
```

## Future Documentation Goals

### Phase 7 Documentation Enhancements

#### 1. Interactive Documentation
- API parameter calculators and validators
- Simulation visualizers and debuggers
- Code examples with live execution

#### 2. Advanced Examples
- Research experiment documentation
- Performance benchmarking guides
- Integration examples with external systems
- Custom plasticity rule implementations

#### 3. Research Documentation
- Scientific methodology documentation
- Experiment protocols and procedures
- Data analysis and interpretation guides
- Publication-ready documentation templates

#### 4. User Support
- Comprehensive FAQ
- Troubleshooting database
- Community contribution guidelines
- Advanced user forums

### Continuous Documentation Improvement

#### Documentation Quality Metrics
```python
# Documentation quality analysis
class DocumentationQualityAnalyzer {
public:
    float analyzeCompleteness(const Documentation& doc) {
        // Check if all public APIs are documented
        float api_coverage = calculateApiCoverage(doc);
        
        // Check if examples are provided
        float example_coverage = calculateExampleCoverage(doc);
        
        // Check for troubleshooting information
        float troubleshooting_coverage = hasTroubleshooting(doc);
        
        return (api_coverage * 0.5f) + (example_coverage * 0.3f) + (troubleshooting_coverage * 0.2f);
    }
    
    float analyzeClarity(const Documentation& doc) {
        // Check for clear explanations
        float explanation_quality = assessExplanationQuality(doc);
        
        // Check for logical organization
        float organization_quality = assessOrganization(doc);
        
        // Check for consistency
        float consistency_quality = assessConsistency(doc);
        
        return (explanation_quality * 0.4f) + (organization_quality * 0.3f) + (consistency_quality * 0.3f);
    }
};
```

## References

1. NLM Architecture Documentation
2. NLM Scientific Background
3. NLM Performance Guide
4. NLM Research Methods
5. NLM Experiment Guide
6. Phase 6 Final Audit Report

---

*Document Version: 1.0*
*Last Updated: 2026-10-07*
*Status: In Progress*