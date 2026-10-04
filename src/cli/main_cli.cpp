// NLM (熙然) - Neural Learning Machine
// Phase 3: Configuration and CLI
//
// This phase implements comprehensive configuration management and CLI interface with:
// - Configuration validation system with detailed error reporting
// - Command-line interface with subcommands for configuration management
// - Configuration file operations (load/save/export/import)
// - Help system with auto-generated documentation
// - Command history and completion support

#include "NLMCLI.hpp"

int main(int argc, char** argv) {
    nlm::NLMCLI cli;
    
    if (!cli.parse(argc, argv)) {
        return 1;
    }
    
    return cli.execute();
}
