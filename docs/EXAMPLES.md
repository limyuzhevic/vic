# NLM Examples

## Overview

This directory contains working code examples demonstrating NLM functionality at different complexity levels. Each example is designed to be runnable and educational, showing practical applications of the NLM system.

## Example Structure

Each example follows a standard structure:

```
examples/
├── beginner/
│   ├── simple_neural_network.cpp          # Basic neuron simulation
│   ├── stdp_learning.cpp                  # STDP demonstration
│   └── working_memory.cpp                  # Working memory example
├── intermediate/
│   ├── sensorimotor_loop.cpp              # Sensorimotor control
│   ├── reward_learning.cpp                # Reward-based learning
│   └── curiosity_driven.cpp               # Exploration behavior
├── advanced/
│   ├── episodic_memory.cpp                # Episodic memory usage
│   ├── concept_formation.cpp              # Abstract reasoning
│   └── neural_planning.cpp                 # Action planning
└── research/
    ├── developmental.py                   # Development experiments
    ├── prediction_models.py                # Forward model learning
    └── custom_plasticity.py                # Custom plasticity rules
```

## Beginner Examples

### 1. Simple Neural Network

**File:** `examples/beginner/simple_neural_network.cpp`

This example demonstrates basic neural network setup and simulation using NLM components.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <vector>

int main() {
    // Create basic neural network configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Simple Neural Network Example" << std::endl;
    std::cout << "================================" << std::endl;
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "Step " << step << ": "
                     << brain.getTotalSpikeCount() << " spikes, "
                     << brain.getAverageFiringRate() << " Hz average rate" << std::endl;
        }
    }
    
    std::cout << "\nFinal Statistics:" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
```

### 2. STDP Learning Demonstration

**File:** `examples/beginner/stdp_learning.cpp`

This example demonstrates Spike-Timing-Dependent Plasticity learning.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <vector>

int main() {
    // Configure STDP parameters
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "STDP Learning Demonstration" << std::endl;
    std::cout << "============================" << std::endl;
    
    // Get STDP system for configuration
    auto* stdp = brain.getSTDP();
    if (stdp) {
        stdp->configure(0.01f, 0.012f, 20.0f);
    }
    
    // Run learning experiment with pre-post spike timing
    for (nlm::SimulationStep step = 0; step < 3000; ++step) {
        brain.step(step, step * 0.001);
        
        // Create pre-synaptic spike (neuron 0)
        if (step % 100 == 0) {
            brain.injectCurrent(nlm::NeuronId(0), 2.0f);
            std::cout << "Pre-synaptic spike at step " << step << std::endl;
        }
        
        // Create post-synaptic spike (neuron 1) with different timing
        if (step % 150 == 0) {
            brain.injectCurrent(nlm::NeuronId(1), 2.0f);
            std::cout << "Post-synaptic spike at step " << step << std::endl;
        }
        
        // Log synapse weight changes every 500 steps
        if (step % 500 == 0 && step > 0) {
            auto* current_stdp = brain.getSTDP();
            if (current_stdp) {
                // Analyze weight changes (simplified)
                std::cout << "Step " << step << ": Learning occurred" << std::endl;
            }
        }
    }
    
    std::cout << "\nSTDP Experiment Complete:" << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Total learning events: " << step / 50 << std::endl;
    
    return 0;
}
```

### 3. Working Memory Example

**File:** `examples/beginner/working_memory.cpp`

This example demonstrates working memory functionality.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <vector>

int main() {
    // Configure for working memory experiment
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("working_memory_capacity", 100);
    config->set("working_memory_decay", 0.9f);
    config->set("simulation_timestep", 0.001);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Working Memory Example" << std::endl;
    std::cout << "======================" << std::endl;
    
    auto* working_memory = brain.getWorkingMemory();
    if (!working_memory) {
        std::cerr << "Working memory not available!" << std::endl;
        return 1;
    }
    
    // Run working memory demonstration
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        // Process sensory input
        std::vector<float> input = generate_input_pattern(step);
        nlm::SensoryInput sensory_input(input);
        brain.receiveSensoryInput(sensory_input);
        
        // Step brain
        brain.step(step, step * 0.001);
        
        // Update working memory
        working_memory->update(0.001);
        
        // Store important information every 50 steps
        if (step % 50 == 0) {
            auto active_neurons = working_memory->getMemoryNeurons();
            working_memory->storeToMemory(active_neurons);
            std::cout << "Step " << step << ": Stored " 
                     << active_neurons.size() << " items in working memory" << std::endl;
        }
        
        // Check memory capacity
        int active_count = working_memory->getActiveCount();
        int capacity = working_memory->getCapacity();
        float utilization = (active_count * 100.0f) / capacity;
        
        if (step % 200 == 0) {
            std::cout << "Memory utilization: " << utilization << "% (" 
                     << active_count << "/" << capacity << " items)" << std::endl;
        }
    }
    
    std::cout << "\nWorking Memory Results:" << std::endl;
    std::cout << "  Final memory items: " << working_memory->getActiveCount() << std::endl;
    std::cout << "  Memory decay rate: " << working_memory->getDecayRate() << std::endl;
    std::cout << "  Memory stability: " << working_memory->getStability() << std::endl;
    
    return 0;
}

// Helper function to generate input patterns
std::vector<float> generate_input_pattern(nlm::SimulationStep step) {
    std::vector<float> input(10, 0.0f); // 10-dimensional input
    
    // Create different patterns based on step
    switch (step % 5) {
        case 0:
            // Random pattern
            for (auto& value : input) {
                value = static_cast<float>(rand()) / RAND_MAX;
            }
            break;
        case 1:
            // Step pattern
            for (size_t i = 0; i < input.size(); ++i) {
                input[i] = (i < input.size() / 2) ? 1.0f : 0.0f;
            }
            break;
        case 2:
            // Sine wave pattern
            for (size_t i = 0; i < input.size(); ++i) {
                input[i] = std::sin(static_cast<float>(i) * 0.1f * step * 0.001f);
            }
            break;
        case 3:
            // Increasing pattern
            for (size_t i = 0; i < input.size(); ++i) {
                input[i] = static_cast<float>(i + 1) / input.size();
            }
            break;
        case 4:
            // Decreasing pattern
            for (size_t i = 0; i < input.size(); ++i) {
                input[i] = static_cast<float>(input.size() - i) / input.size();
            }
            break;
    }
    
    return input;
}
```

## Intermediate Examples

### 1. Sensorimotor Loop

**File:** `examples/intermediate/sensorimotor_loop.cpp`

This example demonstrates closed-loop sensorimotor control.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include "SimpleWorld.hpp"
#include <iostream>

class SimpleRobotEnvironment {
private:
    std::vector<std::vector<float>> grid;
    nlm::Position robot_position;
    nlm::Position goal_position;
    
public:
    SimpleRobotEnvironment() {
        // Initialize 10x10 grid
        grid = std::vector<std::vector<float>>(10, std::vector<float>(10, 0.0f));
        
        // Set robot at start (0,0)
        robot_position = {0.0f, 0.0f};
        
        // Set goal at (9,9)
        goal_position = {9.0f, 9.0f};
        
        // Add obstacles
        grid[3][3] = -1.0f; // Wall
        grid[3][4] = -1.0f;
        grid[4][3] = -1.0f;
    }
    
    WorldState getCurrentState() {
        // Create state representation
        std::vector<float> state;
        
        // Robot position
        state.push_back(robot_position.x);
        state.push_back(robot_position.y);
        
        // Goal position relative to robot
        state.push_back(goal_position.x - robot_position.x);
        state.push_back(goal_position.y - robot_position.y);
        
        // Obstacle information
        for (int x = -2; x <= 2; ++x) {
            for (int y = -2; y <= 2; ++y) {
                int world_x = robot_position.x + x;
                int world_y = robot_position.y + y;
                
                if (world_x >= 0 && world_x < 10 && world_y >= 0 && world_y < 10) {
                    state.push_back(grid[world_x][world_y]);
                } else {
                    state.push_back(-2.0f); // Unknown/out of bounds
                }
            }
        }
        
        return WorldState(state);
    }
    
    float executeAction(const nlm::Action& action) {
        nlm::Position new_position = robot_position;
        
        switch (action.type) {
            case nlm::ActionType::MoveForward:
                new_position.x += 1.0f;
                new_position.y += 1.0f;
                break;
            case nlm::ActionType::TurnLeft:
                std::swap(new_position.x, new_position.y);
                new_position.y = -new_position.y;
                break;
            case nlm::ActionType::TurnRight:
                std::swap(new_position.x, new_position.y);
                new_position.x = -new_position.x;
                break;
            case nlm::ActionType::Wait:
                // No movement
                break;
        }
        
        // Check for collision
        if (new_position.x >= 0 && new_position.x < 10 && 
            new_position.y >= 0 && new_position.y < 10 &&
            grid[new_position.x][new_position.y] >= 0.0f) {
            
            // Update position
            robot_position = new_position;
            
            // Calculate reward
            float reward = 0.0f;
            
            // Goal reward
            if (robot_position.x == goal_position.x && 
                robot_position.y == goal_position.y) {
                reward += 10.0f;
            }
            
            // Obstacle penalty
            if (grid[robot_position.x][robot_position.y] < 0.0f) {
                reward -= 5.0f;
            }
            
            // Step cost
            reward -= 0.1f;
            
            return reward;
        }
        
        return -10.0f; // Collision penalty
    }
    
    std::string getDescription() const {
        return "10x10 grid with obstacles, robot must reach top-right corner";
    }
};

int main() {
    // Configure for sensorimotor experiment
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.05f);
    config->set("dopamine_baseline", 0.1f);
    config->set("curiosity_rate", 0.02f);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Create environment
    auto environment = std::make_unique<SimpleRobotEnvironment>();
    
    std::cout << "Sensorimotor Loop Example" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "Environment: " << environment->getDescription() << std::endl;
    
    // Run sensorimotor loop
    const size_t total_steps = 5000;
    float total_reward = 0.0f;
    size_t goal_reached = 0;
    
    for (nlm::SimulationStep step = 0; step < total_steps; ++step) {
        // Get current state
        WorldState world_state = environment->getCurrentState();
        SensoryInput sensory_input = world_state.toSensoryInput();
        
        // Process input
        brain.receiveSensoryInput(sensory_input);
        
        // Step brain
        brain.step(step, step * 0.001f);
        
        // Generate action
        auto action = brain.produceAction();
        
        // Execute action and get reward
        float reward = environment->executeAction(action.get());
        total_reward += reward;
        
        // Apply reward as neuromodulation
        nlm::DopamineSignal dopamine_reward(reward);
        brain.applyNeuromodulation(dopamine_reward);
        
        // Check for goal achievement
        if (environment->hasReachedGoal()) {
            goal_reached++;
            std::cout << "Step " << step << ": Goal reached! Reward: " 
                     << reward << 
                     << " (Total goals: " << goal_reached << ")" << std::endl;
        }
        
        // Log progress every 500 steps
        if (step % 500 == 0) {
            std::cout << "Step " << step << ": Reward: " << reward 
                     << ", Total: " << total_reward 
                     << ", Action: " << (action ? action->toString() : "Wait")
                     << ", Goals: " << goal_reached << std::endl;
        }
    }
    
    std::cout << "\nSensorimotor Results:" << std::endl;
    std::cout << "  Total reward: " << total_reward << std::endl;
    std::cout << "  Goals reached: " << goal_reached << std::endl;
    std::cout << "  Success rate: " << (goal_reached * 100.0f / (total_steps / 500)) << "%" << std::endl;
    std::cout << "  Average reward per step: " << (total_reward / total_steps) << std::endl;
    
    return 0;
}
```

### 2. Reward Learning

**File:** `examples/intermediate/reward_learning.cpp`

This example demonstrates reward-based learning with dopamine signaling.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <random>

class RewardEnvironment {
private:
    struct State {
        float position;
        float velocity;
        float time;
    };
    
    State current_state;
    float cumulative_reward;
    std::mt19937 rng;
    
public:
    RewardEnvironment() : rng(std::random_device()()) {
        current_state = {0.0f, 0.0f, 0.0f};
        cumulative_reward = 0.0f;
    }
    
    WorldState getCurrentState() {
        // Encode state as sensory input
        std::vector<float> state;
        state.push_back(current_state.position);
        state.push_back(current_state.velocity);
        state.push_back(current_state.time);
        
        // Add noise for stochasticity
        std::normal_distribution<float> noise(0.0f, 0.1f);
        for (auto& value : state) {
            value += noise(rng);
        }
        
        return WorldState(state);
    }
    
    float executeAction(const nlm::Action& action) {
        // Update state based on action
        switch (action.type) {
            case nlm::ActionType::MoveForward:
                current_state.velocity += 0.5f;
                break;
            case nlm::ActionType::TurnLeft:
                current_state.velocity *= 0.8f;
                break;
            case nlm::ActionType::TurnRight:
                current_state.velocity *= 1.2f;
                break;
            case nlm::ActionType::Wait:
                current_state.velocity *= 0.95f;
                break;
        }
        
        // Physics update
        current_state.position += current_state.velocity * 0.001f;
        current_state.velocity *= 0.99f; // Friction
        current_state.time += 0.001f;
        
        // Calculate reward based on position
        float reward = 0.0f;
        
        // Goal reward at position 10
        if (current_state.position > 10.0f) {
            reward += 10.0f;
        }
        
        // Time penalty
        reward -= 0.01f;
        
        // Velocity bonus
        reward += std::abs(current_state.velocity) * 0.01f;
        
        cumulative_reward += reward;
        
        return reward;
    }
    
    float getCumulativeReward() const {
        return cumulative_reward;
    }
};

int main() {
    // Configure for reward learning experiment
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 800);
    config->set("region_count", 2);
    config->set("dopamine_baseline", 0.2f);
    config->set("dopamine_gain", 2.0f);
    config->set("curiosity_rate", 0.01f);
    config->set("simulation_timestep", 0.001f);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    auto environment = std::make_unique<RewardEnvironment>();
    
    std::cout << "Reward Learning Experiment" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // Configure dopamine system
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        dopamine->configure(0.2f, 2.0f); // baseline, gain
    }
    
    // Run reward learning simulation
    const size_t total_steps = 8000;
    std::vector<float> reward_history;
    
    for (nlm::SimulationStep step = 0; step < total_steps; ++step) {
        // Get current state and sensory input
        WorldState world_state = environment->getCurrentState();
        SensoryInput sensory_input = world_state.toSensoryInput();
        
        // Process input
        brain.receiveSensoryInput(sensory_input);
        brain.step(step, step * 0.001f);
        
        // Generate action
        auto action = brain.produceAction();
        
        // Execute and get reward
        float reward = environment->executeAction(action.get());
        reward_history.push_back(reward);
        
        // Apply reward as neuromodulation
        nlm::DopamineSignal dopamine_reward(reward);
        brain.applyNeuromodulation(dopamine_reward);
        
        // Log every 500 steps
        if (step % 500 == 0) {
            float avg_reward = 0.0f;
            for (size_t i = std::max(size_t(0), step - 500); i <= step; ++i) {
                if (i < reward_history.size()) {
                    avg_reward += reward_history[i];
                }
            }
            avg_reward /= std::min(size_t(500), reward_history.size());
            
            std::cout << "Step " << step << ": Reward=" << reward 
                     << ", Avg(500)= " << avg_reward
                     << ", Cumulative=" << environment->getCumulativeReward()
                     << ", Action=" << (action ? action->toString() : "Wait") << std::endl;
        }
    }
    
    // Analyze reward learning
    std::cout << "\nReward Learning Analysis:" << std::endl;
    
    // Calculate statistics
    float avg_reward = std::accumulate(reward_history.begin(), reward_history.end(), 0.0f) / reward_history.size();
    float max_reward = *std::max_element(reward_history.begin(), reward_history.end());
    float min_reward = *std::min_element(reward_history.begin(), reward_history.end());
    
    std::cout << "  Average reward: " << avg_reward << std::endl;
    std::cout << "  Maximum reward: " << max_reward << std::endl;
    std::cout << "  Minimum reward: " << min_reward << std::endl;
    std::cout << "  Total reward: " << environment->getCumulativeReward() << std::endl;
    
    // Check dopamine activity
    if (dopamine) {
        float final_dopamine = dopamine->getLevel();
        float plasticity_effect = dopamine->getPlasticityFactor();
        std::cout << "  Final dopamine level: " << final_dopamine << std::endl;
        std::cout << "  Plasticity effect: " << plasticity_effect << std::endl;
    }
    
    return 0;
}
```

### 3. Curiosity-Driven Behavior

**File:** `examples/intermediate/curiosity_driven.cpp`

This example demonstrates curiosity-driven exploration.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <random>

class CuriosityEnvironment {
private:
    struct Cell {
        float value;
        bool visited;
        std::vector<float> history;
    };
    
    std::vector<std::vector<Cell>> grid;
    nlm::Position position;
    float time;
    std::mt19937 rng;
    
public:
    CuriosityEnvironment() : rng(std::random_device()()) {
        // Initialize 8x8 grid with random values
        grid = std::vector<std::vector<Cell>>(8, std::vector<Cell>(8));
        
        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                grid[x][y].value = static_cast<float>(rand()) / RAND_MAX;
                grid[x][y].visited = false;
            }
        }
        
        position = {0, 0};
        time = 0.0f;
        grid[0][0].visited = true;
        grid[0][0].history.push_back(grid[0][0].value);
    }
    
    WorldState getCurrentState() {
        // Create state representation
        std::vector<float> state;
        
        // Current position
        state.push_back(position.x);
        state.push_back(position.y);
        
        // Cell value at current position
        state.push_back(grid[position.x][position.y].value);
        
        // Visit history
        float avg_history = 0.0f;
        if (!grid[position.x][position.y].history.empty()) {
            for (float value : grid[position.x][position.y].history) {
                avg_history += value;
            }
            avg_history /= grid[position.x][position.y].history.size();
        }
        state.push_back(avg_history);
        
        // Time
        state.push_back(time);
        
        return WorldState(state);
    }
    
    float executeAction(const nlm::Action& action) {
        nlm::Position new_position = position;
        
        switch (action.type) {
            case nlm::ActionType::MoveForward:
                new_position.x = (new_position.x + 1) % 8;
                new_position.y = (new_position.y + 1) % 8;
                break;
            case nlm::ActionType::TurnLeft:
                std::swap(new_position.x, new_position.y);
                new_position.y = 7 - new_position.y;
                break;
            case nlm::ActionType::TurnRight:
                std::swap(new_position.x, new_position.y);
                new_position.x = 7 - new_position.x;
                break;
            case nlm::ActionType::Wait:
                // Stay in place
                break;
        }
        
        // Check if we can move
        if (new_position.x >= 0 && new_position.x < 8 && 
            new_position.y >= 0 && new_position.y < 8) {
            
            position = new_position;
            
            if (!grid[position.x][position.y].visited) {
                grid[position.x][position.y].visited = true;
            }
            
            grid[position.x][position.y].history.push_back(grid[position.x][position.y].value);
            time += 0.001f;
            
            // Calculate reward
            float reward = 0.0f;
            
            // Novelty reward (difference from expected)
            float expected_value = grid[position.x][position.y].value * 0.8f;
            float novelty = std::abs(grid[position.x][position.y].value - expected_value);
            reward += novelty * 5.0f;
            
            // Exploration bonus for unvisited cells
            if (!grid[position.x][position.y].visited) {
                reward += 2.0f;
            }
            
            // Time penalty
            reward -= 0.001f;
            
            return reward;
        }
        
        return -5.0f; // Invalid move penalty
    }
    
    bool has_visited_cell(int x, int y) const {
        return grid[x][y].visited;
    }
    
    float get_grid_coverage() const {
        int visited_count = 0;
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                if (cell.visited) {
                    visited_count++;
                }
            }
        }
        return (visited_count * 100.0f) / 64;
    }
};

int main() {
    // Configure for curiosity-driven exploration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1200);
    config->set("region_count", 3);
    config->set("curiosity_rate", 0.05f);
    config->set("novelty_threshold", 0.3f);
    config->set("novelty_sensitivity", 2.0f);
    config->set("dopamine_gain", 1.5f);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    auto environment = std::make_unique<CuriosityEnvironment>();
    
    std::cout << "Curiosity-Driven Exploration Example" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Configure curiosity systems
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        curiosity->configure(0.05f);
    }
    
    auto* novelty = brain.getNovelty();
    if (novelty) {
        novelty->configure(0.3f, 2.0f); // threshold, sensitivity
    }
    
    // Run curiosity-driven exploration
    const size_t total_steps = 6000;
    float total_reward = 0.0f;
    size_t exploration_steps = 0;
    
    for (nlm::SimulationStep step = 0; step < total_steps; ++step) {
        // Get current state
        WorldState world_state = environment->getCurrentState();
        SensoryInput sensory_input = world_state.toSensoryInput();
        
        // Process input
        brain.receiveSensoryInput(sensory_input);
        brain.step(step, step * 0.001f);
        
        // Generate action
        auto action = brain.produceAction();
        
        // Execute and get reward
        float reward = environment->executeAction(action.get());
        total_reward += reward;
        
        // Apply reward and novelty
        nlm::DopamineSignal dopamine_reward(reward);
        brain.applyNeuromodulation(dopamine_reward);
        
        // Track exploration
        if (reward > 1.0f) {
            exploration_steps++;
        }
        
        // Log every 500 steps
        if (step % 500 == 0) {
            float curiosity_level = curiosity ? curiosity->getLevel() : 0.0f;
            std::cout << "Step " << step << ": Reward=" << reward
                     << ", Curiosity=" << curiosity_level
                     << ", Coverage=" << environment->get_grid_coverage() << "%"
                     << ", Exploration steps=" << exploration_steps << std::endl;
        }
    }
    
    std::cout << "\nCuriosity-Driven Exploration Results:" << std::endl;
    std::cout << "  Total reward: " << total_reward << std::endl;
    std::cout << "  Exploration efficiency: " << (exploration_steps * 100.0f / total_steps) << "%" << std::endl;
    std::cout << "  Final grid coverage: " << environment->get_grid_coverage() << "%" << std::endl;
    
    // Analyze curiosity behavior
    if (curiosity) {
        float final_curiosity = curiosity->getLevel();
        std::cout << "  Final curiosity level: " << final_curiosity << std::endl;
    }
    
    if (novelty) {
        float novelty_sensitivity = novelty->getSensitivity();
        std::cout << "  Novelty sensitivity: " << novelty_sensitivity << std::endl;
    }
    
    return 0;
}
```

## Advanced Examples

### 1. Episodic Memory

**File:** `examples/advanced/episodic_memory.cpp`

This example demonstrates episodic memory usage and replay.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <vector>

struct Experience {
    nlm::SimulationStep step;
    float reward;
    nlm::Timestamp timestamp;
    std::vector<nlm::NeuronId> active_neurons;
    std::vector<float> neuron_activations;
};

int main() {
    // Configure for episodic memory experiment
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 3000);
    config->set("max_episodes", 5000);
    config->set("replay_interval", 100);
    config->set("consolidation_interval", 500);
    config->set("working_memory_capacity", 500);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Episodic Memory Example" << std::endl;
    std::cout << "=======================" << std::endl;
    
    auto* episodic_memory = brain.getEpisodicMemory();
    auto* working_memory = brain.getWorkingMemory();
    
    if (!episodic_memory || !working_memory) {
        std::cerr << "Memory systems not available!" << std::endl;
        return 1;
    }
    
    // Collect experiences
    std::vector<Experience> experiences;
    const size_t total_steps = 4000;
    
    for (nlm::SimulationStep step = 0; step < total_steps; ++step) {
        // Create input based on step pattern
        std::vector<float> input(8, 0.0f);
        for (size_t i = 0; i < input.size(); ++i) {
            input[i] = std::sin(static_cast<float>(i) * 0.5f + step * 0.001f);
        }
        
        // Create sensory input
        SensoryInput sensory_input(input);
        brain.receiveSensoryInput(sensory_input);
        
        // Step brain
        brain.step(step, step * 0.001f);
        
        // Update working memory
        working_memory->update(0.001f);
        
        // Capture current neural state for episodic memory
        if (step % 100 == 0) {
            Experience experience;
            experience.step = step;
            experience.timestamp = step * 0.001f;
            experience.reward = 0.0f; // Simplified reward
            
            // Capture active neurons from working memory
            auto memory_neurons = working_memory->getMemoryNeurons();
            experience.active_neurons = memory_neurons;
            
            // Capture activation levels
            for (nlm::NeuronId neuron : memory_neurons) {
                float activation = working_memory->getNeuronActivation(neuron);
                experience.neuron_activations.push_back(activation);
            }
            
            experiences.push_back(experience);
            episodic_memory->storeEpisode(experience);
            
            std::cout << "Step " << step << ": Stored episode " << experiences.size() - 1 << std::endl;
        }
        
        // Trigger replay periodically
        if (step % 300 == 0 && step > 0) {
            auto replays = episodic_memory->getEpisodesForReplay(5);
            std::cout << "Step " << step << ": Replaying " << replays.size() << " episodes" << std::endl;
            
            for (const auto* episode : replays) {
                episodic_memory->replayEpisode(episode);
            }
        }
        
        // Trigger consolidation periodically
        if (step % 500 == 0 && step > 0) {
            episodic_memory->consolidate(0.3f);
            std::cout << "Step " << step << ": Consolidated memory" << std::endl;
        }
    }
    
    // Analyze episodic memory
    std::cout << "\nEpisodic Memory Analysis:" << std::endl;
    std::cout << "  Episodes stored: " << episodic_memory->getEpisodeCount() << std::endl;
    std::cout << "  Episodes ready for replay: " << episodic_memory->getReplayableEpisodes() << std::endl;
    std::cout "  Episodes available for replay: " << episodic_memory->getEpisodesForReplay(1).size() << std::endl;
    
    // Calculate memory metrics
    float memory_efficiency = episodic_memory->getConsolidationEfficiency();
    float memory_decay = episodic_memory->getMemoryDecay();
    
    std::cout << "  Memory consolidation efficiency: " << memory_efficiency << std::endl;
    std::cout << "  Memory decay rate: " << memory_decay << std::endl;
    
    // Test memory recall
    if (!experiences.empty()) {
        Experience test_experience = experiences.back();
        float similarity_score = episodic_memory->calculateRecallSimilarity(test_experience);
        std::cout << "  Recall similarity for latest episode: " << similarity_score << std::endl;
    }
    
    return 0;
}
```

### 2. Concept Formation

**File:** `examples/advanced/concept_formation.cpp`

This example demonstrates abstract concept formation.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <vector>
#include <map>

struct Pattern {
    std::vector<float> features;
    std::string category;
};

class ConceptFormationExample {
private:
    std::vector<Pattern> training_patterns;
    nlm::Brain& brain;
    
public:
    ConceptFormationExample(nlm::Brain& b) : brain(b) {
        // Generate training patterns for concept learning
        generate_training_patterns();
    }
    
    void generate_training_patterns() {
        // Generate patterns for different concepts
        
        // Red-like patterns (high red component)
        for (int i = 0; i < 15; ++i) {
            Pattern pattern;
            pattern.features = {
                0.8f + static_cast<float>(rand()) / RAND_MAX * 0.2f,  // Red
                static_cast<float>(rand()) / RAND_MAX,                  // Green
                static_cast<float>(rand()) / RAND_MAX,                  // Blue
            };
            pattern.category = "Red";
            training_patterns.push_back(pattern);
        }
        
        // Blue-like patterns (high blue component)
        for (int i = 0; i < 15; ++i) {
            Pattern pattern;
            pattern.features = {
                static_cast<float>(rand()) / RAND_MAX,                  // Red
                static_cast<float>(rand()) / RAND_MAX,                  // Green
                0.8f + static_cast<float>(rand()) / RAND_MAX * 0.2f,  // Blue
            };
            pattern.category = "Blue";
            training_patterns.push_back(pattern);
        }
        
        // Green-like patterns (high green component)
        for (int i = 0; i < 15; ++i) {
            Pattern pattern;
            pattern.features = {
                static_cast<float>(rand()) / RAND_MAX,                  // Red
                0.8f + static_cast<float>(rand()) / RAND_MAX * 0.2f,  // Green
                static_cast<float>(rand()) / RAND_MAX,                  // Blue
            };
            pattern.category = "Green";
            training_patterns.push_back(pattern);
        }
        
        // Yellow-like patterns (high red+green)
        for (int i = 0; i < 15; ++i) {
            Pattern pattern;
            pattern.features = {
                0.7f + static_cast<float>(rand()) / RAND_MAX * 0.3f,  // Red
                0.7f + static_cast<float>(rand()) / RAND_MAX * 0.3f,  // Green
                static_cast<float>(rand()) / RAND_MAX,                  // Blue
            };
            pattern.category = "Yellow";
            training_patterns.push_back(pattern);
        }
    }
    
    void run_concept_learning() {
        std::cout << "Concept Formation Example" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << "Learning to categorize visual patterns:" << std::endl;
        
        auto* concept_formation = brain.getConceptFormation();
        if (!concept_formation) {
            std::cerr << "Concept formation system not available!" << std::endl;
            return;
        }
        
        // Initialize concept formation
        concept_formation->initialize(&brain);
        concept_formation->setLearningRate(0.01f);
        concept_formation->setMinSamples(5);
        
        // Process training patterns
        std::cout << "\nTraining with " << training_patterns.size() << " patterns..." << std::endl;
        
        for (size_t step = 0; step < 3000; ++step) {
            // Select random pattern
            int pattern_idx = step % training_patterns.size();
            const Pattern& pattern = training_patterns[pattern_idx];
            
            // Create neural representation of pattern
            nlm::SensoryInput sensory_input(pattern.features);
            brain.receiveSensoryInput(sensory_input);
            
            // Step brain
            brain.step(step, step * 0.001f);
            
            // Update concept formation
            concept_formation->processInput(pattern.features);
            
            // Check for new concepts every 500 steps
            if (step % 500 == 0) {
                auto new_concepts = concept_formation->getNewConcepts();
                if (!new_concepts.empty()) {
                    std::cout << "Step " << step << ": New concepts detected:" << std::endl;
                    for (const auto& concept : new_concepts) {
                        std::cout << "  - Concept: " << concept.name << std::endl;
                        std::cout << "    Samples: " << concept.samples.size() << std::endl;
                        std::cout << "    Stability: " << concept.stability << std::endl;
                    }
                }
                
                // Test concept categorization
                test_concept_categorization(concept_formation, pattern_idx);
            }
            
            // Log progress
            if (step % 750 == 0) {
                int concept_count = concept_formation->getConceptCount();
                float concept_stability = concept_formation->getConceptStability();
                std::cout << "Step " << step << ": " << concept_count 
                         << " concepts formed, stability: " << concept_stability << std::endl;
            }
        }
        
        // Final concept analysis
        analyze_concepts(concept_formation);
    }
    
    void test_concept_categorization(nlm::ConceptFormation* concepts, int pattern_idx) {
        // Test if concepts correctly categorize test patterns
        if (!concepts) return;
        
        const Pattern& test_pattern = training_patterns[pattern_idx];
        
        // Get concept that best matches the pattern
        auto* best_concept = concepts->getBestConcept(test_pattern.features);
        
        if (best_concept) {
            std::cout << "  Test categorization: Pattern " << test_pattern.category 
                     << " matched concept: " << best_concept->name << std::endl;
            
            if (best_concept->name == test_pattern.category) {
                std::cout << "    ✓ Correct categorization!" << std::endl;
            } else {
                std::cout << "    ✗ Incorrect categorization!" << std::endl;
            }
        }
    }
    
    void analyze_concepts(nlm::ConceptFormation* concepts) {
        if (!concepts) return;
        
        std::cout << "\nFinal Concept Analysis:" << std::endl;
        
        int concept_count = concepts->getConceptCount();
        std::cout << "  Total concepts formed: " << concept_count << std::endl;
        
        if (concept_count > 0) {
            float concept_stability = concepts->getConceptStability();
            std::cout << "  Overall concept stability: " << concept_stability << std::endl;
            
            // Show concept details
            std::cout << "\nConcepts found:" << std::endl;
            for (int i = 0; i < concept_count; ++i) {
                auto* concept = concepts->getConcept(i);
                if (concept) {
                    std::cout << "  " << i+1 << ". " << concept->name 
                             << " (" << concept->samples.size() << " samples, "
                             << concept->stability << " stability)" << std::endl;
                }
            }
        }
    }
};

int main() {
    // Configure for concept formation experiment
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 2000);
    config->set("region_count", 3);
    config->set("concept_learning_rate", 0.01f);
    config->set("min_concept_samples", 5);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Run concept formation example
    ConceptFormationExample example(brain);
    example.run_concept_learning();
    
    return 0;
}
```

### 3. Neural Planning

**File:** `examples/advanced/neural_planning.cpp`

This example demonstrates neural action planning.

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <vector>

struct PlannedAction {
    nlm::Action action;
    float expected_reward;
    nlm::Timestamp execution_time;
    std::vector<PlannedAction> sub_actions;
};

int main() {
    // Configure for neural planning experiment
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 2500);
    config->set("region_count", 4);
    config->set("planning_depth", 8);
    config->set("planning_window", 500);
    config->set("simulation_timestep", 0.001f);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Neural Planning Example" << std::endl;
    std::cout << "=======================" << std::endl;
    
    auto* planner = brain.getPlanner();
    if (!planner) {
        std::cerr << "Planning system not available!" << std::endl;
        return 1;
    }
    
    // Initialize planner
    planner->initialize(&brain);
    planner->setPlanningDepth(8);
    planner->setPlanningWindow(500);
    
    std::cout << "Planning system configured:" << std::endl;
    std::cout << "  Planning depth: 8 steps" << std::endl;
    std::cout << "  Planning horizon: 500 time steps" << std::endl;
    
    // Simulate environment for planning
    struct EnvironmentState {
        nlm::Position robot_position;
        nlm::Position goal_position;
        std::vector<nlm::Position> obstacles;
        float time_remaining;
    };
    
    EnvironmentState current_state;
    current_state.robot_position = {0.0f, 0.0f};
    current_state.goal_position = {10.0f, 10.0f};
    current_state.time_remaining = 5.0f; // 5 seconds total
    
    // Add some obstacles
    current_state.obstacles = {
        {3.0f, 3.0f}, {4.0f, 3.0f}, {5.0f, 3.0f},
        {3.0f, 6.0f}, {4.0f, 6.0f}, {5.0f, 6.0f}
    };
    
    // Run planning simulation
    const size_t total_steps = 4000;
    std::vector<PlannedAction> action_history;
    
    for (nlm::SimulationStep step = 0; step < total_steps; ++step) {
        // Get current brain state for planning
        BrainState brain_state;
        // (In real implementation, capture full brain state)
        
        // Generate action plan
        auto action_plan = planner->planAction(brain_state, step % planner->getPlanningWindow());
        
        if (action_plan) {
            // Record the planned action
            PlannedAction recorded_action;
            recorded_action.action = action_plan->actions[0]; // First action
            recorded_action.expected_reward = action_plan->expected_reward;
            recorded_action.execution_time = step * 0.001f;
            
            action_history.push_back(recorded_action);
            
            // Execute the action in simulated environment
            execute_planned_action(recorded_action, current_state);
            
            // Calculate actual reward
            float actual_reward = calculate_environment_reward(current_state);
            
            // Update planner with outcome
            planner->updateActionOutcome(action_plan->actions[0], actual_reward);
            
            // Log planning progress
            if (step % 300 == 0) {
                std::cout << "Step " << step << ": Planned " << action_history.size() << " actions" << std::endl;
                std::cout << "  Current action: " << (action_plan->actions[0] ? action_plan->actions[0]->toString() : "Wait")
                         << ", Expected reward: " << action_plan->expected_reward
                         << ", Actual: " << actual_reward << std::endl;
                
                // Show planning metrics
                float planning_efficiency = planner->getEfficiency();
                int success_rate = planner->getSuccessRate();
                std::cout << "  Planning efficiency: " << planning_efficiency << std::endl;
                std::cout << "  Success rate: " << success_rate << "%" << std::endl;
            }
            
            // Check for completion
            if (current_state.time_remaining <= 0.0f) {
                std::cout << "Goal reached within time!" << std::endl;
                break;
            }
        }
    }
    
    // Analyze planning performance
    std::cout << "\nNeural Planning Analysis:" << std::endl;
    std::cout << "  Total actions planned: " << action_history.size() << std::endl;
    
    if (planner) {
        float planning_efficiency = planner->getEfficiency();
        int success_rate = planner->getSuccessRate();
        int avg_depth = planner->getAveragePlanningDepth();
        
        std::cout << "  Final planning efficiency: " << planning_efficiency << std::endl;
        std::cout << "  Final success rate: " << success_rate << "%" << std::endl;
        std::cout << "  Average planning depth: " << avg_depth << " steps" << std::endl;
        
        // Analyze action sequence quality
        analyze_action_sequences(action_history);
    }
    
    return 0;
}

// Helper functions (would be implemented)
void execute_planned_action(const PlannedAction& action, EnvironmentState& state) {
    // Implement action execution logic
}

float calculate_environment_reward(const EnvironmentState& state) {
    // Calculate reward based on state
    return 0.0f;
}

void analyze_action_sequences(const std::vector<PlannedAction>& actions) {
    // Analyze the quality and effectiveness of planned actions
}
```

## Research Examples

### 1. Developmental Experiments

**File:** `examples/research/developmental.py`

This example demonstrates neural development and maturation using Python bindings.

```python
#!/usr/bin/env python3
"""
Developmental experiment for NLM.
Studies how neural systems develop and adapt over time.
"""

import numpy as np
import matplotlib.pyplot as plt
from nlm import Brain, Config
import json

class DevelopmentalExperiment:
    def __init__(self, config):
        self.config = config
        self.brain = Brain(config)
        self.development_log = []
        
    def run_development_study(self, duration_steps):
        """Run long-term developmental study."""
        print("Starting Developmental Study")
        print("===========================")
        
        if not self.brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        for step in range(duration_steps):
            # Record developmental stage
            stage = self.brain.get_developmental_stage()
            
            # Run brain cycle
            self.brain.step(step, step * 0.001)
            
            # Log key metrics
            if step % 100 == 0:
                self.log_developmental_state(step, stage)
                self.visualize_progress()
            
            # Apply developmental changes at key milestones
            if step % 1000 == 0:
                self.brain.develop()
                print(f"Developmental update at step {step}")
        
        return self.development_log
    
    def log_developmental_state(self, step, stage):
        """Record developmental state."""
        state = {
            'step': step,
            'stage': int(stage),
            'plasticity_factor': self.get_plasticity_factor(),
            'memory_capacity': self.get_memory_capacity(),
            'learning_rate': self.get_learning_rate()
        }
        self.development_log.append(state)
    
    def visualize_progress(self):
        """Visualize developmental progress."""
        if not self.development_log:
            return
            
        stages = [log['stage'] for log in self.development_log]
        plasticity = [log['plasticity_factor'] for log in self.development_log]
        
        plt.figure(figsize=(12, 8))
        
        plt.subplot(2, 2, 1)
        plt.plot(stages, marker='o')
        plt.title('Developmental Stage')
        plt.ylabel('Stage')
        
        plt.subplot(2, 2, 2)
        plt.plot(plasticity, color='red', marker='s')
        plt.title('Plasticity Factor')
        plt.ylabel('Plasticity')
        
        plt.tight_layout()
        plt.show()
    
    def get_plasticity_factor(self):
        """Get current plasticity factor."""
        auto* development = self.brain.get_development_system();
        if (development) {
            return development->get_plasticity_factor();
        }
        return 1.0f;
    
    def get_memory_capacity(self):
        """Get current memory capacity."""
        auto* working_memory = self.brain.get_working_memory();
        if (working_memory) {
            return working_memory->get_capacity();
        }
        return 0;
    
    def get_learning_rate(self):
        """Get current learning rate."""
        auto* dopamine = self.brain.get_dopamine();
        if (dopamine) {
            return dopamine->get_plasticity_factor();
        }
        return 0.0f;

if __name__ == "__main__":
    # Create developmental configuration
    config = Config()
    config.set("neuron_count", 5000)
    config.set("region_count", 5)
    config.set("critical_period_start", 200)
    config.set("critical_period_end", 2000)
    config.set("development_plasticity_factor", 2.0f)
    
    # Run developmental study
    experiment = DevelopmentalExperiment(config)
    results = experiment.run_development_study(10000)
    
    print("\nDevelopmental Study Complete!")
    print(f"Total observations: {len(results)}")
    
    # Save results
    with open('developmental_results.json', 'w') as f:
        json.dump(results, f, indent=2)
```

### 2. Prediction Model Learning

**File:** `examples/research/prediction_models.py`

This example demonstrates forward model learning and prediction.

```python
#!/usr/bin/env python3
"""
Prediction model learning experiment for NLM.
Studies how neural systems predict future states.
"""

import numpy as np
import matplotlib.pyplot as plt
from nlm import Brain, Config
import json

class PredictionModelExperiment:
    def __init__(self, config):
        self.config = config
        self.brain = Brain(config)
        self.prediction_log = []
        self.error_log = []
        
    def run_prediction_study(self, duration_steps):
        """Run prediction model learning study."""
        print("Starting Prediction Model Study")
        print("=============================")
        
        if (!self.brain.initialize()) {
            throw runtime_error("Brain initialization failed");
        }
        
        for (SimulationStep step = 0; step < duration_steps; ++step) {
            // Generate sensory input with predictable pattern
            SensoryInput input = generate_predictable_input(step);
            
            // Feed input to brain
            brain.receiveSensoryInput(input);
            brain.step(step, step * 0.001);
            
            // Get prediction from brain
            auto* prediction = brain.get_prediction_system();
            if (prediction) {
                float prediction_error = prediction->update(input);
                
                // Log prediction performance
                if (step % 50 == 0) {
                    log_prediction_performance(step, input, prediction_error, prediction);
                }
                
                error_log.push_back({step, prediction_error});
            }
            
            // Apply reward based on prediction accuracy
            if (prediction_error > 0.1f) {
                // Large error - high reward for correcting
                nlm::DopamineSignal reward_signal(1.0f - prediction_error);
                brain.applyNeuromodulation(reward_signal);
            }
            
            if (step % 100 == 0) {
                visualize_prediction_progress();
            }
        }
        
        return prediction_log, error_log;
    
    void log_prediction_performance(SimulationStep step, 
                                   const SensoryInput& input,
                                   float prediction_error,
                                   PredictionSystem* prediction) {
        PredictionLogEntry entry;
        entry.step = step;
        entry.input_magnitude = calculate_input_magnitude(input);
        entry.prediction_error = prediction_error;
        
        if (prediction) {
            entry.prediction_accuracy = prediction->getAccuracy();
            entry.prediction_confidence = prediction->getConfidence();
            entry.prediction_horizon = prediction->getHorizon();
        }
        
        prediction_log.push_back(entry);
    }
    
    void visualize_prediction_progress() {
        if (prediction_log.empty()) {
            return;
        }
        
        steps = [entry.step for entry in prediction_log];
        errors = [entry.prediction_error for entry in prediction_log];
        accuracy = [entry.prediction_accuracy for entry in prediction_log];
        
        plt.figure(figsize=(15, 5))
        
        plt.subplot(1, 3, 1)
        plt.plot(steps, errors, color='red', label='Prediction Error')
        plt.title('Prediction Error Over Time')
        plt.xlabel('Step')
        plt.ylabel('Error')
        plt.legend()
        
        plt.subplot(1, 3, 2)
        plt.plot(steps, accuracy, color='green', label='Prediction Accuracy')
        plt.title('Prediction Accuracy Over Time')
        plt.xlabel('Step')
        plt.ylabel('Accuracy')
        plt.legend()
        
        plt.subplot(1, 3, 3)
        plt.plot(steps, errors, color='red', label='Error')
        plt.plot(steps, accuracy, color='green', label='Accuracy')
        plt.title('Prediction Performance')
        plt.xlabel('Step')
        plt.legend()
        
        plt.tight_layout()
        plt.show()
    }
    
    float calculate_input_magnitude(const SensoryInput& input) const {
        float sum = 0.0f;
        const auto& data = input.getData();
        for (float value : data) {
            sum += std::abs(value);
        }
        return sum / data.size();
    }
    
    SensoryInput generate_predictable_input(SimulationStep step) {
        // Generate input with predictable patterns
        std::vector<float> data(10, 0.0f);
        
        // Create different patterns based on step
        if (step % 4 == 0) {
            // Sine wave pattern
            for (size_t i = 0; i < data.size(); ++i) {
                data[i] = std::sin(static_cast<float>(i) * 0.5f + step * 0.001f);
            }
        } else if (step % 4 == 1) {
            // Ramp pattern
            for (size_t i = 0; i < data.size(); ++i) {
                data[i] = static_cast<float>(i + 1) / data.size() * step * 0.01f;
            }
        } else if (step % 4 == 2) {
            // Square pattern
            for (size_t i = 0; i < data.size(); ++i) {
                data[i] = (i % 2 == 0) ? 1.0f : -1.0f;
            }
        } else {
            // Random pattern
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
            
            for (auto& value : data) {
                value = dis(gen);
            }
        }
        
        return SensoryInput(data);
    }
};

int main() {
    // Create prediction model configuration
    Config config;
    config.set("neuron_count", 3000);
    config.set("region_count", 3);
    config.set("prediction_horizon", 100);
    config.set("prediction_update_rate", 0.05f);
    config.set("prediction_accuracy", 0.9f);
    
    // Run prediction model study
    PredictionModelExperiment experiment(config);
    prediction_results, error_results = experiment.runPredictionStudy(5000);
    
    // Analyze prediction learning
    std::cout << "\nPrediction Model Study Complete!" << std::endl;
    std::cout << "Total prediction steps: " << prediction_results.size() << std::endl;
    
    // Calculate prediction metrics
    float avg_error = 0.0f;
    float min_error = std::numeric_limits<float>::max();
    float max_error = std::numeric_limits<float>::lowest();
    
    for (const auto& entry : error_results) {
        avg_error += entry.error;
        min_error = std::min(min_error, entry.error);
        max_error = std::max(max_error, entry.error);
    }
    
    if (!error_results.empty()) {
        avg_error /= error_results.size();
        
        std::cout << "Prediction Error Statistics:" << std::endl;
        std::cout << "  Average error: " << avg_error << std::endl;
        std::cout << "  Minimum error: " << min_error << std::endl;
        std::cout << "  Maximum error: " << max_error << std::endl;
        
        // Calculate prediction learning curve
        std::vector<float> learning_curve;
        size_t window = 100;
        
        for (size_t i = window; i < error_results.size(); ++i) {
            float window_avg = 0.0f;
            for (size_t j = i - window; j < i; ++j) {
                window_avg += error_results[j].error;
            }
            window_avg /= window;
            learning_curve.push_back(window_avg);
        }
        
        if (!learning_curve.empty()) {
            float final_accuracy = 1.0f - learning_curve.back();
            std::cout << "  Final prediction accuracy: " << final_accuracy << std::endl;
            
            if (final_accuracy > 0.8f) {
                std::cout << "  ✓ Excellent prediction accuracy!" << std::endl;
            } else if (final_accuracy > 0.6f) {
                std::cout << "  ⚠ Moderate prediction accuracy" << std::endl;
            } else {
                std::cout << "  ✗ Poor prediction accuracy" << std::endl;
            }
        }
    }
    
    return 0;
}
```

### 3. Custom Plasticity Rules

**File:** `examples/research/custom_plasticity.py`

This example demonstrates custom plasticity rule implementation.

```python
#!/usr/bin/env python3
"""
Custom plasticity rules experiment for NLM.
Studies how different plasticity rules affect learning.
"""

import numpy as np
import matplotlib.pyplot as plt
from nlm import Brain, Config
import json

class CustomPlasticityExperiment:
    def __init__(self, config):
        this->config = config;
        this->brain = Brain(config);
        this->plasticity_log = [];
        
    bool run_custom_plasticity_study(size_t duration_steps) {
        print("Starting Custom Plasticity Study");
        print("==================================");
        
        if (!brain->initialize()) {
            throw runtime_error("Brain initialization failed");
        }
        
        // Get plasticity systems
        STDP* stdp = brain->getSTDP();
        Hebbian* hebbian = brain->getHebbian();
        
        for (SimulationStep step = 0; step < duration_steps; ++step) {
            // Generate different spike patterns based on plasticity type
            switch (step % 5) {
                case 0:
                    generate_standard_stdp_pattern(brain, step);
                    break;
                case 1:
                    generate_hebbian_pattern(brain, step);
                    break;
                case 2:
                    generate_combined_pattern(brain, step);
                    break;
                case 3:
                    generate_reinforcement_pattern(brain, step);
                    break;
                case 4:
                    generate_exploration_pattern(brain, step);
                    break;
            }
            
            // Step brain
            brain->step(step, step * 0.001);
            
            // Log plasticity metrics
            if (step % 100 == 0) {
                log_plasticity_metrics(step, stdp, hebbian);
            }
            
            // Visualize progress periodically
            if (step % 500 == 0) {
                visualize_plasticity_progress();
            }
        }
        
        return plasticity_log;
    }
    
    void generate_standard_stdp_pattern(Brain& brain, SimulationStep step) {
        // Standard STDP pattern: pre before post
        if (step % 50 == 0) {
            // Strong pre-synaptic input
            brain->injectCurrent(NeuronId(0), 2.0f);
        }
        if (step % 75 == 0) {
            // Post-synaptic spike
            brain->injectCurrent(NeuronId(1), 2.0f);
        }
    }
    
    void generate_hebbian_pattern(Brain& brain, SimulationStep step) {
        // Hebbian pattern: correlated activity
        if (step % 30 == 0) {
            // Synchronized pre and post activity
            brain->injectCurrent(NeuronId(0), 1.0f);
            brain->injectCurrent(NeuronId(1), 1.0f);
        }
    }
    
    void generate_combined_pattern(Brain& brain, SimulationStep step) {
        // Combined pattern: STDP + Hebbian
        if (step % 40 == 0) {
            brain->injectCurrent(NeuronId(0), 1.5f);
        }
        if (step % 60 == 0) {
            brain->injectCurrent(NeuronId(1), 1.5f);
        }
        if (step % 90 == 0) {
            brain->injectCurrent(NeuronId(2), 1.0f);
        }
    }
    
    void generate_reinforcement_pattern(Brain& brain, SimulationStep step) {
        // Reinforcement pattern: dopamine-modulated
        if (step % 45 == 0) {
            brain->injectCurrent(NeuronId(0), 2.0f);
            
            // Apply strong dopamine reward
            DopamineSignal reward_signal(1.0f);
            brain->applyNeuromodulation(reward_signal);
        }
        if (step % 70 == 0) {
            brain->injectCurrent(NeuronId(1), 2.0f);
        }
    }
    
    void generate_exploration_pattern(Brain& brain, SimulationStep step) {
        // Exploration pattern: novelty-driven
        if (step % 35 == 0) {
            brain->injectCurrent(NeuronId(0), 1.0f);
        }
        if (step % 55 == 0) {
            brain->injectCurrent(NeuronId(1), 1.0f);
        }
        if (step % 80 == 0) {
            // High novelty
            auto* novelty = brain->getNovelty();
            if (novelty) {
                float novelty_value = novelty->update(0.001);
                if (novelty_value > 0.5f) {
                    // Strong curiosity response
                    DopamineSignal curiosity_signal(0.5f);
                    brain->applyNeuromodulation(curiosity_signal);
                }
            }
        }
    }
    
    void log_plasticity_metrics(SimulationStep step, STDP* stdp, Hebbian* hebbian) {
        PlasticityLogEntry entry;
        entry.step = step;
        
        if (stdp) {
            entry.stdp_ltp_weight = stdp->getLTPWeight();
            entry.stdp_ltd_weight = stdp->getLTDWeight();
            entry.stdp_time_constant = stdp->getTimeConstant();
            entry.stdp_activity_level = stdp->getActivityLevel();
        }
        
        if (hebbian) {
            entry.hebbian_learning_rate = hebbian->getLearningRate();
            entry.hebbian_activity_level = hebbian->getActivityLevel();
        }
        
        // Calculate net plasticity effect
        entry.net_plasticity = calculate_net_plasticity(entry);
        
        plasticity_log.push_back(entry);
    }
    
    void visualize_plasticity_progress() {
        if (plasticity_log.empty()) {
            return;
        }
        
        steps = [entry.step for entry in plasticity_log];
        stdp_plasticity = [entry.net_plasticity for entry in plasticity_log];
        hebbian_plasticity = [entry.hebbian_activity_level for entry in plasticity_log];
        
        plt.figure(figsize=(15, 10));
        
        plt.subplot(2, 2, 1)
        plt.plot(steps, stdp_plasticity, color='blue', label='STDP Plasticity')
        plt.title('STDP Plasticity Evolution')
        plt.ylabel('Plasticity Strength')
        plt.legend()
        
        plt.subplot(2, 2, 2)
        plt.plot(steps, hebbian_plasticity, color='green', label='Hebbian Plasticity')
        plt.title('Hebbian Plasticity Evolution')
        plt.ylabel('Hebbian Strength')
        plt.legend()
        
        plt.subplot(2, 2, 3)
        plt.plot(steps, stdp_plasticity, color='blue', label='STDP')
        plt.plot(steps, hebbian_plasticity, color='green', label='Hebbian')
        plt.title('Plasticity Systems Interaction')
        plt.ylabel('Strength')
        plt.legend()
        
        plt.subplot(2, 2, 4)
        plt.hist(stdp_plasticity, bins=20, alpha=0.7, color='blue', label='STDP');
        plt.hist(hebbian_plasticity, bins=20, alpha=0.7, color='green', label='Hebbian');
        plt.title('Plasticity Distribution')
        plt.xlabel('Plasticity Strength')
        plt.ylabel('Frequency')
        plt.legend()
        
        plt.tight_layout()
        plt.show();
    }
    
    float calculate_net_plasticity(const PlasticityLogEntry& entry) {
        float stdp_contribution = (entry.stdp_ltp_weight + entry.stdp_ltd_weight) * 0.5f;
        float hebbian_contribution = entry.hebbian_learning_rate * entry.hebbian_activity_level;
        
        return stdp_contribution + hebbian_contribution;
    }
};

int main() {
    // Create custom plasticity configuration
    Config config;
    config.set("neuron_count", 2000);
    config.set("region_count", 2);
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 20.0f);
    config.set("hebbian_learning_rate", 0.001f);
    
    // Run custom plasticity study
    CustomPlasticityExperiment experiment(config);
    results = experiment.runCustomPlasticityStudy(6000);
    
    // Analyze plasticity results
    std::cout << "\nCustom Plasticity Study Complete!" << std::endl;
    std::cout << "Total plasticity observations: " << results.size() << std::endl;
    
    // Calculate plasticity statistics
    float avg_stdp = 0.0f;
    float avg_hebbian = 0.0f;
    float max_plasticity = std::numeric_limits<float>::lowest();
    
    for (const auto& entry : results) {
        avg_stdp += entry.stdp_ltp_weight;
        avg_hebbian += entry.hebbian_learning_rate;
        max_plasticity = std::max(max_plasticity, entry.net_plasticity);
    }
    
    if (!results.empty()) {
        avg_stdp /= results.size();
        avg_hebbian /= results.size();
        
        std::cout << "Plasticity Rule Analysis:" << std::endl;
        std::cout << "  Average STDP strength: " << avg_stdp << std::endl;
        std::cout << "  Average Hebbian strength: " << avg_hebbian << std::endl;
        std::cout << "  Maximum net plasticity: " << max_plasticity << std::endl;
        
        // Compare different plasticity rules
        std::cout << "\nPlasticity Rule Comparison:" << std::endl;
        std::cout << "  STDP-dominated learning: " << (avg_stdp > avg_hebbian ? "YES" : "NO") << std::endl;
        std::cout << "  Hebbian-dominated learning: " << (avg_hebbian > avg_stdp ? "YES" : "NO") << std::endl;
        
        // Determine optimal rule
        float rule_effectiveness = std::max(avg_stdp, avg_hebbian);
        if (rule_effectiveness > 0.05f) {
            std::cout << "  ✓ Effective plasticity rule found!" << std::endl;
        } else {
            std::cout << "  ✗ Plasticity rules need adjustment" << std::endl;
        }
    }
    
    return 0;
}
```

## Example Build Instructions

### Building Examples

To build and run examples, follow these steps:

```bash
# Navigate to the examples directory
cd examples

# Build all examples
mkdir build
cd build
cmake ..
make -j4

# Or build individual examples
make simple_neural_network
make stdp_learning
make working_memory
make sensorimotor_loop
make reward_learning
make curiosity_driven
make episodic_memory
make concept_formation
make neural_planning
```

### Running Examples

```bash
# Run individual examples
./simple_neural_network
./stdp_learning
./working_memory
./sensorimotor_loop

# Run advanced examples
./episodic_memory
./concept_formation
./neural_planning
```

### Python Research Examples

For research examples, install Python dependencies:

```bash
cd examples/research
pip install -r requirements.txt

# Run research experiments
python developmental.py
python prediction_models.py
python custom_plasticity.py
```

## Example Output

Each example produces detailed output showing:

- **Simulation progress** with step-by-step updates
- **Performance metrics** including accuracy, efficiency, and success rates
- **System statistics** showing neural activity and memory usage
- **Learning curves** tracking progress over time
- **Final analysis** summarizing experiment results

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Tutorial Documentation](TUTORIALS/README.md)
- [Cheat Sheet](CHEAT_SHEET.md)
- [FAQ](FAQ.md)
- [Implementation Notes](IMPLEMENTATION_NOTES.md)
- [Coding Guidelines](CODING_GUIDELINES.md)
