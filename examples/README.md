"""
NLM Enhanced Python API Example Scripts

This directory contains example scripts demonstrating the enhanced NLM Python API.
These examples show how to use the high-level abstractions for common tasks.

### Required Dependencies

```bash
pip install enhanced-pynlm
```

### Basic Setup

All examples assume you have the enhanced NLM module available:

```python
import enhanced_pynlm as nlm
```

## Example Scripts

### 1. simple_brain.py

Basic brain simulation with enhanced API.

```python
import enhanced_pynlm as nlm

def main():
    print("=== NLM Enhanced API Example: Simple Brain Simulation ===")
    
    # Create and initialize brain (one-liner!)
    brain = nlm.Brain()
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    print("Running 100 simulation steps...")
    brain.simulate(100)
    
    # Get and display results
    stats = brain.getStats()
    print(f"\nSimulation Results:")
    print(f"  Total neurons: {stats['total_neurons']}")
    print(f"  Total synapses: {stats['total_synapses']}")
    print(f"  Firing neurons: {stats['firing_neurons']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.2f}")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Development stage: {stats['development_stage']}")
    
    # Save brain state
    print(f"\nSaving brain state...")
    brain.save("simple_brain_example")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
```

**Run:**
```bash
python simple_brain.py
```

### 2. agent_exploration.py

Complete agent-world experiment with exploration and learning.

```python
import enhanced_pynlm as nlm

def main():
    print("=== NLM Enhanced API Example: Agent-World Exploration ===")
    
    # Setup environment
    print("Setting up environment...")
    brain = nlm.Brain()
    world = nlm.World()
    world.setup(width=25, height=25, visionWidth=8, visionHeight=8)
    world.setAgentPosition(12.5, 12.5)  # Center of world
    
    # Create and initialize agent
    print("Creating agent...")
    agent = nlm.Agent(brain)
    agent.initialize()
    agent.enableAll()  # Enable all learning features
    
    # Run exploration episode
    print("Starting exploration episode (500 steps)...")
    results = agent.runEpisode(maxSteps=500)
    
    # Display results
    print(f"\nExploration Results:")
    print(f"  Average reward per step: {results['total_reward']:.4f}")
    print(f"  Final position: ({results['final_x']:.1f}, {results['final_y']:.1f})")
    print(f"  Success rate: {100 * (abs(results['final_x'] - 12.5) < 5):.1f}%")
    
    # Run additional episodes for better statistics
    print("\nRunning additional exploration episodes...")
    total_rewards = []
    final_positions = []
    
    for episode in range(3):
        print(f"  Episode {episode + 1}/3...")
        agent = nlm.Agent(brain)  # New agent for each episode
        agent.initialize()
        agent.enableAll()
        
        episode_results = agent.runEpisode(maxSteps=200)
        total_rewards.append(episode_results['total_reward'])
        final_positions.append((episode_results['final_x'], episode_results['final_y']))
    
    # Calculate average statistics
    avg_reward = sum(total_rewards) / len(total_rewards)
    avg_final_x = sum(p[0] for p in final_positions) / len(final_positions)
    avg_final_y = sum(p[1] for p in final_positions) / len(final_positions)
    
    print(f"\nAggregate Results (3 episodes):")
    print(f"  Average reward per step: {avg_reward:.4f}")
    print(f"  Average final position: ({avg_final_x:.1f}, {avg_final_y:.1f})")
    print(f"  Overall success rate: {100 * (abs(avg_final_x - 12.5) < 5):.1f}%")
    
    # Save experiment results
    brain.save("agent_exploration_example")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
```

**Run:**
```bash
python agent_exploration.py
```

### 3. learning_simulation.py

Advanced learning simulation with memory and development.

```python
import enhanced_pynlm as nlm

def main():
    print("=== NLM Enhanced API Example: Learning with Memory ===")
    
    # Create brain with memory support
    brain = nlm.Brain()
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Memory systems: Working={brain.getWorkingMemory() is not None}, "
          f"Episodic={brain.getEpisodicMemory() is not None}")
    
    # Run learning simulation with varying input patterns
    print("\nRunning learning simulation with varying inputs...")
    print("Step  | Firing Neurons | Working Memory | Novelty Level")
    print("------|----------------|----------------|--------------")
    
    for step in range(0, 501, 100):
        # Create different input patterns based on step
        if step < 200:
            # Increasing pattern
            sensory_input = [0.1 * ((step + i) % 10) / 10 for i in range(8)]
        elif step < 400:
            # Decreasing pattern  
            sensory_input = [0.1 * (9 - ((step - 200 + i) % 10)) / 10 for i in range(8)]
        else:
            # Random pattern
            import random
            sensory_input = [random.random() for _ in range(8)]
        
        # Run step
        brain.step(step, sensory_input)
        
        # Get statistics
        stats = brain.getStats()
        
        # Display progress every 100 steps
        if step % 100 == 0:
            print(f"{step:5d} | {stats['firing_neurons']:14d} | "
                  f"{stats.get('working_memory_traces', 0):14d} | "
                  f"{stats.get('curiosity_level', 0):11.3f}")
    
    # Final statistics
    final_stats = brain.getStats()
    
    print(f"\nLearning Simulation Results:")
    print(f"  Total steps: 500")
    print(f"  Average firing rate: {final_stats['average_firing_rate']:.2f}")
    print(f"  Total spikes: {final_stats['total_spikes']}")
    print(f"  Development stage: {final_stats['development_stage']}")
    
    # Check memory statistics
    if 'working_memory_traces' in final_stats:
        print(f"  Final working memory traces: {final_stats['working_memory_traces']}")
    if 'episodic_memory_episodes' in final_stats:
        print(f"  Final episodic memory episodes: {final_stats['episodic_memory_episodes']}")
    if 'novelty_level' in final_stats:
        print(f"  Final novelty level: {final_stats['novelty_level']:.3f}")
    
    # Save learning results
    print(f"\nSaving learning simulation...")
    brain.save("learning_simulation_example")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
```

**Run:**
```bash
python learning_simulation.py
```

### 4. benchmark.py

Performance benchmarking of enhanced API.

```python
import enhanced_pynlm as nlm
import time

def main():
    print("=== NLM Enhanced API Benchmark ===")
    
    # Benchmark 1: Brain creation and initialization
    print("\nBenchmark 1: Brain creation and initialization")
    start_time = time.time()
    
    for i in range(5):
        brain = nlm.Brain()
        brain.initialize()
    
    end_time = time.time()
    print(f"  5 brain creations: {end_time - start_time:.3f} seconds")
    print(f"  Average per brain: {(end_time - start_time) / 5:.3f} seconds")
    
    # Benchmark 2: Simulation performance
    print("\nBenchmark 2: Simulation performance")
    brain = nlm.Brain()
    brain.initialize()
    
    start_time = time.time()
    brain.simulate(100)
    end_time = time.time()
    
    print(f"  100 simulation steps: {end_time - start_time:.3f} seconds")
    print(f"  Average per step: {(end_time - start_time) / 100:.5f} seconds")
    
    # Benchmark 3: Statistics collection
    print("\nBenchmark 3: Statistics collection")
    
    start_time = time.time()
    for _ in range(100):
        stats = brain.getStats()
    end_time = time.time()
    
    print(f"  100 stats calls: {end_time - start_time:.3f} seconds")
    print(f"  Average per call: {(end_time - start_time) / 100:.5f} seconds")
    
    # Benchmark 4: Save performance
    print("\nBenchmark 4: Save performance")
    
    start_time = time.time()
    for i in range(3):
        brain.save(f"benchmark_{i}")
    end_time = time.time()
    
    print(f"  3 save operations: {end_time - start_time:.3f} seconds")
    print(f"  Average per save: {(end_time - start_time) / 3:.3f} seconds")
    
    print("\n=== Benchmark Complete ===")

if __name__ == "__main__":
    main()
```

**Run:**
```bash
python benchmark.py
```

## Quick Start Guide

### Installation

```bash
# Install the enhanced API
pip install enhanced-pynlm

# Or install from source
pip install -e .
```

### First Example

```python
import enhanced_pynlm as nlm

# Create and initialize brain
brain = nlm.Brain()
brain.initialize()

# Run simulation
brain.simulate(100)

# Get results
stats = brain.getStats()
print(f"Simulation completed with {stats['total_neurons']} neurons")
```

### Complete Example

```python
import enhanced_pynlm as nlm

# Create agent with world
agent = nlm.Agent(nlm.Brain())
agent.initialize()
agent.enableAll()

# Run exploration
results = agent.runEpisode(maxSteps=1000)
print(f"Exploration completed! Average reward: {results['total_reward']}")
```

## API Usage Tips

### 1. Memory Management

Monitor memory usage:

```python
stats = brain.getStats()
print(f"Working memory: {stats.get('working_memory_traces', 0)} traces")
```

### 2. Performance Monitoring

Track simulation progress:

```python
for step in range(0, 1000, 100):
    brain.simulate(step)
    stats = brain.getStats()
    print(f"Step {step}: {stats['firing_neurons']} firing neurons")
```

### 3. Save Progress Regularly

Always save your work:

```python
brain.save("checkpoints/simulation_001")
# ... continue simulation
brain.save("checkpoints/simulation_002")
```

## Common Errors and Solutions

### Error: Brain initialization fails

**Solution:** Ensure you call `initialize()` before using the brain:

```python
brain = nlm.Brain()
brain.initialize()  # Required!
brain.simulate(100)  # Now works
```

### Error: Memory usage is high

**Solution:** Use smaller simulations or save progress regularly:

```python
brain = nlm.Brain()
brain.initialize()
brain.simulate(100)  # Smaller simulation
brain.save("checkpoints/step_001")  # Save progress
```

### Error: Action not working

**Solution:** Ensure agent is properly initialized:

```python
agent = nlm.Agent(brain)
agent.initialize()
agent.enableAll()
```

## Getting Help

For help with the enhanced API:

1. **Documentation:** Check the `ENHANCED_API.md` file
2. **Examples:** Run the example scripts in the `examples/` directory
3. **Issues:** Report bugs or request features on GitHub
4. **Community:** Join the NLM discussion forum

## Contributing

We welcome contributions to the enhanced API! To contribute:

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Update documentation
5. Submit a pull request

## License

This enhanced API is part of the NLM project and is licensed under the MIT License.

## Authors

Enhanced Python API by NLM Team
Original NLM project by the NLM research team.

---

*Last updated: $(date +%Y-%m-%d)*
*Version: Enhanced API 1.0*
