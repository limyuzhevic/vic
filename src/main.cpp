// NLM CLI Main Entry Point
// Provides command-line interface for NLM (Neural Learning Machine)
// Integrates with the existing NLM framework and provides advanced user controls

#include "cli/NLMCLI.hpp"
#include <iostream>
#include <vector>

// Command line argument parsing
std::vector<std::string> parseCommandLineArguments(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle both --option and -o formats
        if (arg.substr(0, 2) == "--") {
            // Long option
            args.push_back(arg.substr(2));
        } else if (arg.substr(0, 1) == "-") {
            // Short option - expand to long form where possible
            if (arg == "-h") args.push_back("help");
            else if (arg == "-v") args.push_back("version");
            else if (arg == "-i") args.push_back("init");
            else if (arg == "-r") {
                // Handle -r with next argument if it's not another option
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args.push_back("run");
                    args.push_back(argv[++i]);
                } else {
                    args.push_back("run");
                }
            } else if (arg == "-s") {
                // Handle -s with next argument
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args.push_back("save");
                    args.push_back(argv[++i]);
                } else {
                    args.push_back("save");
                }
            } else {
                // Keep short option as-is
                args.push_back(arg.substr(1));
            }
        } else {
            // Regular argument
            args.push_back(arg);
        }
    }
    
    return args;
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        std::vector<std::string> args = parseCommandLineArguments(argc, argv);
        
        // Handle help/version flags early
        if (args.empty()) {
            std::cout << "NLM - Neural Learning Machine (Phase 6: Final Integration)" << std::endl;
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h        Show help information" << std::endl;
            std::cout << "  --version, -v     Show version information" << std::endl;
            std::cout << "  --demo, -d        Run demo mode" << std::endl;
            std::cout << "  --init <template> Initialize with template" << std::endl;
            std::cout << "  --run [steps]     Run simulation" << std::endl;
            std::cout << "  --save <file>     Save brain state" << std::endl;
            std::cout << "  --load <file>     Load brain state" << std::endl;
            std::cout << "  --experiment <type> Run experiment" << std::endl;
            std::cout << "  --config <file>   Load configuration" << std::endl;
            std::cout << std::endl;
            std::cout << "For interactive mode, run without arguments." << std::endl;
            return 0;
        }
        
        // Check for help/version flags
        for (const auto& arg : args) {
            if (arg == "help" || arg == "h") {
                std::cout << "NLM - Neural Learning Machine (Phase 6: Final Integration)" << std::endl;
                std::cout << "Advanced command-line interface for brain simulation and control." << std::endl;
                std::cout << std::endl;
                std::cout << "Available commands (use with -- prefix):" << std::endl;
                std::cout << "  --help, -h        Show help information" << std::endl;
                std::cout << "  --version, -v     Show version information" << std::endl;
                std::cout << "  --demo, -d        Run demo mode" << std::endl;
                std::cout << "  --init <template> Initialize with configuration template" << std::endl;
                std::cout << "  --run [steps]     Run simulation" << std::endl;
                std::cout << "  --save <file>     Save brain state to file" << std::endl;
                std::cout << "  --load <file>     Load brain state from file" << std::endl;
                std::cout << "  --status          Show current system status" << std::endl;
                std::cout << "  --stats           Show detailed statistics" << std::endl;
                std::cout << "  --monitor         Start monitoring mode" << std::endl;
                std::cout << "  --experiment <type> Run experiment (phase6, memory, neuromodulation, prediction, cognition, development)" << std::endl;
                std::cout << "  --template        Manage configuration templates" << std::endl;
                std::cout << std::endl;
                std::cout << "Templates: demo, research, fast_test, full_integration, curiosity, development" << std::endl;
                return 0;
            }
            
            if (arg == "version" || arg == "v") {
                std::cout << "NLM - Neural Learning Machine (Phase 6: Final Integration)" << std::endl;
                std::cout << "Version: 2.0.0 (Enhanced CLI)" << std::endl;
                std::cout << "Build: 2026-09-29" << std::endl;
                std::cout << "Architecture: Experimental artificial developmental brain" << std::endl;
                std::cout << "Features: Integrated memory, prediction, cognition, neuromodulation" << std::endl;
                return 0;
            }
        }
        
        // Start NLM CLI
        nlm::startNLM(args);
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}