# NLM (Neural Learning Machine) - Framework Overview

NLM (熙然) is a comprehensive neural learning machine framework that implements a complete artificial developmental brain with advanced learning, memory, and cognitive systems.

## Quick Start

### 1. Installation

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# Install (optional)
sudo make install
```

### 2. Basic Usage

```bash
# Run the main neural network simulation
./build/nlm

# With custom configuration
./build/nlm --config configs/my_config.cfg

# With advanced commands
./build/nlm --config configs/my_config.cfg --brain-info --memory-info
```

### 3. Python Bindings

```bash
# Install Python bindings
pip install .

# Example Python script
python examples/simple_example.py
```

## Key Features

### 🧠 Advanced Neural Systems

- **Real LIF neurons**: Leaky Integrate-and-Fire neurons with event-driven spike propagation
- **Synaptic plasticity**: STDP (Spike-Timing-Dependent Plasticity) and Hebbian learning
- **Structural plasticity**: Synaptogenesis (formation) and pruning
- **Multi-region architecture**: Hierarchical brain organization

### 🧠 Integrated Memory Systems

- **Working memory**: Transient active information storage
- **Episodic memory**: Experience and event recording
- **Associative memory**: Pattern and concept associations

### 🧠 Prediction and Cognition

- **Prediction system**: Forward models and error computation
- **Attention system**: Selective focus and resource allocation
- **Planning system**: Action planning and decision making
- **Concept formation**: Pattern discovery and abstraction

### 🧠 Neuromodulation and Development

- **Dopamine system**: Reward prediction and reinforcement learning
- **Curiosity system**: Exploration motivation and intrinsic reward
- **Development system**: Age-dependent plasticity and maturation

## Core Components

### 1. nlm::Brain
The central neural processing unit with all integrated systems.

**Key Features:**
- Real spiking neural computation
- Multi-region architecture
- Integrated memory and prediction systems
- Neuromodulation and development
- Event-driven spike propagation

### 2. nlm::Config
Enhanced configuration management with validation and schema support.

**Key Features:**
- Parameter validation and type checking
- Schema-based configuration
- Default value management
- Help system and documentation
- Command-line and file configuration

### 3. nlm::Agent
Bridge between brain and environment for embodied agents.

**Key Features:**
- Sensory processing and integration
- Motor command generation
- Learning system integration
- Development and adaptation

### 4. nlm::World
Environment simulation and interaction.

**Key Features:**
- 2D grid environment
- State management
- Action execution
- Sensory perception

## Advanced Features

### 1. Command Line Interface

Comprehensive command line interface for advanced users:

```bash
# Brain state commands
--brain-info              Show detailed brain information
--brain-reset             Reset brain state
--brain-checkpoint        Create checkpoint

# Memory management commands
--memory-info             Show memory system information

# Configuration commands
--config-show             Show current configuration
--config-save             Save configuration to file

# Debug and profiling
--debug-mode              Enable debugging
--performance-stats       Show performance statistics

# Validation
--validate-config         Validate configuration
--validate-brain          Validate brain state

# Export/import
--export-brain <file>     Export brain state
--import-brain <file>     Import brain state

# Help
--advanced-help           Show advanced help
--help                    Show basic help
```

### 2. Configuration Validation

Automatic validation of configuration parameters with detailed error reporting:

```cpp
// Create enhanced configuration
auto config = nlm::createDefaultConfig();

// Set configuration values
config->set("neuron_count", 1000);
config->set("simulation_timestep", 0.001);

// Validate configuration
if (!config->isValid()) {
    for (const auto& error : config->getValidationErrors()) {
        std::cout << "Error: " << error << std::endl;
    }
}
```

### 3. Memory Management

Advanced memory management with working memory, episodic memory, and associative memory:

```cpp
// Working memory for active information
if (brain->getWorkingMemory()) {
    brain->getWorkingMemory()->storeToNeuron(neuronId, activation);
    brain->getWorkingMemory()->update(timestep);
}

// Episodic memory for experience storage
if (brain->getEpisodicMemory()) {
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = reward;
    brain->getEpisodicMemory()->storeEpisode(episode);
}
```

### 4. Learning and Development

Comprehensive learning systems with reward modulation, curiosity, and development:

```cpp
// Create agent with learning systems
auto agent = nlm::createAgentBrain(brain);

// Enable learning systems
agent->enableRewardModulation(true);  // Reward-based learning
agent->enableCuriosity(true);          // Curiosity-driven exploration
agent->enableDevelopment(true);        // Developmental plasticity
```

### 5. Performance Optimization

Built-in performance optimizations:

- **Parallel processing**: Parallel execution for large-scale simulations
- **Memory pools**: Efficient memory allocation
- **SIMD vectorization**: CPU instruction-level optimization
- **Sparse connectivity**: Reduced memory footprint for large networks

## Configuration Parameters

### Neural System Parameters
- `random_seed`: Random seed for reproducible experiments
- `simulation_timestep`: Simulation timestep in seconds
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Connection probability for random connectivity

### Plasticity Parameters
- `plasticity_learning_rate`: General plasticity learning rate
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Synaptogenesis rate
- `pruning_rate`: Pruning rate

### Neuromodulation Parameters
- `dopamine_baseline`: Baseline dopamine level

### Reward System Parameters
- `reward_discount_factor`: Reward discount factor

### Environment Parameters
- `environment_name`: Environment name
- `environment_width`: Environment width
- `environment_height`: Environment height

### Logging Parameters
- `log_level`: Logging level
- `log_to_file`: Log to file
- `log_filename`: Log filename

### Simulation Parameters
- `max_simulation_steps`: Maximum simulation steps
- `simulation_time_limit`: Simulation time limit

### Visualization Parameters
- `visualization_enabled`: Enable visualization
- `visualization_update_rate`: Visualization update rate

## Example Projects

### 1. Simple Neural Network Simulation

`examples/simple_example.py` - Demonstrates basic NLM usage with a simple agent and environment.

### 2. Robot Navigation

`examples/robot_navigation.py` - Implements autonomous robot navigation using NLM's cognitive and motor systems.

### 3. Scientific Discovery

`examples/scientific_discovery.py` - Demonstrates NLM's ability to discover patterns and make predictions in scientific domains.

## Testing and Validation

### 1. Unit Tests

```bash
# Run all tests
cd tests
python -m pytest test_*.py

# Run specific test suite
python -m pytest test_brain.py -v
```

### 2. Integration Tests

```bash
# Run integration tests
python -m pytest integration/ -v

# Run performance benchmarks
python -m pytest performance/ -v
```

### 3. Configuration Validation

```bash
# Validate configuration
./build/nlm --validate-config

# Validate brain state
./build/nlm --validate-brain

# Check system consistency
./build/nlm --config-check
```

## Performance Guidelines

### 1. Scaling Recommendations

**For small simulations (100-1000 neurons):**
- Use standard configuration
- Enable visualization for debugging
- Use single-threaded execution

**For medium simulations (1000-10000 neurons):**
- Consider parallel processing
- Optimize memory usage
- Use checkpointing for long runs

**For large simulations (>10000 neurons):**
- Enable parallel processing
- Use sparse connectivity
- Consider memory pool optimizations

### 2. Memory Usage

Typical memory usage for different simulation sizes:

| Neurons | Memory Usage | Estimated Time |
|---------|-------------|----------------|
| 100     | ~10 MB      | < 1 second     |
| 1000    | ~100 MB     | ~10 seconds    |
| 10000   | ~1 GB       | ~2 minutes     |
| 100000  | ~10 GB      | ~30 minutes    |

### 3. Performance Optimization

- **Enable parallel processing** for simulations with >1000 neurons
- **Use checkpointing** for simulations longer than 10 minutes
- **Optimize configuration** to reduce unnecessary computation
- **Monitor memory usage** to prevent allocation failures

## Troubleshooting

### 1. Common Issues

**Issue**: Brain initialization fails
**Solution**: Check configuration parameters and ensure all required values are set.

**Issue**: High memory usage
**Solution**: Reduce neuron count, enable sparse connectivity, or use memory pools.

**Issue**: Slow performance
**Solution**: Enable parallel processing, optimize configuration, or use SIMD instructions.

**Issue**: Configuration validation errors
**Solution**: Check configuration parameters against schema, use `--config-show` for help.

### 2. Error Messages

**Configuration validation errors**: Use `--validate-config` and check error messages.
**Memory allocation errors**: Check available memory and system resources.
**File I/O errors**: Verify file permissions and disk space.
**Performance bottlenecks**: Use `--performance-stats` to identify bottlenecks.

## Further Resources

### Documentation
- [API Reference](https://nlm.readthedocs.io/api) - Complete API documentation
- [User Guide](https://nlm.readthedocs.io/user-guide) - User instructions and examples
- [Advanced Topics](https://nlm.readthedocs.io/advanced-topics) - Advanced usage patterns

### Research
- [NLM Research Papers](https://github.com/nlm-project/research) - Research papers and technical reports
- [NLM Conference Papers](https://github.com/nlm-project/conferences) - Conference proceedings

### Community
- [GitHub Issues](https://github.com/nlm-project/nlm/issues) - Bug reports and feature requests
- [Discord Server](https://discord.gg/nlm) - Community discussion
- [Mailing List](https://groups.google.com/g/nlm-users) - User announcements

## Getting Help

### 1. Documentation
- Check the [official documentation](https://nlm.readthedocs.io) for detailed information
- Read the examples in the `examples/` directory
- Consult the API reference for method signatures and parameters

### 2. Community Support
- Post questions on [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- Join the [Discord community](https://discord.gg/nlm)
- Subscribe to the [mailing list](https://groups.google.com/g/nlm-users)

### 3. Technical Support
- For technical support, contact the development team
- Check the [troubleshooting guide](https://nlm.readthedocs.io/troubleshooting) for common issues

## Contributing

### 1. Code Contributions
- Fork the repository
- Create a feature branch
- Implement your changes
- Add tests for new functionality
- Submit a pull request

### 2. Documentation Contributions
- Update the documentation in `src/doc/`
- Add examples in the `examples/` directory
- Improve API documentation

### 3. Testing Contributions
- Add unit tests for new functionality
- Add integration tests for end-to-end scenarios
- Add performance benchmarks

### 4. Bug Reports
- Report bugs through [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- Include reproduction steps and error messages
- Provide environment information and configuration details

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Copyright

Copyright (c) 2023-2026 NLM Authors. All rights reserved.

## Acknowledgements

This framework is the result of years of research and development in artificial intelligence and computational neuroscience. We thank our contributors and supporters for making this project possible.

### Key Contributors
- [NLM Development Team](https://github.com/orgs/nlm-project/people)
- [Scientific Advisors](https://github.com/nlm-project/advisors)
- [Funding Organizations](https://github.com/nlm-project/funding)

### Related Projects
- [NLM Extensions](https://github.com/nlm-project/extensions) - Extensions and plugins
- [NLM Examples](https://github.com/nlm-project/examples) - Additional examples and demonstrations
- [NLM Tutorials](https://github.com/nlm-project/tutorials) - Educational tutorials and workshops

---

*This README was generated automatically by the NLM framework documentation system.*
For the latest version and updates, visit [https://github.com/nlm-project/nlm](https://github.com/nlm-project/nlm)