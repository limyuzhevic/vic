#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace nlm {

class CLIParser {
public:
    struct Argument {
        std::string longName;
        char shortName;
        std::string description;
        std::string defaultValue;
        bool required;
        std::function<void(const std::string&, Config&)> action;
    };
    
    CLIParser(const std::string& programName);
    
    // Add argument definitions
    void addArgument(const std::string& longName, char shortName, 
                    const std::string& description, const std::string& defaultValue = "",
                    bool required = false);
    
    void addArgumentAction(const std::string& longName,
                          std::function<void(const std::string&, Config&)> action);
    
    // Parse command line arguments
    bool parse(int argc, char** argv, Config& config);
    
    // Display help information
    void showHelp() const;
    
    // Get parsed arguments
    const std::map<std::string, std::string>& getParsedArgs() const { return parsedArgs; }
    
    // Get program name
    const std::string& getProgramName() const { return programName; }
    
private:
    std::string programName;
    std::vector<Argument> arguments;
    std::map<std::string, std::string> parsedArgs;
    
    // Helper methods
    void parseStandardArgs(const std::string& arg, Config& config);
    void handleFileArg(const std::string& filepath, Config& config);
    void handleConfigArg(const std::string& keyValue, Config& config);
    void handleNeuronArg(const std::string& value, Config& config);
    void handleStepArg(const std::string& value, Config& config);
    void handleSTDPArg(const std::string& value, Config& config);
    void handleHelp() const;
};

} // namespace nlm