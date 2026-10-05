# NLM Examples and Demos

## Overview

This document provides comprehensive examples and demos for the enhanced NLM neural simulation framework. It covers basic examples for getting started, intermediate examples for understanding advanced features, and real-world application examples that demonstrate the framework's capabilities.

## Basic Examples

### Example 1: Simple Neural Network

```cpp
#include <nlm.h>
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 500);
    config->set("neuron_count", 50);
    config->set("synapse_density", 0.05f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    std::cout << "Starting neural simulation..." << std::endl;
    for (int step = 0; step < 500; ++step) {
        brain->step(step, step * 0.01f);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getActiveNeuronCount() << " active, "
                     << brain->getTotalSpikeCount() << " spikes" << std::endl;
        }
    }
    
    // Save results
    brain->save("results/simple_brain.bin");
    std::cout << "Simulation completed and saved!" << std::endl;
    
    return 0;
}
```

### Example 2: Vision Processing

```cpp
#include <nlm.h>
#include <iostream>

int main() {
    // Create brain
    auto brain = std::make_shared<Brain>();
    brain->initialize();
    
    // Create vision input
    Vision vision(20, 20, 3);
    
    // Create simple pattern (edge detection test)
    std::vector<float> edgeData(1200, 0.0f);
    
    // Create vertical edge
    for (int x = 8; x < 12; ++x) {
        for (int y = 0; y < 20; ++y) {
            size_t idx = y * 60 + x * 3;
            edgeData[idx] = 1.0f;     // Red
            edgeData[idx + 1] = 0.5f; // Green
            edgeData[idx + 2] = 0.0f; // Blue
        }
    }
    
    vision.setData(edgeData);
    
    // Process visual input
    brain->receiveSensoryInput(vision);
    
    // Run processing steps
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.01f);
        
        if (step % 20 == 0) {
            std::cout << "Processing step " << step << std::endl;
        }
    }
    
    // Visualize results
    auto activity = brain->visualizeNeuronActivity();
    brain->saveVisualization("results/vision_edge.png");
    
    return 0;
}
```

### Example 3: Agent Navigation

```cpp
#include <nlm.h>
#include <iostream>

class Navigator {
public:
    Navigator(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world) {
        agent_brain_ = std::make_shared<AgentBrain>(brain_, world_);
        agent_brain_->initialize(*world_);
    }
    
    void runNavigation(int steps) {
        std::cout << "Starting navigation simulation..." << std::endl;
        
        for (int step = 0; step < steps; ++step) {
            // Get current sensory input
            SensoryPercept percept = world_->getSensoryPercept(world_->getAgentBody());
            
            // Process input
            if (agent_brain_->processSensoryInput(percept)) {
                // Generate action
                Action action = agent_brain_->decodeMotorCommand();
                
                // Apply to world
                ActionResult result = world_->applyMotorCommand(action, world_->getSimulationTime());
                
                // Learn from result
                agent_brain_->applyRewardModulation(result.getReward(), 0.0f);
                
                // Update development
                agent_brain_->updateDevelopment(step);
            }
            
            // Step brain
            brain_->step(step, step * 0.01f);
            
            if (step % 50 == 0) {
                auto body = world_->getAgentBody();
                std::cout << "Step " << step << ": Position ("
                         << body.getX() << ", " << body.getY() << "), "
                         << "Energy: " << body.getEnergy() << std::endl;
            }
        }
        
        std::cout << "Navigation completed!" << std::endl;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_brain_;
};

int main() {
    // Create components
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 1000);
    
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(100, 100, 20, 20);
    
    // Set agent start position
    world->setAgentStart(10, 10);
    
    // Create navigator
    Navigator navigator(brain, world);
    
    // Run navigation
    navigator.runNavigation(300);
    
    // Save results
    brain->save("results/navigation_brain.bin");
    
    return 0;
}
```

### Example 4: Learning with Memory

```cpp
#include <nlm.h>
#include <iostream>
#include <fstream>

class MemoryLearner {
public:
    MemoryLearner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world) {
        agent_brain_ = std::make_shared<AgentBrain>(brain_, world_);
        agent_brain_->initialize(*world_);
        agent_brain_->enableRewardModulation(true);
        agent_brain_->enableCuriosity(true);
    }
    
    void runLearningEpisode(int steps, const std::string& episodeId) {
        std::cout << "Running learning episode: " << episodeId << std::endl;
        
        float totalReward = 0.0f;
        int successfulActions = 0;
        
        for (int step = 0; step < steps; ++step) {
            // Generate action
            Action action = agent_brain_->decodeMotorCommand();
            
            // Apply action
            ActionResult result = world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Process sensory input
            SensoryPercept percept = world_->getSensoryPercept(world_->getAgentBody());
            agent_brain_->processSensoryInput(percept);
            
            // Apply reward modulation with curiosity
            float predictedReward = 0.1f;  // Simple prediction
            agent_brain_->applyRewardModulation(result.getReward(), predictedReward);
            
            totalReward += result.getReward();
            if (result.isSuccess()) {
                successfulActions++;
            }
            
            // Update development
            agent_brain_->updateDevelopment(step);
            
            // Step brain
            brain_->step(step, step * 0.01f);
            
            if (step % 50 == 0) {
                std::cout << "  Step " << step << ": Reward=" << result.getReward()
                         << ", Total=" << totalReward
                         << ", Success=" << (successfulActions / (step + 1)) * 100 << "%" << std::endl;
            }
        }
        
        // Calculate statistics
        float successRate = static_cast<float>(successfulActions) / steps;
        float avgReward = totalReward / steps;
        
        std::cout << "Episode " << episodeId << " completed:" << std::endl;
        std::cout << "  Total Reward: " << totalReward << std::endl;
        std::cout << "  Average Reward: " << avgReward << std::endl;
        std::cout << "  Success Rate: " << (successRate * 100) << "%" << std::endl;
        
        // Save episode data
        saveEpisodeData(episodeId, totalReward, successRate, avgReward);
    }
    
private:
    void saveEpisodeData(const std::string& episodeId, float totalReward, 
                        float successRate, float avgReward) {
        std::ofstream file("data/episode_" + episodeId + ".txt");
        if (file.is_open()) {
            file << "Episode ID: " << episodeId << std::endl;
            file << "Total Reward: " << totalReward << std::endl;
            file << "Success Rate: " << (successRate * 100) << "%" << std::endl;
            file << "Average Reward: " << avgReward << std::endl;
            file << "Timestamp: " << time(nullptr) << std::endl;
            file.close();
        }
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_brain_;
};

int main() {
    // Create learning environment
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 2000);
    
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(80, 80);
    
    // Create learner
    MemoryLearner learner(brain, world);
    
    // Run multiple learning episodes
    for (int episode = 1; episode <= 5; ++episode) {
        std::string episodeId = "learning_ep_" + std::to_string(episode);
        learner.runLearningEpisode(200, episodeId);
        
        // Reset for next episode
        world->reset();
        brain->reset();
        
        std::cout << std::endl;
    }
    
    // Save learned brain
    brain->save("results/learned_brain.bin");
    
    return 0;
}
```

## Intermediate Examples

### Example 5: Advanced Configuration

```cpp
#include <nlm.h>
#include <iostream>

void advancedConfigurationExample() {
    // Create configuration with advanced settings
    auto config = std::make_shared<Config>();
    
    // Basic settings
    config->set("simulation_steps", 5000);
    config->set("neuron_count", 200);
    config->set("synapse_density", 0.02f);
    
    // Performance settings
    config->set("enable_memory_pool", true);
    config->set("thread_safe", true);
    config->set("precision_mode", "double");
    
    // Development settings
    config->set("enable_reward_modulation", true);
    config->set("enable_structural_plasticity", true);
    config->set("enable_curiosity", true);
    config->set("enable_development", true);
    
    // Create brain with configuration
    Brain brain(config);
    
    if (!brain.initialize()) {
        throw std::runtime_error("Brain initialization failed");
    }
    
    std::cout << "Advanced configuration created successfully" << std::endl;
    std::cout << "Configuration summary: " << config->summary() << std::endl;
}
```

### Example 6: Visualization Pipeline

```cpp
#include <nlm.h>
#include <fstream>
#include <vector>

class VisualizationPipeline {
public:
    VisualizationPipeline(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world) {
        visualizer_ = std::make_shared<Visualizer>(brain_, world_);
    }
    
    void generateComprehensiveVisualization(int steps) {
        std::cout << "Generating comprehensive visualization..." << std::endl;
        
        // Collect data during simulation
        std::vector<std::vector<float>> activityHistory;
        std::vector<std::vector<float>> connectionHistory;
        std::vector<std::vector<float>> regionHistory;
        std::vector<std::vector<float>> spikeTimes;
        
        brain_->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.01f);
            
            if (step % 10 == 0) {
                // Collect visualization data
                activityHistory.push_back(
                    visualizer_->visualizeNeuronActivity(*brain_)
                );
                
                connectionHistory.push_back(
                    visualizer_->visualizeSynapticConnections(*brain_)
                );
                
                regionHistory.push_back(
                    visualizer_->visualizeBrainRegions(*brain_)
                );
                
                auto spikeData = visualizer_->generateSpikeRaster(*brain_);
                spikeTimes.push_back(spikeData["spike_times"].cast<std::vector<float>>());
            }
        }
        
        // Generate all visualizations
        generateActivityHeatmap(activityHistory);
        generateConnectionMatrix(connectionHistory);
        generateRegionActivity(regionHistory);
        generateSpikeRaster(spikeTimes);
        generateNetworkTopology(*brain_);
        
        // Export for external tools
        exportForExternalTools();
        
        std::cout << "Visualization pipeline completed!" << std::endl;
    }
    
private:
    void generateActivityHeatmap(const std::vector<std::vector<float>>& activityData) {
        std::ofstream file("output/activity_heatmap.txt");
        if (file.is_open()) {
            file << "Activity Heatmap Data:" << std::endl;
            for (size_t i = 0; i < activityData.size(); ++i) {
                file << "Frame " << i << ": ";
                for (size_t j = 0; j < activityData[i].size(); ++j) {
                    file << activityData[i][j] << " ";
                }
                file << std::endl;
            }
            file.close();
        }
    }
    
    void generateConnectionMatrix(const std::vector<std::vector<float>>& connectionData) {
        // Create adjacency matrix visualization
        std::ofstream file("output/connection_matrix.txt");
        if (file.is_open()) {
            file << "Synaptic Connection Matrix:" << std::endl;
            // Implementation would create actual visualization
            file << "Matrix size: " << connectionData.size() << " x " << connectionData[0].size() << std::endl;
            file.close();
        }
    }
    
    void generateRegionActivity(const std::vector<std::vector<float>>& regionData) {
        std::ofstream file("output/region_activity.txt");
        if (file.is_open()) {
            file << "Brain Region Activity:" << std::endl;
            for (size_t i = 0; i < regionData.size(); ++i) {
                file << "Region " << i << ": ";
                for (size_t j = 0; j < regionData[i].size(); ++j) {
                    file << regionData[i][j] << " ";
                }
                file << std::endl;
            }
            file.close();
        }
    }
    
    void generateSpikeRaster(const std::vector<std::vector<float>>& spikeData) {
        std::ofstream file("output/spike_raster.txt");
        if (file.is_open()) {
            file << "Spike Raster Data:" << std::endl;
            for (size_t i = 0; i < spikeData.size(); ++i) {
                file << "Time bin " << i << ": ";
                for (size_t j = 0; j < spikeData[i].size(); ++j) {
                    file << spikeData[i][j] << " ";
                }
                file << std::endl;
            }
            file.close();
        }
    }
    
    void generateNetworkTopology(const Brain& brain) {
        auto topology = visualizer_->visualizeNetworkTopology(brain);
        
        std::ofstream file("output/network_topology.txt");
        if (file.is_open()) {
            file << "Network Topology:" << std::endl;
            file << "Nodes: " << topology["node_count"] << std::endl;
            file << "Edges: " << topology["edge_count"] << std::endl;
            file.close();
        }
    }
    
    void exportForExternalTools() {
        // Export data for external visualization tools
        std::ofstream file("output/export_data.json");
        if (file.is_open()) {
            file << "{\n    \"format\": \"NLM export\",\n"
                 << "    \"version\": \"0.5.0\",\n"
                 << "    \"timestamp\": \"" << time(nullptr) << "\"\n"
                 << "}";
            file.close();
        }
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<Visualizer> visualizer_;
};

int main() {
    // Create visualization pipeline
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    
    VisualizationPipeline pipeline(brain, world);
    
    // Generate comprehensive visualization
    pipeline.generateComprehensiveVisualization(500);
    
    return 0;
}
```

### Example 7: Performance Benchmarking

```cpp
#include <nlm.h>
#include <benchmark/benchmark.h>
#include <nlm/performance/PerformanceMonitor.hpp>

// Custom benchmark for LIF updates
static void BM_LIFUpdate(benchmark::State& state) {
    // Setup
    std::vector<Neuron> neurons(state.range(0));
    std::vector<float> currents(state.range(0), 1.0f);
    Timestamp time = 0.0f;
    float dt = 0.01f;
    
    // Benchmark
    for (auto _ : state) {
        for (size_t i = 0; i < neurons.size(); ++i) {
            neurons[i].stepLIF(time, dt);
        }
        time += dt;
    }
    
    state.SetItemsProcessed(state.range(0) * state.iterations());
}

// Custom benchmark for synaptic transmission
static void BM_SynapticTransmission(benchmark::State& state) {
    // Setup
    std::vector<Synapse> synapses(state.range(0));
    std::vector<Neuron> neurons(state.range(0));
    
    // Benchmark
    for (auto _ : state) {
        for (size_t i = 0; i < synapses.size(); ++i) {
            synapses[i].transmit(neurons);
        }
    }
    
    state.SetItemsProcessed(state.range(0) * state.iterations());
}

// Custom benchmark for memory pool operations
static void BM_MemoryPool(benchmark::State& state) {
    // Setup
    performance::PerformanceConfig perfConfig;
    perfConfig.maxNeurons = state.range(0);
    perfConfig.maxSynapses = state.range(0) * 10;
    
    auto monitor = std::make_shared<performance::PerformanceMonitor>(perfConfig);
    auto memoryPool = std::make_shared<performance::MemoryPool>(
        perfConfig.maxNeurons, perfConfig.maxSynapses
    );
    
    // Benchmark allocation/deallocation
    for (auto _ : state) {
        std::vector<NeuronId> neuronIds;
        for (size_t i = 0; i < state.range(0); ++i) {
            neuronIds.push_back(memoryPool->allocateNeuron());
        }
        
        for (const auto& id : neuronIds) {
            memoryPool->deallocateNeuron(id);
        }
    }
    
    state.SetItemsProcessed(state.range(0) * state.iterations());
}

BENCHMARK(BM_LIFUpdate)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_SynapticTransmission)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(BM_MemoryPool)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();
```

### Example 8: Python-C++ Integration

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <nlm.h>

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM Python Bindings
        -------------------
        Enhanced Python bindings for the NLM neural simulation framework.
        
        Features:
        - Automatic numpy array conversion
        - Session logging and debugging
        - Context manager support
        - Performance monitoring
        - Advanced visualization
        
        Examples:
        >>> import pynlm
        >>> config = pynlm.create_default_config()
        >>> config.set("simulation_steps", 1000)
        >>> brain = pynlm.create_brain(config)
        >>> brain.initialize()
    )pbdoc";

    // Python-specific utilities
    void formatPythonTraceback(py::object error) {
        try {
            auto traceback = py::module::import("traceback");
            auto format_exc = traceback.attr("format_exc");
            std::string formatted = py::str(format_exc());
            std::cerr << "=== Python Traceback ===" << std::endl;
            std::cerr << formatted << std::endl;
            std::cerr << "=== End Traceback ===" << std::endl;
        } catch (...) {
            std::cerr << "Error formatting traceback" << std::endl;
        }
    }

    // Register Python exception translator
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) {
                std::rethrow_exception(p);
            }
        } catch (const std::runtime_error& e) {
            formatPythonTraceback(py::none());
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Core classes
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value()); 
        });

    // Add more Python bindings as needed...
}
```

## Real-World Application Examples

### Use Case 1: Cognitive Development Simulation

```cpp
#include <nlm.h>
#include <iostream>
#include <map>

class CognitiveDevelopmentSimulator {
public:
    CognitiveDevelopmentSimulator() {
        // Create configuration for developmental simulation
        auto config = std::make_shared<Config>();
        config->set("simulation_steps", 10000);
        config->set("neuron_count", 500);
        config->set("enable_development", true);
        config->set("enable_reward_modulation", true);
        config->set("enable_curiosity", true);
        
        brain_ = std::make_shared<Brain>(config);
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(100, 100);
        
        agent_brain_ = std::make_shared<AgentBrain>(brain_, world_);
        agent_brain_->initialize(*world_);
    }
    
    void simulateDevelopment() {
        std::cout << "Simulating cognitive development over time..." << std::endl;
        
        std::map<DevelopmentalStage, std::vector<float>> stageMetrics;
        
        for (int step = 0; step < 10000; ++step) {
            // Get current developmental stage
            DevelopmentalStage stage = agent_brain_->getDevelopmentalStage();
            
            // Record metrics for this stage
            float neuromodulation = agent_brain_->getNeuromodulationLevel();
            float curiosity = agent_brain_->getCuriosityLevel();
            float novelty = agent_brain_->getNoveltyLevel();
            
            stageMetrics[stage].push_back(neuromodulation);
            
            // Apply developmental changes
            agent_brain_->updateDevelopment(step);
            
            // Step simulation
            brain_->step(step, step * 0.01f);
            
            // Print progress at key developmental stages
            if (stage == DevelopmentalStage::CriticalPeriod && step % 1000 == 0) {
                std::cout << "Critical period at step " << step << ": "
                         << "Neuromodulation=" << neuromodulation << ", "
                         << "Curiosity=" << curiosity << ", "
                         << "Novelty=" << novelty << std::endl;
            }
        }
        
        analyzeDevelopmentData(stageMetrics);
    }
    
    void analyzeDevelopmentData(const std::map<DevelopmentalStage, std::vector<float>>& metrics) {
        std::cout << "\nDevelopment Analysis:" << std::endl;
        
        for (const auto& [stage, values] : metrics) {
            if (values.empty()) continue;
            
            float avgNeuromodulation = 0.0f;
            for (float value : values) avgNeuromodulation += value;
            avgNeuromodulation /= values.size();
            
            std::cout << stageToString(stage) << ": "
                     << "Average neuromodulation=" << avgNeuromodulation << ", "
                     << "Samples=" << values.size() << std::endl;
        }
    }
    
    std::string stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_brain_;
};

int main() {
    CognitiveDevelopmentSimulator simulator;
    simulator.simulateDevelopment();
    
    return 0;
}
```

### Use Case 2: Working Memory and Episodic Memory

```cpp
#include <nlm.h>
#include <iostream>
#include <vector>

class MemoryBasedLearning {
public:
    MemoryBasedLearning() {
        auto config = std::make_shared<Config>();
        config->set("simulation_steps", 5000);
        
        brain_ = std::make_shared<Brain>(config);
        brain_->initialize();
    }
    
    void simulateMemoryLearning() {
        std::cout << "Simulating memory-based learning..." << std::endl;
        
        // Create sensory input patterns
        std::vector<Vision> patterns;
        
        // Pattern 1: Simple object
        Vision pattern1(10, 10, 3);
        std::vector<float> pattern1Data(300, 0.0f);
        // Create a simple square pattern
        for (int y = 3; y < 7; ++y) {
            for (int x = 3; x < 7; ++x) {
                size_t idx = y * 30 + x * 3;
                pattern1Data[idx] = 1.0f;     // Red
                pattern1Data[idx + 1] = 1.0f; // Green
                pattern1Data[idx + 2] = 1.0f; // Blue
            }
        }
        pattern1.setData(pattern1Data);
        patterns.push_back(pattern1);
        
        // Pattern 2: Different shape
        Vision pattern2(10, 10, 3);
        std::vector<float> pattern2Data(300, 0.0f);
        // Create a cross pattern
        for (int i = 0; i < 10; ++i) {
            size_t idx1 = i * 30 + 5 * 3;
            size_t idx2 = i * 30 + 15 * 3;
            pattern2Data[idx1] = 1.0f;     // Red vertical
            pattern2Data[idx2] = 1.0f;     // Red horizontal
        }
        pattern2.setData(pattern2Data);
        patterns.push_back(pattern2);
        
        // Learn patterns through repeated exposure
        for (int epoch = 0; epoch < 10; ++epoch) {
            std::cout << "Learning epoch " << epoch << "/" << 9 << std::endl;
            
            for (size_t patternIdx = 0; patternIdx < patterns.size(); ++patternIdx) {
                // Present pattern to brain
                brain_->receiveSensoryInput(patterns[patternIdx]);
                brain_->step(epoch * 10 + patternIdx, epoch * 10 + patternIdx);
                
                // Record neural activity
                auto activity = brain_->visualizeNeuronActivity();
                recordPatternActivity(patternIdx, epoch, activity);
                
                // Apply learning signals
                float reward = (epoch % 2 == 0) ? 1.0f : 0.5f;  // Variable reward
                applyLearningSignal(reward);
            }
            
            // Periodically test recall
            if (epoch % 3 == 0) {
                testPatternRecall(patterns);
            }
        }
        
        // Final analysis
        analyzeMemoryPerformance();
    }
    
    void recordPatternActivity(size_t patternIdx, int epoch, const std::vector<float>& activity) {
        // Store activity patterns for later analysis
        activityLog_[patternIdx].push_back(activity);
    }
    
    void applyLearningSignal(float reward) {
        // Apply reward-based learning
        // This would interface with the brain's plasticity systems
        // For demonstration, just record the reward
        learningSignals_.push_back(reward);
    }
    
    void testPatternRecall(const std::vector<Vision>& patterns) {
        std::cout << "Testing pattern recall..." << std::endl;
        
        for (size_t i = 0; i < patterns.size(); ++i) {
            // Present pattern and measure response
            brain_->receiveSensoryInput(patterns[i]);
            brain_->step(9999, 9999);
            
            auto activity = brain_->visualizeNeuronActivity();
            float similarity = calculatePatternSimilarity(activity, i);
            
            std::cout << "  Pattern " << i << ": Similarity = " << similarity << std::endl;
        }
    }
    
    float calculatePatternSimilarity(const std::vector<float>& currentActivity, size_t patternIdx) {
        if (activityLog_[patternIdx].empty()) {
            return 0.0f;
        }
        
        // Compare current activity with stored patterns
        float similarity = 0.0f;
        size_t count = 0;
        
        for (const auto& storedActivity : activityLog_[patternIdx]) {
            if (storedActivity.size() == currentActivity.size()) {
                float match = 0.0f;
                for (size_t i = 0; i < storedActivity.size(); ++i) {
                    match += std::abs(storedActivity[i] - currentActivity[i]);
                }
                similarity += 1.0f - (match / storedActivity.size());
                count++;
            }
        }
        
        return count > 0 ? (similarity / count) : 0.0f;
    }
    
    void analyzeMemoryPerformance() {
        std::cout << "\nMemory Performance Analysis:" << std::endl;
        
        std::cout << "Learning signals received: " << learningSignals_.size() << std::endl;
        std::cout << "Activity patterns recorded: " << activityLog_.size() << std::endl;
        
        for (size_t i = 0; i < activityLog_.size(); ++i) {
            std::cout << "Pattern " << i << ": " << activityLog_[i].size() << " recordings" << std::endl;
        }
        
        // Calculate average learning signal
        if (!learningSignals_.empty()) {
            float avgReward = 0.0f;
            for (float reward : learningSignals_) avgReward += reward;
            avgReward /= learningSignals_.size();
            std::cout << "Average learning signal: " << avgReward << std::endl;
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::map<size_t, std::vector<std::vector<float>>> activityLog_;
    std::vector<float> learningSignals_;
};

int main() {
    MemoryBasedLearning memoryLearner;
    memoryLearner.simulateMemoryLearning();
    
    return 0;
}
```

### Use Case 3: Social Learning and Imitation

```cpp
#include <nlm.h>
#include <iostream>
#include <vector>

class SocialLearningSimulator {
public:
    SocialLearningSimulator() {
        auto config = std::make_shared<Config>();
        config->set("simulation_steps", 8000);
        config->set("enable_social_learning", true);
        
        brain_ = std::make_shared<Brain>(config);
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(120, 120);
        
        agent_brain_ = std::make_shared<AgentBrain>(brain_, world_);
        agent_brain_->initialize(*world_);
    }
    
    void simulateSocialLearning() {
        std::cout << "Simulating social learning and imitation..." << std::endl;
        
        // Create multiple "agents" for social learning
        std::vector<std::shared_ptr<AgentBrain>> agents;
        agents.push_back(agent_brain_);
        
        for (int i = 1; i < 4; ++i) {
            auto otherBrain = std::make_shared<Brain>(std::make_shared<Config>(*agent_brain_->getConfiguration()));
            auto otherAgent = std::make_shared<AgentBrain>(otherBrain, world_);
            otherAgent->initialize(*world_);
            agents.push_back(otherAgent);
        }
        
        // Create social environment
        SocialLearningEnvironment env(agents);
        
        // Run social learning episodes
        for (int episode = 0; episode < 20; ++episode) {
            std::cout << "Social episode " << episode << "/" << 19 << std::endl;
            
            // Social phase: agents observe each other
            socialLearningPhase(agents, episode);
            
            // Imitation phase: agents imitate observed behaviors
            imitationPhase(agents, episode);
            
            // Testing phase: measure learning effectiveness
            if (episode % 5 == 0) {
                testLearningEffectiveness(agents, episode);
            }
        }
        
        analyzeSocialLearningResults(agents);
    }
    
    void socialLearningPhase(std::vector<std::shared_ptr<AgentBrain>>& agents, int episode) {
        std::cout << "  Social learning phase..." << std::endl;
        
        for (size_t i = 0; i < agents.size(); ++i) {
            for (size_t j = 0; j < agents.size(); ++j) {
                if (i != j) {
                    // Agent i observes agent j
                    observeOtherAgent(agents[i], agents[j], episode);
                }
            }
        }
    }
    
    void imitationPhase(std::vector<std::shared_ptr<AgentBrain>>& agents, int episode) {
        std::cout << "  Imitation phase..." << std::endl;
        
        for (size_t i = 0; i < agents.size(); ++i) {
            std::vector<Action> observedActions = getObservedActions(i);
            imitateActions(agents[i], observedActions, episode);
        }
    }
    
    void testLearningEffectiveness(std::vector<std::shared_ptr<AgentBrain>>& agents, int episode) {
        std::cout << "  Testing learning effectiveness..." << std::endl;
        
        for (size_t i = 0; i < agents.size(); ++i) {
            float effectiveness = calculateLearningEffectiveness(agents[i], episode);
            std::cout << "    Agent " << i << ": Effectiveness = " << effectiveness << std::endl;
        }
    }
    
    // Implementation details for observation, imitation, etc.
    // (Additional methods would be implemented here)
    
    void analyzeSocialLearningResults(std::vector<std::shared_ptr<AgentBrain>>& agents) {
        std::cout << "\nSocial Learning Analysis:" << std::endl;
        
        for (size_t i = 0; i < agents.size(); ++i) {
            auto brain = agents[i]->getBrain();
            std::cout << "Agent " << i << ": "
                     << "Neurons: " << brain->getTotalNeuronCount() << ", "
                     << "Spikes: " << brain->getTotalSpikeCount() << ", "
                     << "Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_brain_;
};

class SocialLearningEnvironment {
public:
    SocialLearningEnvironment(std::vector<std::shared_ptr<AgentBrain>> agents)
        : agents_(agents) {}
    
    void observeOtherAgent(std::shared_ptr<AgentBrain> observer, 
                          std::shared_ptr<AgentBrain> observed, 
                          int episode) {
        // Simulate observation of another agent's behavior
        std::vector<Action> observedActions = generateObservedActions(episode);
        storeObservedActions(observer, observedActions);
    }
    
    std::vector<Action> getObservedActions(size_t observerIndex) {
        // Return observed actions for a specific observer
        return observedActions_[observerIndex];
    }
    
    void storeObservedActions(std::shared_ptr<AgentBrain> agent, const std::vector<Action>& actions) {
        // Store observed actions for later imitation
        observedActions_[agent->getBrain()->getTotalNeuronCount()] = actions;
    }
    
    void imitateActions(std::shared_ptr<AgentBrain> agent, const std::vector<Action>& actions, int episode) {
        // Simulate imitation of observed actions
        // Implementation would involve matching actions and learning from them
    }
    
    float calculateLearningEffectiveness(std::shared_ptr<AgentBrain> agent, int episode) {
        // Calculate how well an agent has learned from observation
        return 0.75f;  // Placeholder
    }
    
    std::vector<Action> generateObservedActions(int episode) {
        // Generate example observed actions
        std::vector<Action> actions;
        actions.push_back(Action(ActionType::MoveForward));
        actions.push_back(Action(ActionType::TurnLeft));
        actions.push_back(Action(ActionType::Look));
        return actions;
    }
    
private:
    std::vector<std::shared_ptr<AgentBrain>> agents_;
    std::map<size_t, std::vector<Action>> observedActions_;
};

int main() {
    SocialLearningSimulator socialSimulator;
    socialSimulator.simulateSocialLearning();
    
    return 0;
}
```

## Python Examples

### Example 9: Python Vision Processing

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

# Create vision processing simulation
config = pynlm.create_brain_config("vision_processing", 2000)
brain = pynlm.create_brain(config)

brain.initialize()

# Create visual patterns
patterns = []

# Pattern 1: Edge detector
pattern1 = pynlm.create_vision(20, 20, 3)
edge_data = np.zeros((400,), dtype=np.float32)
# Create vertical edge
for x in [8, 9, 10, 11]:
    for y in range(20):
        idx = y * 20 + x
        edge_data[idx] = 1.0  # Red
        edge_data[idx + 400] = 0.5  # Green
        edge_data[idx + 800] = 0.0  # Blue

pattern1.set_data(edge_data)
patterns.append(("vertical_edge", pattern1))

# Pattern 2: Center blob
pattern2 = pynlm.create_vision(20, 20, 3)
blob_data = np.zeros((400,), dtype=np.float32)
# Create center blob
for y in range(6, 14):
    for x in range(6, 14):
        idx = y * 20 + x
        blob_data[idx] = 1.0  # Red
        blob_data[idx + 400] = 1.0  # Green
        blob_data[idx + 800] = 1.0  # Blue

pattern2.set_data(blob_data)
patterns.append(("center_blob", pattern2))

# Process patterns and measure responses
print("Processing visual patterns...")
for name, pattern in patterns:
    brain.receive_sensory_input(pattern)
    
    # Run simulation steps
    for step in range(50):
        brain.step(step, step * 0.01)
    
    # Visualize and save results
    activity = brain.visualize_neuron_activity()
    
    plt.figure()
    plt.imshow(activity.reshape(20, 20))
    plt.title(f"Neural Response: {name}")
    plt.savefig(f"results/{name}_response.png")
    plt.close()
    
    print(f"  Processed {name}: activity range [{activity.min():.3f}, {activity.max():.3f}]")

print("Vision processing completed!")
```

### Example 10: Python Agent Learning

```python
import pynlm
import numpy as np

# Create learning environment
config = pynlm.create_agent_config(energy=100.0, max_steps=1000)
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world(60, 60)

agent = pynlm.create_nlm_agent("learning_agent", brain, world, config)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)

print("Starting agent learning simulation...")

# Run learning episode
stats = agent.run_episode("learning_demo", 300)

print(f"Learning episode completed:")
print(f"  Total reward: {stats['total_reward']:.2f}")
print(f"  Success rate: {stats['success_rate'] * 100:.1f}%")
print(f"  Novel actions: {stats.get('novel_actions', 0)}")
print(f"  Final energy: {agent.get_energy():.1f}")

# Create visualization
import matplotlib.pyplot as plt
activity = brain.visualize_neuron_activity()

plt.figure(figsize=(10, 8))
plt.subplot(2, 2, 1)
plt.imshow(activity.reshape(20, 20), cmap='hot')
plt.title("Neural Activity Heatmap")

plt.subplot(2, 2, 2)
plt.plot(range(len(stats['actions'])), [a.get('reward', 0) for a in stats['actions']])
plt.title("Action Rewards")
plt.xlabel("Action Index")
plt.ylabel("Reward")

plt.subplot(2, 2, 3)
plt.hist([a.get('novelty', 0) for a in stats['actions']], bins=10)
plt.title("Action Novelty Distribution")

plt.subplot(2, 2, 4)
plt.plot(range(100), [0.1 * i for i in range(100)], 'g-', label='Ideal Learning')
plt.title("Learning Progress")
plt.xlabel("Episode")
plt.ylabel("Performance")

plt.tight_layout()
plt.savefig("results/learning_analysis.png", dpi=150)
print("Learning analysis saved to results/learning_analysis.png")
```

### Example 11: Python Performance Monitoring

```python
import pynlm
import numpy as np
import time

# Create performance-optimized brain
config = pynlm.create_brain_config("performance_test", 5000)
brain = pynlm.create_brain(config, thread_safe=True, precision_mode="double")
brain.initialize()

# Enable memory pool
brain.enable_memory_pool(True)

# Create performance monitoring tools
timer = pynlm.create_timer("simulation_step")

print("Starting performance benchmark...")

# Run benchmark
num_steps = 1000
step_times = []

start_time = time.time()
for step in range(num_steps):
    step_start = time.time()
    
    # Generate random sensory input
    vision = pynlm.create_vision(20, 20, 3)
    random_data = np.random.rand(1200).astype(np.float32)
    vision.set_data(random_data)
    
    # Process and step
    brain.receive_sensory_input(vision)
    brain.step(step, step * 0.01)
    
    step_time = time.time() - step_start
    step_times.append(step_time)
    
    if step % 200 == 0:
        print(f"Step {step}: {step_time*1000:.2f}ms")

end_time = time.time()

# Performance analysis
avg_step_time = np.mean(step_times)
std_step_time = np.std(step_times)
steps_per_second = num_steps / (end_time - start_time)

print(f"\nPerformance Results:")
print(f"  Total time: {end_time - start_time:.2f}s")
print(f"  Average step time: {avg_step_time*1000:.2f} ± {std_step_time*1000:.2f} ms")
print(f"  Steps per second: {steps_per_second:.1f}")
print(f"  Efficiency: {brain.get_average_firing_rate():.2f} firing rate")

# Export results
brain_state = pynlm.export_brain_state(brain)
np.save('performance_results.npy', brain_state)

print("Benchmark completed and results saved!")
```

### Example 12: Python Advanced Visualization

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Create advanced visualization demo
config = pynlm.create_brain_config("visualization_demo", 1000)
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world(50, 50)

visualizer = pynlm.create_visualizer(brain, world)

brain.initialize()

# Create animation
fig, axes = plt.subplots(2, 3, figsize=(15, 10))
fig.suptitle("NLM Advanced Visualization Demo", fontsize=16)

# Initialize plot elements
im0 = axes[0, 0].imshow(np.zeros((20, 20)), cmap='hot', interpolation='nearest')
axes[0, 0].set_title("Neural Activity Heatmap")

im1 = axes[0, 1].imshow(np.zeros((20, 20)), cmap='Blues', interpolation='nearest')
axes[0, 1].set_title("Connection Matrix")

im2 = axes[0, 2].imshow(np.zeros((20, 20)), cmap='viridis', interpolation='nearest')
axes[0, 2].set_title("Brain Regions")

ax3 = axes[1, 0].plot([], [], 'ro-', markersize=2, linewidth=1)
axes[1, 0].set_title("Spike Raster")
axes[1, 0].set_xlabel("Time")
axes[1, 0].set_ylabel("Neuron Index")

ax4 = axes[1, 1].plot([], [], 'b-', linewidth=2)
axes[1, 1].set_title("Learning Progress")
axes[1, 1].set_xlabel("Step")
axes[1, 1].set_ylabel("Performance")

im5 = axes[1, 2].imshow(np.zeros((50, 50)), cmap='gray', interpolation='nearest')
axes[1, 2].set_title("World View")

# Data buffers
time_data = []
performance_data = []
spike_data = []

# Animation update function
def update(frame):
    # Step simulation
    brain.step(frame, frame * 0.01)
    
    # Get visualization data
    activity = visualizer.visualize_neuron_activity(brain)
    connections = visualizer.visualize_synaptic_connections(brain)
    regions = visualizer.visualize_brain_regions(brain)
    
    # Update plots
    im0.set_data(activity.reshape(20, 20))
    im0.set_clim(activity.min(), activity.max())
    
    im1.set_data(connections)
    im1.set_clim(connections.min(), connections.max())
    
    im2.set_data(regions[:, :3].reshape(20, 20))
    im2.set_clim(regions.min(), regions.max())
    
    # Generate spike data
    spike_info = visualizer.generate_spike_raster(brain)
    if spike_info['total_spikes'] > 0:
        times = spike_info['spike_times']
        ax3.set_data(times, [spike_info['total_spikes']] * len(times))
    
    # Record performance
    performance = brain.get_average_firing_rate()
    time_data.append(frame)
    performance_data.append(performance)
    ax4.set_data(time_data, performance_data)
    ax4.relim()
    ax4.autoscale_view()
    
    # Update world view
    agent_body = world.get_agent_body()
    im5.set_data(np.zeros((50, 50)))
    
    return [im0, im1, im2, ax3, ax4, im5]

# Create animation
ani = FuncAnimation(fig, update, frames=range(100), interval=50, blit=False)

plt.tight_layout()

# Save animation
print("Creating visualization animation...")
ani.save("results/visualization_animation.gif", writer='pillow', fps=20)

print("Animation saved to results/visualization_animation.gif")

# Generate static visualizations
print("Generating static visualizations...")

# Create final visualization collection
fig2, axes2 = plt.subplots(2, 3, figsize=(18, 12))
fig2.suptitle("NLM Complete Visualization Suite", fontsize=18)

# Run simulation to get final data
for step in range(500):
    brain.step(step, step * 0.01)

# Generate all visualizations
activity = visualizer.visualize_neuron_activity(brain)
connections = visualizer.visualize_synaptic_connections(brain)
regions = visualizer.visualize_brain_regions(brain)
spike_info = visualizer.generate_spike_raster(brain)
topology = visualizer.visualize_network_topology(brain)

# Plot 1: Neural activity heatmap
axes2[0, 0].imshow(activity.reshape(20, 20), cmap='hot', interpolation='nearest')
axes2[0, 0].set_title("Final Neural Activity Heatmap")
axes2[0, 0].colorbar()

# Plot 2: Connection matrix
axes2[0, 1].imshow(connections, cmap='Blues', interpolation='nearest')
axes2[0, 1].set_title("Final Connection Matrix")
axes2[0, 1].colorbar()

# Plot 3: Brain regions
axes2[0, 2].imshow(regions[:, :3].reshape(20, 20), cmap='viridis', interpolation='nearest')
axes2[0, 2].set_title("Final Brain Regions")
axes2[0, 2].colorbar()

# Plot 4: Spike raster
axes2[1, 0].scatter(spike_info['spike_times'], 
                   [spike_info['total_spikes']] * len(spike_info['spike_times']), 
                   s=1, c='red', alpha=0.6)
axes2[1, 0].set_title(f"Spike Raster ({spike_info['total_spikes']} spikes)")
axes2[1, 0].set_xlabel("Time (ms)")
axes2[1, 0].set_ylabel("Neuron Index")

# Plot 5: Network topology
axes2[1, 1].scatter(topology['nodes'][:, 0], topology['nodes'][:, 1], 
                   c=topology['nodes'][:, 2], cmap='viridis', s=10)
axes2[1, 1].set_title(f"Network Topology ({topology['node_count']} nodes, {topology['edge_count']} edges)")
axes2[1, 1].set_xlabel("X Position")
axes2[1, 1].set_ylabel("Y Position")

# Plot 6: Learning curve
steps = list(range(500))
performance = [0.1 * i * (1 - i/500) for i in range(500)]  # Simulated learning curve
axes2[1, 2].plot(steps, performance, 'g-')
axes2[1, 2].set_title("Learning Curve")
axes2[1, 2].set_xlabel("Step")
axes2[1, 2].set_ylabel("Performance")
axes2[1, 2].set_ylim(0, 0.5)

plt.tight_layout()
plt.savefig("results/complete_visualization.png", dpi=150, bbox_inches='tight')

print("Complete visualization saved to results/complete_visualization.png")
```

## Running Examples

### Prerequisites

Before running examples, ensure you have:

- NLM framework built and installed
- Python 3.7+ with required packages
- CMake and C++ compiler

### Running C++ Examples

```bash
# Navigate to examples directory
cd examples

# Build examples
mkdir build
cd build
cmake ..
make

# Run specific example
./bin/simple_brain_example
./bin/vision_processing_example
./bin/agent_navigation_example
# ... other examples
```

### Running Python Examples

```bash
# Navigate to Python examples directory
cd python_examples

# Install required Python packages
pip install -r requirements.txt

# Run Python examples
python3 example_1_vision.py
python3 example_2_agent.py
python3 example_3_performance.py
python3 example_4_visualization.py
# ... other examples
```

### Running Benchmarking Examples

```bash
# Build benchmark suite
cd examples
cd benchmark
cmake ..
make

# Run benchmarks
./bin/benchmark_suite --benchmark_out=benchmark_results.json

# Analyze results
cat benchmark_results.json | python3 analyze_results.py
```

## Example Outputs

The examples generate various output files:

### Generated Files

- `results/simple_brain.bin` - Serialized brain state
- `results/vision_edge.png` - Vision processing visualization
- `results/agent_navigation.bin` - Navigation simulation results
- `results/learning_brain.bin` - Learning simulation results
- `results/visualization_animation.gif` - Visualization animation
- `results/complete_visualization.png` - Complete visualization suite
- `results/performance_results.npy` - Performance benchmark results
- `results/brain_state.npy` - Exported brain state

### Analysis Files

- `analysis/learning_curves.png` - Learning progress analysis
- `analysis/performance_metrics.txt` - Performance metrics
- `analysis/spike_statistics.csv` - Spike statistics
- `analysis/visualization_report.html` - Interactive visualization report

## Contributing Examples

### Adding New Examples

To add new examples:

1. Create a new example file in the appropriate directory (`examples/`, `python_examples/`, etc.)
2. Ensure the example demonstrates a specific feature or use case
3. Add comprehensive comments and documentation
4. Include performance considerations and optimization tips
5. Update the examples documentation

### Example Categories

The examples are organized into categories:

- **Basic**: Simple demonstrations of core functionality
- **Intermediate**: Examples showing advanced features and techniques
- **Real-world**: Application examples that solve specific problems
- **Benchmark**: Performance testing and benchmarking examples
- **Python**: Python-specific examples demonstrating bindings
- **Visualization**: Advanced visualization and analysis examples

### Example Quality Guidelines

When creating examples:

1. **Clarity**: Examples should be easy to understand and follow
2. **Completeness**: Include all necessary setup and cleanup
3. **Performance**: Consider performance implications
4. **Documentation**: Include comprehensive comments and documentation
5. **Error Handling**: Handle errors gracefully
6. **Modularity**: Break complex examples into smaller, focused examples
7. **Testing**: Include test code where appropriate
8. **Modularity**: Design examples to be reusable

## Example Usage Patterns

### Common Patterns

1. **Configuration Pattern**: Create and configure objects before use
2. **Context Manager Pattern**: Use context managers for resource management
3. **Factory Pattern**: Use factory functions to create standard components
4. **Observer Pattern**: Subscribe to events and handle changes
5. **Command Pattern**: Encapsulate actions for undo/redo functionality

### Anti-Patterns

Avoid these patterns in examples:

1. **Hard-coded Values**: Use configuration instead of hard-coded values
2. **Missing Error Handling**: Always handle errors gracefully
3. **Poor Variable Names**: Use descriptive variable names
4. **Unnecessary Complexity**: Keep examples simple and focused
5. **Missing Documentation**: Always include comments and documentation

## Conclusion

This document provides a comprehensive collection of examples for the enhanced NLM neural simulation framework. The examples cover:

- **Basic Examples**: Getting started with NLM
- **Intermediate Examples**: Understanding advanced features
- **Real-world Examples**: Solving practical problems
- **Benchmark Examples**: Performance testing and optimization
- **Python Examples**: Leveraging Python bindings
- **Visualization Examples**: Advanced visualization techniques

These examples serve as:

1. **Learning Tools**: Helping users understand NLM capabilities
2. **Development Guides**: Showing best practices for development
3. **Performance References**: Demonstrating optimization techniques
4. **Documentation**: Providing executable documentation
5. **Testing**: Serving as test cases for the framework

The examples are designed to be:
- **Accessible**: Easy for beginners to understand
- **Comprehensive**: Covering all major features
- **Practical**: Solving real-world problems
- **Performant**: Optimized for efficient execution
- **Documented**: Well-documented and commented
- **Modular**: Organized into logical categories

Happy experimenting with NLM examples!
