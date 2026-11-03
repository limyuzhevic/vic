// NLM Advanced Configuration and Command-Line Interface
// Enhanced configuration system with better command-line argument parsing
// Supports configuration files, command-line overrides, and runtime configuration changes

#ifndef NLM_ADVANCED_CONFIG_HPP
#define NLM_ADVANCED_CONFIG_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <functional>

namespace nlm {

class AdvancedConfig {
public:
    // Configuration categories for organization
    enum class Category {
        Simulation,      // Basic simulation parameters
        Neural,          // Neural network parameters
        Learning,        // Learning and plasticity parameters
        Agent,           // Agent behavior parameters
        Visualization,   // Visualization parameters
        Performance,     // Performance tuning parameters
        Debug,           // Debugging parameters
        Custom          // User-defined parameters
    };

    // Configuration parameter descriptor
    struct Parameter {
        std::string name;
        std::string description;
        Category category;
        std::string type;  // "int", "float", "double", "string", "bool"
        std::string defaultValue;
        std::string currentValue;
        std::vector<std::string> allowedValues;
        bool isRequired;
        std::function<void(const std::string&)> callback;
    };

    // Command-line argument parser
    class CommandLineParser {
    public:
        struct Argument {
            std::string name;
            std::string value;
            std::string description;
            bool hasValue;
            bool isRequired;
        };

        CommandLineParser(int argc, char** argv);
        
        // Add command-line argument
        void addArgument(const std::string& name, bool hasValue = true, 
                        const std::string& description = "", 
                        bool isRequired = false);
        
        // Parse command-line arguments
        bool parse();
        
        // Get argument value by name
        std::optional<std::string> getArgument(const std::string& name) const;
        
        // Get all parsed arguments
        const std::vector<Argument>& getArguments() const { return arguments_; }
        
        // Check if argument was provided
        bool hasArgument(const std::string& name) const;
        
        // Get help text
        std::string getHelpText() const;

    private:
        int argc_;
        char** argv_;
        std::vector<Argument> arguments_;
        std::map<std::string, std::string> parsedValues_;
    };

    // Configuration file manager
    class ConfigFileManager {
    public:
        ConfigFileManager();
        
        // Load configuration from file
        bool loadFromFile(const std::string& filename);
        
        // Save configuration to file
        bool saveToFile(const std::string& filename) const;
        
        // Get file extension supported
        static std::vector<std::string> getSupportedExtensions() {
            return {".cfg", ".json", ".yaml", ".yml", ".ini"};
        }
        
        // Validate configuration file
        bool validateFile(const std::string& filename) const;

    private:
        std::string currentFile_;
    };

    // Runtime configuration manager
    class RuntimeConfigManager {
    public:
        RuntimeConfigManager();
        
        // Apply configuration overrides
        bool applyOverrides(const std::map<std::string, std::string>& overrides);
        
        // Get configuration as string
        std::string toString() const;
        
        // Export configuration to different formats
        std::string exportToJSON() const;
        std::string exportToYAML() const;
        std::string exportToINI() const;
        
        // Import configuration from different formats
        bool importFromJSON(const std::string& json);
        bool importFromYAML(const std::string& yaml);
        bool importFromINI(const std::string& ini);

    private:
        std::map<std::string, std::string> runtimeOverrides_;
    };

    // Advanced configuration manager (main class)
    class AdvancedConfigManager {
    public:
        AdvancedConfigManager();
        ~AdvancedConfigManager();
        
        // Initialize with basic configuration
        bool initialize();
        
        // Load configuration from file
        bool loadFromFile(const std::string& filename);
        
        // Load configuration from command line
        bool loadFromCommandLine(int argc, char** argv);
        
        // Apply configuration overrides
        bool applyOverrides(const std::map<std::string, std::string>& overrides);
        
        // Get parameter value by name
        template<typename T>
        T getParameter(const std::string& name, const T& defaultValue) const;
        
        // Set parameter value
        template<typename T>
        bool setParameter(const std::string& name, const T& value);
        
        // Get parameter description
        std::string getParameterDescription(const std::string& name) const;
        
        // Get all parameters in a category
        std::vector<Parameter> getParametersByCategory(Category category) const;
        
        // Validate current configuration
        std::vector<std::string> validateConfiguration() const;
        
        // Get configuration summary
        std::string getConfigurationSummary() const;
        
        // Save current configuration to file
        bool saveConfiguration(const std::string& filename) const;
        
        // Get help text for all parameters
        std::string getHelpText() const;
        
        // Reset configuration to defaults
        void resetToDefaults();
        
        // Export configuration to file
        bool exportConfiguration(const std::string& filename, 
                                 const std::string& format = "auto") const;
        
        // Import configuration from file
        bool importConfiguration(const std::string& filename);

    private:
        std::unique_ptr<Config> basicConfig_;
        std::unique_ptr<CommandLineParser> parser_;
        std::unique_ptr<ConfigFileManager> fileManager_;
        std::unique_ptr<RuntimeConfigManager> runtimeManager_;
        std::map<std::string, Parameter> parameters_;
        bool initialized_;
    };

} // namespace nlm

#endif // NLM_ADVANCED_CONFIG_HPP