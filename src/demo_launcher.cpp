// NLM Unified Demo Launcher
// Provides a unified interface for running all NLM demos with better CLI support
// Usage: ./nlm_demo --phase <phase> [options]
//        ./nlm_demo --list-phases
//        ./nlm_demo --benchmark
//        ./nlm_demo --interactive

#include <thread>

using namespace nlm;

enum class DemoPhase {
    Phase2,    // Real Neural Computation
    Phase3,    // World Interaction
    Phase4,    // Cognitive Systems
    Phase5,    // Performance & Sandbox
    Phase6,    // Final Integration
    Phase6Text // Text-as-a-Sense
};

struct DemoInfo {
    DemoPhase phase;
    std::string name;
    std::string description;
    std::string executableName;
};

class DemoLauncher {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    
    std::unordered_map<DemoPhase, DemoInfo> demoInfo = {
        {DemoPhase::Phase2, {
            DemoPhase::Phase2,
            "Phase 2: Real Neural Computation",
            "Real LIF neuron dynamics, spike propagation, and basic plasticity",
            "nlm"
        }},
        {DemoPhase::Phase3, {
            DemoPhase::Phase3,
            "Phase 3: World Interaction",
            "Sensory input, motor output, reward prediction, development",
            "nlm_phase3_demo"
        }},
        {DemoPhase::Phase4, {
            DemoPhase::Phase4,
            "Phase 4: Cognitive Systems",
            "Working memory, episodic memory, prediction, planning",
            "nlm_phase4_demo"
        }},
        {DemoPhase::Phase5, {
            DemoPhase::Phase5,
            "Phase 5: Performance & Sandbox",
            "Performance optimizations, checkpointing, personality sandbox",
            "nlm_phase5_demo"
        }},
        {DemoPhase::Phase6, {
            DemoPhase::Phase6,
            "Phase 6: Final Integration",
            "All systems integrated into coherent brain loop",
            "nlm_phase6_demo"
        }},
        {DemoPhase::Phase6Text, {
            DemoPhase::Phase6Text,
            "Phase 6: Text-as-a-Sense",
            "Text processing, concept formation, social learning",
            "nlm_phase6_text_demo"
        }}
    };
    
    std::unordered_map<std::string, DemoPhase> phaseMap = {
        {"2", DemoPhase::Phase2},
        {"3", DemoPhase::Phase3},
        {"4", DemoPhase::Phase4},
        {"5", DemoPhase::Phase5},
        {"6", DemoPhase::Phase6},
        {"6t", DemoPhase::Phase6Text},
        {"phase2", DemoPhase::Phase2},
        {"phase3", DemoPhase::Phase3},
        {"phase4", DemoPhase::Phase4},
        {"phase5", DemoPhase::Phase5},
        {"phase6", DemoPhase::Phase6},
        {"phase6-text", DemoPhase::Phase6Text}
    };
    
public:
    DemoLauncher() {
        config = std::make_shared<Config>();
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    bool parseCommandLine(int argc, char** argv) {
        if (argc <= 1) {
            printHelp();
            return false;
        }
        
        std::string arg1 = argv[1];
        
        if (arg1 == "--help" || arg1 == "-h") {
            printHelp();
            return false;
        } else if (arg1 == "--list-phases") {
            listPhases();
            return false;
        } else if (arg1 == "--benchmark") {
            std::cout << "Running comprehensive benchmark..." << std::endl;
            // TODO: Run benchmark
            return false;
        } else if (arg1 == "--interactive") {
            std::cout << "Starting interactive mode..." << std::endl;
            return runInteractiveMode();
        }
        
        // Check for phase option
        DemoPhase phase = getPhaseFromArg(arg1);
        if (phase == DemoPhase::Phase2) {
            // Phase 2 demo (main.cpp)
            return runPhase2Demo(argc, argv);
        } else {
            std::cout << "ERROR: Demo for phase " << arg1 << " not yet implemented in launcher" << std::endl;
            std::cout << "Use --list-phases to see available demos" << std::endl;
            return false;
        }
    }
    
    DemoPhase getPhaseFromArg(const std::string& arg) {
        auto it = phaseMap.find(arg);
        if (it != phaseMap.end()) {
            return it->second;
        }
        return DemoPhase::Phase2; // Default to Phase 2
    }
    
    void printHelp() {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║                   NLM DEMO LAUNCHER - HELP                     ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
        
        std::cout << std::endl;
        std::cout << "USAGE:" << std::endl;
        std::cout << "  ./nlm_demo [OPTIONS] [PHASE]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "PHASE OPTIONS:" << std::endl;
        std::cout << "  --phase <number>    Run specific phase (2-6, 6t for text)" << std::endl;
        std::cout << "  2|3|4|5|6|6t        Short form phase selection" << std::endl;
        std::cout << "  phase2|phase3|...   Long form phase selection" << std::endl;
        std::cout << std::endl;
        
        std::cout << "GLOBAL OPTIONS:" << std::endl;
        std::cout << "  --help, -h          Show this help message" << std::endl;
        std::cout << "  --list-phases       List all available phases" << std::endl;
        std::cout << "  --benchmark         Run performance benchmarks" << std::endl;
        std::cout << "  --interactive       Start interactive mode" << std::endl;
        std::cout << std::endl;
        
        std::cout << "CONFIGURATION OPTIONS:" << std::endl;
        std::cout << "  --config <file>     Load configuration from file" << std::endl;
        std::cout << "  --neurons <count>   Set number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <count>     Set simulation steps (default: 1000)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "AVAILABLE PHASES:" << std::endl;
        for (const auto& pair : demoInfo) {
            const auto& info = pair.second;
            std::cout << "  " << info.name << std::endl;
            std::cout << "    " << info.description << std::endl;
        }
        std::cout << std::endl;
    }
    
    void listPhases() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    AVAILABLE NLM PHASES                       ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        int i = 1;
        for (const auto& pair : demoInfo) {
            const auto& info = pair.second;
            std::cout << std::setw(2) << i << ". " << info.name << std::endl;
            std::cout << "   " << info.description << std::endl;
            std::cout << "   Executable: " << info.executableName << std::endl;
            std::cout << std::endl;
            i++;
        }
        
        std::cout << "Short options: 2, 3, 4, 5, 6, 6t" << std::endl;
        std::cout << "Long options: phase2, phase3, phase4, phase5, phase6, phase6-text" << std::endl;
    }
    
    bool runPhase2Demo(int argc, char** argv) {
        std::cout << "Starting Phase 2 Demo: " << demoInfo[DemoPhase::Phase2].name << std::endl;
        std::cout << "This is the main demo executable (nlm)" << std::endl;
        
        // For now, just print the command that should be run
        std::cout << std::endl;
        std::cout << "To run Phase 2 demo, execute:" << std::endl;
        std::cout << "  ./nlm" << std::endl;
        std::cout << std::endl;
        
        // Parse any additional options
        bool helpRequested = false;
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--help" || arg == "-h") {
                helpRequested = true;
            } else if (arg.find("--") == 0) {
                std::cout << "Note: Option " << arg << " not yet implemented in launcher" << std::endl;
            }
        }
        
        if (helpRequested) {
            printHelp();
        }
        
        return true;
    }
    
    bool runInteractiveMode() {
        std::cout << "Interactive mode is not yet implemented." << std::endl;
        std::cout << "Available features coming soon:" << std::endl;
        std::cout << "  - Live brain control" << std::endl;
        std::cout << "  - Visualization control" << std::endl;
        std::cout << "  - Real-time monitoring" << std::endl;
        std::cout << "  - Configuration editing" << std::endl;
        return false;
    }
};

int main(int argc, char** argv) {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║                    NLM UNIFIED DEMO LAUNCHER                    ║
║                                                               ║
║              Running NLM Brain Demos Made Simple               ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
    
    DemoLauncher launcher;
    return launcher.parseCommandLine(argc, argv) ? 0 : 1;
}
