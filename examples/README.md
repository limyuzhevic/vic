# NLM Examples Documentation
# =============================

This document provides comprehensive documentation for NLM (Neural Learning Machine) examples. The examples directory contains practical demonstrations of NLM functionality, from basic usage to advanced applications. Each example includes code, explanation, and expected output.

## Overview

The NLM examples directory contains five example scripts that demonstrate different aspects of NLM functionality:

1. **example1_basic_brain.py** - Basic brain creation and initialization
2. **example2_agent_simulation.py** - Complete agent simulation with world interaction
3. **example3_development_learning.py** - Development and learning simulation
4. **example4_experiment_management.py** - Configuration management and experiment tracking
5. **example5_benchmarking.py** - Benchmarking and performance analysis

Each example is designed to be:
- **Educational**: Teaches NLM concepts and usage patterns
- **Practical**: Demonstrates real-world applications
- **Comprehensive**: Covers all major NLM features
- **Testable**: Can be run independently to verify functionality

## Example Structure

Each example follows a consistent structure:

1. **Documentation**: Comprehensive docstring explaining the example's purpose
2. **Code**: Complete Python script demonstrating NLM usage
3. **Comments**: Inline comments explaining key concepts
4. **Output**: Expected output and explanations
5. **Extensions**: Suggestions for further exploration

## Running Examples

### Prerequisites

Before running any example, ensure you have:

```bash
pip install pynlm
```

### Basic Execution

All examples can be run directly from the examples directory:

```bash
cd examples
python example1_basic_brain.py
python example2_agent_simulation.py
python example3_development_learning.py
python example4_experiment_management.py
python example5_benchmarking.py
```

### Advanced Usage

Examples can also be imported and used programmatically:

```python
import sys
sys.path.append('examples')

from example1_basic_brain import basic_brain_example
from example2_agent_simulation import run_agent_simulation
from example3_development_learning import development_learning_example
from example4_experiment_management import main as run_experiment_management
from example5_benchmarking import main as run_benchmarking
```

## Example Details

### example1_basic_brain.py
# ==================================================

**Purpose**: Basic brain creation and initialization

**Description**: This example demonstrates the simplest way to create and initialize a brain using the NLM Python API. It covers:

- Default configuration creation
- Brain configuration customization
- Brain initialization
- Basic simulation loop
- Statistics collection and display

**Key Concepts**:

1. **Configuration Management**: Using `pynlm.createDefaultConfig()` and `config.set()`
2. **Brain Creation**: Using `pynlm.createBrain()` and `brain.initialize()`
3. **Simulation Loop**: Iterating through simulation steps
4. **Statistics Collection**: Accessing brain state information

**Code Overview**:

```python
def basic_brain_example():
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Customize configuration
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

**Expected Output**:

```
=== Basic Brain Creation Example ===
Created brain with 500 neurons
Total synapses: 75000
Step 0: 50 firing neurons
Step 25: 60 firing neurons
Step 50: 55 firing neurons
Step 75: 58 firing neurons
Simulation completed successfully!
```

**Learning Objectives**:

- Understand the basic brain lifecycle (create, initialize, simulate)
- Learn how to customize brain configuration
- Understand basic brain statistics and metrics
- Learn how to run and control simulations

**Extensions**:

1. Try different neuron counts and observe effects
2. Customize synapse density and observe connectivity
3. Run longer simulations and observe learning behavior
4. Experiment with different configuration values

### example2_agent_simulation.py
# ================================================

**Purpose**: Complete agent simulation with world interaction

**Description**: This example demonstrates a complete agent simulation with brain, agent, and world components working together. It covers:

- Multi-component integration (brain + agent + world)
- Sensory processing and motor output
- Reward-based learning
- Developmental tracking
- Complex agent behavior

**Key Concepts**:

1. **Agent Architecture**: Integration of brain, agent, and world
2. **Sensory Processing**: Converting world percepts to brain input
3. **Motor Output**: Converting brain activity to world actions
4. **Learning Systems**: Reward modulation, development, curiosity
5. **Simulation Integration**: Complete agent-world interaction loop

**Code Overview**:

```python
def run_agent_simulation(num_steps=1000):
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
```

**Expected Output** (sample):

```
=== Complete Agent Simulation Example ===
Step 0:
  Firing neurons: 125
  Curiosity: 0.123
  Novelty: 0.456
  Dev Stage: 1
Step 100:
  Firing neurons: 142
  Curiosity: 0.234
  Novelty: 0.567
  Dev Stage: 2
Step 200:
  Firing neurons: 138
  Curiosity: 0.345
  Novelty: 0.678
  Dev Stage: 3
...
Step 500:
  Firing neurons: 156
  Curiosity: 0.567
  Novelty: 0.789
  Dev Stage: 4
Agent simulation completed!
```

**Learning Objectives**:

- Understand agent-based architectures in NLM
- Learn how to integrate brain, agent, and world components
- Understand sensory processing and motor control
- Learn about neuromodulation systems (reward, curiosity, development)
- Experience complex agent behavior simulation

**Extensions**:

1. **World Variations**: Modify world configuration for different environments
2. **Agent Variations**: Test different agent configurations and behaviors
3. **Learning Variations**: Explore different learning parameter combinations
4. **Multi-Agent Simulation**: Extend to multiple interacting agents

### example3_development_learning.py
# ===========================================

**Purpose**: Development and learning simulation

**Description**: This example demonstrates how development stages and learning mechanisms interact during a simulation. It covers:

- Developmental trajectory tracking
- Critical period effects
- Synaptic plasticity and pruning
- Learning across developmental stages
- Long-term memory formation

**Key Concepts**:

1. **Developmental Stages**: Tracking and modeling developmental progression
2. **Critical Periods**: Time windows for sensitive learning
3. **Plasticity Dynamics**: Synaptic strengthening and weakening
4. **Learning Curves**: How learning changes with development
5. **Memory Consolidation**: Long-term memory formation and retrieval

**Code Overview**:

```python
def development_learning_example():
    # Create configuration optimized for development studies
    config = pynlm.createDefaultConfig()
    
    # Set development parameters
    config.set("development.initial_plasticity", 1.0)
    config.set("development.critical_period_start", 100)
    config.set("development.critical_period_end", 500)
    config.set("development.pruning_rate", 0.0001)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Enable development and learning
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    # Track development stages
    developmental_stages = []
    plasticity_rates = []
    firing_rates = []
    
    # Run simulation with developmental tracking
    for step in range(1000):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get motor command and apply
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Record development metrics
        if step % 50 == 0:
            developmental_stages.append({
                "step": step,
                "stage": brain.getDevelopmentalStage().value,
                "plasticity_rate": brain.getSynapticPlasticityRate(),
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel()
            })
            
            # Update development
            agent.updateDevelopment(0.1)
```

**Expected Output** (sample):

```
=== Development and Learning Example ===
Development simulation completed!
Recorded 20 developmental snapshots
Saved to: ~/nlm_dev_data/development_simulation.json

Development Summary:
  Final developmental stage: 4
  Average firing rate: 142.3
  Final curiosity level: 0.789

Developmental Stage Analysis:
  Stage 1 (0-100): Learning rate = 1.0, Firing rate = 125.4
  Stage 2 (100-300): Learning rate = 0.6, Firing rate = 138.7
  Stage 3 (300-500): Learning rate = 0.3, Firing rate = 145.2
  Stage 4 (500-1000): Learning rate = 0.1, Firing rate = 148.9
```

**Learning Objectives**:

- Understand developmental trajectories in artificial neural systems
- Learn how plasticity changes with development
- Observe critical period effects and learning windows
- Understand developmental stage-dependent learning
- Experience long-term simulation tracking and analysis

**Extensions**:

1. **Development Variations**: Different initial plasticity levels, critical period timing
2. **Environment Variations**: Different world configurations for different development scenarios
3. **Learning Variations**: Different reward structures and learning rates
4. **Cross-Developmental Analysis**: Compare development across multiple conditions

### example4_experiment_management.py
# ============================================

**Purpose**: Configuration management and experiment tracking

**Description**: This example demonstrates how to manage configurations, run experiments, and track results systematically. It covers:

- Configuration management and versioning
- Experiment parameter variations
- Systematic experiment execution
- Results collection and analysis
- Experiment tracking and management

**Key Concepts**:

1. **Experiment Design**: Creating experimental configurations with parameter variations
2. **Configuration Management**: Loading, modifying, and saving configurations
3. **Parameter Sweeps**: Systematically testing different parameter combinations
4. **Results Analysis**: Analyzing and comparing experimental results
5. **Experiment Tracking**: Managing and organizing experimental data

**Code Overview**:

```python
def create_experiment_config(base_config, modifications, experiment_name):
    """Create a new configuration based on a base configuration with modifications."""
    # Load base configuration
    with open(base_config, 'r') as f:
        config = json.load(f)
    
    # Apply modifications
    for key, value in modifications.items():
        config[key] = value
    
    # Add experiment metadata
    config["experiment_name"] = experiment_name
    config["creation_time"] = time.time()
    config["description"] = f"Experiment {experiment_name}"
    
    return config

def run_experiment(config, experiment_name, output_dir):
    """Run an experiment with the given configuration."""
    print(f"Running experiment: {experiment_name}")
    
    # Create brain from config
    pynlm_config = pynlm.createDefaultConfig()
    
    # Apply config parameters to pynlm config
    if "neuron_count" in config:
        pynlm_config.set("brain.neuron_count", config["neuron_count"])
    if "plasticity_learning_rate" in config:
        pynlm_config.set("plasticity.stdp.learning_rate", config["plasticity_learning_rate"])
    
    brain = pynlm.createBrain(pynlm_config)
    brain.initialize()
    
    # Run simulation
    steps = config.get("max_simulation_steps", 500)
    for step in range(steps):
        brain.step(step)
        
        # Log progress
        if step % 100 == 0:
            print(f"  Step {step}/{steps}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Save results
    results = {
        "experiment_name": experiment_name,
        "config": config,
        "final_neuron_count": brain.getTotalNeuronCount(),
        "final_synapse_count": brain.getTotalSynapseCount(),
        "final_firing_rate": brain.getAverageFiringRate(),
        "simulation_steps": steps,
        "completion_time": time.time()
    }
    
    # Save results file
    results_file = Path(output_dir) / f"{experiment_name}_results.json"
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"  Experiment completed. Results saved to: {results_file}")
    return results
```

**Expected Output** (sample):

```
=== Configuration Management and Experiment Tracking ===
Running experiment: test_exp
  Step 0/500: 125 firing neurons
  Step 100/500: 138 firing neurons
  Step 200/500: 142 firing neurons
  Step 300/500: 145 firing neurons
  Step 400/500: 148 firing neurons
  Step 500/500: 152 firing neurons
  Experiment completed. Results saved to: ./experiments/test_exp_results.json

Experiment Summary:
  Name: test_exp
  Final neurons: 1000
  Final synapses: 100000
  Final firing rate: 145.2
  Steps simulated: 500

Experiment files in ./experiments:
  test_exp_config.json
  test_exp_results.json
```

**Learning Objectives**:

- Understand experimental design and parameter management
- Learn how to create and run systematic experiments
- Experience experiment tracking and results analysis
- Learn how to organize experimental data and results
- Understand parameter sweeps and factorial experiments

**Extensions**:

1. **Multi-Factor Experiments**: Combine multiple parameters in factorial designs
2. **Experimental Automation**: Automate experiment execution and analysis
3. **Results Visualization**: Add visualization of experimental results
4. **Experiment Databases**: Store and manage large numbers of experiments

### example5_benchmarking.py
# ==========================================

**Purpose**: Benchmarking and performance analysis

**Description**: This example demonstrates how to benchmark NLM performance and analyze the results to optimize brain configuration. It covers:

- Performance benchmarking across configurations
- Parameter optimization and tuning
- Performance analysis and visualization
- Resource efficiency analysis
- Configuration comparison and optimization

**Key Concepts**:

1. **Benchmarking**: Measuring and comparing performance across configurations
2. **Parameter Optimization**: Finding optimal parameter combinations
3. **Performance Analysis**: Analyzing and interpreting benchmark results
4. **Resource Efficiency**: Optimizing memory and CPU usage
5. **Configuration Comparison**: Comparing different configuration options

**Code Overview**:

```python
def benchmark_configuration(config_name, steps=100, iterations=5, 
                          variations=None):
    """Benchmark a specific configuration."""
    print(f"=== Benchmarking Configuration: {config_name} ===")
    
    if variations is None:
        variations = [0.01, 0.05, 0.1]
    
    results = {
        "config_name": config_name,
        "steps": steps,
        "iterations": iterations,
        "variations": variations,
        "results": {}
    }
    
    # Run benchmarks for each variation
    for variation in variations:
        print(f"\nBenchmarking variation: plasticity_learning_rate = {variation}")
        
        times = []
        firing_rates = []
        
        for iteration in range(iterations):
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("plasticity.stdp.learning_rate", variation)
            config.set("brain.neuron_count", 500)  # Fixed size for fair comparison
            
            # Create and initialize brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time the simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            iteration_time = end_time - start_time
            times.append(iteration_time)
            firing_rates.append(brain.getAverageFiringRate())
            
            print(f"  Iteration {iteration + 1}: {iteration_time:.3f}s, "
                  f"Firing rate: {firing_rates[-1]:.3f}")
        
        # Calculate statistics
        avg_time = np.mean(times)
        std_time = np.std(times)
        avg_firing_rate = np.mean(firing_rates)
        std_firing_rate = np.std(firing_rates)
        
        results["results"][str(variation)] = {
            "avg_time": avg_time,
            "std_time": std_time,
            "avg_firing_rate": avg_firing_rate,
            "std_firing_rate": std_firing_rate,
            "steps_per_second": steps / avg_time if avg_time > 0 else 0
        }
```

**Expected Output** (sample):

```
=== NLM Benchmarking and Performance Analysis ===
=== Benchmarking Configuration: default ===
Benchmarking variation: plasticity_learning_rate = 0.01
  Iteration 1: 0.123s, Firing rate: 0.125
  Iteration 2: 0.125s, Firing rate: 0.128
  Iteration 3: 0.124s, Firing rate: 0.126
  Iteration 4: 0.126s, Firing rate: 0.127
  Iteration 5: 0.125s, Firing rate: 0.129
  Results for variation 0.01:
    Average time: 0.125s ± 0.001s
    Average firing rate: 0.127 ± 0.001
    Steps per second: 800.0

=== Benchmark Analysis ===
Best performing configuration:
  Config: high_plasticity
  Plasticity rate: 0.1
  Performance: 800.0 steps/second
  Average firing rate: 0.129

=== Benchmark Summary ===
Total configurations tested: 3
Best configuration: high_plasticity
Best plasticity rate: 0.1
Best performance: 800.0 steps/second
Benchmark results saved to: benchmark_results/
```

**Learning Objectives**:

- Understand benchmarking methodology and best practices
- Learn how to measure and compare NLM performance
- Experience parameter optimization and tuning
- Learn performance analysis and visualization
- Understand resource efficiency and optimization

**Extensions**:

1. **Advanced Benchmarking**: Add system-level performance monitoring
2. **Machine Learning Optimization**: Use ML to find optimal parameters
3. **Long-running Benchmarks**: Run extended benchmarks for stability analysis
4. **Comparative Analysis**: Compare across different hardware platforms

## Common Usage Patterns

### Quick Start

For rapid exploration of NLM functionality:

```bash
cd examples
python example1_basic_brain.py
```

### Comprehensive Analysis

For detailed analysis of multiple aspects:

```bash
cd examples
python example2_agent_simulation.py > agent_simulation.log
python example3_development_learning.py > development_learning.log
python example4_experiment_management.py --name comprehensive_test --output-dir ./experiments
python example5_benchmarking.py > benchmarking_results.log
```

### Script Integration

For integration with existing workflows:

```bash
#!/bin/bash
# Run comprehensive NLM analysis pipeline

# Step 1: Basic brain analysis
echo "=== Step 1: Basic Brain Analysis ==="
python examples/example1_basic_brain.py > basic_analysis.txt

# Step 2: Agent simulation analysis
echo "=== Step 2: Agent Simulation Analysis ==="
python examples/example2_agent_simulation.py > agent_simulation.txt

# Step 3: Development analysis
echo "=== Step 3: Development Analysis ==="
python examples/example3_development_learning.py > development_analysis.txt

# Step 4: Experiment management
echo "=== Step 4: Experiment Management ==="
python examples/example4_experiment_management.py --name comprehensive --output-dir ./experiments

# Step 5: Performance benchmarking
echo "=== Step 5: Performance Benchmarking ==="
python examples/example5_benchmarking.py > benchmarking_results.txt

# Step 6: Results analysis
echo "=== Step 6: Results Analysis ==="
cat > comprehensive_report.txt << EOF
=== NLM Examples Comprehensive Analysis ===
Generated: $(date)

Contents:
- Basic Brain Analysis: basic_analysis.txt
- Agent Simulation: agent_simulation.txt
- Development Analysis: development_analysis.txt
- Experiment Management: ./experiments/comprehensive*
- Performance Benchmarking: benchmarking_results.txt

Key Findings:
$(tail -20 basic_analysis.txt development_analysis.txt benchmarking_results.txt)
EOF

echo "Analysis complete. Check comprehensive_report.txt for summary."
```

## Example Customization

### Modifying Examples

Examples are designed to be easily customized. Common modifications include:

1. **Parameter Changes**: Adjust neuron count, learning rates, simulation steps
2. **Configuration Changes**: Use different configuration templates
3. **Output Changes**: Modify output format and logging
4. **Analysis Changes**: Add custom analysis and visualization

### Template Examples

```python
# Template for customizing example 1
import pynlm

def customized_basic_brain(neuron_count=500, synapse_density=0.15, steps=100):
    """Customized version of example 1 with different parameters."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", synapse_density)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons")
    
    return brain

# Usage
brain = customized_basic_brain(1000, 0.2, 200)
```

### Extending Examples

Examples can be extended by:

1. **Adding New Features**: Incorporate additional NLM capabilities
2. **Creating Variants**: Generate variations for different use cases
3. **Integration**: Combine examples for comprehensive workflows
4. **Testing**: Use examples as test cases for new functionality

## Example Integration

### Using Examples in Research Workflows

Examples can be integrated into research workflows:

```python
# Research workflow example
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from examples.example1_basic_brain import basic_brain_example
from examples.example2_agent_simulation import run_agent_simulation
from examples.example3_development_learning import development_learning_example

def run_research_pipeline():
    """Run comprehensive research pipeline using examples."""
    
    # Step 1: Basic brain analysis
    print("Step 1: Basic Brain Analysis")
    brain1 = basic_brain_example()
    
    # Step 2: Agent simulation
    print("Step 2: Agent Simulation")
    brain2, agent2, world2 = run_agent_simulation(500)
    
    # Step 3: Development analysis
    print("Step 3: Development Analysis")
    brain3, agent3, world3, dev_data = development_learning_example()
    
    # Step 4: Comparative analysis
    print("Step 4: Comparative Analysis")
    # Compare results across examples
    firing_rates = [
        brain1.getAverageFiringRate(),
        brain2.getAverageFiringRate(),
        brain3.getAverageFiringRate()
    ]
    
    print(f"Average firing rates: {firing_rates}")
    print(f"Standard deviation: {np.std(firing_rates):.3f}")
    
    # Step 5: Visualization
    print("Step 5: Visualization")
    plt.figure(figsize=(12, 4))
    
    plt.subplot(1, 3, 1)
    plt.hist(firing_rates, bins=10)
    plt.title("Firing Rate Distribution")
    plt.xlabel("Firing Rate (Hz)")
    plt.ylabel("Frequency")
    
    plt.subplot(1, 3, 2)
    plt.scatter([1, 2, 3], firing_rates)
    plt.title("Comparison of Examples")
    plt.xlabel("Example Index")
    plt.ylabel("Firing Rate (Hz)")
    
    plt.subplot(1, 3, 3)
    plt.boxplot(firing_rates)
    plt.title("Box Plot of Firing Rates")
    plt.ylabel("Firing Rate (Hz)")
    
    plt.tight_layout()
    plt.savefig("firing_rate_comparison.png")
    plt.close()
    
    print("Research pipeline completed!")
    return {
        'brain1': brain1,
        'brain2': brain2,
        'brain3': brain3,
        'agent2': agent2,
        'world2': world2,
        'agent3': agent3,
        'world3': world3,
        'dev_data': dev_data
    }

# Run research pipeline
results = run_research_pipeline()
```

### Using Examples in Teaching

Examples are excellent for teaching NLM concepts:

```python
# Teaching workflow example
from examples.example1_basic_brain import basic_brain_example
from examples.example2_agent_simulation import run_agent_simulation
from examples.example3_development_learning import development_learning_example

def teach_nlm_concepts():
    """Teach NLM concepts using examples."""
    
    print("=== NLM Teaching Module ===")
    print("\n1. Introduction to Basic Brain Simulation")
    print("   - Create a simple brain")
    print("   - Run basic simulation")
    print("   - Understand brain statistics")
    
    brain1 = basic_brain_example()
    
    print("\n2. Agent-Based Behavior")
    print("   - Integrate brain with agent and world")
    print("   - Learn sensory-motor coordination")
    print("   - Understand learning systems")
    
    brain2, agent2, world2 = run_agent_simulation(300)
    
    print("\n3. Developmental Processes")
    print("   - Track developmental trajectories")
    print("   - Observe learning across development")
    print("   - Understand critical periods")
    
    brain3, agent3, world3, dev_data = development_learning_example()
    
    print("\n4. Advanced Applications")
    print("   - Explore advanced NLM features")
    print("   - Understand system integration")
    print("   - Practice experimental design")
    
    return {
        'basic_brain': brain1,
        'agent_simulation': (brain2, agent2, world2),
        'development_learning': (brain3, agent3, world3, dev_data)
    }
```

## Example Index

| Example | Category | Complexity | Time Required | Key Concepts |
|---------|----------|------------|---------------|--------------|
| example1_basic_brain.py | Fundamentals | Basic | 1-2 minutes | Brain creation, simulation, basic statistics |
| example2_agent_simulation.py | Integration | Intermediate | 3-5 minutes | Agent architecture, sensory-motor loops, learning |
| example3_development_learning.py | Advanced | Intermediate | 5-8 minutes | Developmental stages, plasticity, critical periods |
| example4_experiment_management.py | Research | Advanced | 5-10 minutes | Experimental design, parameter sweeps, results analysis |
| example5_benchmarking.py | Performance | Advanced | 10-15 minutes | Benchmarking, optimization, performance analysis |

## Conclusion

The NLM examples provide a comprehensive foundation for understanding and working with neural simulation systems. They cover:

1. **Fundamentals**: Basic brain creation and simulation
2. **Integration**: Complete agent-world systems
3. **Development**: Developmental processes and learning
4. **Research**: Experimental design and analysis
5. **Performance**: Benchmarking and optimization

Each example is designed to be:
- **Accessible**: Easy to understand for beginners
- **Comprehensive**: Covers all major NLM features
- **Practical**: Demonstrates real-world applications
- **Extensible**: Can be customized for specific needs

By working through these examples, users can develop a deep understanding of NLM capabilities and applications, preparing them for advanced research and development work. 

---

*This documentation is part of the NLM (Neural Learning Machine) project. 
For questions, issues, or feedback, please refer to the project documentation or contact the NLM team.*
