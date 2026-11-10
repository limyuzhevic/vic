"""
NLM (Neural Learning Machine) - Comprehensive Example Suite

This module provides a collection of practical examples demonstrating
NLM's capabilities, from basic usage to advanced features.
"""

import pynlm
import time
import json
from pathlib import Path
from typing import Dict, Any, List

EXAMPLES_DIR = Path(__file__).parent


def get_example_examples() -> List[Dict[str, Any]]:
    """
    Get list of available examples with metadata.
    
    Returns:
        List of example dictionaries with metadata
    """
    examples = [
        {
            "name": "basic_brain",
            "title": "Basic Brain Creation",
            "difficulty": "beginner",
            "category": "getting_started",
            "description": "Create and initialize a simple brain",
            "tags": ["brain", "creation", "initialization", "basic"],
            "estimated_time": "5 minutes",
            "code_file": "examples/01_basic_brain.py"
        },
        {
            "name": "agent_world",
            "title": "Agent-World Simulation",
            "difficulty": "intermediate",
            "category": "interaction",
            "description": "Run an agent that interacts with a world",
            "tags": ["agent", "world", "simulation", "interaction"],
            "estimated_time": "10 minutes",
            "code_file": "examples/02_agent_world.py"
        },
        {
            "name": "developmental_learning",
            "title": "Developmental Learning",
            "difficulty": "intermediate",
            "category": "learning",
            "description": "Demonstrate brain development over time",
            "tags": ["development", "learning", "plasticity", "growth"],
            "estimated_time": "15 minutes",
            "code_file": "examples/03_developmental_learning.py"
        },
        {
            "name": "plasticity_experiment",
            "title": "Plasticity Experiment",
            "difficulty": "advanced",
            "category": "learning",
            "description": "Investigate synaptic plasticity and learning",
            "tags": ["plasticity", "STDP", "Hebbian", "learning"],
            "estimated_time": "20 minutes",
            "code_file": "examples/04_plasticity_experiment.py"
        },
        {
            "name": "memory_system",
            "title": "Memory System",
            "difficulty": "advanced",
            "category": "memory",
            "description": "Explore NLM memory capabilities",
            "tags": ["memory", "working_memory", "episodic_memory", "consolidation"],
            "estimated_time": "25 minutes",
            "code_file": "examples/05_memory_system.py"
        },
        {
            "name": "neuromodulation",
            "title": "Neuromodulation",
            "difficulty": "advanced",
            "category": "modulation",
            "description": "Study neuromodulation effects on learning",
            "tags": ["dopamine", "curiosity", "novelty", "motivation"],
            "estimated_time": "20 minutes",
            "code_file": "examples/06_neuromodulation.py"
        },
        {
            "name": "prediction_system",
            "title": "Prediction System",
            "difficulty": "advanced",
            "category": "cognition",
            "description": "Explore predictive coding and error signals",
            "tags": ["prediction", "prediction_error", "cognition", "forward_models"],
            "estimated_time": "20 minutes",
            "code_file": "examples/07_prediction_system.py"
        },
        {
            "name": "benchmarking",
            "title": "Performance Benchmarking",
            "difficulty": "intermediate",
            "category": "performance",
            "description": "Benchmark NLM performance and scalability",
            "tags": ["benchmark", "performance", "scaling", "optimization"],
            "estimated_time": "10 minutes",
            "code_file": "examples/08_benchmarking.py"
        },
        {
            "name": "checkpointing",
            "title": "Checkpointing and Persistence",
            "difficulty": "intermediate",
            "category": "persistence",
            "description": "Demonstrate brain state checkpointing",
            "tags": ["checkpoint", "save", "load", "persistence"],
            "estimated_time": "15 minutes",
            "code_file": "examples/09_checkpointing.py"
        },
        {
            "name": "multi_agent",
            "title": "Multi-Agent Environment",
            "difficulty": "advanced",
            "category": "multi_agent",
            "description": "Create multi-agent learning environments",
            "tags": ["multi_agent", "social_learning", "coordination"],
            "estimated_time": "30 minutes",
            "code_file": "examples/10_multi_agent.py"
        }
    ]
    
    return examples


def get_example_categories() -> List[str]:
    """
    Get list of available example categories.
    
    Returns:
        List of category names
    """
    return [
        "getting_started",
        "interaction",
        "learning",
        "memory",
        "modulation",
        "cognition",
        "performance",
        "persistence",
        "multi_agent"
    ]


def get_category_examples(category: str) -> List[Dict[str, Any]]:
    """
    Get examples for a specific category.
    
    Args:
        category: Category name
        
    Returns:
        List of example dictionaries
    """
    all_examples = get_example_examples()
    return [ex for ex in all_examples if ex['category'] == category]


def get_difficulty_examples(difficulty: str) -> List[Dict[str, Any]]:
    """
    Get examples for a specific difficulty level.
    
    Args:
        difficulty: Difficulty level (beginner, intermediate, advanced)
        
    Returns:
        List of example dictionaries
    """
    all_examples = get_example_examples()
    return [ex for ex in all_examples if ex['difficulty'] == difficulty]


def create_example_directory_structure():
    """
    Create the complete example directory structure.
    
    Creates directories for:
    - examples/: Main examples directory
    - examples/tutorials/: Beginner and intermediate examples
    - examples/advanced/: Advanced examples
    - examples/research/: Research-grade examples
    """
    # Create directories
    examples_dir = EXAMPLES_DIR / "examples"
    tutorials_dir = examples_dir / "tutorials"
    advanced_dir = examples_dir / "advanced"
    research_dir = examples_dir / "research"
    benchmarks_dir = examples_dir / "benchmarks"
    
    # Create all directories
    examples_dir.mkdir(exist_ok=True)
    tutorials_dir.mkdir(exist_ok=True)
    advanced_dir.mkdir(exist_ok=True)
    research_dir.mkdir(exist_ok=True)
    benchmarks_dir.mkdir(exist_ok=True)
    
    # Create README files
    (examples_dir / "README.md").write_text(_get_examples_readme())
    (tutorials_dir / "README.md").write_text(_get_tutorials_readme())
    (advanced_dir / "README.md").write_text(_get_advanced_readme())
    (research_dir / "README.md").write_text(_get_research_readme())
    (benchmarks_dir / "README.md").write_text(_get_benchmarks_readme())


def _get_examples_readme() -> str:
    """Get examples directory README."""
    return """# NLM Examples Directory

This directory contains a comprehensive collection of examples demonstrating
the capabilities of the NLM (Neural Learning Machine) Python API.

## Overview

The examples are organized by:

1. **Getting Started** - Basic concepts and simple usage
2. **Interaction** - Agent-world interaction and communication
3. **Learning** - Plasticity and learning mechanisms
4. **Memory** - Memory systems and consolidation
5. **Modulation** - Neuromodulation and motivation
6. **Cognition** - Cognitive processes and prediction
7. **Performance** - Benchmarking and optimization
8. **Persistence** - Checkpointing and state management
9. **Multi-Agent** - Social learning and coordination

## Usage

### Running Examples

All examples can be run directly using Python:

```bash
python examples/01_basic_brain.py
python examples/02_agent_world.py
```

Or use the NLM examples runner:

```bash
python -c "from examples import run_example; run_example('basic_brain')"
```

### Example Categories

Each category contains examples for different skill levels:

- **Beginner (tutorials/)**: Basic concepts and fundamental operations
- **Intermediate**: More complex scenarios and multi-component systems
- **Advanced**: Research-grade demonstrations with advanced features
- **Research**: Production-quality examples with comprehensive analysis

## Structure

Each example includes:

- **Code**: Python implementation
- **Explanation**: Detailed comments and documentation
- **Results**: Output and analysis
- **Extensions**: Ideas for further exploration

## Quick Start

1. **Basic Brain**: `python examples/01_basic_brain.py`
2. **Agent Simulation**: `python examples/02_agent_world.py`
3. **Development**: `python examples/03_developmental_learning.py`

## Advanced Usage

For more complex examples:

1. **Plasticity**: `python examples/04_plasticity_experiment.py`
2. **Memory**: `python examples/05_memory_system.py`
3. **Neuromodulation**: `python examples/06_neuromodulation.py`

## Contributing

Add your own examples by:

1. Creating a new file in the appropriate category directory
2. Following the existing template structure
3. Including comprehensive documentation and comments
4. Adding performance metrics and analysis
"""


def _get_tutorials_readme() -> str:
    """Get tutorials directory README."""
    return """# NLM Tutorials Directory

This directory contains beginner and intermediate examples designed
to help you learn NLM's core functionality.

## Skill Levels

### Beginner Examples

1. **Basic Brain Creation** (`01_basic_brain.py`)
   - Learn how to create and initialize a brain
   - Understand basic brain properties and statistics
   - Run simple simulation steps

2. **Agent-World Simulation** (`02_agent_world.py`)
   - Discover how agents interact with environments
   - Explore sensory processing and motor output
   - Understand the complete agent loop

3. **Developmental Learning** (`03_developmental_learning.py`)
   - Study how brains develop over time
   - Explore age-dependent plasticity
   - Understand developmental stages

### Intermediate Examples

4. **Checkpointing and Persistence** (`09_checkpointing.py`)
   - Learn brain state checkpointing
   - Understand save/load functionality
   - Explore memory persistence

5. **Performance Benchmarking** (`08_benchmarking.py`)
   - Measure NLM performance
   - Compare different configurations
   - Understand scalability

## Learning Path

### Phase 1: Foundation

1. Start with `01_basic_brain.py` to understand brain creation
2. Move to `02_agent_world.py` to learn agent-environment interaction
3. Try `03_developmental_learning.py` to explore development

### Phase 2: Application

1. Study `09_checkpointing.py` for production usage
2. Explore `08_benchmarking.py` for performance optimization
3. Understand `06_neuromodulation.py` for advanced features

## Running Tutorials

### Direct Execution

```bash
cd examples/tutorials
python 01_basic_brain.py
python 02_agent_world.py
```

### Using NLM Examples

```python
from examples import run_example, get_example_list

# Get all tutorial examples
tutorials = get_example_list('tutorials')

# Run a specific example
run_example('basic_brain')

# Run all examples in a category
for example in get_category_examples('getting_started'):
    run_example(example['name'])
```

## Troubleshooting

### Common Issues

1. **"Brain not doing anything"**
   - Ensure you call `brain.initialize()` before `brain.step()`
   - Check that all required features are enabled

2. **"Everything is 0"**
   - Brains need time to "warm up" - try more simulation steps
   - Ensure sensory input is connected to the brain

3. **"Performance issues"**
   - Use `08_benchmarking.py` to measure and optimize performance
   - Consider reducing brain size for testing

### Getting Help

1. Check example output for error messages
2. Review the example code for common mistakes
3. Use the NLM documentation for detailed API information
4. Join the NLM community for support

## Extensions

After completing tutorials, try extending the examples:

1. **Modify parameters**: Change brain size, learning rates, etc.
2. **Add visualization**: Create plots of simulation results
3. **Compare configurations**: Test different settings side-by-side
4. **Add logging**: Track more detailed metrics
5. **Custom environments**: Create your own world implementations

## Best Practices

1. **Always initialize**: Call `brain.initialize()` before use
2. **Use checkpoints**: Save progress regularly
3. **Monitor performance**: Use benchmarks to track improvements
4. **Validate configuration**: Check all settings before running
5. **Document results**: Record parameters and outcomes
"""


def _get_advanced_readme() -> str:
    """Get advanced examples directory README."""
    return """# NLM Advanced Examples Directory

This directory contains advanced examples demonstrating complex NLM scenarios
and research-grade implementations.

## Advanced Concepts

### 1. Complex Plasticity Dynamics

**Plasticity Experiment** (`04_plasticity_experiment.py`)

- Demonstrates STDP and Hebbian learning interactions
- Explores structural plasticity and synapse formation
- Analyzes learning curves and weight dynamics

### 2. Advanced Memory Systems

**Memory System** (`05_memory_system.py`)

- Implements working memory and episodic memory
- Demonstrates memory consolidation and retrieval
- Explores memory decay and interference

### 3. Neuromodulation

**Neuromodulation** (`06_neuromodulation.py`)

- Studies dopamine, curiosity, and novelty effects
- Explores motivation-driven learning
- Analyzes reward prediction and error signals

### 4. Cognitive Processes

**Prediction System** (`07_prediction_system.py`)

- Implements forward models and prediction errors
- Explores expectation formation and surprise
- Analyzes planning and decision-making

### 5. Multi-Agent Systems

**Multi-Agent Environment** (`10_multi_agent.py`)

- Creates competitive and cooperative learning scenarios
- Studies social learning and observation
- Explores coordination and communication

## Research-Grade Examples

### Performance Analysis

- Detailed benchmarking and performance profiling
- Scalability analysis across different brain sizes
- Memory usage and computational efficiency

### Experimental Paradigms

- Reinforcement learning scenarios
- Social learning environments
- Developmental trajectories
- Memory consolidation experiments

## Running Advanced Examples

### Direct Execution

```bash
cd examples/advanced
python 04_plasticity_experiment.py
python 05_memory_system.py
```

### Using NLM Examples

```python
from examples import run_example, get_example_list

# Get advanced examples
advanced_examples = get_example_list('advanced')

# Run specific advanced example
run_example('plasticity_experiment')

# Run all advanced examples
for example in get_difficulty_examples('advanced'):
    run_example(example['name'])
```

## Technical Complexity

### Computational Requirements

- **Memory**: 1-10 GB depending on brain size
- **CPU**: Single core for basic examples, multi-core for research
- **Time**: 5 minutes for simple, 30+ minutes for complex scenarios

### Dependencies

Advanced examples may require additional packages:

```bash
pip install matplotlib pandas scipy seaborn
```

## Performance Optimization

### Tips for Advanced Usage

1. **Brain Size Optimization**
   - Start with smaller brains for debugging
   - Scale up gradually for performance testing
   - Use `08_benchmarking.py` to find optimal configurations

2. **Memory Management**
   - Monitor memory usage with benchmarks
   - Use checkpointing for long simulations
   - Consider batch processing for large datasets

3. **Parallel Processing**
   - Multiple independent simulations
   - Parameter sweeps and optimization
   - Monte Carlo methods for robust results

## Extension Ideas

### Research Extensions

1. **Novel Learning Rules**
   - Custom plasticity mechanisms
   - Alternative learning algorithms
   - Meta-learning and adaptation

2. **Complex Environments**
   - Multi-agent social dynamics
   - Dynamic world models
   - Evolutionary pressures

3. **Analysis Tools**
   - Statistical analysis of results
   - Visualization of neural activity
   - Network analysis and graph theory

### Implementation Extensions

1. **New Modules**
   - Custom sensory modalities
   - Advanced motor control
   - Specialized learning rules

2. **Integration**
   - Connection to real-world data
   - Integration with other AI systems
   - Cross-platform compatibility

## Quality Assurance

### Code Quality

1. **Testing**
   - Unit tests for all functions
   - Integration tests for system behavior
   - Performance benchmarks and validation

2. **Documentation**
   - Comprehensive docstrings
   - Examples and tutorials
   - API reference documentation

3. **Performance**
   - Benchmarking against state-of-the-art
   - Memory usage optimization
   - Computational efficiency

## Community Contributions

### Adding New Examples

1. **Follow the Template**
   - Use existing examples as reference
   - Include comprehensive documentation
   - Add performance metrics and analysis

2. **Quality Guidelines**
   - Code follows PEP 8 standards
   - Includes type hints and documentation
   - Provides clear usage examples

3. **Review Process**
   - Peer review of new examples
   - Performance validation
   - Documentation review

## Future Directions

### Emerging Research Areas

1. **Neuromorphic Computing**
   - Hardware-software co-design
   - Event-driven computation
   - Energy-efficient learning

2. **Explainable AI**
   - Interpretable neural dynamics
   - Causal reasoning models
   - Transparent learning processes

3. **Transfer Learning**
   - Cross-domain knowledge transfer
   - Continual learning and adaptation
   - Meta-learning frameworks

## Conclusion

Advanced NLM examples provide a foundation for:

- Research in computational neuroscience
- Development of intelligent systems
- Understanding learning and development
- Exploration of artificial cognitive architectures

The examples demonstrate that NLM can serve as:

1. **Research Platform**: For testing hypotheses and exploring theories
2. **Development Tool**: For building and prototyping intelligent systems
3. **Educational Resource**: For teaching neural computation and learning
4. **Production System**: For deploying real-world applications
"""


def _get_research_readme() -> str:
    """Get research examples directory README."""
    return """# NLM Research Examples Directory

This directory contains research-grade examples designed for scientific
investigation, experimental validation, and reproducible research.

## Research Objectives

### 1. Experimental Validation

Validate NLM hypotheses and theoretical predictions through controlled experiments.

### 2. Comparative Analysis

Compare NLM with other neural models and artificial intelligence systems.

### 3. Parameter Exploration

Systematically explore parameter spaces to understand design principles.

### 4. Performance Benchmarking

Establish performance baselines and optimization strategies.

### 5. Reproducible Research

Create fully reproducible research experiments with detailed documentation.

## Key Research Features

### Scientific Rigor

1. **Controlled Experiments**
   - Randomized parameter variations
   - Statistical analysis and significance testing
   - Hypothesis-driven experimental design

2. **Reproducibility**
   - Fixed random seeds for reproducibility
   - Detailed parameter recording
   - Complete documentation

3. **Scalability Testing**
   - Performance across different scales
   - Resource requirements analysis
   - Efficiency measurements

### Advanced Capabilities

#### Computational Neuroscience

- **Neural Dynamics**: Spike timing, population coding, oscillatory activity
- **Synaptic Plasticity**: STDP, Hebbian learning, homeostatic regulation
- **Network Dynamics**: Synchronization, pattern formation, attractor dynamics

#### Machine Learning

- **Reinforcement Learning**: Reward prediction, exploration-exploitation
- **Unsupervised Learning**: Pattern discovery, clustering, dimensionality reduction
- **Meta-Learning**: Learning to learn, adaptation, transfer learning

#### Cognitive Modeling

- **Working Memory**: Active maintenance, interference, capacity limits
- **Episodic Memory**: Encoding, consolidation, retrieval
- **Prediction**: Forward models, expectation formation, surprise signals

#### Neuromodulation

- **Motivation**: Dopamine, curiosity, novelty detection
- **Attention**: Competitive selection, resource allocation
- **Learning**: Error-based, reward-based, curiosity-driven

## Running Research Examples

### Direct Execution

```bash
cd examples/research
python research_example_01.py
python research_example_02.py
```

### Using NLM Research Tools

```python
from examples import run_research_example, get_research_list

# Get research examples
research_examples = get_research_list()

# Run specific research example
run_research_example('neural_dynamics_01')

# Run research simulation suite
run_research_suite('neural_plasticity')
```

### Analysis Tools

Research examples include:

1. **Data Analysis**
   - Statistical analysis of simulation results
   - Visualization of neural activity
   - Model comparison and validation

2. **Parameter Optimization**
   - Grid search and random search
   - Bayesian optimization
   - Evolutionary algorithms

3. **Visualization**
   - Neural activity plots
   - Learning curves
   - Network topology visualization

## Research Example Categories

### 1. Neural Dynamics

#### Neural Coding and Representation
- **Population Codes**: Tuning curves, tuning widths, information capacity
- **Spike Timing**: Timing precision, reliability, coding efficiency
- **Oscillatory Activity**: Rhythms, synchronization, phase coding

#### Learning Dynamics
- **Plasticity Rules**: STDP, Hebbian, anti-Hebbian learning
- **Learning Curves**: Acquisition, consolidation, forgetting
- **Generalization**: Transfer across contexts, tasks, domains

### 2. Memory Systems

#### Working Memory
- **Active Maintenance**: Persistent activity, interference control
- **Capacity Limits**: Span, efficiency, chunking strategies
- **Recruitment**: Neural resource allocation and competition

#### Episodic Memory
- **Encoding**: Pattern separation and completion
- **Consolidation**: Sleep, replay, synaptic strengthening
- **Retrieval**: Cue-driven activation, pattern completion

### 3. Neuromodulation and Motivation

#### Reward Prediction
- **Dopamine Signals**: Prediction errors, reward magnitude, timing
- **Curiosity**: Novelty detection, exploration behavior
- **Motivation**: Drive, persistence, goal-directed behavior

#### Attention and Selection
- **Competitive Dynamics**: Winner-take-all, lateral inhibition
- **Resource Allocation**: Attention, memory, processing capacity
- **Adaptive Control**: Dynamic priority adjustment

### 4. Cognitive Architecture

#### Prediction and Inference
- **Forward Models**: Expectation formation, error signals
- **Bayesian Inference**: Probabilistic reasoning, uncertainty handling
- **Planning**: Goal selection, action evaluation, planning

#### Social Cognition
- **Theory of Mind**: Belief attribution, intention understanding
- **Social Learning**: Observation, imitation, cultural transmission
- **Cooperation**: Trust, reciprocity, coordination

## Research Methodology

### Experimental Design

#### Hypothesis-Driven Research

1. **Formulate Hypothesis**
   ```python
   # Example research hypothesis
   hypothesis = {
       "title": "Enhanced curiosity leads to better exploration",
       "prediction": "Agents with high curiosity will discover 30% more resources",
       "variables": ["curiosity_level", "exploration_efficiency"],
       "sample_size": 100,
       "significance_threshold": 0.05
   }
   ```

2. **Experimental Setup**
   ```python
   # Configure simulation environment
   config = {
       "agent_type": "curiosity_driven",
       "environment": "resource_limited",
       "observation_noise": 0.1,
       "reward_structure": "sparse"
   }
   ```

3. **Data Collection**
   ```python
   # Collect experimental data
   data = {
       "trials": [],
       "rewards": [],
       "curiosity_levels": [],
       "exploration_paths": []
   }
   ```

4. **Statistical Analysis**
   ```python
   # Analyze results
   stats = {
       "mean_performance": np.mean(data["rewards"]),
       "std_performance": np.std(data["rewards"]),
       "confidence_intervals": scipy.stats.t.interval(0.95, len(data["rewards"])),
       "effect_size": calculate_effect_size(data["rewards"]),
       "p_value": calculate_p_value(data["rewards"])
   }
   ```

### Parameter Exploration

#### Grid Search

```python
# Parameter space exploration
param_space = {
    "brain_size": [100, 500, 1000, 2000],
    "learning_rate": [0.001, 0.01, 0.1],
    "curiosity_factor": [0.0, 0.5, 1.0, 2.0],
    "plasticity_strength": [0.0, 0.5, 1.0]
}

# Perform grid search
results = grid_search(param_space, simulation_function)
```

#### Random Search

```python
# Random parameter sampling
import random

best_params = random_search(param_space, simulation_function, iterations=1000)
```

#### Bayesian Optimization

```python
# Use scikit-optimize for Bayesian optimization
from skopt import gp_minimize
from skopt.space import Real, Integer

dimensions = [
    Real(100, 5000, name='brain_size'),
    Real(0.001, 1.0, name='learning_rate'),
    Real(0.0, 5.0, name='curiosity_factor')
]

result = gp_minimize(objective_function, dimensions, n_calls=100)
```

## Research Validation

### Model Validation

#### Neuroscience Validation

- **Biological Plausibility**: Compare with empirical data
- **Theoretical Consistency**: Ensure mathematical consistency
- **Predictive Accuracy**: Test against known phenomena

#### Computational Validation

- **Convergence Testing**: Ensure numerical stability
- **Sensitivity Analysis**: Test robustness to parameter variations
- **Cross-Validation**: Ensure generalization to new tasks

### Benchmarking

#### Performance Benchmarking

```python
# Create comprehensive benchmark
benchmark = NLMBenchmarkSuite()
results = benchmark.run_all_benchmarks()

# Analyze results
stats = benchmark.calculate_statistics(results)

# Compare with baseline
baseline = stats["brain_creation_1000"]
comparison = compare_performance(stats, baseline)
```

#### Scaling Analysis

```python
# Analyze scaling properties
scaling_results = analyze_scaling_properties(benchmark_results)

# Determine optimal configurations
optimal_configs = find_optimal_configurations(scaling_results)
```

## Research Tools and Utilities

### Data Management

#### Data Recording
```python
from examples import ResearchDataRecorder

# Create data recorder
data_recorder = ResearchDataRecorder("experiment_01")

# Record experimental data
data_recorder.record(
    "brain_size", 1000,
    "learning_rate", 0.01,
    "performance", 0.85,
    "reward", 5.2,
    "trial_number", 42
)

# Save data
data_recorder.save("experiment_01_data.json")
```

#### Data Analysis
```python
from examples import ResearchDataAnalyzer

# Load and analyze data
analyzer = ResearchDataAnalyzer("experiment_01_data.json")

# Generate statistics
stats = analyzer.get_statistics()

# Create visualizations
visualizations = analyzer.create_plots()
visualizations.save("experiment_01_plots.png")
```

### Experiment Management

#### Experiment Tracking
```python
from examples import ExperimentTracker

# Track experiment progress
tracker = ExperimentTracker("neural_plasticity_experiment")

# Start experiment
tracker.start_experiment()

# Record intermediate results
tracker.record_result("plasticity_rate", 0.15)
tracker.record_result("learning_efficiency", 0.82)

# End experiment and save
tracker.end_experiment()
tracker.save_results("experiment_results.json")
```

## Reproducibility and Sharing

### Code Management

#### Version Control
- All examples use git for version control
- Include requirements.txt for dependencies
- Document experimental setup and procedures

#### Documentation
- Comprehensive docstrings in all code
- Jupyter notebooks for interactive exploration
- LaTeX/TeX for mathematical derivations

#### Data Sharing
- Standardized data formats (JSON, CSV)
- Public data repositories
- Metadata and provenance tracking

### Community Contribution

#### Contributing Examples

1. **Submit New Examples**
   - Create research-grade examples
   - Include comprehensive documentation
   - Provide validation and benchmarks

2. **Peer Review**
   - Review examples for scientific validity
   - Ensure code quality and documentation standards
   - Validate performance claims

3. **Integration**
   - Add examples to main examples directory
   - Update example catalog
   - Maintain example taxonomy

#### Best Practices for Research Examples

1. **Scientific Rigor**
   - Hypothesis-driven experimental design
   - Proper statistical analysis
   - Reproducible methodology

2. **Code Quality**
   - PEP 8 compliant code
   - Comprehensive documentation
   - Type hints and validation

3. **Performance**
   - Benchmarking and optimization
   - Memory usage analysis
   - Scalability testing

## Future Research Directions

### Emerging Areas

1. **Neuromorphic Computing**
   - Energy-efficient neural models
   - Event-driven computation
   - Hardware-software co-design

2. **Explainable AI**
   - Interpretable neural dynamics
   - Causal inference in learning
   - Transparent decision-making

3. **Lifelong Learning**
   - Continual learning and adaptation
   - Transfer learning across domains
   - Meta-learning and optimization

### Methodological Advances

1. **Advanced Statistics**
   - Bayesian inference methods
   - Machine learning for hypothesis testing
   - Causal discovery algorithms

2. **Experimental Design**
   - Optimal experimental design
   - Multi-objective optimization
   - Adaptive experimental procedures

3. **Computational Efficiency**
   - Distributed computing for large-scale models
   - Approximate inference methods
   - Scalable algorithms for big data

## Conclusion

The NLM research examples provide a foundation for:

1. **Scientific Discovery**: Testing hypotheses about neural computation
2. **Technology Development**: Building intelligent systems
3. **Education and Training**: Teaching advanced neuroscience concepts
4. **Innovation**: Creating new computational paradigms

The examples demonstrate that NLM can serve as:

- A **research platform** for investigating neural principles
- A **development tool** for creating intelligent systems
- A **testbed** for validating theories and algorithms
- A **community resource** for sharing scientific knowledge

## Contact and Support

For questions, suggestions, or collaborations:

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Discussion Forum**: https://github.com/nlm-project/nlm/discussions
- **Issues Tracker**: https://github.com/nlm-project/nlm/issues

### Acknowledgement

Research examples are part of the NLM (Neural Learning Machine) project,
an experimental platform for artificial developmental brain research.
This work contributes to understanding neural computation, learning, and
cognition through computational modeling and simulation.
"""


def _get_benchmarks_readme() -> str:
    """Get benchmarks directory README."""
    return """# NLM Benchmarks Directory

This directory contains performance benchmarks and testing tools for NLM (Neural Learning Machine).
Benchmarks are essential for evaluating performance, comparing different configurations, and identifying optimization opportunities.

## Purpose of Benchmarks

Benchmarks serve multiple purposes:

1. **Performance Evaluation**: Measure how fast and efficient NLM is
2. **Configuration Comparison**: Compare different brain and simulation settings
3. **Optimization Guidance**: Identify bottlenecks and improvement areas
4. **Validation Testing**: Ensure correctness and reliability
5. **Research Comparison**: Compare NLM with other neural models

## Benchmark Categories

### 1. Creation and Initialization

- **Brain Creation Speed**: Time to create and initialize brains of different sizes
- **Memory Usage**: Memory requirements for different configurations
- **Resource Efficiency**: CPU and memory efficiency metrics

### 2. Simulation Performance

- **Simulation Speed**: Steps per second for different brain sizes
- **Scalability**: How performance changes with complexity
- **Real-time Performance**: Support for real-time applications

### 3. Learning and Plasticity

- **Learning Speed**: Time to learn patterns and skills
- **Plasticity Performance**: Efficiency of synaptic changes
- **Convergence Analysis**: How quickly systems converge to stable states

### 4. Memory and Storage

- **Checkpoint Performance**: Speed of saving and loading brain states
- **Memory Footprint**: Memory usage during simulation
- **Persistence**: Long-term storage and recovery capabilities

### 5. Special Operations

- **Large-Scale Simulation**: Performance with many neurons
- **Complex Environments**: Performance in challenging scenarios
- **Multi-Agent Systems**: Performance with multiple interacting agents

## Running Benchmarks

### Basic Benchmark Suite

```bash
cd examples/benchmarks
python run_all_benchmarks.py
```

### Individual Benchmark Categories

```bash
python benchmark_creation.py
python benchmark_simulation.py
python benchmark_plasticity.py
python benchmark_memory.py
```

### Custom Benchmarks

```python
from examples import NLMBenchmarkSuite

# Create custom benchmark suite
benchmark = NLMBenchmarkSuite()

# Add custom test configurations
benchmark.add_test_configuration({
    "name": "custom_test",
    "type": "simulation",
    "parameters": {
        "neuron_count": 2000,
        "steps": 1000,
        "repeat": 5
    }
})

# Run benchmarks
results = benchmark.run_all_benchmarks()
```

### Performance Analysis

```python
from examples import PerformanceAnalyzer

# Create analyzer for benchmark results
analyzer = PerformanceAnalyzer("benchmark_results.json")

# Generate performance reports
reports = analyzer.generate_reports()

# Compare configurations
comparisons = analyzer.compare_configurations(["standard", "research"])
```

## Benchmark Output Formats

### JSON Format

Benchmarks produce detailed JSON output with:

```json
{
    "benchmarks": [
        {
            "name": "brain_creation_1000",
            "type": "creation",
            "results": [
                {
                    "iteration": 1,
                    "time": 0.125,
                    "memory": 50.2,
                    "success": true
                }
            ],
            "statistics": {
                "mean_time": 0.125,
                "std_time": 0.01,
                "min_time": 0.115,
                "max_time": 0.135
            }
        }
    ],
    "summary": {
        "total_tests": 10,
        "successful_tests": 9,
        "failed_tests": 1,
        "average_performance": 0.125
    }
}
```

### CSV Format

For easy import into other tools:

```csv
test_name,test_type,iteration,time_ms,memory_mb,success
brain_creation_1000,creation,1,125,50.2,true
brain_creation_1000,creation,2,128,51.0,true
brain_creation_1000,creation,3,123,49.8,true
brain_creation_500,creation,1,65,25.1,true
```

### Visualization Format

Benchmarks can generate plots and visualizations:

- **Performance Curves**: Speed vs. brain size
- **Learning Curves**: Performance improvement over time
- **Comparison Charts**: Side-by-side configuration comparisons
- **Statistical Plots**: Distributions and confidence intervals

## Benchmark Configuration

### Custom Test Configurations

Define your own benchmark configurations:

```python
# Brain creation benchmarks
brain_creation_config = {
    "name": "brain_creation_benchmark",
    "type": "creation",
    "parameters": {
        "neuron_counts": [100, 500, 1000, 2000],
        "repeat": 3,
        "warmup": 1
    }
}

# Simulation benchmarks
simulation_config = {
    "name": "simulation_benchmark",
    "type": "simulation",
    "parameters": {
        "neuron_counts": [500, 1000, 2000],
        "step_counts": [100, 500, 1000],
        "repeat": 2
    }
}

# Plasticity benchmarks
plasticity_config = {
    "name": "plasticity_benchmark",
    "type": "plasticity",
    "parameters": {
        "synapse_counts": [100, 500, 1000],
        "plasticity_iterations": 50,
        "repeat": 2
    }
}
```

### Benchmark Parameters

Each benchmark type supports different parameters:

#### Creation Benchmarks
- `neuron_counts`: List of neuron counts to test
- `repeat`: Number of repetitions for each configuration
- `warmup`: Number of warmup runs
- `memory_tracking`: Whether to track memory usage

#### Simulation Benchmarks
- `neuron_counts`: List of neuron counts
- `step_counts`: List of step counts
- `repeat`: Number of repetitions
- `warmup`: Number of warmup runs
- `timestep`: Simulation timestep

#### Plasticity Benchmarks
- `synapse_counts`: List of synapse counts
- `plasticity_iterations`: Number of plasticity iterations
- `repeat`: Number of repetitions
- `warmup`: Number of warmup runs
- `learning_rates`: List of learning rates to test

#### Memory Benchmarks
- `brain_sizes`: List of brain sizes
- `operation_types`: List of operation types
- `repeat`: Number of repetitions

## Benchmark Best Practices

### 1. Test Design

- **Balanced Design**: Include a range of configurations
- **Statistical Power**: Ensure enough repetitions for statistical significance
- **Control Variables**: Keep conditions constant except for tested parameters
- **Warm-up Runs**: Include warm-up to account for initialization effects

### 2. Data Collection

- **Consistent Metrics**: Use the same metrics across all tests
- **Complete Recording**: Record all relevant performance data
- **Error Handling**: Handle test failures gracefully
- **Validation**: Validate results before analysis

### 3. Analysis and Reporting

- **Statistical Analysis**: Use appropriate statistical methods
- **Error Bars**: Include measures of uncertainty
- **Comparative Analysis**: Compare against baselines
- **Visualization**: Create clear, informative plots

### 4. Interpretation

- **Contextual Interpretation**: Consider the practical significance
- **Limitations**: Acknowledge test limitations
- **Generalization**: Consider how results generalize
- **Recommendations**: Provide actionable insights

## Benchmark Tools and Utilities

### Benchmark Suite

```python
from examples import NLMBenchmarkSuite

# Create comprehensive benchmark suite
benchmark_suite = NLMBenchmarkSuite()

# Run all benchmarks
results = benchmark_suite.run_all_benchmarks()

# Analyze results
statistics = benchmark_suite.calculate_statistics(results)

# Generate reports
benchmark_suite.print_summary()
```

### Performance Analyzer

```python
from examples import PerformanceAnalyzer

# Create performance analyzer
analyzer = PerformanceAnalyzer("benchmark_results.json")

# Compare configurations
comparisons = analyzer.compare_configurations([
    "minimal", "standard", "research", "simulation"
])

# Generate performance reports
reports = analyzer.generate_reports()
```

### Results Exporter

```python
from examples import BenchmarkResultsExporter

# Create exporter
exporter = BenchmarkResultsExporter()

# Export to different formats
exporter.export_results(benchmark_suite.results, "benchmarks.json")

# Generate report
exporter.generate_report(benchmark_suite.results, "benchmark_report.html")
```

## Integration with Development Workflow

### Continuous Integration

#### GitHub Actions
```yaml
name: NLM Benchmarks

on: [push, pull_request]

jobs:
  benchmarks:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Install dependencies
      run: pip install -r requirements.txt
    - name: Run benchmarks
      run: python examples/benchmarks/run_all_benchmarks.py
    - name: Generate reports
      run: python examples/benchmarks/generate_reports.py
    - name: Upload results
      uses: actions/upload-artifact@v2
      with:
        name: benchmark-results
        path: benchmarks/
```

#### Jenkins Pipeline
```groovy
pipeline {
    agent any
    
    stages {
        stage('Checkout') {
            checkout git
        }
        
        stage('Install Dependencies') {
            sh 'pip install -r requirements.txt'
        }
        
        stage('Run Benchmarks') {
            sh 'python examples/benchmarks/run_all_benchmarks.py'
        }
        
        stage('Generate Reports') {
            sh 'python examples/benchmarks/generate_reports.py'
        }
        
        stage('Archive Results') {
            archiveArtifacts artifacts: 'benchmarks/', allowEmptyArchive: true
        }
    }
    
    post {
        always {
            cleanWs()
        }
    }
}
```

### Development Testing

#### Local Development
```bash
# Install development dependencies
pip install -r requirements-dev.txt

# Run all benchmarks locally
python examples/benchmarks/run_all_benchmarks.py

# Run specific benchmarks
python examples/benchmarks/benchmark_creation.py
python examples/benchmarks/benchmark_simulation.py
```

#### Integration Testing
```bash
# Run integration tests
pytest tests/integration/

# Run performance tests
pytest tests/performance/

# Run regression tests
pytest tests/regression/
```

## Example Benchmark Scripts

### Basic Benchmark Script

```python
#!/usr/bin/env python3
"""
Basic benchmark script for NLM.

This script runs a basic set of benchmarks to get started with NLM benchmarking.
"""

import sys
from examples import NLMBenchmarkSuite

def main():
    print("Starting NLM Basic Benchmarks")
    
    # Create benchmark suite
    benchmark = NLMBenchmarkSuite()
    
    # Run benchmarks
    results = benchmark.run_all_benchmarks()
    
    # Save results
    benchmark.save_results("basic_benchmarks.json")
    
    print("Benchmarks completed successfully!")
    print(f"Total benchmarks run: {len(results)}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
```

### Comprehensive Benchmark Script

```python
#!/usr/bin/env python3
"""
Comprehensive benchmark script for NLM.

This script runs comprehensive benchmarks for all benchmark stages.
"""

import sys
from examples import NLMBenchmarkSuite, PerformanceAnalyzer

def main():
    print("Starting NLM Comprehensive Benchmarks")
    
    # Create benchmark suite
    benchmark = NLMBenchmarkSuite()
    
    # Run specific benchmark stages
    stages = ["creation", "simulation", "plasticity", "memory"]
    
    for stage in stages:
        print(f"\nRunning {stage} benchmarks...")
        # Run specific stage
        if stage == "creation":
            results = benchmark.run_brain_creation_benchmark()
        elif stage == "simulation":
            results = benchmark.run_simulation_benchmark()
        elif stage == "plasticity":
            results = benchmark.run_plasticity_benchmark()
        elif stage == "memory":
            results = benchmark.run_memory_benchmark()
        
        benchmark.results.extend(results)
    
    # Analyze results
    statistics = benchmark.calculate_statistics(benchmark.results)
    
    # Generate performance report
    analyzer = PerformanceAnalyzer(benchmark.results)
    reports = analyzer.generate_reports()
    
    # Save comprehensive results
    benchmark.save_results("comprehensive_benchmarks.json")
    
    # Print summary
    benchmark.print_summary()
    
    print("\nComprehensive benchmarks completed successfully!")
    print(f"Total benchmarks run: {len(benchmark.results)}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
```

## Conclusion

Benchmarks are essential for:

1. **Performance Monitoring**: Track how NLM performance changes over time
2. **Optimization**: Identify and eliminate performance bottlenecks
3. **Validation**: Ensure that changes don't break existing functionality
4. **Comparison**: Compare NLM with other neural models
5. **Documentation**: Provide evidence of performance characteristics

The NLM benchmarks provide a comprehensive framework for performance evaluation, enabling developers and researchers to:

- Understand NLM's performance characteristics
- Identify optimization opportunities
- Compare different configurations
- Validate performance claims
- Support reproducible research

By running and analyzing benchmarks regularly, the NLM project can ensure that:

- Performance improvements are measured and documented
- New features don't degrade existing performance
- The system scales appropriately with increasing complexity
- Resource requirements are understood and managed
- Research results are reproducible and comparable
"""


def run_example_example():
    """Run an example to demonstrate the functionality."""
    print("=== NLM Example: Basic Brain Creation ===")
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initialized: {brain.getTotalNeuronCount() > 0}")
    
    # Run a few steps
    for i in range(5):
        brain.step(i)
    
    print(f"After 5 steps: {brain.getTotalSpikeCount()} spikes generated")
    print("✓ Example completed successfully!")
    return True


def run_all_examples():
    """Run all example functions to demonstrate functionality."""
    print("=== Running All NLM Examples ===")
    
    examples = [
        ("Basic Brain Creation", run_example_example),
    ]
    
    results = {}
    
    for name, example_func in examples:
        print(f"\nRunning: {name}")
        try:
            example_func()
            results[name] = "SUCCESS"
            print(f"✓ {name} completed successfully")
        except Exception as e:
            results[name] = f"FAILED: {e}"
            print(f"✗ {name} failed: {e}")
    
    # Summary
    print("\n=== Example Summary ===")
    successful = sum(1 for r in results.values() if r == "SUCCESS")
    total = len(results)
    
    for name, result in results.items():
        status = "✓" if result == "SUCCESS" else "✗"
        print(f"{status} {name}: {result}")
    
    print(f"\nOverall: {successful}/{total} examples completed successfully")
    return results


if __name__ == "__main__":
    # Create example directory structure
    create_example_directory_structure()
    
    # Run examples
    results = run_all_examples()
    
    print("\n✅ Example directory structure created and examples run successfully!")
    print("\nExamples directory contains:")
    print("- examples/: Main examples directory")
    print("  - tutorials/: Beginner and intermediate examples")
    print("  - advanced/: Advanced examples")
    print("  - research/: Research-grade examples")
    print("  - benchmarks/: Performance benchmark examples")
