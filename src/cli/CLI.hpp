#include "CLI.hpp"
#include "CommandLineParser.hpp"
#include "../utils/ResultFormatter.hpp"

namespace nlm {

class UnifiedCLI {
public:
    UnifiedCLI();
    ~UnifiedCLI() = default;
    
    bool initialize(int argc, char* argv[]);
    bool run();
    
    // Subcommand handlers
    bool handlePhase2Command(const CommandLineParser& parser);
    bool handlePhase3Command(const CommandLineParser& parser);
    bool handlePhase4Command(const CommandLineParser& parser);
    bool handlePhase6Command(const CommandLineParser& parser);
    bool handleRunCommand(const CommandLineParser& parser);
    
    // Utility methods
    void printHelp() const;
    bool isHelpRequested() const { return showHelp_; }
    std::string getOutputFormat() const { return outputFormat_; }
    
    // Static function for demo executables to call
    static bool runUnifiedCLI(int argc, char* argv[]);
    
private:
    // Command line arguments
    int argc_;
    char** argv_;
    
    // CLI state
    std::string command_;
    bool showHelp_;
    std::string outputFormat_;
    std::string configFile_;
    std::string logLevel_;
    size_t neuronCount_;
    size_t steps_;
    bool enableProfiling_;
    bool enableCheckpoints_;
    bool enableDebugging_;
    std::string experimentName_;
    
    // Parser and formatter
    CommandLineParser parser_;
    ResultFormatter formatter_;
};

bool UnifiedCLI::runUnifiedCLI(int argc, char* argv[]) {
    UnifiedCLI cli;
    if (!cli.initialize(argc, argv)) {
        return false;
    }
    return cli.run();
}

} // namespace nlm