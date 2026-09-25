# NLM Usage Examples
# Neural Learning Machine - Phase 6: Integrated Artificial Brain

This document provides practical examples for getting started with NLM (熙然), from simple demonstrations to advanced experimental setups.

## Prerequisites

### System Requirements
```bash
# Minimum requirements
- RAM: 2 GB
- CPU: Single core (for basic functionality)
- Compiler: C++20 compatible compiler
- Build tools: CMake 3.16 or higher

# Recommended for advanced features
- RAM: 8+ GB (for large networks)
- CPU: Multi-core (for parallel processing)
- GPU: Optional (for SIMD acceleration)
```

### Build Instructions
```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# For development with debugging
mkdir debug_build
cd debug_build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

## Basic Usage Examples

### 1. Simple Neural Network Demonstration

This example shows how to create and run a basic neural network:

```bash
# Create a simple config
cat > simple_config.cfg << EOF
# NLM Configuration for Basic Demonstration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.1
EOF

# Run the basic demo
./nlm --config=simple_config.cfg
```

**Expected Output:**
```
╔══════════════════════════════════════════════════════════════╗
║                    NLM — 熙然                                ║
║     Neural Learning Machine                                   ║
╚══════════════════════════════════════════════════════════════╝

Initializing NLM Phase 2 Real Neural Computation...

=== Test 1: Basic Neural Connectivity ===
  Injecting current into 10 neurons...
  Steps completed: 0-49
  Spikes generated: 15
  ✓ Spikes propagate through network

=== Test 2: Plasticity Learning Experiment ===
  Applying repeated input patterns (1000 steps)...
  Step 100 | Spikes: 45 | Firing: 8
  Step 200 | Spikes: 78 | Firing: 12
  Step 300 | Spikes: 112 | Firing: 16
  ...
  Learning experiment demonstrates measurable changes
  Synaptic changes:
    Strengthened: 23
    Weakened: 8
    Unchanged: 45

=== Test 3: STDP Verification ===
  Testing STDP on 5 synapses:
    Synapse 0 before: 0.025
    Synapse 1 before: -0.015
    Synapse 2 before: 0.030
    Synapse 3 before: -0.020
    Synapse 4 before: 0.028

  After correlated activity:
    Synapse 0 after: 0.047 (Δ=0.022)
    Synapse 1 after: -0.032 (Δ=-0.017)
    Synapse 2 after: 0.058 (Δ=0.028)
    Synapse 3 after: -0.048 (Δ=-0.028)
    Synapse 4 after: 0.053 (Δ=0.025)

  ✓ STDP WORKING: Pre-before-post produced potentiation

=== Final Brain Status ===
  Regions: 2
  Total neurons: 500
  Total synapses: 1245
  ...

=== Phase 2 Complete ===
  ✓ Real LIF neuron dynamics implemented
  ✓ Event-driven spike propagation with delays
  ✓ STDP plasticity rule
  ✓ Hebbian plasticity rule
  ✓ Structural plasticity (synaptogenesis/pruning)
  ✓ Learning experiment demonstrates measurable changes
  ✓ Network shows activity-dependent synaptic modification
```

### 2. Advanced Configuration with Command Line

This example demonstrates advanced configuration options:

```bash
# Run with custom configuration and parameters
./nlm \
  --config=configs/production.cfg \
  --neuron_count=2000 \
  --region_count=4 \
  --timestep=0.002 \
  --log_level=Info \
  --plasticity=enable \
  --speed=2.0 \
  --checkpoint=./checkpoints \
  --seed=12345 \
  --log_colors=true \
  --log_file=nlm_production.log
```

**Configuration File (production.cfg):**
```
# NLM Configuration for Production Environment
random_seed = 12345
simulation_timestep = 0.002
neuron_count = 2000
region_count = 4
connection_probability = 0.15
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
stdp_tau = 25.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
environment_name = ComplexWorld
environment_width = 20
environment_height = 20
log_level = INFO
log_to_file = true
log_filename = nlm_production.log
max_simulation_steps = 50000
simulation_time_limit = 300.0
```

### 3. Experimental Mode with Metrics Collection

This example shows how to run experiments with detailed metrics:

```bash
# Run Phase 4 experiment (cognitive capabilities)
./nlm_phase4_demo --config=experiments/cognitive_test.cfg

# Run Phase 5 experiment (lifetime development)
./nlm_phase5_demo --config=experiments/developmental.cfg \
                --duration=1000 \
                --save_interval=100 \
                --output_format=csv

# Run scaling benchmark
./benchmark \
  --neurons="100,500,1000,2000" \
  --steps=1000 \
  --output=benchmark_results.json \
  --visualize
```

**Configuration Example (cognitive_test.cfg):**
```
# NLM Configuration for Cognitive Experiment
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 3
connection_probability = 0.1

# Cognitive experiment parameters
experimental_phase = 4
prediction_accuracy_target = 0.85
working_memory_capacity = 50
episodic_memory_limit = 100
planning_depth = 5
attention_selectivity = 0.8
```

## Integration Examples

### 1. Agent Integration with Sensory-Motor Loop

This example demonstrates integrating NLM with a simulated agent:

```cpp
// Complete agent simulation with NLM
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== NLM Agent Simulation ===" << std::endl;
    
    // Create neural network
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create environment
    SimpleWorld world(10, 10);
    agent->initialize(world);
    
    // Simulation parameters
    const size_t total_steps = 5000;
    const double timestep = 0.001;
    
    // Main simulation loop
    for (size_t step = 0; step < total_steps; ++step) {
        double time = step * timestep;
        
        // Get sensory percept from environment
        SensoryPercept percept = world.getPercept();
        
        // Process through agent brain
        agent->processSensoryInput(percept);
        
        // Get motor command
        MotorCommand action = agent->decodeMotorCommand();
        
        // Execute action
        world.executeAction(action);
        
        // Update NLM brain
        brain->step(step, time);
        
        // Apply reward
        float reward = world.getReward();
        float predicted_reward = agent->getPredictionError();
        agent->applyRewardModulation(reward, predicted_reward);
        
        // Update development
        agent->updateDevelopment(timestep);
        
        // Log progress every 500 steps
        if (step % 500 == 0) {
            std::cout << "Step " << step << ": "
                      << "Action: " << action << ", "
                      << "Reward: " << reward << ", "
                      << "Neurons firing: " << brain->getFiringNeuronCount()
                      << std::endl;
        }
        
        // Save checkpoint every 1000 steps
        if (step % 1000 == 0 && step > 0) {
            std::string filename = "agent_checkpoint_step_" + std::to_string(step) + ".nlm";
            brain->save(filename);
            std::cout << "Checkpoint saved: " << filename << std::endl;
        }
    }
    
    // Print final statistics
    std::cout << std::endl << "=== Simulation Complete ===" << std::endl;
    brain->logStatus();
    
    return 0;
}
```

### 2. Developmental Learning Experiment

This example demonstrates developmental learning over extended periods:

```cpp
// Developmental learning simulation
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include <fstream>
#include <chrono>

int main() {
    std::cout << "=== NLM Developmental Learning Experiment ===" << std::endl;
    
    // Configure for development
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.08f);
    config->set("plasticity_learning_rate", 0.01f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00005f);
    
    auto brain = std::make_shared<Brain>(config);
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Enable all developmental features
    agent->enableDevelopment(true);
    agent->enableStructuralPlasticity(true);
    agent->enableCuriosity(true);
    agent->enableRewardModulation(true);
    
    // Create environment
    SimpleWorld world(8, 8);
    agent->initialize(world);
    
    // Developmental timeline
    const double total_development_time = 600.0;  // 10 minutes of simulated time
    const double timestep = 0.001;
    const size_t steps_per_update = 100;
    
    // Developmental phases
    struct DevelopmentalPhase {
        double start_age;
        double end_age;
        std::string name;
        float plasticity_modifier;
    };
    
    std::vector<DevelopmentalPhase> phases = {
        {0.0, 60.0, "Infant", 1.0f},      // High plasticity
        {60.0, 180.0, "Child", 0.8f},    // Moderate plasticity
        {180.0, 360.0, "Adolescent", 0.6f}, // Lower plasticity
        {360.0, total_development_time, "Adult", 0.3f} // Stable
    };
    
    size_t phase_index = 0;
    double current_age = 0.0;
    
    // Developmental learning loop
    while (current_age < total_development_time) {
        // Get current phase
        DevelopmentalPhase current_phase = phases[phase_index];
        
        // Run simulation segment
        for (size_t step = 0; step < steps_per_update; ++step) {
            double time = current_age * timestep;
            
            // Get environment percept
            SensoryPercept percept = world.getPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Get action
            MotorCommand action = agent->decodeMotorCommand();
            
            // Execute
            world.executeAction(action);
            
            // Update brain
            brain->step(current_age, time);
            
            // Apply reward
            float reward = world.getReward();
            agent->applyRewardModulation(reward, agent->getPredictionError());
        }
        
        // Advance development
        current_age += steps_per_update * timestep * 100.0;  // 100 simulated seconds per update
        
        // Update developmental stage
        agent->updateDevelopment(steps_per_update * timestep);
        
        // Update phase if needed
        if (phase_index < phases.size() - 1 && 
            current_age >= phases[phase_index + 1].start_age) {
            phase_index++;
            std::cout << "Developmental Phase: " << phases[phase_index].name 
                      << " (Age: " << current_age << "s)" << std::endl;
        }
        
        // Save developmental checkpoint
        if (std::fmod(current_age, 100.0) < timestep * 100.0) {
            std::string filename = "developmental_checkpoint_" + 
                                  std::to_string((int)current_age) + ".nlm";
            brain->save(filename);
            
            // Save agent state
            std::string agent_filename = "agent_checkpoint_" + 
                                        std::to_string((int)current_age) + ".nlm";
            std::ofstream agent_file(agent_filename);
            // TODO: Implement agent state serialization
        }
        
        // Print progress
        if (std::fmod(current_age, 500.0) < timestep * 1000.0) {
            std::cout << "Progress: " << (current_age / total_development_time * 100) << "% "
                      << "(Age: " << current_age << "s, "
                      << "Phase: " << phases[phase_index].name << ")" 
                      << std::endl;
        }
    }
    
    // Final analysis
    std::cout << std::endl << "=== Developmental Learning Complete ===" << std::endl;
    std::cout << "Final developmental stage: " << 
                  static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    std::cout << "Final curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "Final reward sensitivity: " << agent->getNeuromodulationLevel() << std::endl;
    
    // Print brain statistics
    brain->logStatus();
    
    return 0;
}
```

## Command-Line Interface Examples

### 1. Basic Configuration

```bash
# Use default configuration
./nlm

# Specify custom configuration file
./nlm --config=my_config.cfg

# Use environment-specific configuration
./nlm --config=configs/development.cfg
```

### 2. Parameter Override

```bash
# Override specific parameters
./nlm \
  --config=configs/production.cfg \
  --neuron_count=1500 \
  --timestep=0.002 \
  --plasticity=disable

# Enable debugging and verbose output
./nlm \
  --log_level=Debug \
  --log_colors=false \
  --checkpoint=./checkpoints \
  --save_every=100
```

### 3. Experimental Control

```bash
# Run with specific experimental parameters
./nlm \
  --experimental_mode=phase4 \
  --target_accuracy=0.9 \
  --memory_capacity=100 \
  --planning_depth=3 \
  --output_dir=./experimental_results \
  --results_format=json

# Run benchmark with custom settings
./benchmark \
  --neurons="100,500,1000,2000" \
  --steps=500 \
  --output=benchmark.csv \
  --compare \
  --show_details
```

## Performance Optimization Examples

### 1. Large Network Configuration

```cpp
// Optimize for very large networks
auto config = std::make_shared<Config>();
config->set("neuron_count", 10000);
config->set("region_count", 10);
config->set("connection_probability", 0.01f);  // Sparse connectivity
config->set("simulation_timestep", 0.01);  // Larger time steps
config->set("max_simulation_steps", 10000);
config->set("replay_interval", 500);
config->set("consolidation_interval", 5000);
```

### 2. Real-Time Performance

```cpp
// Optimize for real-time performance
config->set("simulation_timestep", 0.001);  // High accuracy
config->set("neuron_count", 2000);        // Manageable size
config->set("region_count", 4);           // Good parallelism

// Enable parallel processing
brain->enableParallelProcessing(true);
brain->enableSIMDAcceleration(true);
```

## Troubleshooting Examples

### 1. Configuration Errors

**Problem:** "Invalid configuration! Missing required parameter."

**Solution:**
```bash
# Check if all required parameters are set
./nlm --config=my_config.cfg

# Add missing parameters
./nlm \
  --config=my_config.cfg \
  --neuron_count=1000 \
  --region_count=2 \
  --timestep=0.001
```

### 2. Memory Issues

**Problem:** "Out of memory during initialization."

**Solutions:**
```bash
# Reduce network size
./nlm --neuron_count=500 --region_count=1

# Increase system resources
./nlm --neuron_count=2000 --region_count=4

# Use more efficient configuration
./nlm \
  --neuron_count=1000 \
  --connection_probability=0.05 \
  --simulation_timestep=0.002
```

### 3. Performance Issues

**Problem:** "Simulation too slow for real-time."

**Solutions:**
```cpp
// Optimize configuration
config->set("simulation_timestep", 0.01);  // Larger time steps
config->set("connection_probability", 0.01);  // Fewer synapses
config->set("neuron_count", 500);  // Smaller network

// Monitor performance
brain->logStatus();  // Shows resource usage and bottlenecks
```

## Conclusion

These examples provide a comprehensive introduction to working with NLM (熙然). From simple neural network demonstrations to complex developmental experiments, NLM offers flexibility for various use cases.

**Key takeaways:**

1. **Start simple**: Begin with basic configuration and gradually add complexity
2. **Validate configurations**: Always check configuration validity before initialization
3. **Monitor performance**: Use built-in logging and benchmarking tools
4. **Use appropriate parameters**: Match configuration to your specific requirements
5. **Leverage experimental features**: Use the experimental modes for research and development

NLM's integrated architecture makes it suitable for:
- Educational demonstrations
- Research experiments
- Production applications
- Real-time simulations
- Large-scale neural network studies

For more detailed information about specific features, refer to the NLM API Documentation.
