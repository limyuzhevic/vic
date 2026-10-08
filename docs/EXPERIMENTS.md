# NLM Experiments Documentation
# Experiment Framework and Implementation

## Overview

This document provides comprehensive information about the NLM experiment framework, including experiment types, implementation details, and usage examples. It serves as a guide for researchers and users to understand and conduct experiments with the artificial brain.

## Experiment Framework Architecture

### Core Components

The NLM experiment framework consists of:

1. **Experiment Base Class**
2. **Experiment Registry**
3. **Parameter System**
4. **Result Collection**
5. **Visualization Tools**
6. **Script Support**

### Experiment Lifecycle

1. **Initialization**
   - Load experiment configuration
   - Set up brain state
   - Configure experiment parameters

2. **Execution**
   - Run simulation steps
   - Collect data
   - Apply experimental manipulations
   - Record results

3. **Analysis**
   - Process collected data
   - Generate statistics
   - Create visualizations
   - Validate hypotheses

4. **Output**
   - Save results
   - Generate reports
   - Create visualizations
   - Store data for further analysis

## Experiment Types

### 1. Connectivity Experiments

**Purpose:** Test basic neural communication and network connectivity

**Key Metrics:**
- Spike propagation efficiency
- Connection reliability
- Network stability
- Information transmission

**Typical Parameters:**
- Neuron count: 500-5000
- Connection probability: 0.1-0.3
- Simulation steps: 100-1000
- Input patterns: Random or structured

**Example Experiment:**
```bash
./nlm run connectivity
```

**Implementation Details:**
1. Initialize brain with specified configuration
2. Inject current into sensory neurons
3. Monitor spike propagation
4. Record connectivity metrics
5. Analyze network stability

### 2. Plasticity Experiments

**Purpose:** Demonstrate experience-dependent synaptic changes

**Key Metrics:**
- Synaptic weight changes
- Learning speed
- Memory retention
- Generalization

**Typical Parameters:**
- Plasticity rule: STDP, Hebbian, or combined
- Learning rate: 0.01-0.1
- Experience duration: 500-5000 steps
- Stimulation patterns: Repeated or random

**Example Experiment:**
```bash
./nlm run plasticity
```

**Implementation Details:**
1. Enable specified plasticity rules
2. Apply repeated input patterns
3. Track synaptic weight changes
4. Measure learning curves
5. Analyze memory consolidation

### 3. STDP Verification Experiments

**Purpose:** Validate spike-timing-dependent plasticity

**Key Metrics:**
- LTP accuracy
- LTD accuracy
- Timing precision
- Weight change magnitude

**Typical Parameters:**
- Time window: 0-100 ms
- Spike frequency: 1-50 Hz
- Number of trials: 100-1000
- Pre-post interval: 0-20 ms

**Example Experiment:**
```bash
./nlm run stdp
```

**Implementation Details:**
1. Create correlated pre-post neuronal activity
2. Measure synaptic weight changes
3. Test potentiation and depression
4. Analyze timing relationships
5. Validate biological plausibility

### 4. Learning Experiments

**Purpose:** Comprehensive learning assessment

**Key Metrics:**
- Task accuracy
- Learning curves
- Retention over time
- Transfer performance

**Typical Parameters:**
- Task complexity: Simple to complex
- Training duration: 500-5000 steps
- Testing frequency: Periodic
- Memory challenges: Recall tasks

**Example Experiment:**
```bash
./nlm run learning
```

**Implementation Details:**
1. Define learning task
2. Implement training protocol
3. Measure learning progress
4. Test retention and generalization
5. Analyze learning strategies

### 5. Integration Demo Experiments

**Purpose:** Demonstrate complete system integration

**Key Metrics:**
- System coherence
- Information flow
- Behavioral output
- Adaptability

**Typical Parameters:**
- Simulation duration: 200-1000 steps
- Input complexity: Natural patterns
- Memory load: Multiple episodes
- Neuromodulation levels: Variable

**Example Experiment:**
```bash
./nlm run demo
```

**Implementation Details:**
1. Run comprehensive brain simulation
2. Test memory system integration
3. Assess neuromodulation effects
4. Validate prediction capabilities
5. Measure overall system performance

### 6. Performance Benchmark Experiments

**Purpose:** Measure computational efficiency

**Key Metrics:**
- Processing speed
- Memory efficiency
- Scalability
- Real-time performance

**Typical Parameters:**
- Network size: 500-10000 neurons
- Simulation complexity: Variable
- Hardware configuration: Multi-core
- Performance targets: Application-specific

**Example Experiment:**
```bash
./nlm run benchmark
```

**Implementation Details:**
1. Configure brain for performance testing
2. Run computational benchmarks
3. Measure resource utilization
4. Analyze scalability
5. Compare with reference implementations

### 7. Ablation Study Experiments

**Purpose:** Test the impact of individual components

**Key Metrics:**
- Performance with component removed
- Compensatory mechanisms
- System robustness
- Functional necessity

**Typical Parameters:**
- Component removal: Complete or partial
- Recovery potential: Variable
- Alternative mechanisms: Present or absent
- Performance comparison: Baseline vs. ablated

**Example Experiment:**
```bash
./nlm run ablation
```

**Implementation Details:**
1. Define baseline performance
2. Remove specific components
3. Measure performance impact
4. Test recovery mechanisms
5. Analyze alternative pathways

## Experiment Implementation

### 1. Experiment Base Class

```cpp
class Experiment {
public:
    virtual ~Experiment() = default;
    
    // Initialize experiment
    virtual bool initialize(const Config& config) = 0;
    
    // Run experiment
    virtual bool run(Brain& brain, int steps) = 0;
    
    // Get experiment results
    virtual ExperimentResults getResults() const = 0;
    
    // Get experiment name
    virtual std::string getName() const = 0;
    
    // Get experiment description
    virtual std::string getDescription() const = 0;
    
protected:
    // Helper methods for common operations
    void logProgress(const std::string& message, int step);
    void recordData(const std::string& name, float value);
    float getParameter(const std::string& name, float defaultValue) const;
    void setParameter(const std::string& name, float value);
};
```

### 2. Experiment Registry

```cpp
class ExperimentRegistry {
public:
    // Register experiment
    static void registerExperiment(std::unique_ptr<Experiment> experiment);
    
    // Get experiment by name
    static std::unique_ptr<Experiment> getExperiment(const std::string& name);
    
    // Get all registered experiments
    static std::vector<std::string> getExperimentNames();
    
    // Clear registry
    static void clear();
};
```

### 3. Experiment Results

```cpp
struct ExperimentResults {
    std::string experimentName;
    std::string description;
    std::vector<float> metrics;
    std::vector<std::vector<float>> timeSeries;
    std::map<std::string, float> parameters;
    std::string timestamp;
    
    // Calculate statistics
    float calculateMean(const std::string& metricName) const;
    float calculateStdDev(const std::string& metricName) const;
    float calculateMax(const std::string& metricName) const;
    float calculateMin(const std::string& metricName) const;
    
    // Export results
    std::string toCSV() const;
    std::string toJSON() const;
    void saveToFile(const std::string& filename) const;
};
```

## Experiment Configuration

### Configuration File Format

```json
{
    "experiment": {
        "name": "connectivity",
        "description": "Test basic neural connectivity",
        "parameters": {
            "neuron_count": 1000,
            "region_count": 1,
            "connection_probability": 0.15,
            "simulation_timestep": 0.001,
            "random_seed": 42
        },
        "metrics": ["spike_count", "connectivity_score", "stability"],
        "output": {
            "save_results": true,
            "output_format": "csv",
            "output_directory": "./experiments/connectivity/"
        }
    }
}
```

### Command-Line Configuration

```bash
./nlm run connectivity --steps 1000 --save_results --output_dir ./my_experiment
```

## Experiment Examples

### Example 1: Simple Connectivity Test

```cpp
class ConnectivityExperiment : public Experiment {
public:
    bool initialize(const Config& config) override {
        // Set up brain configuration
        steps_ = config.get<int>("steps", 1000);
        neuronCount_ = config.get<int>("neuron_count", 1000);
        
        return true;
    }
    
    bool run(Brain& brain, int steps) override {
        // Record initial state
        size_t initialSpikes = brain.getTotalSpikeCount();
        
        // Inject input into sensory neurons
        for (size_t i = 0; i < 10 && i < brain.getTotalNeuronCount() / 4; ++i) {
            brain.injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        }
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            brain.step(step, step * 0.001);
            
            // Record progress
            if (step % 100 == 0) {
                logProgress("Step " + std::to_string(step), step);
            }
        }
        
        // Calculate results
        size_t totalSpikes = brain.getTotalSpikeCount() - initialSpikes;
        
        // Store results
        recordData("total_spikes", static_cast<float>(totalSpikes));
        recordData("spike_rate", static_cast<float>(totalSpikes) / steps);
        
        return true;
    }
    
    std::string getName() const override {
        return "connectivity";
    }
    
    std::string getDescription() const override {
        return "Test basic neural connectivity by measuring spike propagation";
    }
    
private:
    int steps_;
    int neuronCount_;
};
```

### Example 2: Plasticity Learning Experiment

```cpp
class PlasticityExperiment : public Experiment {
public:
    bool initialize(const Config& config) override {
        // Set up learning parameters
        learningRate_ = config.get<float>("learning_rate", 0.01);
        trainingSteps_ = config.get<int>("training_steps", 1000);
        
        return true;
    }
    
    bool run(Brain& brain, int steps) override {
        // Reset brain for clean experiment
        brain.reset();
        brain.initialize();
        
        // Enable plasticity
        if (auto* region = brain.getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain.getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Apply learning stimuli
        for (int step = 0; step < trainingSteps_; ++step) {
            // Random stimulation
            if (rand() % 100 < learningRate_ * 100) {
                brain.injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            }
            
            brain.step(step, step * 0.001);
            
            // Record progress
            if (step % 100 == 0) {
                logProgress("Learning step " + std::to_string(step), step);
            }
        }
        
        // Calculate final weights
        std::vector<float> finalWeights;
        if (auto* region = brain.getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        // Calculate learning metrics
        float totalChange = 0.0f;
        size_t weightCount = 0;
        
        for (size_t i = 0; i < std::min(initialWeights.size(), finalWeights.size()); ++i) {
            totalChange += std::abs(finalWeights[i] - initialWeights[i]);
            weightCount++;
        }
        
        float avgChange = weightCount > 0 ? totalChange / weightCount : 0.0f;
        
        // Store results
        recordData("avg_weight_change", avgChange);
        recordData("total_synapses_changed", weightCount);
        recordData("learning_efficiency", avgChange / trainingSteps_);
        
        return true;
    }
    
    std::string getName() const override {
        return "plasticity";
    }
    
    std::string getDescription() const override {
        return "Demonstrate synaptic plasticity through experience";
    }
    
private:
    float learningRate_;
    int trainingSteps_;
};
```

### Example 3: STDP Verification Experiment

```cpp
class STDPExperiment : public Experiment {
public:
    bool initialize(const Config& config) override {
        // Set up STDP parameters
        stdpLTP_ = config.get<float>("stdp_ltp", 0.01);
        stdpLTD_ = config.get<float>("stdp_ltd", 0.012);
        stdpTau_ = config.get<float>("stdp_tau", 20.0f);
        
        return true;
    }
    
    bool run(Brain& brain, int steps) override {
        // Reset and initialize brain
        brain.reset();
        brain.initialize();
        
        // Get first few synapses
        auto* region = brain.getRegion(RegionId(1));
        if (!region) return false;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) return false;
        
        // Record initial weights
        std::vector<float> initialWeights;
        for (size_t i = 0; i < 5; ++i) {
            initialWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
        }
        
        // Create correlated activity: pre then post
        for (int trial = 0; trial < 50; ++trial) {
            // Get neurons
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain.step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Calculate weight changes
        std::vector<float> finalWeights;
        for (size_t i = 0; i < 5; ++i) {
            finalWeights.push_back(synapses[i]->getWeight());
        }
        
        float totalChange = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalChange += finalWeights[i] - initialWeights[i];
        }
        
        float avgChange = totalChange / 5.0f;
        
        // Store results
        recordData("avg_stdp_change", avgChange);
        recordData("total_delta_weight", totalChange);
        recordData("stdp_accuracy", std::abs(avgChange) > 0.001f ? 1.0f : 0.0f);
        
        return true;
    }
    
    std::string getName() const override {
        return "stdp";
    }
    
    std::string getDescription() const override {
        return "Verify spike-timing-dependent plasticity";
    }
    
private:
    float stdpLTP_;
    float stdpLTD_;
    float stdpTau_;
};
```

## Experiment Scripts

### Script Format

```bash
# Experiment configuration
experiment: connectivity
steps: 1000
output_file: connectivity_results.csv

# Commands
step 500
record "connectivity_analysis"
step 1000
export_results
```

### Example Script File

```bash
# Connectivity experiment script
# NLM Experiment Script

# Configuration
experiment: connectivity
steps: 1000
output: connectivity_results.csv

# Initial setup
step 0
record "initialization"

# Main experiment
step 100
record "early_progress"
step 500
record "mid_experiment"
step 1000
record "final_results"

# Save results
export_results

# Verification
run verification_experiment
```

## Experiment Results Analysis

### Result Analysis Methods

1. **Statistical Analysis**
   - Mean, median, standard deviation
   - Confidence intervals
   - Hypothesis testing
   - Effect size calculation

2. **Time Series Analysis**
   - Learning curves
   - Temporal dynamics
   - Pattern detection
   - Change point detection

3. **Comparative Analysis**
   - Baseline vs. experimental
   - Multiple conditions
   - Control experiments
   - Statistical significance

### Visualization Examples

```python
import matplotlib.pyplot as plt
import numpy as np

# Learning curve
steps = np.arange(0, 1000, 100)
spike_rates = np.array([10, 25, 50, 75, 100, 125, 150, 175, 200])

plt.figure(figsize=(10, 6))
plt.plot(steps, spike_rates, 'b-', linewidth=2, marker='o')
plt.xlabel('Simulation Steps')
plt.ylabel('Spike Rate')
plt.title('Learning Curve: Plasticity Experiment')
plt.grid(True, alpha=0.3)
plt.savefig('learning_curve.png')
plt.show()

# Synaptic weight changes
initial_weights = np.random.normal(0.1, 0.02, 100)
final_weights = initial_weights + np.random.normal(0, 0.01, 100)

plt.figure(figsize=(10, 6))
plt.scatter(initial_weights, final_weights, alpha=0.6, c=np.arange(100), cmap='viridis')
plt.plot([-0.2, 0.4], [-0.2, 0.4], 'r--', alpha=0.8)
plt.xlabel('Initial Synaptic Weight')
plt.ylabel('Final Synaptic Weight')
plt.title('STDP: Initial vs. Final Weights')
plt.colorbar(label='Time Step')
plt.grid(True, alpha=0.3)
plt.savefig('weight_changes.png')
plt.show()
```

## Experiment Best Practices

### 1. Experimental Design

1. **Control Groups**
   - Always include control conditions
   - Compare to baseline performance
   - Use randomized assignment

2. **Replication**
   - Run experiments multiple times
   - Use consistent parameters
   - Document all variations

3. **Sample Size**
   - Use adequate number of trials
   - Consider statistical power
   - Balance with computational cost

### 2. Data Collection

1. **Standardized Metrics**
   - Define clear outcome measures
   - Use consistent units
   - Document all variables

2. **Data Quality**
   - Check for anomalies
   - Validate data integrity
   - Implement error detection

3. **Data Management**
   - Organize data systematically
   - Use descriptive file names
   - Maintain version control

### 3. Analysis

1. **Statistical Methods**
   - Choose appropriate tests
   - Check assumptions
   - Report effect sizes
   - Consider multiple comparisons

2. **Visualization**
   - Use clear, informative plots
   - Include error bars
   - Use consistent formatting
   - Label all axes

3. **Documentation**
   - Document analysis methods
   - Record parameter settings
   - Store raw data
   - Save processed results

## Experiment Validation

### 1. Internal Validity

1. **Control for Confounds**
   - Randomize conditions
   - Counterbalance designs
   - Use proper controls

2. **Standardization**
   - Consistent procedures
   - Automated data collection
   - Reproducible methods

3. **Reliability**
   - Test-retest reliability
   - Inter-rater reliability
   - Internal consistency

### 2. External Validity

1. **Generalizability**
   - Test diverse conditions
   - Use representative samples
   - Consider ecological validity

2. **Biological Plausibility**
   - Compare to biological data
   - Use established models
   - Validate mechanisms

3. **Practical Relevance**
   - Address real-world problems
   - Consider applications
   - Evaluate cost-effectiveness

## Future Experiment Development

### 1. New Experiment Types

1. **Social Learning**
   - Model observational learning
   - Test imitation behavior
   - Study social transmission

2. **Causal Reasoning**
   - Test cause-effect understanding
   - Model counterfactual thinking
   - Assess intervention effects

3. **Creative Generation**
   - Test novel pattern generation
   - Measure creativity metrics
   - Evaluate novelty detection

### 2. Advanced Methodologies

1. **Machine Learning Integration**
   - Use AI for experiment design
   - Implement adaptive experiments
   - Apply reinforcement learning

2. **High-Throughput Screening**
   - Automate experiment execution
   - Scale to many conditions
   - Use parallel processing

3. **Virtual Reality**
   - Implement immersive environments
   - Add sensory-rich inputs
   - Test embodied cognition

## Conclusion

The NLM experiment framework provides a powerful platform for investigating artificial brain development and learning. Through systematic experimental design, rigorous analysis, and comprehensive documentation, researchers can gain deep insights into neural computation and cognitive processes.

The framework supports a wide range of experiments, from simple connectivity tests to complex learning assessments, providing valuable tools for both basic research and applied applications.

Future development will focus on expanding the experiment suite, improving analytical methods, and enhancing integration with modern AI approaches.

---

*This documentation is a living document that will be continuously updated as the NLM project evolves.*

© 2026 NLM Project. All rights reserved.
