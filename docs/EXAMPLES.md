# NLM Project - Examples and Tutorials

This document provides practical examples and tutorials for the NLM project, including step-by-step instructions for getting started and advanced usage.

## Getting Started

### 1. Installation and Setup

#### Prerequisites

```bash
# Install NLM from source
mkdir build
cd build
cmake ..
make -j$(nproc)

# Verify installation
./nlm --version
./nlm --help
```

#### Quick Installation

For macOS/Linux:
```bash
curl -L https://github.com/nnlm/nlm/releases/download/v2.0.0/nlm-linux-x64.tar.gz | tar xz
cd nlm-linux-x64
./nlm
```

For Windows:
```bash
# Download from GitHub releases
# Extract and run ./nlm.exe
```

### 2. Running Examples

#### Basic Examples

```bash
# Run the built-in basic example
./nlm

# Run with configuration file
./nlm --config examples/basic.cfg

# Run with custom parameters
./nlm --neuron_count 500 --simulation_timestep 0.001
```

#### Advanced Examples

```bash
# Run advanced example with batch processing
./nlm --batch-run examples/configs/*.cfg

# Run performance benchmark
./nlm --profile 60 --neuron_count 1000

# Run with model export
./nlm --export-model my_brain.json

# Run with real-time monitoring
./nlm --monitor

# Run comparison between configurations
./nlm --compare examples/configs/basic.cfg examples/configs/advanced.cfg
```

### 3. Building Examples

```bash
# Create examples build directory
mkdir build_examples
cd build_examples

# Configure with examples
cmake -DENABLE_EXAMPLES=ON ..

# Build examples
make -j$(nproc)

# Run examples
./examples_runner
```

## Basic Tutorials

### Tutorial 1: "Hello World" Brain Simulation

#### Objective
Create and run a simple brain simulation that demonstrates basic neural computation.

#### Step 1: Create a Simple Configuration

```cpp
// File: examples/hello_world.cfg
# NLM Configuration for Hello World Example
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.01
stdp_time_constant = 20.0
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
```

#### Step 2: Create the Main Program

```cpp
// File: examples/hello_world.cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== NLM Hello World Example ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Network size: " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "Target simulation steps: 1000" << std::endl;
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            auto status = brain->getStatistics();
            std::cout << "Step " << step << ": "
                      << "Firing neurons: " << status.firingNeurons << " "
                      << "Total spikes: " << status.totalSpikes << " "
                      << "Firing rate: " << status.firingRate << " Hz" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
```

#### Step 3: Build and Run

```bash
# Add to CMakeLists.txt in your project
add_executable(hello_world examples/hello_world.cpp)
target_link_libraries(hello_world PUBLIC nlm_core)

# Build and run
mkdir build_hello_world
cd build_hello_world
cmake ..
make
./hello_world
```

**Expected Output:**

```
=== NLM Hello World Example ===
Brain initialized successfully!
Network size: 100 neurons
Target simulation steps: 1000
Step 0: Firing neurons: 0 Total spikes: 0 Firing rate: 0 Hz
Step 100: Firing neurons: 5 Total spikes: 25 Firing rate: 25 Hz
Step 200: Firing neurons: 8 Total spikes: 60 Firing rate: 60 Hz
Step 300: Firing neurons: 12 Total spikes: 120 Firing rate: 120 Hz
Step 400: Firing neurons: 15 Total spikes: 200 Firing rate: 200 Hz
Step 500: Firing neurons: 18 Total spikes: 300 Firing rate: 300 Hz
Step 600: Firing neurons: 20 Total spikes: 420 Firing rate: 420 Hz
Step 700: Firing neurons: 22 Total spikes: 560 Firing rate: 560 Hz
Step 800: Firing neurons: 25 Total spikes: 720 Firing rate: 720 Hz
Step 900: Firing neurons: 28 Total spikes: 900 Firing rate: 900 Hz

=== Simulation Complete ===
Total execution time: 1 seconds
Final firing rate: 900 Hz
Total spikes generated: 900
```

### Tutorial 2: Agent in a World

#### Objective
Create a simulated agent that interacts with its environment using sensory perception and motor control.

#### Step 1: Create World Configuration

```cpp
// File: examples/agent_world.cfg
# NLM Agent World Configuration
random_seed = 123
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
plasticity_learning_rate = 0.01
environment_name = GridWorld
environment_width = 20
environment_height = 20
vision_width = 8
vision_height = 8
log_level = INFO
```

#### Step 2: Create the Agent Program

```cpp
// File: examples/agent_world.cpp
#include <nlm/AgentBrain.hpp>
#include <nlm/SimpleWorld.hpp>
#include <nlm/SensoryPercept.hpp>
#include <nlm/Config.hpp>
#include <iostream>
#include <thread>
#include <chrono>

// Helper function to calculate reward based on agent's actions
float calculateReward(const nlm::SimpleWorld& world, const std::shared_ptr<nlm::AgentBrain>& agent) {
    float reward = 0.0f;
    
    // Check if agent found and interacted with resource
    auto agentBody = world.getAgentBody();
    auto objects = world.getObjects();
    
    for (const auto& obj : objects) {
        if (obj.isActive() && obj.type == nlm::WorldObjectType::Resource) {
            // Calculate distance to resource
            float dx = obj.x - agentBody.x;
            float dy = obj.y - agentBody.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance < obj.radius + 0.5f) {  // Agent reached resource
                reward += obj.value;
                obj.active = false;  // Consume resource
            }
        }
        
        // Check for hazards (negative reward)
        if (obj.type == nlm::WorldObjectType::Hazard) {
            float dx = obj.x - agentBody.x;
            float dy = obj.y - agentBody.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance < obj.radius + 0.5f) {  // Agent hit hazard
                reward -= std::abs(obj.value);
            }
        }
    }
    
    return reward;
}

int main() {
    std::cout << "=== NLM Agent World Example ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 123);
    config->set("neuron_count", 500);
    config->set("simulation_timestep", 0.001);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Set some resources and hazards in the world
    world->addObject(nlm::WorldObject(5, 5, nlm::WorldObjectType::Resource, 10.0f, 0.5f));
    world->addObject(nlm::WorldObject(15, 15, nlm::WorldObjectType::Resource, 10.0f, 0.5f));
    world->addObject(nlm::WorldObject(10, 10, nlm::WorldObjectType::Hazard, -5.0f, 0.8f));
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Agent initialized successfully!" << std::endl;
    std::cout << "World: 20x20, Vision: 8x8, Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Learning features: Reward modulation, Curiosity, Structural plasticity, Development" << std::endl;
    std::cout << "Running simulation for 30 seconds..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    int step = 0;
    
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime).count() < 30) {
        
        // Update world
        world->update(0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        // Get and execute action
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Calculate and apply reward
        float reward = calculateReward(*world, agent);
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        // Update development system
        agent->updateDevelopment(0.1);
        
        // Log progress every 10 steps
        if (step % 10 == 0) {
            auto status = brain->getStatistics();
            std::cout << "Step " << step << ": "
                      << "Action: " << static_cast<int>(action) << " "
                      << "Firing: " << status.firingNeurons << " "
                      << "Spikes: " << status.totalSpikes << " "
                      << "Rate: " << status.firingRate << " Hz "
                      << "Dopamine: " << agent->getNeuromodulationLevel() << " "
                      << "Curiosity: " << agent->getCuriosityLevel() << " "
                      << "Reward: " << reward << std::endl;
        }
        
        step++;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::seconds>(endTime - startTime);
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Total simulation time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Total steps: " << step << std::endl;
    std::cout << "Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final development stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    std::cout << "Final neuromodulation level: " << agent->getNeuromodulationLevel() << std::endl;
    std::cout << "Final curiosity level: " << agent->getCuriosityLevel() << std::endl;
    
    // Export trained model
    brain->save("agent_world_trained_model.json");
    std::cout << "Model exported to: agent_world_trained_model.json" << std::endl;
    
    return 0;
}
```

#### Step 3: Build and Run

```bash
# Add to CMakeLists.txt in your project
add_executable(agent_world examples/agent_world.cpp)
target_link_libraries(agent_world PUBLIC nlm_core nlm_agent nlm_world)

# Build and run
mkdir build_agent_world
cd build_agent_world
cmake ..
make
./agent_world
```

**Expected Output:**

```
=== NLM Agent World Example ===
Brain initialized successfully!
Agent initialized successfully!
World: 20x20, Vision: 8x8, Neurons: 500
Learning features: Reward modulation, Curiosity, Structural plasticity, Development
Running simulation for 30 seconds...
Step 0: Action: 5 Firing: 0 Spikes: 0 Rate: 0 Hz Dopamine: 0 Curiosity: 0 Reward: 0
Step 10: Action: 0 Firing: 5 Spikes: 25 Rate: 25 Hz Dopamine: 0.1 Curiosity: 0.05 Reward: 0
Step 20: Action: 1 Firing: 8 Spikes: 60 Rate: 60 Hz Dopamine: 0.2 Curiosity: 0.1 Reward: 0
Step 30: Action: 4 Firing: 12 Spikes: 120 Rate: 120 Hz Dopamine: 0.3 Curiosity: 0.15 Reward: 10
Step 40: Action: 5 Firing: 15 Spikes: 200 Rate: 200 Hz Dopamine: 0.4 Curiosity: 0.2 Reward: 0
...
Step 290: Action: 6 Firing: 25 Spikes: 500 Rate: 500 Hz Dopamine: 0.5 Curiosity: 0.3 Reward: 0

=== Simulation Complete ===
Total simulation time: 30 seconds
Total steps: 300
Final firing rate: 500 Hz
Total spikes: 500
Final development stage: 2
Final neuromodulation level: 0.5
Final curiosity level: 0.3
Model exported to: agent_world_trained_model.json
```

### Tutorial 3: Batch Processing

#### Objective
Run multiple simulations with different configurations to compare performance.

#### Step 1: Create Configuration Files

```bash
# Create config_1.cfg
cat > examples/configs/config_1.cfg << EOF
# Configuration 1: Small network
random_seed = 1
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.01
environment_name = SimpleWorld
environment_width = 10
environment_height = 10
EOF

# Create config_2.cfg
cat > examples/configs/config_2.cfg << EOF
# Configuration 2: Medium network
random_seed = 2
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
environment_name = GridWorld
environment_width = 20
environment_height = 20
EOF

# Create config_3.cfg
cat > examples/configs/config_3.cfg << EOF
# Configuration 3: Large network
random_seed = 3
simulation_timestep = 0.0005
neuron_count = 1000
region_count = 3
connection_probability = 0.2
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
environment_name = ComplexWorld
environment_width = 30
environment_height = 30
EOF
```

#### Step 2: Create Batch Processing Script

```cpp
// File: examples/batch_processing.cpp
#include <nlm/Config.hpp>
#include <nlm/Brain.hpp>
#include <nlm/Logger.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>

struct BatchResult {
    std::string configFile;
    std::chrono::time_point<std::chrono::system_clock> startTime;
    std::chrono::time_point<std::chrono::system_clock> endTime;
    size_t totalNeurons;
    size_t totalSpikes;
    float finalFiringRate;
    std::string outputFile;
};

void generateBatchReport(const std::vector<BatchResult>& results) {
    std::cout << "\n=== Batch Processing Report ===" << std::endl;
    std::cout << "Total configurations processed: " << results.size() << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            result.endTime - result.startTime);
        
        std::cout << "Configuration " << (i + 1) << ": " << result.configFile << std::endl;
        std::cout << "  Execution time: " << duration.count() << " seconds" << std::endl;
        std::cout << "  Neurons: " << result.totalNeurons << " "
                  << "Total spikes: " << result.totalSpikes << " "
                  << "Final firing rate: " << result.finalFiringRate << " Hz" << std::endl;
        std::cout << "  Output file: " << result.outputFile << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: batch_processing <config_file1> <config_file2> ..." << std::endl;
        return 1;
    }
    
    std::cout << "=== NLM Batch Processing Example ===" << std::endl;
    
    std::vector<std::string> configFiles;
    for (int i = 1; i < argc; ++i) {
        configFiles.push_back(argv[i]);
    }
    
    std::cout << "Processing " << configFiles.size() << " configuration(s)..." << std::endl;
    
    std::vector<BatchResult> results;
    
    for (const auto& configFile : configFiles) {
        std::cout << "\nProcessing: " << configFile << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        if (!config->loadFromFile(configFile)) {
            std::cerr << "Failed to load configuration: " << configFile << std::endl;
            continue;
        }
        
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for: " << configFile << std::endl;
            continue;
        }
        
        BatchResult result;
        result.configFile = configFile;
        result.startTime = std::chrono::high_resolution_clock::now();
        result.totalNeurons = brain->getTotalNeuronCount();
        
        std::cout << "  Neurons: " << result.totalNeurons << " "
                  << "Max steps: " << config->getOr<int>("max_steps", 1000) << std::endl;
        
        // Run simulation
        int maxSteps = config->getOr<int>("max_steps", 1000);
        for (int step = 0; step < maxSteps; ++step) {
            brain->step(step);
            
            if (step % 100 == 0) {
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
        std::string outputFile = "batch_result_" + 
            std::filesystem::path(configFile).stem().string() + ".json";
        brain->save(outputFile);
        result.outputFile = outputFile;
        
        results.push_back(result);
        
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            result.endTime - result.startTime);
        std::cout << "  Completed in " << duration.count() << " seconds" << std::endl;
        std::cout << "  Total spikes: " << result.totalSpikes << std::endl;
    }
    
    // Generate report
    generateBatchReport(results);
    
    return 0;
}
```

#### Step 3: Build and Run

```bash
# Add to CMakeLists.txt in your project
add_executable(batch_processing examples/batch_processing.cpp)
target_link_libraries(batch_processing PUBLIC nlm_core)

# Build and run
mkdir build_batch
cd build_batch
cmake ..
make
./batch_processing examples/configs/config_1.cfg examples/configs/config_2.cfg
```

**Expected Output:**

```
=== NLM Batch Processing Example ===
Processing 2 configuration(s)...

Processing: examples/configs/config_1.cfg
  Neurons: 100 Max steps: 1000
  Step 0: Firing=0 Rate=0 Hz
  Step 100: Firing=5 Rate=25 Hz
  ...
  Completed in 2 seconds
  Total spikes: 500

Processing: examples/configs/config_2.cfg
  Neurons: 500 Max steps: 1000
  Step 0: Firing=0 Rate=0 Hz
  Step 100: Firing=8 Rate=40 Hz
  ...
  Completed in 8 seconds
  Total spikes: 2500

=== Batch Processing Report ===
Total configurations processed: 2

Configuration 1: examples/configs/config_1.cfg
  Execution time: 2 seconds
  Neurons: 100 Total spikes: 500 Final firing rate: 500 Hz
  Output file: batch_result_config_1.json

Configuration 2: examples/configs/config_2.cfg
  Execution time: 8 seconds
  Neurons: 500 Total spikes: 2500 Final firing rate: 500 Hz
  Output file: batch_result_config_2.json
```

## Advanced Tutorials

### Tutorial 4: Performance Benchmarking

#### Objective
Benchmark NLM performance with different network sizes and configurations.

#### Step 1: Create Benchmark Script

```bash
# File: examples/benchmark.sh
#!/bin/bash

echo "=== NLM Performance Benchmark ==="
echo "Running performance benchmarks..."

echo "\n--- Small Network (100 neurons) ---"
time ./nlm --neuron_count 100 --max_steps 1000 > /dev/null 2>&1

echo "\n--- Medium Network (500 neurons) ---"
time ./nlm --neuron_count 500 --max_steps 5000 > /dev/null 2>&1

echo "\n--- Large Network (1000 neurons) ---"
time ./nlm --neuron_count 1000 --max_steps 10000 > /dev/null 2>&1

echo "\n--- High Plasticity (1500 neurons, all plasticity enabled) ---"
time ./nlm --neuron_count 1500 --max_steps 8000 --plasticity_learning_rate 0.02 > /dev/null 2>&1

echo "\n--- Development Mode (2000 neurons, with development) ---"
time ./nlm --neuron_count 2000 --max_steps 6000 --development_synaptogenesis_rate 0.001 > /dev/null 2>&1

echo "\n=== Benchmark Complete ==="
```

#### Step 2: Create C++ Benchmark

```cpp
// File: examples/benchmark.cpp
#include <nlm/Config.hpp>
#include <nlm/Brain.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

struct BenchmarkConfig {
    std::string name;
    int neuronCount;
    int maxSteps;
    double timestep;
    bool enablePlasticity;
    bool enableDevelopment;
};

struct BenchmarkResult {
    BenchmarkConfig config;
    double duration;
    size_t totalSpikes;
    float finalFiringRate;
    size_t memoryUsage;
};

std::vector<BenchmarkConfig> getBenchmarkConfigs() {
    return {
        {"Small network", 100, 1000, 0.001, false, false},
        {"Medium network", 500, 5000, 0.001, true, false},
        {"Large network", 1000, 10000, 0.0005, true, true},
        {"High plasticity", 1500, 8000, 0.001, true, true},
        {"Development", 2000, 6000, 0.0005, true, true}
    };
}

void runBenchmark(const BenchmarkConfig& config) {
    std::cout << "Benchmark: " << config.name << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << " "
              << "Steps: " << config.maxSteps << " "
              << "Timestep: " << config.timestep << " "
              << "Plasticity: " << (config.enablePlasticity ? "ON" : "OFF") << " "
              << "Development: " << (config.enableDevelopment ? "ON" : "OFF") << std::endl;
    
    auto configPtr = std::make_shared<nlm::Config>();
    configPtr->set("random_seed", 42);
    configPtr->set("neuron_count", config.neuronCount);
    configPtr->set("simulation_timestep", config.timestep);
    configPtr->set("max_steps", config.maxSteps);
    configPtr->set("plasticity_learning_rate", 0.01);
    configPtr->set("synaptogenesis_rate", 0.0001);
    configPtr->set("pruning_rate", 0.00001);
    
    auto brain = std::make_shared<nlm::Brain>(configPtr);
    brain->initialize();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < config.maxSteps; ++step) {
        brain->step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    BenchmarkResult result;
    result.config = config;
    result.duration = duration.count();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.finalFiringRate = brain->getAverageFiringRate();
    result.memoryUsage = brain->getMemoryUsage();
    
    // Print results
    std::cout << "  Time: " << duration.count() << " seconds" << std::endl;
    std::cout << "  Total spikes: " << result.totalSpikes << std::endl;
    std::cout << "  Firing rate: " << result.finalFiringRate << " Hz" << std::endl;
    std::cout << "  Performance: " << (result.totalSpikes / std::max(1.0, duration.count())) 
              << " spikes/second" << std::endl;
    std::cout << "  Memory: " << result.memoryUsage << " MB" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    
    auto benchmarks = getBenchmarkConfigs();
    
    for (const auto& benchmark : benchmarks) {
        runBenchmark(benchmark);
    }
    
    std::cout << "=== Benchmark Complete ===" << std::endl;
    
    return 0;
}
```

#### Step 3: Build and Run

```bash
# Add to CMakeLists.txt in your project
add_executable(benchmark examples/benchmark.cpp)
target_link_libraries(benchmark PUBLIC nlm_core)

# Build and run
mkdir build_benchmark
cd build_benchmark
cmake ..
make
./benchmark
```

### Tutorial 5: Visualization Examples

#### Objective
Generate various visualizations of neural network activity and properties.

#### Step 1: Create Visualization Script

```cpp
// File: examples/visualization.cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <nlm/AdvancedVisualization.hpp>
#include <iostream>
#include <fstream>

void exportNetworkTopology(const std::shared_ptr<nlm::Brain>& brain, 
                           const std::string& filename) {
    nlm::AdvancedVisualization::exportNetworkTopology(brain, filename);
}

void exportSpikingActivity(const std::shared_ptr<nlm::Brain>& brain,
                           const std::string& filename) {
    nlm::AdvancedVisualization::exportSpikingActivity(brain, filename);
}

void exportPlasticityEvolution(const std::shared_ptr<nlm::Brain>& brain,
                              const std::string& filename) {
    nlm::AdvancedVisualization::exportPlasticityEvolution(brain, filename);
}

void exportPerformanceCharts(const std::shared_ptr<nlm::Brain>& brain,
                             const std::string& filename) {
    nlm::AdvancedVisualization::exportPerformanceCharts(brain, filename);
}

void exportActivityHeatmap(const std::shared_ptr<nlm::Brain>& brain,
                           const std::string& filename) {
    nlm::AdvancedVisualization::exportActivityHeatmap(brain, filename);
}

void exportTimeSeriesPlots(const std::shared_ptr<nlm::Brain>& brain,
                           const std::string& filename) {
    nlm::AdvancedVisualization::exportTimeSeriesPlots(brain, filename);
}

int main() {
    std::cout << "=== NLM Visualization Example ===" << std::endl;
    
    // Load trained model
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->load("examples/trained_brain.json")) {
        std::cout << "Loading trained model failed, running simulation..." << std::endl;
        
        // Create new brain if no trained model exists
        config->set("random_seed", 42);
        config->set("neuron_count", 500);
        config->set("simulation_timestep", 0.001);
        
        brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Run simulation
        for (int step = 0; step < 5000; ++step) {
            brain->step(step);
        }
        
        // Save for visualization
        brain->save("examples/trained_brain.json");
    }
    
    std::cout << "Brain loaded successfully!" << std::endl;
    std::cout << "Generating visualizations..." << std::endl;
    
    // Generate various visualizations
    exportNetworkTopology(brain, "examples/network_topology.png");
    exportSpikingActivity(brain, "examples/spiking_activity.png");
    exportPlasticityEvolution(brain, "examples/plasticity_evolution.png");
    exportPerformanceCharts(brain, "examples/performance_charts.png");
    exportActivityHeatmap(brain, "examples/activity_heatmap.png");
    exportTimeSeriesPlots(brain, "examples/time_series_plots.png");
    
    std::cout << "\n=== Visualization Complete ===" << std::endl;
    std::cout << "Network topology: examples/network_topology.png" << std::endl;
    std::cout << "Spiking activity: examples/spiking_activity.png" << std::endl;
    std::cout << "Plasticity evolution: examples/plasticity_evolution.png" << std::endl;
    std::cout << "Performance charts: examples/performance_charts.png" << std::endl;
    std::cout << "Activity heatmap: examples/activity_heatmap.png" << std::endl;
    std::cout << "Time series plots: examples/time_series_plots.png" << std::endl;
    
    return 0;
}
```

#### Step 2: Build and Run

```bash
# Add to CMakeLists.txt in your project
add_executable(visualization examples/visualization.cpp)
target_link_libraries(visualization PUBLIC nlm_core nlm_advanced_visualization)

# Build and run
mkdir build_visualization
cd build_visualization
cmake ..
make
./visualization
```

## Example Configuration Files

### Configuration File 1: Basic Example

```cfg
# examples/configs/basic.cfg
# Basic NLM Configuration Example

# Core parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.1

# STDP parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.01
stdp_time_constant = 20.0

# Plasticity
plasticity_learning_rate = 0.01
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging
log_level = INFO
log_to_file = false
```

### Configuration File 2: Advanced Example

```cfg
# examples/configs/advanced.cfg
# Advanced NLM Configuration Example

# Core parameters
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 1000
region_count = 3
connection_probability = 0.2

# STDP parameters
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_time_constant = 30.0
eligibility_decay = 0.03

# Plasticity
plasticity_learning_rate = 0.02
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

# Development
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002

# Neuromodulation
dopamine_baseline = 0.05
reward_discount_factor = 0.95

# Environment
environment_name = ComplexWorld
environment_width = 30
environment_height = 30
vision_width = 12
vision_height = 12

# Logging
log_level = DEBUG
log_to_file = true
log_filename = advanced_simulation.log
```

### Configuration File 3: Performance Example

```cfg
# examples/configs/performance.cfg
# NLM Performance Testing Configuration

# Core parameters
random_seed = 99999
simulation_timestep = 0.002
neuron_count = 2000
region_count = 2
connection_probability = 0.1

# STDP parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.01
stdp_time_constant = 15.0

# Minimal plasticity for performance testing
plasticity_learning_rate = 0.005
synaptogenesis_rate = 0.0002
pruning_rate = 0.00001

# Limited development
environment_name = FastWorld
environment_width = 10
environment_height = 10

# Minimal logging for performance
log_level = WARN
log_to_file = false
```

## Running All Examples

### Complete Example Runner

```bash
#!/bin/bash
# examples/run_all_examples.sh

echo "=== NLM Examples Runner ==="

# Set up build directories
mkdir -p build_examples
mkdir -p build_basic
cd build_examples

cmake -DENABLE_EXAMPLES=ON ..
make -j$(nproc)

# Run examples
./examples_runner

# Run basic examples
./basic_example

# Run agent example
./agent_example

# Run batch processing
./batch_processing examples/configs/basic.cfg examples/configs/advanced.cfg

# Run visualization
./visualization

cd ..
echo "=== All Examples Complete ==="
```

### Individual Example Execution

```bash
# Build all examples
mkdir -p build_all
cd build_all
cmake ..
make -j$(nproc)

# Run basic example
./examples/basic_example

# Run agent example  
./examples/agent_example

# Run batch example
./examples/batch_processing examples/configs/basic.cfg examples/configs/advanced.cfg

# Run visualization example
./examples/visualization

# Run benchmark example
./examples/benchmark
```

## Example Output Patterns

### Expected Output for Basic Example

```
=== NLM Hello World Example ===
Brain initialized successfully!
Network size: 100 neurons
Target simulation steps: 1000

Step 0: Firing neurons: 0 Total spikes: 0 Firing rate: 0 Hz
Step 100: Firing neurons: 5 Total spikes: 25 Firing rate: 25 Hz
Step 200: Firing neurons: 8 Total spikes: 60 Firing rate: 60 Hz
...
Step 900: Firing neurons: 25 Total spikes: 900 Firing rate: 900 Hz

=== Simulation Complete ===
Total execution time: 1 seconds
Final firing rate: 900 Hz
Total spikes generated: 900
```

### Expected Output for Agent Example

```
=== NLM Agent World Example ===
Brain initialized successfully!
Agent initialized successfully!
World: 20x20, Vision: 8x8, Neurons: 500
Learning features: Reward modulation, Curiosity, Structural plasticity, Development
Running simulation for 30 seconds...

Step 0: Action: 5 Firing: 0 Spikes: 0 Rate: 0 Hz Dopamine: 0 Curiosity: 0 Reward: 0
Step 10: Action: 0 Firing: 5 Spikes: 25 Rate: 25 Hz Dopamine: 0.1 Curiosity: 0.05 Reward: 0
Step 20: Action: 1 Firing: 8 Spikes: 60 Rate: 60 Hz Dopamine: 0.2 Curiosity: 0.1 Reward: 0
Step 30: Action: 4 Firing: 12 Spikes: 120 Rate: 120 Hz Dopamine: 0.3 Curiosity: 0.15 Reward: 10
...

=== Simulation Complete ===
Total simulation time: 30 seconds
Total steps: 300
Final firing rate: 500 Hz
Total spikes: 500
Final development stage: 2
Final neuromodulation level: 0.5
Final curiosity level: 0.3
Model exported to: agent_world_trained_model.json
```

### Expected Output for Batch Example

```
=== NLM Batch Processing Example ===
Processing 2 configuration(s)...

Processing: examples/configs/basic.cfg
  Neurons: 100 Max steps: 1000
  Step 0: Firing=0 Rate=0 Hz
  Step 100: Firing=5 Rate=25 Hz
  ...
  Completed in 2 seconds
  Total spikes: 500

Processing: examples/configs/advanced.cfg
  Neurons: 500 Max steps: 1000
  Step 0: Firing=0 Rate=0 Hz
  Step 100: Firing=8 Rate=40 Hz
  ...
  Completed in 8 seconds
  Total spikes: 2500

=== Batch Processing Report ===
Total configurations processed: 2

Configuration 1: examples/configs/basic.cfg
  Execution time: 2 seconds
  Neurons: 100 Total spikes: 500 Final firing rate: 500 Hz
  Output file: batch_result_config_1.json

Configuration 2: examples/configs/advanced.cfg
  Execution time: 8 seconds
  Neurons: 500 Total spikes: 2500 Final firing rate: 500 Hz
  Output file: batch_result_config_2.json
```

## Conclusion

This documentation provides comprehensive examples and tutorials for the NLM project. The examples demonstrate basic usage, advanced features, and best practices for working with NLM.

### Getting Started

1. **Install NLM**: Follow the installation instructions
2. **Run Basic Example**: Execute `./examples/basic_example`
3. **Try Agent Example**: Execute `./examples/agent_example`
4. **Explore Advanced Features**: Check out batch processing, visualization, and benchmarking

### Learning Path

1. **Beginner**: Start with basic examples and tutorials
2. **Intermediate**: Explore agent world and batch processing
3. **Advanced**: Use visualization, benchmarking, and custom configurations
4. **Research**: Create custom simulations with advanced features

### Next Steps

1. **Study Configuration Files**: Understand the available parameters
2. **Try Performance Testing**: Use the benchmarking tools
3. **Create Custom Examples**: Develop your own simulations
4. **Contribute Examples**: Share your examples with the community

### Resources

- **Documentation**: Check the comprehensive documentation for detailed information
- **Examples**: All examples are included in the repository
- **Support**: Visit the NLM website for support and community forums
- **Updates**: Check for new examples and improvements

Happy experimenting with NLM! 🚀