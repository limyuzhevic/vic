# NLM User Guides and Tutorials

## Getting Started with NLM (Neural Learning Machine)

This guide provides comprehensive tutorials for getting started with the NLM experimental artificial brain system. NLM implements real spiking neural computation with integrated memory, neuromodulation, prediction, and learning systems.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Basic Tutorials](#basic-tutorials)
   - [Introduction to NLM](#introduction-to-nlm)
   - [Creating and Running a Simple Brain](#creating-and-running-a-simple-brain)
   - [Understanding Neural Activity](#understanding-neural-activity)
3. [Intermediate Tutorials](#intermediate-tutorials)
   - [Working with AgentBrains](#working-with-agentbrains)
   - [Memory Systems](#memory-systems)
   - [Neuromodulation and Learning](#neuromodulation-and-learning)
4. [Advanced Tutorials](#advanced-tutorials)
   - [Developmental Stages](#developmental-stages)
   - [Plasticity and Learning](#plasticity-and-learning)
   - [Performance Optimization](#performance-optimization)
5. [Best Practices](#best-practices)
6. [Troubleshooting](#troubleshooting)
7. [Further Reading](#further-reading)

## Quick Start

If you're new to NLM, here's how to get started quickly:

### 1. Install NLM
```bash
pip install pynlm
```

### 2. Create Your First Brain
```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with your configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Ready to run simulations!")
```

### 3. Run a Simple Simulation
```python
# Run 100 simulation steps
for step in range(100):
    brain.step(step)
    
    # Inject some current to create activity
    if step % 10 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 20.0f)

print(f"Simulation complete!")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

That's it! You've just run your first NLM simulation!

## Basic Tutorials

### Introduction to NLM

#### What is NLM?

NLM (Neural Learning Machine) is an experimental artificial brain project that:

- **Implements real spiking neural computation** using Leaky Integrate-and-Fire (LIF) neurons
- **Features integrated memory systems** (working, episodic, associative)
- **Includes neuromodulation** (dopamine, curiosity, novelty, prediction error)
- **Supports complex learning** through STDP and Hebbian plasticity
- **Models developmental stages** that affect neural plasticity
- **Combines prediction and cognition** systems

#### Key Components

1. **Brain**: Central neural system implementing real computation
2. **AgentBrain**: Interface connecting brain to environment
3. **SimpleWorld**: Simulated 2D environment for interaction
4. **Memory Systems**: Working, episodic, and associative memory
5. **Neuromodulation**: Reward, curiosity, novelty processing
6. **Plasticity Rules**: STDP, Hebbian, and structural plasticity

### Creating and Running a Simple Brain

This tutorial walks you through creating and running your first NLM brain.

#### Step 1: Set Up Your Environment
```python
import pynlm
import matplotlib.pyplot as plt  # For visualization (optional)

# Set random seed for reproducible results
pynlm.setRandomSeed(42)
```

#### Step 2: Configure Your Brain
```python
# Create a custom configuration
config = pynlm.createDefaultConfig()

# Adjust parameters for your needs
config.neuron_count = 500      # Number of neurons
config.region_count = 2        # Number of brain regions
config.simulation_timestep = 0.001  # Simulation timestep
config.connection_probability = 0.15  # Connection strength
config.random_seed = 42        # For reproducibility

print("Configuration:")
print(f"  Neurons: {config.neuron_count}")
print(f"  Regions: {config.region_count}")
print(f"  Timestep: {config.simulation_timestep}s")
```

#### Step 3: Create and Initialize the Brain
```python
# Create the brain
brain = pynlm.createBrain(config)

# Initialize (this sets up neurons, synapses, and memory systems)
if brain.initialize():
    print("✓ Brain initialized successfully!")
    print(f"  - {brain.getTotalNeuronCount()} neurons")
    print(f"  - {brain.getTotalSynapseCount()} synapses")
    print(f"  - {brain.getRegionCount()} regions")
else:
    print("✗ Brain initialization failed!")
    exit(1)
```

#### Step 4: Run Your First Simulation
```python
# Record initial state
initial_spikes = brain.getTotalSpikeCount()

# Run simulation steps
print("\nRunning simulation...")
for step in range(200):
    # Run one simulation step
    brain.step(step)
    
    # Inject current every 10 steps to create activity
    if step % 10 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 30.0f)
        
        # Log progress
        if step % 50 == 0:
            print(f"  Step {step}: {brain.getTotalSpikeCount() - initial_spikes} new spikes")

# Analyze results
final_spikes = brain.getTotalSpikeCount()
spike_count = final_spikes - initial_spikes
avg_firing = brain.getAverageFiringRate()

print(f"\nSimulation Results:")
print(f"  Total steps: 200")
print(f"  Total spikes: {final_spikes}")
print(f"  New spikes: {spike_count}")
print(f"  Average firing rate: {avg_firing:.2f} Hz")
print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### Understanding Neural Activity

#### Monitoring Brain State
```python
# Get comprehensive brain status
brain.logStatus()

# Access specific neural statistics
print(f"\nNeural Statistics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Active neurons: {brain.getActiveNeuronCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"  Developmental stage: {stage}")

# Get neuromodulation levels (if you have an agent brain)
# agent_brain = pynlm.createAgentBrain(brain)
# print(f"  Dopamine: {agent_brain.getNeuromodulationLevel():.3f}")
# print(f"  Curiosity: {agent_brain.getCuriosityLevel():.3f}")
```

#### Visualizing Neural Activity
```python
# Optional: Create simple visualization
import matplotlib.pyplot as plt

# Create spike histogram
spike_counts = []
for step in range(100):
    brain.step(step)
    spike_counts.append(brain.getTotalSpikeCount())

plt.figure(figsize=(10, 6))
plt.plot(spike_counts, 'b-', alpha=0.7, linewidth=2)
plt.xlabel('Simulation Step')
plt.ylabel('Total Spikes')
plt.title('NLM Neural Activity Over Time')
plt.grid(True, alpha=0.3)
plt.show()
```

## Intermediate Tutorials

### Working with AgentBrains

#### Creating an AgentBrain
```python
# Create world for agent interaction
world = pynlm.createSimpleWorld()
world.configure(width=80, height=60, visionWidth=16, visionHeight=16)

# Place agent in world
world.setAgentStart(40, 30)

# Create agent brain interface
agent_brain = pynlm.createAgentBrain(brain)

# Initialize with world
agent_brain.initialize(world)

print("AgentBrain initialized!")
print(f"  Sensory input size: {agent_brain.getSensoryInputSize()}")
print(f"  Motor output size: {agent_brain.getMotorOutputSize()}")
```

#### Sensory Processing
```python
# Simulate sensory input
print("\nProcessing sensory input...")

# Get current world state
percept = world.getSensoryPercept()

# Visualize sensory input
print("Vision input (first 10 pixels):")
vision = percept.getVision()
for i in range(min(10, len(vision))):
    print(f"  [{i//4},{i%4}] = {vision[i]:.3f}")

# Process through agent brain
agent_brain.processSensoryInput(percept)
print("✓ Sensory processing complete")
```

#### Motor Decoding
```python
# Decode motor command from neural activity
print("\nDecoding motor command...")

command = agent_brain.decodeMotorCommand()
print(f"Decoded command: {command}")

# Convert to human-readable format
command_names = {
    pynlm.MotorCommand.MoveForward: "Move Forward",
    pynlm.MotorCommand.MoveBackward: "Move Backward", 
    pynlm.MotorCommand.TurnLeft: "Turn Left",
    pynlm.MotorCommand.TurnRight: "Turn Right",
    pynlm.MotorCommand.Interact: "Interact",
    pynlm.MotorCommand.Wait: "Wait"
}

print(f"Action: {command_names.get(command, 'Unknown')}")
```

#### Neuromodulation and Learning
```python
# Apply reward signal
print("\nApplying reward modulation...")

# Simulate agent action and get reward
result = world.applyMotorCommand(command, 0.0)
print(f"Action result: Reward = {result.reward}, Success = {result.success}")

# Apply reward modulation to agent brain
agent_brain.applyRewardModulation(result.reward, agent_brain.getPredictionError())

# Check neuromodulation levels
print(f"Neuromodulation levels:")
print(f"  Dopamine: {agent_brain.getNeuromodulationLevel():.3f}")
print(f"  Curiosity: {agent_brain.getCuriosityLevel():.3f}")
print(f"  Novelty: {agent_brain.getNoveltyLevel():.3f}")
print(f"  Prediction Error: {agent_brain.getPredictionError():.3f}")
```

### Memory Systems

#### Working Memory
```python
# Access working memory system
working_memory = brain.getWorkingMemory()

print("\nWorking Memory Operations:")

# Store information
working_memory.store("current_location", "room_A", 0.0)
working_memory.store("energy_level", 0.8, 0.0)
working_memory.store("threat_detected", false, 0.5)

# Retrieve information
location = working_memory.retrieve<std::string>("current_location", 10.0)
energy = working_memory.retrieve<float>("energy_level", 10.0)
threat = working_memory.retrieve<bool>("threat_detected", 10.0)

print(f"  Retrieved location: {location}")
print(f"  Retrieved energy: {energy}")
print(f"  Retrieved threat: {threat}")

# Check memory decay
print(f"  Memory after 15 time units:")
print(f"    Location still available: {working_memory.has("current_location", 15.0)}")
print(f"    Energy still available: {working_memory.has("energy_level", 15.0)}")
```

#### Episodic Memory
```python
# Access episodic memory system
episodic_memory = brain.getEpisodicMemory()

print("\nEpisodic Memory Operations:")

# Create percept and action for episode
percept = pynlm.SensoryPercept()
vision_data = [0.1] * 16
percept.setVision(vision_data)

action = pynlm.Action(pynlm.ActionType.MoveForward)

# Store episode
episode_id = episodic_memory.storeEpisode(
    percept=percept,
    action=action,
    reward=1.0,
    timestamp=0.0
)

print(f"  Stored episode with ID: {episode_id}")

# Replay episode and collect statistics
replay_stats = []
def collect_stats(percept, action, reward):
    replay_stats.append({
        'reward': reward,
        'action_type': action.getType()
    })

episodic_memory.replayEpisode(episode_id, collect_stats)

print(f"  Replayed {len(replay_stats)} events")
if replay_stats:
    total_replay_reward = sum(s['reward'] for s in replay_stats)
    print(f"  Total replay reward: {total_replay_reward}")
```

### Neuromodulation and Learning

#### Reward Prediction Error
```python
# Understand reward prediction error
print("\nReward Prediction Error Analysis:")

# Simulate different scenarios
scenarios = [
    (1.0, 0.5),   # Reward > predicted (positive surprise)
    (0.5, 0.8),   # Reward < predicted (negative surprise)
    (0.8, 0.8),   # Reward == predicted (no surprise)
]

for actual_reward, predicted_reward in scenarios:
    agent_brain.applyRewardModulation(actual_reward, predicted_reward)
    
    error = agent_brain.getPredictionError()
    dopamine = agent_brain.getNeuromodulationLevel()
    
    print(f"  Actual: {actual_reward}, Predicted: {predicted_reward}")
    print(f"    Prediction Error: {error:.3f}")
    print(f"    Dopamine signal: {dopamine:.3f}")
    
    # Interpret the signal
    if error > 0.1:
        print(f"    → Learning: Reward better than expected")
    elif error < -0.1:
        print(f"    → Learning: Reward worse than expected")
    else:
        print(f"    → Learning: Outcome matches expectations")
```

#### Curiosity and Exploration
```python
# Study curiosity-driven exploration
print("\nCuriosity-Driven Exploration:")

# Simulate different novelty levels
novelty_levels = [0.0, 0.2, 0.5, 0.8, 1.0]

for novelty in novelty_levels:
    # Set novelty level
    agent_brain.applyRewardModulation(0.0, -novelty)  # Negative prediction error for novelty
    
    # Get resulting curiosity
    curiosity = agent_brain.getCuriosityLevel()
    
    # Calculate exploration probability
    explore_prob = min(0.5, curiosity * 2.0)  # Example exploration function
    
    print(f"  Novelty: {novelty:.1f} → Curiosity: {curiosity:.3f} → Exploration prob: {explore_prob:.3f}")

# Simulate random exploration choice
print(f"\nRandom exploration example:")
import random
for i in range(5):
    choice = agent_brain.getRandomMotorCommand()
    action_name = command_names.get(choice, 'Unknown')
    print(f"  Choice {i+1}: {action_name}")
```

## Advanced Tutorials

### Developmental Stages

#### Understanding Development
```python
# Study developmental stages
print("\nDevelopmental Stage Analysis:")

# Create agent brain for development testing
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Check initial state
initial_stage = agent_brain.getDevelopmentalStage()
initial_plasticity = agent_brain.getBrain().getDevelopmentSystem().getPlasticityRate()

print(f"  Initial stage: {initial_stage}")
print(f"  Initial plasticity: {initial_plasticity}")

# Simulate development over time
print(f"\nDevelopment over time:")
for time_step in [0, 30, 60, 120, 240, 480]:
    agent_brain.updateDevelopment(time_step * 0.1)  # 0.1 time units per step
    
    stage = agent_brain.getDevelopmentalStage()
    plasticity = agent_brain.getBrain().getDevelopmentSystem().getPlasticityRate()
    
    print(f"  Time {time_step}: {stage} (plasticity: {plasticity:.4f})")

# Note: Different stages affect learning capability
print(f"\nDevelopmental implications:")
print(f"  • Initial stage: High plasticity, rapid learning")
print(f"  • Critical period: High plasticity, sensitive learning")
print(f"  • Maturation: Moderate plasticity, selective learning")
print(f"  • Adult: Low plasticity, stable behavior")
```

#### Developmental Plasticity
```python
# Study plasticity changes with development
print("\nDevelopmental Plasticity Changes:")

# Track plasticity over extended period
plasticity_history = []
for day in range(0, 31, 5):  # Every 5 days for 30 days
    # Convert days to simulation time (assuming 1 day = 600 time units)
    time_units = day * 600
    
    agent_brain.updateDevelopment(time_units * 0.1)  # 0.1 per step
    
    plasticity = agent_brain.getBrain().getDevelopmentSystem().getPlasticityRate()
    plasticity_history.append((day, plasticity))
    
    print(f"  Day {day}: Plasticity = {plasticity:.4f}")

# Analyze plasticity trajectory
print(f"\nPlasticity analysis:")
initial_plasticity = plasticity_history[0][1]
final_plasticity = plasticity_history[-1][1]
drop_percentage = ((initial_plasticity - final_plasticity) / initial_plasticity) * 100

print(f"  Initial plasticity: {initial_plasticity:.4f}")
print(f"  Final plasticity: {final_plasticity:.4f}")
print(f"  Plasticity reduction: {drop_percentage:.1f}%")

# Implications for learning
print(f"\nLearning implications:")
print(f"  • Early development: Fast, flexible learning")
print(f"  • Later development: Slower, stable learning")
print(f"  • Maturity: Refinement of established behaviors")
```

### Plasticity and Learning

#### Hebbian Learning
```python
# Demonstrate Hebbian learning (neurons that fire together wire together)
print("\nHebbian Learning Demonstration:")

# Reset and reinitialize for clean test
brain.reset()
brain.initialize()

# Get regions for testing
regions = brain.getRegions()
if (regions.size() > 0) {
    // Enable Hebbian plasticity
    for (const auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, false, false);  // Only Hebbian
        }
    }

    // Create correlated activity
    auto neurons = regions[0]->getAllNeurons();
    if (neurons.size() >= 3) {
        // Pre-synaptic neuron
        neurons[0]->injectCurrent(40.0f);
        brain.step(0, 0.0);
        
        // Post-synaptic neuron
        neurons[1]->injectCurrent(40.0f);
        brain.step(1, 0.001);
        
        // Record initial weights
        float initial_weight = neurons[0]->getOutgoingSynapses()[0]->getWeight();
        
        // Repeat correlated activity (Hebbian strengthening)
        for (int i = 0; i < 10; ++i) {
            neurons[0]->injectCurrent(30.0f);
            brain.step(i * 2, i * 2 * 0.001);
            
            neurons[1]->injectCurrent(30.0f);
            brain.step(i * 2 + 1, (i * 2 + 1) * 0.001);
        }
        
        // Check weight change
        float final_weight = neurons[0]->getOutgoingSynapses()[0]->getWeight();
        float weight_change = final_weight - initial_weight;
        
        print(f"  Initial weight: {initial_weight:.4f}")
        print(f"  Final weight: {final_weight:.4f}")
        print(f"  Weight change: {weight_change:.4f}")
        
        if (weight_change > 0.01) {
            print(f"  ✓ Hebbian learning: Synapses strengthened")
        } else {
            print(f"  ! Limited Hebbian learning: Consider adjusting parameters")
        }
    }
}
```

#### Spike-Timing-Dependent Plasticity (STDP)
```python
# Demonstrate STDP (pre-before-post → LTP, post-before-pre → LTD)
print("\nSpike-Timing-Dependent Plasticity (STDP):")

// Reset and initialize for clean test
brain.reset()
brain.initialize()

// Get STDP system
auto* stdp = brain.getSTDP();
if (stdp) {
    // Set up synaptic parameters
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(false, true, false);  // Only STDP
        }

        // Get pre- and post-synaptic neurons
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            // Record initial weights
            float initial_pre_weight = neurons[0]->getOutgoingSynapses()[0]->getWeight();
            
            // --- LTP: Pre before post ---
            std::cout << "  Testing LTP (pre → post):" << std::endl;
            
            // Pre fires
            neurons[0]->injectCurrent(50.0f);
            brain.step(0, 0.0);
            
            // Post fires after 20ms
            neurons[1]->injectCurrent(50.0f);
            brain.step(20, 20 * 0.001);
            
            float ltp_weight = neurons[0]->getOutgoingSynapses()[0]->getWeight();
            float ltp_change = ltp_weight - initial_pre_weight;
            
            std::cout << "    Initial weight: " << std::fixed << std::setprecision(4) << initial_pre_weight << std::endl;
            std::cout << "    Weight after LTP: " << std::setprecision(4) << ltp_weight << std::endl;
            std::cout << "    Change: " << std::setprecision(4) << ltp_change << std::endl;
            
            if (ltp_change > 0.001) {
                std::cout << "    ✓ LTP confirmed: Pre-before-post strengthened synapse" << std::endl;
            }

            // --- LTD: Post before pre ---
            std::cout << "\n  Testing LTD (post → pre):" << std::endl;
            
            // Record weight after LTP
            float weight_after_ltp = neurons[0]->getOutgoingSynapses()[0]->getWeight();
            
            // Post fires first
            neurons[1]->injectCurrent(50.0f);
            brain.step(40, 40 * 0.001);
            
            // Pre fires after 20ms
            neurons[0]->injectCurrent(50.0f);
            brain.step(60, 60 * 0.001);
            
            float ltd_weight = neurons[0]->getOutgoingSynapses()[0]->getWeight();
            float ltd_change = ltd_weight - weight_after_ltp;
            
            std::cout << "    Weight after LTP: " << std::setprecision(4) << weight_after_ltp << std::endl;
            std::cout << "    Weight after LTD: " << std::setprecision(4) << ltd_weight << std::endl;
            std::cout << "    Change: " << std::setprecision(4) << ltd_change << std::endl;
            
            if (ltd_change < -0.001) {
                std::cout << "    ✓ LTD confirmed: Post-before-pre weakened synapse" << std::endl;
            }
        }
    }
}
```

### Performance Optimization

#### Performance Benchmarking
```python
// Performance benchmarking example
print("\nPerformance Benchmarking:")

// Create performance profiler
auto profiler = pynlm.createPerformanceProfiler("User Benchmark");

// Benchmark different configurations
auto configs = {
    "Small": pynlm.createDefaultConfigWithNeurons(100),
    "Medium": pynlm.createDefaultConfigWithNeurons(500),
    "Large": pynlm.createDefaultConfigWithNeurons(2000)
};

for (const auto& [name, config] : configs) {
    auto result = profiler.benchmarkSimulationSteps(config, 100, name + " brain (100 steps)");
    
    double steps_per_second = result.stepCount / result.timeSeconds;
    std::cout << std::setw(10) << name << ": " 
              << std::setw(8) << std::fixed << std::setprecision(1) << steps_per_second 
              << " steps/second" << std::endl;
}

// Save results
profiler.endSession();
```

#### Memory Management
```python
// Memory management example
print("\nMemory Management:")

// Create brain with memory tracking
brain = pynlm.createBrain(config)
brain.initialize()

// Monitor memory usage
void monitorMemoryUsage() {
    auto memory_info = brain.getMemoryInfo();
    std::cout << std::left << std::setw(20) << "Memory type"
              << std::right << std::setw(15) << "Usage (MB)"
              << std::right << std::setw(15) << "Capacity" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (const auto& [type, usage] : memory_info) {
        std::cout << std::left << std::setw(20) << type
                  << std::right << std::setw(15) << usage.current
                  << std::right << std::setw(15) << usage.capacity << std::endl;
    }
}

// Periodically monitor memory
for (int step = 0; step < 100; step += 10) {
    brain.step(step);
    if (step % 20 == 0) {
        std::cout << "Step " << step << ": ";
        monitorMemoryUsage();
    }
}
```

## Best Practices

### 1. Configuration Management
```python
// Best practices for configuration

// 1. Use named configurations for different use cases
auto production_config = pynlm.createProductionConfig()
auto research_config = pynlm.createResearchConfig()
auto testing_config = pynlm.createTestingConfig()

// 2. Validate configuration before use
bool is_valid = config.validate();
if (!is_valid) {
    throw std::runtime_error("Invalid configuration");
}

// 3. Use appropriate settings for your use case
if (use_case == "real_time") {
    config.simulation_timestep = 0.0001;  // Fast updates
    config.neuron_count = 100;  // Small for speed
} else if (use_case == "high_accuracy") {
    config.simulation_timestep = 0.01;  // Accurate dynamics
    config.neuron_count = 2000;  // Large for detail
}
```

### 2. Simulation Best Practices
```python
// Best practices for simulation

// 1. Initialize everything before simulation
brain.initialize();
world.initialize();
agent_brain.initialize(world);

// 2. Use consistent timesteps
const double timestep = config.simulation_timestep;
for (size_t step = 0; step < total_steps; ++step) {
    brain.step(step, step * timestep);
}

// 3. Record initial state for comparison
auto initial_state = brain.captureState();

// 4. Run simulation with proper error handling
try {
    for (size_t step = 0; step < total_steps; ++step) {
        brain.step(step);
    }
} catch (const std::exception& e) {
    std::cerr << "Simulation failed: " << e.what() << std::endl;
    // Handle error appropriately
}

// 5. Analyze results thoroughly
auto final_state = brain.captureState();
auto comparison = brain.compareStates(initial_state, final_state);
```

### 3. Memory Management
```python
// Best practices for memory management

// 1. Monitor memory usage
auto memory_info = brain.getMemoryInfo();
for (const auto& [type, usage] : memory_info) {
    if (usage.current > usage.capacity * 0.9) {
        std::cout << "Warning: High memory usage for " << type << std::endl;
    }
}

// 2. Clean up unused objects
brain.clearWorkingMemory();
episodic_memory.clearOldEpisodes(max_age_seconds);

// 3. Use checkpointing for long simulations
if (step % checkpoint_interval == 0) {
    brain.saveCheckpoint(f"checkpoint_{step}.bin");
}

// 4. Restore from checkpoint when needed
if (load_checkpoint) {
    brain.loadCheckpoint("checkpoint.bin");
}
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Brain Initialization Fails
**Problem:** Brain initialization fails with error messages.

**Solution:**
```python
// Check configuration validity
if (!config.validate()) {
    std::cout << "Configuration validation failed:" << std::endl;
    config.printErrors();
}

// Check system resources
auto system_info = pynlm.getSystemInfo();
if (system_info.memory_available < required_memory) {
    std::cout << "Insufficient memory for brain of size " 
              << config.neuron_count << " neurons" << std::endl;
}

// Try with smaller configuration
config.neuron_count = 100;  // Start small
config.connection_probability = 0.05;  // Less connections
```

#### Issue 2: Simulation Runs Too Slow
**Problem:** Simulation performance is unsatisfactory.

**Solution:**
```python
// Profile performance
auto profiler = pynlm.createPerformanceProfiler("Performance Analysis");
profiler.benchmarkSimulationSteps(config, 100, "Current performance");
profiler.endSession();

// Optimize configuration
config.simulation_timestep = 0.005;  // Larger timestep
config.neuron_count = 200;  // Fewer neurons
config.region_count = 1;  // Fewer regions

// Enable optimizations
config.enable_optimizations = true;
config.use_simd = true;
config.enable_memory_pools = true;
```

#### Issue 3: Neural Activity is Too Low
**Problem:** Brain generates insufficient neural activity.

**Solution:**
```python
// Check injection parameters
brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 50.0f);
brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 10.0f);

// Adjust threshold parameters
for (const auto& region : brain.getRegions()) {
    for (auto& neuron : region->getAllNeurons()) {
        neuron.setThreshold(-45.0f);  // Lower threshold
        neuron.setRestingPotential(-65.0f);  // More depolarized
    }
}

// Increase connectivity
config.connection_probability = 0.3;  // More connections
```

#### Issue 4: Memory Usage Grows Uncontrolled
**Problem:** Memory usage increases during simulation without bound.

**Solution:**
```python
// Enable memory management
brain.enableMemoryManagement(true);
brain.setMemoryLimit(1024 * 1024 * 1024);  // 1GB limit

// Use memory-efficient data structures
brain.enableCompactMemoryFormat(true);

// Implement periodic cleanup
for (step in range(simulation_steps)) {
    brain.step(step);
    
    if (step % 1000 == 0) {
        brain.cleanMemory();
        brain.saveMemorySnapshot(f"snapshot_{step}.bin");
    }
}
```

## Further Reading

### Recommended Documentation

1. **Architecture Documentation**: `docs/ARCHITECTURE.md` - Detailed technical architecture
2. **Scientific Background**: `docs/SCIENCE.md` - Theoretical foundations
3. **Phase Documentation**: `docs/PHASE6_FINAL_AUDIT.md` - Implementation details
4. **Performance Analysis**: `docs/LIMITATIONS.md` - Performance characteristics
5. **Python API Reference**: `python/README.md` - Complete API documentation

### Academic Papers and Resources

- **NLM Research Papers**: Multiple papers on neural learning machine theory
- **Computational Neuroscience Literature**: Related to spiking neural networks
- **Machine Learning Resources**: Integration of neural computation with learning
- **Brain Simulation**: Biologically-inspired artificial systems

### Community and Support

#### Online Resources

- **NLM GitHub Repository**: Source code and examples
- **Documentation Wiki**: Detailed tutorials and examples
- **Discussion Forums**: Community support and questions
- **Slack/Discord**: Real-time support and collaboration

#### Support Channels

1. **Bug Reports**: Report issues via GitHub issues
2. **Feature Requests**: Suggest new features and enhancements
3. **Technical Support**: Consult documentation and examples
4. **Research Collaboration**: Partner on research projects

### Future Directions

#### Research Areas

1. **Advanced Learning**: Deep integration with machine learning
2. **Neuromodulation**: Enhanced models of brain neuromodulation
3. **Developmental Psychology**: Better models of cognitive development
4. **Social Learning**: Multi-agent and social learning systems
5. **Transfer Learning**: Cross-domain knowledge transfer

#### Technical Enhancements

1. **Hardware Acceleration**: GPU and specialized hardware support
2. **Distributed Computing**: Large-scale brain simulation
3. **Real-time Applications**: Interactive brain systems
4. **Hybrid Systems**: Integration with traditional AI methods
5. **Explainability**: Interpretability of neural decision-making

### Getting Help

#### When You Need Assistance

1. **Start with Documentation**: Check the comprehensive guides and API references
2. **Reproduce the Issue**: Create minimal examples to isolate problems
3. **Use Examples**: Look at existing examples for common patterns
4. **Report Issues**: Provide detailed bug reports with reproduction steps
5. **Ask Questions**: Use appropriate channels for different types of questions

#### Documentation Structure

- **Getting Started**: Quick start guides and tutorials
- **Reference**: Complete API documentation and parameter descriptions
- **Examples**: Working code examples and use cases
- **Advanced Topics**: Complex patterns and optimization techniques
- **Troubleshooting**: Common issues and their solutions

### Continuous Learning

#### Stay Updated

- **Release Notes**: Check for new versions and features
- **Documentation Updates**: Regular updates and improvements
- **Community Contributions**: New examples and best practices
- **Research Developments**: Latest findings in neural computation

#### Practice and Experiment

- **Hands-on Learning**: Try examples and modify them
- **Experimental Testing**: Test different configurations and parameters
- **Performance Tuning**: Optimize for your specific use case
- **Code Exploration**: Understand the codebase structure

## Conclusion

The NLM (Neural Learning Machine) provides a powerful framework for implementing experimental artificial brains with real spiking neural computation. This guide has provided you with comprehensive tutorials and best practices for getting started with NLM, from basic usage to advanced techniques.

Key takeaways:

1. **Start Simple**: Begin with basic configurations and gradually add complexity
2. **Understand the Architecture**: Learn the components and their interactions
3. **Optimize for Your Use Case**: Tailor configurations to your specific needs
4. **Monitor and Debug**: Use built-in tools to understand and fix issues
5. **Iterate and Improve**: Continuously refine your implementation

NLM represents a significant step forward in biologically-inspired artificial intelligence, combining real neural computation with integrated learning, memory, and developmental systems. Whether you're a researcher, developer, or student, NLM provides the tools you need to explore the frontiers of neural computation.

Happy neural computation! 🚀