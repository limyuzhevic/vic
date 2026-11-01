# NLM Documentation

This directory contains comprehensive documentation for the Neural Learning Machine (NLM) project, including:

- **Architecture Documentation**: System design and architecture
- **Phase Documentation**: Phase-specific information and results
- **Scientific Documentation**: Research methods and biological fidelity
- **Experimental Documentation**: Experiment descriptions and results
- **Performance Documentation**: Performance analysis and scaling
- **Limitations Documentation**: Known limitations and mitigations
- **Advanced Documentation**: Advanced usage patterns and API reference

## Documentation Structure

```
docs/
├── ADVANCED_USAGE.md                  # Advanced usage patterns and API reference
├── ARCHITECTURE.md                    # System architecture overview
├── BIOLOGICAL_FIDELITY.md              # Biological fidelity analysis
├── EXPERIMENTS.md                      # Experiment descriptions
├── FAILURES.md                         # Project failures and lessons learned
├── FINAL_LIMITATIONS.md                # Phase 6 final limitations
├── LIMITATIONS.md                      # General limitations
├── PERFORMANCE.md                      # Performance analysis
├── PHASE6_ARCHITECTURE.md              # Phase 6 architecture details
├── PHASE6_FINAL_AUDIT.md               # Phase 6 final audit results
├── PHASE6_FINAL_RESULTS.md             # Phase 6 final results
├── RESEARCH_METHODS.md                 # Research methods and protocols
├── ROADMAP.md                          # Project roadmap
├── SCALING.md                          # Scaling and optimization
├── SCIENCE.md                          # Scientific background
└── [other files]...
```

## Getting Started

### For Beginners

1. **NLM README.md** - Project overview and basic usage
2. **docs/ARCHITECTURE.md** - System architecture overview
3. **docs/SCIENCE.md** - Scientific background and principles
4. **easy_usage.md** - Simplified usage guide

### For Advanced Users

1. **docs/ADVANCED_USAGE.md** - Advanced usage patterns (this file)
2. **docs/PHASE6_ARCHITECTURE.md** - Phase 6 architecture details
3. **docs/PERFORMANCE.md** - Performance optimization
4. **docs/EXPERIMENTS.md** - Experimental results and analysis

### For Developers

1. **HOW_TO_USE.md** - Detailed usage instructions
2. **pyproject.toml** - Python package configuration
3. **CMakeLists.txt** - Build configuration
4. **docs/ADVANCED_USAGE.md** - API reference and implementation details

## Documentation Categories

### Core Documentation

#### Architecture & Design
- **docs/ARCHITECTURE.md** - Complete system architecture
- **docs/PHASE6_ARCHITECTURE.md** - Phase 6 specific architecture

#### Scientific Background
- **docs/SCIENCE.md** - Scientific principles and background
- **docs/BIOLOGICAL_FIDELITY.md** - Biological fidelity analysis
- **docs/RESEARCH_METHODS.md** - Research methods and protocols

#### Experimental Results
- **docs/EXPERIMENTS.md** - Experiment descriptions and results
- **docs/PHASE6_FINAL_RESULTS.md** - Phase 6 final results
- **docs/PHASE6_FINAL_AUDIT.md** - Phase 6 final audit

#### Performance & Optimization
- **docs/PERFORMANCE.md** - Performance analysis and optimization
- **docs/SCALING.md** - Scaling and performance characteristics

#### Limitations & Issues
- **docs/LIMITATIONS.md** - General system limitations
- **docs/FINAL_LIMITATIONS.md** - Phase 6 specific limitations
- **docs/FAILURES.md** - Project failures and lessons learned

### Advanced Documentation

#### Advanced Usage
- **docs/ADVANCED_USAGE.md** - Advanced usage patterns and API reference

## Code Example Conventions

### Python Code

```python
# Basic NLM usage
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation loop
for step in range(1000):
    brain.step(step)
```

### Configuration Examples

```python
# Advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 4)
config.set("brain.connection_probability", 0.15)
```

### File Paths

All file paths are relative to the project root unless otherwise specified.

### Version References

- **Phase 6**: Final integrated system
- **Phase 5**: Performance optimizations
- **Phase 4**: Advanced cognition systems
- **Phase 3**: World interaction loop
- **Phase 2**: Real neural computation
- **Phase 1**: Project skeleton

## Command Line Interface

### Basic Commands

```bash
# View advanced usage documentation
nlm-cli --help

# Create brain with custom configuration
nlm-cli create-brain --config my_config.json

# Run simulation
nlm-cli run-simulation --steps 1000 --output results.json

# Run batch simulation
nlm-cli batch config1.json config2.json --output-dir results
```

### Advanced Usage

```bash
# Monitor performance
nlm-cli monitor --duration 60 --interval 100

# Save checkpoint
nlm-cli save-brain brain_checkpoint.bin

# Load checkpoint
nlm-cli load-brain brain_checkpoint.bin

# List configuration options
nlm-cli list-config-options
```

## Advanced Documentation Features

### Comprehensive API Reference

The **docs/ADVANCED_USAGE.md** file provides:

- **Complete Python API reference** with all available methods
- **Advanced usage patterns** for experienced users
- **Configuration management** utilities
- **Batch processing** capabilities
- **Performance monitoring** tools
- **Memory system integration** examples
- **Neuromodulation control** techniques
- **Development and evolution** simulations
- **Checkpointing and persistence** management
- **Command line interface** documentation
- **Troubleshooting** advanced issues

### Code Examples

The documentation includes:

```python
# Advanced configuration example
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 4)
config.set("plasticity.stdp.enable", True)
```

```bash
# Command line example
./nlm-cli batch config1.json config2.json \
    --output-dir results --steps-per-config 1000
```

### Advanced Topics

The documentation covers:

1. **Configuration Deep Dive** - Advanced configuration parameters
2. **Brain Management** - Advanced brain manipulation and analysis
3. **Batch Processing** - Large-scale experiment management
4. **Performance Optimization** - Monitoring and optimization techniques
5. **Memory Integration** - Working memory and episodic memory systems
6. **Neuromodulation Control** - Dopamine and curiosity system control
7. **Development & Evolution** - Developmental stage management
8. **Checkpointing & Persistence** - Advanced checkpoint management
9. **Troubleshooting** - Advanced debugging and error resolution

## Contributing to Documentation

To contribute to the NLM documentation:

1. Fork the repository
2. Create a pull request with documentation updates
3. Follow existing documentation conventions
4. Ensure code examples compile and work correctly
5. Update the **docs/ADVANCED_USAGE.md** file for new features

## Additional Resources

- **HOW_TO_USE.md** - Detailed usage instructions
- **easy_usage.md** - Simplified usage guide
- **pyproject.toml** - Python package configuration
- **CMakeLists.txt** - Build configuration
- **nlm-cli** - Command line interface
- **docs/ADVANCED_USAGE.md** - Advanced usage patterns (this file)

## License

This documentation is licensed under the MIT License.

---

**Note**: The `docs/ADVANCED_USAGE.md` file is the comprehensive advanced usage guide that provides detailed information for experienced NLM users, including API reference, advanced patterns, and troubleshooting guidance.