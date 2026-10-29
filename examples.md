# NLM — 熙然: Enhanced Documentation and Examples

**An Experimental Artificial Developmental Brain**

This document provides comprehensive examples and improved documentation for using the NLM (Neural Learning Machine) Phase 6 integration.

## Quick Start Guide

### 1. Basic Installation and Build

```bash
# Clone the repository
cd /path/to/your/project

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Run Phase 6 Integration Demo

```bash
# Run the comprehensive integration test
./nlm_phase6_demo

# Or run the basic Phase 2 neural computation demo
./nlm

# With custom configuration
./nlm --config my_config.cfg
```

### 3. Using Python Bindings

```python
#!/usr/bin/env python3
import pynlm as nlm
import time

# Create a default configuration
config = nlm.createDefaultConfig()

# Set custom parameters
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.15f)

# Create brain with configuration
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Create a world for interaction
world = nlm.createSimpleWorld()
world.configure(16, 16, 16, 16)  # width, height, vision_width, vision_height

# Create agent brain interface
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Configure agent for full functionality
agent.enableRewardModulation(true)
agent.enableCuriosity(true)
agent.enableDevelopment(true)
agent.enableStructuralPlasticity(true)

print("NLM Brain initialized successfully!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total regions: {brain.getRegionCount()}")
```

## Python Examples

### Example 1: Basic Brain Simulation

```python
import pynlm as nlm
import math

def run_simulation_example():
    print("=== NLM Brain Simulation Example ===")
    
    # Create and configure brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(20, 20, 16, 16)
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(true)
    agent.enableCuriosity(true)
    
    # Run simulation loop
    steps = 100
    total_reward = 0.0
    
    for step in range(steps):
        # Get sensory input from world
        region = brain.getRegion(nlm.INVALID_REGION_ID) or brain.getRegions()[0].get()
        percept = world.observe(region)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain simulation step
        brain.step(step, step * 0.001)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyAction(region, action)
        
        # Get reward
        reward = world.computeReward(region)
        total_reward += reward
        
        # Apply neuromodulation
        agent.applyRewardModulation(reward, 0.0f)
        
        # Update development system
        agent.updateDevelopment(0.001)
        
        if step % 20 == 0:
            metrics = nlm.getBrainMetrics(brain)
            print(f"Step {step:3d}: Reward={reward:6.3f}, "
                  f"FiringRate={metrics['firingRate']:6.3f}, "
                  f"TotalSpikes={int(metrics['totalSpikes']):4d}")
    
    avg_reward = total_reward / steps
    print(f"\nSimulation completed!")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Final brain state saved to: checkpoint_simulation.bin")
    
    # Save brain state
    brain.save("checkpoint_simulation.bin")
    
    return True

if __name__ == "__main__":
    run_simulation_example()
```

### Example 2: Advanced Configuration Management

```python
import pynlm as nlm

def create_advanced_config():
    print("=== Advanced Configuration Example ===")
    
    # Create Phase 6 configuration
    config = nlm.createConfig()
    
    # Basic settings
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 2000)
    config.set("region_count", 4)
    config.set("connection_probability", 0.2f)
    
    # Plasticity settings
    config.set("stdp_ltp_weight", 0.025f)
    config.set("stdp_ltd_weight", 0.02f)
    config.set("stdp_tau", 25.0f)
    config.set("plasticity_learning_rate", 0.05f)
    
    # Development settings
    config.set("synaptogenesis_rate", 0.0002f)
    config.set("pruning_rate", 0.00002f)
    
    # Neuromodulation settings
    config.set("dopamine_baseline", 0.1f)
    config.set("curiosity_baseline", 0.05f)
    
    # Reward settings
    config.set("reward_discount_factor", 0.95f)
    
    # Environment settings
    config.set("environment_name", "GridWorld")
    config.set("environment_width", 10)
    config.set("environment_height", 10)
    
    # Memory system settings
    config.set("working_memory_capacity", 50)
    config.set("episodic_memory_capacity", 1000)
    
    # Save configuration to file
    config.saveToFile("advanced_config.cfg")
    
    print("Advanced configuration saved to: advanced_config.cfg")
    print("Configuration summary:")
    print(config.summary())
    
    return config

def load_and_modify_config(filename):
    print(f"\n=== Loading Configuration Example ===")
    
    config = nlm.createConfig()
    
    if config.loadFromFile(filename):
        print(f"Loaded configuration from: {filename}")
        
        # Modify loaded configuration
        original_neurons = config.getOr<int64_t>("neuron_count", 0)
        config.set("neuron_count", original_neurons * 2, nlm.ConfigSource.Runtime)
        
        config.set("enable_checkpointing", true, nlm.ConfigSource.Runtime)
        config.set("enable_development", true, nlm.ConfigSource.Runtime)
        
        print(f"Modified neuron_count from {original_neurons} to {original_neurons * 2}")
        print("Modified configuration:")
        print(config.summary())
        
        return config
    else:
        print(f"Failed to load configuration from: {filename}")
        return None

if __name__ == "__main__":
    # Create advanced configuration
    advanced_config = create_advanced_config()
    
    # Try to load existing configuration
    existing_config = load_and_modify_config("advanced_config.cfg")
    
    print("\n=== Configuration Management Complete ===")
```

### Example 3: Phase 6 Integration Experiment

```python
import pynlm as nlm
import json

def run_phase6_experiment():
    print("=== Phase 6 Integration Experiment ===")
    
    # Create different experiment configurations
    configs = {
        "default": nlm.createPhase6Config(),
        "large_brain": nlm.createPhase6Config(),
        "detailed_memory": nlm.createPhase6Config(),
        "minimal_resources": nlm.createPhase6Config()
    }
    
    # Configure different experiment types
    configs["default"].neuronCount = 500
    configs["default"].maxSteps = 2000
    configs["default"].enableCheckpointing = True
    configs["default"].enableReplay = True
    configs["default"].enableDevelopment = True
    
    configs["large_brain"].neuronCount = 2000
    configs["large_brain"].maxSteps = 5000
    configs["large_brain"].enableCheckpointing = True
    configs["large_brain"].enableReplay = True
    configs["large_brain"].enableDevelopment = True
    
    configs["detailed_memory"].neuronCount = 1000
    configs["detailed_memory"].maxSteps = 10000
    configs["detailed_memory"].enableCheckpointing = True
    configs["detailed_memory"].enableReplay = True
    configs["detailed_memory"].enableDevelopment = True
    
    configs["minimal_resources"].neuronCount = 100
    configs["minimal_resources"].maxSteps = 500
    configs["minimal_resources"].enableCheckpointing = False
    configs["minimal_resources"].enableReplay = False
    configs["minimal_resources"].enableDevelopment = False
    
    # Run experiments
    results = {}
    experiment = nlm.Phase6IntegratedExperiment()
    
    for name, config in configs.items():
        print(f"\nRunning {name} experiment...")
        print(f"  Neurons: {config.neuronCount}, Steps: {config.maxSteps}")
        print(f"  Checkpointing: {config.enableCheckpointing}, Replay: {config.enableReplay}")
        
        result = experiment.run(config)
        results[name] = result
        
        # Format and display results
        formatted_output = nlm.formatPhase6Result(result)
        print("\n" + formatted_output)
        
        # Save results
        with open(f"phase6_results_{name}.json", 'w') as f:
            # Convert result to JSON-serializable format
            json_result = {
                "name": name,
                "total_reward": result.totalReward,
                "avg_firing_rate": result.avgFiringRate,
                "memory_episodes_stored": result.memoryEpisodesStored,
                "dopamine_level": result.dopamineLevel,
                "novelty_level": result.noveltyLevel,
                "curiosity_level": result.curiosityLevel,
                "working_memory_integrated": result.memoryWorkingMemoryIntegrated,
                "episodic_memory_integrated": result.memoryEpisodicMemoryIntegrated,
                "neuromodulation_integrated": result.neuromodulationIntegrated,
                "prediction_integrated": result.predictionIntegrated,
                "development_integrated": result.developmentIntegrated,
                "checkpointing_works": result.checkpointingWorks,
                "replay_works": result.replayWorks,
                "total_wall_clock_time": result.totalWallClockTime,
                "episodes_stored": result.memoryEpisodesStored
            }
            json.dump(json_result, f, indent=2)
    
    # Compare results
    print("\n=== Experiment Comparison ===")
    best_reward = max(results.items(), key=lambda x: x[1].totalReward)
    best_firing = max(results.items(), key=lambda x: x[1].avgFiringRate)
    best_memory = max(results.items(), key=lambda x: x[1].memoryEpisodesStored)
    
    print(f"Best reward: {best_reward[0]} ({best_reward[1].totalReward:.3f})")
    print(f"Best firing rate: {best_firing[0]} ({best_firing[1].avgFiringRate:.3f})")
    print(f"Best memory storage: {best_memory[0]} ({best_memory[1].memoryEpisodesStored:.1f} episodes)")
    
    return results

if __name__ == "__main__":
    experiment_results = run_phase6_experiment()
    print("\n=== Phase 6 Experiment Suite Complete ===")
```

## Command Line Usage

### Enhanced Command Line Interface

```bash
# Show help information
./nlm --help
./nlm -h

# Show version information
./nlm --version
./nlm -v

# Run with custom configuration
./nlm --config my_config.cfg
./nlm -f my_config.cfg

# Run with custom parameters
./nlm --neuron-count 2000 --connection-prob 0.2
./nlm -n 2000 -c 0.2

# Enable additional features
./nlm --enable-checkpointing --enable-replay --enable-development

# Verbose output for debugging
./nlm --verbose
./nlm -V

# Quiet mode (minimal output)
./nlm --quiet
./nlm -q

# Save output to file
./nlm -o simulation_output.txt

# Full example with all options
./nlm --neuron-count 1500 --region-count 3 --connection-prob 0.15 \
     --enable-checkpointing --enable-replay --enable-development \
     --verbose --output simulation_results.txt my_config.cfg
```

## Configuration File Format

### Basic Configuration

```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Advanced Configuration Example

```ini
# Advanced NLM Configuration
# Custom settings for complex simulations

# Core parameters
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 5000
region_count = 8
connection_probability = 0.25

# Memory system parameters
working_memory_capacity = 100
episodic_memory_capacity = 5000
associative_memory_threshold = 0.5

# Plasticity and learning
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
stdp_tau = 30.0
plasticity_learning_rate = 0.02

# Development and maturation
development_synaptogenesis_rate = 0.0003
development_pruning_rate = 0.00005
development_critical_period_length = 1000

# Neuromodulation levels
dopamine_baseline = 0.2
curiosity_baseline = 0.1
novelty_threshold = 0.5
reward_scale = 1.0

# Prediction and cognition
prediction_horizon = 10
planning_depth = 5
concept_formation_threshold = 0.7

# Environment and interaction
environment_name = ComplexWorld
environment_width = 20
environment_height = 20
action_space_size = 8
observation_space_size = 64

# Simulation and performance
max_simulation_steps = 50000
simulation_time_limit = 1000.0
checkpoint_interval = 1000
enable_visualization = true
performance_monitoring = true
```

## Development and Testing

### Running Tests

```bash
# Run all tests
./test_runner

# Run specific test categories
./test_runner --brain-tests
./test_runner --memory-integration-tests
./test_runner --plasticity-tests

# Run tests with verbose output
./test_runner --verbose

# Run tests with custom configuration
./test_runner --config test_config.cfg
```

### Python Testing

```python
#!/usr/bin/env python3
import pynlm as nlm
import unittest
import tempfile
import os

class TestNLMBrain(unittest.TestCase):
    def setUp(self):
        # Create test configuration
        self.config = nlm.createConfig()
        self.config.set("neuron_count", 100)
        self.config.set("region_count", 2)
        
        # Create brain
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test basic brain creation and initialization"""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
    
    def test_brain_step(self):
        """Test brain simulation step"""
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0, 0.0)
        
        # Brain should still be functional
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def test_memory_integration(self):
        """Test memory system integration"""
        working_memory = self.brain.getWorkingMemory()
        episodic_memory = self.brain.getEpisodicMemory()
        
        self.assertIsNotNone(working_memory)
        self.assertIsNotNone(episodic_memory)
        
        # Test storing and retrieving patterns
        test_pattern = [0.1f, 0.5f, 0.9f]
        working_memory.store(test_pattern, 1.0f)
        
        retrieved = working_memory.retrieve()
        self.assertEqual(len(retrieved), len(test_pattern))
    
    def test_neuromodulation(self):
        """Test neuromodulation system"""
        dopamine = self.brain.getDopamine()
        curiosity = self.brain.getCuriosity()
        
        self.assertIsNotNone(dopamine)
        self.assertIsNotNone(curiosity)
        
        # Test neuromodulation levels
        dopamine_level = dopamine.getLevel()
        curiosity_level = curiosity.getCuriosityLevel()
        
        self.assertGreaterEqual(dopamine_level, 0.0)
        self.assertGreaterEqual(curiosity_level, 0.0)
    
    def test_checkpoint_functionality(self):
        """Test checkpoint save/load functionality"""
        checkpoint_file = "test_checkpoint.bin"
        
        # Run some steps to generate activity
        for step in range(50):
            self.brain.step(step, step * 0.001)
        
        # Save checkpoint
        save_result = self.brain.save(checkpoint_file)
        self.assertTrue(save_result)
        
        # Create new brain and load checkpoint
        new_config = nlm.createConfig()
        new_brain = nlm.createBrain(new_config)
        new_brain.initialize()
        
        load_result = new_brain.load(checkpoint_file)
        self.assertTrue(load_result)
        
        # Clean up
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
    
    def test_phase6_integration(self):
        """Test Phase 6 integration experiment"""
        experiment = nlm.Phase6IntegratedExperiment()
        
        # Create simple configuration
        phase6_config = nlm.Phase6Config()
        phase6_config.neuronCount = 200
        phase6_config.maxSteps = 500
        phase6_config.enableCheckpointing = True
        phase6_config.enableReplay = True
        phase6_config.enableDevelopment = True
        
        # Run experiment
        result = experiment.run(phase6_config)
        
        # Check results
        self.assertGreaterEqual(result.totalReward, 0.0)
        self.assertGreaterEqual(result.avgFiringRate, 0.0)
        self.assertTrue(result.memoryWorkingMemoryIntegrated)
        self.assertTrue(result.memoryEpisodicMemoryIntegrated)
        self.assertTrue(result.neuromodulationIntegrated)
        self.assertTrue(result.predictionIntegrated)
        self.assertTrue(result.developmentIntegrated)

if __name__ == "__main__":
    unittest.main()
```

## Performance Optimization Tips

### For Large-Scale Simulations

1. **Increase parallelism**: Use multiple CPU cores for simulation
2. **Optimize memory usage**: Monitor memory usage with `valgrind` or similar tools
3. **Use checkpointing**: Save intermediate states to reduce recomputation
4. **Tune parameters**: Adjust neuron count, connection probability, etc.

### Configuration Optimization

```bash
# Command line examples for performance optimization

# High performance mode
./nlm --neuron-count 10000 --region-count 8 --connection-prob 0.1 \
     --enable-checkpointing --verbose

# Memory efficient mode
./nlm --neuron-count 500 --region-count 2 --connection-prob 0.05 \
     --enable-replay --quiet

# Development and debugging
./nlm --neuron-count 100 --verbose --config debug_config.cfg
```

## Troubleshooting

### Common Issues and Solutions

1. **Configuration file not found**
   - Solution: Ensure the config file exists or use default settings
   - Command: `./nlm --config configs/default.cfg`

2. **Memory usage too high**
   - Solution: Reduce neuron count or region count
   - Command: `./nlm --neuron-count 500 --region-count 2`

3. **Simulation takes too long**
   - Solution: Reduce max steps or increase timestep
   - Command: `./nlm --neuron-count 1000 --connection-prob 0.2`

4. **Python bindings not working**
   - Solution: Check that the Python package is properly installed
   - Command: `pip install -e .` in the project root

5. **Checkpointing fails**
   - Solution: Check disk space and file permissions
   - Command: `./nlm --enable-checkpointing --output checkpoint.bin`

## Additional Resources

### Documentation

- **Architecture Documentation**: `docs/ARCHITECTURE.md`
- **Science Documentation**: `docs/SCIENCE.md`
- **Roadmap**: `docs/ROADMAP.md`
- **Experiments**: `docs/EXPERIMENTS.md`
- **Limitations**: `docs/LIMITATIONS.md`
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md`

### Examples and Tutorials

- **Basic Usage Examples**: `examples/basic_usage.py`
- **Advanced Features**: `examples/advanced_features.py`
- **Performance Tuning**: `examples/performance_tuning.py`
- **Integration Testing**: `examples/integration_testing.py`

### Online Resources

- **Project Website**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io/en/latest/
- **API Reference**: https://nlm.readthedocs.io/en/latest/api/

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

We thank all contributors to the NLM project for their valuable contributions to this experimental artificial brain system. Special thanks to the research team for their guidance and support.

## Contact

For questions, issues, or contributions:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Email: nlm-project@research.org
- Discord: https://discord.gg/nlm-project

---

*Generated with NLM Phase 6 Final Integration tools. Last updated: 2026-10-29*.
