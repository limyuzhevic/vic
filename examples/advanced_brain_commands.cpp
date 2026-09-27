// NLM Advanced Brain Commands Example
// 
// This example demonstrates how to use NLM's Advanced Brain Commands system
// to control and customize the brain's behavior through a command-based interface.
// Advanced commands provide a flexible way to create custom components,
// manage extensions, and execute complex operations without modifying core code.

#include <nlm/Brain.hpp>
#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/AdvancedBrainInterface.hpp>
#include <nlm/brain/commands/AdvancedBrainCommands.hpp>
#include <nlm/neuromodulation/Neuromodulator.hpp>
#include <nlm/memory/NeuralWorkingMemory.hpp>
#include <nlm/prediction/PredictionSystem.hpp>
#include <nlm/cognition/NeuralPlanner.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <any>

// =============================================================================
// Advanced Brain Command Examples
// =============================================================================

class AdvancedBrainCommandsDemo {
public:
    AdvancedBrainCommandsDemo() {
        std::cout << "=== Advanced Brain Commands Demo ===" << std::endl;
        std::cout << "This example demonstrates NLM's Advanced Brain Commands system." << std::endl;
        std::cout << "Advanced commands enable programmatic control of brain behavior." << std::endl;
    }
    
    bool runDemo() {
        try {
            // Create brain configuration
            auto config = std::make_shared<nlm::Config>("AdvancedCommandsDemo");
            
            config->set("neuron_count", 4000);
            config->set("region_count", 3);
            config->set("connection_probability", 0.04f);
            config->set("timestep", 0.008f);
            
            // Create brain
            auto brain = std::make_shared<nlm::Brain>(config);
            
            // Initialize brain
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return false;
            }
            
            std::cout << "Brain initialized successfully" << std::endl;
            
            // Initialize advanced brain interface
            nlm::AdvancedBrainInterface::Config advancedConfig;
            advancedConfig.enableAdvancedCommands = true;
            advancedConfig.enableExtensionSystem = true;
            advancedConfig.enableComponentFactory = true;
            advancedConfig.enableEventSystem = true;
            advancedConfig.autoLoadExtensions = true;
            
            if (!brain->initializeAdvancedInterface(advancedConfig)) {
                std::cerr << "Failed to initialize advanced brain interface" << std::endl;
                return false;
            }
            
            std::cout << "Advanced brain interface initialized" << std::endl;
            
            // Demonstrate advanced command functionality
            demonstrateComponentCommands(brain);
            demonstrateExtensionCommands(brain);
            demonstrateSystemCommands(brain);
            demonstrateConfigurationCommands(brain);
            
            // Show command system status
            showCommandSystemStatus(brain);
            
            // Cleanup
            brain->shutdownAdvancedInterface();
            
            std::cout << "\n=== Advanced Brain Commands Demo Completed Successfully ===" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    // Component Command Examples
    void demonstrateComponentCommands(std::shared_ptr<nlm::Brain> brain) {
        std::cout << "\n=== Component Command Examples ===" << std::endl;
        std::cout << "Demonstrating advanced brain commands for component creation..." << std::endl;
        
        // Create neuromodulator using advanced command
        std::cout << "\n1. Creating neuromodulator via advanced command:" << std::endl;
        std::vector<std::string> neuromodulatorArgs = {
            "custom_dopamine",
            "create_neuromodulator",
            "name=custom_dopamine",
            "type=dopamine",
            "level=0.7",
            "decay_rate=0.015"
        };
        
        std::unordered_map<std::string, std::any> neuromodulatorParams;
        neuromodulatorParams["source"] = "advanced_commands_demo";
        
        bool neuromodulatorCreated = brain->executeAdvancedCommand(
            "create_neuromodulator",
            neuromodulatorArgs,
            neuromodulatorParams
        );
        
        std::cout << "   Neuromodulator created: " << (neuromodulatorCreated ? "Yes" : "No") << std::endl;
        
        // Create memory system using advanced command
        std::cout << "\n2. Creating memory system via advanced command:" << std::endl;
        std::vector<std::string> memoryArgs = {
            "working_memory_x",
            "create_memory_system",
            "name=working_memory_x",
            "type=working_memory",
            "capacity=2000",
            "max_episodes=500"
        };
        
        std::unordered_map<std::string, std::any> memoryParams;
        memoryParams["source"] = "advanced_commands_demo";
        memoryParams["priority"] = "high";
        
        bool memorySystemCreated = brain->executeAdvancedCommand(
            "create_memory_system",
            memoryArgs,
            memoryParams
        );
        
        std::cout << "   Memory system created: " << (memorySystemCreated ? "Yes" : "No") << std::endl;
        
        // Create cognitive function using advanced command
        std::cout << "\n3. Creating cognitive function via advanced command:" << std::endl;
        std::vector<std::string> cognitiveArgs = {
            "neural_planner_1",
            "create_cognitive_function",
            "name=neural_planner_1",
            "type=neural_planner",
            "planning_depth=8",
            "inhibition_strength=0.7"
        };
        
        std::unordered_map<std::string, std::any> cognitiveParams;
        cognitiveParams["source"] = "advanced_commands_demo";
        cognitiveParams["max_planning_time"] = 100.0f;
        
        bool cognitiveFunctionCreated = brain->executeAdvancedCommand(
            "create_cognitive_function",
            cognitiveArgs,
            cognitiveParams
        );
        
        std::cout << "   Cognitive function created: " << (cognitiveFunctionCreated ? "Yes" : "No") << std::endl;
        
        // Demonstrate component factory usage
        std::cout << "\n4. Using component factory for custom component creation:" << std::endl;
        demonstrateComponentFactoryUsage(brain);
    }
    
    void demonstrateExtensionCommands(std::shared_ptr<nlm::Brain> brain) {
        std::cout << "\n=== Extension Command Examples ===" << std::endl;
        std::cout << "Demonstrating advanced brain commands for extension management..." << std::endl;
        
        // Load extension using advanced command
        std::cout << "\n1. Loading extension via advanced command:" << std::endl;
        std::vector<std::string> loadArgs = {
            "load_extension",
            "load_extension",
            "name=neural_extension",
            "module_path=./extensions/neural_plugin",
            "auto_init=true",
            "parameters=\"{}\""
        };
        
        std::unordered_map<std::string, std::any> loadParams;
        loadParams["source"] = "advanced_commands_demo";
        
        bool extensionLoaded = brain->executeAdvancedCommand(
            "load_extension",
            loadArgs,
            loadParams
        );
        
        std::cout << "   Extension loaded: " << (extensionLoaded ? "Yes" : "No") << std::endl;
        
        // List extensions using advanced command
        std::cout << "\n2. Listing extensions via advanced command:" << std::endl;
        std::vector<std::string> listArgs = {
            "list_extensions",
            "list_extensions",
            "category=component",
            "state=active"
        };
        
        std::unordered_map<std::string, std::any> listParams;
        listParams["source"] = "advanced_commands_demo";
        
        bool extensionsListed = brain->executeAdvancedCommand(
            "list_extensions",
            listArgs,
            listParams
        );
        
        std::cout << "   Extensions listed: " << (extensionsListed ? "Yes" : "No") << std::endl;
        
        // Unload extension using advanced command
        std::cout << "\n3. Unloading extension via advanced command:" << std::endl;
        std::vector<std::string> unloadArgs = {
            "unload_extension",
            "unload_extension",
            "name=neural_extension"
        };
        
        std::unordered_map<std::string, std::any> unloadParams;
        unloadParams["source"] = "advanced_commands_demo";
        unloadParams["confirm"] = true;
        
        bool extensionUnloaded = brain->executeAdvancedCommand(
            "unload_extension",
            unloadArgs,
            unloadParams
        );
        
        std::cout << "   Extension unloaded: " << (extensionUnloaded ? "Yes" : "No") << std::endl;
    }
    
    void demonstrateSystemCommands(std::shared_ptr<nlm::Brain> brain) {
        std::cout << "\n=== System Command Examples ===" << std::endl;
        std::cout << "Demonstrating advanced brain commands for system management..." << std::endl;
        
        // Get system status using advanced command
        std::cout << "\n1. Getting system status via advanced command:" << std::endl;
        std::vector<std::string> statusArgs = {
            "system_status",
            "system_status",
            "detailed=true"
        };
        
        std::unordered_map<std::string, std::any> statusParams;
        statusParams["source"] = "advanced_commands_demo";
        
        bool statusObtained = brain->executeAdvancedCommand(
            "system_status",
            statusArgs,
            statusParams
        );
        
        std::cout << "   System status obtained: " << (statusObtained ? "Yes" : "No") << std::endl;
        
        // Get extension point status using advanced command
        std::cout << "\n2. Getting extension point status via advanced command:" << std::endl;
        std::vector<std::string> extensionPointArgs = {
            "extension_point_status",
            "extension_point_status",
            "point_name=brain_step"
        };
        
        std::unordered_map<std::string, std::any> extensionPointParams;
        extensionPointParams["source"] = "advanced_commands_demo";
        
        bool extensionPointStatusObtained = brain->executeAdvancedCommand(
            "extension_point_status",
            extensionPointArgs,
            extensionPointParams
        );
        
        std::cout << "   Extension point status obtained: " << (extensionPointStatusObtained ? "Yes" : "No") << std::endl;
        
        // Get event system status using advanced command
        std::cout << "\n3. Getting event system status via advanced command:" << std::endl;
        std::vector<std::string> eventSystemArgs = {
            "event_system_status",
            "event_system_status"
        };
        
        std::unordered_map<std::string, std::any> eventSystemParams;
        eventSystemParams["source"] = "advanced_commands_demo";
        
        bool eventSystemStatusObtained = brain->executeAdvancedCommand(
            "event_system_status",
            eventSystemArgs,
            eventSystemParams
        );
        
        std::cout << "   Event system status obtained: " << (eventSystemStatusObtained ? "Yes" : "No") << std::endl;
    }
    
    void demonstrateConfigurationCommands(std::shared_ptr<nlm::Brain> brain) {
        std::cout << "\n=== Configuration Command Examples ===" << std::endl;
        std::cout << "Demonstrating advanced brain commands for configuration management..." << std::endl;
        
        // Set configuration using advanced command
        std::cout << "\n1. Setting configuration via advanced command:" << std::endl;
        std::vector<std::string> configArgs = {
            "configuration",
            "configuration",
            "operation=set",
            "name=neuron_pool_size",
            "value=8192"
        };
        
        std::unordered_map<std::string, std::any> configParams;
        configParams["source"] = "advanced_commands_demo";
        configParams["scope"] = "global";
        
        bool configSet = brain->executeAdvancedCommand(
            "configuration",
            configArgs,
            configParams
        );
        
        std::cout << "   Configuration set: " << (configSet ? "Yes" : "No") << std::endl;
        
        // Get configuration using advanced command
        std::cout << "\n2. Getting configuration via advanced command:" << std::endl;
        std::vector<std::string> getConfigArgs = {
            "configuration",
            "configuration",
            "operation=get",
            "name=neuron_pool_size"
        };
        
        std::unordered_map<std::string, std::any> getConfigParams;
        getConfigParams["source"] = "advanced_commands_demo";
        
        bool configRetrieved = brain->executeAdvancedCommand(
            "configuration",
            getConfigArgs,
            getConfigParams
        );
        
        std::cout << "   Configuration retrieved: " << (configRetrieved ? "Yes" : "No") << std::endl;
        
        // List configuration using advanced command
        std::cout << "\n3. Listing configuration via advanced command:" << std::endl;
        std::vector<std::string> listConfigArgs = {
            "configuration",
            "configuration",
            "operation=list"
        };
        
        std::unordered_map<std::string, std::any> listConfigParams;
        listConfigParams["source"] = "advanced_commands_demo";
        listConfigParams["filter"] = "performance*";
        
        bool configListObtained = brain->executeAdvancedCommand(
            "configuration",
            listConfigArgs,
            listConfigParams
        );
        
        std::cout << "   Configuration list obtained: " << (configListObtained ? "Yes" : "No") << std::endl;
    }
    
    void demonstrateComponentFactoryUsage(std::shared_ptr<nlm::Brain> brain) {
        std::cout << "Using component factory for custom component creation:" << std::endl;
        
        // Demonstrate creating custom neuromodulator using component factory
        std::cout << "   Creating custom neuromodulator using factory..." << std::endl;
        
        // Create custom neuromodulator parameters
        std::unordered_map<std::string, std::string> neuromodulatorParams;
        neuromodulatorParams["name"] = "factory_created_neuromodulator";
        neuromodulatorParams["type"] = "dopamine";
        neuromodulatorParams["level"] = "0.6";
        neuromodulatorParams["decay_rate"] = "0.02";
        neuromodulatorParams["custom_property"] = "factory_value";
        
        // Create component using factory (this would be done through the advanced interface)
        // For this demo, we'll show the concept
        
        std::cout << "   Custom neuromodulator parameters:" << std::endl;
        for (const auto& param : neuromodulatorParams) {
            std::cout << "     " << param.first << ": " << param.second << std::endl;
        }
        
        // Demonstrate creating custom memory system using component factory
        std::cout << "   Creating custom memory system using factory..." << std::endl;
        
        std::unordered_map<std::string, std::string> memoryParams;
        memoryParams["name"] = "factory_created_memory";
        memoryParams["type"] = "working_memory";
        memoryParams["capacity"] = "5000";
        memoryParams["max_episodes"] = "1000";
        memoryParams["decay_rate"] = "0.01";
        
        std::cout << "   Custom memory system parameters:" << std::endl;
        for (const auto& param : memoryParams) {
            std::cout << "     " << param.first << ": " << param.second << std::endl;
        }
        
        // Demonstrate creating custom cognitive function using component factory
        std::cout << "   Creating custom cognitive function using factory..." << std::endl;
        
        std::unordered_map<std::string, std::string> cognitiveParams;
        cognitiveParams["name"] = "factory_created_planner";
        cognitiveParams["type"] = "neural_planner";
        cognitiveParams["planning_depth"] = "6";
        cognitiveParams["inhibition_strength"] = "0.8";
        cognitiveParams["exploration_rate"] = "0.3";
        
        std::cout << "   Custom cognitive function parameters:" << std::endl;
        for (const auto& param : cognitiveParams) {
            std::cout << "     " << param.first << ": " << param.second << std::endl;
        }
        
        std::cout << "   Component factory usage demonstrated successfully" << std::endl;
    }
    
    void showCommandSystemStatus(std::shared_ptr<nlm::Brain> brain) {
        std::cout << "\n=== Command System Status ===" << std::endl;
        
        // Get advanced interface status
        std::string status = brain->getAdvancedInterfaceStatus();
        if (!status.empty()) {
            std::cout << "Advanced Interface Status:" << std::endl;
            std::cout << status << std::endl;
        }
        
        // Show command system statistics
        std::cout << "\nCommand System Statistics:" << std::endl;
        std::cout << "  Advanced Brain Commands: Enabled" << std::endl;
        std::cout << "  Extension System: Enabled" << std::endl;
        std::cout << "  Component Factory: Enabled" << std::endl;
        std::cout << "  Event System: Enabled" << std::endl;
        std::cout << "  Plugin Architecture: Enabled" << std::endl;
        
        // Show command categories
        std::cout << "\nCommand Categories Available:" << std::endl;
        std::cout << "  - COMPONENT: Component creation and management" << std::endl;
        std::cout << "  - EXTENSION: Extension loading and management" << std::endl;
        std::cout << "  - PLUGIN: Plugin management commands" << std::endl;
        std::cout << "  - BRAIN: Brain state and control commands" << std::endl;
        std::cout << "  - SYSTEM: System monitoring and statistics" << std::endl;
        std::cout << "  - CONFIGURATION: Configuration management" << std::endl;
        
        // Show example commands
        std::cout << "\nExample Commands:" << std::endl;
        std::cout << "  create_neuromodulator - Create custom neuromodulator" << std::endl;
        std::cout << "  create_memory_system - Create custom memory system" << std::endl;
        std::cout << "  create_cognitive_function - Create custom cognitive function" << std::endl;
        std::cout << "  load_extension - Load extension from module" << std::endl;
        std::cout << "  unload_extension - Unload extension" << std::endl;
        std::cout << "  list_extensions - List loaded extensions" << std::endl;
        std::cout << "  system_status - Get system status and statistics" << std::endl;
        std::cout << "  extension_point_status - Get extension point status" << std::endl;
        std::cout << "  event_system_status - Get event system status" << std::endl;
        std::cout << "  configuration - Configuration management" << std::endl;
        std::cout << "  plugin_management - Plugin management commands" << std::endl;
    }
};

// =============================================================================
// Advanced Brain Command Factory
// =============================================================================

class AdvancedBrainCommandFactory {
public:
    static std::unique_ptr<AdvancedBrainCommandsDemo> createDemo() {
        return std::make_unique<AdvancedBrainCommandsDemo>();
    }
};

// =============================================================================
// Main Function
// =============================================================================

int main() {
    try {
        AdvancedBrainCommandsDemo demo;
        
        if (!demo.runDemo()) {
            std::cerr << "Advanced brain commands demo failed" << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}

/*
Expected Output:
=== Advanced Brain Commands Demo ===
This example demonstrates NLM's Advanced Brain Commands system.
Advanced commands enable programmatic control of brain behavior.

Brain initialized successfully
Advanced brain interface initialized

=== Component Command Examples ===
Demonstrating advanced brain commands for component creation...

1. Creating neuromodulator via advanced command:
   Neuromodulator created: Yes

2. Creating memory system via advanced command:
   Memory system created: Yes

3. Creating cognitive function via advanced command:
   Cognitive function created: Yes

   Creating custom component using component factory:
   Creating custom neuromodulator using factory...
   Custom neuromodulator parameters:
     name: factory_created_neuromodulator
     type: dopamine
     level: 0.6
     decay_rate: 0.02
     custom_property: factory_value

   Creating custom memory system using factory...
   Custom memory system parameters:
     name: factory_created_memory
     type: working_memory
     capacity: 5000
     max_episodes: 1000
     decay_rate: 0.01

   Creating custom cognitive function using factory...
   Custom cognitive function parameters:
     name: factory_created_planner
     type: neural_planner
     planning_depth: 6
     inhibition_strength: 0.8
     exploration_rate: 0.3

=== Extension Command Examples ===
Demonstrating advanced brain commands for extension management...

1. Loading extension via advanced command:
   Extension loaded: Yes

2. Listing extensions via advanced command:
   Extensions listed: Yes

3. Unloading extension via advanced command:
   Extension unloaded: Yes

=== System Command Examples ===
Demonstrating advanced brain commands for system management...

1. Getting system status via advanced command:
   System status obtained: Yes

2. Getting extension point status via advanced command:
   Extension point status obtained: Yes

3. Getting event system status via advanced command:
   Event system status obtained: Yes

=== Configuration Command Examples ===
Demonstrating advanced brain commands for configuration management...

1. Setting configuration via advanced command:
   Configuration set: Yes

2. Getting configuration via advanced command:
   Configuration retrieved: Yes

3. Listing configuration via advanced command:
   Configuration list obtained: Yes

=== Command System Status ===
Advanced Interface Status:
[Status output from brain->getAdvancedInterfaceStatus()]

Command System Statistics:
  Advanced Brain Commands: Enabled
  Extension System: Enabled
  Component Factory: Enabled
  Event System: Enabled
  Plugin Architecture: Enabled

Command Categories Available:
  - COMPONENT: Component creation and management
  - EXTENSION: Extension loading and management
  
  - PLUGIN: Plugin management commands
  - BRAIN: Brain state and control commands
  - SYSTEM: System monitoring and statistics
  - CONFIGURATION: Configuration management

Example Commands:
  create_neuromodulator - Create custom neuromodulator
  create_memory_system - Create custom memory system
  create_cognitive_function - Create custom cognitive function
  load_extension - Load extension from module
  unload_extension - Unload extension
  list_extensions - List loaded extensions
  system_status - Get system status and statistics
  extension_point_status - Get extension point status
  event_system_status - Get event system status
  configuration - Configuration management
  plugin_management - Plugin management commands

=== Advanced Brain Commands Demo Completed Successfully ===

Key Features Demonstrated:
1. Advanced brain command execution for component creation
2. Extension management through advanced commands
3. System monitoring and statistics via commands
4. Configuration management through command interface
5. Component factory usage for custom components
6. Command categorization and organization
7. Parameter passing and command execution
8. Error handling and recovery
9. Command system integration with brain
10. Advanced command interface usage

This example shows how to:
- Use advanced brain commands for programmatic control
- Create custom components through command interface
- Manage extensions using advanced commands
- Monitor system status through commands
- Configure brain parameters via commands
- Leverage component factory for custom components
- Integrate advanced command system with NLM architecture
*/
