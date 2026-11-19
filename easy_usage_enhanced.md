# NLM - Enhanced Python Usage Guide

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## New Features in Enhanced Edition (v2.0.0)

The enhanced Python bindings provide:

### 1. Comprehensive Error Handling
- Specific exception types for different error conditions
- Detailed error messages with context
- Graceful error recovery and validation

### 2. Convenience Methods
- Batch configuration operations
- Statistics dictionaries for easy data access
- Validation functions for configuration and simulation

### 3. Enhanced Documentation
- Detailed docstrings for all methods
- Comprehensive examples and tutorials
- Progress monitoring and debugging utilities

### 4. Type Safety
- Better Python-C++ type conversions
- Runtime type checking and validation
- Consistent API across all classes

### 5. Performance Monitoring
- Memory and CPU usage tracking
- Progress monitoring during simulations
- Performance profiling tools

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain with Enhanced Features

```python
import pynlm

# Create configuration with enhanced error handling
config = pynlm.createDefaultConfig()

# Configure multiple parameters at once
batch_config = {
    "brain": {
        "neuron_count": 1000,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
    }
}
config = pynlm.batch_config(batch_config)

# Create and initialize brain with error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created successfully: {brain.getTotalNeuronCount()} neurons")
except pynlm.BrainError as e:
    print(f"✗ Failed to create brain: {e}")
    exit(1)

# Run simulation with progress monitoring
print("Running simulation...")
for step in range(1000):
    brain.step(step)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        pynlm.log_progress(f"Step {step}", step / 1000.0)

# Get comprehensive statistics
stats = brain.get_stats()
print(f"\nSimulation Results:")
print(f"  Total neurons: {stats['total_neurons']}")
print(f"  Total synapses: {stats['total_synapses']}")
print(f"  Firing neurons: {stats['firing_neurons']}")
print(f"  Average firing rate: {stats['average_firing_rate']:.2f}")
print(f"  Working memory traces: {stats['working_memory_active_traces']}")
print(f"  Novelty level: {stats['novelty_level']:.3f}")
print(f"  Curiosity level: {stats['curiosity_level']:.3f}")
```

### Example 2: Complete Agent with Enhanced World Integration

```python
import pynlm

# Create complete agent setup with validation
def create_enhanced_agent(config_dict, world_config):
    """Create an agent with enhanced validation and error handling."""
    
    # Validate configuration
    if not pynlm.validate_config(config_dict):
        raise ValueError("Invalid agent configuration")
    
    # Create brain
    brain_config = config_dict.get("brain", {})
    config = pynlm.createDefaultConfig()
    
    # Set brain parameters
    if "neuron_count" in brain_config:
        config.set("brain.neuron_count", brain_config["neuron_count"])
    if "connection_probability" in brain_config:
        config.set("brain.connection_probability", brain_config["connection_probability"])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_config.get("width", 20),
        height=world_config.get("height", 20),
        visionWidth=world_config.get("visionWidth", 8),
        visionHeight=world_config.get("visionHeight", 8)
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    return brain, agent, world

# Create agent
brain_config = {"neuron_count": 500, "connection_probability": 0.1}
world_config = {"width": 15, "height": 15, "visionWidth": 8, "visionHeight": 8}

brain, agent, world = create_enhanced_agent({"brain": brain_config}, world_config)

# Run enhanced simulation
def run_enhanced_simulation(brain, agent, world, num_steps=1000):
    """Run simulation with enhanced monitoring and error handling."""
    
    print("Starting enhanced simulation...")
    print(f"Initial stats: {brain.get_stats()}")
    
    for step in range(num_steps):
        # Update world with timing
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input with error handling
        try:
            agent.processSensoryInput(percept)
        except pynlm.SimulationError as e:
            print(f"Step {step}: Sensory input error: {e}")
            continue
        
        # Run brain step
        try:
            brain.step(step)
        except pynlm.SimulationError as e:
            print(f"Step {step}: Brain step error: {e}")
            break
        
        # Decode motor command
        try:
            motor_cmd = agent.decodeMotorCommand()
        except pynlm.SimulationError as e:
            print(f"Step {step}: Motor decoding error: {e}")
            continue
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        try:
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        except pynlm.SimulationError:
            pass  # Reward modulation is optional
        
        # Update development
        try:
            agent.updateDevelopment(0.1)
        except pynlm.SimulationError:
            pass  # Development is optional
        
        # Progress monitoring
        if step % 100 == 0:
            stats = brain.get_stats()
            pynlm.log_progress(
                f"Step {step}: {stats['firing_neurons']} firing neurons, "
                f"curiosity: {stats['curiosity_level']:.3f}",
                step / num_steps
            )
    
    print("\nSimulation complete!")
    print(f"Final stats: {brain.get_stats()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_enhanced_simulation(brain, agent, world, 1000)
```

### Example 3: Batch Configuration and Validation

```python
import pynlm

# Create multiple configurations using batch operations
configurations = [
    {"name": "small", "neuron_count": 100, "connection_probability": 0.05},
    {"name": "medium", "neuron_count": 500, "connection_probability": 0.1},
    {"name": "large", "neuron_count": 1000, "connection_probability": 0.15},
]

# Create all configurations
for config_dict in configurations:
    config = pynlm.createDefaultConfig()
    
    # Apply batch configuration
    batch_config = {
        "brain": {
            "neuron_count": config_dict["neuron_count"],
            "connection_probability": config_dict["connection_probability"]
        }
    }
    pynlm.batch_set_config(config, batch_config)
    
    # Validate configuration
    if pynlm.validate_config(config):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run quick test
        for step in range(50):
            brain.step(step)
        
        print(f"✓ {config_dict['name']} config: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSynapseCount()} synapses")
```

### Example 4: Checkpoint and Recovery

```python
import pynlm
import tempfile
import os

# Create brain with checkpointing support
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 200)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable checkpointing
agent = pynlm.createAgentBrain(brain)
agent.enableStructuralPlasticity(True)

print("Running simulation with checkpointing...")
save_dir = tempfile.mkdtemp()
checkpoint_file = os.path.join(save_dir, "checkpoint.bin")

# Run some steps
for step in range(500):
    # Create dummy world for this example
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    # Create agent and run
    agent.initialize(world)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save checkpoint periodically
    if step % 100 == 0:
        brain.save(checkpoint_file)
        print(f"Checkpoint saved at step {step}")

# Save final checkpoint
brain.save(checkpoint_file)
print(f"Final checkpoint saved: {checkpoint_file}")

# Verify checkpoint file was created
if os.path.exists(checkpoint_file):
    print(f"✓ Checkpoint file exists: {os.path.getsize(checkpoint_file)} bytes")
    
    # Test loading
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    if brain2.load(checkpoint_file):
        print("✓ Checkpoint loaded successfully")
    else:
        print("✗ Checkpoint load failed")
else:
    print("✗ Checkpoint file not found")

# Cleanup
os.remove(checkpoint_file)
os.rmdir(save_dir)
```

### Example 5: Performance Monitoring and Profiling

```python
import pynlm
import time

# Create brain and run performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Starting performance monitoring...")
start_time = time.time()

# Run simulation with profiling
for step in range(1000):
    brain.step(step)
    
    # Get memory usage
    memory_mb = pynlm.get_memory_usage()
    cpu_percent = pynlm.get_cpu_usage()
    
    # Get current statistics
    stats = brain.get_stats()
    
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {elapsed:.2f}s elapsed, "
              f"Memory: {memory_mb:.1f}MB, CPU: {cpu_percent:.1f}%")

elapsed = time.time() - start_time
print(f"\nPerformance Summary:")
print(f"  Total time: {elapsed:.2f}s")
print(f"  Average time per step: {elapsed/1000:.4f}s")
print(f"  Steps per second: {1000/elapsed:.1f}")
print(f"  Memory usage: {memory_mb:.1f}MB")
print(f"  Final stats: {stats}")
```

## Advanced Features

### Error Handling

The enhanced bindings provide specific exception types:

```python
import pynlm

# Exception types
pynlm.NLMException      # General NLM error
pynlm.ConfigError       # Configuration error
pynlm.BrainError        # Brain-related error
pynlm.SimulationError   # Simulation error

# Error handling example
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.BrainError as e:
    print(f"Brain creation failed: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

### Validation Functions

```python
import pynlm

# Validate configuration
def validate_agent_config(config_dict):
    required_keys = ["brain", "world", "agent"]
    return pynlm.validate_config(config_dict, required_keys)

# Validate simulation setup
def validate_simulation_setup(brain, world, agent):
    # Check if all components are properly initialized
    if not brain or not brain.getTotalNeuronCount():
        return False
    if not world:
        return False
    if not agent:
        return False
    return True

# Validate experiment parameters
def validate_experiment_params(params):
    # Check parameter ranges and types
    if "steps" in params and params["steps"] <= 0:
        return False
    if "neuron_count" in params and params["neuron_count"] <= 0:
        return False
    return True
```

### Progress Monitoring

```python
import pynlm
import time

# Progress monitoring example
print("Starting simulation with progress monitoring...")
start_time = time.time()

for step in range(1000):
    # Run simulation step
    brain.step(step)
    
    # Log progress
    progress = (step + 1) / 1000.0
    pynlm.log_progress(
        f"Processing step {step + 1}/1000",
        progress
    )
    
    # Get estimated time remaining
    elapsed = time.time() - start_time
    steps_per_second = (step + 1) / elapsed
    remaining_steps = 1000 - (step + 1)
    estimated_remaining = remaining_steps / steps_per_second
    
    if step % 100 == 0:
        print(f"  Estimated remaining time: {estimated_remaining:.1f}s")
```

### Batch Operations

```python
import pynlm

# Batch configuration operations
def create_multiple_brains(configs):
    """Create multiple brains with different configurations."""
    brains = []
    
    for i, config_dict in enumerate(configs):
        config = pynlm.createDefaultConfig()
        
        # Apply batch configuration
        pynlm.batch_set_config(config, config_dict)
        
        # Validate configuration
        if pynlm.validate_config(config):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append((i, brain))
            print(f"✓ Brain {i + 1} created: {config_dict.get('name', 'unnamed')}")
        else:
            print(f"✗ Brain {i + 1} configuration invalid")
    
    return brains

# Example usage
configs = [
    {"name": "small", "brain.neuron_count": 100, "brain.connection_probability": 0.05},
    {"name": "medium", "brain.neuron_count": 500, "brain.connection_probability": 0.1},
    {"name": "large", "brain.neuron_count": 1000, "brain.connection_probability": 0.15},
]

brains = create_multiple_brains(configs)
```

### Statistics and Monitoring

```python
import pynlm

# Get comprehensive brain statistics
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get statistics as dictionary
stats = brain.get_stats()

# Print formatted statistics
print("=== Brain Statistics ===")
print(f"Neurons: {stats['total_neurons']:,}")
print(f"Synapses: {stats['total_synapses']:,}")
print(f"Active neurons: {stats['active_neurons']:,}")
print(f"Firing neurons: {stats['firing_neurons']:,}")
print(f"Average firing rate: {stats['average_firing_rate']:.3f}")
print(f"Excitatory/inhibition ratio: {stats['excitatory_inhibition_ratio']:.3f}")
print(f"Total spikes: {stats['total_spikes']:,}")
print(f"Development stage: {stats['developmental_stage']}")
print(f"Working memory traces: {stats['working_memory_active_traces']:,}")
print(f"Episodic memory episodes: {stats['episodic_memory_episodes']:,}")
print(f"Novelty level: {stats['novelty_level']:.3f}")
print(f"Curiosity level: {stats['curiosity_level']:.3f}")
print(f"Prediction error: {stats['prediction_error']:.3f}")
print(f"Dopamine level: {stats['dopamine_level']:.3f}")
print(f"Planner active: {stats['planner_active']}")
print(f"Concept formation active: {stats['concept_formation_active']}")
print(f"Attention active: {stats['attention_active']}")
print(f"Development system active: {stats['development_active']}")
print(f"Prediction system active: {stats['prediction_system_active']}")

# Get agent brain statistics
agent = pynlm.createAgentBrain(brain)
agent_stats = agent.get_stats()
print("\n=== Agent Brain Statistics ===")
print(f"Curiosity level: {agent_stats['curiosity_level']:.3f}")
print(f"Novelty level: {agent_stats['novelty_level']:.3f}")
print(f"Prediction error: {agent_stats['prediction_error']:.3f}")
print(f"Neuromodulation level: {agent_stats['neuromodulation_level']:.3f}")
print(f"Developmental stage: {agent_stats['developmental_stage']}")
print(f"Reward modulation enabled: {agent_stats['reward_modulation_enabled']}")
print_f"Structural plasticity enabled: {agent_stats['structural_plasticity_enabled']}")
print_f"Development enabled: {agent_stats['development_enabled']}")
print_f"Curiosity enabled: {agent_stats['curiosity_enabled']}")
```

## Error Handling Guide

### Common Error Types

1. **ConfigError**: Configuration-related errors
   - Missing required parameters
   - Invalid parameter types
   - File I/O errors

2. **BrainError**: Brain-related errors
   - Initialization failures
   - State corruption
   - Simulation errors

3. **SimulationError**: Simulation-specific errors
   - Step execution failures
   - Resource allocation errors
   - Timing errors

4. **NLMException**: General NLM errors
   - System-level errors
   - Unknown error conditions

### Error Handling Patterns

```python
import pynlm

# Basic error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Success!")
except (pynlm.ConfigError, pynlm.BrainError) as e:
    print(f"Error occurred: {e}")
    # Handle error
    pass

# Graceful degradation
def safe_simulation(config_dict):
    try:
        # Create brain with configuration
        config = pynlm.createDefaultConfig()
        pynlm.batch_set_config(config, config_dict)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        print("Simulation completed successfully")
        return brain
    except (pynlm.ConfigError, pynlm.BrainError, pynlm.SimulationError) as e:
        print(f"Simulation failed: {e}")
        print("Attempting graceful degradation...")
        
        # Try with simpler configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simplified simulation
        for step in range(100):
            brain.step(step)
        
        print("Simplified simulation completed")
        return brain
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise pynlm.NLMException("Simulation failed unexpectedly")
```

### Validation and Error Prevention

```python
import pynlm

# Validate configuration before use
def validate_brain_config(config):
    """Validate brain configuration."""
    required_keys = ["brain.neuron_count", "brain.region_count"]
    
    for key in required_keys:
        if not config.has(key):
            raise pynlm.ConfigError(f"Missing required configuration: {key}")
        
        value = config.getOr<std::string>(key, "")
        # Check for valid ranges
        if key == "brain.neuron_count" and value <= 0:
            raise pynlm.ConfigError(f"Invalid neuron count: {value}")
    
    return True

# Validate simulation setup
def validate_simulation_setup(brain, world, agent):
    """Validate simulation setup."""
    if not brain:
        raise pynlm.BrainError("Brain is not initialized")
    
    if brain.getTotalNeuronCount() <= 0:
        raise pynlm.BrainError("Brain has no neurons")
    
    if not world:
        raise pynlm.SimulationError("World is not initialized")
    
    if not agent:
        raise pynlm.BrainError("Agent is not initialized")
    
    # Check compatibility
    if brain.getTotalNeuronCount() != world.getWidth() * world.getHeight():
        print("Warning: Brain and world dimensions don't match")
    
    return True

# Safe simulation runner
def safe_simulate(brain, world, agent, steps):
    """Run simulation with comprehensive error handling."""
    try {
        validate_simulation_setup(brain, world, agent);
        
        for step in range(steps) {
            world.update(0.1);
            agent.processSensoryInput(world.getSensoryPercept());
            brain.step(step);
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
        }
        
        return true;
    } catch (const pynlm::BrainError& e) {
        std::cerr << "Brain error: " << e.what() << std::endl;
        return false;
    } catch (const pynlm::SimulationError& e) {
        std::cerr << "Simulation error: " << e.what() << std::endl;
        return false;
    } catch (const pynlm::ConfigError& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return false;
    }
}
```

## Advanced Usage Examples

### 1. Multi-threaded Simulation

```python
import pynlm
import threading
import queue

# Thread-safe simulation queue
class SimulationQueue {
public:
    void push(std::pair<std::shared_ptr<Brain>, int> task) {
        std::lock_guard<std::mutex> lock(mutex);
        task_queue.push(task);
        condition.notify_one();
    }
    
    std::pair<std::shared_ptr<Brain>, int> pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]() { return !task_queue.empty(); });
        return task_queue.pop();
    }
    
private:
    std::queue<std::pair<std::shared_ptr<Brain>, int>> task_queue;
    std::mutex mutex;
    std::condition_variable condition;
};

// Multi-threaded simulation
def threaded_simulation(brains, steps_per_brain):
    task_queue = SimulationQueue();
    
    for i, brain in enumerate(brains) {
        task_queue.push({brain, steps_per_brain});
    }
    
    threads = [];
    for i in range(num_threads) {
        thread = threading.Thread(target=run_thread, args=(task_queue));
        threads.append(thread);
        thread.start();
    }
    
    for thread in threads {
        thread.join();
    }
    
def run_thread(task_queue) {
    while (true) {
        auto task = task_queue.pop();
        brain = task.first;
        steps = task.second;
        
        for step in range(steps) {
            brain.step(step);
            
            // Check for cancellation
            if cancellation_flag.load() {
                return;
            }
        }
    }
    }
```

### 2. Parallel Processing

```python
import pynlm
import concurrent.futures

# Parallel brain processing
def parallel_process_brains(brains, func):
    with concurrent.futures.ThreadPoolExecutor() as executor {
        futures = [];
        
        for brain in brains {
            future = executor.submit(func, brain);
            futures.append(future);
        }
        
        results = concurrent.futures.wait(futures);
        
        return [future.result() for future in results.done];
    }

# Parallel simulation
def parallel_simulation(brains, steps):
    def simulate_brain(brain) {
        for step in range(steps) {
            brain.step(step);
        }
        return brain.get_stats();
    }
    
    return parallel_process_brains(brains, simulate_brain);
```

### 3. Memory Management

```python
import pynlm

# Memory management utilities
def memory_aware_simulation(brains, steps, max_memory_mb):
    """Run simulation with memory monitoring."""
    
    memory_tracker = MemoryTracker();
    completed_brains = [];
    
    for brain in brains {
        memory_mb = memory_tracker.get_memory_usage();
        
        if memory_mb + memory_tracker.get_brain_memory_usage() > max_memory_mb {
            print("Memory limit exceeded, waiting...");
            time.sleep(1.0);
            continue;
        }
        
        // Run simulation for this brain
        for step in range(steps) {
            brain.step(step);
            
            // Log memory usage periodically
            if step % 100 == 0 {
                memory_tracker.log_memory_usage(brain);
            }
        }
        
        completed_brains.push_back(brain);
        memory_tracker.record_completed_brain();
    }
    
    return completed_brains;

// Memory tracker class
class MemoryTracker {
public:
    void log_memory_usage(std::shared_ptr<Brain> brain) {
        double memory_mb = get_memory_usage();
        auto stats = brain.get_stats();
        
        std::cout << "Memory usage at step " << step << ": "
                  << std::fixed << std::setprecision(1) << memory_mb << "MB"
                  << " (neurons: " << stats['total_neurons'] << ")"
                  << std::endl;
    }
    
    double get_memory_usage() {
        // Get current memory usage (implementation depends on platform)
        #if defined(__linux__)
        // Linux: read from /proc/self/status
        std::ifstream status_file("/proc/self/status");
        std::string line;
        while (std::getline(status_file, line)) {
            if (line.find("VmRSS:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long rss_kb;
                iss >> key >> rss_kb;
                return rss_kb / 1024.0;  // Convert to MB
            }
        }
        #elif defined(_WIN32)
        // Windows: use Windows API
        // ...
        #endif
        return 0.0;
    }
    
    double get_brain_memory_usage(std::shared_ptr<Brain> brain) {
        // Estimate memory usage for brain
        auto stats = brain.get_stats();
        // Rough estimate: 100 bytes per neuron, 50 bytes per synapse
        return (stats['total_neurons'] * 100 + stats['total_synapses'] * 50) / (1024.0 * 1024.0);
    }
    
    void record_completed_brain() {
        completed_count++;
        last_completion = std::chrono::system_clock::now();
    }
    
private:
    size_t completed_count = 0;
    std::chrono::system_clock::time_point last_completion;
};
```

### 4. Distributed Computing

```python
import pynlm
import socket
import pickle

// Client for distributed computing
class NLMClient {
public:
    NLMClient(std::string host, int port) {
        socket = new_socket(host, port);
    }
    
    std::vector<BrainStats> process_brains(std::vector<BrainConfig> configs) {
        // Send configurations to server
        send_configurations(configs);
        
        // Receive results from server
        return receive_results();
    }
    
    void send_configurations(std::vector<BrainConfig> configs) {
        std::string data = serialize(configs);
        socket.send(data);
    }
    
    std::vector<BrainStats> receive_results() {
        std::string data = socket.receive();
        return deserialize<std::vector<BrainStats>>(data);
    }
    
private:
    boost::asio::ip::tcp::socket socket;
};

// Server for distributed computing
class NLMService {
public:
    NLMService(int port) {
        acceptor = new_acceptor(port);
    }
    
    void start() {
        while (true) {
            auto socket = acceptor.accept();
            std::thread(&NLMService::handle_client, this, std::move(socket)).detach();
        }
    }
    
    void handle_client(boost::asio::ip::tcp::socket socket) {
        while (true) {
            auto data = socket.receive();
            if (data.empty()) break;
            
            auto configs = deserialize<std::vector<BrainConfig>>(data);
            auto results = process_configs(configs);
            
            socket.send(serialize(results));
        }
    }
    
    std::vector<BrainStats> process_configs(std::vector<BrainConfig> configs) {
        std::vector<BrainStats> results;
        
        for (auto& config : configs) {
            auto brain = create_brain_from_config(config);
            
            // Run simulation
            for (int step = 0; step < config.steps; ++step) {
                brain.step(step);
            }
            
            results.push_back(brain.get_stats());
        }
        
        return results;
    }
    
private:
    boost::asio::ip::tcp::acceptor acceptor;
};
```

## Performance Optimization Tips

### 1. Configuration Optimization

```python
import pynlm

# Optimize configuration for performance
def optimize_config_for_performance(config):
    // Use appropriate neuron count for the use case
    neuron_count = config.getOr<int>("brain.neuron_count", 1000);
    
    // Set appropriate connection density
    if neuron_count <= 100:
        config.set("brain.connection_probability", 0.2);
    else if neuron_count <= 500:
        config.set("brain.connection_probability", 0.1);
    else:
        config.set("brain.connection_probability", 0.05);
    
    // Enable performance optimizations
    config.set("plasticity.structural.enable", true);
    config.set("neuromod.dopamine.scale", 1.0);
    
    return config;
```

### 2. Simulation Optimization

```python
import pynlm

// Optimize simulation performance
def optimize_simulation(brain, steps) {
    // Batch processing for better cache utilization
    const size_t batch_size = 100;
    
    for (size_t batch = 0; batch < steps; batch += batch_size) {
        size_t current_batch_size = std::min(batch_size, steps - batch);
        
        // Process batch
        for (size_t i = 0; i < current_batch_size; ++i) {
            brain.step(batch + i);
        }
        
        // Optional: yield to other threads
        if (i % 100 == 0) {
            std::this_thread::yield();
        }
    }
}
```

### 3. Memory Management

```python
import pynlm

// Memory-aware simulation
def memory_aware_simulation(brains, steps, memory_limit_mb) {
    std::vector<BrainStats> results;
    size_t completed_brains = 0;
    
    for (auto& brain : brains) {
        // Check memory
        double current_memory = get_memory_usage();
        double estimated_brain_memory = estimate_brain_memory(brain);
        
        if (current_memory + estimated_brain_memory > memory_limit_mb) {
            std::cout << "Memory limit exceeded, skipping brain" << std::endl;
            continue;
        }
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            brain.step(step);
        }
        
        // Clean up
        results.push_back(brain.get_stats());
        brain.reset(); // Reset brain to free memory
        
        completed_brains++;
        std::cout << "Completed brain " << completed_brains 
                  << "/ " << brains.size() << std::endl;
    }
    
    return results;
}
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain is not initialized"
```python
// Problem: Attempting to use a brain before initialization
// Solution: Always call initialize() before use

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
// brain.initialize()  // <- Add this!
brain.step(0)  // This will fail without initialize()
```

#### 2. "Configuration key not found"
```python
// Problem: Accessing configuration keys that don't exist
// Solution: Use getOr() with default values

config = pynlm.createDefaultConfig()
value = config.getOr<int>("brain.neuron_count", 500)  // Safe access
```

#### 3. "Memory usage too high"
```python
// Problem: Simulation consuming too much memory
// Solution: Use memory-aware simulation and reduce configuration

brain_config = {"brain.neuron_count": 100}  // Reduce neurons
brain = pynlm.createBrain(config)
brain.initialize()

// Run simulation with memory monitoring
memory_aware_simulation([brain], 1000, 100)  // 100MB limit
```

#### 4. "Performance slow"
```python
// Problem: Simulation running slowly
// Solution: Optimize configuration and use batch processing

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)      // Moderate size
config.set("brain.connection_probability", 0.1)  // Reasonable density

// Run optimized simulation
optimized_simulation(brain, 1000)
```

#### 5. "Error in sensory processing"
```python
// Problem: Error in sensory input processing
// Solution: Use error handling and validate input

percept = world.getSensoryPercept()
try {
    agent.processSensoryInput(percept);
} catch (const pynlm::SimulationError& e) {
    std::cout << "Sensory processing error: " << e.what() << std::endl;
    // Handle error or continue
}
```

### Error Diagnosis

#### Use Error Messages for Debugging

```python
import pynlm

// Capture and analyze error messages
def debug_simulation(config_dict) {
    try {
        config = pynlm.createDefaultConfig();
        pynlm.batch_set_config(config, config_dict);
        
        brain = pynlm.createBrain(config);
        brain.initialize();
        
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
        }
        
        return true;
    } catch (const pynlm::ConfigError& e) {
        std::cout << "Configuration error: " << e.what() << std::endl;
        return false;
    } catch (const pynlm::BrainError& e) {
        std::cout << "Brain error: " << e.what() << std::endl;
        return false;
    } catch (const pynlm::SimulationError& e) {
        std::cout << "Simulation error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cout << "Unexpected error: " << e.what() << std::endl;
        return false;
    }
}
```

#### Log and Monitor Performance

```python
import pynlm
import time

// Monitor simulation performance
void monitor_simulation(std::shared_ptr<Brain> brain, int steps) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < steps; ++step) {
        auto step_start = std::chrono::high_resolution_clock::now();
        
        brain.step(step);
        
        auto step_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - step_start
        ).count();
        
        if (step % 100 == 0) {
            std::cout << "Step " << step 
                      << ": " << step_time << "ms per step" 
                      << ", " << 1000.0 / step_time << " steps/sec" 
                      << std::endl;
        }
    }
    
    auto total_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - start_time
    ).count();
    
    std::cout << "Total time: " << total_time << "s" 
              << ", " << steps / total_time << " steps/sec" 
              << std::endl;
}
```

## Conclusion

The enhanced NLM Python bindings (v2.0.0) provide a powerful and flexible interface for working with the Neural Learning Machine. Key features include:

1. **Comprehensive Error Handling**: Specific exception types for different error conditions
2. **Convenience Methods**: Batch operations, validation, and helper functions
3. **Type Safety**: Better Python-C++ type conversions and runtime checking
4. **Performance Monitoring**: Memory and CPU usage tracking, progress monitoring
5. **Documentation**: Detailed docstrings and comprehensive examples
6. **Validation**: Configuration validation and simulation setup verification
7. **Memory Management**: Memory-aware simulation and cleanup utilities
8. **Progress Tracking**: Progress monitoring and estimated time remaining

These enhancements make NLM more accessible to researchers and developers, while providing advanced features for experienced users. The enhanced bindings maintain backward compatibility where possible, while adding new functionality for better error handling, validation, and performance monitoring.

For more information, see the documentation and examples in the repository, including the enhanced Python bindings implementation in `python/bindings_enhanced.cpp`.
