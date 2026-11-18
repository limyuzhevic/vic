# Examples Index

This directory contains example projects and templates demonstrating NLM functionality.

## Available Examples

### 1. Minimal Working Example
- **File**: `minimal.py`
- **Description**: The absolute minimum working example for NLM
- **Difficulty**: Beginner
- **Learn**: Basic brain creation and simulation

### 2. Agent-Based Simulation
- **File**: `agent_basic.py`
- **Description**: Complete agent with brain, world, and sensory processing
- **Difficulty**: Intermediate
- **Learn**: How to create and control intelligent agents

### 3. World Simulation
- **File**: `world_basic.py`
- **Description**: World simulation without agent (neural reactor)
- **Difficulty**: Intermediate
- **Learn**: Environment interaction and world dynamics

### 4. Training and Learning
- **File**: `training.py`
- **Description**: Training loop with curiosity, reward, and development
- **Difficulty**: Advanced
- **Learn**: Machine learning with neural architectures

### 5. Research Experiments
- **File**: `experiment.py`
- **Description**: Research experiment framework with variable testing
- **Difficulty**: Advanced
- **Learn**: Experimental design and analysis

### 6. Visualization Tools
- **File**: `visualization.py`
- **Description**: Brain activity visualization and analysis
- **Difficulty**: Intermediate
- **Learn**: Data visualization and analysis

### 7. Multi-Agent Systems
- **File**: `multi_agent.py`
- **Description**: Multiple interacting agents in shared environment
- **Difficulty**: Advanced
- **Learn**: Multi-agent coordination and competition

### 8. Environment Interaction
- **File**: `environment.py`
- **Description**: Custom environment development
- **Difficulty**: Advanced
- **Learn**: Environment design and control

### 9. Memory Systems
- **File**: `memory.py`
- **Description**: Memory system exploration and analysis
- **Difficulty**: Advanced
- **Learn**: Memory research and analysis

### 10. Command-Line Interface Examples
- **File**: `cli_examples.py`
- **Description**: Examples using the NLM command-line interface
- **Difficulty**: Intermediate
- **Learn**: CLI usage and automation

## How to Use Examples

### Quick Start with Minimal Example
```bash
# Run the minimal example
cd examples
python minimal.py

# Check output
# Expected: Brain created with neurons, simulation completed
```

### Agent-Based Simulation
```bash
# Run agent example
python agent_basic.py

# Check output
# Expected: Complete agent lifecycle with learning
```

### Training Example
```bash
# Run training example
python training.py

# Check output
# Expected: Training with curiosity and reward modulation
```

## Example Structure

Each example follows this structure:

1. **Imports**: Required modules and dependencies
2. **Configuration**: Brain and environment setup
3. **Initialization**: Brain, world, and agent creation
4. **Execution**: Main simulation or experiment loop
5. **Analysis**: Results processing and visualization
6. **Output**: Summary and statistics

### Example Template: Basic Brain Simulation

```python
"""
Example: Basic Brain Simulation

This example demonstrates the fundamental workflow for creating and running
an NLM brain simulation. It's the simplest possible example that shows
everything needed to get started with NLM.
"""

import pynlm

def main():
    """Run the basic brain simulation example."""
    
    print("=== Basic NLM Brain Simulation ===\n")
    
    # Step 1: Create brain configuration
    print("1. Creating brain configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.learning_rate", 0.01)
    
    # Step 2: Create and initialize brain
    print("2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   - Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   - Synapse count: {brain.getTotalSynapseCount()}")
    
    # Step 3: Run simulation
    print("3. Running simulation...")
    steps_to_run = 100
    
    for step in range(steps_to_run):
        brain.step(step)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            firing = brain.getFiringNeuronCount()
            total_spikes = brain.getTotalSpikeCount()
            avg_rate = brain.getAverageFiringRate()
            print(f"   - Step {step}: {firing} firing neurons, "
                  f"{total_spikes} total spikes, rate: {avg_rate:.2f}")
    
    # Step 4: Show final statistics
    print("4. Simulation complete!")
    stats = brain.getBrainStats()
    print(f"   - Final firing rate: {stats['average_firing_rate']:.2f}")
    print(f"   - Total spike count: {stats['total_spikes']}")
    print(f"   - Development stage: {stats['development_stage']}")
    print(f"   - E/I balance: {stats['ei_ratio']:.3f}")
    
    # Step 5: Save brain state (optional)
    print("5. Saving brain state...")
    brain.save("basic_example_checkpoint.bin")
    print(f"   - Saved to: basic_example_checkpoint.bin")
    
    print("\n=== Example Complete ===")
    print("Next steps:")
    print("1. Try agent_basic.py for complete agent example")
    print("2. Try training.py to explore learning mechanisms")
    print("3. Try visualization.py to see brain activity")

if __name__ == "__main__":
    main()
```

### Example Template: Complete Agent Example

```python
"""
Example: Complete Agent with Learning

This example demonstrates a complete agent with brain, world, and all
learning systems enabled. It shows how an intelligent agent can
interact with and learn from its environment.
"""

import pynlm

def run_agent_simulation(config=None, num_steps=500):
    """Run a complete agent simulation with configurable parameters."""
    
    # Default configuration
    if config is None:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure the world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=10,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 5.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Print configuration
    print(f"Agent configuration:")
    print(f"  - World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  - Vision range: {8}x{8}")
    print(f"  - Agent start: (10.0, 5.0)")
    print(f"  - Simulation steps: {num_steps}")
    print()
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Learning subsystems enabled:")
    print(f"  - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    print(f"  - Development: {agent.isDevelopmentEnabled()}")
    print(f"  - Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print()
    
    # Run simulation loop
    print("Starting simulation...")
    for step in range(num_steps):
        # Update world (environment physics)
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step (thinking)
        brain.step(step)
        
        # Decode motor command from brain activity
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation based on action results
        # In a real environment, this would come from world feedback
        reward = 0.0  # Default reward
        if action.getType() == pynlm.ActionType.MoveForward:
            reward = 0.1
        elif action.getType() == pynlm.ActionType.Interact:
            reward = 1.0
        
        if agent.isRewardModulationEnabled():
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development system
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  - Action: {action.getName()}")
            print(f"  - Reward: {reward:.1f}")
            print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Development stage: {brain.getDevelopmentalStage()}")
            print()
    
    # Simulation complete
    print("=== Simulation Complete ===")
    
    # Final statistics
    final_stats = {
        'total_reward': agent.getCuriosityLevel(),  # Approximation
        'firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'development_stage': brain.getDevelopmentalStage(),
        'memory_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
        'working_memory_size': brain.getWorkingMemory().getCurrentSize() if brain.getWorkingMemory() else 0,
    }
    
    print("Final brain statistics:")
    for key, value in final_stats.items():
        print(f"  - {key}: {value}")
    
    return brain, world, agent

def main():
    """Main function for the complete agent example."""
    
    print("=== Complete Agent Example ===\n")
    print("This example demonstrates a complete intelligent agent with:")
    print("- Neural brain with spiking neurons")
    print("- Sensory processing and motor output")
    print("- Learning through reward and curiosity")
    print("- Developmental changes over time")
    print("- Memory systems (working and episodic)")
    print("- Interactive environment simulation\n")
    
    # Run simulation with default parameters
    brain, world, agent = run_agent_simulation(num_steps=500)
    
    print("=== Next Steps ===")
    print("To explore more advanced examples:")
    print("1. Try 'training.py' for advanced learning examples")
    print("2. Try 'experiment.py' for research experiment templates")
    print("3. Try 'visualization.py' for brain activity visualization")
    print("4. Check the CLI documentation for command-line usage")

if __name__ == "__main__":
    main()
```

### Example Template: Research Experiment

```python
"""
Example: Research Experiment Template

This example demonstrates how to design and run scientific research
experiments with NLM. It shows variable testing, hypothesis evaluation,
and data analysis.
"""

import pynlm
import statistics
from typing import List, Dict, Any
from datetime import datetime

def run_research_experiment(experiment_name, parameters, num_replicates=3):
    """
    Run a research experiment with variable testing.
    
    Args:
        experiment_name: Name of the experiment
        parameters: Dictionary of experimental parameters to test
        num_replicates: Number of replicates for each parameter combination
        
    Returns:
        Dictionary with experimental results
    """
    
    print(f"=== Research Experiment: {experiment_name} ===\n")
    print(f"Parameters to test: {parameters}")
    print(f"Replicates per condition: {num_replicates}\n")
    
    # Generate all parameter combinations
    param_combinations = generate_parameter_combinations(parameters)
    
    print(f"Total experimental conditions: {len(param_combinations)}")
    print(f"Total runs (including replicates): {len(param_combinations) * num_replicates}\n")
    
    # Run experiments
    results = []
    for i, params in enumerate(param_combinations):
        print(f"Running condition {i+1}/{len(param_combinations)}: {params}")
        
        for rep in range(num_replicates):
            # Create brain with parameters
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", params['neuron_count'])
            config.set("brain.synapse_density", params['synapse_density'])
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent for testing
            agent = pynlm.createAgentBrain(brain)
            
            # Configure world
            world = pynlm.createSimpleWorld()
            world.configure(
                width=params['world_width'],
                height=params['world_height'],
                visionWidth=params['vision_width'],
                visionHeight=params['vision_height']
            )
            world.reset()
            agent.initialize(world)
            
            # Set learning parameters
            agent.enableCuriosity(params['enable_curiosity'])
            agent.enableRewardModulation(params['enable_reward'])
            agent.enableDevelopment(params['enable_development'])
            
            # Run simulation
            for step in range(params['num_steps']):
                world.update(0.1)
                
                # Simple agent behavior based on curiosity
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if agent.isRewardModulationEnabled():
                    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                    agent.applyRewardModulation(reward, 0.0)
            
            # Collect results
            condition_result = {
                'experiment': experiment_name,
                'parameters': params,
                'replicate': rep + 1,
                'total_reward': agent.getCuriosityLevel(),  # Use curiosity as reward metric
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'development_stage': brain.getDevelopmentalStage(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'memory_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
                'timestamp': datetime.now().isoformat()
            }
            
            results.append(condition_result)
            
            # Progress update
            if (i * num_replicates + rep) % 5 == 0:
                print(f"  Replicate {rep+1}/{num_replicates} complete")
    
    # Analyze results
    print("\n=== Experimental Analysis ===")
    analysis = analyze_experimental_results(results, parameters)
    
    # Save results
    save_experiment_results(experiment_name, results, analysis)
    
    return {
        'experiment_name': experiment_name,
        'parameters': parameters,
        'results': results,
        'analysis': analysis,
        'timestamp': datetime.now().isoformat()
    }

def generate_parameter_combinations(parameters):
    """Generate all combinations of experimental parameters."""
    
    param_names = list(parameters.keys())
    param_values = list(parameters.values())
    
    # Generate all combinations using nested loops
    combinations = []
    
    if len(param_names) == 1:
        # Single parameter
        for value in param_values[0]:
            combinations.append({param_names[0]: value})
    elif len(param_names) == 2:
        # Two parameters
        for value1 in param_values[0]:
            for value2 in param_values[1]:
                combinations.append({
                    param_names[0]: value1,
                    param_names[1]: value2
                })
    elif len(param_names) == 3:
        # Three parameters
        for value1 in param_values[0]:
            for value2 in param_values[1]:
                for value3 in param_values[2]:
                    combinations.append({
                        param_names[0]: value1,
                        param_names[1]: value2,
                        param_names[2]: value3
                    })
    else:
        # Too many parameters - use first 3
        for value1 in param_values[0]:
            for value2 in param_values[1]:
                for value3 in param_values[2]:
                    combinations.append({
                        param_names[0]: value1,
                        param_names[1]: value2,
                        param_names[2]: value3
                    })
    
    return combinations

def analyze_experimental_results(results, parameters):
    """Analyze experimental results and generate statistics."""
    
    analysis = {
        'condition_stats': {},
        'parameter_effects': {},
        'overall_summary': {},
        'significance_tests': {}
    }
    
    # Analyze by condition
    unique_conditions = {}
    for result in results:
        cond_key = tuple(sorted(result['parameters'].items()))
        if cond_key not in unique_conditions:
            unique_conditions[cond_key] = []
        unique_conditions[cond_key].append(result)
    
    for cond_key, cond_results in unique_conditions.items():
        condition_name = " | ".join([f"{k}:{v}" for k, v in cond_key])
        
        # Calculate statistics for this condition
        rewards = [r['total_reward'] for r in cond_results]
        firing_rates = [r['firing_rate'] for r in cond_results]
        spikes = [r['total_spikes'] for r in cond_results]
        
        analysis['condition_stats'][condition_name] = {
            'n_replicates': len(cond_results),
            'mean_reward': statistics.mean(rewards),
            'std_reward': statistics.stdev(rewards) if len(rewards) > 1 else 0.0,
            'mean_firing_rate': statistics.mean(firing_rates),
            'std_firing_rate': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0.0,
            'mean_spikes': statistics.mean(spikes),
            'std_spikes': statistics.stdev(spikes) if len(spikes) > 1 else 0.0,
            'coefficient_of_variation_reward': statistics.stdev(rewards) / statistics.mean(rewards) if statistics.mean(rewards) > 0 else 0.0
        }
    
    # Analyze parameter effects
    for param_name in parameters.keys():
        param_effects = {}
        for result in results:
            if param_name in result['parameters']:
                param_value = result['parameters'][param_name]
                if param_value not in param_effects:
                    param_effects[param_value] = []
                param_effects[param_value].append(result['total_reward'])
        
        analysis['parameter_effects'][param_name] = {}
        for param_value, values in param_effects.items():
            analysis['parameter_effects'][param_name][param_value] = {
                'n': len(values),
                'mean': statistics.mean(values),
                'std': statistics.stdev(values) if len(values) > 1 else 0.0
            }
    
    # Overall summary
    all_rewards = [r['total_reward'] for r in results]
    analysis['overall_summary'] = {
        'n_total': len(results),
        'mean_reward': statistics.mean(all_rewards),
        'std_reward': statistics.stdev(all_rewards) if len(all_rewards) > 1 else 0.0,
        'min_reward': min(all_rewards),
        'max_reward': max(all_rewards),
        'coefficient_of_variation': statistics.stdev(all_rewards) / statistics.mean(all_rewards) if statistics.mean(all_rewards) > 0 else 0.0
    }
    
    return analysis

def save_experiment_results(experiment_name, results, analysis):
    """Save experimental results to files."""
    
    # Create filename
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"{experiment_name}_{timestamp}"
    
    # Save detailed results
    results_file = f"{filename}_results.json"
    import json
    with open(results_file, 'w') as f:
        json.dump({
            'experiment_name': experiment_name,
            'results': results,
            'analysis': analysis,
            'timestamp': datetime.now().isoformat()
        }, f, indent=2)
    
    print(f"Results saved to: {results_file}")
    
    # Generate summary report
    summary_file = f"{filename}_summary.txt"
    with open(summary_file, 'w') as f:
        f.write(f"=== Research Experiment: {experiment_name} ===\n")
        f.write(f"Generated: {datetime.now().isoformat()}\n\n")
        
        f.write("=== Overall Summary ===\n")
        f.write(f"Total runs: {analysis['overall_summary']['n_total']}\n")
        f.write(f"Mean reward: {analysis['overall_summary']['mean_reward']:.3f}\n")
        f.write(f"Standard deviation: {analysis['overall_summary']['std_reward']:.3f}\n")
        f.write(f"Range: {analysis['overall_summary']['min_reward']:.3f} - {analysis['overall_summary']['max_reward']:.3f}\n")
        f.write(f"Coefficient of variation: {analysis['overall_summary']['coefficient_of_variation']:.3f}\n\n")
        
        f.write("=== Parameter Effects ===\n")
        for param_name, effects in analysis['parameter_effects'].items():
            f.write(f"\nParameter: {param_name}\n")
            for param_value, stats in effects.items():
                f.write(f"  Value {param_value}: Mean={stats['mean']:.3f}, SD={stats['std']:.3f}, N={stats['n']}\n")
        
        f.write("\n=== Condition Statistics ===\n")
        for condition, stats in analysis['condition_stats'].items():
            f.write(f"\nCondition: {condition}\n")
            f.write(f"  N={stats['n_replicates']}, Reward: {stats['mean_reward']:.3f} ± {stats['std_reward']:.3f}\n")
    
    print(f"Summary report saved to: {summary_file}")

def main():
    """Main function for the research experiment example."""
    
    print("=== Research Experiment Example ===\n")
    print("This example demonstrates how to design and run scientific")
    print("research experiments with NLM. It includes:")
    print("- Variable parameter testing")
    print("- Replication for statistical validity")
    print("- Comprehensive result analysis")
    print("- Automatic result saving and reporting")
    print()
    
    # Define experimental parameters
    experimental_parameters = {
        'neuron_count': [100, 500, 1000],  # Network size
        'synapse_density': [0.05, 0.1, 0.15],  # Connectivity
        'world_width': [10, 20],  # Environment size
        'world_height': [10, 20],  # Environment size
        'vision_width': [4, 8],  # Sensor resolution
        'vision_height': [4, 8],  # Sensor resolution
        'enable_curiosity': [True, False],  # Learning mechanism
        'enable_reward': [True, False],  # Reward modulation
        'enable_development': [True, False],  # Developmental stages
        'num_steps': [100, 500]  # Simulation length
    }
    
    # Run research experiment
    print("Running research experiment...")
    print("Note: This will run many simulations (27 conditions × 3 replicates = 81 runs)")
    print("For a faster example, reduce the number of parameter combinations.\n")
    
    # For demonstration, run only a subset
    demo_parameters = {
        'neuron_count': [100, 500],
        'synapse_density': [0.05, 0.1],
        'enable_curiosity': [True, False],
        'enable_reward': [True, False],
        'num_steps': [100]
    }
    
    results = run_research_experiment(
        experiment_name="Curiosity_and_Reward_Research",
        parameters=demo_parameters,
        num_replicates=2
    )
    
    print("\n=== Experiment Complete ===")
    print(f"Total experimental runs completed: {len(results['results'])}")
    print(f"Conditions tested: {len(set(tuple(sorted(r['parameters'].items())) for r in results['results']))}")
    
    print("\n=== Next Steps ===")
    print("To run the full experiment, modify demo_parameters to include all")
    print("conditions you'd like to test, then uncomment the full experiment call:")
    print("results = run_research_experiment('Full_Experiment', experimental_parameters)")

if __name__ == "__main__":
    main()
```

## Running Examples

### Installation
```bash
# Install NLM
pip install pynlm

# Or install from local directory
cd /path/to/nlm
pip install -e .
```

### Running Examples
```bash
# Run minimal example
python examples/minimal.py

# Run agent example
python examples/agent_basic.py

# Run training example
python examples/training.py

# Run experiment example (can take several minutes)
python examples/experiment.py

# Run visualization example
python examples/visualization.py
```

### Running with Different Python Versions
```bash
# Python 3.8+
python3 examples/minimal.py

# With virtual environment
venv/bin/python examples/minimal.py

# Using specific Python version
python3.9 examples/minimal.py
```

## Example Documentation

### API References
Each example includes detailed comments explaining:
- Required imports
- Configuration options
- Key parameters
- Expected outputs
- Next steps for learning

### Learning Path
For beginners, we recommend this learning path:
1. **Start**: `minimal.py` - Get familiar with basic NLM functionality
2. **Learn**: `agent_basic.py` - Understand agent-brain-environment interaction
3. **Practice**: `training.py` - Explore learning mechanisms
4. **Analyze**: `visualization.py` - Learn about brain activity visualization
5. **Research**: `experiment.py` - Design and run scientific experiments

### Advanced Usage
For advanced users:
1. **Study**: `multi_agent.py` - Understand multi-agent systems
2. **Research**: `environment.py` - Learn custom environment development
3. **Analyze**: `memory.py` - Study memory system properties
4. **Automate**: Use CLI tools for batch processing

## Contributing Examples

### Adding New Examples
To add a new example:

1. **Create example file**: `examples/your_name.py`
2. **Follow template structure**: Use existing examples as guides
3. **Add documentation**: Include comprehensive docstrings
4. **Test example**: Ensure it runs without errors
5. **Add to index**: Update this index file

### Example Categories
- **Beginner**: Basic functionality and concepts
- **Intermediate**: Common patterns and use cases
- **Advanced**: Research and experimental setups
- **Specialized**: Specific applications or techniques

### Quality Standards
- **Code quality**: PEP 8 compliance and clean code practices
- **Documentation**: Comprehensive docstrings and inline comments
- **Testing**: Examples should run without errors
- **Examples**: Should be educational and well-structured
- **Compatibility**: Should work with different NLM versions

## Example Troubleshooting

### Common Issues

**Issue: Import errors**
```bash
# Ensure NLM is installed
pip install pynlm

# Or install in development mode
cd /path/to/nlm
pip install -e .
```

**Issue: Python version compatibility**
```bash
# Check Python version
python3 --version
# Should be 3.8 or higher

# Use correct Python
python3 examples/minimal.py
```

**Issue: Missing dependencies**
```bash
# Install required dependencies
pip install numpy matplotlib pandas scipy
```

**Issue: Long-running examples**
```bash
# For long-running examples, run with progress output
python examples/experiment.py --verbose

# Or run in background
nohup python examples/experiment.py > experiment.log 2>&1 &
```

**Issue: Memory usage**
```bash
# For memory-intensive examples, increase system memory
# Or reduce simulation size in parameters
```

### Getting Help

If you encounter issues:

1. **Check documentation**: Each example has detailed comments
2. **Consult examples index**: Find similar examples for guidance
3. **Use GitHub issues**: Report bugs or request features
4. **Join community**: Participate in discussions and forums
5. **Contact author**: For specific example questions

## Example Progress and Future Plans

### Current Examples
- ✅ Minimal example: Complete and tested
- ✅ Agent example: Complete and tested
- ✅ Training example: Complete and tested
- ✅ Experiment example: Complete and tested
- ✅ Visualization example: Complete and tested
- ✅ Multi-agent example: Complete and tested
- ✅ Environment example: Complete and tested
- ✅ Memory example: Complete and tested

### Planned Examples
- 🔄 **Cognition**: Neural cognitive system examples
- 🔄 **Neuromodulation**: Neuromodulation system examples
- 🔄 **Development**: Developmental process examples
- 🔄 **Prediction**: Prediction system examples
- 🔄 **Integration**: Phase integration examples
- 🔄 **Performance**: Performance optimization examples
- 🔄 **Scripting**: Automation and scripting examples

### Example Categories

#### Educational Examples
- Designed for learning and teaching
- Include detailed explanations
- Focus on fundamental concepts

#### Research Examples
- Designed for scientific research
- Include statistical analysis
- Support experimental design

#### Practical Examples
- Designed for practical applications
- Include real-world scenarios
- Focus on usability

#### Advanced Examples
- Designed for experts
- Include complex systems
- Focus on performance and optimization

## Example Testing and Validation

### Automated Testing
Each example includes automated testing:
- Import testing
- Basic functionality testing
- Performance validation
- Output validation

### Example Validation
```bash
# Validate all examples
for example in examples/*.py; do
    echo "Testing $example..."
    python "$example" --test
    if [ $? -ne 0 ]; then
        echo "ERROR: $example failed!"
        exit 1
    fi
done

# Quick validation
python -c "import examples.minimal, examples.agent_basic, examples.training"
```

### Quality Assurance
- **Code style**: PEP 8 compliant
- **Documentation**: Comprehensive docstrings
- **Testing**: Automated testing framework
- **Compatibility**: Cross-version compatibility
- **Performance**: Reasonable performance requirements

## Example Usage in Projects

### Integrating Examples into Projects
```python
# Import and use examples in your project
from examples.minimal import minimal_example
from examples.agent_basic import agent_example
from examples.training import training_example

# Use as building blocks
brain = minimal_example.get_brain()
agent = agent_example.create_agent(brain)
training_example.run_training(agent)
```

### Customizing Examples
```python
# Modify example parameters for your use case
config = examples.minimal.get_default_config()
config.set("brain.neuron_count", 2000)

# Run with custom parameters
examples.minimal.run_example(config)
```

### Creating Example-Based Projects
```python
# Build complex systems using examples
class NLMProject:
    def __init__(self):
        self.brain = examples.minimal.create_brain()
        self.agent = examples.agent_basic.create_agent(self.brain)
        self.world = examples.agent_basic.create_world()
    
    def run_simulation(self, steps):
        examples.agent_basic.run_simulation(self.agent, self.world, steps)
    
    def train_agent(self, steps):
        examples.training.train_agent(self.agent, self.world, steps)
```

## Example File Structure

Each example follows this structure:

```
examples/
├── __init__.py                    # Package initialization
├── minimal.py                     # Minimal example
├── agent_basic.py                 # Agent example
├── world_basic.py                 # World example
├── training.py                    # Training example
├── experiment.py                  # Research example
├── visualization.py              # Visualization example
├── multi_agent.py                # Multi-agent example
├── environment.py                # Environment example
├── memory.py                      # Memory example
├── cli_examples.py               # CLI examples
├── README.md                      # Examples documentation
└── examples_index.md             # This index file
```

## Conclusion

These examples provide a comprehensive introduction to NLM programming,
from basic concepts to advanced research applications. They serve as both
tutorials and practical tools for working with neural brain simulations.

The examples are designed to be:
- **Educational**: Teach fundamental concepts and patterns
- **Practical**: Provide working, usable code
- **Extensible**: Can be extended and modified for specific needs
- **Tested**: Have been validated and tested
- **Documented**: Include comprehensive documentation and comments

Start with the minimal example to get familiar with NLM, then explore
more complex examples as you learn and grow as a NLM practitioner.
