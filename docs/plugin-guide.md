# NLM Plugin Guide

This guide provides comprehensive plugin documentation for NLM (Neural Learning Machine). It covers:

- **Plugin Overview**: Plugin system overview and architecture
- **Plugin Architecture**: Plugin architecture and design
- **Plugin Development**: Plugin development and implementation
- **Plugin Usage**: Plugin usage and integration
- **Plugin Examples**: Plugin examples and use cases
- **Plugin Testing**: Plugin testing and validation
- **Plugin Documentation**: Plugin documentation
- **Plugin Resources**: Plugin resources and tools

## 🎯 Why Plugins Matter

Plugins are crucial for NLM because:

1. **Extensibility**: Allow extending NLM functionality without modifying core code
2. **Customization**: Enable customization of NLM for specific use cases
3. **Integration**: Integrate NLM with other systems and tools
4. **Performance**: Optimize performance by adding specialized plugins
5. **Debugging**: Add debugging and monitoring plugins
6. **Visualization**: Add visualization plugins for better understanding
7. **Analysis**: Add analysis plugins for deeper insights
8. **Simulation**: Add simulation plugins for different scenarios

## 🏗️ Plugin Overview

### Plugin System Overview

NLM plugin system provides a framework for extending NLM functionality:

1. **Plugin Interface**: Abstract interface for all plugins
2. **Plugin Factory**: Factory for creating plugins
3. **Plugin Registry**: Registry for managing plugins
4. **Plugin Loader**: Loader for loading plugins
5. **Plugin Manager**: Manager for managing plugins

### Plugin Architecture

NLM plugin system architecture:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Core                                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────┐    ┌─────────────────────┐              │
│  │     Plugin System   │───▶│       NLM Brain     │              │
│  │                   │    │                   │              │
│  │ ┌─────────────────┐ │    │ ┌─────────────────┐ │              │
│  │ │  Plugin Factory │ │───▶│   Brain Interface │ │              │
│  │ └─────────────────┘ │    │ └─────────────────┘ │              │
│  │   ┌───────────────┐  │    │                    │              │
│  │   │ Plugin Registry│ │───▶│   Neural Dynamics  │              │
│  │   └───────────────┘  │    │                   │              │
│  │                      │    │ ┌─────────────────┐ │              │
│  │ ┌─────────────────┐  │    │ │  Memory System  │ │              │
│  │ │ Plugin Loader   │ │───▶│                   │ │              │
│  │ └─────────────────┘  │    │ │ ┌───────────────┐│ │              │
│  │                      │    │ ││   Working     ││ │              │
│  │ ┌─────────────────┐  │    │ ││   Memory      ││ │              │
│  │ │ Plugin Manager  │ │───▶││   System       ││ │              │
│  │ └─────────────────┘  │    │ └───────────────┘│ │              │
│  │                      │    │                  │              │
│  │ ┌─────────────────┐  │    │ ┌─────────────────┐ │              │
│  │ │   Event System  │ │───▶│   Action System │ │              │
│  │ └─────────────────┘  │    │ └─────────────────┘ │              │
│  └─────────────────────────────────────────────────────────────┘
└─────────────────────────────────────────────────────────────────┘
```

### Plugin System Components

Plugin system components:

1. **Plugin Interface**: Abstract interface defining plugin API
2. **Plugin Factory**: Factory for creating plugins
3. **Plugin Registry**: Registry for managing plugins
4. **Plugin Loader**: Loader for loading plugins from files
5. **Plugin Manager**: Manager for managing plugin lifecycle

## 🏗️ Plugin Architecture

### Plugin Interface

Abstract interface for all plugins:

```cpp
// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getContact() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    
    // Plugin operations
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    
    // Plugin-specific operations
    virtual void onEvent(const Event& event) = 0;
    virtual void onMessage(const std::string& message) = 0;
};
```

### Plugin Factory

Factory for creating plugins:

```cpp
// Plugin factory
class PluginFactory {
public:
    static std::unique_ptr<IPlugin> createPlugin(const std::string& name);
    static std::vector<std::string> getAvailablePlugins();
    static std::string getPluginVersion(const std::string& name);
    static std::string getPluginDescription(const std::string& name);
    static std::string getPluginAuthor(const std::string& name);
    static std::string getPluginContact(const std::string& name);
};
```

### Plugin Registry

Registry for managing plugins:

```cpp
// Plugin registry
class PluginRegistry {
public:
    static PluginRegistry& getInstance();
    
    // Register plugin
    void registerPlugin(std::shared_ptr<IPlugin> plugin);
    
    // Unregister plugin
    void unregisterPlugin(const std::string& name);
    
    // Get plugin
    std::shared_ptr<IPlugin> getPlugin(const std::string& name);
    
    // Get all plugins
    std::vector<std::shared_ptr<IPlugin>> getPlugins() const;
    
    // Check if plugin is available
    bool hasPlugin(const std::string& name) const;
    
    // Load plugin from file
    bool loadPlugin(const std::string& filePath);
    
    // Unload plugin
    bool unloadPlugin(const std::string& name);
    
    // Get plugin file extensions
    std::vector<std::string> getPluginFileExtensions() const;
};
```

### Plugin Loader

Loader for loading plugins from files:

```cpp
// Plugin loader
class PluginLoader {
public:
    static std::shared_ptr<IPlugin> loadPlugin(const std::string& filePath);
    static std::vector<std::shared_ptr<IPlugin>> loadPluginsFromDirectory(const std::string& directory);
    static bool isPluginFile(const std::string& filePath);
    static std::vector<std::string> getPluginFilesFromDirectory(const std::string& directory);
};
```

### Plugin Manager

Manager for managing plugin lifecycle:

```cpp
// Plugin manager
class PluginManager {
public:
    static PluginManager& getInstance();
    
    // Initialize plugin manager
    bool initialize(const Config& config);
    
    // Shutdown plugin manager
    void shutdown();
    
    // Enable/disable all plugins
    void enableAllPlugins(bool enable);
    
    // Update all plugins
    void updateAllPlugins(float dt);
    
    // Render all plugins
    void renderAllPlugins();
    
    // Send event to all plugins
    void sendEventToAllPlugins(const Event& event);
    
    // Send message to all plugins
    void sendMessageToAllPlugins(const std::string& message);
    
    // Get enabled plugins
    std::vector<std::shared_ptr<IPlugin>> getEnabledPlugins() const;
    
    // Get disabled plugins
    std::vector<std::shared_ptr<IPlugin>> getDisabledPlugins() const;
};
```

## 💻 Plugin Development

### 1. Creating a Plugin

Create a new plugin:

```cpp
// MyPlugin.hpp
#pragma once

#include "IPlugin.hpp"

class MyPlugin : public IPlugin {
public:
    MyPlugin();
    ~MyPlugin() override;
    
    // Plugin metadata
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    std::string getContact() const override;
    
    // Plugin lifecycle
    bool initialize(const Config& config) override;
    void shutdown() override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // Plugin operations
    void update(float dt) override;
    void render() override;
    
    // Plugin-specific operations
    void onEvent(const Event& event) override;
    void onMessage(const std::string& message) override;
    
private:
    bool enabled_;
    Config config_;
    // Add your plugin-specific data members
};
```

```cpp
// MyPlugin.cpp
#include "MyPlugin.hpp"

MyPlugin::MyPlugin() : enabled_(false) {}

MyPlugin::~MyPlugin() {}

std::string MyPlugin::getName() const {
    return "MyPlugin";
}

std::string MyPlugin::getVersion() const {
    return "1.0.0";
}

std::string MyPlugin::getDescription() const {
    return "This is a sample plugin for NLM";
}

std::string MyPlugin::getAuthor() const {
    return "Your Name";
}

std::string MyPlugin::getContact() const {
    return "your.email@example.com";
}

bool MyPlugin::initialize(const Config& config) {
    config_ = config;
    enabled_ = true;
    
    // Initialize plugin-specific resources
    // ...
    
    return true;
}

void MyPlugin::shutdown() {
    // Shutdown plugin-specific resources
    // ...
    
    enabled_ = false;
}

bool MyPlugin::isEnabled() const {
    return enabled_;
}

void MyPlugin::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void MyPlugin::update(float dt) {
    if (!enabled_) return;
    
    // Update plugin logic
    // ...
}

void MyPlugin::render() {
    if (!enabled_) return;
    
    // Render plugin visualization
    // ...
}

void MyPlugin::onEvent(const Event& event) {
    // Handle events
    // ...
}

void MyPlugin::onMessage(const std::string& message) {
    // Handle messages
    // ...
}
```

### 2. Plugin Registration

Register plugin with plugin registry:

```cpp
// Plugin registration
void registerPlugin() {
    auto plugin = std::make_shared<MyPlugin>();
    PluginRegistry::getInstance().registerPlugin(plugin);
}

// Use constructor registration
class PluginRegistration {
public:
    PluginRegistration() {
        registerPlugin();
    }
};

// Global registration
static PluginRegistration globalPluginRegistration;
```

### 3. Plugin Loading

Load plugin from file:

```python
# Python equivalent - load plugin
import pynlm

# Load plugin from file
plugin = pynlm.loadPlugin("my_plugin.py")

# Load all plugins from directory
plugins = pynlm.loadPluginsFromDirectory("plugins/")

# Get available plugins
available_plugins = pynlm.getAvailablePlugins()

# Get plugin information
if "MyPlugin" in available_plugins:
    plugin = pynlm.getPlugin("MyPlugin")
    print(f"Name: {plugin.getName()}")
    print(f"Version: {plugin.getVersion()}")
    print(f"Description: {plugin.getDescription()}")
```

## 🔌 Plugin Usage

### 1. Loading Plugins

Load plugins in NLM:

```cpp
#include "PluginManager.hpp"
#include "PluginRegistry.hpp"

// Initialize plugin manager
bool PluginManager::initialize(const Config& config) {
    // Load plugins from configuration
    if (config.has("plugins.search_paths")) {
        auto search_paths = config.getStringList("plugins.search_paths");
        for (const auto& path : search_paths) {
            PluginLoader::loadPluginsFromDirectory(path);
        }
    }
    
    return true;
}
```

```python
# Python equivalent - load plugins
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set plugin configuration
config.set("plugins.enabled", True)
config.set("plugins.auto_load", True)
config.set("plugins.search_paths", ["./plugins", "/usr/lib/nlm/plugins"])

# Initialize plugin manager
plugin_manager = pynlm.createPluginManager()
plugin_manager.initialize(config)

# Load all plugins
plugin_manager.loadAllPlugins()
```

### 2. Using Plugins

Use plugins in NLM:

```cpp
// Use plugins in NLM
class NLM {
public:
    // Initialize NLM with plugins
    bool initialize(const Config& config) {
        // Initialize brain
        if (!brain_.initialize(config)) {
            return false;
        }
        
        // Initialize plugin manager
        if (!plugin_manager_.initialize(config)) {
            return false;
        }
        
        return true;
    }
    
    // Update NLM
    void update(float dt) {
        // Update brain
        brain_.update(dt);
        
        // Update plugins
        plugin_manager_.updateAllPlugins(dt);
        
        // Render brain
        brain_.render();
        
        // Render plugins
        plugin_manager_.renderAllPlugins();
    }
    
    // Send event to plugins
    void sendEvent(const Event& event) {
        plugin_manager_.sendEventToAllPlugins(event);
    }
    
    // Send message to plugins
    void sendMessage(const std::string& message) {
        plugin_manager_.sendMessageToAllPlugins(message);
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<PluginManager> plugin_manager_;
};
```

```python
# Python equivalent - use plugins
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create plugin manager
plugin_manager = pynlm.createPluginManager()
plugin_manager.initialize(config)

# Load plugins
plugin_manager.loadAllPlugins()

# Enable plugins
plugin_manager.enableAllPlugins(True)

# Update system
for step in range(100):
    brain.step(step)
    plugin_manager.updateAllPlugins(0.1)
    
    # Render system
    brain.render()
    plugin_manager.renderAllPlugins()

# Send event
event = pynlm.createEvent("simulation_step", {"step": step})
plugin_manager.sendEventToAllPlugins(event)

# Send message
plugin_manager.sendMessageToAllPlugins("simulation_complete")
```

### 3. Plugin Configuration

Configure plugins:

```cpp
// Configure plugin
class Plugin : public IPlugin {
public:
    bool initialize(const Config& config) override {
        // Store configuration
        config_ = config;
        
        // Configure plugin-specific settings
        if (config.has("plugin.some_setting")) {
            some_setting_ = config.getBool("plugin.some_setting");
        }
        
        if (config.has("plugin.some_parameter")) {
            some_parameter_ = config.getFloat("plugin.some_parameter");
        }
        
        return true;
    }
    
    void setEnabled(bool enabled) override {
        // Update enabled state
        enabled_ = enabled;
        
        // Handle state change
        if (enabled_) {
            onEnable();
        } else {
            onDisable();
        }
    }
    
private:
    void onEnable() {
        // Handle plugin enable
        // ...
    }
    
    void onDisable() {
        // Handle plugin disable
        // ...
    }
    
    Config config_;
    bool enabled_ = false;
    bool some_setting_ = false;
    float some_parameter_ = 0.0f;
    // Add your plugin-specific data members
};
```

```python
# Python equivalent - configure plugin
import pynlm

# Create plugin with configuration
class ConfiguredPlugin(pynlm.IPlugin):
    def __init__(self):
        self.enabled = False
        self.some_setting = False
        self.some_parameter = 0.0
    
    def initialize(self, config):
        # Store configuration
        self.config = config
        
        # Configure plugin-specific settings
        if config.has("plugin.some_setting"):
            self.some_setting = config.getBool("plugin.some_setting")
        
        if config.has("plugin.some_parameter"):
            self.some_parameter = config.getFloat("plugin.some_parameter")
        
        self.enabled = True
        return True
    
    def setEnabled(self, enabled):
        # Update enabled state
        self.enabled = enabled
        
        # Handle state change
        if self.enabled:
            self.onEnable()
        else:
            self.onDisable()
    
    def onEnable(self):
        # Handle plugin enable
        print("Plugin enabled")
    
    def onDisable(self):
        # Handle plugin disable
        print("Plugin disabled")
    
    def getName(self):
        return "ConfiguredPlugin"
    
    def getVersion(self):
        return "1.0.0"
    
    def getDescription(self):
        return "A configured plugin example"
    
    def getAuthor(self):
        return "Your Name"
    
    def getContact(self):
        return "your.email@example.com"
    
    def update(self, dt):
        if not self.enabled:
            return
        
        # Update plugin logic
        print(f"Plugin updating with dt: {dt}")
    
    def render(self):
        if not self.enabled:
            return
        
        # Render plugin visualization
        print("Plugin rendering")
    
    def onEvent(self, event):
        # Handle events
        print(f"Plugin received event: {event.name}")
    
    def onMessage(self, message):
        # Handle messages
        print(f"Plugin received message: {message}")

# Use configured plugin
plugin = ConfiguredPlugin()
plugin.initialize(config)
plugin.update(0.1)
plugin.render()
```

## 📚 Plugin Examples

### 1. Visualization Plugin

Visualization plugin example:

```cpp
// VisualizationPlugin.hpp
#pragma once

#include "IPlugin.hpp"
#include <string>
#include <vector>

struct VisualizationData {
    float timestamp;
    std::vector<float> neuron_activities;
    std::vector<float> synapse_weights;
    std::vector<std::pair<int, int>> connections;
};

class VisualizationPlugin : public IPlugin {
public:
    VisualizationPlugin();
    ~VisualizationPlugin() override;
    
    // Plugin metadata
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    std::string getContact() const override;
    
    // Plugin lifecycle
    bool initialize(const Config& config) override;
    void shutdown() override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // Plugin operations
    void update(float dt) override;
    void render() override;
    
    // Plugin-specific operations
    void onEvent(const Event& event) override;
    void onMessage(const std::string& message) override;
    
    // Visualization-specific operations
    void addVisualizationData(const VisualizationData& data);
    void clearVisualizationData();
    void generateVisualization(const std::string& filename);
    void setVisualizationTheme(const std::string& theme);
    
private:
    bool enabled_;
    Config config_;
    std::vector<VisualizationData> visualization_data_;
    std::string theme_ = "default";
    // Add your visualization-specific data members
};
```

```cpp
// VisualizationPlugin.cpp
#include "VisualizationPlugin.hpp"

VisualizationPlugin::VisualizationPlugin() : enabled_(false) {}

VisualizationPlugin::~VisualizationPlugin() {}

std::string VisualizationPlugin::getName() const {
    return "VisualizationPlugin";
}

std::string VisualizationPlugin::getVersion() const {
    return "1.0.0";
}

std::string VisualizationPlugin::getDescription() const {
    return "Plugin for generating visualizations of brain activity";
}

std::string VisualizationPlugin::getAuthor() const {
    return "Your Name";
}

std::string VisualizationPlugin::getContact() const {
    return "your.email@example.com";
}

bool VisualizationPlugin::initialize(const Config& config) {
    config_ = config;
    enabled_ = true;
    
    // Configure visualization settings
    if (config.has("visualization.theme")) {
        theme_ = config.getString("visualization.theme");
    }
    
    if (config.has("visualization.enabled")) {
        enabled_ = config.getBool("visualization.enabled");
    }
    
    return true;
}

void VisualizationPlugin::shutdown() {
    visualization_data_.clear();
    enabled_ = false;
}

bool VisualizationPlugin::isEnabled() const {
    return enabled_;
}

void VisualizationPlugin::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void VisualizationPlugin::update(float dt) {
    if (!enabled_) return;
    
    // Update visualization data
    // ...
}

void VisualizationPlugin::render() {
    if (!enabled_) return;
    
    // Render visualizations
    // ...
}

void VisualizationPlugin::onEvent(const Event& event) {
    if (event.name == "simulation_step") {
        // Handle simulation step events
        // ...
    }
}

void VisualizationPlugin::onMessage(const std::string& message) {
    if (message == "generate_visualization") {
        // Generate visualization
        // ...
    }
}
```

```python
# Python equivalent - visualization plugin
import pynlm
from typing import List, Dict, Any

class VisualizationPlugin(pynlm.IPlugin):
    def __init__(self):
        self.enabled = False
        self.theme = "default"
        self.visualization_data = []
    
    def initialize(self, config):
        # Store configuration
        self.config = config
        
        # Configure visualization settings
        if config.has("visualization.theme"):
            self.theme = config.getString("visualization.theme")
        
        if config.has("visualization.enabled"):
            self.enabled = config.getBool("visualization.enabled")
        
        if config.has("visualization.export_formats"):
            self.export_formats = config.getStringList("visualization.export_formats")
        
        self.enabled = True
        return True
    
    def shutdown(self):
        # Clear visualization data
        self.visualization_data.clear()
        self.enabled = False
    
    def getName(self):
        return "VisualizationPlugin"
    
    def getVersion(self):
        return "1.0.0"
    
    def getDescription(self):
        return "Plugin for generating visualizations of brain activity"
    
    def getAuthor(self):
        return "Your Name"
    
    def getContact(self):
        return "your.email@example.com"
    
    def update(self, dt):
        if not self.enabled:
            return
        
        # Update visualization data
        print(f"Visualization plugin updating with dt: {dt}")
    
    def render(self):
        if not self.enabled:
            return
        
        # Render visualizations
        print("Visualization plugin rendering")
    
    def onEvent(self, event):
        # Handle events
        if event.name == "simulation_step":
            # Handle simulation step events
            print(f"Visualization plugin received simulation step event")
    
    def onMessage(self, message):
        # Handle messages
        if message == "generate_visualization":
            # Generate visualization
            print("Visualization plugin generating visualization")
    
    def addVisualizationData(self, data):
        # Add visualization data
        self.visualization_data.append(data)
    
    def generateVisualization(self, filename):
        # Generate visualization
        print(f"Visualization plugin generating visualization: {filename}")
```

### 2. Debugging Plugin

Debugging plugin example:

```cpp
// DebugPlugin.hpp
#pragma once

#include "IPlugin.hpp"
#include <string>
#include <vector>
#include <map>

struct DebugInfo {
    float timestamp;
    std::string message;
    std::map<std::string, std::string> details;
    std::string severity;
};

class DebugPlugin : public IPlugin {
public:
    DebugPlugin();
    ~DebugPlugin() override;
    
    // Plugin metadata
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;
    std::string getContact() const override;
    
    // Plugin lifecycle
    bool initialize(const Config& config) override;
    void shutdown() override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // Plugin operations
    void update(float dt) override;
    void render() override;
    
    // Plugin-specific operations
    void onEvent(const Event& event) override;
    void onMessage(const std::string& message) override;
    
    // Debugging-specific operations
    void log(const std::string& message, const std::string& severity = "INFO");
    void logDebug(const std::string& message);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void clearLog();
    std::vector<DebugInfo> getLog() const;
    
private:
    bool enabled_;
    Config config_;
    std::vector<DebugInfo> log_;
    bool console_logging_enabled_ = true;
    bool file_logging_enabled_ = false;
    std::string log_file_;
    // Add your debugging-specific data members
};
```

```cpp
// DebugPlugin.cpp
#include "DebugPlugin.hpp"
#include <fstream>

DebugPlugin::DebugPlugin() : enabled_(false) {}

DebugPlugin::~DebugPlugin() {
    if (file_logging_enabled_) {
        std::ofstream log_file(log_file_, std::ios::app);
        log_file.close();
    }
}

std::string DebugPlugin::getName() const {
    return "DebugPlugin";
}

std::string DebugPlugin::getVersion() const {
    return "1.0.0";
}

std::string DebugPlugin::getDescription() const {
    return "Plugin for debugging and logging NLM operations";
}

std::string DebugPlugin::getAuthor() const {
    return "Your Name";
}

std::string DebugPlugin::getContact() const {
    return "your.email@example.com";
}

bool DebugPlugin::initialize(const Config& config) {
    config_ = config;
    enabled_ = true;
    
    // Configure debugging settings
    if (config.has("debug.enabled")) {
        enabled_ = config.getBool("debug.enabled");
    }
    
    if (config.has("debug.console_logging")) {
        console_logging_enabled_ = config.getBool("debug.console_logging");
    }
    
    if (config.has("debug.file_logging")) {
        file_logging_enabled_ = config.getBool("debug.file_logging");
    }
    
    if (config.has("debug.log_file")) {
        log_file_ = config.getString("debug.log_file");
    }
    
    return true;
}

void DebugPlugin::shutdown() {
    log_;
    enabled_ = false;
}

bool DebugPlugin::isEnabled() const {
    return enabled_;
}

void DebugPlugin::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void DebugPlugin::update(float dt) {
    if (!enabled_) return;
    
    // Update debug logging
    // ...
}

void DebugPlugin::render() {
    if (!enabled_) return;
    
    // Render debug information
    // ...
}

void DebugPlugin::onEvent(const Event& event) {
    // Handle events
    log(event.name, "INFO");
}

void DebugPlugin::onMessage(const std::string& message) {
    // Handle messages
    log(message, "INFO");
}

void DebugPlugin::log(const std::string& message, const std::string& severity) {
    DebugInfo debug_info;
    debug_info.timestamp = 0.0f;  // Current time
    debug_info.message = message;
    debug_info.severity = severity;
    
    log_.push_back(debug_info);
    
    // Console logging
    if (console_logging_enabled_) {
        std::cout << "[" << severity << "] " << message << std::endl;
    }
    
    // File logging
    if (file_logging_enabled_) {
        std::ofstream log_file(log_file_, std::ios::app);
        if (log_file.is_open()) {
            log_file << "[" << debug_info.timestamp << "] "
                     << "[" << severity << "] "
                     << message << std::endl;
            log_file.close();
        }
    }
}

void DebugPlugin::logDebug(const std::string& message) {
    log(message, "DEBUG");
}

void DebugPlugin::logInfo(const std::string& message) {
    log(message, "INFO");
}

void DebugPlugin::logWarning(const std::string& message) {
    log(message, "WARNING");
}

void DebugPlugin::logError(const std::string& message) {
    log(message, "ERROR");
}

void DebugPlugin::clearLog() {
    log_.clear();
}

std::vector<DebugInfo> DebugPlugin::getLog() const {
    return log_;
}
```

```python
# Python equivalent - debugging plugin
import pynlm
from typing import List, Dict, Any
from datetime import datetime

class DebugPlugin(pynlm.IPlugin):
    def __init__(self):
        self.enabled = False
        self.log = []
        self.console_logging_enabled = True
        self.file_logging_enabled = False
        self.log_file = "debug.log"
    
    def initialize(self, config):
        # Store configuration
        self.config = config
        
        # Configure debugging settings
        if config.has("debug.enabled"):
            self.enabled = config.getBool("debug.enabled")
        
        if config.has("debug.console_logging"):
            self.console_logging_enabled = config.getBool("debug.console_logging")
        
        if config.has("debug.file_logging"):
            self.file_logging_enabled = config.getBool("debug.file_logging")
        
        if config.has("debug.log_file"):
            self.log_file = config.getString("debug.log_file")
        
        self.enabled = True
        return True
    
    def shutdown(self):
        # Clear log
        self.log.clear()
        self.enabled = False
    
    def getName(self):
        return "DebugPlugin"
    
    def getVersion(self):
        return "1.0.0"
    
    def getDescription(self):
        return "Plugin for debugging and logging NLM operations"
    
    def getAuthor(self):
        return "Your Name"
    
    def getContact(self):
        return "your.email@example.com"
    
    def update(self, dt):
        if not self.enabled:
            return
        
        # Update debug logging
        print(f"Debug plugin updating with dt: {dt}")
    
    def render(self):
        if not self.enabled:
            return
        
        # Render debug information
        print("Debug plugin rendering")
    
    def onEvent(self, event):
        # Handle events
        self.log_event(event)
    
    def onMessage(self, message):
        # Handle messages
        self.log_message(message)
    
    def log_event(self, event):
        # Log event
        debug_info = {
            'timestamp': datetime.now().isoformat(),
            'message': event.name,
            'details': event.data,
            'severity': 'INFO'
        }
        self.log.append(debug_info)
        
        # Console logging
        if self.console_logging_enabled:
            print(f"[{debug_info['severity']}] {debug_info['message']}")
        
        # File logging
        if self.file_logging_enabled:
            with open(self.log_file, 'a') as f:
                f.write(f"[{debug_info['timestamp']}] [{debug_info['severity']}] {debug_info['message']}\n")
    
    def log_message(self, message):
        # Log message
        debug_info = {
            'timestamp': datetime.now().isoformat(),
            'message': message,
            'details': {},
            'severity': 'INFO'
        }
        self.log.append(debug_info)
        
        # Console logging
        if self.console_logging_enabled:
            print(f"[{debug_info['severity']}] {debug_info['message']}")
        
        # File logging
        if self.file_logging_enabled:
            with open(self.log_file, 'a') as f:
                f.write(f"[{debug_info['timestamp']}] [{debug_info['severity']}] {debug_info['message']}\n")
    
    def getLog(self):
        return self.log
```

## 🔧 Plugin Testing

### 1. Plugin Testing

Test plugins:

```python
import pynlm
import pytest

# Test plugin
class TestPlugin:
    def test_plugin_initialization(self):
        """Test plugin initialization"""
        plugin = pynlm.IPlugin()
        config = pynlm.createDefaultConfig()
        
        # Test initialization
        assert plugin.initialize(config) == True
        assert plugin.isEnabled() == True
        
        # Test shutdown
        plugin.shutdown()
        assert plugin.isEnabled() == False
    
    def test_plugin_update(self):
        """Test plugin update"""
        plugin = pynlm.IPlugin()
        config = pynlm.createDefaultConfig()
        
        # Initialize plugin
        assert plugin.initialize(config) == True
        
        # Test update
        plugin.update(0.1)
        assert plugin.isEnabled() == True
        
        # Test shutdown
        plugin.shutdown()
    
    def test_plugin_render(self):
        """Test plugin render"""
        plugin = pynlm.IPlugin()
        config = pynlm.createDefaultConfig()
        
        # Initialize plugin
        assert plugin.initialize(config) == True
        
        # Test render
        plugin.render()
        assert plugin.isEnabled() == True
        
        # Test shutdown
        plugin.shutdown()
    
    def test_plugin_events(self):
        """Test plugin events"""
        plugin = pynlm.IPlugin()
        config = pynlm.createDefaultConfig()
        
        # Initialize plugin
        assert plugin.initialize(config) == True
        
        # Test event handling
        event = pynlm.createEvent("test_event", {"data": "test_data"})
        plugin.onEvent(event)
        
        # Test message handling
        plugin.onMessage("test_message")
        
        # Test shutdown
        plugin.shutdown()

# Run tests
if __name__ == "__main__":
    pytest.main([__file__, "-v"])
```

### 2. Plugin Integration Testing

Test plugin integration:

```python
import pynlm

def test_plugin_integration():
    """Test plugin integration"""
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("plugins.enabled", True)
    config.set("plugins.auto_load", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create plugin manager
    plugin_manager = pynlm.createPluginManager()
    plugin_manager.initialize(config)
    
    # Load plugins
    plugin_manager.loadAllPlugins()
    
    # Enable plugins
    plugin_manager.enableAllPlugins(True)
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        plugin_manager.updateAllPlugins(0.1)
        
        # Check if plugins are working
        assert plugin_manager.getEnabledPlugins() > 0
    
    # Send event
    event = pynlm.createEvent("test_event", {"data": "test_data"})
    plugin_manager.sendEventToAllPlugins(event)
    
    # Send message
    plugin_manager.sendMessageToAllPlugins("test_message")
    
    print("✅ Plugin integration test passed")

# Run integration test
if __name__ == "__main__":
    test_plugin_integration()
```

## 📚 Plugin Documentation

### 1. Plugin Documentation Index

#### Plugin Documentation

1. **Plugin Overview**: Plugin system overview and architecture
2. **Plugin Architecture**: Plugin architecture and design
3. **Plugin Development**: Plugin development and implementation
4. **Plugin Usage**: Plugin usage and integration
5. **Plugin Examples**: Plugin examples and use cases
6. **Plugin Testing**: Plugin testing and validation
7. **Plugin Documentation**: Plugin documentation
8. **Plugin Resources**: Plugin resources and tools

### 2. Plugin Quick Reference

#### Quick Plugin Reference

| Plugin | Purpose | File | Description |
|--------|---------|------|-------------|
| **Visualization Plugin** | Generate visualizations | `visualization_plugin.hpp` | Plugin for generating visualizations of brain activity |
| **Debug Plugin** | Debugging and logging | `debug_plugin.hpp` | Plugin for debugging and logging NLM operations |
| **Memory Plugin** | Memory management | `memory_plugin.hpp` | Plugin for memory management |
| **Performance Plugin** | Performance monitoring | `performance_plugin.hpp` | Plugin for performance monitoring |
| **Analysis Plugin** | Data analysis | `analysis_plugin.hpp` | Plugin for data analysis |

#### Quick Plugin Commands

```bash
# Load plugin
nlm --load-plugin plugin_name

# Unload plugin
nlm --unload-plugin plugin_name

# Enable plugin
nlm --enable-plugin plugin_name

# Disable plugin
nlm --disable-plugin plugin_name

# List plugins
nlm --list-plugins

# Get plugin information
nlm --plugin-info plugin_name
```

### 3. Plugin Tutorials

#### Plugin Tutorial 1: Creating a Simple Plugin

1. **Create a new plugin file**: Create `my_plugin.hpp` and `my_plugin.cpp`
2. **Implement plugin interface**: Implement `IPlugin` interface
3. **Register plugin**: Register plugin with plugin registry
4. **Load plugin**: Load plugin with plugin manager
5. **Use plugin**: Use plugin in NLM

```cpp
// MyPlugin.hpp
#pragma once

#include "IPlugin.hpp"

class MyPlugin : public IPlugin {
public:
    // Implementation
};
```

```cpp
// MyPlugin.cpp
#include "MyPlugin.hpp"

// Implementation
```

```python
# Python equivalent
import pynlm

# Create plugin
class MyPlugin(pynlm.IPlugin):
    def initialize(self, config):
        # Implementation
        return True
    
    def shutdown(self):
        # Implementation
    
    def update(self, dt):
        # Implementation
    
    # Implement other IPlugin methods
```

## 🔧 Plugin Resources

### 1. Plugin Development Resources

#### Development Tools

| Tool | Purpose | Link |
|------|---------|------|
| **Plugin SDK** | Plugin development kit | `plugins/plugin_sdk/` |
| **Plugin Examples** | Plugin examples | `plugins/examples/` |
| **Plugin Templates** | Plugin templates | `plugins/templates/` |
| **Plugin Documentation** | Plugin documentation | `docs/plugin-guide.md` |
| **Plugin Tutorials** | Plugin tutorials | `docs/tutorials/plugin-tutorial.md` |

#### Development Resources

1. **Plugin API Reference**: API reference for plugin development
2. **Plugin Architecture**: Architecture documentation for plugins
3. **Plugin Examples**: Examples of plugin development
4. **Plugin Templates**: Templates for creating plugins
5. **Plugin Best Practices**: Best practices for plugin development

### 2. Plugin Usage Resources

#### Usage Resources

| Resource | Purpose | Link |
|----------|---------|------|
| **Plugin Usage Guide** | Guide for using plugins | `docs/plugin-guide.md` |
| **Plugin Integration** | Integration guide for plugins | `docs/integration/plugin-integration.md` |
| **Plugin Configuration** | Configuration guide for plugins | `docs/configuration/plugin-configuration.md` |
| **Plugin Troubleshooting** | Troubleshooting guide for plugins | `docs/troubleshooting/plugin-troubleshooting.md` |

#### Usage Resources

1. **Quick Start**: Quick start guide for plugin usage
2. **Configuration**: Configuration guide for plugins
3. **Integration**: Integration guide for plugins
4. **Troubleshooting**: Troubleshooting guide for plugins

### 3. Plugin Community

#### Community Resources

| Resource | Purpose | Link |
|----------|---------|------|
| **Plugin Forum** | Discussion forum for plugins | `https://community.kilo.ai/plugins` |
| **Plugin Discord** | Discord server for plugins | `https://discord.gg/nlm-plugins` |
| **Plugin GitHub** | GitHub repository for plugins | `https://github.com/your-repo/nlm-plugins` |
| **Plugin Issues** | Issue tracker for plugins | `https://github.com/your-repo/nlm-plugins/issues` |
| **Plugin Pull Requests** | Pull request submission for plugins | `https://github.com/your-repo/nlm-plugins/pulls` |

#### Community Guidelines

1. **Be respectful**: Treat all community members respectfully
2. **Be helpful**: Help others when possible
3. **Share knowledge**: Share knowledge and experience
4. **Report issues**: Report issues promptly
5. **Contribute**: Contribute to the project

## 📊 Plugin Metrics

### Plugin Usage Metrics

| Metric | Target | Current |
|--------|--------|---------|
| **Plugin Load Time** | < 1 second | - |
| **Plugin Update Time** | < 1ms per update | - |
| **Plugin Render Time** | < 1ms per render | - |
| **Plugin Memory Usage** | < 10 MB | - |
| **Plugin CPU Usage** | < 1% | - |

### Plugin Development Metrics

| Metric | Target | Current |
|--------|--------|---------|
| **Plugin Development Time** | < 1 hour | - |
| **Plugin Testing Time** | < 1 hour | - |
| **Plugin Documentation Time** | < 1 hour | - |
| **Plugin Integration Time** | < 1 hour | - |
| **Plugin Deployment Time** | < 1 hour | - |

## 🏆 Plugin Success Criteria

Plugins are successful when:

1. **Plugin Functionality**: Plugin functionality is correct
2. **Plugin Performance**: Plugin performance is acceptable
3. **Plugin Integration**: Plugin integration is seamless
4. **Plugin Documentation**: Plugin documentation is comprehensive
5. **Plugin Testing**: Plugin tests are comprehensive
6. **Plugin Deployment**: Plugin deployment is smooth

## 📚 Plugin Summary

This completes the NLM Plugin Guide. It provides comprehensive documentation for the NLM plugin system, including:

- **Plugin Overview**: Plugin system overview and architecture
- **Plugin Architecture**: Plugin architecture and design
- **Plugin Development**: Plugin development and implementation
- **Plugin Usage**: Plugin usage and integration
- **Plugin Examples**: Plugin examples and use cases
- **Plugin Testing**: Plugin testing and validation
- **Plugin Documentation**: Plugin documentation
- **Plugin Resources**: Plugin resources and tools

## 🔌 Final Plugin Steps

To successfully implement plugins:

1. **Plan plugin requirements**: Plan plugin requirements and design
2. **Implement plugin**: Implement plugin with proper interface
3. **Test plugin**: Test plugin with comprehensive tests
4. **Document plugin**: Document plugin with comprehensive documentation
5. **Integrate plugin**: Integrate plugin with NLM
6. **Deploy plugin**: Deploy plugin to production
7. **Monitor plugin**: Monitor plugin performance
8. **Maintain plugin**: Maintain plugin over time

Following these steps will help you successfully implement NLM plugins and ensure successful system operation.🧠✨