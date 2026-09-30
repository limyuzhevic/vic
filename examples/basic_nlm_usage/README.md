# Basic NLM Usage Examples

This directory contains basic examples for getting started with the NLM (Neural Learning Machine) system.

## Overview

These examples demonstrate fundamental NLM operations and provide a foundation for understanding the system.

## Available Examples

### 1. Basic Brain Creation
**File:** `basic_brain.cpp`

**Description:** Creates a simple brain with default configuration and performs basic operations.

**Features:**
- Brain initialization with default parameters
- Basic neural dynamics simulation
- Memory system usage
- State management

**To Compile:**
```bash
mkdir build_basic
cd build_basic
cmake ../basic_brain.cpp -DCMAKE_BUILD_TYPE=Release
make
./basic_brain
```

### 2. Simple World Simulation
**File:** `simple_world.cpp`

**Description:** Creates a simple world environment and runs a basic simulation with the brain.

**Features:**
- World environment setup
- Agent movement simulation
- Sensorimotor integration
- Basic reinforcement learning

**To Compile:**
```bash
mkdir build_world
cd build_world
cmake ../simple_world.cpp -DCMAKE_BUILD_TYPE=Release
make
./simple_world
```

### 3. Memory System Example
**File:** `memory_system.cpp`

**Description:** Demonstrates working memory, episodic memory, and associative memory usage.

**Features:**
- Working memory encoding and retrieval
- Episodic memory storage and replay
- Associative memory pattern completion
- Memory consolidation

**To Compile:**
```bash
mkdir build_memory
cd build_memory
cmake ../memory_system.cpp -DCMAKE_BUILD_TYPE=Release
make
./memory_system
```

### 4. Plugin System Example
**File:** `plugin_example.cpp`

**Description:** Demonstrates plugin system usage with custom plugins.

**Features:**
- Plugin creation and registration
- Plugin lifecycle management
- Event-driven plugin functionality
- Plugin capabilities and configuration

**To Compile:**
```bash
mkdir build_plugin
cd build_plugin
cmake ../plugin_example.cpp -DCMAKE_BUILD_TYPE=Release
make
./plugin_example
```

## Python Examples

### 1. Basic Python Interface
**File:** `basic_python.py`

**Description:** Basic example of NLM Python interface usage.

**Features:**
- System initialization
- Brain and world creation
- Simulation execution
- State management
- Stimulus injection

**To Run:**
```bash
python basic_python.py
```

### 2. Advanced Python Example
**File:** `advanced_python.py`

**Description:** Advanced example with complex NLM operations.

**Features:**
- Custom configuration
- Event-driven programming
- Multiple experiments
- Visualization
- Data integration

**To Run:**
```bash
python advanced_python.py
```

## How to Use Examples

### C++ Examples

1. **Compile and Run**
   ```bash
   mkdir build_example
   cd build_example
   cmake ../example_file.cpp -DCMAKE_BUILD_TYPE=Release
   make
   ./example_file
   ```

2. **Modify Configuration**
   Edit the example file to adjust parameters like:
   - Number of neurons
   - Simulation duration
   - Time step
   - Brain configuration

### Python Examples

1. **Run Example**
   ```bash
   python example_file.py
   ```

2. **With Arguments**
   ```bash
   python example_file.py --config config.json --output output_dir
   ```

3. **With Environment Variables**
   ```bash
   NLM_LOG_LEVEL=debug python example_file.py
   ```

## Example Features

### Basic Features

1. **System Initialization**
   - Initialize NLM brain and world
   - Configure system parameters
   - Set up simulation environment

2. **Core Operations**
   - Run simulation loop
   - Inject stimuli
   - Monitor brain state
   - Update neural dynamics

3. **Memory Management**
   - Working memory operations
   - Episodic memory storage
   - Memory retrieval
   - Memory consolidation

4. **State Management**
   - Save brain state
   - Load brain state
   - Checkpoint management
   - State persistence

### Advanced Features

1. **Plugin System**
   - Create custom plugins
   - Load and manage plugins
   - Use plugin capabilities
   - Plugin lifecycle management

2. **Event System**
   - Emit events
   - Handle events
   - Event filtering
   - Event-driven programming

3. **Visualization**
   - Real-time monitoring
   - Neural activity plotting
   - Brain state visualization
   - Performance metrics

4. **Performance**
   - Benchmarking
   - Performance optimization
   - Profiling
   - Resource management

## Getting Started

### Prerequisites

1. **C++ Development**
   - g++ compiler
   - CMake
   - Make

2. **Python Development**
   - Python 3.8+
   - pip
   - numpy (optional)

### Installation

1. **Clone Repository**
   ```bash
   git clone https://github.com/nlm-project/nlm.git
   cd nlm
   ```

2. **Build C++ Components**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Install Python Package**
   ```bash
   cd ..
   pip install -e python/
   ```

### Running Examples

#### C++ Examples

```bash
# Navigate to examples directory
cd examples

# Compile and run each example
for example in *.cpp; do
    echo "Compiling $example..."
    mkdir build_$example
    cd build_$example
    cmake ../$example -DCMAKE_BUILD_TYPE=Release
    make
    ./${example%.cpp}
    cd ..
    rm -rf build_$example
    echo "Completed $example"
    echo

done
```

#### Python Examples

```bash
# Navigate to examples directory
cd examples

# Run each Python example
for script in *.py; do
    echo "Running $script..."
    python $script
    echo "Completed $script"
    echo

done
```

## Example Files

### C++ Examples

- **`basic_brain.cpp`**: Basic brain creation and operations
- **`simple_world.cpp`**: Simple world simulation
- **`memory_system.cpp`**: Memory system operations
- **`plugin_example.cpp`**: Plugin system usage

### Python Examples

- **`basic_python.py`**: Basic Python interface usage
- **`advanced_python.py`**: Advanced Python operations

### Configuration Files

- **`config.json`**: Example configuration
- **`*.ini`**: Configuration templates

### Data Files

- **`sample_data.json`**: Sample input data
- **`test_data.csv`**: Test data

## Example Code Structure

### Basic Template

```cpp
#include <nlm/brain.h>
#include <nlm/world.h>
#include <iostream>

int main() {
    try {
        // Initialize NLM system
        NLM::Brain brain;
        NLM::World world(100, 100);
        
        // Configure brain
        NLM::BrainConfig config;
        config.neuron_count = 100;
        config.synaptic_density = 0.1f;
        
        // Initialize brain
        brain.initialize(config);
        world.initialize();
        
        // Run simulation
        for (int i = 0; i < 100; ++i) {
            world.update(0.1f);
            brain.update_neural_dynamics(0.1f);
        }
        
        std::cout << "Example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Python Template

```python
import pynlm

def main():
    try:
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
        
        print("Example completed successfully!")
        return 0
        
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    main()
```

## Advanced Examples

### Plugin Development

This example demonstrates creating and using custom plugins:

```python
import pynlm

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

### Memory System

This example demonstrates working with different memory systems:

```python
import pynlm

# Working memory
working_memory = interface.get_working_memory()
item_id = working_memory.store([0.1f, 0.2f, 0.3f])
retrieved = working_memory.retrieve(item_id)

# Episodic memory
episodic_memory = interface.get_episodic_memory()
memory_id = episodic_memory.store(experience)
experiences = episodic_memory.retrieve(query)

# Associative memory
associative_memory = interface.get_associative_memory()
pattern = [0.5f, 0.5f, 0.5f]
completed = associative_memory.complete_pattern(pattern)
```

## Troubleshooting

### Common Issues

1. **Compilation Errors**
   - Check CMake version
   - Verify compiler compatibility
   - Check for missing dependencies

2. **Runtime Errors**
   - Check system configuration
   - Verify brain initialization
   - Monitor system resources

3. **Plugin Errors**
   - Check plugin permissions
   - Verify plugin configuration
   - Review plugin code

4. **Memory Issues**
   - Check memory limits
   - Verify memory allocation
   - Profile memory usage

### Debug Commands

```bash
# Enable debug mode
python example.py --debug

# Check logs
python example.py --log-level debug

# Profile performance
python -m cProfile -o profile.stats example.py
```

## Support

For additional support:

1. **Documentation**: https://docs.nlm.org
2. **GitHub Issues**: https://github.com/nlm-project/nlm/issues
3. **Community Forum**: https://community.nlm.org
4. **API Reference**: https://api.nlm.org

## License

This project is licensed under the MIT License.

All examples are provided under the same license as the main NLM project.

## Contact

For questions or feedback:

- Email: support@nlm.org
- GitHub: https://github.com/nlm-project/nlm/issues
- Slack: #nlm-examples

---

This directory contains basic examples for the NLM (Neural Learning Machine) system. These examples provide a foundation for understanding and working with the NLM system, from simple brain creation to advanced plugin development.

For more detailed information, please refer to the main NLM documentation and API reference.