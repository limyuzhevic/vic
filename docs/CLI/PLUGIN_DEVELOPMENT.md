# NLM CLI - Plugin Development Guide

## Overview

This document provides comprehensive information about NLM CLI plugin development. It covers plugin architecture, implementation, testing, distribution, and best practices for creating custom plugins that extend the NLM CLI functionality.

## Plugin Architecture

### Plugin System Overview

The NLM CLI plugin system provides a flexible architecture for extending functionality through plugins. Plugins can:

1. **Add new commands**: Extend the CLI with custom commands
2. **Extend existing commands**: Modify or enhance existing commands
3. **Add event handlers**: Respond to system events
4. **Provide configuration**: Add configuration parameters
5. **Export data**: Handle data export in new formats
6. **Integrate external tools**: Connect with external systems

### Plugin Interface

#### Core Plugin Interface

```cpp
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getEmail() const = 0;
    
    // Lifecycle management
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual void shutdown() = 0;
    
    // Command management
    virtual std::vector<std::string> getCommands() const = 0;
    virtual bool handleCommand(const std::string& command, const std::vector<std::string>& args) = 0;
    
    // Event handling
    virtual bool handleEvent(const std::string& eventType, const std::any& eventData) = 0;
    
    // Configuration
    virtual bool configure(const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getConfiguration() const = 0;
    
    // Dependency management
    virtual std::vector<std::string> getRequiredPlugins() const = 0;
    virtual std::vector<std::string> getOptionalPlugins() const = 0;
};
```

#### Plugin Manifest

Plugins can include a manifest file describing their capabilities:

```json
{
    "name": "example_plugin",
    "version": "1.0.0",
    "description": "Example plugin for NLM CLI",
    "author": "Example Author",
    "email": "author@example.com",
    "license": "MIT",
    "dependencies": ["config_manager"],
    "commands": ["example", "test"],
    "events": ["simulation_started", "simulation_ended"],
    "configuration": {
        "example_param": {
            "type": "string",
            "default": "default_value",
            "description": "Example parameter"
        }
    }
}
```

### Plugin Loading and Management

#### Plugin Loading Process

1. **Discovery**: Scan plugin directories for plugin files
2. **Validation**: Validate plugin manifest and dependencies
3. **Loading**: Load plugin libraries and initialize plugins
4. **Registration**: Register plugin commands and event handlers
5. **Configuration**: Apply plugin configuration
6. **Initialization**: Call plugin initialization methods

#### Plugin Management Commands

```bash
# List available plugins
nlm plugin list

# Enable a plugin
nlm plugin enable <plugin_name>

# Disable a plugin
nlm plugin disable <plugin_name>

# Load a plugin
nlm plugin load <plugin_path>

# Unload a plugin
nlm plugin unload <plugin_name>

# Get plugin information
nlm plugin info <plugin_name>

# Check plugin dependencies
nlm plugin dependencies <plugin_name>
```

## Creating a Plugin

### Plugin Structure

A typical plugin has the following structure:

```
plugins/
├── example_plugin/
│   ├── include/
│   │   └── plugin.hpp               # Plugin header
│   ├── src/
│   │   └── plugin.cpp               # Plugin implementation
│   ├── resources/
│   │   └── manifest.json            # Plugin manifest
│   ├── config/
│   │   └── schema.json              # Configuration schema
│   ├── docs/
│   │   └── README.md                # Plugin documentation
│   ├── tests/
│   │   ├── test_plugin.cpp          # Plugin tests
│   │   └── test_integration.cpp     # Integration tests
│   ├── examples/
│   │   └── example.cpp              # Usage examples
│   └── build/
│       └── CMakeLists.txt          # Build configuration
```

### Basic Plugin Implementation

#### Plugin Header

```cpp
// plugins/example_plugin/include/plugin.hpp

#pragma once

#include "cli/NLMCLI.hpp"
#include <string>
#include <vector>
#include <any>
#include <map>

class ExamplePlugin : public IPlugin {
public:
    ExamplePlugin();
    ~ExamplePlugin() override;
    
    // Identification
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    std::string getEmail() const override;
    
    // Lifecycle management
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    
    // Command management
    std::vector<std::string> getCommands() const override;
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override;
    
    // Event handling
    bool handleEvent(const std::string& eventType, const std::any& eventData) override;
    
    // Configuration
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfiguration() const override;
    
    // Dependency management
    std::vector<std::string> getRequiredPlugins() const override;
    std::vector<std::string> getOptionalPlugins() const override;
    
private:
    // Plugin-specific data
    std::map<std::string, std::any> configuration_;
    bool initialized_;
};
```

#### Plugin Implementation

```cpp
// plugins/example_plugin/src/plugin.cpp

#include "plugin.hpp"
#include <iostream>
#include <fstream>

ExamplePlugin::ExamplePlugin() : initialized_(false) {}

ExamplePlugin::~ExamplePlugin() {
    shutdown();
}

std::string ExamplePlugin::getName() const {
    return "example_plugin";
}

std::string ExamplePlugin::getVersion() const {
    return "1.0.0";
}

std::string ExamplePlugin::getDescription() const {
    return "Example plugin for NLM CLI demonstration";
}

std::string ExamplePlugin::getAuthor() const {
    return "NLM Project";
}

std::string ExamplePlugin::getEmail() const {
    return "dev@nlm-project.org";
}

bool ExamplePlugin::initialize(const std::map<std::string, std::any>& config) {
    if (initialized_) {
        return true;
    }
    
    configuration_ = config;
    
    // Initialize plugin-specific resources
    // Load configuration files
    // Register event handlers
    
    std::cout << "Initializing Example Plugin..." << std::endl;
    initialized_ = true;
    return true;
}

void ExamplePlugin::shutdown() {
    if (!initialized_) {
        return;
    }
    
    // Clean up plugin-specific resources
    // Unregister event handlers
    // Save state
    
    std::cout << "Shutting down Example Plugin..." << std::endl;
    initialized_ = false;
    configuration_.clear();
}

std::vector<std::string> ExamplePlugin::getCommands() const {
    return {"example", "test", "analyze"};
}

bool ExamplePlugin::handleCommand(const std::string& command, const std::vector<std::string>& args) {
    if (!initialized_) {
        std::cerr << "Plugin not initialized" << std::endl;
        return false;
    }
    
    if (command == "example") {
        return handleExampleCommand(args);
    } else if (command == "test") {
        return handleTestCommand(args);
    } else if (command == "analyze") {
        return handleAnalyzeCommand(args);
    }
    
    return false;
}

bool ExamplePlugin::handleEvent(const std::string& eventType, const std::any& eventData) {
    // Handle plugin-specific events
    if (eventType == "simulation_started") {
        return onSimulationStarted(eventData);
    } else if (eventType == "simulation_ended") {
        return onSimulationEnded(eventData);
    }
    
    return false;
}

bool ExamplePlugin::configure(const std::map<std::string, std::any>& config) {
    // Validate configuration
    // Apply configuration
    configuration_ = config;
    return true;
}

std::map<std::string, std::any> ExamplePlugin::getConfiguration() const {
    return configuration_;
}

std::vector<std::string> ExamplePlugin::getRequiredPlugins() const {
    return {"config_manager", "performance_monitor"};
}

std::vector<std::string> ExamplePlugin::getOptionalPlugins() const {
    return {"export_manager", "checkpoint_manager"};
}

// Plugin-specific command handlers
bool ExamplePlugin::handleExampleCommand(const std::vector<std::string>& args) {
    std::cout << "Example command executed" << std::endl;
    if (!args.empty()) {
        std::cout << "Arguments: ";
        for (const auto& arg : args) {
            std::cout << arg << " ";
        }
        std::cout << std::endl;
    }
    return true;
}

bool ExamplePlugin::handleTestCommand(const std::vector<std::string>& args) {
    std::cout << "Test command executed" << std::endl;
    // Run plugin tests
    return runTests();
}

bool ExamplePlugin::handleAnalyzeCommand(const std::vector<std::string>& args) {
    std::cout << "Analyze command executed" << std::endl;
    // Perform analysis
    return performAnalysis();
}

// Event handlers
bool ExamplePlugin::onSimulationStarted(const std::any& eventData) {
    std::cout << "Simulation started event received" << std::endl;
    // Plugin-specific initialization for simulation
    return true;
}

bool ExamplePlugin::onSimulationEnded(const std::any& eventData) {
    std::cout << "Simulation ended event received" << std::endl;
    // Plugin-specific cleanup for simulation
    return true;
}

// Plugin-specific methods
bool ExamplePlugin::runTests() {
    std::cout << "Running plugin tests..." << std::endl;
    // Implement tests
    return true;
}

bool ExamplePlugin::performAnalysis() {
    std::cout << "Performing analysis..." << std::endl;
    // Implement analysis
    return true;
}
```

### Plugin Configuration

#### Configuration Schema

Plugins can define configuration schemas:

```json
// plugins/example_plugin/config/schema.json
{
    "type": "object",
    "properties": {
        "example_param": {
            "type": "string",
            "description": "Example parameter",
            "default": "default_value"
        },
        "example_number": {
            "type": "number",
            "description": "Example number parameter",
            "default": 1.0,
            "minimum": 0.0,
            "maximum": 100.0
        },
        "example_boolean": {
            "type": "boolean",
            "description": "Example boolean parameter",
            "default": false
        },
        "example_array": {
            "type": "array",
            "description": "Example array parameter",
            "default": [],
            "items": {
                "type": "string"
            }
        }
    },
    "required": ["example_param"]
}
```

#### Configuration Management

```cpp
// Plugin configuration management
struct PluginConfiguration {
    std::string name;
    std::any value;
    std::string description;
    bool required;
    std::string validation_rule;
};

class PluginConfigurationManager {
public:
    bool loadConfiguration(const std::string& pluginName, const std::string& filepath);
    bool saveConfiguration(const std::string& pluginName, const std::string& filepath);
    bool validateConfiguration(const std::string& pluginName);
    std::map<std::string, std::any> getConfiguration(const std::string& pluginName) const;
    bool setConfiguration(const std::string& pluginName, const std::map<std::string, std::any>& config);
};
```

### Plugin Events

#### Event System

Plugins can handle events:

```cpp
// Event types
const std::vector<std::string> EVENT_TYPES = {
    "simulation_started",
    "simulation_ended",
    "checkpoint_created",
    "checkpoint_loaded",
    "simulation_step",
    "command_executed",
    "plugin_loaded",
    "plugin_unloaded",
    "configuration_changed",
    "performance_update"
};

// Event data
struct EventData {
    std::string type;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::any> data;
    std::string source;
};
```

#### Event Handling

```cpp
bool ExamplePlugin::handleEvent(const std::string& eventType, const std::any& eventData) {
    auto* event = std::any_cast<EventData*>(&eventData);
    if (!event) {
        return false;
    }
    
    if (event->type == "simulation_started") {
        // Handle simulation start
        return onSimulationStarted(event->data);
    } else if (event->type == "checkpoint_created") {
        // Handle checkpoint creation
        return onCheckpointCreated(event->data);
    }
    
    return false;
}
```

## Plugin Testing

### Unit Testing

#### Test Framework

Plugins should be thoroughly tested:

```cpp
// plugins/example_plugin/tests/test_plugin.cpp

#include <gtest/gtest.h>
#include "plugin.hpp"

class ExamplePluginTest : public ::testing::Test {
protected:
    void SetUp() override {
        plugin_ = std::make_unique<ExamplePlugin>();
        config_ = {
            {"example_param", "test_value"},
            {"example_number", 42.0},
            {"example_boolean", true}
        };
    }
    
    void TearDown() override {
        plugin_->shutdown();
    }
    
    std::unique_ptr<ExamplePlugin> plugin_;
    std::map<std::string, std::any> config_;
};

TEST_F(ExamplePluginTest, Initialization) {
    EXPECT_TRUE(plugin_->initialize(config_));
    EXPECT_TRUE(plugin_->initialized_);
}

TEST_F(ExamplePluginTest, Configuration) {
    plugin_->initialize(config_);
    auto retrievedConfig = plugin_->getConfiguration();
    EXPECT_EQ(std::any_cast<std::string>(retrievedConfig["example_param"]), "test_value");
}

TEST_F(ExamplePluginTest, CommandHandling) {
    plugin_->initialize(config_);
    EXPECT_TRUE(plugin_->handleCommand("example", {"arg1", "arg2"}));
}
```

#### Integration Testing

```cpp
// plugins/example_plugin/tests/test_integration.cpp

#include <gtest/gtest.h>
#include "plugin.hpp"
#include "NLMCLI.hpp"

class ExamplePluginIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize NLM CLI
        // Load plugin
        // Set up test environment
    }
    
    void TearDown() override {
        // Clean up test environment
    }
};

TEST_F(ExamplePluginIntegrationTest, PluginLoading) {
    // Test plugin loading
    EXPECT_TRUE(NLMCLI::getPluginManager()->loadPlugin("example_plugin"));
}

TEST_F(ExamplePluginIntegrationTest, CommandExecution) {
    // Test command execution through plugin
    std::vector<std::string> args = {"arg1", "arg2"};
    EXPECT_TRUE(NLMCLI::executeCommand("example", args));
}
```

### Performance Testing

#### Performance Benchmarks

```cpp
// plugins/example_plugin/tests/performance_test.cpp

#include <benchmark/benchmark.h>
#include "plugin.hpp"

static void PluginCommandPerformance(benchmark::State& state) {
    ExamplePlugin plugin;
    std::map<std::string, std::any> config = {
        {"example_param", "test_value"}
    };
    
    plugin.initialize(config);
    
    for (auto _ : state) {
        plugin.handleCommand("example", {"test", "data"});
    }
}

BENCHMARK(PluginCommandPerformance);

BENCHMARK_MAIN();
```

## Plugin Distribution

### Package Management

#### Plugin Packaging

Create plugin packages with:

```bash
# Create plugin package
declare_plugin_package(
    NAME example_plugin
    VERSION 1.0.0
    DESCRIPTION "Example plugin for NLM CLI"
    AUTHOR "NLM Project"
    LICENSE "MIT"
    FILES
        plugin.hpp
        plugin.cpp
        manifest.json
        config/schema.json
    DEPENDENCIES
        config_manager
        performance_monitor
)
```

#### Distribution Formats

- **Binary packages**: Compiled plugins for different platforms
- **Source packages**: Plugin source code for building
- **Container images**: Plugin container images
- **NPM packages**: For JavaScript plugins
- **NuGet packages**: For .NET plugins

### Plugin Installation

#### Installation Commands

```bash
# Install from package manager
nlm plugin install example_plugin

# Install from file
nlm plugin load ./plugins/example_plugin/plugin.so

# Install from git repository
nlm plugin install git+https://github.com/nlm-project/example-plugin.git

# Install with configuration
nlm plugin install example_plugin --config config.json
```

#### Installation Process

1. **Download**: Download plugin package
2. **Validate**: Validate plugin manifest and dependencies
3. **Install**: Install plugin files
4. **Configure**: Apply plugin configuration
5. **Initialize**: Initialize plugin
6. **Register**: Register plugin commands and event handlers

## Plugin Development Best Practices

### 1. Plugin Architecture

#### Separation of Concerns

- **Plugin Core**: Core plugin functionality
- **Command Handlers**: Command processing logic
- **Event Handlers**: Event handling logic
- **Configuration**: Configuration management
- **Data Management**: Data storage and retrieval

#### Modularity

- Keep plugin code modular and maintainable
- Use clear interfaces between components
- Implement proper error handling
- Use dependency injection for better testability

### 2. Plugin Quality

#### Code Quality

- Follow C++ coding standards
- Implement comprehensive error handling
- Write clean, readable code
- Add appropriate comments and documentation

#### Documentation

- Document plugin API
- Provide usage examples
- Document configuration parameters
- Include troubleshooting information

#### Testing

- Implement unit tests
- Implement integration tests
- Perform performance testing
- Test with different configurations

### 3. Plugin Security

#### Security Best Practices

- Validate all input
- Use proper error handling
- Implement access control
- Log plugin activities
- Use secure plugin loading

#### Security Measures

```cpp
// Input validation
bool validateInput(const std::string& input) {
    // Check for malicious content
    // Validate input format
    // Check for injection attempts
}

// Access control
bool checkAccess(const std::string& command, const std::vector<std::string>& args) {
    // Check user permissions
    // Verify command authorization
    // Log access attempt
}
```

### 4. Plugin Performance

#### Performance Optimization

- Optimize plugin initialization
- Use efficient data structures
- Implement lazy loading
- Minimize memory usage
- Use appropriate concurrency patterns

#### Performance Monitoring

```cpp
// Performance monitoring
struct PluginPerformanceMetrics {
    double initialization_time;
    double command_execution_time;
    double memory_usage;
    double cpu_usage;
    size_t command_count;
    size_t error_count;
};

class PluginPerformanceMonitor {
public:
    void recordCommandExecution(const std::string& command, double executionTime);
    void recordError(const std::string& error);
    PluginPerformanceMetrics getMetrics() const;
    void resetMetrics();
};
```

### 5. Plugin Compatibility

#### Version Compatibility

- Support multiple NLM CLI versions
- Implement version detection
- Handle version conflicts
- Provide backward compatibility

#### Platform Compatibility

- Support multiple platforms
- Implement platform detection
- Use platform-independent APIs
- Handle platform-specific issues

### 6. Plugin Documentation

#### API Documentation

Generate comprehensive API documentation:

```bash
# Generate API documentation
generate_api_docs(
    INPUT plugin.hpp plugin.cpp
    OUTPUT docs/api/plugin.md
    FORMAT markdown
    TEMPLATE doxygen
)
```

#### User Documentation

Create user-friendly documentation:

```bash
# Create user documentation
create_documentation(
    NAME "Example Plugin User Guide"
    SOURCE plugin.cpp
    OUTPUT docs/user_guide/example_plugin.md
    TYPE markdown
)
```

## Plugin Examples

### Example 1: Export Plugin

#### Plugin Structure

```cpp
// plugins/export_plugin/include/export_plugin.hpp

#pragma once

#include "cli/NLMCLI.hpp"
#include <string>
#include <vector>
#include <any>
#include <map>

class ExportPlugin : public IPlugin {
public:
    ExportPlugin();
    ~ExportPlugin() override;
    
    // IPlugin interface
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    std::string getEmail() const override;
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    
    std::vector<std::string> getCommands() const override;
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override;
    
    bool handleEvent(const std::string& eventType, const std::any& eventData) override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfiguration() const override;
    
    std::vector<std::string> getRequiredPlugins() const override;
    std::vector<std::string> getOptionalPlugins() const override;
    
private:
    // Export functionality
    bool exportData(const std::string& type, const std::string& format, const std::string& path);
    bool exportConfiguration(const std::string& format, const std::string& path);
    bool exportResults(const std::string& format, const std::string& path);
    
    std::map<std::string, std::any> configuration_;
    bool initialized_;
};
```

#### Export Commands

```cpp
// Plugin command handlers
bool ExportPlugin::handleCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "export") {
        return handleExportCommand(args);
    } else if (command == "format") {
        return handleFormatCommand(args);
    } else if (command == "configure") {
        return handleConfigureCommand(args);
    }
    
    return false;
}

bool ExportPlugin::handleExportCommand(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cerr << "Usage: export <type> <format> <path>" << std::endl;
        return false;
    }
    
    std::string type = args[0];
    std::string format = args[1];
    std::string path = args[2];
    
    return exportData(type, format, path);
}
```

### Example 2: Analysis Plugin

#### Plugin Structure

```cpp
// plugins/analysis_plugin/include/analysis_plugin.hpp

#pragma once

#include "cli/NLMCLI.hpp"
#include <string>
#include <vector>
#include <any>
#include <map>
#include <chrono>

class AnalysisPlugin : public IPlugin {
public:
    AnalysisPlugin();
    ~AnalysisPlugin() override;
    
    // IPlugin interface
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    std::string getEmail() const override;
    
    bool initialize(const std::map<std::string, std::any>& config) override;
    void shutdown() override;
    
    std::vector<std::string> getCommands() const override;
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override;
    
    bool handleEvent(const std::string& eventType, const std::any& eventData) override;
    
    bool configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfiguration() const override;
    
    std::vector<std::string> getRequiredPlugins() const override;
    std::vector<std::string> getOptionalPlugins() const override;
    
private:
    // Analysis functionality
    bool analyzeSimulation(const std::string& outputPath);
    bool analyzePerformance(const std::string& outputPath);
    bool analyzeNeurons(const std::string& outputPath);
    bool analyzeSynapses(const std::string& outputPath);
    
    std::map<std::string, std::any> configuration_;
    bool initialized_;
    std::chrono::system_clock::time_point lastAnalysisTime_;
};
```

#### Analysis Commands

```cpp
// Analysis command handlers
bool AnalysisPlugin::handleCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "analyze") {
        return handleAnalyzeCommand(args);
    } else if (command == "profile") {
        return handleProfileCommand(args);
    } else if (command == "metrics") {
        return handleMetricsCommand(args);
    }
    
    return false;
}

bool AnalysisPlugin::handleAnalyzeCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: analyze <output_path> [options]" << std::endl;
        return false;
    }
    
    std::string outputPath = args[0];
    std::string options = args.size() > 1 ? args[1] : "";
    
    return analyzeSimulation(outputPath);
}
```

## Plugin Development Workflow

### 1. Planning

- Define plugin requirements
- Design plugin architecture
- Create plugin manifest
- Plan testing strategy

### 2. Implementation

- Write plugin code
- Implement command handlers
- Implement event handlers
- Test plugin functionality

### 3. Testing

- Write unit tests
- Write integration tests
- Perform performance testing
- Test with different configurations

### 4. Documentation

- Document plugin API
- Write user documentation
- Create examples
- Update plugin manifest

### 5. Distribution

- Package plugin
- Test installation
- Create documentation
- Publish plugin

## Plugin Development Tips

### Getting Started

1. **Start simple**: Begin with a basic plugin
2. **Use existing plugins**: Study and extend existing plugins
3. **Follow patterns**: Follow established patterns and conventions
4. **Test thoroughly**: Test your plugin thoroughly
5. **Document well**: Document your plugin well

### Common Pitfalls

1. **Memory leaks**: Be careful with memory management
2. **Thread safety**: Consider thread safety
3. **Error handling**: Implement proper error handling
4. **Configuration validation**: Validate configuration
5. **Plugin dependencies**: Handle plugin dependencies

### Best Practices

1. **Use modern C++**: Use modern C++ features
2. **Follow coding standards**: Follow coding standards
3. **Write clean code**: Write clean, readable code
4. **Document thoroughly**: Document your code
5. **Test regularly**: Test your code regularly

## Plugin Support

### Getting Help

- **Documentation**: Check plugin documentation
- **Examples**: Look at plugin examples
- **Community**: Join plugin community
- **Issues**: Report issues
- **Contributing**: Contribute to plugins

### Plugin Development Resources

- **NLM CLI Documentation**: https://nlm.readthedocs.io/cli
- **Plugin Development Guide**: https://github.com/nlm-project/plugin-dev
- **Plugin Examples**: https://github.com/nlm-project/plugin-examples
- **Plugin Community**: https://github.com/nlm-project/plugin-community
- **Contributing Guide**: https://github.com/nlm-project/nlm/blob/main/CONTRIBUTING.md

## Plugin License

This plugin development guide is part of the NLM (熙然) Neural Learning Machine project. It is licensed under the MIT License.

## Plugin Copyright

© 2024 NLM Project. All rights reserved.
