# NLM Getting Started Guide

## Welcome to NLM!

This guide will help you get started with the Neural Learning Machine (NLM) platform. Whether you're a researcher, developer, or student, this documentation will guide you through installation, basic usage, and exploration of NLM's capabilities.

## Prerequisites

### System Requirements

NLM requires:
- **Operating System**: Linux, macOS, or Windows (with WSL)
- **Compiler**: C++20 compatible compiler (GCC 9+, Clang 12+, MSVC 2019+)
- **Build Tools**: CMake 3.16 or higher
- **Memory**: At least 2 GB RAM (more for large-scale simulations)
- **Disk Space**: 1 GB for basic installation, more for large networks

### Optional Dependencies

- **Python**: For Python bindings (version 3.8+)
- **pybind11**: For Python integration
- **Graphics**: For visualization (optional)
- **Debug Tools**: For debugging and profiling

## Quick Installation

### Step 1: Clone the Repository

```bash
# Clone the NLM repository
cd /workspace/app
git clone https://github.com/your-org/nlm.git
cd nlm

# Or if already cloned
# cd /workspace/app
```

### Step 2: Build with CMake

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (choose your configuration)
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DCMAKE_BUILD_TYPE=Debug  # For debugging

# Build the project (use all available cores)
cmake --build . -j$(nproc)

# Build for Python bindings
cd ..
pip install -e python
```

### Step 3: Run Tests

```bash
# Run the advanced API test script
bash test_advanced_api.sh

# Or run the implementation test script
bash test_implementation.sh
```

## Basic Usage

### Running the Basic Demo

```bash
# Run NLM with basic configuration
./build/nlm --neuron-count=1000 --simulation-timestep=0.01 --max-steps=1000

# Run with visualization
./build/nlm --neuron-count=500 --max-steps=5000 --enable-checkpointing=true

# Run performance benchmark
./build/nlm --benchmark --neurons=10000 --steps=10000

# Get help information
./build/nlm --help
```

### Configuration File Usage

Create a configuration file (e.g., `my_config.cfg`):

```
# Basic NLM configuration
neuron_count = 2000
simulation_timestep = 0.001
enable_plasticity = true
enable_neuromodulation = true
enable_curiosity = true
enable_novelty = true
enable_prediction_error = true
max_steps = 10000
output_dir = ./results
log_level = info
```

Run with configuration file:

```bash
./build/nlm --config=my_config.cfg
```

### Python API Usage

```python
import pynlm

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'simulation_timestep': 0.001,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True,
    'enable_structural_plasticity': True,
    'enable_development': True,
    'learning_rate': 0.001,
    'discount_factor': 0.99
})

# Create brain and agent
brain = pynlm.createBrain(brain_config)
agent_brain = pynlm.createAgentBrain(brain)

# Create advanced agent
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'Q_LEARNING',
    'learning_rate': 0.01,
    'discount_factor': 0.95,
    'replay_buffer_size': 1000
})

# Start learning
advanced_agent.startLearning()

# Run simulation
for episode in range(10):
    for step in range(100):
        # Your simulation code here
        state = [float(i) for i in range(20)]  # Example state
        action = advanced_agent.selectAction(state)
        reward = advanced_agent.computeCustomReward(state, action, state)
        advanced_agent.addExperience(state, action, reward, state, False)
        advanced_agent.performRLStep()
```

## Learning Path

### For Beginners

1. **Explore the Architecture**: Read `/workspace/app/docs/architecture.md`
2. **Run Basic Examples**: Start with the simple demo
3. **Understand Configuration**: Learn the configuration system
4. **Try Python Bindings**: Get familiar with the Python API
5. **Explore Tutorials**: Follow the beginner tutorials

### For Intermediate Users

1. **Study Performance**: Read `/workspace/app/docs/performance.md`
2. **Configure Advanced Features**: Set up neuromodulation, prediction, memory
3. **Debug Issues**: Use debugging tools and logging
4. **Optimize Performance**: Implement performance optimizations
5. **Create Experiments**: Design and run your own experiments

### For Advanced Users

1. **Deep Architecture**: Study Phase 5 performance infrastructure
2. **Research Development**: Create new neural models or plasticity rules
3. **Integration**: Connect NLM with other systems
4. **Large-Scale**: Run millions of neurons
5. **Contribution**: Contribute to the research platform

## Available Examples

### Command Line Examples

```bash
# Example 1: Basic configuration
./build/nlm --neuron-count=500 --simulation-timestep=0.01

# Example 2: With plasticity and neuromodulation
./build/nlm --neuron-count=1000 --enable-plasticity=true --enable-neuromodulation=true

# Example 3: Performance benchmark
./build/nlm --benchmark --neurons=5000 --steps=5000
```

### Python API Examples

```python
# Example 1: Basic configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 1000,
    'enable_plasticity': True,
    'enable_neuromodulation': True
})

# Example 2: Advanced agent with RL
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'DEEP_Q_LEARNING',
    'replay_buffer_size': 5000
})

# Example 3: Monitoring and visualization
pynlm.python.MonitoringSystem.startMetricsStreaming(brain)
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Build Failures
**Solution**: Ensure you have a C++20 compatible compiler and CMake 3.16+

```bash
# Check compiler version
g++ --version
# Should show version 9.0 or higher

# Check CMake version
cmake --version
# Should show version 3.16 or higher
```

#### Issue: Python Bindings Not Found
**Solution**: Install Python dependencies and build Python module

```bash
# Install Python dependencies
pip install -r python/requirements.txt

# Build Python module
cd /workspace/app
pip install -e python
```

#### Issue: Out of Memory
**Solution**: Reduce neuron count or enable performance optimizations

```bash
# For memory constraints, use smaller configurations
./build/nlm --neuron-count=500 --enable-performance-optimizations=true
```

### Getting Help

#### Online Resources
- **GitHub Repository**: https://github.com/your-org/nlm
- **Documentation**: https://docs.nlm.org
- **Discussions**: GitHub Discussions

#### Local Help
- **Command Line Help**: `./build/nlm --help`
- **Python Documentation**: `python/README.md`
- **Debug Output**: Enable verbose logging with `--log-level=debug`

## Next Steps

### Immediate Actions
1. **Install and Build**: Complete the installation process
2. **Run Demos**: Try the basic examples to understand the workflow
3. **Explore Documentation**: Read the architecture and configuration guides
4. **Set Up Python**: Install and test the Python bindings

### Learning Activities
1. **Study the Code**: Explore the source code in `/workspace/app/src/`
2. **Experiment**: Create your own configurations and experiments
3. **Join the Community**: Participate in discussions and contribute
4. **Research**: Use NLM for your research projects

### Development Activities
1. **Extend Features**: Add new neural models or plasticity rules
2. **Performance Optimization**: Implement performance improvements
3. **New Integrations**: Connect NLM with other systems
4. **Documentation**: Improve and extend documentation

## Support

### Documentation
- This Getting Started Guide: `/workspace/app/docs/getting_started.md`
- Main Documentation: `/workspace/app/docs/README.md`
- Python API: `/workspace/app/python/README.md`

### Examples
- Command Line Examples: `/workspace/app/docs/examples/command_line_examples.md`
- Python API Examples: `/workspace/app/docs/examples/python_api_examples.md`
- Configuration Examples: `/workspace/app/docs/examples/configuration_examples.md`

### Testing
- Run Tests: `bash test_advanced_api.sh`
- Run Implementation Tests: `bash test_implementation.sh`

## Learning Resources

### Tutorial System
- **Beginner Tutorial**: `/workspace/app/docs/tutorials/beginner.md`
- **Intermediate Tutorial**: `/workspace/app/docs/tutorials/intermediate.md`
- **Advanced Tutorial**: `/workspace/app/docs/tutorials/advanced.md`

### Best Practices
- **Performance Best Practices**: `/workspace/app/docs/best_practices/performance_best_practices.md`
- **Configuration Best Practices**: `/workspace/app/docs/best_practices/configuration_best_practices.md`
- **Debugging Best Practices**: `/workspace/app/docs/best_practices/debugging_best_practices.md`

## Conclusion

You've completed the initial setup of NLM! Here's what you should do next:

1. **Get Oriented**: Read the architecture documentation
2. **Experiment**: Run the basic examples
3. **Explore**: Try the Python bindings
4. **Learn**: Follow the tutorial system
5. **Optimize**: Implement performance improvements
6. **Research**: Use NLM for your research projects

NLM provides a powerful platform for implementing biologically-inspired artificial intelligence. Its unique architecture, based on spiking neural networks and developmental processes, offers a fundamentally different approach to machine learning and cognition.

Happy coding and learning with NLM! 🧠✨