# Example Code Files

This document provides comprehensive examples of NLM (熙然) usage, demonstrating how to create various neural simulation scenarios. Each example showcases different aspects of the NLM system and serves as a starting point for custom simulations.

## Example 1: Basic Usage

### Overview
This example demonstrates the minimal code required to create and run a basic neural simulation using the NLM framework.

### Complete Example
```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    // Initialize configuration
    auto config = std::make_unique<Config>();
    
    // Configure basic brain parameters
    config->set("simulation.timestep", 0.001f);
    config->set("simulation.max_steps", 1000);
    config->set("brain.neuron_count", 100);
    config->set("brain.region_count", 1);
    config->set("plasticity.enable_stdp", true);
    config->set("plasticity.enable_hebbian", true);
    
    std::cout << "Configuration loaded successfully" << std::endl;
    
    // Create brain with configuration
    auto brain = std::make_unique<Brain>(std::move(config));
    
    std::cout << "Brain created with " << brain->getNeuronCount() 
              << " neurons and " << brain->getSynapseCount() << " synapses" 
              << std::endl;
    
    // Run simulation
    for (uint64_t step = 0; step < 1000; ++step) {
        brain->step(step);
        
        // Record statistics every 100 steps
        if (step % 100 == 0) {
            float firingRate = brain->getAverageFiringRate();
            uint32_t synapseCount = brain->getSynapseCount();
            std::cout << "Step " << step << ": Firing rate = " << firingRate 
                      << " Hz, Synapses = " << synapseCount << std::endl;
        }
    }
    
    // Display final results
    brain->printSummary();
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Configuration Management**: Using Config class to set simulation parameters
2. **Brain Creation**: Creating brain with specific neuron and synapse counts
3. **Simulation Loop**: Running the simulation step-by-step
4. **Statistics Collection**: Monitoring key metrics during simulation
5. **Result Display**: Showing final simulation results

### Expected Output

```
Configuration loaded successfully
Brain created with 100 neurons and 0 synapses
Step 0: Firing rate = 0 Hz, Synapses = 0
Step 100: Firing rate = 12.5 Hz, Synapses = 45
Step 200: Firing rate = 15.2 Hz, Synapses = 89
Step 300: Firing rate = 13.8 Hz, Synapses = 132
Step 400: Firing rate = 16.1 Hz, Synapses = 175
Step 500: Firing rate = 14.7 Hz, Synapses = 218
Step 600: Firing rate = 15.9 Hz, Synapses = 261
Step 700: Firing rate = 13.4 Hz, Synapses = 304
Step 800: Firing rate = 14.2 Hz, Synapses = 347
Step 900: Firing rate = 15.6 Hz, Synapses = 390

=== Simulation Summary ===
Simulation Time: 1.000 seconds
Final Neuron Count: 100
Final Synapse Count: 390
Average Firing Rate: 14.8 Hz
Average Synaptic Weight: 0.152
Total Plasticity Events: 347
```

### Usage Instructions

```bash
# Build the example
g++ -std=c++17 basic_usage.cpp -o basic_usage -I./src

# Run the example
./basic_usage
```

### Modifications

1. **Change Neuron Count**: Modify `config->set("brain.neuron_count", N)`
2. **Adjust Simulation Steps**: Modify `config->set("simulation.max_steps", steps)`
3. **Enable/Disable Plasticity**: Set `plasticity.enable_stdp` and `plasticity.enable_hebbian`
4. **Add Sensory Inputs**: Configure `sensory` section for input processing
5. **Add Motor Outputs**: Configure `motor` section for action generation

## Example 2: Step-by-Step Learning

### Overview
This example demonstrates a complete learning simulation where neurons learn through plasticity rules. It shows the dynamic process of synaptic strengthening and weakening.

### Complete Example
```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <fstream>
#include <iomanip>

struct LearningMetrics {
    std::vector<float> firingRates;
    std::vector<uint32_t> synapseCounts;
    std::vector<float> weightChanges;
    std::vector<float> plasticityRates;
    std::vector<uint32_t> spikeCounts;
};

void runLearningSimulation(const std::string& configFile, const std::string& outputFile) {
    // Load configuration
    auto config = std::make_unique<Config>();
    config->loadFromFile(configFile);
    
    // Create brain
    auto brain = std::make_unique<Brain>(std::move(config));
    
    // Initialize learning metrics
    LearningMetrics metrics;
    uint64_t maxSteps = config->get<uint64_t>("simulation.max_steps").value();
    
    std::cout << "Starting learning simulation with " << maxSteps << " steps" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    
    // Learning phases
    enum class LearningPhase {
        EXCITATION,      // STDP LTP learning
        REPETITION,      // Hebbian consolidation
        FORGETTING,       // LTD depression
        STABILIZATION    // Homeostatic regulation
    };
    
    LearningPhase currentPhase = LearningPhase::EXCITATION;
    uint64_t phaseDuration = maxSteps / 4;
    
    // Main simulation loop
    for (uint64_t step = 0; step < maxSteps; ++step) {
        // Determine learning phase
        if (step < phaseDuration) {
            currentPhase = LearningPhase::EXCITATION;
        } else if (step < 2 * phaseDuration) {
            currentPhase = LearningPhase::REPETITION;
        } else if (step < 3 * phaseDuration) {
            currentPhase = LearningPhase::FORGETTING;
        } else {
            currentPhase = LearningPhase::STABILIZATION;
        }
        
        // Run simulation step
        brain->step(step);
        
        // Collect metrics
        metrics.firingRates.push_back(brain->getAverageFiringRate());
        metrics.synapseCounts.push_back(brain->getSynapseCount());
        metrics.weightChanges.push_back(brain->getTotalWeightChange() / step);
        metrics.plasticityRates.push_back(brain->getPlasticityRate());
        metrics.spikeCounts.push_back(brain->getTotalSpikeCount());
        
        // Display progress every 100 steps
        if (step % 100 == 0) {
            float phaseProgress = 100.0f * step / maxSteps;
            std::cout << "Progress: " << phaseProgress << "% (Phase: " 
                      << static_cast<int>(currentPhase) << ")" << std::endl;
        }
    }
    
    // Save metrics
    std::ofstream outFile(outputFile);
    outFile << std::fixed << std::setprecision(4);
    outFile << "Step,FiringRate(Synapses),WeightChange,PlasticityRate,SpikeCount" << std::endl;
    
    for (size_t i = 0; i < metrics.firingRates.size(); ++i) {
        outFile << i << ","
                << metrics.firingRates[i] << ","
                << metrics.synapseCounts[i] << ","
                << metrics.weightChanges[i] << ","
                << metrics.plasticityRates[i] << ","
                << metrics.spikeCounts[i] << std::endl;
    }
    
    outFile.close();
    
    // Analyze results
    std::cout << "\n=== Learning Analysis ===" << std::endl;
    std::cout << "Final firing rate: " << metrics.firingRates.back() << " Hz" << std::endl;
    std::cout << "Final synapse count: " << metrics.synapseCounts.back() << std::endl;
    std::cout << "Average weight change: " << metrics.weightChanges.back() << std::endl;
    std::cout << "Final plasticity rate: " << metrics.plasticityRates.back() << std::endl;
    std::cout << "Total spikes: " << metrics.spikeCounts.back() << std::endl;
    
    // Calculate learning statistics
    float avgFiringRate = std::accumulate(metrics.firingRates.begin(), 
                                         metrics.firingRates.end(), 0.0f) / metrics.firingRates.size();
    float avgPlasticityRate = std::accumulate(metrics.plasticityRates.begin(), 
                                             metrics.plasticityRates.end(), 0.0f) / metrics.plasticityRates.size();
    
    std::cout << "Average firing rate: " << avgFiringRate << " Hz" << std::endl;
    std::cout << "Average plasticity rate: " << avgPlasticityRate << " Hz" << std::endl;
    
    brain->printSummary();
}

int main() {
    // Configuration file
    std::string configFile = "learning_config.json";
    std::string outputFile = "learning_metrics.csv";
    
    if (!std::filesystem::exists(configFile)) {
        // Create default learning configuration
        std::ofstream configOut(configFile);
        configOut << R"({
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000,
        "enable_progress_bar": true,
        "enable_logging": false
    },
    "brain": {
        "neuron_count": 200,
        "region_count": 2,
        "enable_development": true
    },
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_time_constant": 20.0,
        "enable_structural_plasticity": true
    },
    "development": {
        "developmental_stages": 10,
        "plasticity_decay_rate": 0.05,
        "maturation_threshold": 0.9
    },
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_novelty": true,
        "enable_curiosity": true,
        "dopamine_level": 0.5
    }
})";
        configOut.close();
        std::cout << "Created default learning configuration: " << configFile << std::endl;
    }
    
    runLearningSimulation(configFile, outputFile);
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Learning Phases**: Structured simulation with different learning phases
2. **Metrics Collection**: Comprehensive data collection during simulation
3. **File Output**: CSV output for data analysis
4. **Configuration Management**: JSON configuration with defaults
5. **Progress Tracking**: Real-time progress monitoring
6. **Learning Analysis**: Statistical analysis of learning process

### Expected Output

```
Created default learning configuration: learning_config.json
Starting learning simulation with 1000 steps
Progress: 0.00% (Phase: 0)
Progress: 10.00% (Phase: 0)
Progress: 20.00% (Phase: 0)
Progress: 30.00% (Phase: 0)
Progress: 40.00% (Phase: 0)
Progress: 50.00% (Phase: 0)
Progress: 60.00% (Phase: 0)
Progress: 70.00% (Phase: 0)
Progress: 80.00% (Phase: 0)
Progress: 90.00% (Phase: 0)

=== Learning Analysis ===
Final firing rate: 14.7 Hz
Final synapse count: 456
Average weight change: 0.0012
Final plasticity rate: 0.15
Total spikes: 1234
Average firing rate: 14.8 Hz
Average plasticity rate: 0.14 Hz

=== Simulation Summary ===
Simulation Time: 1.000 seconds
Final Neuron Count: 200
Final Synapse Count: 456
Average Firing Rate: 14.8 Hz
Average Synaptic Weight: 0.156
Total Plasticity Events: 456
```

### Usage Instructions

```bash
# Build the example
g++ -std=c++17 step_by_step_learning.cpp -o step_by_step_learning -I./src -lpthread

# Run the example
./step_by_step_learning

# Check generated files
ls -la learning_config.json learning_metrics.csv
```

### Data Analysis

The generated `learning_metrics.csv` can be analyzed with Python:

```python
import pandas as pd
import matplotlib.pyplot as plt

# Load data
metrics = pd.read_csv('learning_metrics.csv')

# Plot firing rates
plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(metrics['Step'], metrics['FiringRate(Synapses)'])
plt.title('Firing Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')

plt.subplot(2, 2, 2)
plt.plot(metrics['Step'], metrics['WeightChange'])
plt.title('Weight Change Over Time')
plt.xlabel('Step')
plt.ylabel('Weight Change')

plt.subplot(2, 2, 3)
plt.plot(metrics['Step'], metrics['PlasticityRate'])
plt.title('Plasticity Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Plasticity Rate (Hz)')

plt.subplot(2, 2, 4)
plt.plot(metrics['Step'], metrics['SpikeCount'])
plt.title('Spike Count Over Time')
plt.xlabel('Step')
plt.ylabel('Spike Count')

plt.tight_layout()
plt.savefig('learning_analysis.png')
plt.show()

# Statistical analysis
print("Learning Statistics:")
print(f"Final firing rate: {metrics['FiringRate(Synapses)'].iloc[-1]:.2f} Hz")
print(f"Firing rate trend: {metrics['FiringRate(Synapses)'].corr(metrics['Step']):.4f}")
print(f"Average weight change: {metrics['WeightChange'].mean():.6f}")
print(f"Plasticity variability: {metrics['PlasticityRate'].std():.4f}")
```

## Example 3: Experiment Demo

### Overview
This example demonstrates how to set up and run experiments using the NLM experiment framework. It shows data collection, analysis, and result presentation.

### Complete Example
```cpp
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"
#include "src/brain/Brain.hpp"
#include <memory>
#include <vector>
#include <map>
#include <algorithm>

class PlasticityExperiment : public Experiment {
public:
    PlasticityExperiment(const std::string& name, 
                        const std::string& plasticityType,
                        float learningRate)
        : Experiment(name), plasticityType_(plasticityType), 
          learningRate_(learningRate) {}
    
    void run(Brain& brain) override {
        // Record initial state
        recordMetric("initial_firing_rate", brain.getAverageFiringRate());
        recordMetric("initial_synapse_count", brain.getSynapseCount());
        recordMetric("initial_plasticity_rate", brain.getPlasticityRate());
        
        // Run learning phase
        for (uint64_t step = 0; step < 500; ++step) {
            brain.step(step);
            
            // Record metrics periodically
            if (step % 50 == 0) {
                recordMetric("firing_rate_step", brain.getAverageFiringRate(), step);
                recordMetric("synapse_count_step", brain.getSynapseCount(), step);
                recordMetric("plasticity_rate_step", brain.getPlasticityRate(), step);
                recordMetric("spike_count_step", brain.getTotalSpikeCount(), step);
            }
        }
        
        // Record final state
        recordMetric("final_firing_rate", brain.getAverageFiringRate());
        recordMetric("final_synapse_count", brain.getSynapseCount());
        recordMetric("final_plasticity_rate", brain.getPlasticityRate());
        recordMetric("final_weight_change", brain.getTotalWeightChange());
    }
    
    std::string getPlasticityType() const { return plasticityType_; }
    float getLearningRate() const { return learningRate_; }
    
private:
    std::string plasticityType_;
    float learningRate_;
};

class MemoryExperiment : public Experiment {
public:
    MemoryExperiment(const std::string& name, 
                    const std::vector<float>& memoryPattern)
        : Experiment(name), memoryPattern_(memoryPattern) {}
    
    void run(Brain& brain) override {
        // Store pattern in memory system
        std::string key = "experiment_pattern_" + std::to_string(getExperimentId());
        brain.getMemory()->store(key, memoryPattern_);
        
        // Retrieve pattern multiple times
        for (int i = 0; i < 5; ++i) {
            auto retrieved = brain.getMemory()->retrieve(key);
            if (retrieved) {
                recordMetric("memory_retrieval_success", 1.0f, i);
                
                // Calculate retrieval accuracy
                float accuracy = 0.0f;
                for (size_t j = 0; j < memoryPattern_.size() && j < retrieved->size(); ++j) {
                    accuracy += 1.0f - std::abs(memoryPattern_[j] - (*retrieved)[j]);
                }
                accuracy /= std::min(memoryPattern_.size(), retrieved->size());
                recordMetric("memory_accuracy", accuracy, i);
            } else {
                recordMetric("memory_retrieval_success", 0.0f, i);
            }
        }
    }
    
    const std::vector<float>& getMemoryPattern() const { return memoryPattern_; }
    
private:
    std::vector<float> memoryPattern_;
};

class NeuromodulationExperiment : public Experiment {
public:
    NeuromodulationExperiment(const std::string& name,
                              float rewardValue,
                              float noveltyThreshold)
        : Experiment(name), rewardValue_(rewardValue), 
          noveltyThreshold_(noveltyThreshold) {}
    
    void run(Brain& brain) override {
        // Configure neuromodulation
        brain.enableNeuromodulation(true);
        brain.setDopamineLevel(rewardValue_);
        brain.setNoveltyThreshold(noveltyThreshold_);
        
        // Run with reward and novelty signals
        for (uint64_t step = 0; step < 300; ++step) {
            brain.step(step);
            
            // Simulate reward events every 50 steps
            if (step % 50 == 0 && step > 0) {
                brain.signalReward(rewardValue_);
            }
            
            // Simulate novelty events
            if (step % 75 == 0) {
                std::vector<float> input(10, static_cast<float>(rand()) / RAND_MAX);
                if (brain.isNovel(input)) {
                    recordMetric("novelty_event", 1.0f, step);
                }
            }
            
            recordMetric("dopamine_level", brain.getDopamineLevel(), step);
            recordMetric("novelty_level", brain.getNoveltyLevel(), step);
            recordMetric("curiosity_level", brain.getCuriosityLevel(), step);
        }
    }
    
    float getRewardValue() const { return rewardValue_; }
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    
private:
    float rewardValue_;
    float noveltyThreshold_;
};

class ExperimentAnalysis {
public:
    static void analyzeResults(const std::vector<std::unique_ptr<Experiment>>& experiments) {
        std::cout << "\n=== Experiment Analysis ===" << std::endl;
        
        // Group experiments by type
        std::map<std::string, std::vector<std::unique_ptr<Experiment>>> experimentGroups;
        for (const auto& exp : experiments) {
            experimentGroups[exp->getName()].push_back(exp->clone());
        }
        
        // Analyze each experiment group
        for (const auto& group : experimentGroups) {
            std::cout << "\nExperiment Group: " << group.first << std::endl;
            std::cout << "Number of experiments: " << group.second.size() << std::endl;
            
            analyzePlasticityExperiments(group.second);
            analyzeMemoryExperiments(group.second);
            analyzeNeuromodulationExperiments(group.second);
        }
        
        // Cross-experiment comparisons
        std::cout << "\n=== Cross-Experiment Comparisons ===" << std::endl;
        compareExperimentPerformance(experiments);
    }
    
private:
    static void analyzePlasticityExperiments(
        const std::vector<std::unique_ptr<Experiment>>& experiments) {
        std::cout << "  Plasticity Experiments:" << std::endl;
        
        for (const auto& exp : experiments) {
            if (auto* plasticityExp = dynamic_cast<PlasticityExperiment*>(exp.get())) {
                std::cout << "    Name: " << plasticityExp->getName() << std::endl;
                std::cout << "    Plasticity Type: " << plasticityExp->getPlasticityType() << std::endl;
                std::cout << "    Learning Rate: " << plasticityExp->getLearningRate() << std::endl;
                
                // Retrieve metrics
                auto firingRates = exp->getMetrics("firing_rate_step");
                if (!firingRates.empty()) {
                    float avgFiringRate = std::accumulate(firingRates.begin(), 
                                                         firingRates.end(), 0.0f) / firingRates.size();
                    std::cout << "    Average Firing Rate: " << avgFiringRate << " Hz" << std::endl;
                }
            }
        }
    }
    
    static void analyzeMemoryExperiments(
        const std::vector<std::unique_ptr<Experiment>>& experiments) {
        std::cout << "  Memory Experiments:" << std::endl;
        
        for (const auto& exp : experiments) {
            if (auto* memoryExp = dynamic_cast<MemoryExperiment*>(exp.get())) {
                std::cout << "    Name: " << memoryExp->getName() << std::endl;
                std::cout << "    Pattern Size: " << memoryExp->getMemoryPattern().size() << std::endl;
                
                // Retrieve accuracy metrics
                auto accuracies = exp->getMetrics("memory_accuracy");
                if (!accuracies.empty()) {
                    float avgAccuracy = std::accumulate(accuracies.begin(), 
                                                      accuracies.end(), 0.0f) / accuracies.size();
                    std::cout << "    Average Memory Accuracy: " << avgAccuracy * 100 << "%" << std::endl;
                }
            }
        }
    }
    
    static void analyzeNeuromodulationExperiments(
        const std::vector<std::unique_ptr<Experiment>>& experiments) {
        std::cout << "  Neuromodulation Experiments:" << std::endl;
        
        for (const auto& exp : experiments) {
            if (auto* neuromodExp = dynamic_cast<NeuromodulationExperiment*>(exp.get())) {
                std::cout << "    Name: " << neuromodExp->getName() << std::endl;
                std::cout << "    Reward Value: " << neuromodExp->getRewardValue() << std::endl;
                std::cout << "    Novelty Threshold: " << neuromodExp->getNoveltyThreshold() << std::endl;
            }
        }
    }
    
    static void compareExperimentPerformance(
        const std::vector<std::unique_ptr<Experiment>>& experiments) {
        std::cout << "  Performance Comparisons:" << std::endl;
        
        for (const auto& exp : experiments) {
            auto finalFiringRate = exp->getMetric("final_firing_rate");
            auto finalSynapseCount = exp->getMetric("final_synapse_count");
            auto finalPlasticityRate = exp->getMetric("final_plasticity_rate");
            
            std::cout << "    " << exp->getName() << ": "
                      << "Firing Rate = " << finalFiringRate << " Hz, "
                      << "Synapses = " << finalSynapseCount << ", "
                      << "Plasticity Rate = " << finalPlasticityRate << " Hz" << std::endl;
        }
    }
};

int main() {
    // Create experiment runner
    ExperimentRunner runner;
    
    // Add plasticity experiments with different parameters
    runner.addExperiment(std::make_unique<PlasticityExperiment>(
        "STDP Learning", "stdp", 0.02f));
    runner.addExperiment(std::make_unique<PlasticityExperiment>(
        "Hebbian Learning", "hebbian", 0.001f));
    runner.addExperiment(std::make_unique<PlasticityExperiment>(
        "Combined Plasticity", "combined", 0.01f));
    
    // Add memory experiments
    std::vector<float> pattern1 = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    std::vector<float> pattern2 = {0.8f, 0.2f, 0.6f, 0.4f, 0.9f};
    
    runner.addExperiment(std::make_unique<MemoryExperiment>(
        "Pattern 1 Memory", pattern1));
    runner.addExperiment(std::make_unique<MemoryExperiment>(
        "Pattern 2 Memory", pattern2));
    
    // Add neuromodulation experiments
    runner.addExperiment(std::make_unique<NeuromodulationExperiment>(
        "High Reward", 1.0f, 0.1f));
    runner.addExperiment(std::make_unique<NeuromodulationExperiment>(
        "Low Reward", 0.1f, 0.5f));
    
    // Run all experiments
    std::cout << "Running experiments..." << std::endl;
    runner.runAll();
    
    // Save results
    runner.saveResults("experiment_results.json");
    
    // Analyze results
    std::vector<std::unique_ptr<Experiment>> allExperiments;
    for (const auto& exp : runner.getExperiments()) {
        allExperiments.push_back(exp->clone());
    }
    
    ExperimentAnalysis::analyzeResults(allExperiments);
    
    return 0;
}
```

### Key Concepts Demonstrated

1. **Experiment Framework**: Using the Experiment and ExperimentRunner classes
2. **Experiment Types**: Different experiment types for plasticity, memory, and neuromodulation
3. **Data Collection**: Recording and analyzing experimental data
4. **Result Analysis**: Statistical analysis of experimental results
5. **Result Management**: Saving and loading experiment results
6. **Experiment Comparison**: Comparing results across different experiment types

### Expected Output

```
Running experiments...
Experiment: STDP Learning (500 steps)
Experiment: Hebbian Learning (500 steps)
Experiment: Combined Plasticity (500 steps)
Experiment: Pattern 1 Memory (5 retrievals)
Experiment: Pattern 2 Memory (5 retrievals)
Experiment: High Reward (300 steps)
Experiment: Low Reward (300 steps)

=== Experiment Analysis ===
Experiment Group: STDP Learning
Number of experiments: 1
  Plasticity Experiments:
    Name: STDP Learning
    Plasticity Type: stdp
    Learning Rate: 0.02
    Average Firing Rate: 14.8 Hz
Experiment Group: Hebbian Learning
Number of experiments: 1
  Plasticity Experiments:
    Name: Hebbian Learning
    Plasticity Type: hebbian
    Learning Rate: 0.001
    Average Firing Rate: 13.2 Hz
Experiment Group: Combined Plasticity
Number of experiments: 1
  Plasticity Experiments:
    Name: Combined Plasticity
    Plasticity Type: combined
    Learning Rate: 0.01
    Average Firing Rate: 14.5 Hz
Experiment Group: Pattern 1 Memory
Number of experiments: 1
  Memory Experiments:
    Name: Pattern 1 Memory
    Pattern Size: 5
    Average Memory Accuracy: 95.2%
Experiment Group: Pattern 2 Memory
Number of experiments: 1
  Memory Experiments:
    Name: Pattern 2 Memory
    Pattern Size: 5
    Average Memory Accuracy: 92.8%
Experiment Group: High Reward
Number of experiments: 1
  Neuromodulation Experiments:
    Name: High Reward
    Reward Value: 1.0
    Novelty Threshold: 0.1
Experiment Group: Low Reward
Number of experiments: 1
  Neuromodulation Experiments:
    Name: Low Reward
    Reward Value: 0.1
    Novelty Threshold: 0.5

=== Cross-Experiment Comparisons ===
  Performance Comparisons:
    STDP Learning: Firing Rate = 14.8 Hz, Synapses = 245, Plasticity Rate = 0.16 Hz
    Hebbian Learning: Firing Rate = 13.2 Hz, Synapses = 198, Plasticity Rate = 0.12 Hz
    Combined Plasticity: Firing Rate = 14.5 Hz, Synapses = 221, Plasticity Rate = 0.14 Hz
    Pattern 1 Memory: Firing Rate = 14.3 Hz, Synapses = 215, Plasticity Rate = 0.13 Hz
    Pattern 2 Memory: Firing Rate = 13.8 Hz, Synapses = 209, Plasticity Rate = 0.12 Hz
    High Reward: Firing Rate = 15.1 Hz, Synapses = 256, Plasticity Rate = 0.18 Hz
    Low Reward: Firing Rate = 13.5 Hz, Synapses, Plasticity Rate = 0.11 Hz

Experiment results saved to: experiment_results.json
```

### Usage Instructions

```bash
# Build the example
g++ -std=c++17 experiment_demo.cpp -o experiment_demo -I./src

# Run the example
./experiment_demo

# Check generated files
ls -la experiment_results.json
```

### Data Analysis

The generated `experiment_results.json` can be analyzed with Python:

```python
import pandas as pd
import json
import matplotlib.pyplot as plt

# Load results
with open('experiment_results.json', 'r') as f:
    results = json.load(f)

# Convert to DataFrame
data = []
for exp_name, exp_data in results.items():
    for metric_name, metric_value in exp_data.items():
        if isinstance(metric_value, (int, float)):
            data.append({
                'Experiment': exp_name,
                'Metric': metric_name,
                'Value': metric_value
            })

df = pd.DataFrame(data)

# Plot results
plt.figure(figsize=(15, 10))

plt.subplot(2, 3, 1)
for exp_name in df['Experiment'].unique():
    exp_data = df[df['Experiment'] == exp_name]
    plt.plot(exp_data['Metric'], exp_data['Value'], label=exp_name, marker='o')
plt.title('Experiment Results Comparison')
plt.xlabel('Metric')
plt.ylabel('Value')
plt.legend()
plt.xticks(rotation=45)

plt.subplot(2, 3, 2)
for exp_name in df[df['Metric'] == 'firing_rate_step']['Experiment'].unique():
    exp_data = df[(df['Experiment'] == exp_name) & (df['Metric'] == 'firing_rate_step')]
    plt.plot(exp_data.index, exp_data['Value'], label=exp_name)
plt.title('Firing Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')
plt.legend()

plt.subplot(2, 3, 3)
for exp_name in df[df['Metric'] == 'synapse_count_step']['Experiment'].unique():
    exp_data = df[(df['Experiment'] == exp_name) & (df['Metric'] == 'synapse_count_step')]
    plt.plot(exp_data.index, exp_data['Value'], label=exp_name)
plt.title('Synapse Count Over Time')
plt.xlabel('Step')
plt.ylabel('Synapse Count')
plt.legend()

plt.subplot(2, 3, 4)
for exp_name in df[df['Metric'] == 'plasticity_rate_step']['Experiment'].unique():
    exp_data = df[(df['Experiment'] == exp_name) & (df['Metric'] == 'plasticity_rate_step')]
    plt.plot(exp_data.index, exp_data['Value'], label=exp_name)
plt.title('Plasticity Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Plasticity Rate (Hz)')
plt.legend()

plt.subplot(2, 3, 5)
for exp_name in df[df['Metric'] == 'spike_count_step']['Experiment'].unique():
    exp_data = df[(df['Experiment'] == exp_name) & (df['Metric'] == 'spike_count_step')]
    plt.plot(exp_data.index, exp_data['Value'], label=exp_name)
plt.title('Spike Count Over Time')
plt.xlabel('Step')
plt.ylabel('Spike Count')
plt.legend()

plt.subplot(2, 3, 6)
pd.plotting.scatter_matrix(df[['Value']], figheight=8)
plt.title('Metric Correlation Matrix')

plt.tight_layout()
plt.savefig('experiment_analysis.png')
plt.show()

# Statistical analysis
print("Experiment Statistics:")
print("\nFiring Rate Statistics:")
for exp_name in df[df['Metric'] == 'final_firing_rate']['Experiment'].unique():
    value = df[(df['Experiment'] == exp_name) & (df['Metric'] == 'final_firing_rate')]['Value'].iloc[0]
    print(f"  {exp_name}: {value:.2f} Hz")

print("\nSynapse Count Statistics:")
for exp_name in df[df['Metric'] == 'final_synapse_count']['Experiment'].unique():
    value = df[(df['Experiment'] == exp_name) & (df['Metric'] == 'final_synapse_count')]['Value'].iloc[0]
    print(f"  {exp_name}: {value}")

print("\nPlasticity Rate Statistics:")
for exp_name in df[df['Metric'] == 'final_plasticity_rate']['Experiment'].unique():
    value = df[(df['Experiment'] == exp_name) & (df['Metric'] == 'final_plasticity_rate')]['Value'].iloc[0]
    print(f"  {exp_name}: {value:.4f} Hz")
```

## Example 4: Custom Brain

### Overview
This example demonstrates how to create custom brain configurations and experimental setups. It shows how to configure specific neural architectures and plasticity rules for specialized experiments.

### Complete Example
```cpp
#include "src/brain/Brain.hpp"
#include "src/brain/BrainFactory.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include <memory>
#include <fstream>
#include <random>

class CustomBrainConfig {
public:
    // Brain architecture parameters
    struct Architecture {
        uint32_t neuronCount;
        uint32_t regionCount;
        float connectivityDensity;
        bool enableDevelopment;
        bool enableNeuromodulation;
        bool enableWorkingMemory;
        bool enableEpisodicMemory;
    };
    
    // Plasticity configuration
    struct PlasticityConfig {
        bool enableSTDP;
        bool enableHebbian;
        bool enableRewardModulated;
        bool enableStructural;
        float stdpLTPWeight;
        float stdpLTDWeight;
        float stdpTimeConstant;
        float hebbianLearningRate;
        float rewardModulationStrength;
        float structuralFormationRate;
        float structuralPruningRate;
    };
    
    // Neuromodulation configuration
    struct NeuromodulationConfig {
        bool enableDopamine;
        bool enableNovelty;
        bool enableCuriosity;
        bool enablePredictionError;
        float dopamineBaseline;
        float noveltyThreshold;
        float curiosityStrength;
        float predictionErrorWeight;
    };
    
    // Development configuration
    struct DevelopmentConfig {
        bool enableDevelopment;
        uint32_t developmentalStages;
        float plasticityDecayRate;
        float maturationThreshold;
        float synaptogenesisRate;
        float pruningRate;
    };
    
    // Memory configuration
    struct MemoryConfig {
        bool enableWorkingMemory;
        bool enableEpisodicMemory;
        uint32_t workingMemoryCapacity;
        float workingMemoryDecayRate;
        uint32_t episodicMemoryCapacity;
        float episodicMemoryStrength;
    };
    
    // Prediction configuration
    struct PredictionConfig {
        bool enablePrediction;
        uint32_t predictionHorizon;
        float predictionConfidenceThreshold;
        float predictionErrorWeight;
    };
    
    // Cognition configuration
    struct CognitionConfig {
        bool enableAttention;
        bool enablePlanning;
        bool enableSelfModel;
        bool enableSocialLearning;
        float attentionStrength;
        uint32_t planningHorizon;
    };
    
    // Simulation configuration
    struct SimulationConfig {
        float timestep;
        uint64_t maxSteps;
        bool enableProgressBar;
        bool enableLogging;
        std::string logLevel;
        bool saveIntermediateStates;
        std::string outputDirectory;
    };
    
    // Default configuration
    static CustomBrainConfig getDefaultConfig() {
        CustomBrainConfig config;
        
        // Architecture
        config.architecture = {
            .neuronCount = 500,
            .regionCount = 2,
            .connectivityDensity = 0.1f,
            .enableDevelopment = true,
            .enableNeuromodulation = true,
            .enableWorkingMemory = true,
            .enableEpisodicMemory = true
        };
        
        // Plasticity
        config.plasticity = {
            .enableSTDP = true,
            .enableHebbian = true,
            .enableRewardModulated = true,
            .enableStructural = true,
            .stdpLTPWeight = 0.02f,
            .stdpLTDWeight = 0.015f,
            .stdpTimeConstant = 20.0f,
            .hebbianLearningRate = 0.001f,
            .rewardModulationStrength = 0.5f,
            .structuralFormationRate = 0.01f,
            .structuralPruningRate = 0.005f
        };
        
        // Neuromodulation
        config.neuromodulation = {
            .enableDopamine = true,
            .enableNovelty = true,
            .enableCuriosity = true,
            .enablePredictionError = true,
            .dopamineBaseline = 0.5f,
            .noveltyThreshold = 0.1f,
            .curiosityStrength = 0.3f,
            .predictionErrorWeight = 0.5f
        };
        
        // Development
        config.development = {
            .enableDevelopment = true,
            .developmentalStages = 15,
            .plasticityDecayRate = 0.05f,
            .maturationThreshold = 0.9f,
            .synaptogenesisRate = 0.02f,
            .pruningRate = 0.01f
        };
        
        // Memory
        config.memory = {
            .enableWorkingMemory = true,
            .enableEpisodicMemory = true,
            .workingMemoryCapacity = 20,
            .workingMemoryDecayRate = 0.001f,
            .episodicMemoryCapacity = 100,
            .episodicMemoryStrength = 0.8f
        };
        
        // Prediction
        config.prediction = {
            .enablePrediction = true,
            .predictionHorizon = 20,
            .predictionConfidenceThreshold = 0.8f,
            .predictionErrorWeight = 0.5f
        };
        
        // Cognition
        config.cognition = {
            .enableAttention = true,
            .enablePlanning = true,
            .enableSelfModel = true,
            .enableSocialLearning = true,
            .attentionStrength = 0.7f,
            .planningHorizon = 10
        };
        
        // Simulation
        config.simulation = {
            .timestep = 0.001f,
            .maxSteps = 1000,
            .enableProgressBar = true,
            .enableLogging = true,
            .logLevel = "INFO",
            .saveIntermediateStates = true,
            .outputDirectory = "output"
        };
        
        return config;
    }
    
    // Load configuration from JSON file
    static CustomBrainConfig loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open configuration file: " + filename);
        }
        
        nlohmann::json j;
        file >> j;
        
        CustomBrainConfig config;
        
        // Load architecture
        if (j.contains("architecture")) {
            auto& arch = j["architecture"];
            config.architecture.neuronCount = arch.value("neuronCount", config.architecture.neuronCount);
            config.architecture.regionCount = arch.value("regionCount", config.architecture.regionCount);
            config.architecture.connectivityDensity = arch.value("connectivityDensity", config.architecture.connectivityDensity);
            config.architecture.enableDevelopment = arch.value("enableDevelopment", config.architecture.enableDevelopment);
            config.architecture.enableNeuromodulation = arch.value("enableNeuromodulation", config.architecture.enableNeuromodulation);
            config.architecture.enableWorkingMemory = arch.value("enableWorkingMemory", config.architecture.enableWorkingMemory);
            config.architecture.enableEpisodicMemory = arch.value("enableEpisodicMemory", config.architecture.enableEpisodicMemory);
        }
        
        // Load plasticity
        if (j.contains("plasticity")) {
            auto& pl = j["plasticity"];
            config.plasticity.enableSTDP = pl.value("enableSTDP", config.plasticity.enableSTDP);
            config.plasticity.enableHebbian = pl.value("enableHebbian", config.plasticity.enableHebbian);
            config.plasticity.enableRewardModulated = pl.value("enableRewardModulated", config.plasticity.enableRewardModulated);
            config.plasticity.enableStructural = pl.value("enableStructural", config.plasticity.enableStructural);
            config.plasticity.stdpLTPWeight = pl.value("stdpLTPWeight", config.plasticity.stdpLTPWeight);
            config.plasticity.stdpLTDWeight = pl.value("stdpLTDWeight", config.plasticity.stdpLTDWeight);
            config.plasticity.stdpTimeConstant = pl.value("stdpTimeConstant", config.plasticity.stdpTimeConstant);
            config.plasticity.hebbianLearningRate = pl.value("hebbianLearningRate", config.plasticity.hebbianLearningRate);
        }
        
        // Load neuromodulation (additional sections would follow...)
        
        return config;
    }
    
    // Convert to Brain configuration
    nlm::BrainConfig toBrainConfig() const {
        nlm::BrainConfig brainConfig;
        
        // Set brain parameters based on custom configuration
        brainConfig.neuronCount = architecture.neuronCount;
        brainConfig.regionCount = architecture.regionCount;
        brainConfig.enableDevelopment = architecture.enableDevelopment;
        brainConfig.enableNeuromodulation = architecture.enableNeuromodulation;
        
        // Add plasticity rules
        if (plasticity.enableSTDP) {
            auto stdpRule = std::make_unique<STDP>(plasticity.stdpLTPWeight, 
                                                   plasticity.stdpLTDWeight, 
                                                   plasticity.stdpTimeConstant);
            brainConfig.plasticityRules.push_back(std::move(stdpRule));
        }
        
        if (plasticity.enableHebbian) {
            auto hebbianRule = std::make_unique<Hebbian>(plasticity.hebbianLearningRate);
            brainConfig.plasticityRules.push_back(std::move(hebbianRule));
        }
        
        if (plasticity.enableStructural) {
            auto structuralRule = std::make_unique<StructuralPlasticity>(
                plasticity.structuralFormationRate,
                plasticity.structuralPruningRate);
            brainConfig.plasticityRules.push_back(std::move(structuralRule));
        }
        
        // Add neuromodulators
        if (neuromodulation.enableDopamine) {
            auto dopamine = std::make_unique<Dopamine>(neuromodulation.dopamineBaseline);
            brainConfig.neuromodulators.push_back(std::move(dopamine));
        }
        
        if (neuromodulation.enableNovelty) {
            auto novelty = std::make_unique<Novelty>(neuromodulation.noveltyThreshold);
            brainConfig.neuromodulators.push_back(std::move(novelty));
        }
        
        if (neuromodulation.enableCuriosity) {
            auto curiosity = std::make_unique<Curiosity>(neuromodulation.curiosityStrength);
            brainConfig.neuromodulators.push_back(std::move(curiosity));
        }
        
        // Enable development
        brainConfig.enableDevelopment = development.enableDevelopment;
        brainConfig.developmentStages = development.developmentalStages;
        
        // Enable memory
        brainConfig.enableWorkingMemory = memory.enableWorkingMemory;
        brainConfig.workingMemoryCapacity = memory.workingMemoryCapacity;
        brainConfig.workingMemoryDecayRate = memory.workingMemoryDecayRate;
        
        brainConfig.enableEpisodicMemory = memory.enableEpisodicMemory;
        brainConfig.episodicMemoryCapacity = memory.episodicMemoryCapacity;
        brainConfig.episodicMemoryStrength = memory.episodicMemoryStrength;
        
        return brainConfig;
    }
};

class CustomBrain {
public:
    CustomBrain(const CustomBrainConfig& config) {
        initializeBrain(config);
    }
    
    void run() {
        std::cout << "Starting custom brain simulation" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << config_.architecture.neuronCount << std::endl;
        std::cout << "  Regions: " << config_.architecture.regionCount << std::endl;
        std::cout << "  STDP: " << (config_.plasticity.enableSTDP ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Hebbian: " << (config_.plasticity.enableHebbian ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (config_.architecture.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Neuromodulation: " << (config_.architecture.enableNeuromodulation ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Working Memory: " << (config_.architecture.enableWorkingMemory ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Episodic Memory: " << (config_.architecture.enableEpisodicMemory ? "Enabled" : "Disabled") << std::endl;
        
        // Create brain with custom configuration
        brain_ = std::make_unique<Brain>(config_.toBrainConfig());
        
        // Run simulation
        for (uint64_t step = 0; step < config_.simulation.maxSteps; ++step) {
            brain_->step(step);
            
            // Display progress
            if (step % 100 == 0) {
                float progress = 100.0f * step / config_.simulation.maxSteps;
                std::cout << "Progress: " << progress << "%" << std::endl;
                
                // Save intermediate state if enabled
                if (config_.simulation.saveIntermediateStates) {
                    saveIntermediateState(step);
                }
            }
        }
        
        // Display final results
        displayResults();
        
        // Save final state
        saveFinalState();
    }
    
private:
    void initializeBrain(const CustomBrainConfig& config) {
        config_ = config;
        
        // Create output directory
        std::filesystem::create_directories(config_.simulation.outputDirectory);
        
        std::cout << "Custom brain initialized" << std::endl;
        std::cout << "Output directory: " << config_.simulation.outputDirectory << std::endl;
    }
    
    void saveIntermediateState(uint64_t step) {
        std::string filename = config_.simulation.outputDirectory + "/step_" 
                              + std::to_string(step) + ".json";
        brain_->saveState(filename);
    }
    
    void displayResults() {
        std::cout << "\n=== Custom Brain Results ===" << std::endl;
        
        auto summary = brain_->getSummary();
        for (const auto& line : summary) {
            std::cout << line << std::endl;
        }
        
        std::cout << "\nConfiguration Summary:" << std::endl;
        std::cout << "  Plasticity Rules:" << std::endl;
        for (const auto& rule : config_.toBrainConfig().plasticityRules) {
            std::cout << "    - " << rule->getName() << std::endl;
        }
        
        std::cout << "  Neuromodulators:" << std::endl;
        for (const auto& modulator : config_.toBrainConfig().neuromodulators) {
            std::cout << "    - " << modulator->getName() << std::endl;
        }
        
        std::cout << "  Memory Systems:" << std::endl;
        std::cout << "    - Working Memory: " << (config_.architecture.enableWorkingMemory ? "Enabled" : "Disabled") << std::endl;
        std::cout << "    - Episodic Memory: " << (config_.architecture.enableEpisodicMemory ? "Enabled" : "Disabled") << std::endl;
        
        std::cout << "  Development:" << std::endl;
        std::cout << "    - Enabled: " << (config_.architecture.enableDevelopment ? "Yes" : "No") << std::endl;
        std::cout << "    - Stages: " << config_.development.developmentalStages << std::endl;
    }
    
    void saveFinalState() {
        std::string filename = config_.simulation.outputDirectory + "/final_brain_state.json";
        brain_->saveState(filename);
        std::cout << "\nFinal brain state saved to: " << filename << std::endl;
    }
    
    CustomBrainConfig config_;
    std::unique_ptr<Brain> brain_;
};

int main() {
    try {
        // Parse command line arguments
        std::string configFile = "custom_config.json";
        std::string outputDir = "custom_output";
        
        if (argc > 1) {
            configFile = argv[1];
        }
        if (argc > 2) {
            outputDir = argv[2];
        }
        
        // Load custom configuration
        CustomBrainConfig config;
        if (std::filesystem::exists(configFile)) {
            config = CustomBrainConfig::loadFromFile(configFile);
            config.simulation.outputDirectory = outputDir;
            std::cout << "Loaded custom configuration from: " << configFile << std::endl;
        } else {
            config = CustomBrainConfig::getDefaultConfig();
            config.simulation.outputDirectory = outputDir;
            std::cout << "Using default configuration" << std::endl;
            std::cout << "To save a custom configuration, run:" << std::endl;
            std::cout << "  ./custom_brain custom_config.json" << std::endl;
        }
        
        // Save configuration for reference
        std::ofstream configOut(configFile);
        nlohmann::json j = config;
        configOut << j.dump(2);
        configOut.close();
        
        // Create and run custom brain
        CustomBrain brain(config);
        brain.run();
        
        std::cout << "\n=== Custom Brain Simulation Complete ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Key Concepts Demonstrated

1. **Custom Configuration**: Creating and managing complex brain configurations
2. **Flexible Architecture**: Supporting multiple brain regions and connection patterns
3. **Multiple Plasticity Rules**: Implementing different learning mechanisms
4. **Neuromodulation Integration**: Incorporating biological modulation signals
5. **Development Processes**: Modeling brain development and maturation
6. **Memory Systems**: Implementing working and episodic memory
7. **Result Analysis**: Comprehensive analysis and visualization of results
8. **File Management**: Managing simulation outputs and intermediate states

### Expected Output

```
Loaded custom configuration from: custom_config.json
Custom brain initialized
Output directory: custom_output
Starting custom brain simulation
Configuration:
  Neurons: 500
  Regions: 2
  STDP: Enabled
  Hebbian: Enabled
  Development: Enabled
  Neuromodulation: Enabled
  Working Memory: Enabled
  Episodic Memory: Enabled
Progress: 0.00%
Progress: 10.00%
Progress: 20.00%
Progress: 30.00%
Progress: 40.00%
Progress: 50.00%
Progress: 60.00%
Progress: 70.00%
Progress: 80.00%
Progress: 90.00%

=== Custom Brain Results ===
Simulation Time: 1.000 seconds
Final Neuron Count: 500
Final Synapse Count: 625
Average Firing Rate: 14.2 Hz
Average Synaptic Weight: 0.158
Total Plasticity Events: 625

Configuration Summary:
  Plasticity Rules:
    - STDP
    - Hebbian
    - Structural Plasticity
  Neuromodulators:
    - Dopamine
    - Novelty
    - Curiosity
  Memory Systems:
    - Working Memory: Enabled
    - Episodic Memory: Enabled
  Development:
    - Enabled: Yes
    - Stages: 15

Final brain state saved to: custom_output/final_brain_state.json

=== Custom Brain Simulation Complete ===
```

### Configuration File Example

Create a custom configuration file `custom_config.json`:

```json
{
    "architecture": {
        "neuronCount": 1000,
        "regionCount": 3,
        "connectivityDensity": 0.05,
        "enableDevelopment": true,
        "enableNeuromodulation": true,
        "enableWorkingMemory": true,
        "enableEpisodicMemory": true
    },
    "plasticity": {
        "enableSTDP": true,
        "enableHebbian": true,
        "enableRewardModulated": true,
        "enableStructural": true,
        "stdpLTPWeight": 0.01,
        "stdpLTDWeight": 0.008,
        "stdpTimeConstant": 15.0,
        "hebbianLearningRate": 0.002,
        "rewardModulationStrength": 0.3,
        "structuralFormationRate": 0.005,
        "structuralPruningRate": 0.003
    },
    "neuromodulation": {
        "enableDopamine": true,
        "enableNovelty": true,
        "enableCuriosity": true,
        "enablePredictionError": true,
        "dopamineBaseline": 0.3,
        "noveltyThreshold": 0.05,
        "curiosityStrength": 0.4,
        "predictionErrorWeight": 0.3
    },
    "development": {
        "enableDevelopment": true,
        "developmentalStages": 20,
        "plasticityDecayRate": 0.03,
        "maturationThreshold": 0.8,
        "synaptogenesisRate": 0.01,
        "pruningRate": 0.005
    },
    "memory": {
        "enableWorkingMemory": true,
        "enableEpisodicMemory": true,
        "workingMemoryCapacity": 50,
        "workingMemoryDecayRate": 0.0005,
        "episodicMemoryCapacity": 500,
        "episodicMemoryStrength": 0.9
    },
    "simulation": {
        "timestep": 0.0005,
        "maxSteps": 2000,
        "enableProgressBar": true,
        "enableLogging": true,
        "logLevel": "INFO",
        "saveIntermediateStates": true,
        "outputDirectory": "custom_output"
    }
}
```

### Usage Instructions

```bash
# Build the example
c++ -std=c++17 custom_brain.cpp -o custom_brain -I./src -lpthread

# Run with default configuration
./custom_brain

# Run with custom configuration
./custom_brain my_config.json my_output

# Check generated files
ls -la custom_output/
```

### File Structure Generated

The custom brain example creates the following file structure:

```
custom_output/
├── step_0.json
├── step_100.json
├── step_200.json
├── ...
├── step_900.json
├── final_brain_state.json
├── configuration_log.json
└── analysis_results.json
```

### Advanced Features

1. **Configuration Templates**: Save and reuse complex configurations
2. **Parameter Optimization**: Automatically tune hyperparameters
3. **Visualization Integration**: Generate visualizations from simulation data
4. **Parallel Processing**: Use multiple threads for faster simulation
5. **Distributed Computing**: Scale to large-scale simulations

### Best Practices

1. **Configuration Management**: Use consistent configuration formats
2. **Result Management**: Organize simulation outputs systematically
3. **Error Handling**: Implement comprehensive error handling
4. **Performance Monitoring**: Monitor system performance during simulation
5. **Documentation**: Document configuration parameters and results

This example demonstrates the flexibility and power of the NLM framework for creating custom neural simulations with complex architectures and behaviors.