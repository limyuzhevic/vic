# NLM Examples Directory

This directory contains comprehensive examples demonstrating the NLM (Neural Learning Machine) system.

## Overview

The NLM examples provide practical demonstrations of the system's capabilities, from basic usage to advanced features. Each example showcases different aspects of the NLM system and provides hands-on experience with the Python interface, plugin system, and various NLM components.

## Example Structure

Each example directory contains:

1. **README.md** - Example documentation and instructions
2. **source files** - Example code implementation
3. **configuration files** - Example configurations
4. **data files** - Sample data for testing

## Available Examples

### 1. Basic NLM Usage
**Location:** `basic_nlm_usage/`

**Purpose:** Introduction to basic NLM system usage and fundamental operations.

**Key Features:**
- System initialization
- Brain and world creation
- Basic simulation loop
- Stimulus injection
- State management

### 2. Advanced Configuration
**Location:** `advanced_config/`

**Purpose:** Advanced configuration examples for complex NLM setups.

**Key Features:**
- Custom brain configurations
- Neural network topology setup
- Plasticity rule configuration
- Neuromodulation setup
- Memory system configuration

### 3. Plugin Development
**Location:** `plugin_development/`

**Purpose:** Examples of creating and using NLM plugins.

**Key Features:**
- Plugin creation and lifecycle management
- Custom neuron types
- Custom learning rules
- Neuromodulation plugins
- Integration plugins
- Analysis plugins

### 4. Monitoring and Visualization
**Location:** `monitoring_visualization/`

**Purpose:** Examples of monitoring, visualization, and analysis.

**Key Features:**
- Real-time monitoring
- Brain state visualization
- Neural activity plotting
- Performance metrics
- Dashboard creation

### 5. Python Scripting
**Location:** `python_scripting/`

**Purpose:** Python scripting examples and automation.

**Key Features:**
- Batch processing
- Automated experiments
- Data analysis
- Report generation
- Custom workflows

### 6. Benchmarking
**Location:** `benchmarking/`

**Purpose:** Performance benchmarking and optimization examples.

**Key Features:**
- Performance measurement
- Benchmark comparison
- Optimization techniques
- Profiling examples
- Performance analysis

### 7. Experiments
**Location:** `experiments/`

**Purpose:** Experimental setup and analysis examples.

**Key Features:**
- Experimental design
- Parameter optimization
- Result analysis
- Statistical analysis
- Experiment automation

## How to Use Examples

### Running Examples

1. **Install Prerequisites**
   ```bash
   # Install NLM Python interface
   pip install -e python/
   
   # Install optional dependencies
   pip install numpy matplotlib pandas scipy seaborn
   ```

2. **Navigate to Examples Directory**
   ```bash
   cd /path/to/nlm/examples
   ```

3. **Run Example**
   ```bash
   python example_name.py
   ```

### Working with Examples

#### Basic Usage Example

This example demonstrates fundamental NLM operations:

```python
import pynlm

# Initialize system
interface = pynlm.initialize_nlm()

# Create brain
brain = interface.create_brain()

# Create world
world = interface.create_world()

# Run simulation
interface.run_loop(duration=10, dt=0.1)

# Clean up
interface.reset_system()
```

#### Advanced Configuration Example

This example demonstrates advanced configuration:

```python
import pynlm

# Initialize system
interface = pynlm.initialize_nlm()

# Create custom configuration
config = pynlm.Config()
config.set("brain.neuron_count", 1000)
config.set("brain.synaptic_density", 0.1)
config.set("brain.learning_rate", 0.01)

# Create brain with configuration
brain = interface.create_brain(config)
interface.set_brain(brain)

# Run simulation
interface.run_loop(duration=5, dt=0.01)
```

#### Plugin Development Example

This example demonstrates plugin development:

```python
import pynlm

# Create custom plugin
class CustomPlugin(pynlm.Plugin):
    def _initialize_plugin(self):
        self.register_event_handler(
            pynlm.EventType.STATE_CHANGED,
            self._on_state_change
        )
    
    def _enable_plugin(self):
        print(f"Plugin enabled: {self.info.name}")
    
    def _on_state_change(self, event):
        print(f"State changed: {event.event_type}")

# Create and load plugin
plugin = CustomPlugin(
    name='custom_plugin',
    version='1.0.0',
    description='Custom plugin',
    author='Author',
    plugin_type=pynlm.PluginType.UTILITY
)

# Use plugin
plugin.initialize(interface)
plugin.enable()
```

## Example Categories

### Category 1: Basic Examples

- **Basic Usage**: Introduction to NLM system
- **Simple Brain Creation**: Basic brain configuration
- **First Simulation**: Running initial simulation
- **State Management**: Saving and loading states

### Category 2: Configuration Examples

- **Custom Configuration**: Advanced brain setup
- **Neural Network Topology**: Custom network structures
- **Plasticity Rules**: Custom learning rules
- **Neuromodulation**: Custom neuromodulators

### Category 3: Plugin Examples

- **Brain Plugins**: Custom neural architectures
- **Plasticity Plugins**: Custom learning rules
- **Memory Plugins**: Custom memory systems
- **Sensory Plugins**: Custom sensory processing
- **Motor Plugins**: Custom action selection
- **Development Plugins**: Custom developmental stages
- **Integration Plugins**: External system interfaces
- **Analysis Plugins**: Custom analysis tools
- **Utility Plugins**: General-purpose utilities

### Category 4: Visualization Examples

- **Real-time Monitoring**: Live brain state monitoring
- **Neural Activity Visualization**: Neural activity plots
- **Memory System Visualization**: Memory system displays
- **Performance Visualization**: Performance metrics
- **Dashboard Creation**: Customizable dashboards

### Category 5: Analysis Examples

- **Data Analysis**: Statistical analysis of neural data
- **Performance Analysis**: Performance benchmarking
- **Behavioral Analysis**: Behavioral pattern analysis
- **Neural Analysis**: Neural activity analysis
- **Connectomics**: Neural connectivity analysis

### Category 6: Automation Examples

- **Batch Processing**: Processing multiple simulations
- **Automated Experiments**: Running parameter sweeps
- **Result Analysis**: Automated result analysis
- **Report Generation**: Generating analysis reports

## Example Files

### Source Code Examples

Each example includes:

1. **Main Example File**
   - `example.py`: Main example implementation
   - `example.cpp`: C++ example implementation

2. **Configuration Files**
   - `config.json`: Example configuration
   - `*.ini`: Configuration templates
   - `*.yaml`: YAML configuration

3. **Data Files**
   - `sample_data.json`: Sample input data
   - `test_data.csv`: Test data
   - `*.npy`: NumPy data files

4. **Documentation**
   - `README.md`: Example documentation
   - `HOWTO.md`: Usage instructions
   - `API.md`: API reference

## Development Guidelines

### Contributing Examples

When adding new examples:

1. **Code Quality**
   - Follow existing code style
   - Include comprehensive comments
   - Use meaningful variable names
   - Add error handling

2. **Documentation**
   - Write clear README
   - Document command-line usage
   - Include parameter descriptions
   - Provide examples

3. **Testing**
   - Include unit tests
   - Test edge cases
   - Verify functionality
   - Performance test critical paths

4. **Best Practices**
   - Use proper error handling
   - Implement resource cleanup
   - Use appropriate data structures
   - Follow project conventions

### Running Examples

#### Prerequisites

- Python 3.8+
- NLM Python interface installed
- Required dependencies (numpy, matplotlib, etc.)

#### Running Examples

1. **Basic Execution**
   ```bash
   python examples/example_name.py
   ```

2. **With Arguments**
   ```bash
   python examples/example_name.py --config config.json --output output_dir
   ```

3. **With Environment Variables**
   ```bash
   NLM_LOG_LEVEL=debug python examples/example_name.py
   ```

4. **With Profiling**
   ```bash
   python -m cProfile -o profile.stats examples/example_name.py
   ```

## Example Index

### Quick Reference

| Example | Location | Description |
|---------|----------|-------------|
| Basic Usage | `basic_nlm_usage/` | Introduction to NLM system |
| Advanced Config | `advanced_config/` | Advanced configuration examples |
| Plugin Dev | `plugin_development/` | Plugin development examples |
| Monitoring | `monitoring_visualization/` | Monitoring and visualization |
| Python Scripting | `python_scripting/` | Python scripting examples |
| Benchmarking | `benchmarking/` | Performance benchmarking |
| Experiments | `experiments/` | Experimental setup and analysis |

### Example Categories

1. **Getting Started**: Basic examples for newcomers
2. **Advanced**: Complex examples for experienced users
3. **Development**: Examples for developers
4. **Performance**: Performance optimization examples
5. **Integration**: System integration examples
6. **Research**: Research and analysis examples

## Support

For additional support:

1. **Documentation**: https://docs.nlm.org
2. **GitHub Issues**: https://github.com/nlm-project/nlm/issues
3. **Community Forum**: https://community.nlm.org
4. **API Reference**: https://api.nlm.org
5. **Examples Documentation**: https://examples.nlm.org

## License

This project is licensed under the MIT License.

All examples are provided under the same license as the main NLM project.

## Contact

For questions or feedback:

- Email: support@nlm.org
- GitHub: https://github.com/nlm-project/nlm/issues
- Slack: #nlm-examples

## Acknowledgments

The NLM examples project acknowledges the contributions of:

- All NLM developers and researchers
- Example contributors
- Documentation writers
- Test developers
- Performance engineers

This examples directory is maintained by the NLM development team and is continuously updated with new examples and improvements.

---

**Note:** This examples directory is part of the NLM (Neural Learning Machine) project. For additional information, please refer to the main NLM documentation and API reference.