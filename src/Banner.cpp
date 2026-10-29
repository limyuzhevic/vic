// NLM (熙然) - Neural Learning Machine
// Banner implementation
//

#include "Banner.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;

void nlm::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void nlm::printSystemInfo(const std::string& configFile) {
    NLM_LOG_INFO("NLM Neural Learning Machine - Phase 2")
    NLM_LOG_INFO("Real neural computation with LIF neurons")
    NLM_LOG_INFO("Event-driven spike propagation with delays")
    NLM_LOG_INFO("STDP and Hebbian plasticity rules")
    NLM_LOG_INFO("Structural plasticity (synaptogenesis/pruning)")
    NLM_LOG_INFO("");
    
    if (!configFile.empty()) {
        NLM_LOG_INFO("Configuration file: " + configFile);
    }
}

bool nlm::parseCommandLine(int argc, char** argv, std::string& configFile, bool& help) {
    configFile = "configs/default.cfg";
    help = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    return true;
}

void nlm::printHelp() {
    std::cout << "NLM - Neural Learning Machine (Phase 2)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --config=file           Specify configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --config file           Same as above (alternative syntax)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm                    Run with default config" << std::endl;
    std::cout << "  nlm --config=myconfig.cfg" << std::endl;
    std::cout << "  nlm --help" << std::endl;
}