#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

namespace nlm {

class CommandLineParser {
public:
    struct Option {
        std::string name;
        char short_name;
        std::string description;
        std::string default_value;
        bool required;
        bool has_value;
    };
    
    CommandLineParser() = default;
    
    // Parse command line arguments
    bool parse(int argc, char** argv) {
        args_.clear();
        options_.clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for options
            if (arg.find("--") == 0) {
                size_t eq_pos = arg.find('=');
                if (eq_pos != std::string::npos) {
                    // Option with value
                    std::string name = arg.substr(2, eq_pos - 2);
                    std::string value = arg.substr(eq_pos + 1);
                    options_[name] = value;
                } else {
                    // Boolean option
                    std::string name = arg.substr(2);
                    options_[name] = "true";
                }
            } else if (arg.find('-') == 0 && arg.length() > 1) {
                // Short option (not fully implemented, but extensible)
                // For simplicity, we'll treat them as long options
                std::string name = arg.substr(1);
                options_[name] = "true";
            } else {
                // Positional argument
                positional_args_.push_back(arg);
            }
        }
        
        return validateRequiredOptions();
    }
    
    // Add an option definition
    void addOption(const std::string& name, char short_name, 
                   const std::string& description, const std::string& default_value = "",
                   bool required = false, bool has_value = true) {
        Option opt;
        opt.name = name;
        opt.short_name = short_name;
        opt.description = description;
        opt.default_value = default_value;
        opt.required = required;
        opt.has_value = has_value;
        
        options_map_[name] = opt;
    }
    
    // Get option value
    std::string getOption(const std::string& name, const std::string& default_value = "") const {
        auto it = options_.find(name);
        if (it != options_.end()) {
            return it->second;
        }
        
        // Check options_map for default value
        auto opt_it = options_map_.find(name);
        if (opt_it != options_map_.end() && !opt_it->second.default_value.empty()) {
            return opt_it->second.default_value;
        }
        
        return default_value;
    }
    
    // Check if option is present
    bool hasOption(const std::string& name) const {
        return options_.find(name) != options_.end();
    }
    
    // Get positional argument
    std::string getPositionalArg(size_t index) const {
        if (index < positional_args_.size()) {
            return positional_args_[index];
        }
        return "";
    }
    
    // Get number of positional arguments
    size_t getPositionalArgCount() const {
        return positional_args_.size();
    }
    
    // Print help
    void printHelp() const {
        std::cout << "=== Command Line Options ===" << std::endl;
        std::cout << std::endl;
        
        for (const auto& pair : options_map_) {
            const Option& opt = pair.second;
            
            // Print short option if available
            if (opt.short_name != '\0') {
                std::cout << "  -" << opt.short_name << "";
            } else {
                std::cout << "      ";
            }
            
            // Print long option
            std::cout << "  --" << opt.name;
            
            // Print description
            if (!opt.description.empty()) {
                std::cout << "  " << opt.description;
            }
            
            // Print default value
            if (!opt.default_value.empty()) {
                std::cout << " (default: " << opt.default_value << ")";
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
        
        if (!positional_args_.empty()) {
            std::cout << "Positional arguments:" << std::endl;
            for (size_t i = 0; i < positional_args_.size(); ++i) {
                std::cout << "  " << i << ": " << positional_args_[i] << std::endl;
            }
        }
    }
    
    // Validate required options
    bool validateRequiredOptions() const {
        for (const auto& pair : options_map_) {
            const Option& opt = pair.second;
            if (opt.required && !hasOption(opt.name)) {
                std::cerr << "Error: Required option --" << opt.name << " is missing" << std::endl;
                return false;
            }
        }
        return true;
    }
    
private:
    std::map<std::string, std::string> options_;
    std::map<std::string, Option> options_map_;
    std::vector<std::string> positional_args_;
};

// Global command line parser
CommandLineParser& getGlobalCommandLineParser() {
    static CommandLineParser parser;
    return parser;
}

// Convenience function to parse command line
bool parseCommandLine(int argc, char** argv) {
    CommandLineParser& parser = getGlobalCommandLineParser();
    return parser.parse(argc, argv);
}

// Get command line option
std::string getCommandLineOption(const std::string& name, const std::string& default_value = "") {
    return getGlobalCommandLineParser().getOption(name, default_value);
}

// Check if command line option exists
bool hasCommandLineOption(const std::string& name) {
    return getGlobalCommandLineParser().hasOption(name);
}

} // namespace nlm