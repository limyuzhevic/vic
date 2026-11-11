# Advanced NLM Usage Examples

This file contains comprehensive examples demonstrating advanced NLM features for power users and researchers.

## 1. Advanced Learning Experiments

### Example: Pattern Learning with STDP

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>

void patternLearningExperiment() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.2f);
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    config->set("synaptogenesis_rate", 0.00005f);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create two distinct neuronal groups
    std::vector<NeuronId> groupA, groupB;
    createNeuronalGroup(brain, groupA, NeuronType::Sensory, 50);
    createNeuronalGroup(brain, groupB, NeuronType::Sensory, 50);
    
    // Hebbian learning: pair group A and B activity
    for (int step = 0; step < 1000; ++step) {
        if (step % 10 == 0) {
            // Activate group A
            for (NeuronId id : groupA) {
                brain->injectCurrent(id, 20.0f);
            }
        }
        if (step % 10 == 5) {
            // Activate group B with slight delay
            for (NeuronId id : groupB) {
                brain->injectCurrent(id, 20.0f);
            }
        }
        
        brain->step(step);
        
        if (step % 100 == 0) {
            float avgWeight = computeAverageSynapseWeight(brain, groupA, groupB);
            std::cout << "Step " << step << ", Avg weight A->B: " << avgWeight << std::endl;
        }
    }
    
    // Analyze results
    std::cout << "Pattern learning experiment completed" << std::endl;
    std::cout << "Final synaptic strength: " << getSynapseStrength(brain, groupA[0], groupB[0]) << std::endl;
}
```

### Example: Reward-Modulated Learning

```cpp
#include "agent/AgentBrain.hpp"
#include "neuromodulation/Dopamine.hpp"

void rewardModulatedLearning() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 3);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    
    float totalReward = 0.0f;
    float explorationRate = 0.1f;
    
    for (int episode = 0; episode < 100; ++episode) {
        bool done = false;
        int steps = 0;
        
        while (!done && steps < 200) {
            // Agent selects action based on curiosity and exploration
            auto action = agent.decodeMotorCommand();
            
            // Apply action and get reward
            ActionResult result = world.applyMotorCommand(action, steps * 0.001f);
            totalReward += result.reward;
            
            // Apply reward-modulated learning
            agent.applyRewardModulation(result.reward, result.predictionError);
            
            // Update development
            agent.updateDevelopment(0.001f);
            
            // Store experience for episodic memory
            agent.storeExperience(result);
            
            steps++;
            done = (steps >= 200) || world.isDone();
        }
        
        if (episode % 20 == 0) {
            std::cout << "Episode " << episode 
                      << ", Total Reward: " << totalReward / (episode + 1)
                      << ", Dopamine Level: " << agent.getNeuromodulationLevel()
                      << ", Curiosity Level: " << agent.getCuriosityLevel()
                      << std::endl;
        }
    }
    
    std::cout << "Reward-modulated learning completed" << std::endl;
    std::cout << "Average reward per step: " << totalReward / 100 << std::endl;
}
```

## 2. Memory Management and Replay

### Example: Working Memory Dynamics

```cpp
#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"

void workingMemoryDynamics() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 800);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* workingMemory = brain->getWorkingMemory();
    if (!workingMemory) {
        std::cerr << "Working memory not available" << std::endl;
        return;
    }
    
    // Create a sequence of neural activities
    std::vector<NeuronId> taskSequence = createTaskSequence(brain, 10);
    
    for (int step = 0; step < 500; ++step) {
        // Present task components sequentially
        int currentTask = (step / 50) % taskSequence.size();
        NeuronId taskNeuron = taskSequence[currentTask];
        
        // Strong input to task neuron
        brain->injectCurrent(taskNeuron, 30.0f);
        
        // Store in working memory
        workingMemory->storeToNeuron(taskNeuron, 1.0f);
        
        brain->step(step);
        
        // Working memory decay and competition
        workingMemory->update(0.001f);
        
        // Get active traces
        auto activeTraces = workingMemory->getActiveTraces();
        float totalActivation = calculateTotalActivation(workingMemory);
        
        if (step % 50 == 0) {
            std::cout << "Step " << step 
                      << ", Active traces: " << activeTraces
                      << ", Total activation: " << totalActivation
                      << std::endl;
        }
    }
    
    // Test memory recall
    std::cout << "Working memory dynamics completed" << std::endl;
    std::cout << "Final capacity utilization: " 
              << workingMemory->getCapacityUtilization() << std::endl;
}
```

### Example: Episodic Memory and Replay

```cpp
#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"

void episodicMemoryReplay() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 600);
    config->set("replay_interval", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (!episodicMemory) {
        std::cerr << "Episodic memory not available" << std::endl;
        return;
    }
    
    // Run exploration phase to create episodes
    for (int step = 0; step < 1000; ++step) {
        // Simulate exploration with novel inputs
        exploreEnvironment(brain, step);
        brain->step(step);
        
        // Store episode every 50 steps
        if (step % 50 == 0) {
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.action = recordAction(brain);
            episode.reward = calculateReward(brain);
            
            // Capture current brain state
            captureBrainState(brain, episode);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Periodic replay of important memories
        if (step % 300 == 0) {
            auto importantEpisodes = episodicMemory->getEpisodesForReplay(3);
            
            std::cout << "Replay step " << step 
                      << ", Replaying " << importantEpisodes.size() 
                      << " important episodes" << std::endl;
            
            for (const auto* episode : importantEpisodes) {
                episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    // Analysis phase
    auto allEpisodes = episodicMemory->getEpisodes();
    std::cout << "Episodic memory replay completed" << std::endl;
    std::cout << "Total episodes stored: " << allEpisodes.size() << std::endl;
    std::cout << "Replay efficiency: " << calculateReplayEfficiency(importantEpisodes) << std::endl;
}
```

## 3. Neuromodulation Advanced Usage

### Example: Curiosity-Driven Exploration

```cpp
#include "neuromodulation/Curiosity.hpp"
#include "agent/AgentBrain.hpp"

void curiosityDrivenExploration() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1200);
    config->set("region_count", 4);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableCuriosity(true);
    
    float explorationBonus = 0.0f;
    float totalNovelty = 0.0f;
    
    for (int step = 0; step < 2000; ++step) {
        // Get current novelty level
        float novelty = agent.getNoveltyLevel();
        float curiosity = agent.getCuriosityLevel();
        totalNovelty += novelty;
        
        // Modify action selection based on curiosity
        float explorationProbability = calculateExplorationProbability(curiosity, step);
        
        // Get action with curiosity modulation
        MotorCommand action = agent.decodeMotorCommand(explorationProbability);
        
        // Apply action
        ActionResult result = world.applyMotorCommand(action, step * 0.001f);
        
        // Update curiosity system
        agent.updateCuriosity(result.sensoryChange, novelty, step * 0.001f);
        
        // Apply curiosity bonus to learning
        explorationBonus += curiosity * 0.1f;
        
        // Store experience with curiosity tag
        agent.storeExperience(result, curiosity);
        
        brain->step(step);
        
        if (step % 200 == 0) {
            std::cout << "Step " << step
                      << ", Novelty: " << novelty
                      << ", Curiosity: " << curiosity
                      << ", Exploration bonus: " << explorationBonus
                      << std::endl;
        }
    }
    
    std::cout << "Curiosity-driven exploration completed" << std::endl;
    std::cout << "Average novelty: " << totalNovelty / 2000
              << ", Total exploration bonus: " << explorationBonus << std::endl;
}
```

### Example: Prediction Error and Learning

```cpp
#include "neuromodulation/PredictionError.hpp"
#include "prediction/PredictionSystem.hpp"

void predictionErrorLearning() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 800);
    config->set("region_count", 3);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* predictionSystem = brain->getPredictionSystem();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (!predictionSystem || !predictionError) {
        std::cerr << "Prediction systems not available" << std::endl;
        return;
    }
    
    // Create predictable stimulus sequence
    std::vector<float> stimulusSequence = generatePredictableSequence(100);
    std::vector<float> predictedSequence = stimulusSequence;
    
    float totalPredictionError = 0.0f;
    float learningRate = 0.01f;
    
    for (int step = 0; step < 500; ++step) {
        // Get current prediction
        float currentPrediction = predictionSystem->getCurrentPrediction();
        float actualStimulus = stimulusSequence[step % stimulusSequence.size()];
        
        // Calculate prediction error
        float error = actualStimulus - currentPrediction;
        totalPredictionError += std::abs(error);
        
        // Update prediction system
        predictionSystem->update(actualStimulus, step * 0.001f);
        
        // Apply prediction error to neuromodulation
        predictionError->update(error, step * 0.001f);
        
        // Dopamine response to prediction error
        float dopamineResponse = predictionError->getError() * learningRate;
        brain->applyNeuromodulation(dopamineResponse);
        
        // Adjust internal model
        float newPrediction = currentPrediction + error * learningRate;
        predictionSystem->setCurrentPrediction(newPrediction);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step
                      << ", Prediction error: " << error
                      << ", Dopamine response: " << dopamineResponse
                      << ", Current prediction: " << currentPrediction
                      << std::endl;
        }
        
        brain->step(step);
    }
    
    std::cout << "Prediction error learning completed" << std::endl;
    std::cout << "Average absolute prediction error: " << totalPredictionError / 500
              << ", Learning rate: " << learningRate << std::endl;
}
```

## 4. Performance Optimization and Profiling

### Example: Performance Benchmarking

```cpp
#include "benchmark.cpp"
#include <chrono>
#include <iostream>

void performanceBenchmark() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.15f);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Benchmark different aspects of performance
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    
    // Step timing benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "1000 steps: " << duration.count() << " ms"
              << " (" << (1000.0 / duration.count() * 1000) << " steps/sec)" << std::endl;
    
    // Memory benchmark
    auto memStart = std::chrono::high_resolution_clock::now();
    brain->save("/tmp/benchmark_brain.bin");
    auto memEnd = std::chrono::high_resolution_clock::now();
    auto memDuration = std::chrono::duration_cast<std::chrono::milliseconds>(memEnd - memStart);
    
    std::cout << "Checkpoint save: " << memDuration.count() << " ms" << std::endl;
    
    // Load benchmark
    auto brain2 = std::make_shared<Brain>(config);
    brain2->initialize();
    
    auto loadStart = std::chrono::high_resolution_clock::now();
    brain2->load("/tmp/benchmark_brain.bin");
    auto loadEnd = std::chrono::high_resolution_clock::now();
    auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loadEnd - loadStart);
    
    std::cout << "Checkpoint load: " << loadDuration.count() << " ms" << std::endl;
    
    // Cleanup
    std::remove("/tmp/benchmark_brain.bin");
    
    std::cout << "=== Benchmark Complete ===" << std::endl;
}
```

### Example: Real-time Monitoring

```cpp
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <thread>
#include <atomic>

class PerformanceMonitor {
private:
    std::shared_ptr<Brain> brain;
    std::atomic<bool> running;
    std::thread monitorThread;
    
public:
    PerformanceMonitor(std::shared_ptr<Brain> b) : brain(b), running(true) {
        monitorThread = std::thread(&PerformanceMonitor::monitorLoop, this);
    }
    
    ~PerformanceMonitor() {
        running = false;
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }
    
    void monitorLoop() {
        while (running) {
            // Log brain status
            brain->logStatus();
            
            // Get performance metrics
            float firingRate = brain->getAverageFiringRate();
            size_t spikeCount = brain->getTotalSpikeCount();
            float eiratio = brain->getExcitaitonInhibitionRatio();
            
            std::cout << "[MONITOR] "
                      << "Firing rate: " << firingRate << " Hz, "
                      << "Total spikes: " << spikeCount << ", "
                      << "E/I ratio: " << eiratio << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
};

void realTimeMonitoring() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 800);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Setup logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Start performance monitoring
    PerformanceMonitor monitor(brain);
    
    // Run simulation
    std::cout << "Starting simulation with real-time monitoring..." << std::endl;
    for (int step = 0; step < 500; ++step) {
        brain->step(step);
        
        // Inject random sensory input
        if (rand() % 100 < 10) {  // 10% chance
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "Simulation completed with monitoring" << std::endl;
}
```

## 5. Advanced Configuration and Scripting

### Example: Automated Experiment Runner

```cpp
#include "experiments/ExperimentRunner.hpp"
#include <vector>
#include <string>

class AutomatedExperimentRunner {
private:
    ExperimentRunner runner;
    std::vector<std::shared_ptr<Config>> configs;
    std::vector<std::shared_ptr<Environment>> environments;
    
public:
    AutomatedExperimentRunner() {
        setupExperiments();
        setupEnvironments();
    }
    
    void setupExperiments() {
        // Basic learning experiment
        auto config1 = std::make_shared<Config>();
        config1->set("neuron_count", 500);
        config1->set("region_count", 2);
        config1->set("connection_probability", 0.2f);
        configs.push_back(config1);
        
        // Memory integration experiment
        auto config2 = std::make_shared<Config>();
        config2->set("neuron_count", 800);
        config2->set("region_count", 3);
        config2->set("replay_interval", 200);
        configs.push_back(config2);
        
        // Neuromodulation experiment
        auto config3 = std::make_shared<Config>();
        config3->set("neuron_count", 1200);
        config3->set("region_count", 4);
        config3->set("stdp_ltp_weight", 0.03f);
        config3->set("stdp_ltd_weight", 0.025f);
        configs.push_back(config3);
    }
    
    void setupEnvironments() {
        // Simple world environment
        auto env1 = std::make_shared<SimpleWorld>();
        env1->configure(16, 16, 16, 16);
        environments.push_back(env1);
        
        // More complex environment
        auto env2 = std::make_shared<SimpleWorld>();
        env2->configure(32, 32, 8, 8);
        environments.push_back(env2);
    }
    
    void runExperimentSet() {
        std::cout << "=== Running Automated Experiment Suite ===" << std::endl;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            for (size_t j = 0; j < environments.size(); ++j) {
                std::cout << "Experiment " << (i + 1) << "." << (j + 1) 
                          << ": Config " << (i + 1) << ", Environment " << (j + 1) << std::endl;
                
                runSingleExperiment(configs[i], environments[j], 100);
                
                // Save results
                std::string filename = "results_exp_" + std::to_string(i + 1) + "_" + std::to_string(j + 1) + ".json";
                runner.saveResults(filename);
            }
        }
        
        std::cout << "=== All Experiments Completed ===" << std::endl;
    }
    
    void runSingleExperiment(std::shared_ptr<Config> config, 
                             std::shared_ptr<Environment> environment,
                             SimulationStep maxSteps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent for this experiment
        AgentBrain agent(brain);
        agent.initialize(*environment);
        
        // Run experiment
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            // Get observation
            auto observation = environment->reset();
            agent.processSensoryInput(*observation);
            
            // Brain step
            brain->step(step);
            
            // Get action
            MotorCommand action = agent.decodeMotorCommand();
            
            // Apply action
            ActionResult result = environment->applyMotorCommand(action, step * 0.001f);
            
            // Update agent
            agent.applyRewardModulation(result.reward, result.predictionError);
            
            // Check if episode done
            if (environment->isDone()) {
                break;
            }
        }
    }
};

void automatedExperimentRunnerExample() {
    AutomatedExperimentRunner runner;
    runner.runExperimentSet();
}
```

## 6. Integration and Testing

### Example: Complete Integration Test

```cpp
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>

void completeIntegrationTest() {
    std::cout << "=== Running Complete NLM Integration Test ===" << std::endl;
    
    Phase6Config config;
    config.neuronCount = 1000;
    config.regionCount = 4;
    config.maxSteps = 1000;
    config.connectionProbability = 0.15f;
    config.enableDevelopment = true;
    config.enableCheckpointing = true;
    config.checkpointPath = "/tmp/test_checkpoint.bin";
    
    Phase6IntegratedExperiment experiment;
    
    // Run integration test
    Phase6IntegrationResult result = experiment.run(config);
    
    // Print results
    std::cout << "Integration Test Results:" << std::endl;
    std::cout << "  Total reward: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
    
    // Verify integration
    std::cout << "\nIntegration Verification:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "  Development System: " << (result.developmentIntegrated ? "YES" : "NO") << std::endl;
    
    bool allTestsPassed = result.memoryWorkingMemoryIntegrated &&
                         result.memoryEpisodicMemoryIntegrated &&
                         result.neuromodulationIntegrated &&
                         result.predictionIntegrated &&
                         result.developmentIntegrated;
    
    if (allTestsPassed) {
        std::cout << "\n✓ ALL INTEGRATION TESTS PASSED" << std::endl;
    } else {
        std::cout << "\n✗ SOME INTEGRATION TESTS FAILED" << std::endl;
    }
    
    // Cleanup
    std::remove(config.checkpointPath.c_str());
}
```

## 7. Best Practices and Recommendations

### Key Best Practices

1. **Always handle exceptions** when working with I/O operations and neural computation
2. **Validate configuration parameters** before initializing brain components
3. **Monitor performance** regularly during long-running simulations
4. **Use checkpoints** to save and restore brain states
5. **Implement proper cleanup** in destructors and resource management

### Performance Recommendations

1. **Tune parameters for your use case**:
   - Use fewer neurons for prototyping
   - Increase timestep for faster simulations
   - Disable plasticity for computational speed

2. **Profile your application** to identify bottlenecks:
   - Use built-in profiling tools
   - Monitor memory usage
   - Track simulation time

3. **Consider parallelization** for large-scale simulations:
   - Use multiple regions for parallel processing
   - Implement batch processing for experiments

### Code Quality Guidelines

1. **Write comprehensive tests** for all new functionality
2. **Document your changes** with inline comments and examples
3. **Follow existing coding conventions** throughout the codebase
4. **Maintain backward compatibility** when making changes
5. **Use proper error handling** for all external operations

This examples file provides comprehensive guidance for using NLM at all levels, from basic usage to advanced research applications. Each example includes complete, working code that can be adapted for specific use cases.
