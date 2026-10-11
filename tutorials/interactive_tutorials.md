# NLM - Interactive Tutorial System

## Overview

This section provides an interactive tutorial system for the NLM (Neural Learning Machine) framework. The tutorials guide users through the core concepts, getting started with the Python API, CLI commands, and advanced features.

Each tutorial includes:
- Learning objectives
- Step-by-step instructions
- Interactive examples
- Checkpoints to verify understanding
- Extensions for deeper learning

---

## Tutorial 1: Getting Started with NLM

### Learning Objectives
- Understand the basic components of the NLM framework
- Create and initialize a simple brain
- Run your first simulation
- Explore the Python API

### Setup

First, ensure you have NLM installed:

```bash
pip install pynlm
```

Or for development from source:

```bash
pip install -e .
```

### Tutorial Steps

#### Step 1: Import and Create Basic Components

```python
import pynlm
import time

print("=== NLM Interactive Tutorial ===\n")

# 1. Create configuration
print("Step 1: Creating configuration...")
config = pynlm.createDefaultConfig()
print(f"✓ Created default configuration")

# 2. Create and initialize brain
print("\nStep 2: Creating and initializing brain...")
brain = pynlm.createBrain(config)
brain.initialize()
print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"✓ {brain.getTotalSynapseCount()} synapses")

# 3. Run a simple simulation
print("\nStep 3: Running initial simulation...")
for step in range(10):
    brain.step(step)
    if step % 5 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print(f"✓ Completed {step + 1} simulation steps")
```

#### Step 2: Exploring Brain Statistics

```python
print("\nStep 2: Exploring brain statistics...")

# Get basic statistics
stats = {
    "Total Neurons": brain.getTotalNeuronCount(),
    "Total Synapses": brain.getTotalSynapseCount(),
    "Firing Neurons": brain.getFiringNeuronCount(),
    "Active Neurons": brain.getActiveNeuronCount(),
    "Average Firing Rate": brain.getAverageFiringRate(),
    "Total Spikes": brain.getTotalSpikeCount(),
    "Excitation/Inhibition Ratio": brain.getExcitationInhibitionRatio(),
}

print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key:25}: {value}")

# Interactive exploration
print("\nInteractive Exploration:")
print("1. Check if brain is healthy")
healthy = brain.getTotalNeuronCount() > 0 and brain.getTotalSynapseCount() > 0
print(f"   Brain health: {'✓ Healthy' if healthy else '✗ Unhealthy'}")

print("2. Check developmental stage")
stage = brain.getDevelopmentalStage()
print(f"   Development stage: {stage}")

print("3. Get plasticity status")
plasticity = brain.getPlasticityStatus()
print(f"   Plasticity enabled: {plasticity}")
```

#### Step 3: Adding Regions

```python
print("\nStep 3: Adding brain regions...")

# Add different regions
region1 = brain.addRegion("Cortex")
region2 = brain.addRegion("Hippocampus")

print(f"✓ Added Cortex region (ID: {region1})")
print(f"✓ Added Hippocampus region (ID: {region2})")

# Get all regions
regions = brain.getRegions()
print(f"✓ Total regions: {len(regions)}")

# Example: Get region details
if region1:
    print(f"✓ Region 1 is valid: {region1 != INVALID_NEURON_ID}")
```

#### Step 4: Saving and Loading

```python
print("\nStep 4: Saving and loading brain state...")

# Save brain state
brain.save("tutorial_brain.brain")
print("✓ Saved brain state to 'tutorial_brain.brain'")

# Create a new brain and load the state
new_config = pynlm.createDefaultConfig()
new_brain = pynlm.createBrain(new_config)
new_brain.load("tutorial_brain.brain")

print(f"✓ Loaded brain has {new_brain.getTotalNeuronCount()} neurons")
print(f"✓ Loaded brain has {new_brain.getTotalSynapseCount()} synapses")

# Verify they're equivalent
neurons_match = new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
print(f"✓ Neuron counts match: {neurons_match}")
```

### Tutorial Checkpoint

**Test your understanding:**
1. Can you create a brain with 500 neurons instead of the default?
2. What happens if you call `step()` without calling `initialize()` first?
3. How do you check if a brain is in a healthy state?

**Solutions (for reference):**
1. Modify the config: `config.set("brain.neuron_count", 500)`
2. This should raise a runtime error - brain must be initialized first
3. Use `brain.getTotalNeuronCount() > 0 and brain.getTotalSynapseCount() > 0`

### Extensions

#### Create a Population

```python
print("\nExtension: Creating a population...")

# Create a population of excitatory neurons
pop = brain.createPopulation(pynlm.NeuronType.Excitatory, 100)
print(f"✓ Created population with {pop.getNeuronCount()} neurons")

# Get neurons from population
neurons = pop.getNeurons()
print(f"✓ Population has {len(neurons)} neurons")

# Check population type
print(f"✓ Population type: {pop.getNeuronType()}")
```

#### Connect Regions

```python
print("\nExtension: Connecting regions...")

# Connect cortex to hippocampus
brain.connectRegions(region1, region2, weight=0.5, delay=2)
print("✓ Connected cortex to hippocampus")

# Get inter-region connections
connections = brain.getInterRegionConnections()
print(f"✓ Total inter-region connections: {len(connections)}")

# Check connection details
for conn in connections:
    print(f"  {conn.sourceRegion} → {conn.targetRegion} (weight: {conn.weight})")
```

#### Run Multiple Steps Efficiently

```python
print("\nExtension: Running simulation efficiently...")

# Use batch processing
steps = 1000
start_time = time.time()

for step in range(steps):
    brain.step(step)
    
    # Log progress
    if step % 100 == 0:
        print(f"  Progress: {step}/{steps} steps "
              f"({(step/steps)*100:.1f}%)")

elapsed = time.time() - start_time
print(f"✓ Completed {steps} steps in {elapsed:.2f} seconds")
print(f"✓ Average time per step: {elapsed/steps*1000:.2f} ms")
```

---

## Tutorial 2: Agent Simulation

### Learning Objectives
- Understand the brain-world-agent architecture
- Create and run agent simulations
- Learn about interaction between brain and environment
- Explore neuromodulation and learning

### Tutorial Steps

#### Step 1: Setting Up the Agent System

```python
print("\n=== Tutorial 2: Agent Simulation ===\n")

# Create brain and agent brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

print("✓ Created agent system:")
print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
print(f"  - Agent input size: {agent.getSensoryInputSize()} bytes")
print(f"  - Agent output size: {agent.getMotorOutputSize()} commands")
print(f"  - World: {world.getWidth()}x{height} cells")
```

#### Step 2: Running an Episode

```python
print("\nStep 2: Running agent episode...")

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run episode
episode_length = 200
start_time = time.time()

for step in range(episode_length):
    # 1. Update world (pass time)
    world.update(0.1)
    
    # 2. Get sensory percept from world
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # 4. Run brain step (think)
    brain.step(step)
    
    # 5. Decode motor command from brain activity
    motor_cmd = agent.decodeMotorCommand()
    
    # 6. Apply motor command to world
    result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # 7. Apply reward modulation
    # Use agent's internal signals as reward
    internal = percept.getInternal()
    reward = internal[0] if internal else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # 8. Update development
    agent.updateDevelopment(0.1)
    
    # Print status
    if step % 50 == 0:
        print(f"  Step {step}:")
        print(f"    Action: {motor_cmd}")
        print(f"    Reward: {reward:.3f}")
        print(f"    Firing: {brain.getFiringNeuronCount()} neurons")
        print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"    Novelty: {agent.getNoveltyLevel():.3f}")

elapsed = time.time() - start_time
print(f"\n✓ Completed episode in {elapsed:.2f} seconds")
```

#### Step 3: Analyzing Behavior

```python
print("\nStep 3: Analyzing agent behavior...")

# Get comprehensive statistics
brain_stats = brain.getStatistics()
learning_state = agent.getLearningState()
neuromodulation = agent.getAllNeuromodulationLevels()

print("Brain Statistics:")
for key, value in brain_stats.items():
    print(f"  {key:20}: {value:10.2f}")

print("\nLearning State:")
for key, value in learning_state.items():
    print(f"  {key:20}: {'Enabled' if value else 'Disabled'}")

print("\nNeuromodulation Levels:")
for key, value in neuromodulation.items():
    print(f"  {key:20}: {value:10.3f}")

# Calculate behavior metrics
avg_firing_rate = brain.getAverageFiringRate()
curiosity_level = agent.getCuriosityLevel()
exploration_rate = agent.getCuriosityLevel() * 100  # Percentage

print(f"\nBehavior Summary:")
print(f"  Average firing rate: {avg_firing_rate:8.2f} Hz")
print(f"  Exploration drive: {curiosity_level:8.3f} (0-1)")
print(f"  Exploration rate: {exploration_rate:8.1f}%")
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Check agent health
agent_healthy = agent.getBrain().isHealthy()
print(f"  Agent health: {'✓ Good' if agent_healthy else '✗ Poor'}")
```

#### Step 4: Creating Custom Agents

```python
print("\nStep 4: Creating custom agent configurations...")

# Function to create different agent types
def create_agent_profile(profile_name, enable_reward, enable_curiosity, enable_development):
    """Create agent with specified profile."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Apply profile settings
    agent.enableRewardModulation(enable_reward)
    agent.enableCuriosity(enable_curiosity)
    agent.enableDevelopment(enable_development)
    agent.enableStructuralPlasticity(True)  # Always enabled
    
    return brain, agent, world

# Create different agent profiles
profiles = {
    'conservative': {'reward': False, 'curiosity': False, 'development': False},
    'balanced': {'reward': True, 'curiosity': True, 'development': True},
    'exploratory': {'reward': True, 'curiosity': True, 'development': True},
}

for profile_name, settings in profiles.items():
    brain, agent, world = create_agent_profile(
        profile_name,
        settings['reward'],
        settings['curiosity'],
        settings['development']
    )
    
    # Run quick test
    for _ in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(0)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    avg_curiosity = agent.getCuriosityLevel()
    print(f"  {profile_name:12}: curiosity={avg_curiosity:.3f}")

print(f"✓ Created {len(profiles)} agent profiles")
```

### Tutorial Checkpoint

**Test your understanding:**
1. What are the 3 main components of the agent architecture?
2. What does each agent subsystem do?
3. How do you enable/disable learning features?

**Solutions (for reference):**
1. Brain, AgentBrain, SimpleWorld
2. Brain (thinking), AgentBrain (sensory/motor processing), SimpleWorld (environment)
3. Use `agent.enableRewardModulation(True/False)`, `agent.enableCuriosity(True/False)`, etc.

### Extensions

#### Complete Episode Function

```python
def run_complete_episode(brain, agent, world, max_steps=500, visualize=False):
    """Run a complete agent episode with optional visualization."""
    
    episode_data = {
        'steps': [],
        'total_reward': 0.0,
        'max_curiosity': 0.0,
        'time_elapsed': 0.0
    }
    
    start_time = time.time()
    
    for step in range(max_steps):
        # Get current state
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process and act
        agent.processSensoryInput(percept)
        brain.step(step)
        motor_cmd = agent.decodeMotorCommand()
        result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Reward and development
        reward = sum(percept.getInternal()) if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record data
        episode_data['steps'].append({
            'step': step,
            'reward': reward,
            'action': str(motor_cmd),
            'curiosity': agent.getCuriosityLevel(),
            'firing': brain.getFiringNeuronCount()
        })
        
        episode_data['total_reward'] += reward
        episode_data['max_curiosity'] = max(episode_data['max_curiosity'], 
                                          agent.getCuriosityLevel())
        
        if visualize and step % 50 == 0:
            print(f"Step {step}: {motor_cmd} (reward: {reward:.2f})")
    
    episode_data['time_elapsed'] = time.time() - start_time
    return episode_data

# Use the function
# episode = run_complete_episode(brain, agent, world, max_steps=200, visualize=True)
```

#### Agent Comparison

```python
print("\nExtension: Comparing agent strategies...")

# Define different strategies
strategies = [
    ('random', False, False, False),
    ('exploitation', True, False, False),
    ('exploration', False, True, False),
    ('balanced', True, True, True),
]

results = {}

for strategy_name, reward, curiosity, development in strategies:
    brain, agent, world = create_agent_profile(
        strategy_name, reward, curiosity, development
    )
    
    # Run comparison episode
    for _ in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(0)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Record results
    results[strategy_name] = {
        'final_curiosity': agent.getCuriosityLevel(),
        'final_development': brain.getDevelopmentalStage(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_rate': brain.getAverageFiringRate()
    }

# Compare strategies
print("\nStrategy Comparison:")
print(f"{'Strategy':<12} {'Curiosity':<10} {'Firing':<8} {'Avg Rate':<10}")
print("-" * 45)
for strategy, data in results.items():
    print(f"{strategy:<12} {data['final_curiosity']:<10.3f} "
          f"{data['firing_neurons']:<8} {data['avg_rate']:<10.2f}")
```

---

## Tutorial 3: Advanced CLI Usage

### Learning Objectives
- Understand the NLM command-line interface
- Use CLI commands for exploration, simulation, and analysis
- Create custom scripts for batch processing
- Automate common workflows

### Available CLI Commands

```bash
nlm-explore     [OPTIONS]              Interactive exploration
nlm-learn       [OPTIONS]              Run learning simulations
nlm-simulate    [OPTIONS]              Run simple simulations
nlm-analyze     [OPTIONS]              Analyze and visualize results
nlm-benchmark   [OPTIONS]              Benchmark performance
nlm-debug       [OPTIONS]              Advanced debugging
nlm-profile     [OPTIONS]              Profile memory and performance
nlm-export     [OPTIONS]              Export results in various formats
nlm            [COMMAND] [OPTIONS]     Main CLI entry point
```

### Tutorial Steps

#### Step 1: Basic Exploration

```python
print("\n=== Tutorial 3: Advanced CLI Usage ===\n")

print("Step 1: Basic NLM CLI exploration...")
print("This tutorial demonstrates using the NLM command-line interface.")
print("\nTo run these commands, use the following syntax:")
print("> nlm-explore --steps 100")
print("> nlm-learn --preset advanced --steps 500")
print("> nlm-simulate --steps 200 --learning")
print("> nlm-analyze --filepath brain_state.brain")
print("> nlm-benchmark --output benchmark_results.json")
print("> nlm-export --filepath brain_state.brain --format json")

print("\nFor detailed help on any command, use:")
print("  nlm-explore --help")
print("  nlm-learn --help")
print("  nlm-simulate --help")
print("  nlm-analyze --help")
print("  nlm-benchmark --help")
print("  nlm-debug --help")
print("  nlm-profile --help")
print("  nlm-export --help")
```

#### Step 2: Creating Custom CLI Scripts

```python
print("\nStep 2: Creating custom CLI scripts...")

# Example: Custom exploration script
exploration_script = '''
#!/usr/bin/env python3
"""
Custom exploration script for NLM
"""

import sys
sys.path.append(sys.path[0])

from nlm_cli import explore_command

def main():
    # Simulate command line arguments
    sys.argv = ['nlm-explore', '--steps', '200']
    
    try:
        explore_command(None)  # Using predefined args
        print("\\nExploration completed successfully!")
    except Exception as e:
        print(f"Error during exploration: {e}")

if __name__ == '__main__':
    main()
'''

with open('custom_exploration.py', 'w') as f:
    f.write(exploration_script)

print("✓ Created custom exploration script: custom_exploration.py")

# Example: Batch simulation script
batch_script = '''
#!/usr/bin/env python3
"""
Batch simulation script for NLM
"""

import sys
sys.path.append(sys.path[0])

from pynlm import createBrain, createSimpleWorld, createAgentBrain, createDefaultConfig
import time

def run_batch_simulation(scenarios, output_file='batch_results.json'):
    """Run multiple simulation scenarios."""
    
    results = {}
    
    for scenario_name, config in scenarios.items():
        print(f"Running scenario: {scenario_name}")
        
        # Create components
        config_obj = createDefaultConfig()
        brain = createBrain(config_obj)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(**config.get('world', {}))
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Apply scenario settings
        for setting, value in config.get('agent', {}).items():
            if hasattr(agent, f'enable{setting.title()}'):
                getattr(agent, f'enable{setting.title()}')(value)
        
        # Run simulation
        steps = config.get('steps', 100)
        start_time = time.time()
        
        for step in range(steps):
            world.update(config.get('timestep', 0.1))
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        elapsed = time.time() - start_time
        
        # Store results
        results[scenario_name] = {
            'steps': steps,
            'duration': elapsed,
            'neurons': brain.getTotalNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        }
    
    # Save results
    import json
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\\nBatch simulation completed. Results saved to {output_file}")
    return results

# Define scenarios
scenarios = {
    'baseline': {
        'world': {'width': 20, 'height': 20, 'visionWidth': 8, 'visionHeight': 8},
        'agent': {'reward': True, 'curiosity': False, 'development': False},
        'steps': 100
    },
    'learning': {
        'world': {'width': 30, 'height': 30, 'visionWidth': 8, 'visionHeight': 8},
        'agent': {'reward': True, 'curiosity': True, 'development': True},
        'steps': 200
    },
    'exploration': {
        'world': {'width': 15, 'height': 15, 'visionWidth': 6, 'visionHeight': 6},
        'agent': {'reward': False, 'curiosity': True, 'development': False},
        'steps': 150
    }
}

# Run batch simulation
run_batch_simulation(scenarios, 'exploration_results.json')
'''

with open('batch_simulation.py', 'w') as f:
    f.write(batch_script)

print("✓ Created batch simulation script: batch_simulation.py")

# Example: Analysis script
analysis_script = '''
#!/usr/bin/env python3
"""
Analysis script for NLM results
"""

import json
import matplotlib.pyplot as plt
import numpy as np

def analyze_batch_results(input_file='exploration_results.json'):
    """Analyze batch simulation results."""
    
    with open(input_file, 'r') as f:
        results = json.load(f)
    
    print("Batch Results Analysis:")
    print("=" * 50)
    
    for scenario_name, data in results.items():
        print(f"\\nScenario: {scenario_name}")
        print(f"  Steps: {data['steps']}")
        print(f"  Duration: {data['duration']:.2f}s")
        print(f"  Neurons: {data['neurons']}")
        print(f"  Firing neurons: {data['firing']}")
        print(f"  Average firing rate: {data['rate']:.2f} Hz")
        print(f"  Curiosity level: {data['curiosity']:.3f}")
        print(f"  Novelty level: {data['novelty']:.3f}")
        
        # Calculate performance metrics
        steps_per_second = data['steps'] / data['duration']
        firing_percentage = data['firing'] / data['neurons'] * 100
        
        print(f"  Steps per second: {steps_per_second:.1f}")
        print(f"  Firing percentage: {firing_percentage:.1f}%")
    
    # Create comparison chart
    scenarios = list(results.keys())
    firing_rates = [results[s]['rate'] for s in scenarios]
    curiosities = [results[s]['curiosity'] for s in scenarios]
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
    
    # Firing rate comparison
    ax1.bar(scenarios, firing_rates, color='skyblue', alpha=0.7)
    ax1.set_title('Average Firing Rate by Scenario')
    ax1.set_ylabel('Firing Rate (Hz)')
    ax1.grid(True, alpha=0.3)
    
    # Curiosity level comparison
    ax2.bar(scenarios, curiosities, color='lightgreen', alpha=0.7)
    ax2.set_title('Curiosity Level by Scenario')
    ax2.set_ylabel('Curiosity Level (0-1)')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('analysis_results.png', dpi=150)
    print(f"\\nAnalysis chart saved as 'analysis_results.png'")
    
    return results

# Run analysis
analyze_batch_results('exploration_results.json')
'''

with open('analyze_results.py', 'w') as f:
    f.write(analysis_script)

print("✓ Created analysis script: analyze_results.py")
```

#### Step 3: Running CLI Commands Programmatically

```python
print("\nStep 3: Running CLI commands programmatically...")

import subprocess
import json

def run_nlm_command(command_args):
    """Run NLM CLI command and return results."""
    
    # Build command
    cmd = ['python3'] + ['nlm-cli'] + command_args
    
    try:
        # Run command
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        
        if result.returncode == 0:
            print(f"✓ Command executed successfully")
            if result.stdout:
                print(f"  Output: {result.stdout[:200]}...")
            return True, result.stdout
        else:
            print(f"✗ Command failed with return code {result.returncode}")
            if result.stderr:
                print(f"  Error: {result.stderr[:200]}...")
            return False, result.stderr
            
    except subprocess.TimeoutExpired:
        print(f"✗ Command timed out after 30 seconds")
        return False, "Timeout"
    except FileNotFoundError:
        print(f"✗ NLM CLI not found. Please ensure it's installed.")
        return False, "CLI not found"
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False, str(e)

# Example commands
print("\nExample CLI commands:")
commands = [
    (['explore', '--steps', '50'], 'Quick exploration'),
    (['learn', '--preset', 'balanced', '--steps', '100'], 'Learning simulation'),
    (['simulate', '--steps', '150', '--learning'], 'Simple simulation with learning'),
]

for cmd_args, description in commands:
    print(f"\n{description}:")
    print(f"  Command: nlm {' '.join(cmd_args)}")
    
    # Note: These commands would actually run
    # For demonstration, we'll just show what would happen
    success, output = run_nlm_command(cmd_args)
    if success:
        print(f"  Result: ✓ Success")
    else:
        print(f"  Result: ✗ Failed")

print(f"\nNote: Actual CLI commands require the NLM CLI to be installed and in PATH.")
print(f"See the install.sh script for installation instructions.")
```

### Tutorial Checkpoint

**Test your understanding:**
1. What are the main categories of CLI commands?
2. What does each command category do?
3. How would you create a custom CLI script?

**Solutions (for reference):**
1. Exploration, Learning, Simulation, Analysis, Benchmarking, Debugging, Profiling, Export
2. Each command category serves different user needs from beginners (explore, simulate) to advanced users (benchmark, profile)
3. Create a Python script that uses the nlm_cli module or subprocess to call NLM commands

### Extensions

#### Automated Experiment Runner

```python
def run_automated_experiment(experiment_configs):
    """Run multiple experiments automatically."""
    
    results = []
    
    for exp_name, config in experiment_configs.items():
        print(f"\\nRunning experiment: {exp_name}")
        
        # Create experiment directory
        exp_dir = f"experiments/{exp_name}"
        os.makedirs(exp_dir, exist_ok=True)
        
        # Run simulation
        brain, agent, world = create_agent_profile(
            exp_name,
            config.get('reward', True),
            config.get('curiosity', True),
            config.get('development', True)
        )
        
        # Save initial state
        brain.save(f"{exp_dir}/initial_state.brain")
        
        # Run episode
        for step in range(config.get('steps', 100)):
            world.update(config.get('timestep', 0.1))
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Save final state
        brain.save(f"{exp_dir}/final_state.brain")
        
        # Record results
        exp_result = {
            'name': exp_name,
            'duration': config.get('steps', 100) * config.get('timestep', 0.1),
            'neurons': brain.getTotalNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        
        results.append(exp_result)
        print(f"  Completed: {exp_result}")
    
    return results

# Define experiment configurations
experiment_configs = {
    'baseline': {
        'reward': True,
        'curiosity': False,
        'development': False,
        'steps': 50
    },
    'curiosity_driven': {
        'reward': False,
        'curiosity': True,
        'development': False,
        'steps': 75
    },
    'learning_focused': {
        'reward': True,
        'curiosity': True,
        'development': True,
        'steps': 100
    }
}

# Run experiments
# exp_results = run_automated_experiment(experiment_configs)
```

#### Real-time Monitoring

```python
import time
import threading
from pynlm import createBrain, createSimpleWorld, createAgentBrain, createDefaultConfig

def monitor_agent(agent, brain, world, interval=5.0, stop_event=None):
    """Monitor agent in real-time with background thread."""
    
    def monitor_loop():
        while not stop_event or not stop_event.is_set():
            try:
                # Get current metrics
                stats = {
                    'timestamp': time.time(),
                    'step': brain.getTotalSpikeCount(),
                    'neurons': brain.getTotalNeuronCount(),
                    'firing': brain.getFiringNeuronCount(),
                    'rate': brain.getAverageFiringRate(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'development': brain.getDevelopmentalStage()
                }
                
                # Log metrics
                log_entry = json.dumps(stats) + '\n'
                
                with open('real_time_log.csv', 'a') as f:
                    f.write(log_entry)
                
                print(f"[{stats['timestamp']:.1f}] "
                      f"Firing: {stats['firing']}/{stats['neurons']} "
                      f"Rate: {stats['rate']:.2f} Hz "
                      f"Curiosity: {stats['curiosity']:.3f}")
                
                time.sleep(interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(interval)
    
    # Start monitoring thread
    monitor_thread = threading.Thread(target=monitor_loop)
    monitor_thread.daemon = True
    monitor_thread.start()
    
    return monitor_thread

# Example usage
def run_monitoring_episode():
    """Run episode with real-time monitoring."""
    
    # Create components
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    agent = createAgentBrain(brain)
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Start monitoring
    stop_event = threading.Event()
    monitor_thread = monitor_agent(agent, brain, world, interval=2.0, stop_event=stop_event)
    
    print("Running episode with real-time monitoring...")
    print("Check 'real_time_log.csv' for monitoring data.")
    
    # Run episode
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Stop monitoring
    stop_event.set()
    monitor_thread.join(timeout=5.0)
    
    print("Episode completed with monitoring.")
    print("To analyze monitoring data, use the analyze_results.py script.")

# Run with monitoring
# run_monitoring_episode()
```

---

## Conclusion

You have completed the NLM Interactive Tutorial System! This comprehensive tutorial covers:

1. **Basic Usage**: Creating brains, running simulations, exploring statistics
2. **Agent Systems**: Brain-world-agent architecture, sensory processing, motor control
3. **Advanced CLI**: Command-line interface usage, custom scripts, automation
4. **Real-world Applications**: Experiment design, monitoring, analysis

### Next Steps

1. **Explore the Documentation**: Read `ARCHITECTURE.md` for detailed system architecture
2. **Run Experiments**: Use the CLI commands for different simulation types
3. **Customize Scripts**: Modify the provided scripts to suit your research needs
4. **Analyze Results**: Use the analysis tools to interpret simulation data
5. **Contribute**: Consider contributing improvements to the tutorial system

### Resources

- **Main Documentation**: `HOW_TO_USE.md` - Complete usage guide
- **Architecture**: `docs/ARCHITECTURE.md` - System design
- **Science**: `docs/SCIENCE.md` - Scientific background
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment descriptions
- **CLI Commands**: `nlm-cli.py` - Command-line interface
- **Python Bindings**: Enhanced with error handling and convenience methods
- **Installation**: `install.sh` - Installation script

### Key Learnings

- NLM provides a comprehensive framework for spiking neural network simulations
- The Python API offers both high-level convenience and low-level control
- CLI commands enable automation and batch processing
- Error handling ensures robust operation
- Real-time monitoring and analysis tools support scientific research

The NLM framework is designed to support research in:
- Neural development and maturation
- Learning and memory
- Neuromodulation
- Cognitive architecture
- Bio-inspired robotics

Feel free to extend this tutorial system and contribute your own examples and improvements! The NLM project welcomes contributions from the research community.
