// NLM (熙然) - Neural Learning Machine
// Modular Command Line Interface
// Phase 2: Real Neural Computation

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

namespace nlm {

class CLIParser {
public:
    CLIParser();
    virtual ~CLIParser() = default;
    
    void addOption(const std::string& name, const std::string& shortOpt, 
                   const std::string& description, bool required = false);
    void addFlag(const std::string& name, const std::string& shortOpt,
                 const std::string& description);
    
    bool parse(int argc, char** argv);
    bool isSet(const std::string& name) const;
    
    std::string getString(const std::string& name, const std::string& defaultValue = "") const;
    int getInt(const std::string& name, int defaultValue = 0) const;
    double getDouble(const std::string& name, double defaultValue = 0.0) const;
    bool getBool(const std::string& name, bool defaultValue = false) const;
    
    std::string getConfigFile() const;
    std::string getOutputFile() const;
    std::string getLogLevel() const;
    std::vector<std::string> getTestNames() const;
    bool runAllTests() const;
    bool runSimulation() const;
    bool runHelp() const;
    bool runVersion() const;
    
    void printHelp() const;
    void printVersion() const;
    
private:
    struct Option {
        std::string name;
        std::string shortOpt;
        std::string description;
        bool required;
        bool hasValue;
        std::string defaultValue;
        bool set;
    };
    
    std::vector<Option> options_;
    std::unordered_map<std::string, std::string> values_;
    std::string configFile_;
    std::string outputFile_;
    std::string logLevel_;
    std::vector<std::string> testNames_;
    bool runAllTests_;
    bool runSimulation_;
};

} // namespace nlm

#endif // NLM_CLI_H