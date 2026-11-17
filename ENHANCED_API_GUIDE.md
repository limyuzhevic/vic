# Enhanced NLM Python API - Quick Start Guide

## Overview

This guide demonstrates the new research-oriented features and convenience methods added to the NLM Python API. These improvements make it easier for both beginners and advanced researchers to work with the neural simulation framework.

## Key Improvements

### 1. Convenience Functions for Research

The NLM API now includes specialized functions designed for research workflows:

#### `createResearchBrain()`
Create a research-ready brain with sensible defaults:

```python
import pynlm

# Create a medium-sized research brain (recommended for most research)
brain = pynlm.createResearchBrain()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Create specific research configurations
brain_small = pynlm.createResearchBrain("small")   # 100 neurons
brain_medium = pynlm.createResearchBrain("medium") # 1000 neurons  
brain_large = pynlm.createResearchBrain("large")   # 10000 neurons
```

#### `createStandardEnvironment()`
Create a standardized research environment:

```python
world = pynlm.createStandardEnvironment()
world.configure(20, 20, 8, 8)  # 20x20 world with 8x8 vision
world.reset()
```

### 2. Research Experiment Runner

The `runResearchExperiment()` function automates complete research workflows:

```python
import pynlm

# Setup research components
brain = pynlm.createResearchBrain("medium")
world = pynlm.createStandardEnvironment()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete experiment
results = pynlm.runResearchExperiment(
    brain=brain,
    world=world,
    agent=agent,
    numSteps=1000,
    enableLearning=True,
    enableCuriosity=True
)

print("Experiment completed!")
print(f"Total spikes: {results['total_spikes']}")
print(f"Learning enabled: {results['learning_enabled']}")
```

### 3. Result Export for Analysis

Export experiment results to CSV for analysis:

```python
# Export results using the new convenience function
filename = "my_experiment.csv"
pynlm.exportExperimentResults(results, filename)

# The exported CSV includes:
# - experiment metrics
# - configuration parameters  
# - final system state
```

### 4. Research Configuration Presets

Access predefined research configurations:

```python
# Get all research presets
presets = pynlm.getResearchPresets()

# Available presets:
# - "small": 100 neurons for rapid prototyping
# - "medium": 1000 neurons for exploratory research  
# - "large": 10000 neurons for comprehensive studies

for name, preset in presets.items():
    print(f"{name}: {preset['neuron_count']} neurons, {preset['region_count']} regions")
```

### 5. Enhanced Error Handling

The API now provides custom Python exceptions for better error handling:

```python
import pynlm

try:
    # Your research code here
    results = pynlm.runResearchExperiment(...)
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid arguments: {e}")
except pynlm.OutOfRangeError as e:
    print(f"Out of range: {e}")
```

## Complete Example: A Research Workflow

Here's a complete example demonstrating the enhanced NLM API for a research study:

```python
#!/usr/bin/env python3
"""
Example: Neural Dynamics Research Study

This example demonstrates a complete research workflow using the enhanced NLM API.
"""

import pynlm
import time
import json

def main():
    print("=== Neural Dynamics Research Study ===\n")
    
    # 1. Setup research configuration
    print("1. Configuring research parameters")
    config = {
        "experiment_name": "neural_dynamics_2024",
        "brain_size": "medium",
        "duration_steps": 2000,
        "enable_learning": True,
        "enable_curiosity": True,
        "enable_development": True
    }
    
    print(f"Research configuration: {config}")
    
    # 2. Create research components using enhanced API
    print("\n2. Creating research components")
    brain = pynlm.createResearchBrain(config["brain_size"])
    world = pynlm.createStandardEnvironment()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"• Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"• World: {world.getWidth()}x{world.getHeight()} with {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()} vision")
    
    # 3. Enable research features
    print("\n3. Enabling research features")
    if config["enable_learning"]:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        print("• Learning enabled: reward modulation + structural plasticity")
    
    if config["enable_curiosity"]:
        agent.enableCuriosity(True)
        print("• Curiosity enabled: exploration behavior")
    
    if config["enable_development"]:
        agent.enableDevelopment(True)
        print("• Development enabled: brain maturation")
    
    # 4. Run the research experiment
    print(f"\n4. Running experiment ({config['duration_steps']} steps)")
    start_time = time.time()
    
    results = pynlm.runResearchExperiment(
        brain=brain,
        world=world,
        agent=agent,
        numSteps=config["duration_steps"],
        enableLearning=config["enable_learning"],
        enableCuriosity=config["enable_curiosity"]
    )
    
    end_time = time.time()
    
    # 5. Analyze and export results
    print("\n5. Analyzing and exporting results")
    
    # Add research metadata
    results.update({
        "experiment_name": config["experiment_name"],
        "duration_seconds": end_time - start_time,
        "steps_per_second": config["duration_steps"] / (end_time - start_time),
        "research_version": pynlm.__version__,
        "analysis_timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
    })
    
    print(f"• Experiment completed in {end_time - start_time:.2f} seconds")
    print(f"• Processing rate: {results['steps_per_second']:.1f} steps/second")
    print(f"• Total neural activity: {results['total_spikes']} spikes")
    
    # Export results for analysis
    filename = f"{config['experiment_name']}_results.csv"
    pynlm.exportExperimentResults(results, filename)
    print(f"• Results exported to: {filename}")
    
    # Create summary JSON
    summary = {
        "experiment": config["experiment_name"],
        "parameters": config,
        "results": {k: v for k, v in results.items() if k not in ["experiment_name", "duration_seconds", "steps_per_second"]},
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
    }
    
    with open(f"{config['experiment_name']}_summary.json", "w") as f:
        json.dump(summary, f, indent=2)
    
    print(f"• Summary JSON created: {config['experiment_name']}_summary.json")
    
    print("\n" + "="*50)
    print("Research study completed successfully!")
    print("="*50)

if __name__ == "__main__":
    main()
```

## Benefits for Different User Levels

### For Beginners
- **Simple setup**: One-line brain creation with sensible defaults
- **Guided workflows**: Automated experiment runner with sensible parameters
- **Error handling**: Clear error messages and custom exceptions
- **Documentation**: Comprehensive examples and guidance

### For Intermediate Researchers
- **Research presets**: Pre-configured experiment setups
- **Standard environments**: Consistent testing conditions
- **Result export**: Ready-to-analyze data formats
- **Batch processing**: Export results for statistical analysis

### For Advanced Researchers
- **Flexible configuration**: Fine-grained control over all parameters
- **Modular design**: Easy integration with existing research workflows
- **Custom presets**: Define your own research configurations
- **API extensibility**: Hook into any part of the research process

## Installation and Usage

The enhanced API is available in the same package as the original NLM bindings. Simply:

```bash
pip install pynlm
```

Then use the enhanced features:

```python
import pynlm

# All new features are available immediately
brain = pynlm.createResearchBrain()
results = pynlm.runResearchExperiment(...)
```

## Backward Compatibility

All enhancements maintain full backward compatibility:

- Existing code using `pynlm.createBrain(config)` continues to work unchanged
- All original classes and methods remain available
- New features are additive and opt-in
- No breaking changes to the existing API

## Conclusion

The enhanced NLM Python API transforms the framework from a research prototype into a production-ready research tool. By providing:

1. **Convenience**: One-line setup for common research tasks
2. **Automation**: Built-in experiment runners and workflows  
3. **Integration**: Ready-to-use result export for analysis
4. **Accessibility**: Better error handling and documentation
5. **Flexibility**: Presets and customization for all research needs

Researchers can now focus on their scientific questions rather than technical setup and data management, while advanced users retain full control over their experiments.

The enhanced API successfully addresses all the issues identified in the codebase analysis:
- ✅ O(N²) bottleneck fixed
- ✅ Memory management improved
- ✅ Python API made more accessible
- ✅ Research tools added
- ✅ Documentation and testing improved

This represents a significant step forward in making NLM a premier open-source tool for brain-inspired AI research and education.
