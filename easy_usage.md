# NLM (熙然) — Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Quick Start

This project implements an experimental brain-like system that learns from experience through neural computation. It's designed to simulate how a developing brain might acquire capabilities through interaction with an environment.

### Installation

#### Prerequisites

```bash
# Required:
- C++ compiler with C++20 support (g++ 11+, clang 14+)
- CMake 3.16+
- Python 3.8+
- pybind11 (for Python bindings)

# Optional (for development):
- Python package manager (pip)
- NumPy (for advanced Python features)
```

#### Build Instructions

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Build Python bindings (if configured)
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
pip install .
```

### Running Demos

#### Phase 2: Real Neural Computation (Basic Learning)
```bash
# Build and run the basic neural computation demo
./nlm
```

#### Phase 3: World Interaction (Motor Control)
```bash
./nlm_phase3_demo
```

#### Phase 4: Advanced Cognition (Planning & Memory)
```bash
./nlm_phase4_demo
```

#### Phase 5: Performance & Checkpoints
```bash
./nlm_phase5_demo
```

#### Phase 6: Integration (Full Brain Test)
```bash
./nlm_phase6_demo
```

#### Phase 6 Text Demo (Advanced Learning)
```bash
./nlm_phase6_text_demo
```

#### Benchmark
```bash
./nlm_benchmark
```

### Python Usage

#### Basic Brain Interface

```python
import pynlm
import numpy as np

# Create a default configuration
config = pynlm.create_default_config()

# Create a brain with the configuration
brain = pynlm.create_brain(config)
brain.initialize()

# Create a world for interaction
world = pynlm.create_simple_world()
world.configure(20, 20, 5, 5)

# Create an agent brain
agent_brain = pynlm.create_agent_brain(brain)
agent_brain.initialize(world)

# Run a simulation step for n steps
for step in range(1000):
    # Inject sensory input (example: random vision data)
    vision_data = np.random.randn(5, 5, 3).astype(np.float32)
    sensor = pynlm.Vision(5, 5, 3)
    sensor.set_data(vision_data)
    
    brain.receive_sensory_input(sensor)
    brain.step(step, step * 0.001)
    
    # Get action from agent brain
    action = agent_brain.decode_motor_command()
    
    # Apply action in world
    world.apply_motor_command(action, step * 0.001)
```

#### Advanced Features

```python
import pynlm
import numpy as np

# Create configuration with custom settings
config = pynlm.create_default_config()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.2)
config.set("random_seed", 42)

# Create brain
brain = pynlm.create_brain(config)
brain.initialize()

# Create visualization and set up callbacks
from pynlm import visualization
viz = visualization.VizualizationInterface(brain)

# Set up learning callbacks
learning_callbacks = {
    "on_spike": lambda neuron: print(f"Neuron {neuron.index()} spiked"),
    "on_synapse_change": lambda synapse: print(f"Synapse weight changed to {synapse.get_weight():.3f}"),
    "on_memory_store": lambda episode: print(f"Memory episode stored: {len(episode)} events")
}

# Enable advanced plasticity
pynlm.experimental_advanced_plasticity(brain)

# Run learning experiment
brain.step(0, 0.0)
brain.step(1, 0.001)
brain.step(2, 0.002)

# Save brain state for later use
brain.save("my_brain_state.bin")

# Load brain state
brain.load("my_brain_state.bin")

# Get neural statistics
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.3f}")
print(f"Excitatory/Inhibitory ratio: {brain.get_excitation_inhibition_ratio():.3f}")

# Get memory statistics
print(f"Memory working memory: {brain.get_working_memory_size()} items")
print(f"Episodic memory: {brain.get_episodic_memory_count()} episodes")
```

### Configuration Management

#### Configuration File Format (JSON)

```json
{
    "nlm": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        
        "plasticity": {
            "learning_rate": 0.01,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "stdp_time_constant": 20.0
        },
        
        "development": {
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0001
        },
        
        "neuromodulation": {
            "dopamine_baseline": 0.1,
            "curiosity_baseline": 0.05,
            "novelty_threshold": 0.3
        },
        
        "reward": {
            "discount_factor": 0.99,
            "reward_scaling": 1.0
        },
        
        "world": {
            "width": 10,
            "height": 10,
            "max_energy": 100.0,
            "energy_decay_rate": 0.01
        },
        
        "logging": {
            "level": "INFO",
            "to_file": false,
            "filename": "nlm.log"
        }
    }
}
```

#### Creating Custom Configurations

```python
import pynlm
import json

# Create configuration from Python dictionary
config_data = {
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.15,
    "plasticity_learning_rate": 0.02,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.025,
    "enable_debug": True
}

config = pynlm.create_default_config()
# Set configuration values
for key, value in config_data.items():
    config.set(key, value)

# Save configuration to file
config.save_to_file("custom_config.json")

# Load configuration from file
config2 = pynlm.create_default_config()
config2.load_from_file("custom_config.json")

# Export configuration as Python dictionary
config_dict = config.to_python_dict()
print(config_dict)
```

### Advanced Python Examples

#### 1. Reinforcement Learning Example

```python
import pynlm
import numpy as np
import time

class SimpleRLAgent:
    def __init__(self, world_size=10):
        self.config = pynlm.create_default_config()
        self.config.set("neuron_count", 500)
        self.config.set("region_count", 2)
        self.config.set("random_seed", int(time.time()))
        
        self.brain = pynlm.create_brain(self.config)
        self.world = pynlm.create_simple_world()
        self.world.configure(world_size, world_size, 3, 3)
        
        self.agent_brain = pynlm.create_agent_brain(self.brain)
        self.agent_brain.initialize(self.world)
        
        self.total_reward = 0.0
        self.episode_count = 0
        
    def run_episode(self, max_steps=1000):
        """Run a single learning episode"""
        self.brain.reset()
        self.agent_brain.reset()
        
        for step in range(max_steps):
            # Get current state
            percept = self.world.get_sensory_percept()
            
            # Process sensory input
            self.agent_brain.process_sensory_input(percept)
            
            # Get action
            action = self.agent_brain.decode_motor_command()
            
            # Execute action in world
            action_result = self.world.apply_motor_command(action, step * 0.001)
            
            # Apply reward modulation
            self.agent_brain.apply_reward_modulation(
                action_result.reward, 
                action_result.predicted_reward
            )
            
            # Update development
            self.agent_brain.update_development(step * 0.001)
            
            # Accumulate reward
            self.total_reward += action_result.reward
            
            # Check if episode should end
            if action_result.success == False or step >= max_steps - 1:
                break
        
        self.episode_count += 1
        return self.total_reward
    
    def train(self, num_episodes=100):
        """Train the agent"""
        print(f"Training for {num_episodes} episodes...")
        
        for episode in range(num_episodes):
            episode_reward = self.run_episode()
            
            if (episode + 1) % 10 == 0:
                print(f"Episode {episode + 1}: Total reward = {episode_reward:.2f}")
        
        print(f"Training complete. Average reward per episode: {self.total_reward / num_episodes:.2f}")
        
        # Save trained brain
        self.brain.save(f"trained_agent_{num_episodes}.bin")

# Usage example
if __name__ == "__main__":
    agent = SimpleRLAgent(world_size=20)
    agent.train(num_episodes=50)
```

#### 2. Neuroevolution Example

```python
import pynlm
import numpy as np
from multiprocessing import Pool
import time

class BrainPopulation:
    def __init__(self, population_size=10, mutation_rate=0.1):
        self.population_size = population_size
        self.mutation_rate = mutation_rate
        self.population = []
        self.generation = 0
        
        # Create initial population
        for i in range(population_size):
            brain = self.create_random_brain()
            self.population.append({
                "brain": brain,
                "fitness": 0.0,
                "id": i,
                "config": self.create_random_config()
            })
    
    def create_random_config(self):
        """Create a random configuration"""
        config = pynlm.create_default_config()
        
        # Randomly mutate configuration parameters
        if np.random.random() < 0.5:
            config.set("neuron_count", np.random.randint(200, 2000))
        
        if np.random.random() < 0.5:
            config.set("connection_probability", np.random.uniform(0.05, 0.3))
        
        if np.random.random() < 0.5:
            config.set("plasticity_learning_rate", np.random.uniform(0.01, 0.05))
        
        return config
    
    def create_random_brain(self):
        """Create a brain with random initial state"""
        config = self.create_random_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        return brain
    
    def evaluate_fitness(self, brain_index):
        """Evaluate fitness of a brain in a simplified environment"""
        individual = self.population[brain_index]
        brain = individual["brain"]
        world = pynlm.create_simple_world()
        world.configure(8, 8, 3, 3)
        
        total_reward = 0.0
        steps_without_progress = 0
        
        for step in range(500):
            # Simple task: reach a goal position
            brain.step(step, step * 0.001)
            
            # Get action
            agent_brain = pynlm.create_agent_brain(brain)
            agent_brain.initialize(world)
            action = agent_brain.decode_motor_command()
            
            # Apply action
            result = world.apply_motor_command(action, step * 0.001)
            total_reward += result.reward
            
            # Check for stagnation
            if abs(result.reward) < 0.01:
                steps_without_progress += 1
                if steps_without_progress > 100:
                    break
            else:
                steps_without_progress = 0
        
        return total_reward
    
    def evaluate_population(self):
        """Evaluate fitness of entire population using parallel processing"""
        with Pool() as pool:
            fitnesses = pool.map(self.evaluate_fitness, range(self.population_size))
        
        for i, fitness in enumerate(fitnesses):
            self.population[i]["fitness"] = fitness
    
    def evolve(self):
        """Evolve the population"""
        print(f"Generation {self.generation}")
        
        # Evaluate fitness
        self.evaluate_population()
        
        # Calculate statistics
        fitnesses = [ind["fitness"] for ind in self.population]
        avg_fitness = np.mean(fitnesses)
        best_fitness = np.max(fitnesses)
        
        print(f"  Average fitness: {avg_fitness:.2f}")
        print(f"  Best fitness: {best_fitness:.2f}")
        
        # Create next generation
        new_population = []
        
        # Elitism: keep the best individual
        best_idx = np.argmax(fitnesses)
        new_population.append(self.population[best_idx].copy())
        
        # Generate offspring
        while len(new_population) < self.population_size:
            # Select parents
            parent1_idx = np.random.choice(self.population_size, p=np.power(fitnesses / np.sum(fitnesses), 2))
            parent2_idx = np.random.choice(self.population_size, p=np.power(fitnesses / np.sum(fitnesses), 2))
            
            parent1 = self.population[parent1_idx]
            parent2 = self.population[parent2_idx]
            
            # Crossover
            child = self.crossover(parent1, parent2)
            
            # Mutate
            self.mutate(child)
            
            new_population.append(child)
        
        self.population = new_population
        self.generation += 1
    
    def crossover(self, parent1, parent2):
        """Perform crossover between two parents"""
        child_config = pynlm.create_default_config()
        
        # Randomly mix configuration parameters from parents
        for key in parent1["config"].get_keys():
            if np.random.random() < 0.5:
                child_config.set(key, parent1["config"].getOr<double>(key, 0.0))
            else:
                child_config.set(key, parent2["config"].getOr<double>(key, 0.0))
        
        # Create child brain
        child_brain = pynlm.create_brain(child_config)
        child_brain.initialize()
        
        return {
            "brain": child_brain,
            "fitness": 0.0,
            "id": len(self.population),
            "config": child_config
        }
    
    def mutate(self, individual):
        """Mutate an individual"""
        config = individual["config"]
        
        for key in config.get_keys():
            if np.random.random() < self.mutation_rate:
                # Get current value
                value = config.getOr<double>(key, 0.0)
                
                # Apply random mutation
                if key == "neuron_count":
                    new_value = max(200, min(2000, value + np.random.choice([-50, 0, 50])))
                elif key == "connection_probability":
                    new_value = max(0.05, min(0.5, value + np.random.uniform(-0.05,  authoritarian
                    new_value = max(0.05, min(0.5, value + np.random.uniform(-0.05, 0.05)))
                elif key == "plasticity_learning_rate":
                    new_value = max(0.01, min(0.1, value + np.random.uniform(-0.01, 0.01))
                else:
                    new_value = value
                
                config.set(key, new_value)
    
    def save_population(self, filename):
        """Save population to file"""
        population_data = {
            "generation": self.generation,
            "population": []
        }
        
        for individual in self.population:
            # Save brain state
            individual["brain"].save(f"population_{individual['id']}.bin")
            
            population_data["population"].append({
                "id": individual["id"],
                "fitness": individual["fitness"],
                "config": individual["config"].to_json()
            }
        
        # Save as JSON
        import json
        with open(filename, 'w') as f:
            json.dump(population_data, f, indent=2)
    
    def load_population(self, filename):
        """Load population from file"""
        import json
        
        with open(filename, 'r') as f:
            population_data = json.load(f)
        
        self.generation = population_data["generation"]
        self.population = []
        
        for i, individual_data in enumerate(population_data["population"]):
            config = pynlm.Config.from_json(individual_data["config"])
            brain = pynlm.create_brain(config)
            brain.load(f"population_{individual_data['id']}.bin")
            
            self.population.append({
                "brain": brain,
                "fitness": individual_data["fitness"],
                "id": individual_data["id"],
                "config": config
            })

# Usage example
if __name__ == "__main__":
    # Create population
    population = BrainPopulation(population_size=20, mutation_rate=0.1)
    
    # Evolve for several generations
    for generation in range(10):
        population.evolve()
        
        if generation % 5 == 0:
            population.save_population(f"population_gen_{generation}.json")
    
    # Save final population
    population.save_population("final_population.json")
    print("Neuroevolution complete!")
```

### Troubleshooting

#### Common Issues

1. **Build Failures**
   - Ensure you have all required dependencies
   - Check CMake version (minimum 3.16)
   - Verify compiler supports C++20

2. **Python Binding Issues**
   - Ensure pybind11 is installed
   - Check Python version compatibility
   - Verify build includes Python bindings

3. **Runtime Errors**
   - Check configuration values are within expected ranges
   - Ensure brain is properly initialized before use
   - Verify world configuration matches brain expectations

#### Debugging Tips

```python
import pynlm
import traceback

# Enable debug logging
config = pynlm.create_default_config()
config.set("enable_debug", True)

# Wrap brain operations in try-catch
brain = pynlm.create_brain(config)
try:
    brain.initialize()
    brain.step(0, 0.0)
except Exception as e:
    print(f"Error: {e}")
    traceback.print_exc()
```

### Performance Optimization

#### Tips for Faster Simulations

1. **Reduce Neuron Count**
   ```python
   config.set("neuron_count", 500)  # Smaller than default 1000
   ```

2. **Adjust Connection Probability**
   ```python
   config.set("connection_probability", 0.1)  # Reduce from 0.15
   ```

3. **Use Sparse Operations**
   ```python
   # Only process active neurons
   active_neurons = brain.get_firing_neuron_count()
   ```

4. **Batch Processing**
   ```python
   # Process multiple steps at once
   for step in range(100):
       brain.step(step, step * 0.001)
   ```

5. **Memory Management**
   ```python
   # Save intermediate states
   brain.save("checkpoint.bin")
   # Load when needed
   brain.load("checkpoint.bin")
   ```

### Advanced Usage

#### 1. Multiple Brains in Simulation

```python
import pynlm

# Create multiple brains for comparison
brains = []
for i in range(5):
    config = pynlm.create_default_config()
    config.set("random_seed", i)  # Different seeds
    config.set("neuron_count", 500)
    brain = pynlm.create_brain(config)
    brain.initialize()
    brains.append(brain)

# Run each brain with same inputs
for step in range(100):
    for i, brain in enumerate(brains):
        # All brains see same sensory input
        sensor = pynlm.Vision(5, 5, 3)
        sensor.set_data(np.random.randn(5, 5, 3).astype(np.float32))
        
        brain.receive_sensory_input(sensor)
        brain.step(step, step * 0.001)
```

#### 2. Custom Worlds

```python
import pynlm

class CustomWorld(pynlm.SimpleWorld):
    def __init__(self, width=20, height=20):
        super().__init__()
        self.configure(width, height, 3, 3)
        self.goal_position = (width // 2, height // 2)
        self.agent_start = (1, 1)
        
    def reset(self):
        super().reset()
        self.set_agent_start(*self.agent_start)
        
    def is_at_goal(self):
        body = self.get_agent_body()
        return abs(body.x - self.goal_position[0]) <= 0.5 and abs(body.y - self.goal_position[1]) <= 0.5
```

### Python API Reference

#### Core Classes

- **Config**: Brain configuration and parameter management
- **Brain**: Neural simulation core
- **AgentBrain**: Agent interface for brain control
- **SimpleWorld**: Basic 2D simulation environment
- **Vision**: Visual sensory input with NumPy support
- **Audio**: Audio sensory input with NumPy support
- **Action**: Motor actions and commands

#### Key Methods

- **Config**: `load_from_file`, `save_to_file`, `set`, `get`, `validate`
- **Brain**: `initialize`, `step`, `receive_sensory_input`, `produce_action`, `save`, `load`
- **AgentBrain**: `initialize`, `process_sensory_input`, `decode_motor_command`, `apply_reward_modulation`

#### NumPy Integration

The Python bindings include NumPy support for:

- **Vision**: `set_data_numpy()`, `get_data_numpy()`
- **Audio**: `set_data_numpy()`, `get_sample_rate()`
- **Configuration**: Efficient array operations

### Future Enhancements

The NLM project is actively developed with planned features:

1. **Advanced Memory Systems**
   - Hierarchical memory structures
   - Consolidation mechanisms
   - Retrieval optimization

2. **Enhanced Plasticity**
   - Neuromodulator-based learning
   diskusi yang lebih kompleks
   - Activity-dependent refinement

3. **Cognitive Architecture**
   - Complex planning algorithms
   - Concept formation
   - Abstract reasoning

4. **Development Extensions**
   - Multi-stage development
   - Social learning
   - Cultural transmission

5. **Application Frameworks**
   - Domain-specific interfaces
   - Integration with other systems
   - Specialized learning environments

### Getting Help

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Contributing

We welcome contributions to the NLM project! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch
3. Follow the existing code style
4. Write tests for new functionality
5. Update documentation
6. Submit a pull request

### License

MIT License - See LICENSE file for details.

---

*Project maintained by the NLM Research Team*
*Documentation generated: $(date)*
*Version: 2.0.0-enhanced*