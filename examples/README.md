# NLM Examples Directory

This directory contains example scripts demonstrating various NLM (Neural Learning Machine) use cases, from basic functionality to advanced training and analysis.

## Overview

These examples are designed to help users get started with NLM quickly and understand its capabilities. Each example demonstrates a different aspect of the NLM framework.

## Example Scripts

### Example 1: Basic Brain Simulation (`example1_basic_brain.py`)

**Purpose:** Demonstrates the most basic NLM functionality.

**What it does:**
- Creates a brain with default configuration
- Initializes the brain
- Runs simulation steps
- Shows basic brain statistics
- Saves brain state

**Learning outcomes:**
- Understand basic NLM brain creation and initialization
- Learn how to run simulation steps
- Get familiar with brain statistics
- Practice saving/loading brain states

**Difficulty:** Beginner

### Example 2: Agent in World Simulation (`example2_agent_world.py`)

**Purpose:** Demonstrates agent-environment interaction.

**What it does:**
- Creates a complete agent-world system
- Configures and initializes a 2D world
- Sets up agent with learning systems enabled
- Runs a simulation where the agent explores and learns
- Shows sensory processing and motor output
- Demonstrates reward-based learning

**Learning outcomes:**
- Understand the agent-world architecture
- Learn how sensory input is processed
- See how motor commands are generated
- Experience exploration and learning in action

**Difficulty:** Intermediate

### Example 3: Training with Advanced API (`example3_advanced_training.py`)

**Purpose:** Demonstrates advanced training capabilities.

**What it does:**
- Uses the AdvancedAgentTrainer for comprehensive training
- Creates advanced training configurations
- Implements curriculum learning
- Shows adaptive control during training
- Performs evaluation and analysis
- Demonstrates hyperparameter optimization

**Learning outcomes:**
- Learn advanced training concepts
- Understand curriculum learning and adaptive controls
- Experience hyperparameter optimization
- See comprehensive training analysis

**Difficulty:** Advanced

### Example 4: Visualization and Analysis (`example4_visualization_analysis.py`)

**Purpose:** Demonstrates visualization and analysis tools.

**What it does:**
- Creates rich visualizations of brain dynamics
- Generates agent performance analysis
- Performs world dynamics visualization
- Conducts statistical analysis
- Creates comprehensive reports
- Saves all visualizations to files

**Learning outcomes:**
- Learn how to visualize neural activity
- Understand performance analysis techniques
- Create professional-quality plots
- Perform statistical analysis

**Difficulty:** Intermediate to Advanced

### Example 5: Checkpointing and Reproducibility (`example5_checkpointing_reproducibility.py`)

**Purpose:** Demonstrates checkpoint management and reproducible simulations.

**What it does:**
- Shows how to create and save simulation checkpoints
- Demonstrates loading from checkpoints
- Compares different simulation runs
- Analyzes reproducibility
- Creates comprehensive reports
- Manages multiple simulation scenarios

**Learning outcomes:**
- Understand checkpoint management
- Learn how to reproduce experiments
- See how to manage simulation states
- Practice reproducibility analysis

**Difficulty:** Intermediate to Advanced

## Running Examples

### Prerequisites

1. Install NLM:
   ```bash
   pip install pynlm
   ```

2. Install visualization and analysis dependencies (for examples 4 and 5):
   ```bash
   pip install matplotlib numpy scipy pandas seaborn plotly jupyter
   ```

3. Ensure the enhanced training and analysis APIs are available:
   ```bash
   pip install -e .  # Install in development mode with all features
   ```

### Running Individual Examples

Run each example from the project root:

```bash
# Example 1: Basic brain simulation
python examples/example1_basic_brain.py

# Example 2: Agent-world simulation
python examples/example2_agent_world.py

# Example 3: Advanced training
python examples/example3_advanced_training.py

# Example 4: Visualization and analysis
python examples/example4_visualization_analysis.py

# Example 5: Checkpointing and reproducibility
python examples/example5_checkpointing_reproducibility.py
```

### Running All Examples

Create and run a comprehensive test:

```bash
#!/bin/bash

# Run all examples in sequence
echo "Running NLM Examples in Order"
echo "==========================="

python examples/example1_basic_brain.py
if [ $? -ne 0 ]; then
    echo "Example 1 failed"
    exit 1
fi

echo ""
python examples/example2_agent_world.py
if [ $? -ne 0 ]; then
    echo "Example 2 failed"
    exit 1
fi

echo ""
python examples/example3_advanced_training.py
if [ $? -ne 0 ]; then
    echo "Example 3 failed"
    exit 1
fi

echo ""
python examples/example4_visualization_analysis.py
if [ $? -ne 0 ]; then
    echo "Example 4 failed"
    exit 1
fi

echo ""
python examples/example5_checkpointing_reproducibility.py
if [ $? -ne 0 ]; then
    echo "Example 5 failed"
    exit 1
fi

echo ""
echo "All examples completed successfully!"
```

### Running with Different Environments

**With virtual environment:**
```bash
# Create virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install dependencies
pip install pynlm matplotlib numpy scipy pandas seaborn plotly jupyter
pip install -e .  # Install with development features

# Run examples
python examples/example1_basic_brain.py
```

**With Docker:**
```bash
# Build and run with Docker
docker build -t nlm-examples .
docker run --rm -v $(pwd):/app nlm-examples examples/example2_agent_world.py
```

## Example Outputs

Each example generates output files and visualizations:

### Example 1 Outputs:
- `basic_brain_checkpoint.bin` - Saved brain state

### Example 2 Outputs:
- `agent_world_brain.bin` - Saved agent-world brain state

### Example 3 Outputs:
- `checkpoints/best_model.h5` - Best model checkpoint
- Training analysis files

### Example 4 Outputs:
- `brain_dynamics.png` - Neural activity visualization
- `agent_performance.png` - Agent performance analysis
- `world_dynamics.png` - Environmental interaction visualization
- `analysis_report.txt` - Comprehensive analysis report

### Example 5 Outputs:
- `nlm_checkpoints/` - Directory with all checkpoints
- `checkpointing_report.txt` - Reproducibility report

## Additional Resources

### Documentation

- **Main Documentation:** `README.md` - Project overview and setup
- **API Reference:** `python/api_reference.md` - Complete API documentation
- **Architecture Guide:** `docs/ARCHITECTURE.md` - System architecture
- **Phase 6 Final Audit:** `docs/PHASE6_FINAL_AUDIT.md` - Current integration status

### Advanced Features

- **Visualization Tools:** `python/visualization_analysis.md` - Enhanced visualization capabilities
- **Training APIs:** `python/training_apis.md` - Advanced training and evaluation APIs
- **Installation Guide:** `python/INSTALL.md` - Enhanced installation and build process

### Research and Experiments

- **Phase 4 Experiments:** `src/experiments/Phase4Experiment.cpp` - Cognitive experiments
- **Phase 5 Experiments:** `src/experiments/Phase5Experiment.cpp` - Performance optimization experiments
- **Phase 6 Integration:** `src/experiments/Phase6IntegratedExperiment.cpp` - Final integration experiments

## Tips for Using Examples

### 1. Modifying Examples

Customize examples for your specific needs:

```python
# Example: Modify configuration in example2_agent_world.py
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)  # More neurons
config.set("brain.synapse_density", 0.2)  # More connections
```

### 2. Adding New Examples

Create new example files in the examples directory following the existing pattern.

### 3. Running with Custom Worlds

Create custom world configurations and pass them to examples.

### 4. Performance Tuning

For long-running examples, consider using:

```python
# Add progress bars
from tqdm import tqdm

for step in tqdm(range(num_steps)):
    # Your simulation code
```

### 5. Debugging

Add debugging output to examples:

```python
import logging
logging.basicConfig(level=logging.DEBUG)

# Add print statements
print(f"Step {step}: Reward = {reward}, Curiosity = {curiosity}")
```

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'nlm.training'**
   - Ensure the enhanced training APIs are installed: `pip install -e .`

2. **ImportError: No module named 'nlm.visualization'**
   - Install visualization dependencies: `pip install matplotlib numpy seaborn`

3. **Memory issues**
   - Reduce neuron count in configuration: `config.set("brain.neuron_count", 500)`

4. **Performance issues**
   - Use fewer simulation steps: `for step in range(50):` instead of `for step in range(1000):`

### Getting Help

If you encounter issues:

1. Check the example output for error messages
2. Review the main documentation in `README.md`
3. Check the API reference in `python/api_reference.md`
4. Join the NLM community for support
5. Report issues on the NLM GitHub repository

## Contributing

### Improving Examples

1. Fork the repository
2. Create a new example file in the examples directory
3. Follow the existing example patterns
4. Add comprehensive documentation
5. Test the example thoroughly
6. Submit a pull request

### Adding New Features

1. Extend the existing training APIs
2. Add new visualization capabilities
3. Enhance checkpoint management
4. Implement advanced analysis tools

## Conclusion

These examples provide a comprehensive introduction to NLM capabilities, from basic brain simulation to advanced training and analysis. By working through these examples, users can quickly become proficient with NLM and understand its full potential for neural simulation research and applications.

The examples are designed to be educational while remaining practical, providing a solid foundation for both learning and production use of NLM.

Happy exploring with NLM!
