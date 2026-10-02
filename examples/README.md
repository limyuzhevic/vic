# NLM Examples Directory

This directory contains ready-to-run example scripts demonstrating different aspects of the Neural Learning Machine (NLM) framework.

## Overview

NLM examples are organized by complexity:

- **basic/** - Getting started with the simplest possible examples
- **intermediate/** - Practical examples with world interaction and learning
- **advanced/** - Complex examples showing full system capabilities

## How to Use

### Build and Install

1. Build NLM from source:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. Build Python bindings:
   ```bash
   cd .. (back to project root)
   pip install -e .
   ```

### Running Examples

#### C++ Examples (require compiled binaries)

All C++ examples require the compiled NLM binaries. You can run them after building:

```bash
# Basic examples
./examples/basic/silent_brain.exe
./examples/basic/minimal_brain.exe

# Intermediate examples  
./examples/intermediate/agent_example.exe
./examples/intermediate/world_interaction.exe

# Advanced examples
./examples/advanced/full_integration.exe
./examples/advanced/development_example.exe
```

#### Python Examples

Python examples can be run directly with Python 3.8+:

```bash
python examples/basic/01_silent_brain.py
python examples/intermediate/01_agent_world.py
python examples/advanced/01_neural_developer.py
```

### Example Categories

#### Basic Examples

| File | Description |
|------|-------------|
| `basic/01_silent_brain.py` | Create a brain and make it think without any input |
| `basic/02_minimal_brain.py` | The absolute simplest possible example (one line) |
| `basic/03_brain_stats.py` | Show how to get brain statistics and metrics |

#### Intermediate Examples

| File | Description |
|------|-------------|
| `intermediate/01_agent_world.py` | Complete agent with brain, world, and interaction |
| `intermediate/02_learning_agent.py` | Agent that learns from rewards and experiences |
| `intermediate/03_development_demo.py` | Demonstrate brain development over time |
| `intermediate/04_memory_example.py` | Show memory systems in action |

#### Advanced Examples

| File | Description |
|------|-------------|
| `advanced/01_neural_developer.py` | Advanced agent with all features enabled |
| `advanced/02_research_experiment.py` | Research-grade experimental setup |
| `advanced/03_checkpoint_demo.py` | Demonstrate checkpoint save/load functionality |
| `advanced/04_performance_test.py` | Performance benchmarking and optimization |

## Example Scripts Reference

### Basic Examples

#### 01. Silent Brain

Creates a brain that operates without any external input:

```python
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think for 10 steps
for i in range(10):
    brain.step(i)

print(f"Brain completed {brain.getTotalSpikeCount()} spikes")
```

#### 02. Minimal Brain

The absolute minimum to get a working brain:

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons!")
```

#### 03. Brain Statistics

Demonstrates how to collect and use brain metrics:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### Intermediate Examples

#### 01. Agent World

Complete agent system with brain, world, and sensory-motor loops:

```python
import pynlm

def run_agent_simulation():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable key features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes input
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Position: ({world.getAgentBody().x():.1f}, {world.getAgentBody().y():.1f})")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation()
print("Simulation complete!")
```

#### 02. Learning Agent

Agent that improves through trial and error:

```python
import pynlm

def create_learning_agent():
    """Create an agent that learns from experiences."""
    # Configuration for learning
    config = pynlm.createDefaultConfig()
    config.set("plasticity.learning_rate", 0.01)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("neuromodulation.novelty.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    brain.initialize()
    
    return brain, agent

def train_agent(agent, world, episodes=100):
    """Train agent using reinforcement learning."""
    for episode in range(episodes):
        # Reset for new episode
        world.reset()
        agent.reset()
        
        # Run episode
        for step in range(100):
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step)
            
            # Choose action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Log progress
        if episode % 20 == 0:
            print(f"Episode {episode}: "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"PredictionError={agent.getPredictionError():.3f}")
    
    return agent

# Create and train agent
brain, agent = create_learning_agent()
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)

trained_agent = train_agent(agent, world)
print("Training complete!")
```

#### 03. Development Demo

Show how brains develop and mature over time:

```python
import pynlm

def demonstrate_development():
    """Demonstrate brain development stages."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    brain.initialize()
    
    # Enable development
    agent.enableDevelopment(True)
    
    print("Brain Development Over Time:")
    print("=" * 40)
    
    # Simulate development over time
    for day in range(0, 40, 5):  # 40 "days" in simulation time
        # Advance development
        agent.updateDevelopment(10.0)  # 10 units of development per "day"
        
        # Get current stage
        stage = agent.getDevelopmentalStage()
        stage_name = "Initial" if stage == pynlm.DevelopmentalStage.Initial else \
                    "Critical Period" if stage == pynlm.DevelopmentalStage.CriticalPeriod else \
                    "Maturation" if stage == pynlm.DevelopmentalStage.Maturation else \
                    "Adult" if stage == pynlm.DevelopmentalStage.Adult else \
                    "Aging"
        
        # Show metrics
        print(f"Day {day:2d}: {stage_name:15s} "
              f"Plasticity: {agent.getNeuromodulationLevel():.3f} "
              f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("\nDevelopment complete!")

demonstrate_development()
```

### Advanced Examples

#### 01. Neural Developer

Advanced agent with all features enabled:

```python
import pynlm

def advanced_neural_developer():
    """Advanced agent demonstrating all NLM capabilities."""
    # Configuration for advanced features
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)  # Larger brain
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromodulation.dopamine.scale", 1.0)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("neuromodulation.novelty.enable", True)
    config.set("neuromodulation.prediction_error.enable", True)
    config.set("development.synaptogenesis_rate", 0.001)
    config.set("development.pruning_rate", 0.0001)
    
    # Create complete agent system
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent.initialize(world)
    
    # Enable ALL features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced Neural Developer Example")
    print("===============================")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"World: {world.getWidth()}x{world.getHeight()} with {world.getVisionWidth()}x{world.getVisionHeight()} vision")
    print()
    
    # Run advanced simulation
    for step in range(1000):
        # Update world
        world.update(0.05)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and learns
        brain.step(step)
        
        # Make decision
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward and learn
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.05)
        
        # Log every 100 steps
        if step % 100 == 0:
            print(f"Step {step:4d}: "
                  f"Firing={brain.getFiringNeuronCount():4d} "
                  f"Curiosity={agent.getCuriosityLevel():.3f} "
                  f"Novelty={agent.getNoveltyLevel():.3f} "
                  f"PredictionError={agent.getPredictionError():.3f} "
                  f"DevStage={agent.getDevelopmentalStage():s}")
    
    return brain, agent, world

# Run advanced example
brain, agent, world = advanced_neural_developer()
print("\nAdvanced example complete!")
```

#### 02. Research Experiment

Research-grade experimental setup:

```python
import pynlm
import json
import time

def research_experiment():
    """Research experiment with controlled variables."""
    # Experimental parameters
    experimental_params = {
        "brain_size": 1000,
        "plasticity_enabled": True,
        "development_enabled": True,
        "neuromodulation_enabled": True,
        "novelty_sensitivity": 0.5,
        "reward_scale": 1.0,
        "learning_rate": 0.01,
        "simulation_steps": 2000,
        "repetitions": 5
    }
    
    print("Research Experiment: Neural Development")
    print("=" * 50)
    
    results = []
    
    for rep in range(experimental_params["repetitions"]):
        print(f"\nRepetition {rep + 1}/{experimental_params['repetitions']}")
        
        # Create experiment configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", experimental_params["brain_size"])
        config.set("plasticity.learning_rate", experimental_params["learning_rate"])
        config.set("development.synaptogenesis_rate", 0.0005)
        config.set("development.pruning_rate", 0.00005)
        
        # Create agent system
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
        world.reset()
        
        agent.initialize(world)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        start_time = time.time()
        for step in range(experimental_params["simulation_steps"]):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        elapsed = time.time() - start_time
        
        # Record results
        rep_result = {
            "repetition": rep + 1,
            "final_curiosity": agent.getCuriosityLevel(),
            "final_prediction_error": agent.getPredictionError(),
            "final_plasticity_modifier": agent.getNeuromodulationLevel(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "simulation_time": elapsed,
            "developmental_stage": str(agent.getDevelopmentalStage())
        }
        
        results.append(rep_result)
        
        # Log progress
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Prediction Error: {agent.getPredictionError():.3f}")
        print(f"  Development Stage: {rep_result['developmental_stage']}")
        print(f"  Time: {elapsed:.2f}s")
    
    # Analyze results
    print(f"\nResults Summary (over {experimental_params['repetitions']} repetitions):")
    print("-" * 60)
    
    avg_curiosity = sum(r["final_curiosity"] for r in results) / len(results)
    avg_prediction_error = sum(r["final_prediction_error"] for r in results) / len(results)
    
    print(f"Average Final Curiosity: {avg_curiosity:.3f}")
    print(f"Average Final Prediction Error: {avg_prediction_error:.3f}")
    print(f"Neuron Count Range: {min(r['total_neurons'] for r in results)} - {max(r['total_neurons'] for r in results)}")
    print(f"Synapse Count Range: {min(r['total_synapses'] for r in results)} - {max(r['total_synapses'] for r in results)}")
    
    # Save results
    with open(f"experiment_results_{int(time.time())}.json", "w") as f:
        json.dump({
            "parameters": experimental_params,
            "results": results,
            "summary": {
                "avg_curiosity": avg_curiosity,
                "avg_prediction_error": avg_prediction_error,
                "total_neurons_avg": sum(r["total_neurons"] for r in results) / len(results),
                "total_synapses_avg": sum(r["total_synapses"] for r in results) / len(results)
            }
        }, f, indent=2)
    
    print(f"\nResults saved to experiment_results_{int(time.time())}.json")

# Run research experiment
research_experiment()
```

## Quick Start Script

Create a `run_examples.py` script in your project directory to quickly test all examples:

```python
#!/usr/bin/env python3
"""
Quick script to run all NLM examples.
Run with: python run_examples.py
"""

import subprocess
import sys
import os

def run_example(script_path, description):
    """Run a single example script."""
    print(f"\n{'='*60}")
    print(f"Running: {description}")
    print(f"Script: {script_path}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run([sys.executable, script_path], 
                              capture_output=True, text=True, timeout=60)
        
        if result.returncode == 0:
            print("SUCCESS")
            print(result.stdout)
        else:
            print("FAILED")
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            
    except subprocess.TimeoutExpired:
        print("TIMEOUT (example took too long)")
    except FileNotFoundError:
        print(f"ERROR: Script not found: {script_path}")
    except Exception as e:
        print(f"ERROR: {e}")

def main():
    """Run all examples in the examples directory."""
    examples_dir = os.path.join(os.path.dirname(__file__), "examples")
    
    if not os.path.exists(examples_dir):
        print(f"ERROR: Examples directory not found: {examples_dir}")
        return
    
    print("NLM Examples Quick Runner")
    print("=" * 40)
    print(f"Found examples directory: {examples_dir}")
    
    # Run basic examples
    basic_dir = os.path.join(examples_dir, "basic")
    if os.path.exists(basic_dir):
        print(f"\nRunning basic examples from {basic_dir}")
        for file in sorted(os.listdir(basic_dir)):
            if file.endswith(".py"):
                run_example(os.path.join(basic_dir, file), f"Basic Example: {file}")
    
    # Run intermediate examples
    intermediate_dir = os.path.join(examples_dir, "intermediate")
    if os.path.exists(intermediate_dir):
        print(f"\nRunning intermediate examples from {intermediate_dir}")
        for file in sorted(os.listdir(intermediate_dir)):
            if file.endswith(".py"):
                run_example(os.path.join(intermediate_dir, file), f"Intermediate Example: {file}")
    
    # Run advanced examples
    advanced_dir = os.path.join(examples_dir, "advanced")
    if os.path.exists(advanced_dir):
        print(f"\nRunning advanced examples from {advanced_dir}")
        for file in sorted(os.listdir(advanced_dir)):
            if file.endswith(".py"):
                run_example(os.path.join(advanced_dir, file), f"Advanced Example: {file}")
    
    print(f"\n{'='*60}")
    print("Example runner completed!")
    print(f"{'='*60}")

if __name__ == "__main__":
    main()
```

## Notes

- **Python examples** require the `pynlm` package to be installed
- **C++ examples** require compiled NLM binaries
- **Performance note**: Some examples may take several minutes to run
- **Memory usage**: Advanced examples may use significant memory (2000+ neurons)
- **System requirements**: Examples may require 4GB+ RAM for complex simulations

## Getting Help

If you encounter issues:

1. Check that `pynlm` is properly installed:
   ```bash
   pip install -e .
   ```

2. Verify build dependencies:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. Consult the documentation:
   - `README.md` - High-level overview
   - `HOW_TO_USE.md` - Detailed usage instructions
   - `docs/ARCHITECTURE.md` - Technical architecture
   - `docs/SCIENCE.md` - Scientific background

4. Check example output for specific errors and adjust parameters as needed.

## Example Output Samples

Typical example output shows metrics like:

```
Step   0: Firing=  12 Curiosity=0.123 Novelty=0.456 PredictionError=0.012 DevStage=Critical Period
Step 100: Firing=  89 Curiosity=0.567 Novelty=0.234 PredictionError=0.045 DevStage=Maturation
Step 200: Firing= 156 Curiosity=0.789 Novelty=0.678 PredictionError=0.123 DevStage=Adult
```

Or learning progress:

```
Brain Development Over Time:
==========================================
Day   0: Initial             Plasticity: 0.000 Curiosity: 0.000
Day   5: Initial             Plasticity: 0.123 Curiosity: 0.111
Day  10: Critical Period     Plasticity: 0.456 Curiosity: 0.333
Day  15: Critical Period     Plasticity: 0.789 Curiosity: 0.666
Day  20: Maturation          Plasticity: 0.234 Curiosity: 0.444
Day  25: Maturation          Plasticity: 0.456 Curiosity: 0.555
Day  30: Adult               Plasticity: 0.123 Curiosity: 0.333
Day  35: Adult               Plasticity: 0.045 Curiosity: 0.222
Day  40: Adult               Plasticity: 0.012 Curiosity: 0.111
```

These examples provide a comprehensive starting point for working with NLM and can be modified and extended for your specific research or development needs.