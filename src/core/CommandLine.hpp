#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>

namespace nlm {

class CommandLineParser {
public:
    struct Argument {
        std::string name;
        std::string description;
        std::string defaultValue;
        bool required = false;
        std::string type; // "string", "int", "float", "bool"
    };

    CommandLineParser(const std::string& programName) : programName(programName) {}
    
    void addArgument(const std::string& name, const std::string& description, 
                     const std::string& defaultValue = "", bool required = false,
                     const std::string& type = "string") {
        arguments.push_back({name, description, defaultValue, required, type});
    }
    
    bool parse(int argc, char* argv[]) {
        // Parse --key value pairs and --flag boolean flags
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key value
            if (arg.substr(0, 2) == "--") {
                std::string key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string value = argv[++i];
                    values[key] = value;
                } else {
                    values[key] = "true"; // boolean flag
                }
            }
        }
        
        // Validate required arguments
        for (const auto& arg : arguments) {
            if (arg.required && values.find(arg.name) == values.end()) {
                std::cerr << "Error: Required argument --" << arg.name << " not provided" << std לפיית std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    std::string getValue(const std::string& name, const std::string& defaultValue = "") const {
        auto it = values.find(name);
        if (it != values.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    int getInt(const std::string& name, int defaultValue = 0) const {
        auto it = values.find(name);
        if (it != values.end()) {
            try {
                return std::stoi(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    float getFloat(const std::string& name, float defaultValue = 0.0f) const {
        auto it = values.find(name);
        if (it != values.end()) {
            try {
                return std::stof(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    bool getBool(const std::string& name, bool defaultValue = false) const {
        auto it = values.find(name);
        if (it != values.end()) {
            std::string val = it->second;
            std::transform(val.begin(), val.end(), val.begin(), ::tolower);
            return val == "true" || val == "1" || val == "yes" || val == "on";
        }
        return defaultValue;
    }
    
    bool hasArgument(const std::string& name) const {
        return values.find(name) != values.end();
    }
    
    void printHelp() const {
        std::cout << "Usage: " << programName << " [options]" << std::endl << std::endl;
        
        std::cout << "Options:" << std::endl;
        for (const auto& arg : arguments) {
            std::cout << "  --" << arg.name;
            if (!arg.type.empty() && arg.type != "string") {
                std::cout << " <" << arg.type << ">";
            }
            if (!arg.defaultValue.empty()) {
                std::cout << " (default: " << arg.defaultValue << ")";
            }
            if (arg.required) {
                std::cout << " (required)";
            }
            std::cout << std::endl;
            std::cout << "    " << arg.description << std::endl << std::endl;
        }
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << programName << " --config demo.cfg --steps 1000 --learning-rate 0.01" << std::endl;
        std::cout << "  " << programName << " --neurons 1000 --plasticity --verbose" << std::endl;
    }

private:
    std::string programName;
    std::vector<Argument> arguments;
    std::map<std::string, std::string> values;
};

// Helper function to parse command line arguments and get common options
struct DemoOptions {
    std::string configFile = "configs/default.cfg";
    int steps = 1000;
    float learningRate = 0.01f;
    int neurons = 500;
    int regions = 1;
    float connectionProb = 0.15f;
    bool enablePlasticity = true;
    bool verbose = false;
    std::string experimentMode = "plasticity"; // "connectivity", "plasticity", "stdp", "learning"
};

DemoOptions parseDemoOptions(int argc, char* argv[]) {
    DemoOptions options;
    CommandLineParser parser("nlm_demo");
    
    // Add common demo arguments
    parser.addArgument("config", "Configuration file path", "configs/default.cfg");
    parser.addArgument("steps", "Number of simulation steps", "1000", false, "int");
    parser.addArgument("learning-rate", "Learning rate for plasticity", "0.01", false, "float");
    parser.addArgument("neurons", "Number of neurons in brain", "500", false, "int");
    parser.addArgument("regions", "Number of neural regions", "1", false, "int");
    parser.addArgument("connection-prob", "Connection probability between neurons", "0.15", false, "float");
    parser.addArgument("plasticity", "Enable synaptic plasticity", "false", false, "bool");
    parser.addArgument("verbose", "Enable verbose output", "false", false, "bool");
    parser.addArgument("mode", "Experiment mode: connectivity, plasticity, stdp, learning", "plasticity");
    
    if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        parser.printHelp();
        exit(0);
    }
    
    if (parser.parse(argc, argv)) {
        options.configFile = parser.getValue("config", "configs/default.cfg");
        options.steps = parser.getInt("steps", 1000);
        options.learningRate = parser.getFloat("learning-rate", 0.01f);
        options.neurons = parser.getInt("neurons", 500);
        options.regions = parser.getInt("regions", 1);
        options.connectionProb = parser.getFloat("connection-prob", 0.15f);
        options.enablePlasticity = parser.getBool("plasticity", false);
        options.verbose = parser.getBool("verbose", false);
        options.experimentMode = parser.getValue("mode", "plasticity");
    }
    
    return options;
}

} // namespace nlm
