#!/usr/bin/env python3
"""
Comprehensive Documentation for Enhanced NLM (Neural Learning Machine)

This module provides detailed documentation for the enhanced NLM framework,
including usage examples, API references, and best practices for getting
started with neural simulation.
"""

__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"

from nlm import (
    BrainSimulation,
    ExperimentManager,
    get_brain_simulation,
    get_experiment_manager,
    NLMConfigManager,
    create_default_config,
    create_small_config,
    create_medium_config,
    create_large_config,
)
from nlm.error_handling import (
    NLMError,
    ConfigurationError,
    SimulationError,
    BrainError,
    handle_error,
    validate_configuration,
)

# Import examples for demonstration
try:
    from .examples import basic_usage, experiment_templates, visualization_examples
    EXAMPLES_AVAILABLE = True
except ImportError:
    EXAMPLES_AVAILABLE = False

# Documentation sections
class DocumentationSection:
    """Class representing a documentation section"""
    
    def __init__(self, title: str, content: str):
        self.title = title
        self.content = content
    
    def display(self):
        """Display the documentation section"""
        print(f"=== {self.title} ===")
        print(self.content)
        print()


# Main documentation functions
def get_quick_start_guide() -> str:
    """Get quick start guide for NLM"""
    return """
NLM (Neural Learning Machine) - Quick Start Guide
=================================================

This guide provides the essential steps to get started with NLM neural simulation.

## Step 1: Installation

1. Install NLM with Python bindings:
   ```bash
   pip install nlm
   ```

2. For development, clone the repository:
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   pip install -e .
   ```

## Step 2: Basic Configuration

Create a configuration file or use the built-in configurations:

```python
from nlm import create_medium_config, BrainSimulation

# Use medium configuration
config = create_medium_config()
simulation = BrainSimulation(config)
```

## Step 3: Initialize Brain

```python
simulation = BrainSimulation()
simulation.initialize()
```

## Step 4: Run Simulation

```python
# Run basic simulation
results = simulation.run_simulation(1000)
print(f"Total spikes: {results['total_spikes']}")
print(f"Average firing rate: {results['average_firing_rate']:.3f} Hz")
```

## Step 5: Save and Load

```python
# Save simulation state
simulation.save_simulation("my_simulation.checkpoint")

# Create new simulation and load saved state
new_simulation = BrainSimulation()
new_simulation.initialize()
new_simulation.load_simulation("my_simulation.checkpoint")
```

## Step 6: Run Experiments

```python
from nlm import get_experiment_manager

experiment_manager = get_experiment_manager()
simulation = BrainSimulation()
simulation.initialize()

# Run working memory experiment
results = experiment_manager.run_experiment(
    simulation.brain,
    "working_memory",
    trials=10,
    steps_per_trial=100
)
```

## Quick Commands

Using the NLM CLI:

```bash
# Run quick demo
nlm-run --quick-demo

# Run Phase 2 demo
nlm-run --phase2 --neurons 500 --steps 500

# Run Phase 4 demo
nlm-run --phase4

# Create configuration
nlm-config create my_config

# Validate configuration
nlm-config validate my_config.json

# Start interactive Python session
nlm-python interactive
```

## Getting Help

```python
# Display help
help(nlm)

# Display error information
help(nlm.error_handling.NLMError)

# View examples
import nlm.examples.basic_usage as examples
examples.example_basic_usage()
```

That's it! You're now ready to start exploring NLM neural simulation.
For more detailed examples, see the examples/ directory in the repository.
"""


def get_api_reference() -> str:
    """Get API reference documentation"""
    return """
NLM API Reference
===================

## Core Classes

### BrainSimulation
Manages brain simulation with enhanced interface.

**Methods:**
- `__init__(config=None)` - Initialize with optional configuration
- `initialize()` - Initialize the brain
- `run_simulation(steps, enable_checkpoints=False)` - Run simulation
- `inject_sensory_input(input_data, input_type='internal')` - Inject sensory input
- `get_brain_state()` - Get current brain state
- `save_simulation(filepath)` - Save simulation state
- `load_simulation(filepath)` - Load simulation state

**Properties:**
- `brain` - The underlying NLM brain object
- `current_step` - Current simulation step
- `results` - Simulation results

### ExperimentManager
Manages running NLM experiments.

**Methods:**
- `__init__()` - Initialize experiment manager
- `run_experiment(brain, experiment_name, trials=10, steps_per_trial=100)` - Run specific experiment
- `run_all_experiments(brain, trials=10, steps_per_trial=100)` - Run all experiments

**Available Experiments:**
- temporal_prediction - Temporal prediction learning
- working_memory - Working memory capacity testing
- episodic_recall - Episodic memory recall accuracy
- concept_formation - Concept formation and generalization
- attention - Neural attention mechanisms
- planning - Multi-step planning abilities
- self_model - Self-model development
- social_learning - Social learning and imitation
- continual_learning - Continual learning and adaptation
- phase4_integrated - Integrated Phase 4 experiments

### NLMConfigManager
Enhanced configuration management.

**Methods:**
- `__init__(config_dir='configs')` - Initialize with config directory
- `create_config(name, config_data=None)` - Create configuration file
- `load_config(name)` - Load configuration from file
- `validate_config(config)` - Validate configuration dictionary
- `show_config(name)` - Display configuration contents
- `list_configs()` - List available configurations
- `export_config(name, output_format='ini')` - Export configuration

### Error Classes

**NLMError:** Base exception class for all NLM errors
**ConfigurationError:** Error in configuration handling
**SimulationError:** Error in simulation execution
**BrainError:** Error in brain initialization or operation

## Configuration Functions

### create_default_config()
Returns a default configuration dictionary.

### create_small_config()
Returns a small configuration for quick testing.

### create_medium_config()
Returns a medium configuration for standard experiments.

### create_large_config()
Returns a large configuration for intensive research.

## Utility Functions

### handle_error(error, error_code='UNKNOWN_ERROR', context=None, recovery_strategy='log_and_reraise')
Handle errors using the global error handler.

### validate_configuration(config)
Validate configuration dictionary.

### format_error_for_display(error)
Format error for user-friendly display.

### create_error_context(error_code, recovery_strategy='log_and_reraise', context=None)
Create error context for specific code blocks.

## Examples Module

### basic_usage
Examples for basic NLM usage:
- example_basic_usage() - Basic brain simulation
- example_configuration_management() - Configuration management
- example_quick_demo() - Quick demo function
- example_simulation_with_input() - Simulation with sensory input
- example_configuration_comparison() - Compare different configurations
- example_error_handling() - Error handling examples
- example_save_load() - Save and load simulation state

### experiment_templates
Examples for experiment management:
- example_experiment_manager_setup() - Set up experiment manager
- example_single_experiment() - Run a single experiment
- example_all_experiments() - Run all experiments
- example_experiment_with_input() - Experiment with sensory input
- example_experiment_results_analysis() - Analyze experiment results
- example_experiment_reproduction() - Reproduce experiments
- example_experiment_parameter_sweep() - Parameter sweep experiments

### visualization_examples
Examples for visualization:
- example_basic_brain_visualization() - Basic brain state visualization
- example_connectivity_visualization() - Neural connectivity visualization
- example_activity_animation() - Activity animation
- example_comparison_visualization() - Comparison visualization
- example_progress_visualization() - Progress tracking visualization
"""


def get_best_practices() -> str:
    """Get best practices for using NLM"""
    return """
NLM Best Practices
===================

## Configuration Management

### Use Appropriate Configuration Size
1. **Quick Testing**: Use small configurations for testing and debugging
2. **Standard Experiments**: Use medium configurations for regular experiments
3. **Research**: Use large configurations for intensive research

```python
from nlm import create_small_config, create_medium_config, create_large_config

# For testing
config = create_small_config()

# For experiments  
config = create_medium_config()

# For research
config = create_large_config()
```

### Validate Configurations
Always validate your configuration before running simulations:

```python
from nlm.error_handling import validate_configuration, ConfigurationError

config = {"neuron_count": 1000, "region_count": 2, "connection_probability": 0.1}
try:
    if validate_configuration(config):
        print("Configuration is valid")
    else:
        print("Configuration is invalid")
except ConfigurationError as e:
    print(f"Configuration error: {e}")
```

## Simulation Management

### Start with Simple Simulations
1. Begin with small neuron counts (100-500 neurons)
2. Use fewer simulation steps initially (100-500 steps)
3. Test with a single region before expanding to multiple regions

```python
# Start with simple configuration
simulation = BrainSimulation(create_small_config())
simulation.initialize()
results = simulation.run_simulation(100)
```

### Use Checkpoints Wisely
- Enable checkpoints for long-running simulations
- Set appropriate checkpoint intervals based on simulation length
- Monitor checkpoint file sizes to avoid disk space issues

```python
# Run simulation with checkpoints every 1000 steps
results = simulation.run_simulation(5000, enable_checkpoints=True)
```

### Monitor Progress
- Use the NLM CLI status command to monitor simulation progress
- Set up logging to track simulation performance
- Implement error handling for long-running simulations

```bash
nlm-status
```

## Error Handling

### Use Built-in Error Recovery
1. Let NLM handle errors with its built-in recovery mechanisms
2. Use the error context manager for specific error handling
3. Provide informative suggestions for error recovery

```python
from nlm.error_handling import create_error_context, NLMError

# Use error context for specific error handling
with create_error_context("SIMULATION_ERROR", recovery_strategy="recover"):
    # Your simulation code here
    simulation.run_simulation(1000)
```

### Handle Different Error Types Appropriately
- **ConfigurationError**: Check configuration files and parameters
- **SimulationError**: Reduce simulation complexity or check for infinite loops
- **BrainError**: Verify brain initialization and configuration

```python
from nlm.error_handling import ConfigurationError, SimulationError, BrainError

try:
    # Your simulation code here
    simulation.run_simulation(1000)
except ConfigurationError as e:
    print(f"Configuration problem: {e}")
    print("Suggestions:", e.suggestions)
except SimulationError as e:
    print(f"Simulation problem: {e}")
    print("Suggestions:", e.suggestions)
except BrainError as e:
    print(f"Brain problem: {e}")
    print("Suggestions:", e.suggestions)
```

## Experiment Management

### Design Experiments Properly
1. Choose appropriate trial counts based on simulation complexity
2. Use smaller trial counts for testing (2-5 trials)
3. Use larger trial counts for final experiments (10-50 trials)
4. Balance between statistical power and computational cost

```python
# For testing: fewer trials, shorter duration
experiment_manager.run_experiment(
    brain, "working_memory", 
    trials=3, steps_per_trial=50
)

# For final experiments: more trials, longer duration
experiment_manager.run_experiment(
    brain, "working_memory",
    trials=20, steps_per_trial=200
)
```

### Run Controlled Experiments
1. Keep all parameters constant except the one being tested
2. Use randomization to avoid order effects
3. Document all experimental conditions

```python
# Run multiple conditions with different parameters
conditions = [
    {"trials": 5, "steps": 100, "config": create_small_config()},
    {"trials": 10, "steps": 200, "config": create_medium_config()},
]

for i, condition in enumerate(conditions):
    print(f"\nCondition {i+1}: {condition}")
    simulation = BrainSimulation(condition["config"])
    simulation.initialize()
    
    results = experiment_manager.run_experiment(
        simulation.brain,
        "working_memory",
        trials=condition["trials"],
        steps_per_trial=condition["steps"]
    )
    
    # Compare results across conditions
    if isinstance(results, dict):
        print(f"Results: {results}")
```

## Visualization and Analysis

### Use Appropriate Visualization Methods
1. **Basic Analysis**: Use 2D plots for quick insights
2. **Complex Analysis**: Use 3D plots for multi-dimensional data
3. **Animation**: Use animations for time-series data
4. **Comparative Analysis**: Use side-by-side comparisons

### Follow Visualization Best Practices
- Label all axes and provide clear titles
- Use appropriate color schemes and scales
- Include statistical indicators where appropriate
- Save visualizations with descriptive filenames

```python
# Example visualization with proper labeling
import matplotlib.pyplot as plt

plt.figure(figsize=(10, 6))
plt.plot(range(steps), spike_counts, 'b-', linewidth=2, label='Spikes')
plt.xlabel('Simulation Steps')
plt.ylabel('Spike Count')
plt.title('Neuronal Activity Over Time')
plt.legend()
plt.grid(True, alpha=0.3)
plt.savefig('activity_over_time.png', dpi=150, bbox_inches='tight')
```

## Documentation and Code Quality

### Write Self-Documenting Code
1. Use descriptive variable and function names
2. Add docstrings to all public functions
3. Include type hints where possible
4. Use comments to explain complex algorithms

```python
def run_brain_simulation(config, steps):
    """
    Run brain simulation with given configuration and steps.
    
    Args:
        config (dict): Brain configuration parameters
        steps (int): Number of simulation steps to run
        
    Returns:
        dict: Simulation results including spike count, firing rate, etc.
        
    Raises:
        ConfigurationError: If configuration is invalid
        SimulationError: If simulation fails
        BrainError: If brain initialization fails
    """
    # Implementation here
```

### Maintain Comprehensive Documentation
1. Keep README files updated with examples
2. Document configuration parameters and their effects
3. Provide troubleshooting guides for common issues
4. Include API documentation with examples

## Performance Optimization

### Profile Your Simulations
1. Use timing to identify performance bottlenecks
2. Monitor memory usage for large simulations
3. Optimize configurations for your hardware
4. Consider parallel processing for large-scale simulations

### Use Appropriate Resource Levels
1. Match configuration to available hardware resources
2. Use checkpointing for long-running simulations
3. Monitor system resources during simulation
4. Use smaller configurations for development and testing

## Getting Help

### Available Resources
1. **NLM Documentation**: See docs/ directory for comprehensive documentation
2. **Error Logs**: Check nlm_errors.log for detailed error information
3. **Examples**: Run examples in examples/ directory
4. **Community**: Join NLM user community for support

### Troubleshooting Common Issues
1. **Configuration Problems**: Use 'nlm-config validate <file>' to check
2. **Performance Issues**: Reduce configuration size or optimize parameters
3. **Memory Issues**: Use smaller configurations or enable checkpointing
4. **Unknown Errors**: Check error logs and use error context for debugging

### Reporting Issues
1. Provide detailed error messages and stack traces
2. Include configuration files and simulation parameters
3. Document steps to reproduce the issue
4. Check existing issues before reporting new ones

These best practices will help you effectively use and extend the NLM framework
for your neural simulation needs.
"""


def get_examples_index() -> str:
    """Get index of all examples"""
    return """
NLM Examples Index
===================

## Basic Usage Examples
File: examples/basic_usage.py

These examples demonstrate fundamental NLM usage patterns:

1. **Basic Usage**: Complete brain simulation workflow
2. **Configuration Management**: Create, validate, and export configurations
3. **Quick Demo**: Use built-in quick demo function
4. **Simulation with Input**: Run simulations with sensory input
5. **Configuration Comparison**: Compare different configuration sizes
6. **Error Handling**: Demonstrate error handling and recovery
7. **Save and Load**: Demonstrate simulation state persistence

## Experiment Management Examples
File: examples/experiment_templates.py

These examples demonstrate experiment management and analysis:

1. **Experiment Manager Setup**: Initialize and configure experiment manager
2. **Single Experiment**: Run a specific experiment
3. **All Experiments**: Run all available experiments
4. **Experiment with Input**: Run experiments with sensory input
5. **Results Analysis**: Analyze and compare experiment results
6. **Experiment Reproduction**: Reproduce experiments with different configurations
7. **Parameter Sweep**: Run parameter sweep experiments

## Visualization Examples
File: examples/visualization_examples.py

These examples demonstrate visualization capabilities:

1. **Basic Brain Visualization**: Basic brain state visualization
2. **Connectivity Visualization**: Neural connectivity graphs
3. **Activity Animation**: Animated neuron activity visualization
4. **Comparison Visualization**: Compare different configurations
5. **Progress Visualization**: Track simulation progress over time

## How to Run Examples

### Install Dependencies
```bash
pip install matplotlib numpy
```

### Run Basic Usage Examples
```bash
python -c "import nlm.examples.basic_usage as examples; examples.example_basic_usage()"
```

### Run Experiment Examples
```bash
python -c "import nlm.examples.experiment_templates as examples; examples.example_single_experiment()"
```

### Run Visualization Examples
```bash
python -c "import nlm.examples.visualization_examples as examples; examples.example_progress_visualization()"
```

### View All Examples
```python
# Import and run all examples
import nlm.examples.basic_usage as basic
import nlm.examples.experiment_templates as experiments
import nlm.examples.visualization_examples as visualization

print("Basic Usage Examples:")
basic.example_basic_usage()
basic.example_configuration_management()
basic.example_quick_demo()

print("\nExperiment Examples:")
experiments.example_experiment_manager_setup()
experiments.example_single_experiment()

print("\nVisualization Examples:")
visualization.example_progress_visualization()
```

## Example File Structure

Each example file follows a consistent structure:

1. **Imports**: Import necessary NLM modules
2. **Function Definitions**: Define example functions
3. **Main Execution**: Run examples when script is executed directly
4. **Documentation**: Include docstrings and comments
5. **Error Handling**: Handle errors gracefully
6. **Output**: Provide informative output and feedback

## Custom Examples

### Create Your Own Examples
1. Start with basic usage examples
2. Modify parameters to test different scenarios
3. Combine multiple concepts for complex workflows
4. Document your examples with clear comments

### Example Template
```python
"""Your custom example description"""

from nlm import BrainSimulation, create_medium_config

def your_example_function():
    """Describe what this example does"""
    # Implementation here
    pass

if __name__ == "__main__":
    your_example_function()
```

## Additional Resources

### Configuration Templates
See configs/ directory for pre-configured templates.

### Documentation
See docs/ directory for comprehensive documentation.

### API Reference
See nlm/ directory for detailed API documentation.

### Tutorials
See tutorials/ directory for step-by-step tutorials.

These examples provide a solid foundation for working with NLM.
Customize and extend them to suit your specific research needs.
"""


def get_troubleshooting_guide() -> str:
    """Get troubleshooting guide for common issues"""
    return """
NLM Troubleshooting Guide
===========================

## Common Issues and Solutions

### Configuration Errors

**Issue**: Configuration file not found or invalid format
**Solution**: 
1. Use `nlm-config validate <file>` to check configuration
2. Create configuration using `nlm-config create <name>`
3. Check that configuration file is valid JSON or INI format

**Example**:
```bash
# Create configuration template
nlm-config create my_config

# Validate configuration
nlm-config validate configs/my_config.json
```

**Issue**: Missing required configuration fields
**Solution**:
1. Check configuration documentation for required fields
2. Ensure all numeric values are properly formatted
3. Verify connection_probability is between 0.0 and 1.0

**Example**:
```python
# Check required fields
required_fields = ['neuron_count', 'region_count', 'connection_probability']
```

### Simulation Errors

**Issue**: Simulation runs too slowly or hangs
**Solution**:
1. Reduce configuration size (neurons, regions, steps)
2. Check for infinite loops in simulation logic
3. Verify hardware resources are sufficient
4. Use checkpointing for long simulations

**Example**:
```python
# Start with small configuration
simulation = BrainSimulation(create_small_config())
simulation.initialize()
results = simulation.run_simulation(100)  # Fewer steps
```

**Issue**: High memory usage
**Solution**:
1. Reduce simulation complexity
2. Use smaller configuration
3. Enable checkpointing to reduce memory footprint
4. Monitor system memory usage

**Example**:
```python
# Use memory-efficient configuration
config = create_small_config()
simulation = BrainSimulation(config)
```

### Brain Initialization Errors

**Issue**: Brain fails to initialize
**Solution**:
1. Check configuration values are within valid ranges
2. Verify random seed is valid
3. Ensure all brain systems can be initialized
4. Try resetting and reinitializing

**Example**:
```python
from nlm.error_handling import BrainError

try:
    simulation = BrainSimulation()
    simulation.initialize()
except BrainError as e:
    print(f"Brain initialization failed: {e}")
    print("Suggestions:", e.suggestions)
```

### Memory System Errors

**Issue**: Memory systems run out of capacity
**Solution**:
1. Increase memory system capacity in configuration
2. Check for memory leaks in simulation
3. Try reducing simulation duration
4. Clear memory systems and restart

**Example**:
```python
# Check memory capacity
if simulation.brain.getWorkingMemory():
    print(f"Working memory capacity: {simulation.brain.getWorkingMemory().getCapacity()}")
```

### Experiment Errors

**Issue**: Experiment runs for too long or fails
**Solution**:
1. Reduce number of trials or steps per trial
2. Check experiment implementation for errors
3. Verify experiment requirements are met
4. Use simpler experiment for testing

**Example**:
```python
# Start with simpler experiment
experiment_manager.run_experiment(
    brain, "working_memory",
    trials=2, steps_per_trial=50
)
```

### Visualization Errors

**Issue**: Visualization fails or produces empty plots
**Solution**:
1. Check that visualization libraries are installed
2. Verify data is available for visualization
3. Check plotting code for errors
4. Use fallback visualization methods

**Example**:
```python
try:
    import matplotlib.pyplot as plt
    # Create visualization
    plt.plot([1, 2, 3], [1, 2, 3])
    plt.savefig('plot.png')
except ImportError:
    print("Matplotlib not available. Install with: pip install matplotlib")
```

### Python API Errors

**Issue**: Python API not available or functions not found
**Solution**:
1. Ensure NLM Python bindings are properly installed
2. Check that required modules are imported
3. Verify Python package is correctly structured
4. Check for syntax errors in your code

**Example**:
```python
# Check imports
from nlm import BrainSimulation, ExperimentManager
try:
    simulation = BrainSimulation()
    print("Import successful")
except ImportError as e:
    print(f"Import error: {e}")
```

### Performance Issues

**Issue**: Simulation performance is poor
**Solution**:
1. Profile simulation to identify bottlenecks
2. Optimize configuration parameters
3. Check for inefficient code patterns
4. Use smaller configuration for testing

**Example**:
```python
import time

start_time = time.time()
simulation.run_simulation(1000)
end_time = time.time()

print(f"Simulation took {end_time - start_time:.2f} seconds")
```

### File I/O Errors

**Issue**: Cannot save or load files
**Solution**:
1. Check file permissions
2. Verify file paths are correct
3. Ensure disk space is available
4. Check file format compatibility

**Example**:
```python
# Check file permissions and paths
try:
    simulation.save_simulation("simulation.checkpoint")
    print("Saved successfully")
except Exception as e:
    print(f"Save failed: {e}")
    # Check file permissions and disk space
```

## Advanced Troubleshooting

### Debug Simulation Issues

**Step 1**: Reproduce the Issue
1. Create a minimal reproduction case
2. Document all steps to reproduce
3. Keep configuration as simple as possible

**Step 2**: Check Error Messages
1. Read all error messages carefully
2. Note exact error codes and descriptions
3. Check error logs for detailed information

**Step 3**: Use Error Context
```python
from nlm.error_handling import create_error_context

with create_error_context("SIMULATION_ERROR", recovery_strategy="recover"):
    # Your simulation code here
    simulation.run_simulation(1000)
```

**Step 4**: Check System Resources
```bash
# Check system resources
free -h          # Memory
nvidia-smi       # GPU (if applicable)
```

**Step 5**: Profile Simulation
```python
import cProfile
import pstats

cProfile.run('simulation.run_simulation(1000)', 'simulation_profile')
stats = pstats.Stats('simulation_profile')
stats.sort_stats('cumulative').print_stats(20)
```

### Validate Configuration

**Step 1**: Check Required Fields
```python
def validate_config(config):
    required = ['neuron_count', 'region_count', 'connection_probability']
    for field in required:
        if field not in config:
            raise ValueError(f"Missing required field: {field}")
```

**Step 2**: Check Field Types
```python
def check_field_types(config):
    if not isinstance(config['neuron_count'], int):
        raise ValueError("neuron_count must be an integer")
    if not (0.0 <= config['connection_probability'] <= 1.0):
        raise ValueError("connection_probability must be between 0.0 and 1.0")
```

**Step 3**: Use Validation Tools
```bash
# Use NLM validation tools
nlm-config validate configs/your_config.json
```

### Monitor Simulation Progress

**Step 1**: Set Up Logging
```python
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
```

**Step 2**: Add Progress Indicators
```python
def run_simulation_with_progress(simulation, steps):
    for step in range(steps):
        simulation.step(step)
        if step % 100 == 0:
            logger.info(f"Step {step}/{steps}")
```

**Step 3**: Monitor Resource Usage
```python
import psutil
import time

process = psutil.Process()
for step in range(steps):
    simulation.step(step)
    
    # Check memory usage
    memory_info = process.memory_info()
    if memory_info.rss > MAX_MEMORY:
        logger.warning(f"High memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
```

### Recovery Strategies

**Step 1**: Use Built-in Recovery
```python
from nlm.error_handling import handle_error, ConfigurationError

try:
    # Your simulation code here
    simulation.run_simulation(1000)
except Exception as e:
    # Attempt recovery
    recovered_error = handle_error(
        e, "SIMULATION_ERROR", 
        recovery_strategy="recover"
    )
    if recovered_error:
        print("Recovery attempted. Retrying...")
        # Retry simulation
```

**Step 2**: Fall Back to Simpler Configuration
```python
# If simulation fails, try with simpler configuration
from nlm import create_small_config

try:
    simulation = BrainSimulation(create_small_config())
    simulation.initialize()
    simulation.run_simulation(100)
except Exception as e:
    print("Failed with small config. Check system resources.")
```

**Step 3**: Use Checkpoints
```python
# Enable checkpointing for long simulations
results = simulation.run_simulation(5000, enable_checkpoints=True)
```

## Getting Help

### When to Get Help

**Contact Support If**:
- You've tried all troubleshooting steps
- The issue is not documented
- You need specialized assistance
- You have time-sensitive requirements

**Self-Help First**:
- Check documentation and examples
- Use error messages and suggestions
- Try simpler configurations
- Check system resources

### How to Get Help

**Online Resources**:
1. **Documentation**: https://nlm.readthedocs.io
2. **GitHub Issues**: https://github.com/nlm-project/nlm/issues
3. **Community Forum**: https://community.nlm-project.org
4. **GitHub Repository**: https://github.com/nlm-project/nlm

**Email Support**:
```
<support@nlm-project.org>

Include:
- Error messages and stack traces
- Configuration files
- Steps to reproduce
- System information
```

**Issue Template**:
```
## Problem
[Describe the problem]

## Steps to Reproduce
1. [Step 1]
2. [Step 2]
3. [Step 3]

## Error Messages
[Copy error messages]

## Configuration
[Include configuration file]

## System Information
- OS: [Operating System]
- Python: [Python version]
- NLM: [NLM version]
- Hardware: [Hardware specifications]

## Expected Behavior
[Describe expected behavior]

## Actual Behavior
[Describe actual behavior]
```

## Next Steps

1. **Try the troubleshooting guide above**
2. **Check for existing solutions**
3. **Reproduce the issue with minimal configuration**
4. **Document all steps and observations**
5. **Contact support if issue persists**

Remember: Most NLM issues can be resolved by:
- Using appropriate configuration sizes
- Checking error messages and suggestions
- Validating configurations
- Using built-in recovery mechanisms
- Following best practices

Good luck with your NLM neural simulation!
"""


def main():
    """Display all documentation sections"""
    print("NLM (Neural Learning Machine) - Enhanced Documentation")
    print("=" * 60)
    print()
    
    # Display documentation sections
    sections = [
        ("Quick Start Guide", get_quick_start_guide()),
        ("API Reference", get_api_reference()),
        ("Best Practices", get_best_practices()),
        ("Examples Index", get_examples_index()),
        ("Troubleshooting Guide", get_troubleshooting_guide()),
    ]
    
    for title, content in sections:
        section = DocumentationSection(title, content)
        section.display()
    
    print("Documentation display complete!")
    print("For more information, see the individual documentation files.")


if __name__ == "__main__":
    main()
