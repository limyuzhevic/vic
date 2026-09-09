// Command-line interface for NLM
// Provides easy-to-use interface for running experiments and interacting with the brain

#include "nlm/ApplicationCLI.hpp"
#include "nlm/ExperimentRunnerCLI.hpp"
#include "nlm/ConfigCLI.hpp"

int main(int argc, char* argv[]) {
    NLM_LOG_INFO("NLM Command Line Interface v6.0 - Phase 6 Integration");
    NLM_LOG_INFO("=====================================================");
    
    ApplicationCLI app(argc, argv);
    
    if (!app.parseArguments()) {
        app.printHelp();
        return 1;
    }
    
    return app.run();
}
