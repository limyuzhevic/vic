// NLM CLI Main Interface
// Comprehensive command-line interface for NLM system control

#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <string>
#include <memory>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {
namespace cli {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI() = default;
    
    int run(int argc, char** argv);
    
private:
    void initializeLogger();
    void printUsage() const;
    void printVersion() const;
    
    int handleRunCommand(int argc, char** argv);
    int handleConfigCommand(int argc, char** argv);
    int handleExperimentCommand(int argc, char** argv);
    int handleMonitorCommand(int argc, char** argv);
    int handleBenchmarkCommand(int argc, char** argv);
    int handleSaveCommand(int argc, char** argv);
    int handleLoadCommand(int argc, char** argv);
    int handleProfileCommand(int argc, char** argv);
    
    void printHelp(const std::string& command) const;
    void printConfigHelp() const;
    void printExperimentHelp() const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_HPP
