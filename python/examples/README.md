# NLM Python Examples Directory
# Collection of example scripts demonstrating NLM Python API features

# This directory contains example scripts for different use cases and complexity levels

## Contents

1. **basic_examples/** - Basic usage examples for beginners
2. **advanced_examples/** - Advanced examples for experienced users
3. **research_examples/** - Examples for research and experimentation
4. **demo_scripts/** - Demo scripts for showcasing features

## Basic Examples

### 1. hello_world.py
**Simple "Hello World" example showing basic NLM usage**

```python
import pynlm

print("Hello, NLM World!")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### 2. simple_simulation.py
**Basic brain simulation example**

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### 3. world_interaction.py
**Example of brain interacting with a world**

```python
import pynlm

# Create complete setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run interaction
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("World interaction complete!")
```

## Advanced Examples

### 1. learning_simulation.py
**Complex learning simulation with all features**

```python
import pynlm
import time

# Create experiment setup with all features enabled
brain, world, agent = pynlm.createReadyToRunSimpleExperiment()

# Enable additional features
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
start = time.time()
total_reward, successes, spikes, stage = pynlm.run_learning_simulation(
    brain, world, agent, 200
)

elapsed = time.time() - start
print(f"Learning simulation: {elapsed:.2f}s, Reward: {total_reward:.2f}")
```

### 2. custom_config.py
**Example of creating custom configurations**

```python
import pynlm

# Create config with specific parameters
config = pynlm.create_config_with_seed(42)
config.set("neuron_count", 1000)
config.set("connection_probability", 0.2)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Custom brain: {brain.getTotalNeuronCount()} neurons")
```

### 3. sensory_analysis.py
**Example of analyzing sensory inputs**

```python
import pynlm
import numpy as np

# Create world and get various sensory inputs
world = pynlm.createSimpleWorld()
world.configure(32, 32, 8, 8)
world.reset()

percept = world.getSensoryPercept()
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()

# Analyze sensory data
print(f"Vision data shape: {np.array(vision).shape}")
print(f"Touch signals: {len(touch)}")
print(f"Internal signals: {len(internal)}")
```

### 4. batch_processing_example.py
**Example of batch processing**

```python
import pynlm

brain = pynlm.createDefaultBrain()
brain.initialize()

# Define custom processing function
def analyze_step(brain, step, time):
    firing = brain.getFiringNeuronCount()
    spikes = brain.getTotalSpikeCount()
    print(f"Step {step}: {firing} firing, {spikes} total spikes")

print("Analyzing brain steps 0-99...")
pynlm.batch_process_steps(brain, analyze_step, 0, 99)
```

## Research Examples

### 1. plasticity_study.py
**Research example studying synaptic plasticity**

```python
import pynlm

# Create brain with plasticity enabled
brain = pynlm.createDefaultBrain()
brain.initialize()

# Enable plasticity
for region in brain.getRegions():
    for syn in region.getSynapses():
        syn.enablePlasticity(True, True, False)  # Hebbian + STDP

# Run plasticity experiment
initial_weights = []
for region in brain.getRegions():
    for syn in region.getSynapses():
        initial_weights.append(syn.getWeight())

# Apply learning stimuli
for step in range(1000):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
    brain.step(step, step * 0.001)

# Analyze weight changes
final_weights = []
for region in brain.getRegions():
    for syn in region.getSynapses():
        final_weights.append(syn.getWeight())

weight_changes = [f - i for f, i in zip(final_weights, initial_weights)]
print(f"Average weight change: {np.mean(weight_changes):.4f}")
```

### 2. development_study.py
**Example studying brain development**

```python
import pynlm

brain = pynlm.createDefaultBrain()
brain.initialize()

# Monitor development stages
print("Monitoring brain development...")
for step in range(0, 1000, 100):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {stage}")

# Test developmental plasticity
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
print("Critical period stage set")
```

### 3. neuromodulation_study.py
**Example studying neuromodulation effects**

```python
import pynlm

brain, world, agent = pynlm.createReadyToRunSimpleExperiment()

# Test different neuromodulation levels
for trial in range(5):
    # Run simulation
    total_reward, successes, spikes, stage = pynlm.run_learning_simulation(
        brain, world, agent, 50
    )
    
    # Get neuromodulation levels
    neuromod = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    
    print(f"Trial {trial}: Neuromod={neuromod:.3f}, Curiosity={curiosity:.3f}, Reward={total_reward:.2f}")
```

## Demo Scripts

### 1. run_demo.py
**Script to run all demo examples**

```python
import subprocess
import sys

examples = [
    "basic_examples/hello_world.py",
    "basic_examples/simple_simulation.py",
    "advanced_examples/learning_simulation.py",
]

for example in examples:
    print(f"\nRunning: {example}")
    print("=" * 50)
    subprocess.run([sys.executable, example])
```

### 2. benchmark.py
**Benchmarking script for performance testing**

```python
import pynlm
import time

def benchmark_simulation(num_neurons, num_steps):
    config = pynlm.create_config_with_seed(42)
    config.set("neuron_count", num_neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start = time.time()
    for step in range(num_steps):
        brain.step(step)
    elapsed = time.time() - start
    
    return elapsed, brain.getTotalSpikeCount()

# Run benchmarks
print("Benchmarking NLM Python performance...")
results = []
for neurons in [100, 500, 1000]:
    elapsed, spikes = benchmark_simulation(neurons, 100)
    results.append((neurons, elapsed, spikes))
    print(f"Neurons: {neurons:4d}, Time: {elapsed:.3f}s, Spikes: {spikes}")
```

## Usage Instructions

### To Run Examples

```bash
# From project root directory
cd /path/to/nlm

# Install the package
pip install -e python/

# Run specific examples
python python/examples/basic_examples/hello_world.py

# Run demo script
python python/demo.py

# Run tests
python python/test_enhanced_bindings.py
```

### To Add New Examples

1. Create an example directory (basic_examples, advanced_examples, etc.)
2. Add Python files with descriptive names
3. Include comments explaining the concepts
4. Add at least one output example
5. Include error handling and validation

### Example Structure

```python
# Filename: example_name.py
"""
Brief description of what this example demonstrates.

Key concepts:
- Concept 1
- Concept 2
- Concept 3

Expected output:
Some descriptive output or metrics
"""

import pynlm

def main():
    # Example code here
    pass

if __name__ == "__main__":
    main()
```

## Best Practices

### Code Style
- Follow PEP 8 style guide
- Include docstrings for all functions
- Use descriptive variable names
- Add error handling and validation

### Example Quality
- Provide clear explanations
- Include expected outputs
- Use realistic use cases
- Demonstrate both simple and advanced features

### Testing Examples
- Examples should run without errors
- Include print statements showing results
- Handle edge cases gracefully
- Demonstrate error conditions

## Example Gallery

The examples directory provides a comprehensive gallery of NLM Python API capabilities, from basic "Hello World" to advanced research simulations. Each example is designed to be educational, self-contained, and ready to run.

These examples serve as both documentation and testing, ensuring that the NLM Python interface remains functional and useful for the broader developer community.