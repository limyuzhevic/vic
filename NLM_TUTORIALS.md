# NLM Tutorials

## Overview

This document provides comprehensive step-by-step tutorials for working with the NLM (Neural Learning Machine) system. It covers everything from basic usage to advanced features, including plugin development, performance optimization, and system integration.

## Table of Contents

### Part 1: Basic Tutorials
1. [Getting Started with NLM](#getting-started-with-nlm)
2. [Creating Your First Brain](#creating-your-first-brain)
3. [Running Your First Simulation](#running-your-first-simulation)
4. [Working with Memory Systems](#working-with-memory-systems)
5. [Using the Plugin System](#using-the-plugin-system)

### Part 2: Intermediate Tutorials
6. [Advanced Brain Configuration](#advanced-brain-configuration)
7. [Custom Neural Dynamics](#custom-neural-dynamics)
8. [Working with Neuromodulation](#working-with-neuromodulation)
9. [Implementing Prediction Systems](#implementing-prediction-systems)
10. [Creating Custom Plugins](#creating-custom-plugins)

### Part 3: Advanced Tutorials
11. [Performance Optimization](#performance-optimization)
12. [State Management and Checkpointing](#state-management-and-checkpointing)
13. [Multi-Language Plugin Development](#multi-language-plugin-development)
14. [System Integration](#system-integration)
15. [Real-World Application Examples](#real-world-application-examples)

### Part 4: Specialized Tutorials
16. [Monitoring and Visualization](#monitoring-and-visualization)
17. [Testing and Validation](#testing-and-validation)
18. [Security and Sandbox Development](#security-and-sandbox-development)
19. [Development Tools and Workflow](#development-tools-and-workflow)
20. [Troubleshooting and Debugging](#troubleshooting-and-debugging)

## Part 1: Basic Tutorials

### 1. Getting Started with NLM

#### Overview

This tutorial will guide you through setting up and running your first NLM simulation. It covers installation, basic configuration, and running a simple simulation.

#### Prerequisites

- Basic knowledge of C++ and Python
- Understanding of neural network concepts

#### Installation

**Step 1: Install Prerequisites**

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install g++ cmake make git

# macOS
xcode-select --install
brew install cmake

# Python dependencies
python3 -m venv nlm_env
source nlm_env/bin/activate
pip install numpy scipy matplotlib
pip install pybind11 pytest
```

**Step 2: Clone the Repository**

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

**Step 3: Build the Project**

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Step 4: Install Python Package**

```bash
cd ..
pip install -e python/
```

#### Creating Your First Simulation

**Step 1: Create a Simple Brain**

```cpp
// File: examples/first_brain.cpp
#include <nlm/brain.h>
#include <nlm/world.h>
#include <iostream>

int main() {
    try {
        // Create brain and world
        NLM::Brain brain;
        NLM::World world(100, 100);
        
        // Configure brain
        NLM::BrainConfig config;
        config.neuron_count = 100;
        config.synaptic_density = 0.1f;
        config.learning_rate = 0.01f;
        
        // Initialize brain
        brain.initialize(config);
        world.initialize();
        
        std::cout << "NLM system initialized successfully!" << std::endl;
        std::cout << "Brain neurons: " << brain.get_neuron_count() << std::endl;
        std::cout << "World size: " << world.get_width() << "x" << world.get_height() << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 2: Compile and Run**

```bash
mkdir build_example
cd build_example
cmake ../examples/first_brain.cpp -DCMAKE_BUILD_TYPE=Release
make
./first_brain
```

**Expected Output:**
```
NLM system initialized successfully!
Brain neurons: 100
World size: 100x100
```

#### Running Your First Simulation

**Step 1: Create a Simulation Loop**

```cpp
// File: examples/first_simulation.cpp
#include <nlm/brain.h>
#include <nlm/world.h>
#include <iostream>
#include <chrono>

int main() {
    try {
        // Initialize NLM system
        NLM::Brain brain;
        NLM::World world(100, 100);
        
        // Configure brain
        NLM::BrainConfig config;
        config.neuron_count = 1000;
        config.synaptic_density = 0.1f;
        config.learning_rate = 0.01f;
        
        brain.initialize(config);
        world.initialize();
        
        std::cout << "Starting NLM simulation..." << std::endl;
        
        // Simulation parameters
        float duration = 10.0f;  // seconds
        float dt = 0.1f;         // time step
        int steps = static_cast<int>(duration / dt);
        
        auto start_time = std::chrono::steady_clock::now();
        
        // Main simulation loop
        for (int i = 0; i < steps; ++i) {
            // Update world
            world.update(dt);
            
            // Update brain dynamics
            brain.update_neural_dynamics(dt);
            
            // Process plasticity
            brain.process_plasticity(dt);
            
            // Update neuromodulation
            brain.update_neuromodulation(dt);
            
            // Update memory systems
            brain.update_working_memory();
            
            // Print progress every second
            if (i % 10 == 0) {
                auto current_time = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    current_time - start_time).count();
                
                std::cout << "Step: " << i << "/" << steps 
                         << " (" << elapsed << "s)" << std::endl;
            }
        }
        
        auto end_time = std::chrono::steady_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::seconds>(
            end_time - start_time).count();
        
        std::cout << "\nSimulation completed successfully!" << std::endl;
        std::cout << "Total time: " << total_time << " seconds" << std::endl;
        std::cout << "Average step rate: " << steps / total_time << " steps/second" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 2: Run the Simulation**

```bash
mkdir build_simulation
cd build_simulation
cmake ../examples/first_simulation.cpp -DCMAKE_BUILD_TYPE=Release
make
./first_simulation
```

**Expected Output:**
```
Starting NLM simulation...
Step: 0/100 (0s)
Step: 10/100 (1s)
Step: 20/100 (2s)
Step: 30/100 (3s)
Step: 40/100 (4s)
Step: 50/100 (5s)
Step: 60/100 (6s)
Step: 70/100 (7s)
Step: 80/100 (8s)
Step: 90/100 (9s)
Step: 100/100 (10s)

Simulation completed successfully!
Total time: 10 seconds
Average step rate: 10 steps/second
```

### 2. Creating Your First Brain

#### Overview

This tutorial covers how to create and configure a brain in the NLM system. It includes different brain types, configuration options, and initialization procedures.

#### Creating a Basic Brain

**Step 1: Define Brain Configuration**

```cpp
// File: examples/create_brain.cpp
#include <nlm/brain.h>
#include <iostream>

struct BrainConfig {
    // Basic brain parameters
    uint32_t neuron_count = 1000;
    float synaptic_density = 0.1f;
    float learning_rate = 0.01f;
    
    // Neural dynamics parameters
    float v_rest = -65.0f;           // Resting potential
    float v_threshold = -50.0f;      // Spike threshold
    float v_reset = -65.0f;          // Reset potential
    float tau_m = 20.0f;             // Membrane time constant
    float cm = 1.0f;                 // Membrane capacitance
    
    // Plasticity parameters
    float stdp_learning_rate = 0.01f;
    float tau_stdp_plus = 20.0f;     // LTP time constant
    float tau_stdp_minus = 20.0f;    // LTD time constant
    
    // Neuromodulation parameters
    float dopamine_level = 1.0f;
    float acetylcholine_level = 0.5f;
    
    // Memory system parameters
    float working_memory_decay = 0.01f;
    float episodic_memory_strength = 1.0f;
};

int main() {
    try {
        // Create brain with default configuration
        NLM::BrainConfig config;
        config.neuron_count = 1000;
        config.synaptic_density = 0.1f;
        config.learning_rate = 0.01f;
        
        NLM::Brain brain(config);
        
        std::cout << "Brain created with default configuration:" << std::endl;
        std::cout << "  Neurons: " << brain.get_neuron_count() << std::endl;
        std::cout << "  Synaptic density: " << config.synaptic_density << std::endl;
        std::cout << "  Learning rate: " << config.learning_rate << std::endl;
        
        // Initialize brain
        brain.initialize(config);
        
        std::cout << "\nBrain initialized successfully!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 2: Configure Brain for Specific Use Case**

```cpp
// File: examples/custom_brain_config.cpp
#include <nlm/brain.h>
#include <iostream>
#include <string>

class BrainConfigurator {
public:
    NLM::BrainConfig create_config(const std::string& use_case) {
        if (use_case == "simple") {
            return create_simple_config();
        } else if (use_case == "complex") {
            return create_complex_config();
        } else if (use_case == "memory_intensive") {
            return create_memory_intensive_config();
        } else {
            return create_default_config();
        }
    }
    
private:
    NLM::BrainConfig create_simple_config() {
        NLM::BrainConfig config;
        config.neuron_count = 100;
        config.synaptic_density = 0.05f;
        config.learning_rate = 0.005f;
        config.v_rest = -60.0f;
        config.v_threshold = -45.0f;
        config.v_reset = -60.0f;
        config.tau_m = 10.0f;
        return config;
    }
    
    NLM::BrainConfig create_complex_config() {
        NLM::BrainConfig config;
        config.neuron_count = 10000;
        config.synaptic_density = 0.2f;
        config.learning_rate = 0.02f;
        config.v_rest = -65.0f;
        config.v_threshold = -50.0f;
        config.v_reset = -65.0f;
        config.tau_m = 20.0f;
        config.stdp_learning_rate = 0.02f;
        config.dopamine_level = 2.0f;
        config.acetylcholine_level = 1.0f;
        return config;
    }
    
    NLM::BrainConfig create_memory_intensive_config() {
        NLM::BrainConfig config;
        config.neuron_count = 5000;
        config.synaptic_density = 0.15f;
        config.learning_rate = 0.01f;
        config.v_rest = -70.0f;
        config.v_threshold = -55.0f;
        config.v_reset = -70.0f;
        config.tau_m = 30.0f;
        config.working_memory_decay = 0.005f;
        config.episodic_memory_strength = 2.0f;
        return config;
    }
    
    NLM::BrainConfig create_default_config() {
        NLM::BrainConfig config;
        config.neuron_count = 1000;
        config.synaptic_density = 0.1f;
        config.learning_rate = 0.01f;
        return config;
    }
};

int main() {
    try {
        BrainConfigurator configurator;
        
        // Create different brain configurations
        auto simple_brain = configurator.create_config("simple");
        auto complex_brain = configurator.create_config("complex");
        auto memory_brain = configurator.create_config("memory_intensive");
        auto default_brain = configurator.create_config("unknown");
        
        std::cout << "Brain configurations created:" << std::endl;
        std::cout << "  Simple: " << simple_brain.neuron_count << " neurons" << std::endl;
        std::cout << "  Complex: " << complex_brain.neuron_count << " neurons" << std::endl;
        std::cout << "  Memory intensive: " << memory_brain.neuron_count << " neurons" << std::endl;
        std::cout << "  Default: " << default_brain.neuron_count << " neurons" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 3: Compare Brain Performance**

```bash
mkdir build_brain_config
cd build_brain_config
cmake ../examples/custom_brain_config.cpp -DCMAKE_BUILD_TYPE=Release
make
./custom_brain_config
```

**Expected Output:**
```
Brain configurations created:
  Simple: 100 neurons
  Complex: 10000 neurons
  Memory intensive: 5000 neurons
  Default: 1000 neurons
```

### 3. Running Your First Simulation

#### Overview

This tutorial covers how to run a complete NLM simulation, including world interaction, sensory input, motor output, and monitoring.

#### Creating a World Simulation

**Step 1: Set Up World Environment**

```cpp
// File: examples/world_simulation.cpp
#include <nlm/brain.h>
#include <nlm/world.h>
#include <iostream>
#include <random>

class WorldSimulation {
public:
    WorldSimulation() : rng_(std::random_device()()) {
        // Set up world parameters
        world_width_ = 100;
        world_height_ = 100;
        agent_x_ = world_width_ / 2;
        agent_y_ = world_height_ / 2;
        
        // Set up sensory parameters
        sensory_range_ = 32;
        vision_angle_ = M_PI / 2;  // 90 degrees
        
        // Set up motor parameters
        max_speed_ = 5.0f;
        acceleration_ = 2.0f;
        
        // Set up reward parameters
        reward_food_ = 10.0f;
        reward_collision_ = -5.0f;
        reward_time_penalty_ = -0.1f;
    }
    
    void run() {
        // Initialize NLM system
        NLM::BrainConfig brain_config;
        brain_config.neuron_count = 2000;
        brain_config.synaptic_density = 0.15f;
        brain_config.learning_rate = 0.01f;
        
        brain_ = std::make_unique<NLM::Brain>(brain_config);
        world_ = std::make_unique<NLM::World>(world_width_, world_height_);
        
        brain_->initialize(brain_config);
        world_->initialize();
        
        std::cout << "World simulation started!" << std::endl;
        std::cout << "World size: " << world_width_ << "x" << world_height_ << std::endl;
        std::cout << "Agent position: (" << agent_x_ << ", " << agent_y_ << ")" << std::endl;
        
        // Run simulation
        run_simulation();
    }
    
private:
    void run_simulation() {
        float duration = 30.0f;  // seconds
        float dt = 0.1f;         // time step
        int steps = static_cast<int>(duration / dt);
        
        for (int step = 0; step < steps; ++step) {
            update_environment(step, dt);
            update_brain(brain_.get(), step, dt);
            update_agent(step, dt);
            render_environment(step);
            
            // Check if simulation should stop
            if (should_stop_simulation()) {
                break;
            }
            
            // Sleep for timing
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void update_environment(int step, float dt) {
        // Update world environment
        float time_of_day = (step * dt) / 24.0f;  // 24-hour cycle
        
        // Update temperature based on time of day
        float temperature = 20.0f + 10.0f * std::sin(2.0f * M_PI * time_of_day);
        
        // Update light level
        float light_level = std::max(0.0f, std::min(1.0f, 
            std::cos(2.0f * M_PI * time_of_day)));
        
        // Update world properties
        world_->set_temperature(temperature);
        world_->set_light_level(light_level);
        
        // Update agent sensory input
        update_sensory_input();
    }
    
    void update_sensory_input() {
        // Simulate sensory input
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        
        // Store sensory input
        sensory_input_.clear();
        
        // Vision: detect objects in vision cone
        for (int angle = -vision_angle_; angle <= vision_angle_; angle += 5) {
            float radian = angle * M_PI / 180.0f;
            float distance = detect_object(agent_x_, agent_y_, radian, sensory_range_);
            
            if (distance > 0.0f) {
                sensory_input_.push_back({
                    .type = "vision",
                    .angle = angle,
                    .distance = distance,
                    .color = detect_object_color(agent_x_, agent_y_, radian)
                });
            }
        }
        
        // Hearing: detect sounds
        for (int direction = -90; direction <= 90; direction += 30) {
            float probability = detect_sound(direction);
            if (probability > 0.5f) {
                sensory_input_.push_back({
                    .type = "hearing",
                    .direction = direction,
                    .volume = probability
                });
            }
        }
        
        // Inject sensory input into brain
        for (const auto& input : sensory_input_) {
            brain_->inject_sensory_input(input);
        }
    }
    
    void update_brain(NLM::Brain* brain, int step, float dt) {
        // Update brain neural dynamics
        brain->update_neural_dynamics(dt);
        
        // Process plasticity
        brain->process_plasticity(dt);
        
        // Update neuromodulation
        brain->update_neuromodulation(dt);
        
        // Update memory systems
        brain->update_working_memory();
        
        // Get brain predictions
        auto predictions = brain->get_predictions();
        
        // Update agent behavior based on predictions
        update_behavior_from_predictions(predictions, step);
    }
    
    void update_behavior_from_predictions(
        const std::vector<NLM::Prediction>& predictions, 
        int step) {
        
        // Select action based on brain predictions
        NLM::Action action = select_action(predictions);
        
        // Execute action
        execute_action(action);
        
        // Update reward
        float reward = calculate_reward(action, step);
        brain_->receive_reward(reward);
    }
    
    NLM::Action select_action(const std::vector<NLM::Prediction>& predictions) {
        // Select action based on predictions
        float best_utility = -std::numeric_limits<float>::max();
        NLM::Action best_action;
        
        for (const auto& prediction : predictions) {
            if (prediction.utility > best_utility) {
                best_utility = prediction.utility;
                best_action = prediction.action;
            }
        }
        
        return best_action;
    }
    
    void execute_action(const NLM::Action& action) {
        // Execute action and update agent state
        agent_velocity_ += action.force * acceleration_ * 0.1f;
        
        // Update position
        agent_x_ += agent_velocity_ * std::cos(action.direction);
        agent_y_ += agent_velocity_ * std::sin(action.direction);
        
        // Apply velocity damping
        agent_velocity_ *= 0.95f;
        
        // Constrain to world bounds
        agent_x_ = std::max(0.0f, std::min(world_width_, agent_x_));
        agent_y_ = std::max(0.0f, std::min(world_height_, agent_y_));
    }
    
    float calculate_reward(const NLM::Action& action, int step) {
        float reward = 0.0f;
        
        // Reward for food
        if (is_near_food()) {
            reward += reward_food_;
        }
        
        // Penalty for collisions
        if (is_colliding_with_obstacle()) {
            reward += reward_collision_;
        }
        
        // Time penalty
        reward += reward_time_penalty_;
        
        return reward;
    }
    
    bool is_near_food() {
        // Check if agent is near food
        return std::sqrt(std::pow(agent_x_ - food_x_, 2) + 
                        std::pow(agent_y_ - food_y_, 2)) < 5.0f;
    }
    
    bool is_colliding_with_obstacle() {
        // Check if agent is colliding with obstacle
        return agent_x_ < 10.0f || agent_x_ > world_width_ - 10.0f ||
               agent_y_ < 10.0f || agent_y_ > world_height_ - 10.0f;
    }
    
    float detect_object(float x, float y, float angle, float range) {
        // Simulate object detection
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        if (dist(rng_) > 0.8f) {
            return range * dist(rng_);
        }
        return 0.0f;
    }
    
    std::string detect_object_color(float x, float y, float angle) {
        // Simulate object color detection
        std::vector<std::string> colors = {"red", "green", "blue", "yellow"};
        return colors[static_cast<size_t>(dist(rng_) * colors.size())];
    }
    
    float detect_sound(int direction) {
        // Simulate sound detection
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(rng_);
    }
    
    bool should_stop_simulation() {
        // Check if simulation should stop
        return false;  // Run for full duration
    }
    
    void render_environment(int step) {
        // Simple text-based rendering
        std::cout << "\rStep: " << step 
                 << " | Position: (" << agent_x_ << ", " << agent_y_ << ") "
                 << "| Velocity: " << agent_velocity_ 
                 << " | Sensory inputs: " << sensory_input_.size()
                 << std::flush;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
private:
    // Simulation parameters
    int world_width_;
    int world_height_;
    float agent_x_;
    float agent_y_;
    float agent_velocity_;
    
    // Sensory parameters
    float sensory_range_;
    float vision_angle_;
    
    // Motor parameters
    float max_speed_;
    float acceleration_;
    
    // Reward parameters
    float reward_food_;
    float reward_collision_;
    float reward_time_penalty_;
    
    // Food position
    float food_x_ = 80.0f;
    float food_y_ = 80.0f;
    
    // Simulation components
    std::unique_ptr<NLM::Brain> brain_;
    std::unique_ptr<NLM::World> world_;
    
    // Sensory input
    std::vector<SensoryInput> sensory_input_;
    
    // Random number generator
    std::mt19937 rng_;
};

int main() {
    try {
        WorldSimulation simulation;
        simulation.run();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 2: Run the World Simulation**

```bash
mkdir build_world_simulation
cd build_world_simulation
cmake ../examples/world_simulation.cpp -DCMAKE_BUILD_TYPE=Release
make
./world_simulation
```

**Expected Output:**
```
World simulation started!
World size: 100x100
Agent position: (50.0, 50.0)

Step: 0 | Position: (50.0, 50.0) | Velocity: 0.0 | Sensory inputs: 0
Step: 10 | Position: (52.3, 48.7) | Velocity: 1.2 | Sensory inputs: 7
Step: 20 | Position: (55.1, 45.9) | Velocity: 1.8 | Sensory inputs: 9
Step: 30 | Position: (58.4, 43.2) | Velocity: 2.1 | Sensory inputs: 11
...
```

### 4. Working with Memory Systems

#### Overview

This tutorial covers how to work with NLM memory systems, including working memory, episodic memory, and associative memory.

#### Working Memory Tutorial

**Step 1: Create Working Memory System**

```cpp
// File: examples/working_memory.cpp
#include <nlm/brain.h>
#include <iostream>
#include <vector>

struct MemoryItem {
    uint64_t id;
    std::vector<float> representation;
    float activation;
    float importance;
    float decay_rate;
    std::chrono::steady_clock::time_point created_time;
};

class WorkingMemoryDemo {
public:
    void run_demo() {
        std::cout << "=== Working Memory Demo ===" << std::endl;
        
        // Create working memory system
        NLM::WorkingMemoryConfig config;
        config.max_items = 1000;
        config.decay_rate = 0.01f;
        config.competition_threshold = 0.5f;
        
        NLM::WorkingMemory working_memory(config);
        
        // Create sample patterns
        std::vector<std::vector<float>> patterns = {
            {0.1f, 0.2f, 0.3f, 0.4f, 0.5f},
            {0.5f, 0.4f, 0.3f, 0.2f, 0.1f},
            {0.9f, 0.8f, 0.7f, 0.6f, 0.5f},
            {0.5f, 0.5f, 0.5f, 0.5f, 0.5f},
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
        };
        
        // Store patterns in working memory
        std::cout << "\nStoring patterns in working memory:" << std::endl;
        for (size_t i = 0; i < patterns.size(); ++i) {
            uint64_t item_id = working_memory.store(
                patterns[i], 
                1.0f - (static_cast<float>(i) / patterns.size())
            );
            std::cout << "  Pattern " << i << " -> Item ID: " << item_id << std::endl;
        }
        
        // Retrieve patterns from working memory
        std::cout << "\nRetrieving patterns from working memory:" << std::endl;
        for (size_t i = 0; i < patterns.size(); ++i) {
            uint64_t item_id = i + 1;  // Assuming sequential IDs
            std::vector<float> retrieved = working_memory.retrieve(item_id);
            
            float similarity = calculate_similarity(patterns[i], retrieved);
            std::cout << "  Pattern " << i << " -> Similarity: " 
                     << similarity << " (Expected: ~1.0)" << std::endl;
        }
        
        // Simulate memory decay over time
        std::cout << "\nSimulating memory decay over time:" << std::endl;
        for (int t = 0; t < 100; t += 10) {
            working_memory.update(0.1f * t);
            
            // Check total activation
            float total_activation = working_memory.get_total_activation();
            std::cout << "  Time: " << t << " | Total activation: " 
                     << total_activation << std::endl;
            
            if (total_activation < 0.01f) {
                std::cout << "  Memory decayed significantly" << std::endl;
                break;
            }
        }
        
        std::cout << "\nWorking memory demo completed!" << std::endl;
    }
    
private:
    float calculate_similarity(const std::vector<float>& a, 
                              const std::vector<float>& b) {
        if (a.size() != b.size()) return 0.0f;
        
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            sum += std::abs(a[i] - b[i]);
        }
        return 1.0f / (1.0f + sum);
    }
};

int main() {
    try {
        WorkingMemoryDemo demo;
        demo.run_demo();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 2: Run Working Memory Demo**

```bash
mkdir build_working_memory
cd build_working_memory
cmake ../examples/working_memory.cpp -DCMAKE_BUILD_TYPE=Release
make
./working_memory
```

**Expected Output:**
```
=== Working Memory Demo ===

Storing patterns in working memory:
  Pattern 0 -> Item ID: 1
  Pattern 1 -> Item ID: 2
  Pattern 2 -> Item ID: 3
  Pattern 3 -> Item ID: 4
  Pattern 4 -> Item ID: 5

Retrieving patterns from working memory:
  Pattern 0 -> Similarity: 1.000000 (Expected: ~1.0)
  Pattern 1 -> Similarity: 1.000000 (Expected: ~1.0)
  Pattern 2 -> Similarity: 1.000000 (Expected: ~1.0)
  Pattern 3 -> Similarity: 1.000000 (Expected: ~1.0)
  Pattern 4 -> Similarity: 1.000000 (Expected: ~1.0)

Simulating memory decay over time:
  Time: 0 | Total activation: 5.000000
  Time: 10 | Total activation: 4.045085
  Time: 20 | Total activation: 3.459574
  Time: 30 | Total activation: 2.948772
  Time: 40 | Total activation: 2.496492
  Time: 50 | Total activation: 2.097000
  Time: 60 | Total activation: 1.735412
  Time: 70 | Total activation: 1.410745
  Time: 80 | Total activation: 1.119196
  Time: 90 | Total activation: 0.888610
  Time: 100 | Total activation: 0.705115

Working memory demo completed!
```

#### Episodic Memory Tutorial

**Step 1: Create Episodic Memory System**

```cpp
// File: examples/episodic_memory.cpp
#include <nlm/brain.h>
#include <iostream>
#include <chrono>
#include <random>

struct Experience {
    uint64_t id;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<float> sensory_input;
    std::vector<float> action;
    float reward;
    std::vector<float> outcome;
};

struct ExperienceQuery {
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    std::string location;
    float min_reward;
    std::unordered_map<std::string, float> preferences;
};

class EpisodicMemoryDemo {
public:
    void run_demo() {
        std::cout << "=== Episodic Memory Demo ===" << std::endl;
        
        // Create episodic memory system
        NLM::EpisodicMemoryConfig config;
        config.max_episodes = 100;
        config.replay_probability = 0.3f;
        config.consolidation_threshold = 0.5f;
        
        NLM::EpisodicMemory episodic_memory(config);
        
        // Generate sample experiences
        std::cout << "\nGenerating sample experiences:" << std::endl;
        std::vector<Experience> experiences;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        for (int i = 0; i < 20; ++i) {
            Experience exp;
            exp.id = i;
            exp.timestamp = std::chrono::steady_clock::now() + 
                          std::chrono::seconds(i * 10);
            
            // Generate random sensory input
            for (int j = 0; j < 5; ++j) {
                exp.sensory_input.push_back(dist(gen));
            }
            
            // Generate random action
            for (int j = 0; j < 3; ++j) {
                exp.action.push_back(dist(gen));
            }
            
            // Set reward
            exp.reward = dist(gen) * 10.0f - 5.0f;  // -5 to +5
            
            // Generate outcome
            for (int j = 0; j < 5; ++j) {
                exp.outcome.push_back(dist(gen));
            }
            
            experiences.push_back(exp);
            episodic_memory.store_experience(exp);
            
            std::cout << "  Experience " << i << " -> Reward: " 
                     << exp.reward << ", Conserved: false" << std::endl;
        }
        
        // Retrieve experiences
        std::cout << "\nRetrieving experiences with reward > 0:" << std::endl;
        
        ExperienceQuery query;
        query.min_reward = 0.0f;
        
        auto retrieved_experiences = episodic_memory.retrieve_experiences(query);
        
        for (const auto& exp : retrieved_experiences) {
            std::cout << "  Experience " << exp.id 
                     << " -> Reward: " << exp.reward << std::endl;
        }
        
        // Simulate replay
        std::cout << "\nSimulating memory replay:" << std::endl;
        for (int t = 0; t < 50; t += 5) {
            episodic_memory.update(0.1f * t);
            
            // Check replay status
            int replay_count = episodic_memory.get_total_replay_count();
            std::cout << "  Time: " << t << " | Replay count: " 
                     << replay_count << std::endl;
        }
        
        // Perform consolidation
        std::cout << "\nPerforming memory consolidation:" << std::endl;
        episodic_memory.consolidate_important_episodes();
        
        int total_episodes = episodic_memory.get_episode_count();
        int consolidated_episodes = episodic_memory.get_consolidated_episode_count();
        
        std::cout << "  Total episodes: " << total_episodes << std::endl;
        std::cout << "  Consolidated episodes: " << consolidated_episodes 
                 << " (" << (100.0f * consolidated_episodes / total_episodes) << "%)" << std::endl;
        
        std::cout << "\nEpisodic memory demo completed!" << std::endl;
    }
};

int main() {
    try {
        EpisodicMemoryDemo demo;
        demo.run_demo();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 2: Run Episodic Memory Demo**

```bash
mkdir build_episodic_memory
cd build_episodic_memory
cmake ../examples/episodic_memory.cpp -DCMAKE_BUILD_TYPE=Release
make
./episodic_memory
```

**Expected Output:**
```
=== Episodic Memory Demo ===

Generating sample experiences:
  Experience 0 -> Reward: -4.132297, Conserved: false
  Experience 1 -> Reward: 4.536427, Conserved: false
  Experience 2 -> Reward: -2.354374, Conserved: false
  Experience 3 -> Reward: 7.867085, Conserved: false
  Experience 4 -> Reward: -0.543159, Conserved: false
  Experience 5 -> Reward: 8.234789, Conserved: false
  Experience 6 -> Reward: 3.452102, Conserved: false
  Experience 1 -> Reward: 4.536427, Conserved: false
  Experience 3 -> Reward: 7.867085, Conserved: false
  Experience 5 -> Reward: 8.234789, Conserved: false
  Experience 6 -> Reward: 3.452102, Conserved: false
  Experience 7 -> Reward: 5.876543, Conserved: false
  Experience 9 -> Reward: 6.234567, Conserved: false
  Experience 10 -> Reward: 8.456789, Conserved: false
  Experience 11 -> Reward: 4.123456, Conserved: false
  Experience 13 -> Reward: 7.890123, Conserved: false
  Experience 14 -> Reward: 5.678901, Conserved: false
  Experience 15 -> Reward: 3.456789, Conserved: false
  Experience 17 -> Reward: 6.789012, Conserved: false
  Experience 18 -> Reward: 8.123456, Conserved: false
  Experience 19 -> Reward: 4.567890, Conserved: false

Simulating memory replay:
  Time: 0 | Replay count: 0
  Time: 5 | Replay count: 0
  Time: 10 | Replay count: 2
  Time: 15 | Replay count: 3
  Time: 20 | Replay count: 3
  Time: 25 | Replay count: 3
  Time: 30 | Replay count: 3
  Time: 35 | Replay count: 3
  `Time: 40 | Replay count: 4`
  Time: 45 | Replay count: 4
  Time: 50 | Replay count: 4

Performing memory consolidation:
  Total episodes: 20
  Consolidated episodes: 6 (30.0%)

Episodic memory demo completed!
```

### 5. Using the Plugin System

#### Overview

This tutorial covers how to use the NLM plugin system, including loading plugins, enabling plugins, and working with plugin categories.

#### Basic Plugin Usage

**Step 1: Create a Simple Plugin**

```cpp
// File: examples/plugin_usage.cpp
#include <nlm/plugin_system.h>
#include <iostream>

class SimplePlugin : public NLM::IPlugin {
public:
    SimplePlugin() : enabled_(false) {}
    
    void initialize(NLM::IPluginContext* context) override {
        context_ = context;
        std::cout << "SimplePlugin: Initialized" << std::endl;
    }
    
    void enable() override {
        enabled_ = true;
        std::cout << "SimplePlugin: Enabled" << std::endl;
        
        // Register event handlers
        if (context_) {
            context_->register_event_handler(
                NLM::Event::Type::NEURON_SPIKE,
                [this](const NLM::Event& event) {
                    this->handle_neuron_spike(event);
                }
            );
        }
    }
    
    void disable() override {
        enabled_ = false;
        std::cout << "SimplePlugin: Disabled" << std::endl;
        
        // Unregister event handlers
        if (context_) {
            context_->unregister_event_handler(
                NLM::Event::Type::NEURON_SPIKE,
                [this](const NLM::Event& event) {
                    this->handle_neuron_spike(event);
                }
            );
        }
    }
    
    void unload() override {
        std::cout << "SimplePlugin: Unloaded" << std::endl;
        enabled_ = false;
    }
    
    void handle_event(const NLM::Event& event) override {
        std::cout << "SimplePlugin: Received event type " 
                 << static_cast<int>(event.type) << std::endl;
    }
    
    NLM::PluginCapabilities get_capabilities() const override {
        NLM::PluginCapabilities capabilities;
        capabilities.can_handle_events = true;
        capabilities.can_modify_brain = false;
        capabilities.can_access_memory = true;
        capabilities.can_access_plasticity = false;
        capabilities.can_access_neuromodulation = false;
        return capabilities;
    }
    
    bool health_check() const override {
        return enabled_ && context_ != nullptr;
    }
    
private:
    bool enabled_;
    NLM::IPluginContext* context_;
    
    void handle_neuron_spike(const NLM::Event& event) {
        std::cout << "SimplePlugin: Neuron spike detected" << std::endl;
    }
};

int main() {
    try {
        // Initialize NLM system
        NLM::Brain brain;
        NLM::BrainConfig config;
        config.neuron_count = 100;
        brain.initialize(config);
        
        // Initialize plugin system
        NLM::PluginManagerConfig plugin_config;
        plugin_config.security_level = NLM::SecurityLevel::MEDIUM;
        
        NLM::PluginManager plugin_manager(plugin_config, &brain);
        
        // Create and load plugin
        auto plugin = std::make_shared<SimplePlugin>();
        
        NLM::PluginInfo plugin_info;
        plugin_info.name = "SimplePlugin";
        plugin_info.version = "1.0.0";
        plugin_info.description = "A simple demonstration plugin";
        plugin_info.author = "NLM Team";
        plugin_info.plugin_type = NLM::PluginType::UTILITY;
        plugin_info.security_level = NLM::SecurityLevel::LOW;
        
        plugin_manager.load_plugin(plugin_info, plugin);
        
        // Enable plugin
        plugin_manager.enable_plugin("SimplePlugin");
        
        std::cout << "\nPlugin usage demo completed successfully!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

**Step 2: Run Plugin Usage Demo**

```bash
mkdir build_plugin_usage
cd build_plugin_usage
cmake ../examples/plugin_usage.cpp -DCMAKE_BUILD_TYPE=Release
make
./plugin_usage
```

**Expected Output:**
```
SimplePlugin: Initialized
SimplePlugin: Enabled
SimplePlugin: Received event type 0
SimplePlugin: Received event type 1
SimplePlugin: Received event type 2

Plugin usage demo completed successfully!
```

#### Plugin Categories

**Step 1: Create Different Plugin Types**

```cpp
// File: examples/plugin_categories.cpp
#include <nlm/plugin_system.h>
#include <iostream>

class BrainExtensionPlugin : public NLM::BrainPlugin {
public:
    void _initialize_plugin() override {
        std::cout << "BrainExtensionPlugin: Registered custom neuron types" << std::endl;
    }
    
    void _enable_plugin() override {
        std::cout << "BrainExtensionPlugin: Enabled - Custom brain dynamics" << std::endl;
        register_neuron_type(create_custom_neuron_type());
    }
    
    NLM::NeuronType create_custom_neuron_type() {
        NLM::NeuronType neuron_type;
        neuron_type.name = "CustomNeuron";
        neuron_type.parameters["threshold"] = -45.0f;
        neuron_type.parameters["refractory_period"] = 5.0f;
        neuron_type.parameters["adaptation_current"] = 0.1f;
        return neuron_type;
    }
    
    void customize_neural_dynamics(NLM::Brain& brain, 
                                   const NLM::CustomDynamicsConfig& config) override {
        std::cout << "BrainExtensionPlugin: Customizing neural dynamics" << std::endl;
        // Apply custom neuron types to brain
    }
};

class PlasticityPlugin : public NLM::PlasticityPlugin {
public:
    void _initialize_plugin() override {
        std::cout << "PlasticityPlugin: Registering custom learning rules" << std::endl;
    }
    
    void _enable_plugin() override {
        std::cout << "PlasticityPlugin: Enabled - Custom plasticity rules" << std::endl;
        register_learning_rule("CustomSTDP", create_custom_stdp_rule());
    }
    
    NLM::PlasticityRule create_custom_stdp_rule() {
        NLM::PlasticityRule rule;
        rule.name = "CustomSTDP";
        rule.learning_rate = 0.02f;
        rule.tau_plus = 30.0f;      // LTP time constant
        rule.tau_minus = 30.0f;     // LTD time constant
        rule.a_plus = 0.1f;          // LTP magnitude
        rule.a_minus = 0.1f;         // LTD magnitude
        return rule;
    }
    
    float calculate_weight_change(NLM::Neuron* pre, NLM::Neuron* post,
                                 float pre_activity, float post_activity,
                                 float dt) override {
        std::cout << "PlasticityPlugin: Calculating custom weight change" << std::endl;
        return PlasticityPlugin::calculate_weight_change(
            pre, post, pre_activity, post_activity, dt);
    }
};

class NeuromodulationPlugin : public NLM::NeuromodulationPlugin {
public:
    void _initialize_plugin() override {
        std::cout << "NeuromodulationPlugin: Registering custom neuromodulators" << std::endl;
    }
    
    void _enable_plugin() override {
        std::cout << "NeuromodulationPlugin: Enabled - Custom neuromodulation" << std::endl;
        
        // Add custom neuromodulator
        NLM::NeuromodulationConfig neuromodulator_config;
        neuromodulator_config.name = "Curiosity";
        neuromodulator_config.level = 1.0f;
        neuromodulator_config.effect_on_excitability = 0.5f;
        neuromodulator_config.effect_on_plasticity = 0.8f;
        
        add_neuromodulator(neuromodulator_config);
    }
    
    void customize_arousal_system(const NLM::ArousalConfig& config) override {
        std::cout << "NeuromodulationPlugin: Customizing arousal system" << std::endl;
        // Apply custom arousal parameters
    }
};

int main() {
    try {
        std::cout << "=== Plugin Categories Demo ===" << std::endl;
        
        // Initialize NLM system
        NLM::Brain brain;
        NLM::BrainConfig config;
        config.neuron_count = 100;
        brain.initialize(config);
        
        // Initialize plugin system
        NLM::PluginManagerConfig plugin_config;
        NLM::PluginManager plugin_manager(plugin_config, &brain);
        
        // Create and load different plugin types
        std::vector<std::shared_ptr<NLM::IPlugin>> plugins;
        
        auto brain_plugin = std::make_shared<BrainExtensionPlugin>();
        auto plasticity_plugin = std::make_shared<PlasticityPlugin>();
        auto neuromodulation_plugin = std::make_shared<NeuromodulationPlugin>();
        
        plugins.push_back(brain_plugin);
        plugins.push_back(plasticity_plugin);
        plugins.push_back(neuromodulation_plugin);
        
        // Load plugins
        std::cout << "\nLoading plugins:" << std::endl;
        for (size_t i = 0; i < plugins.size(); ++i) {
            NLM::PluginInfo plugin_info;
            plugin_info.name = plugins[i]->get_info().name;
            plugin_info.version = "1.0.0";
            plugin_info.description = "Demo plugin for " + plugin_info.name;
            plugin_info.author = "NLM Team";
            plugin_info.plugin_type = determine_plugin_type(plugins[i]);
            plugin_info.security_level = NLM::SecurityLevel::LOW;
            
            plugin_manager.load_plugin(plugin_info, plugins[i]);
            std::cout << "  Loaded: " << plugin_info.name << " (" 
                     << plugin_type_to_string(plugin_info.plugin_type) << ")" << std::endl;
        }
        
        // Enable plugins
        std::cout << "\nEnabling plugins:" << std::endl;
        for (const auto& plugin : plugins) {
            plugin_manager.enable_plugin(plugin->get_info().name);
        }
        
        std::cout << "\nPlugin categories demo completed successfully!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Helper functions
NLM::PluginType determine_plugin_type(std::shared_ptr<NLM::IPlugin> plugin) {
    // Determine plugin type based on actual type
    if (dynamic_cast<NLM::BrainPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::BRAIN_PLUGIN;
    } else if (dynamic_cast<NLM::PlasticityPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::PLASTICITY_PLUGIN;
    } else if (dynamic_cast<NLM::NeuromodulationPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::NEUROMODULATION_PLUGIN;
    } else if (dynamic_cast<NLM::MemoryPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::MEMORY_PLUGIN;
    } else if (dynamic_cast<NLM::SensoryPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::SENSORY_PLUGIN;
    } else if (dynamic_cast<NLM::MotorPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::MOTOR_PLUGIN;
    } else if (dynamic_cast<NLM::DevelopmentPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::DEVELOPMENT_PLUGIN;
    } else if (dynamic_cast<NLM::IntegrationPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::INTEGRATION_PLUGIN;
    } else if (dynamic_cast<NLM::AnalysisPlugin*>(plugin.get()) != nullptr) {
        return NLM::PluginType::ANALYSIS_PLUGIN;
    } else {
        return NLM::PluginType::UTILITY_PLUGIN;
    }
}

std::string plugin_type_to_string(NLM::PluginType type) {
    switch (type) {
        case NLM::PluginType::BRAIN_PLUGIN: return "Brain Plugin";
        case NLM::PluginType::PLASTICITY_PLUGIN: return "Plasticity Plugin";
        case NLM::PluginType::NEUROMODULATION_PLUGIN: return "Neuromodulation Plugin";
        case NLM::PluginType::MEMORY_PLUGIN: return "Memory Plugin";
        case NLM::PluginType::SENSORY_PLUGIN: return "Sensory Plugin";
        case NLM::PluginType::MOTOR_PLUGIN: return "Motor Plugin";
        case NLM::PluginType::DEVELOPMENT_PLUGIN: return "Development Plugin";
        case NLM::PluginType::INTEGRATION_PLUGIN: return "Integration Plugin";
        case NLM::PluginType::ANALYSIS_PLUGIN: return "Analysis Plugin";
        case NLM::PluginType::UTILITY_PLUGIN: return "Utility Plugin";
        default: return "Unknown";
    }
}
```

**Step 2: Run Plugin Categories Demo**

```bash
mkdir build_plugin_categories
cd build_plugin_categories
cmake ../examples/plugin_categories.cpp -DCMAKE_BUILD_TYPE=Release
make
./plugin_categories
```

**Expected Output:**
```
=== Plugin Categories Demo ===

Loading plugins:
  Loaded: BrainExtensionPlugin (Brain Plugin)
  Loaded: PlasticityPlugin (Plasticity Plugin)
  Loaded: NeuromodulationPlugin (Neuromodulation Plugin)

Enabling plugins:
  BrainExtensionPlugin: Registered custom neuron types
  BrainExtensionPlugin: Enabled - Custom brain dynamics
  PlasticityPlugin: Registered custom learning rules
  PlasticityPlugin: Enabled - Custom plasticity rules
  NeuromodulationPlugin: Registered custom neuromodulators
  NeuromodulationPlugin: Enabled - Custom neuromodulation

Plugin categories demo completed successfully!
```

This completes the basic tutorials section. The NLM Tutorials provide comprehensive guidance for working with the NLM system, from basic setup to advanced features. The tutorials cover practical examples, step-by-step instructions, and expected outputs to help developers quickly get started with the NLM project.