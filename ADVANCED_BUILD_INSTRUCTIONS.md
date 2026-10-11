# Advanced CLI and Plugin System Build Instructions

## Build Requirements

### Prerequisites

1. **CMake 3.16 or higher**
2. **C++20 compiler** (GCC 9+, Clang 10+, MSVC 19.2+)
3. **nlohmann/json library**
4. **yaml-cpp library**
5. **Python 3.6+** (for Python bindings, optional)

### Quick Build

#### Step 1: Clone the repository

```bash
git clone <repository-url>
cd <project-directory>
```

#### Step 2: Configure build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### Step 3: Build

```bash
cmake --build . -- -j$(nproc)
```

#### Step 4: Run tests (optional)

```bash
ctest --output-on-failure
```

## Building Advanced Components

The advanced CLI and plugin system is automatically included when building the main project. No additional build steps are required.

### Manual Build

To build just the advanced components:

```bash
cd build
cmake .. -DBUILD_ADVANCED_CLI=ON
cmake --build . --target nlm_advanced_cli
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_ADVANCED_CLI` | Build advanced CLI and plugin system | ON |
| `NLM_BUILD_PYTHON_BINDINGS` | Build Python bindings | ON |
| `CMAKE_BUILD_TYPE` | Build type (Debug, Release, RelWithDebInfo) | Release |

## Running Advanced CLI

### Basic Usage

```bash
# Interactive mode
./nlm_advanced_cli

# Batch mode
./nlm_advanced_cli batch my_script.txt

# Run with help
./nlm_advanced_cli help
```

### Example Commands

```bash
# Start interactive session
./nlm_advanced_cli

# Save brain state
> save my_brain_state.json

# Load brain state
> load my_brain_state.json

# Run experiment
> run plasticity --steps 1000 --learning-rate 0.05

# List plugins
> plugins

# Load plugin
> load-plugin CustomPlasticityPlugin

# Run batch script
./nlm_advanced_cli batch demo_script.txt
```

## Plugin Development

### Creating a New Plugin

1. **Create Plugin Class**: Inherit from `nlm::cli::Plugin`
2. **Implement Required Methods**: Follow the plugin interface
3. **Create Factory Function**: Export `createPlugin()` function
4. **Build Plugin**: Add to CMakeLists.txt
5. **Test Plugin**: Verify integration
6. **Package**: Create distribution package

### Plugin Example

```cpp
// Example: MyCustomPlugin.cpp
#include "nlm/cli/Plugin.hpp"
#include <string>

class MyCustomPlugin : public nlm::cli::Plugin {
public:
    const std::string& getName() const override {
        static const std::string name = "MyCustomPlugin";
        return name;
    }
    
    const std::string& getVersion() const override {
        static const std::string version = "1.0.0";
        return version;
    }
    
    // Implement other required methods...
    
    bool initialize(std::shared_ptr<PluginManager> manager, 
                   std::shared_ptr<Config> config) override {
        // Initialize plugin
        pluginManager_ = manager;
        config_ = config;
        return true;
    }
    
    void update(TimestepDuration dt) override {
        // Update plugin state
    }
    
    bool onPluginMessage(const std::string& message, 
                        const std::vector<std::string>& data) override {
        // Handle plugin messages
        return true;
    }
};

extern "C" {
    std::shared_ptr<nlm::cli::Plugin> createPlugin() {
        return std::make_shared<MyCustomPlugin>();
    }
}
```

### Plugin Build Requirements

Each plugin library must:

1. **Export `createPlugin()` function**:
   ```cpp
   std::shared_ptr<nlm::cli::Plugin> createPlugin() {
       return std::make_shared<MyPlugin>();
   }
   ```

2. **Be built as a shared library**:
   ```cmake
   add_library(MyPlugin SHARED src/plugins/MyPlugin.cpp)
   target_link_libraries(MyPlugin PUBLIC nlm_core)
   ```

3. **Be placed in plugin directory**: `./plugins/` or specified via `--plugin-dir`

## Configuration

### Advanced Configuration File

Create `advanced_config.yaml`:

```yaml
nlm:
  cli:
    interactive_mode: true
    auto_complete: true
    command_history: true
    help_format: "detailed"
    completion_delay: 0.1
    
  plugins:
    directory: "./plugins"
    auto_load: ["CustomPlasticityPlugin", "CustomNeuromodulatorPlugin"]
    plugin_path: "./external_plugins/"
    
  performance:
    profiling_enabled: true
    monitoring_interval: 1.0
    max_memory_mb: 2048
    
  experiments:
    default_steps: 1000
    default_learning_rate: 0.05
    enable_plasticity: true
    enable_structural: false
```

### Command Line Configuration

```bash
# Override configuration values
./nlm_advanced_cli \
  --config custom_config.yaml \
  --plugin-dir ./external_plugins \
  --auto-load CustomPlasticityPlugin,CustomSensoryPlugin \
  --performance-monitoring \
  --profiling-output profile.json
```

## Plugin Examples

### 1. CustomPlasticityPlugin

**Type**: Plasticity Plugin  
**Location**: `src/plugins/CustomPlasticityPlugin.cpp`  
**Features**:
- Adaptive learning rates based on network activity
- Custom STDP variant with sigmoidal modulation
- Hebbian component for co-activation
- Neuromodulator integration

### 2. CustomNeuromodulatorPlugin

**Type**: Neuromodulator Plugin  
**Location**: `src/plugins/CustomNeuromodulatorPlugin.cpp`  
**Features**:
- Adaptive dopamine regulation
- Novelty detection and gain modulation
- Curiosity-driven exploration
- Action value learning

### 3. CustomSensoryPlugin

**Type**: Sensory Plugin  
**Location**: `src/plugins/CustomSensoryPlugin.cpp`  
**Features**:
- Multimodal sensory fusion
- Feature extraction and analysis
- Selective attention mechanism
- Adaptive thresholding

## Performance Optimization

### Profiling and Optimization

1. **Enable profiling**: `./nlm_advanced_cli profile start`
2. **Monitor performance**: `./nlm_advanced_cli stats`
3. **Analyze bottlenecks**: `./nlm_advanced_cli analyze performance`
4. **Optimize plugins**: Use plugin-specific optimization flags

### Performance Monitoring Commands

```bash
# Start profiling
> profile start --output profile_data.json

# Show current performance
> stats --cpu --memory --network

# Analyze performance
> analyze performance --time-range 24h --output summary.pdf

# Compare configurations
> compare-performance old_config new_config
```

## Testing

### Running Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R test_plugin -V

# Run advanced CLI tests
ctest -R advanced_cli -V
```

### Plugin Testing

Test each plugin individually:

```bash
# Test CustomPlasticityPlugin
./test_plugin CustomPlasticityPlugin

# Test CustomNeuromodulatorPlugin
./test_plugin CustomNeuromodulatorPlugin

# Test CustomSensoryPlugin
./test_plugin CustomSensoryPlugin
```

### Integration Testing

```bash
# Test plugin loading and unloading
./integration_test --plugin-dir ./test_plugins

# Test batch processing
./integration_test --batch test_script.txt

# Test error handling
./integration_test --error-simulation
```

## Troubleshooting

### Common Issues and Solutions

#### Plugin Loading Failures

**Symptom**: Plugin fails to load with error "Plugin not found: CustomPlugin"

**Solutions**:
1. Verify plugin library exists: `./plugins/CustomPlugin.so`
2. Check dependencies: `ldd ./plugins/CustomPlugin.so`
3. Ensure plugin is built with correct architecture
4. Check plugin compatibility with current NLM version

#### Command Line Errors

**Symptom**: "Unknown command: xyz"

**Solutions**:
1. Check command spelling
2. Use `help` to list available commands
3. Check for case sensitivity
4. Verify CLI version compatibility

#### Performance Issues

**Symptom**: High CPU usage or slow response times

**Solutions**:
1. Enable performance profiling
2. Check for memory leaks
3. Optimize plugin code
4. Reduce simulation timestep
5. Check for bottlenecks in plugin communication

### Error Messages

| Error | Description | Solution |
|-------|-------------|----------|
| PLUGIN_LOAD_FAILED | Failed to load plugin | Check file path and dependencies |
| BRAIN_SAVE_FAILED | Failed to save brain state | Check disk space and permissions |
| EXPERIMENT_NOT_FOUND | Experiment doesn't exist | Use `help experiments` |
| PLUGIN_DEPENDENCY_MISSING | Required plugin not loaded | Load required plugins first |

## Documentation

### Generated Documentation

The advanced CLI and plugin system includes comprehensive documentation:

1. **API Documentation**: Generated from header files
2. **User Guide**: `docs/ADVANCED_USAGE.md`
3. **Plugin Reference**: Plugin-specific documentation
4. **Configuration Guide**: Configuration file examples
5. **Tutorial**: Step-by-step usage examples

### Viewing Documentation

```bash
# View advanced usage guide
less docs/ADVANCED_USAGE.md

# View API documentation
python -m pydoc nlm.cli
# or
doxygen docs/api_reference/html/index.html
```

## Support and Contributions

### Support

For issues and questions:

1. **GitHub Issues**: Submit issues at `https://github.com/nlm/nlm/issues`
2. **Community Forum**: Discuss on `https://community.nlm.org`
3. **Documentation**: Check `docs/` directory for guides
4. **Examples**: Look at `examples/` directory for usage examples

### Contributing Plugins

1. **Fork the repository**
2. **Create plugin** following guidelines
3. **Test thoroughly**
4. **Submit pull request**
5. **Documentation and examples**

### Plugin Standards

Plugins should follow these standards:

1. **Naming**: Use clear, descriptive names
2. **Versioning**: Use semantic versioning
3. **Documentation**: Include comprehensive documentation
4. **Error Handling**: Implement robust error handling
5. **Performance**: Optimize for performance
6. **Compatibility**: Ensure compatibility with all supported NLM versions

## Conclusion

The advanced CLI and plugin system provides a comprehensive framework for:

- **Extending NLM functionality** through plugins
- **Automating complex experiments** with batch processing
- **Monitoring and optimizing performance** with advanced profiling
- **Managing brain states** for reproducibility
- **Customizing neural computation** with plugin-specific configurations

This system enables power users to build sophisticated NLM applications while maintaining backward compatibility and ease of use.

For more information, visit the NLM documentation and contribute to making NLM more powerful and extensible!
