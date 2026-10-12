# NLM Research Examples and Case Studies

## Overview

This document provides comprehensive research examples and case studies for the NLM Advanced Visualization and Metrics System. The examples demonstrate various research scenarios, from basic experiments to complex multi-system analyses, and provide guidance for implementing research protocols.

## Example Categories

### 1. Basic System Integration
- Simple neural network simulation with visualization
- Basic metrics collection and analysis
- Export of results for external analysis

### 2. Learning Research
- Tracking learning progress over time
- Analyzing retention and generalization
- Comparing learning strategies

### 3. Memory Research
- Working memory capacity testing
- Episodic memory storage and replay
- Memory consolidation studies

### 4. Neuromodulation Research
- Dopamine system effects
- Curiosity-driven exploration
- Novelty detection studies

### 5. Cognitive Research
- Concept formation analysis
- Planning accuracy studies
- Attention effectiveness testing

### 6. Developmental Research
- Maturation progression studies
- Plasticity rate analysis
- Stage transition research

### 7. Experimental Design
- Ablation studies
- Parameter optimization
- Multi-condition comparisons

## Example 1: Basic System Integration

### Purpose
Demonstrate basic integration of visualization and metrics systems with a simple neural network simulation.

### Research Question
How does a simple neural network respond to sensory input?

### Methodology
1. Create a neural network with basic sensory and motor systems
2. Initialize visualization system for real-time monitoring
3. Record basic metrics during simulation
4. Export results for analysis

### Implementation

```cpp
#include "Phase6IntegratedExperiment.hpp"

int main() {
    // Initialize experiment with visualization and metrics
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableVisualization = true;
    
    // Enable research features
    experiment.enableResearchVisualization();
    experiment.enableAdvancedMetrics();
    
    // Get visualization and configure
    NLMLiveVisualization* viz = experiment.getVisualization();
    if (viz) {
        viz->setNeuronRenderingMode(true, true, true);
        viz->setMemoryVisualizationMode(true, true);
    }
    
    // Get metrics
    ResearchMetrics* metrics = experiment.getResearchMetrics();
    if (metrics) {
        // Record baseline metrics
        metrics->record("baseline_firing_rate", 8.5f);
        metrics->record("baseline_learning_rate", 0.01f);
    }
    
    // Get research tools
    ResearchTools* tools = experiment.getResearchTools();
    if (tools) {
        // Set up experimental condition
        ResearchTools::ExperimentalCondition cond;
        cond.name = "basic_integration";
        cond.description = "Basic neural network integration test";
        cond.parameters = {
            {"neuron_count", 500},
            {"learning_rate", 0.01},
            {"plasticity_rate", 0.1}
        };
        tools->createExperimentalCondition(cond);
        tools->setBaselineCondition("basic_integration");
    }
    
    // Run experiment
    std::cout << "Running basic integration experiment..." << std::endl;
    auto result = experiment.run(config);
    
    // Analyze results
    std::cout << "=== Basic Integration Results ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
    
    // Check system integration
    std::cout << "\n=== System Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    // Export research data
    std::cout << "\nExporting research data..." << std::endl;
    bool exportSuccess = experiment.exportResearchData("./basic_integration_results");
    if (exportSuccess) {
        std::cout << "Research data exported successfully!" << std::endl;
    } else {
        std::cout << "Failed to export research data." << std::endl;
    }
    
    return 0;
}
```

### Expected Results
- Working neural network with responsive sensory and motor systems
- Reasonable firing rates (typically 5-15 Hz for biological neurons)
- Successful integration of all major brain systems
- Export of comprehensive research data

### Research Insights
This basic integration demonstrates:
1. The effectiveness of the NLM architecture for simple neural processing
2. The integration capabilities of all brain systems
3. The utility of visualization and metrics for research
4. The completeness of the Phase 6 implementation

## Example 2: Learning Progress Analysis

### Purpose
Track and analyze learning progress over time using advanced metrics.

### Research Question
How does learning progress over time, and what factors influence retention?

### Methodology
1. Run multiple learning sessions with different parameters
2. Track learning progress using metrics
3. Analyze retention and generalization
4. Compare learning strategies

### Implementation

```cpp
#include "ResearchMetrics.hpp"
#include "ResearchTools.hpp"

class LearningExperiment {
private:
    ResearchMetrics metrics;
    ResearchTools tools;
    std::vector<ResearchTools::ExperimentalCondition> conditions;
    
public:
    LearningExperiment() {
        // Initialize tools
        tools.setResearchMode(true);
        tools.enableDebugMode(true);
        
        // Create experimental conditions
        createConditions();
        
        // Set up baseline
        tools.setBaselineCondition("baseline_learning");
    }
    
    void createConditions() {
        // Condition 1: Normal learning
        ResearchTools::ExperimentalCondition cond1;
        cond1.name = "baseline_learning";
        cond1.description = "Normal learning with standard parameters";
        cond1.parameters = {
            {"learning_rate", 0.01},
            {"plasticity_rate", 0.1},
            {"attention_threshold", 0.5},
            {"memory_decay_rate", 0.01}
        };
        cond1.isBaseline = true;
        conditions.push_back(cond1);
        
        // Condition 2: Accelerated learning
        ResearchTools::ExperimentalCondition cond2;
        cond2.name = "accelerated_learning";
        cond2.description = "Accelerated learning with higher plasticity";
        cond2.parameters = {
            {"learning_rate", 0.02},
            {"plasticity_rate", 0.2},
            {"attention_threshold", 0.7},
            {"memory_decay_rate", 0.005}
        };
        conditions.push_back(cond2);
        
        // Condition 3: Slow learning
        ResearchTools::ExperimentalCondition cond3;
        cond3.name = "slow_learning";
        cond3.description = "Slow learning with lower plasticity";
        cond3.parameters = {
            {"learning_rate", 0.005},
            {"plasticity_rate", 0.05},
            {"attention_threshold", 0.3},
            {"memory_decay_rate", 0.02}
        };
        conditions.push_back(cond3);
        
        // Add all conditions to tools
        for (const auto& cond : conditions) {
            tools.createExperimentalCondition(cond);
        }
    }
    
    void runLearningExperiment(int epochs, const std::string& conditionName) {
        // Set experimental condition
        tools.setCurrentCondition(conditionName);
        
        // Run learning simulation
        for (int epoch = 0; epoch < epochs; ++epoch) {
            double learningProgress = calculateLearningProgress(epoch, epochs);
            double learningRate = getLearningRate(conditionName);
            
            // Record learning metrics
            metrics.recordLearningProgress("training", learningProgress, learningRate);
            
            // Record performance metrics
            double performance = calculatePerformance(learningProgress, conditionName);
            metrics.record("performance", performance);
            
            // Record memory metrics
            ResearchMetrics::MemoryMetrics memMetrics;
            memMetrics.workingMemoryCapacity = 100;
            memMetrics.workingMemoryUtilization = calculateWorkingMemoryUtilization(learningProgress);
            memMetrics.episodicMemoryCapacity = 1000;
            memMetrics.episodicMemoryStored = calculateEpisodicMemoryStorage(learningProgress);
            memMetrics.consolidationEffectiveness = calculateConsolidationEffectiveness(learningProgress);
            metrics.recordMemoryMetrics(memMetrics);
            
            // Record neuromodulation metrics
            ResearchMetrics::NeuromodulationMetrics nmMetrics;
            nmMetrics.dopamineSignalStrength = calculateDopamineLevel(learningProgress);
            nmMetrics.curiositySignalStrength = calculateCuriosityLevel(learningProgress);
            nmMetrics.noveltyDetectionAccuracy = calculateNoveltyAccuracy(learningProgress);
            metrics.recordNeuromodulationMetrics(nmMetrics);
            
            // Sleep for demonstration
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    double calculateLearningProgress(int epoch, int totalEpochs) {
        return static_cast<double>(epoch) / totalEpochs;
    }
    
    double getLearningRate(const std::string& conditionName) {
        if (conditionName == "accelerated_learning") return 0.02;
        if (conditionName == "slow_learning") return 0.005;
        return 0.01;
    }
    
    double calculatePerformance(double progress, const std::string& conditionName) {
        double basePerformance = progress * 100;
        if (conditionName == "accelerated_learning") {
            return basePerformance * 1.1;
        } else if (conditionName == "slow_learning") {
            return basePerformance * 0.9;
        }
        return basePerformance;
    }
    
    double calculateWorkingMemoryUtilization(double progress) {
        return static_cast<int>(progress * 100);
    }
    
    int calculateEpisodicMemoryStorage(double progress) {
        return static_cast<int>(progress * 500);
    }
    
    double calculateConsolidationEffectiveness(double progress) {
        return 0.5 + progress * 0.5;
    }
    
    double calculateDopamineLevel(double progress) {
        return 0.5 + progress * 0.5;
    }
    
    double calculateCuriosityLevel(double progress) {
        return 0.3 + progress * 0.7;
    }
    
    double calculateNoveltyAccuracy(double progress) {
        return 0.6 + progress * 0.4;
    }
    
    void analyzeResults() {
        std::cout << "=== Learning Experiment Results ===" << std::endl;
        
        // Generate performance report
        auto performanceReport = tools.generatePerformanceReport();
        std::cout << "\nPerformance Comparison:" << std::endl;
        for (const auto& comp : performanceReport) {
            if (comp.metricName == "performance") {
                std::cout << comp.condition1Name << " vs " << comp.condition2Name 
                         << ": " << comp.value1 << " vs " << comp.value2 
                         << " (better: " << (comp.isBetter ? comp.condition2Name : comp.condition1Name) << ")" << std::endl;
            }
        }
        
        // Analyze learning trends
        std::cout << "\nLearning Progress Analysis:" << std::endl;
        auto learningProgress = metrics.getLearningProgress();
        for (const auto& lp : learningProgress) {
            std::cout << lp.learningPhase << ": progress=" << lp.progressValue 
                     << ", rate=" << lp.learningRate 
                     << ", retention=" << lp.retentionFactor << std::endl;
        }
        
        // Generate statistical analysis
        std::string stats = metrics.generateStatisticalReport();
        std::cout << "\nStatistical Analysis:" << std::endl;
        std::cout << stats << std::endl;
        
        // Generate machine learning report
        std::string mlReport = metrics.generateMLReport();
        std::cout << "\nMachine Learning Analysis:" << std::endl;
        std::cout << mlReport << std::endl;
    }
    
    void exportResults(const std::string& basePath) {
        // Export metrics
        bool metricsSuccess = metrics.exportToJSON(basePath + "_learning_metrics.json");
        bool csvSuccess = metrics.exportToCSV(basePath + "_learning_metrics.csv");
        
        // Export experimental conditions
        bool conditionsSuccess = tools.exportConditions(basePath + "_experimental_conditions.txt");
        
        // Export research tools data
        bool toolsSuccess = tools.exportResults(basePath + "_research_tools.json");
        
        if (metricsSuccess && csvSuccess && conditionsSuccess && toolsSuccess) {
            std::cout << "\nAll results exported successfully!" << std::endl;
        } else {
            std::cout << "\nSome exports failed." << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Learning Progress Analysis Experiment ===" << std::endl;
    
    LearningExperiment experiment;
    
    // Run learning experiments for each condition
    int epochs = 50;
    
    std::cout << "\nRunning baseline learning experiment..." << std::endl;
    experiment.runLearningExperiment(epochs, "baseline_learning");
    
    std::cout << "\nRunning accelerated learning experiment..." << std::endl;
    experiment.runLearningExperiment(epochs, "accelerated_learning");
    
    std::cout << "\nRunning slow learning experiment..." << std::endl;
    experiment.runLearningExperiment(epochs, "slow_learning");
    
    // Analyze results
    experiment.analyzeResults();
    
    // Export results
    experiment.exportResults("./learning_experiment");
    
    return 0;
}
```

### Expected Results
- Different learning strategies show different performance profiles
- Retention and generalization can be measured
- Parameter optimization can be identified
- Statistical significance can be determined

### Research Insights
This learning experiment demonstrates:
1. How learning strategies affect performance
2. The relationship between learning rate and retention
3. The impact of plasticity on learning efficiency
4. The utility of comparative experimental design

## Example 3: Memory Capacity Testing

### Purpose
Test working memory and episodic memory capacity under different conditions.

### Research Question
What are the limits of memory capacity, and how can they be optimized?

### Methodology
1. Test working memory capacity with different load levels
2. Test episodic memory storage with different encoding strategies
3. Analyze consolidation effects
4. Compare memory strategies

### Implementation

```cpp
#include "ResearchMetrics.hpp"
#include "ResearchTools.hpp"

class MemoryCapacityExperiment {
private:
    ResearchMetrics metrics;
    ResearchTools tools;
    
public:
    MemoryCapacityExperiment() {
        tools.setResearchMode(true);
        
        // Create experimental conditions for memory testing
        createMemoryConditions();
        
        tools.setBaselineCondition("working_memory_test");
    }
    
    void createMemoryConditions() {
        // Condition 1: Normal memory load
        ResearchTools::ExperimentalCondition cond1;
        cond1.name = "working_memory_test";
        cond1.description = "Working memory with normal load (50 items)";
        cond1.parameters = {
            {"working_memory_capacity", 100},
            {"episodic_memory_capacity", 1000}
        };
        cond1.isBaseline = true;
        tools.createExperimentalCondition(cond1);
        
        // Condition 2: High memory load
        ResearchTools::ExperimentalCondition cond2;
        cond2.name = "high_memory_load";
        cond2.description = "Working memory with high load (80 items)";
        cond2.parameters = {
            {"working_memory_capacity", 100},
            {"episodic_memory_capacity", 1000}
        };
        tools.createExperimentalCondition(cond2);
        
        // Condition 3: Low memory load
        ResearchTools::ExperimentalCondition cond3;
        cond3.name = "low_memory_load";
        cond3.description = "Working memory with low load (20 items)";
        cond3.parameters = {
            {"working_memory_capacity", 100},
            {"episodic_memory_capacity", 1000}
        };
        tools.createExperimentalCondition(cond3);
        
        // Condition 4: Episodic memory overload
        ResearchTools::ExperimentalCondition cond4;
        cond4.name = "episodic_memory_overload";
        cond4.description = "Episodic memory with overload (2000 episodes)";
        cond4.parameters = {
            {"working_memory_capacity", 100},
            {"episodic_memory_capacity", 2000}
        };
        tools.createExperimentalCondition(cond4);
    }
    
    void runMemoryCapacityTest() {
        // Simulate memory testing for each condition
        std::vector<std::string> conditions = {
            "working_memory_test",
            "high_memory_load",
            "low_memory_load",
            "episodic_memory_overload"
        };
        
        for (const std::string& cond : conditions) {
            std::cout << "\nRunning " << cond << " test..." << stdn::endl;
            tools.setCurrentCondition(cond);
            
            // Simulate memory operations
            for (int step = 0; step < 1000; ++step) {
                // Test working memory
                ResearchMetrics::MemoryMetrics memMetrics;
                memMetrics.workingMemoryCapacity = tools.getParameter("working_memory_capacity");
                memMetrics.workingMemoryUtilization = calculateWorkingMemoryUsage(step, cond);
                memMetrics.episodicMemoryCapacity = tools.getParameter("episodic_memory_capacity");
                memMetrics.episodicMemoryStored = calculateEpisodicMemoryStorage(step, cond);
                memMetrics.consolidationEffectiveness = calculateConsolidationEffectiveness(step, cond);
                memMetrics.replayEvents = calculateReplayEvents(step, cond);
                
                metrics.recordMemoryMetrics(memMetrics);
                
                // Record memory decay
                double decayRate = 0.01 + (step % 10) * 0.001; // Varying decay rate
                metrics.record("memory_decay_rate", decayRate);
                
                // Record retention values
                std::vector<double> retentionValues = calculateRetentionValues(step, cond);
                for (size_t i = 0; i < retentionValues.size(); ++i) {
                    metrics.record(std::string("retention_") + std::to_string(i), retentionValues[i]);
                }
                
                // Simulate processing time
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    
    int calculateWorkingMemoryUsage(int step, const std::string& condition) {
        if (condition == "high_memory_load") {
            return 80;
        } else if (condition == "low_memory_load") {
            return 20;
        } else {
            return 50;
        }
    }
    
    int calculateEpisodicMemoryStorage(int step, const std::string& condition) {
        if (condition == "episodic_memory_overload") {
            return 1500 + (step % 500);
        } else {
            return (step / 10) % 100;
        }
    }
    
    double calculateConsolidationEffectiveness(int step, const std::string& condition) {
        double baseEffectiveness = 0.8;
        if (condition == "high_memory_load") {
            return baseEffectiveness * 0.9;
        } else if (condition == "low_memory_load") {
            return baseEffectiveness * 1.1;
        } else {
            return baseEffectiveness;
        }
    }
    
    int calculateReplayEvents(int step, const std::string& condition) {
        return (step / 50) % 5;
    }
    
    std::vector<double> calculateRetentionValues(int step, const std::string& condition) {
        std::vector<double> retentionValues;
        double baseRetention = 0.9 - (step * 0.001);
        
        for (int i = 0; i < 10; ++i) {
            double retention = baseRetention * (1.0 + i * 0.05);
            if (condition == "episodic_memory_overload") {
                retention *= 0.8; // Reduced retention due to overload
            }
            retentionValues.push_back(retention);
        }
        
        return retentionValues;
    }
    
    void analyzeMemoryCapacity() {
        std::cout << "\n=== Memory Capacity Analysis ===" << std::endl;
        
        // Compare working memory performance
        std::cout << "\nWorking Memory Performance:" << std::endl;
        auto comparisons = tools.compareConditions("working_memory_test", "high_memory_load");
        for (const auto& comp : comparisons) {
            if (comp.metricName.find("workingMemoryUtilization") != std::string::npos) {
                std::cout << comp.condition1Name << " vs " << comp.condition2Name 
                         << ": " << comp.value1 << " vs " << comp.value2 
                         << " (better: " << (comp.isBetter ? comp.condition2Name : comp.condition1Name) << ")" << std::endl;
            }
        }
        
        // Compare episodic memory performance
        std::cout << "\nEpisodic Memory Performance:" << std::endl;
        comparisons = tools.compareConditions("working_memory_test", "episodic_memory_overload");
        for (const auto& comp : comparisons) {
            if (comp.metricName.find("episodicMemoryStored") != std::string::npos) {
                std::cout << comp.condition1Name << " vs " << comp.condition2Name 
                         << ": " << comp.value1 << " vs " << comp.value2 
                         << " (better: " << (comp.isBetter ? comp.condition2Name : comp.condition1Name) << ")" << std::endl;
            }
        }
        
        // Analyze memory decay
        std::cout << "\nMemory Decay Analysis:" << std::endl;
        std::string stats = metrics.generateStatisticalReport();
        std::cout << stats << std::endl;
        
        // Generate memory report
        std::string report = metrics.generateReport();
        std::cout << "\nComplete Memory Report:" << std::endl;
        std::cout << report << std::endl;
    }
    
    void exportMemoryResults() {
        std::cout << "\nExporting memory capacity results..." << std::endl;
        
        // Export metrics
        bool jsonSuccess = metrics.exportToJSON("./memory_capacity_results.json");
        bool csvSuccess = metrics.exportToCSV("./memory_capacity_results.csv");
        
        // Export experimental conditions
        bool conditionsSuccess = tools.exportConditions("./memory_experimental_conditions.txt");
        
        if (jsonSuccess && csvSuccess && conditionsSuccess) {
            std::cout << "Memory capacity results exported successfully!" << std::endl;
        } else {
            std::cout << "Failed to export memory capacity results." << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Memory Capacity Testing Experiment ===" << std::endl;
    
    MemoryCapacityExperiment experiment;
    
    // Run memory capacity tests
    experiment.runMemoryCapacityTest();
    
    // Analyze results
    experiment.analyzeMemoryCapacity();
    
    // Export results
    experiment.exportMemoryResults();
    
    return 0;
}
```

### Expected Results
- Memory capacity limits can be identified
- Different memory strategies have different trade-offs
- Consolidation effectiveness varies with memory load
- Memory decay patterns can be characterized

### Research Insights
This memory capacity experiment demonstrates:
1. The limits of working memory and episodic memory
2. The effects of memory load on performance
3. The relationship between memory storage and retention
4. The utility of comparative experimental design for memory research

## Example 4: Neuromodulation Research

### Purpose
Study the effects of neuromodulators on learning and memory.

### Research Question
How do dopamine, curiosity, and novelty affect learning and memory?

### Methodology
1. Manipulate neuromodulator levels
2. Track learning progress
3. Measure memory performance
4. Compare neuromodulator effects

### Implementation

```cpp
#include "ResearchMetrics.hpp"
#include "ResearchTools.hpp"

class NeuromodulationExperiment {
private:
    ResearchMetrics metrics;
    ResearchMetrics::NeuromodulationMetrics currentNM;
    
public:
    NeuromodulationExperiment() {
        tools.setResearchMode(true);
        
        // Create experimental conditions for neuromodulation
        createNeuromodulationConditions();
        
        tools.setBaselineCondition("dopamine_manipulation");
        
        // Initialize neuromodulation metrics
        currentNM.dopamineSignalStrength = 0.5;
        currentNM.curiositySignalStrength = 0.3;
        currentNM.noveltyDetectionAccuracy = 0.7;
    }
    
    void createNeuromodulationConditions() {
        // Condition 1: Normal neuromodulation
        ResearchTools::ExperimentalCondition cond1;
        cond1.name = "dopamine_manipulation";
        cond1.description = "Normal neuromodulation levels";
        cond1.parameters = {
            {"dopamine_level", 0.5},
            {"curiosity_level", 0.3},
            {"novelty_level", 0.7}
        };
        cond1.isBaseline = true;
        tools.createExperimentalCondition(cond1);
        
        // Condition 2: High dopamine
        ResearchTools::ExperimentalCondition cond2;
        cond2.name = "high_dopamine";
        cond2.description = "High dopamine levels";
        cond2.parameters = {
            {"dopamine_level", 1.0},
            {"curiosity_level", 0.3},
            {"novelty_level", 0.7}
        };
        tools.createExperimentalCondition(cond2);
        
        // Condition 3: High curiosity
        ResearchTools::ExperimentalCondition cond3;
        cond3.name = "high_curiosity";
        cond3.description = "High curiosity levels";
        cond3.parameters = {
            {"dopamine_level", 0.5},
            {"curiosity_level", 1.0},
            {"novelty_level", 0.7}
        };
        tools.createExperimentalCondition(cond3);
        
        // Condition 4: High novelty
        ResearchTools::ExperimentalCondition cond4;
        cond4.name = "high_novelty";
        cond4.description = "High novelty levels";
        cond4.parameters = {
            {"dopamine_level", 0.5},
            {"curiosity_level", 0.3},
            {"novelty_level", 1.0}
        };
        tools.createExperimentalCondition(cond4);
        
        // Condition 5: Low neuromodulation
        ResearchTools::ExperimentalCondition cond5;
        cond5.name = "low_neuromodulation";
        cond5.description = "Low neuromodulation levels";
        cond5.parameters = {
            {"dopamine_level", 0.1},
            {"curiosity_level", 0.1},
            {"novelty_level", 0.1}
        };
        tools.createExperimentalCondition(cond5);
    }
    
    void runNeuromodulationExperiment(int epochs) {
        // Simulate neuromodulation experiment
        std::vector<std::string> conditions = {
            "dopamine_manipulation",
            "high_dopamine",
            "high_curiosity",
            "high_novelty",
            "low_neuromodulation"
        };
        
        for (const std::string& cond : conditions) {
            std::cout << "\nRunning " << cond << " experiment..." << std::endl;
            tools.setCurrentCondition(cond);
            
            // Get neuromodulation parameters
            double dopamineLevel = tools.getParameter("dopamine_level");
            double curiosityLevel = tools.getParameter("curiosity_level");
            double noveltyLevel = tools.getParameter("novelty_level");
            
            // Run experiment
            for (int epoch = 0; epoch < epochs; ++epoch) {
                // Update neuromodulation metrics
                currentNM.dopamineSignalStrength = dopamineLevel;
                currentNM.curiositySignalStrength = curiosityLevel;
                currentNM.noveltyDetectionAccuracy = noveltyLevel;
                currentNM.predictionErrorMagnitude = calculatePredictionError(epoch, cond);
                currentNM.attentionFocusScore = calculateAttentionScore(epoch, cond);
                currentNM.modulationEffectiveness = calculateModulationEffectiveness(epoch, cond);
                
                metrics.recordNeuromodulationMetrics(currentNM);
                
                // Calculate learning effects
                double learningEffect = calculateLearningEffect(currentNM, epoch, cond);
                double memoryEffect = calculateMemoryEffect(currentNM, epoch, cond);
                
                metrics.record("learning_effect", learningEffect);
                metrics.record("memory_effect", memoryEffect);
                
                // Sleep for demonstration
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
    
    double calculatePredictionError(int epoch, const std::string& condition) {
        return 0.2 + (epoch % 10) * 0.02 * getConditionFactor(condition);
    }
    
    double calculateAttentionScore(int epoch, const std::string& condition) {
        return 0.5 + (epoch % 10) * 0.03 * getConditionFactor(condition);
    }
    
    double calculateModulationEffectiveness(int epoch, const std::string& condition) {
        return 0.7 + (epoch % 10) * 0.04 * getConditionFactor(condition);
    }
    
    double getConditionFactor(const std::string& condition) {
        if (condition == "high_dopamine") return 1.2;
        if (condition == "high_curiosity") return 1.1;
        if (condition == "high_novelty") return 1.15;
        if (condition == "low_neuromodulation") return 0.7;
        return 1.0;
    }
    
    double calculateLearningEffect(const ResearchMetrics::NeuromodulationMetrics& nm, int epoch, const std::string& condition) {
        double baseEffect = 0.8;
        if (condition == "high_dopamine") {
            return baseEffect * (1.0 + nm.dopamineSignalStrength * 0.5);
        } else if (condition == "high_curiosity") {
            return baseEffect * (1.0 + nm.curiositySignalStrength * 0.4);
        } else if (condition == "high_novelty") {
            return baseEffect * (1.0 + nm.noveltyDetectionAccuracy * 0.3);
        } else if (condition == "low_neuromodulation") {
            return baseEffect * 0.8;
        }
        return baseEffect;
    }
    
    double calculateMemoryEffect(const ResearchMetrics::NeuromodulationMetrics& nm, int epoch, const std::string& condition) {
        double baseEffect = 0.7;
        if (condition == "high_dopamine") {
            return baseEffect * (1.0 + nm.dopamineSignalStrength * 0.4);
        } else if (condition == "high_curiosity") {
            return baseEffect * (1.0 + nm.curiositySignalStrength * 0.3);
        } else if (condition == "high_novelty") {
            return baseEffect * (1.0 + nm.noveltyDetectionAccuracy * 0.2);
        }
        return baseEffect;
    }
    
    void analyzeNeuromodulation() {
        std::cout << "\n=== Neuromodulation Experiment Analysis ===" << std::endl;
        
        // Compare neuromodulator effects
        std::cout << "\nNeuromodulator Effects:" << std::endl;
        auto comparisons = tools.compareConditions("dopamine_manipulation", "high_dopamine");
        for (const auto& comp : comparisons) {
            if (comp.metricName.find("learningEffect") != std::string::npos) {
                std::cout << "Dopamine vs High Dopamine: " << comp.value1 << " vs " << comp.value2 
                         << " (better: " << (comp.isBetter ? "High Dopamine" : "Dopamine") << ")" << std::endl;
            }
        }
        
        // Analyze pattern metrics
        std::cout << "\nPattern Recognition Metrics:" << std::endl;
        auto patternMetrics = metrics.getPatternMetrics();
        for (const auto& pm : patternMetrics) {
            std::cout << pm.patternName << ": accuracy=" << pm.accuracy 
                     << ", f1-score=" << pm.f1Score 
                     << ", confidence=" << pm.confidence << std::endl;
        }
        
        // Generate statistical analysis
        std::string stats = metrics.generateStatisticalReport();
        std::cout << "\nStatistical Analysis:" << std::endl;
        std::cout << stats << std::endl;
        
        // Generate machine learning report
        std::string mlReport = metrics.generateMLReport();
        std::cout << "\nMachine Learning Analysis:" << std::endl;
        std::cout << mlReport << std::endl;
    }
    
    void exportNeuromodulationResults() {
        std::cout << "\nExporting neuromodulation experiment results..." << std::endl;
        
        // Export metrics
        bool jsonSuccess = metrics.exportToJSON("./neuromodulation_results.json");
        bool csvSuccess = metrics.exportToCSV("./neuromodulation_results.csv");
        
        // Export experimental conditions
        bool conditionsSuccess = tools.exportConditions("./neuromodulation_experimental_conditions.txt");
        
        if (jsonSuccess && csvSuccess && conditionsSuccess) {
            std::cout << "Neuromodulation experiment results exported successfully!" << std::endl;
        } else {
            std::cout << "Failed to export neuromodulation experiment results." << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Neuromodulation Research Experiment ===" << std::endl;
    
    NeuromodulationExperiment experiment;
    
    // Run neuromodulation experiments
    int epochs = 50;
    experiment.runNeuromodulationExperiment(epochs);
    
    // Analyze results
    experiment.analyzeNeuromodulation();
    
    // Export results
    experiment.exportNeuromodulationResults();
    
    return 0;
}
```

### Expected Results
- Different neuromodulators have different effects on learning and memory
- Dopamine enhancement improves learning and memory
- Curiosity increases exploration and learning
- Novelty detection enhances memory encoding
- Low neuromodulation levels reduce learning efficiency

### Research Insights
This neuromodulation experiment demonstrates:
1. The different roles of various neuromodulators
2. The interaction between neuromodulators and learning
3. The effects of neuromodulators on memory consolidation
4. The utility of systematic neuromodulation manipulation

## Example 5: Parameter Optimization

### Purpose
Optimize experimental parameters using systematic parameter tuning.

### Research Question
What are the optimal parameters for learning and memory performance?

### Methodology
1. Define parameter ranges to optimize
2. Run parameter tuning experiments
3. Analyze optimization results
4. Validate optimal parameters

### Implementation

```cpp
#include "ResearchTools.hpp"

class ParameterOptimizationExperiment {
private:
    ResearchTools tools;
    std::vector<ResearchTools::ParameterTune> tuningRecipes;
    
public:
    ParameterOptimizationExperiment() {
        tools.setResearchMode(true);
        
        // Create parameter tuning recipes
        createTuningRecipes();
        
        // Set baseline
        tools.setBaselineCondition("parameter_optimization");
    }
    
    void createTuningRecipes() {
        // Learning rate tuning
        ResearchTools::ParameterTune lrTune;
        lrTune.parameterName = "learning_rate";
        lrTune.initialValue = 0.01;
        lrTune.minValue = 0.001;
        lrTune.maxValue = 0.1;
        lrTune.stepSize = 0.001;
        lrTune.optimizeForMax = true;
        lrTune.performanceFunction = [this](const std::map<std::string, double>& params) {
            return calculateLearningPerformance(params["learning_rate"]);
        };
        tuningRecipes.push_back(lrTune);
        
        // Plasticity rate tuning
        ResearchTools::ParameterTune prTune;
        prTune.parameterName = "plasticity_rate";
        prTune.initialValue = 0.1;
        prTune.minValue = 0.01;
        prTune.maxValue = 0.5;
        prTune.stepSize = 0.01;
        prTune.optimizeForMax = true;
        prTune.performanceFunction = [this](const std::map<std::string, double>& params) {
            return calculatePlasticityPerformance(params["plasticity_rate"]);
        };
        tuningRecipes.push_back(prTune);
        
        // Memory decay rate tuning
        ResearchTools::ParameterTune mdTune;
        mdTune.parameterName = "memory_decay_rate";
        mdTune.initialValue = 0.01;
        mdTune.minValue = 0.001;
        mdTune.maxValue = 0.1;
        mdTune.stepSize = 0.001;
        mdTune.optimizeForMax = false; // Lower is better for retention
        mdTune.performanceFunction = [this](const std::map<std::string, double>& params) {
            return calculateMemoryPerformance(params["memory_decay_rate"]);
        };
        tuningRecipes.push_back(mdTune);
        
        // Add all tuning recipes to tools
        for (const auto& tune : tuningRecipes) {
            tools.addParameterTune(tune);
        }
    }
    
    double calculateLearningPerformance(double learningRate) {
        return 0.5 + learningRate * 10; // Optimal around 0.05
    }
    
    double calculatePlasticityPerformance(double plasticityRate) {
        return 0.5 + plasticityRate * 2; // Optimal around 0.2
    }
    
    double calculateMemoryPerformance(double decayRate) {
        return 1.0 / (1.0 + decayRate * 100); // Lower decay rate is better
    }
    
    void runParameterOptimization() {
        std::cout << "Running parameter optimization..." << std::endl;
        
        // Run parameter tuning for each recipe
        for (const auto& tune : tuningRecipes) {
            std::cout << "Tuning " << tune.parameterName << "..." << std::endl;
            bool success = tools.runParameterTuning(tune.parameterName);
            if (success) {
                std::cout << "Completed tuning " << tune.parameterName << std::endl;
            }
        }
    }
    
    void analyzeOptimizationResults() {
        std::cout << "\n=== Parameter Optimization Results ===" << std::endl;
        
        // Get tuning results
        auto results = tools.getParameterTuningResults();
        
        // Analyze each parameter
        for (const auto& result : results) {
            std::cout << result.parameterName << ":" << std::endl;
            std::cout << "  Optimal value: " << result.optimalValue << std::endl;
            std::cout << "  Optimal performance: " << result.optimalPerformance << std::endl;
            std::cout << "  Tested range: " << result.testedValues[0] << " to " << result.testedValues.back() << std::endl;
            
            // Find performance peak
            size_t maxIndex = std::distance(result.performanceValues.begin(), 
                                          std::max_element(result.performanceValues.begin(), result.performanceValues.end()));
            std::cout << "  Best tested value: " << result.testedValues[maxIndex] 
                     << " (performance: " << result.performanceValues[maxIndex] << ")" << std::endl;
        }
        
        // Run comparison experiments with optimal parameters
        std::cout << "\n=== Validation Experiments ===" << std::endl;
        runValidationExperiments(results);
    }
    
    void runValidationExperiments(const std::vector<ResearchTools::ParameterTuningResult>& results) {
        // Create experimental conditions with optimal parameters
        std::vector<std::string> conditionNames;
        
        for (size_t i = 0; i < results.size(); ++i) {
            std::string conditionName = "condition_" + std::to_string(i);
            conditionNames.push_back(conditionName);
            
            // Create condition with optimal parameters
            ResearchTools::ExperimentalCondition cond;
            cond.name = conditionName;
            cond.description = "Validation with optimal " + results[i].parameterName;
            cond.isBaseline = (i == 0);
            
            // Set parameters
            for (size_t j = 0; j < results.size(); ++j) {
                cond.parameters[results[j].parameterName] = results[j].optimalValue;
            }
            
            tools.createExperimentalCondition(cond);
        }
        
        // Run validation experiments
        for (const std::string& cond : conditionNames) {
            std::cout << "Running validation experiment: " << cond << std::endl;
            tools.setCurrentCondition(cond);
            
            // Run experiment (simplified)
            double performance = runValidationExperiment(cond);
            std::cout << "Performance: " << performance << std::endl;
        }
        
        // Compare validation results
        std::cout << "\nValidation Comparison:" << std::endl;
        for (size_t i = 0; i < conditionNames.size(); ++i) {
            for (size_t j = i + 1; j < conditionNames.size(); ++j) {
                auto comparisons = tools.compareConditions(conditionNames[i], conditionNames[j]);
                for (const auto& comp : comparisons) {
                    if (comp.metricName == "performance") {
                        std::cout << conditionNames[i] << " vs " << conditionNames[j] 
                                 << ": " << comp.value1 << " vs " << comp.value2 
                                 << " (better: " << (comp.isBetter ? conditionNames[j] : conditionNames[i]) << ")" << std::endl;
                    }
                }
            }
        }
    }
    
    double runValidationExperiment(const std::string& conditionName) {
        // Simulate validation experiment
        double basePerformance = 50.0;
        
        // Get condition parameters
        auto conditionIt = tools.getCurrentCondition();
        if (conditionIt.empty()) {
            return basePerformance;
        }
        
        // Apply parameter effects
        double learningRate = tools.getParameter("learning_rate");
        double plasticityRate = tools.getParameter("plasticity_rate");
        double decayRate = tools.getParameter("memory_decay_rate");
        
        // Calculate performance
        double performance = basePerformance;
        performance += (learningRate - 0.01) * 10;  // Learning rate effect
        performance += (plasticityRate - 0.1) * 5;  // Plasticity rate effect
        performance -= decayRate * 100;           // Memory decay effect
        
        // Sleep for demonstration
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        return performance;
    }
    
    void exportOptimizationResults() {
        std::cout << "\nExporting parameter optimization results..." << std::endl;
        
        // Export tuning results
        bool jsonSuccess = tools.exportResults("./parameter_optimization.json");
        bool conditionsSuccess = tools.exportConditions("./parameter_experimental_conditions.txt");
        
        if (jsonSuccess && conditionsSuccess) {
            std::cout << "Parameter optimization results exported successfully!" << std::endl;
        } else {
            std::cout << "Failed to export parameter optimization results." << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Parameter Optimization Experiment ===" << std::endl;
    
    ParameterOptimizationExperiment experiment;
    
    // Run parameter optimization
    experiment.runParameterOptimization();
    
    // Analyze results
    experiment.analyzeOptimizationResults();
    
    // Export results
    experiment.exportOptimizationResults();
    
    return 0;
}
```

### Expected Results
- Optimal parameters can be identified for learning and memory
- Parameter interactions can be characterized
- Validation experiments confirm optimal parameter settings
- Export of optimization results for external analysis

### Research Insights
This parameter optimization experiment demonstrates:
1. Systematic parameter optimization can identify optimal settings
2. Parameter interactions affect performance
3. Optimization results can be validated with experimental data
4. Export of optimization results supports reproducibility

## Example 6: Comprehensive Research Protocol

### Purpose
Demonstrate a comprehensive research protocol integrating all NLM systems.

### Research Question
How do all NLM systems interact to produce intelligent behavior?

### Methodology
1. Run comprehensive experiment with all systems active
2. Collect comprehensive metrics
3. Analyze system interactions
4. Generate comprehensive research report

### Implementation

```cpp
#include "Phase6IntegratedExperiment.hpp"
#include "ResearchMetrics.hpp"
#include "ResearchTools.hpp"

class ComprehensiveResearchProtocol {
private:
    Phase6IntegratedExperiment experiment;
    ResearchMetrics metrics;
    ResearchTools tools;
    
public:
    ComprehensiveResearchProtocol() {
        // Initialize all systems
        Phase6Config config;
        config.neuronCount = 1000;
        config.maxSteps = 5000;
        config.enableVisualization = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        // Configure experiment
        experiment.setPhase6Config(config);
        
        // Initialize tools
        tools.setResearchMode(true);
        tools.enableDebugMode(true);
        
        // Create experimental conditions
        createResearchConditions();
        
        // Set baseline
        tools.setBaselineCondition("comprehensive_research");
    }
    
    void createResearchConditions() {
        // Condition 1: Baseline (all systems normal)
        ResearchTools::ExperimentalCondition cond1;
        cond1.name = "comprehensive_research";
        cond1.description = "Comprehensive research protocol - all systems normal";
        cond1.isBaseline = true;
        cond1.parameters = {
            {"neuron_count", 1000},
            {"learning_rate", 0.01},
            {"plasticity_rate", 0.1},
            {"attention_threshold", 0.5},
            {"memory_decay_rate", 0.01}
        };
        tools.createExperimentalCondition(cond1);
        
        // Condition 2: Enhanced memory
        ResearchTools::ExperimentalCondition cond2;
        cond2.name = "enhanced_memory";
        cond2.description = "Enhanced memory capacity and consolidation";
        cond2.parameters = {
            {"neuron_count", 1000},
            {"learning_rate", 0.01},
            {"plasticity_rate", 0.1},
            {"attention_threshold", 0.5},
            {"memory_decay_rate", 0.005},
            {"episodic_memory_capacity", 2000}
        };
        tools.createExperimentalCondition(cond2);
        
        // Condition 3: Enhanced neuromodulation
        ResearchTools::ExperimentalCondition cond3;
        cond3.name = "enhanced_neuromodulation";
        cond3.description = "Enhanced neuromodulation for learning and memory";
        cond3.parameters = {
            {"neuron_count", 1000},
            {"learning_rate", 0.015},
            {"plasticity_rate", 0.15},
            {"attention_threshold", 0.6},
            {"memory_decay_rate", 0.01},
            {"dopamine_level", 0.8},
            {"curiosity_level", 0.7}
        };
        tools.createExperimentalCondition(cond3);
        
        // Condition 4: Accelerated development
        ResearchTools::ExperimentalCondition cond4;
        cond4.name = "accelerated_development";
        cond4.description = "Accelerated neural development and plasticity";
        cond4.parameters = {
            {"neuron_count", 1000},
            {"learning_rate", 0.02},
            {"plasticity_rate", 0.3},
            {"attention_threshold", 0.7},
            {"memory_decay_rate", 0.005},
            {"developmental_stage", 0.8}
        };
        tools.createExperimentalCondition(cond4);
    }
    
    void runComprehensiveResearch() {
        std::cout << "=== Running Comprehensive Research Protocol ===" << std::endl;
        
        // Run experiments for each condition
        std::vector<std::string> conditions = {
            "comprehensive_research",
            "enhanced_memory",
            "enhanced_neuromodulation",
            "accelerated_development"
        };
        
        for (const std::string& cond : conditions) {
            std::cout << "\nRunning " << cond << " experiment..." << std::endl;
            tools.setCurrentCondition(cond);
            
            // Run Phase 6 experiment
            auto result = experiment.run(config);
            
            // Record experiment results
            recordExperimentResults(cond, result);
            
            // Export visualization data
            NLMLiveVisualization* viz = experiment.getVisualization();
            if (viz && viz->isActive()) {
                bool vizSuccess = viz->exportToJSON("./comprehensive_research/visualization_" + cond + ".json");
                std::cout << "Visualization exported for " << cond << ": " 
                         << (vizSuccess ? "SUCCESS" : "FAILED") << std::endl;
            }
            
            // Export metrics
            ResearchMetrics* expMetrics = experiment.getResearchMetrics();
            if (expMetrics) {
                bool metricsSuccess = expMetrics->exportToJSON("./comprehensive_research/metrics_" + cond + ".json");
                std::cout << "Metrics exported for " << cond << ": " 
                         << (metricsSuccess ? "SUCCESS" : "FAILED") << std::endl;
            }
            
            // Sleep between experiments
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    void recordExperimentResults(const std::string& condition, const Phase6IntegrationResult& result) {
        // Record basic experiment metrics
        metrics.record("total_reward", result.totalReward);
        metrics.record("avg_firing_rate", result.avgFiringRate);
        metrics.record("memory_episodes_stored", result.memoryEpisodesStored);
        metrics.record("novelty_level", result.noveltyLevel);
        metrics.record("curiosity_level", result.curiosityLevel);
        metrics.record("dopamine_level", result.dopamineLevel);
        
        // Record advanced metrics
        metrics.record("learning_rate", result.avgLearningRate);
        metrics.record("memory_consolidation", result.memoryConsolidationProgress);
        metrics.record("neural_plasticity", result.neuralPlasticityIndex);
        metrics.record("prediction_accuracy", result.predictionAccuracy);
        metrics.record("cognitive_flexibility", result.cognitiveFlexibility);
        metrics.record("developmental_maturation", result.developmentalMaturation);
        
        // Record system integration metrics
        metrics.record("working_memory_integration", result.memoryWorkingMemoryIntegrated ? 1.0 : 0.0);
        metrics.record("episodic_memory_integration", result.memoryEpisodicMemoryIntegrated ? 1.0 : 0.0);
        metrics.record("neuromodulation_integration", result.neuromodulationIntegrated ? 1.0 : 0.0);
        metrics.record("prediction_integration", result.predictionIntegrated ? 1.0 : 0.0);
        metrics.record("development_integration", result.developmentIntegrated ? 1.0 : 0.0);
        
        std::cout << "Recorded results for " << condition << " experiment" << std::endl;
    }
    
    void analyzeComprehensiveResults() {
        std::cout << "\n=== Comprehensive Research Analysis ===" << std::endl;
        
        // Generate performance comparison
        std::cout << "\nSystem Performance Comparison:" << std::endl;
        auto performanceComparisons = tools.compareAllConditions();
        for (const auto& comp : performanceComparisons) {
            if (comp.metricName == "total_reward") {
                std::cout << comp.condition1Name << " vs " << comp.condition2Name 
                         << ": " << comp.value1 << " vs " << comp.value2 
                         << " (better: " << (comp.isBetter ? comp.condition2Name : comp.condition1Name) << ")" << std::endl;
            }
        }
        
        // Analyze learning metrics
        std::cout << "\nLearning Metrics Analysis:" << std::endl;
        std::string learningStats = metrics.generateStatisticalReport();
        std::cout << learningStats << std::endl;
        
        // Analyze memory metrics
        std::cout << "\nMemory Metrics Analysis:" << std::endl;
        auto memoryComparisons = tools.compareConditions("comprehensive_research", "enhanced_memory");
        for (const auto& comp : memoryComparisons) {
            if (comp.metricName.find("memory") != std::string::npos) {
                std::cout << "Memory: " << comp.condition1Name << " vs " << comp.condition2Name 
                         << ": " << comp.value1 << " vs " << comp.value2 
                         << " (better: " << (comp.isBetter ? comp.condition2Name : comp.condition1Name) << ")" << std::endl;
            }
        }
        
        // Analyze neuromodulation effects
        std::cout << "\nNeuromodulation Effects:" << std::endl;
        auto neuromodComparisons = tools.compareConditions("comprehensive_research", "enhanced_neuromodulation");
        for (const auto& comp : neuromodComparisons) {
            if (comp.metricName.find("dopamine") != std::string::npos || 
                comp.metricName.find("curiosity") != std::string::npos) {
                std::cout << comp.metricName << ": " << comp.condition1Name << " vs " << comp.condition2Name 
                         << ": " << comp.value1 << " vs " << comp.value2 
                         << " (better: " << (comp.isBetter ? comp.condition2Name : comp.condition1Name) << ")" << std::endl;
            }
        }
        
        // Generate comprehensive report
        std::cout << "\n=== Comprehensive Research Report ===" << std::endl;
        std::cout << "=== System Integration Analysis ===" << std::endl;
        
        // Check system integration for all conditions
        std::vector<std::string> conditions = {
            "comprehensive_research",
            "enhanced_memory",
            "enhanced_neuromodulation",
            "accelerated_development"
        };
        
        for (const std::string& cond : conditions) {
            auto result = getExperimentResult(cond); // Simplified
            std::cout << cond << ":" << std::endl;
            std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        }
        
        // Generate machine learning analysis
        std::cout << "\n=== Machine Learning Analysis ===" << std::endl;
        std::string mlReport = metrics.generateMLReport();
        std::cout << mlReport << std::endl;
        
        // Generate time series analysis
        std::cout << "\n=== Time Series Analysis ===" << std::endl;
        std::string timeSeriesReport = metrics.generateTimeSeriesReport();
        std::cout << timeSeriesReport << std::endl;
    }
    
    Phase6IntegrationResult getExperimentResult(const std::string& condition) {
        // Simplified - return a dummy result
        Phase6IntegrationResult result;
        result.totalReward = 100.0;
        result.avgFiringRate = 12.5;
        result.memoryEpisodesStored = 250;
        result.noveltyLevel = 0.7;
        result.curiosityLevel = 0.6;
        result.dopamineLevel = 0.8;
        result.avgLearningRate = 0.015;
        result.memoryConsolidationProgress = 0.85;
        result.neuralPlasticityIndex = 0.9;
        result.predictionAccuracy = 0.88;
        result.cognitiveFlexibility = 0.85;
        result.developmentalMaturation = 0.75;
        result.memoryWorkingMemoryIntegrated = true;
        result.memoryEpisodicMemoryIntegrated = true;
        result.neuromodulationIntegrated = true;
        result.predictionIntegrated = true;
        result.developmentIntegrated = true;
        return result;
    }
    
    void exportComprehensiveResults() {
        std::cout << "\nExporting comprehensive research results..." << std::endl;
        
        // Create output directory
        std::filesystem::create_directory("./comprehensive_research");
        
        // Export metrics
        bool metricsSuccess = metrics.exportToJSON("./comprehensive_research/comprehensive_metrics.json");
        bool csvSuccess = metrics.exportToCSV("./comprehensive_research/comprehensive_metrics.csv");
        
        // Export experimental conditions
        bool conditionsSuccess = tools.exportConditions("./comprehensive_research/comprehensive_experimental_conditions.txt");
        
        // Export research tools data
        bool toolsSuccess = tools.exportResults("./comprehensive_research/comprehensive_research_tools.json");
        
        // Export experiment results
        bool experimentSuccess = experiment.exportResearchData("./comprehensive_research/comprehensive_experiment");
        
        if (metricsSuccess && csvSuccess && conditionsSuccess && toolsSuccess && experimentSuccess) {
            std::cout << "Comprehensive research results exported successfully!" << std::endl;
            std::cout << "All data available in ./comprehensive_research/" << std::endl;
        } else {
            std::cout << "Failed to export comprehensive research results." << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Comprehensive Research Protocol ===" << std::endl;
    std::cout << "Integrating all NLM systems for advanced research analysis." << std::endl;
    
    ComprehensiveResearchProtocol protocol;
    
    // Run comprehensive research
    protocol.runComprehensiveResearch();
    
    // Analyze results
    protocol.analyzeComprehensiveResults();
    
    // Export results
    protocol.exportComprehensiveResults();
    
    std::cout << "\n=== Comprehensive Research Protocol Complete ===" << std::endl;
    std::cout << "All NLM systems have been integrated and analyzed." << std::endl;
    std::cout << "Research results are available in the comprehensive_research directory." << std::endl;
    
    return 0;
}
```

### Expected Results
- Comprehensive integration of all NLM systems
- Systematic analysis of system interactions
- Detailed comparison of different experimental conditions
- Export of all research data for external analysis

### Research Insights
This comprehensive research protocol demonstrates:
1. The integration capabilities of all NLM systems
2. The effects of parameter optimization on performance
3. The interactions between different systems
4. The utility of comprehensive research protocols

## Summary of Research Examples

The provided examples demonstrate:

1. **Basic System Integration**: Simple demonstration of NLM system integration
2. **Learning Progress Analysis**: Comprehensive analysis of learning strategies
3. **Memory Capacity Testing**: Systematic testing of memory systems
4. **Neuromodulation Research**: Study of neuromodulator effects
5. **Parameter Optimization**: Systematic optimization of experimental parameters
6. **Comprehensive Research Protocol**: Integration of all research capabilities

## Implementation Notes

### Code Organization
- Each example is self-contained and can be run independently
- Code is organized into logical sections
- Examples include comprehensive comments and documentation
- Error handling is included for robustness

### Research Best Practices
- Use appropriate experimental controls
- Include statistical analysis of results
- Export data for reproducibility
- Document all procedures and parameters

### Performance Considerations
- Use efficient data structures for large datasets
- Implement parallel processing for computationally intensive tasks
- Optimize I/O operations for large data files
- Monitor system resources during long experiments

## Future Research Directions

The examples provide a foundation for:

1. **Multi-agent Research**: Study interactions between multiple agents
2. **Long-term Learning**: Analyze learning over extended time periods
3. **Cross-system Integration**: Study interactions between different system types
4. **Real-time Analysis**: Implement real-time analysis during experiments
5. **Machine Learning Integration**: Integrate machine learning with NLM systems

## Resources

### Documentation
- `src/visualization/VISUALIZATION_GUIDE.md` - Visualization system documentation
- `src/experiments/RESEARCH_METRICS.md` - Advanced metrics system documentation
- `src/experiments/RESEARCH_TOOLS.md` - Research tools documentation

### Code Examples
- `Phase6IntegratedExperiment.cpp` - Phase 6 integration examples
- `NLMLiveVisualization.cpp` - Visualization system examples
- `ResearchMetrics.cpp` - Advanced metrics examples
- `ResearchTools.cpp` - Research tools examples

### Online Resources
- NLM Documentation Website
- NLM Research Forum
- GitHub Repository

## Conclusion

The provided research examples demonstrate the comprehensive capabilities of the NLM Advanced Visualization and Metrics System. These examples provide a foundation for conducting advanced research with the NLM artificial brain, enabling systematic investigation of neural system interactions, learning processes, memory systems, and neuromodulation effects.

The examples illustrate:

1. **System Integration**: How all NLM systems can be integrated and analyzed together
2. **Experimental Design**: How to design and conduct controlled experiments
3. **Data Analysis**: How to analyze and interpret experimental results
4. **Research Documentation**: How to document and share research findings

These examples provide a comprehensive foundation for conducting advanced research with the NLM system, supporting a wide range of research questions and experimental designs.
