# NLM - Neural Learning Machine

## Overview

NLM (熙然) is an experimental artificial developmental brain project that investigates computational brain-like systems. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**NLM is NOT:**
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

**NLM IS intended to become:**
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start Guide

### Installation

```bash
# Clone the repository
https://github.com/nlm-project/nlm.git

# Navigate to the project directory
cd nlm

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# The Python bindings are built via pyproject.toml
# If using Python, ensure you have pybind11 and scikit-build-core installed
```

### Basic Usage

```python
# Import NLM modules
from nlm import Config, ConfigSource, Brain
from nlm_enhanced import create_brain_with_config, run_learning_study

# Create a standard brain configuration
brain = create_brain_with_config({
    'neuron_count': 500,
    'region_count': 1,
    'connection_prob': 0.15
})

# Initialize the brain
brain.initialize()

# Run a learning experiment
results = run_learning_study(
    brain,
    scenario='simple_associative',
    steps=1000
)

# Analyze the results
from nlm_enhanced import analyze_network
analysis = analyze_network(brain)
```

### Command Line Interface

```bash
# Create a new brain
nlm create --neuron-count 500 --region-count 1 --save-config config.json

# Load an existing configuration
nlm load --file config.json

# Run a learning experiment
nlm experiment --scenario simple_associative --steps 1000 --output results.json

# Analyze the brain state
nlm analyze --output analysis.json

# Create a research scenario
nlm research --scenario-type plasticity_analysis

# Run comprehensive tests
nlm comprehensive --output test_results.json
```

## NLM Architecture

### Phase 6: Final Integration

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

#### Integrated Systems:

**Memory Systems**
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

**Neuromodulation Integration**
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

**Prediction System**
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

**Cognition Systems**
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

**Development Integration**
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

**Persistence**
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

**Replay and Consolidation**
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Core Components

### C++ Core Library

**Directory:** `src/`

**Main Components:**
- `core/`: Core utilities (configuration, random number generation, logging)
- `brain/`: Neural components (neurons, synapses, regions, plasticity rules)
- `dynamics/`: Neural dynamics (LIF neurons, spike systems)
- `plasticity/`: Plasticity rules (STDP, Hebbian, structural)
- `development/`: Developmental system
- `neuromodulation/`: Neuromodulators (dopamine, curiosity, novelty)
- `memory/`: Memory systems (working, episodic, associative)
- `prediction/`: Prediction systems
- `cognition/`: Cognitive mechanisms
- `sensory/`: Sensory processing
- `motor/`: Motor system
- `environment/`: Environment interface
- `experiments/`: Experiment framework
- `visualization/`: Visualization

### Python Bindings

**Directory:** `python/`

**Core Classes:**
- `Config`: Configuration management
- `Brain`: Neural simulation brain
- `SimpleWorld`: Environment interface
- `AgentBrain`: Agent brain interface
- `SensoryInput`: Base sensory input
- `Vision`: Vision sensory input
- `Audio`: Audio sensory input
- `InternalSignals`: Internal signals
- `Action`: Motor action representation
- `WorldObject`: World object representation
- `AgentBody`: Agent body state
- `ActionResult`: Action result from world
- `SensoryPercept`: Sensory percept data

### Enhanced Python API

**File:** `EnhancedPythonAPI.py`

**Key Features:**
- High-level factory functions for common scenarios
- Advanced error handling and validation
- Comprehensive logging and monitoring
- Visualization and analysis tools
- Research scenario generators
- Performance monitoring and profiling
- Configuration management utilities

## Examples

### Example 1: Basic Learning Experiment

```python
"""
This example demonstrates a simple associative learning experiment.
The brain learns to associate patterns through Hebbian plasticity.
"""

from nlm_enhanced import create_brain_with_config, run_learning_study, analyze_network
from nlm_enhanced import create_visualization_data, export_results_safely

def basic_learning_example():
    # Create a learning-ready brain
    brain = create_brain_with_config({
        'neuron_count': 200,
        'region_count': 1,
        'connection_prob': 0.1,
        'hebbian_rate': 0.05
    })
    
    # Run a learning experiment
    results = run_learning_study(
        brain,
        scenario='simple_associative',
        steps=1000
    )
    
    # Analyze results
    analysis = analyze_network(brain)
    
    # Create visualization data
    visualization = create_visualization_data(brain)
    
    # Export everything
    export_results_safely(results, 'basic_learning_results.json')
    export_results_safely(analysis, 'basic_learning_analysis.json')
    export_results_safely(visualization, 'basic_learning_visualization.json')
    
    print("Basic learning example completed successfully!")

if __name__ == "__main__":
    basic_learning_example()
```

### Example 2: Advanced Research Scenario

```python
"""
This example demonstrates an advanced research scenario focusing on
neuromodulation and learning.
"""

from nlm_enhanced import NLMEnhancedAPI, create_research_workspace, run_research_scenario

def neuromodulation_research_example():
    # Initialize the enhanced API
    api = NLMEnhancedAPI()
    
    # Create a research workspace
    workspace = create_research_workspace('neuromodulation_effects')
    
    # Create brain with research configuration
    config = workspace['configurations']['brain']
    
    # Configure research parameters
    research_params = {
        'dopamine_sensitivity': 0.8,
        'curiosity_rate': 0.05,
        'novelty_sensitivity': 0.6,
        'prediction_error_weight': 0.7
    }
    
    # Create research scenario
    research_results = run_research_scenario(
        'neuromodulation_effects',
        **research_params
    )
    
    # Run experiment (simplified)
    # In practice, you would create a Brain instance with the config
    # and run the research scenario
    
    print("Neuromodulation research example setup complete!")
    
    return workspace, research_results

if __name__ == "__main__":
    workspace, results = neuromodulation_research_example()
```

### Example 3: Interactive Command Line

```python
"""
Example of using the NLM command line interface for interactive research.
"""

import subprocess
import json
from datetime import datetime

def demonstrate_cli_usage():
    print("=== NLM Command Line Interface Demo ===\n")
    
    # Example 1: Create a brain
    print("1. Creating brain configuration...")
    create_cmd = [
        'python', 'nlm_cli.py', 'create',
        '--neuron-count', '500',
        '--region-count', '1',
        '--connection-prob', '0.15',
        '--save-config', 'demo_config.json'
    ]
    
    # In practice, you would run this command
    # For demo purposes, we'll show what it would do
    print(f"   Command: {' '.join(create_cmd)}")
    print("   Result: Brain created with 500 neurons, 1 region\n")
    
    # Example 2: Run experiment
    print("2. Running learning experiment...")
    experiment_cmd = [
        'python', 'nlm_cli.py', 'experiment',
        '--scenario', 'simple_associative',
        '--steps', '1000',
        '--output', 'demo_results.json'
    ]
    print(f"   Command: {' '.join(experiment_cmd)}")
    print("   Result: Learning experiment completed with 1000 steps\n")
    
    # Example 3: Analyze results
    print("3. Analyzing experiment results...")
    analyze_cmd = [
        'python', 'nlm_cli.py', 'analyze',
        '--output', 'demo_analysis.json'
    ]
    print(f"   Command: {' '.join(analyze_cmd)}")
    print("   Result: Network analysis completed\n")
    
    # Example 4: Create visualization
    print("4. Creating visualization...")
    viz_cmd = [
        'python', 'nlm_cli.py', 'visualization',
        '--output', 'demo_visualization.json',
        '--html', 'demo_visualization.html'
    ]
    print(f"   Command: {' '.join(viz_cmd)}")
    print("   Result: HTML visualization generated\n")
    
    print("=== CLI Demo Complete ===")

if __name__ == "__main__":
    demonstrate_cli_usage()
```

### Example 4: Performance Analysis

```python
"""
Example demonstrating performance monitoring and analysis.
"""

from nlm_enhanced import NLMEnhancedAPI
import time

def performance_analysis_example():
    api = NLMEnhancedAPI()
    
    # Create brain with known configuration
    brain = api.create_enhanced_brain(
        neuron_count=1000,
        region_count=3,
        simulation_timestep=0.001
    )
    
    # Monitor performance during learning
    def performance_callback(progress):
        print(f"Progress: Step {progress['step']}, "
              f"Speed: {progress['steps_per_sec']:.2f} steps/sec")
    
    print("Starting performance monitoring...")
    
    # Run experiment with monitoring
    start_time = time.time()
    
    results = api.run_learning_experiment(
        brain,
        num_steps=2000,
        log_interval=500
    )
    
    end_time = time.time()
    
    # Calculate performance metrics
    total_time = end_time - start_time
    steps_per_second = 2000 / total_time
    
    print(f"\nPerformance Summary:")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Steps per second: {steps_per_second:.2f}")
    print(f"  Performance: {'Good' if steps_per_second > 100 else 'Average' if steps_per_second > 50 else 'Needs improvement'}")
    
    # Analyze performance bottlenecks
    analysis = api.analyze_network_connectivity(brain)
    
    if analysis['network_health']['connectivity_score'] < 0.5:
        print("\nRecommendations:")
        print("  • Consider reducing neuron count for better performance")
        print("  • Optimize plasticity parameters")
        print("  • Implement synaptic sparsity if needed")
    
    return results, analysis

if __name__ == "__main__":
    results, analysis = performance_analysis_example()
```

## API Reference

### Core NLM Classes

#### Config
```python
class Config:
    """Configuration management for NLM."""
    
    def __init__(self):
        """Create a new configuration object."""
    
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from JSON file."""
    
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to JSON file."""
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource::Runtime):
        """Set configuration value."""
    
    def get(self, key: str) -> Optional[Any]:
        """Get configuration value."""
    
    def getOr(self, key: str, default: Any) -> Any:
        """Get configuration value with default."""
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys."""
    
    def clear(self):
        """Clear all configuration entries."""
    
    def summary(self) -> str:
        """Get configuration summary."""
```

#### Brain
```python
class Brain:
    """Central neural simulation brain."""
    
    def __init__(self, config: Config):
        """Create brain with configuration."""
    
    def initialize(self) -> bool:
        """Initialize brain with configuration."""
    
    def step(self, step: SimulationStep, time: Timestamp = 0.0):
        """Perform simulation step."""
    
    def receiveSensoryInput(self, input: SensoryInput):
        """Inject sensory input into brain."""
    
    def produceAction(self) -> Action:
        """Produce motor action based on neural activity."""
    
    # Additional methods for accessing brain state and statistics
```

### Enhanced Python API

#### NLMEnhancedAPI Class
```python
class NLMEnhancedAPI:
    """Enhanced API for NLM with high-level convenience functions."""
    
    def create_enhanced_brain(self, **kwargs) -> Brain:
        """Create brain with standard configuration."""
    
    def create_learning_scenario(self, scenario_type: str, **kwargs) -> Config:
        """Create configuration for specific learning scenario."""
    
    def run_learning_experiment(self, brain: Brain, **kwargs) -> Dict[str, Any]:
        """Run learning experiment with progress tracking."""
    
    def analyze_network_connectivity(self, brain: Brain) -> Dict[str, Any]:
        """Analyze and return network connectivity statistics."""
    
    def create_visualization_data(self, brain: Brain) -> Dict[str, Any]:
        """Create visualization data for network graphs."""
    
    def run_research_scenario(self, brain: Brain, research_type: str, **kwargs) -> Dict[str, Any]:
        """Run research scenario with specific parameters."""
```

## Research Scenarios

### 1. Plasticity Analysis
```python
"""
Research scenario for studying synaptic plasticity mechanisms.
Investigates how different plasticity rules affect learning and adaptation.
"""

parameters = {
    'neuron_count': 1000,
    'connection_probability': 0.15,
    'plasticity_rules': ['STDP', 'Hebbian', 'Structural'],
    'activity_dependency': True
}

# Create research workspace
workspace = create_research_workspace('plasticity_analysis')
```

### 2. Memory Integration
```python
"""
Research scenario for investigating memory system integration.
Studies how working memory, episodic memory, and associative memory interact.
"""

parameters = {
    'working_memory_capacity': 100,
    'episodic_memory_capacity': 1000,
    'associative_patterns': 50,
    'replay_frequency': 0.1
}

# Create research workspace
workspace = create_research_workspace('memory_integration')
```

### 3. Neuromodulation Effects
```python
"""
Research scenario for studying neuromodulator effects on learning.
Investigates how dopamine, curiosity, and novelty affect behavior.
"""

parameters = {
    'dopamine_sensitivity': 0.8,
    'curiosity_rate': 0.05,
    'novelty_sensitivity': 0.6,
    'prediction_error_weight': 0.7
}

# Create research workspace
workspace = create_research_workspace('neuromodulation_effects')
```

## Configuration Files

### Default Configuration
```json
{
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "hebbian_learning_rate": 0.01,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

### Research Configuration Templates

#### High-Resolution Study
```json
{
    "neuron_count": 2000,
    "region_count": 5,
    "connection_probability": 0.1,
    "simulation_timestep": 0.0005,
    "random_seed": 123,
    "working_memory_capacity": 200,
    "episodic_memory_capacity": 2000,
    "associative_memory_pattern_size": 100
}
```

#### Real-Time Learning
```json
{
    "neuron_count": 1000,
    "region_count": 3,
    "connection_probability": 0.2,
    "simulation_timestep": 0.001,
    "random_seed": 456,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.02,
    "stdp_tau": 30.0,
    "dopamine_sensitivity": 0.9,
    "curiosity_rate": 0.08,
    "novelty_sensitivity": 0.7
}
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Import Errors
**Issue:** "ImportError: cannot import name 'NLM_AVAILABLE'"

**Solution:** Ensure all required dependencies are installed:
```bash
pip install pybind11 scikit-build-core numpy matplotlib
```

#### 2. Memory Errors
**Issue:** "MemoryError: Unable to allocate enough memory"

**Solution:** Reduce network size or increase system memory:
```python
# Use smaller network
brain = create_enhanced_brain(neuron_count=200, region_count=2)
```

#### 3. Configuration Errors
**Issue:** "ValueError: Invalid configuration parameters"

**Solution:** Use valid configuration parameters:
```python
brain = create_enhanced_brain(
    neuron_count=500,
    region_count=1,
    connection_prob=0.15,  # Must be between 0 and 1
    timestep=0.001  # Must be positive
)
```

#### 4. Performance Issues
**Issue:** "Experiment took too long to complete"

**Solution:** Optimize network or reduce simulation steps:
```python
results = run_learning_study(brain, scenario='simple_associative', steps=500)
```

### Getting Help

#### Interactive Help
```bash
# Show general help
nlm --help

# Show help for specific command
nlm help create
nlm help experiment
nlm help analyze
```

#### Online Resources
- **Documentation:** https://nlm.readthedocs.io
- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Issue Tracker:** https://github.com/nlm-project/nlm/issues
- **Discussions:** https://github.com/nlm-project/nlm/discussions

#### Support
For technical support, please:
1. Check the documentation for common issues
2. Search the issue tracker for existing problems
3. Create a new issue with detailed error messages
4. Include configuration and log files when reporting bugs

## Legal and License

This project is licensed under the MIT License. Please see the `LICENSE` file for details.

## Authors

NLM is developed by the NLM Research Team. For scientific background, please refer to the `docs/` directory.

## Acknowledgments

- The NLM project builds on extensive research in computational neuroscience
- Special thanks to all contributors and researchers in the field
- This project is part of the broader effort to understand and model brain-like systems

## Version Information

Current version: 0.1.0
Last updated: 2026-10-09

For the latest updates and changes, please refer to the `CHANGELOG.md` file (if available) or the GitHub repository.