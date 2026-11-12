// NLM Subcommands Implementation
// Contains all subcommand implementations

#include "nlm/Subcommands/RunCommand.hpp"
#include "nlm/Subcommands/AnalyzeCommand.hpp"
#include "nlm/Subcommands/ExportCommand.hpp"
#include "nlm/Subcommands/VisualizeCommand.hpp"
#include "nlm/Subcommands/ConfigCommand.hpp"
#include "nlm/Subcommands/ToolsCommand.hpp"

namespace nlm {
    
    // Register all commands
    void registerAllCommands() {
        REGISTER_COMMAND(run, 
            "Run demonstrations and experiments",
            "Run NLM demonstrations and experiments. The run command supports multiple modes:\n\n"
            "  nlm run phase3 [options]    - Run Phase 3 world interaction demo\n"
            "  nlm run phase4 [options]    - Run Phase 4 cognitive emergence demo\n"
            "  nlm run phase5 [options]    - Run Phase 5 personality/sandbox demo\n"
            "  nlm run phase6 [options]    - Run Phase 6 integration demo\n"
            "  nlm run benchmark [options] - Run performance benchmarks\n\n"
            "Options:\n"
            "  --episodes N         Number of episodes (default: 5)\n"
            "  --steps N            Number of steps per episode (default: 200)\n"
            "  --help               Show this help message",
            []() { return std::make_unique<RunCommand>(); });
        
        REGISTER_COMMAND(analyze,
            "Analyze brain state and activity",
            "Analyze brain state, neural activity, and performance metrics.\n\n"
            "Usage: nlm analyze <mode> [options]\n\n"
            "Modes:\n"
            "  connectivity     Analyze neural connectivity patterns\n"
            "  plasticity       Analyze synaptic plasticity\n"
            "  stdp            Verify STDP mechanism\n"
            "  learning        Analyze learning progress\n"
            "  memory          Analyze memory systems\n"
            "  all             Run all analyses\n\n"
            "Options:\n"
            "  --help           Show this help message",
            []() { return std::make_unique<AnalyzeCommand>(); });
        
        REGISTER_COMMAND(export,
            "Export data and results",
            "Export brain data, experiment results, and configurations.\n\n"
            "Formats:\n"
            "  json             Export as JSON format\n"
            "  csv              Export as CSV format\n"
            "  protobuf         Export as protobuf\n"
            "  txt              Export as text format\n\n"
            "Targets:\n"
            "  brain            Export brain state\n"
            "  config           Export configuration\n"
            "  metrics          Export performance metrics\n"
            "  all              Export all data\n\n"
            "Options:\n"
            "  --output FILE    Output file path\n"
            "  --format FORMAT  Output format (default: txt)\n"
            "  --help           Show this help message",
            []() { return std::make_unique<ExportCommand>(); });
        
        REGISTER_COMMAND(visualize,
            "Visualize brain data and results",
            "Create visualizations of brain activity and experiment results.\n\n"
            "Types:\n"
            "  neurons          Visualize neuron activity\n"
            "  connections     Visualize neural connections\n"
            "  spikes          Visualize spike patterns\n"
            "  memory          Visualize memory traces\n"
            "  plasticity      Visualize synaptic changes\n"
            "  all             Create all visualizations\n\n"
            "Outputs:\n"
            "  --plot           Create interactive plots\n"
            "  --video          Create animation video\n"
            "  --svg            Export as SVG\n"
            "  --html           Create web-based visualization\n\n"
            "Options:\n"
            "  --output DIR     Output directory\n"
            "  --width N        Plot width (default: 800)\n"
            "  --height N       Plot height (default: 600)\n"
            "  --help           Show this help message",
            []() { return std::make_unique<VisualizeCommand>(); });
        
        REGISTER_COMMAND(config,
            "Manage configuration",
            "Manage NLM configuration settings.\n\n"
            "Commands:\n"
            "  list            List all configuration options\n"
            "  get <key>       Get a configuration value\n"
            "  set <key> <val> Set a configuration value\n"
            "  reset           Reset to default configuration\n"
            "  load <file>     Load configuration from file\n"
            "  save <file>     Save configuration to file\n"
            "  validate        Validate current configuration\n\n"
            "Options:\n"
            "  --help          Show this help message",
            []() { return std::make_unique<ConfigCommand>(); });
        
        REGISTER_COMMAND(tools,
            "Advanced tools and utilities",
            "Advanced tools and utilities for brain research.\n\n"
            "Tools:\n"
            "  --checkpoint    Create brain checkpoint\n"
            "  --replay        Replay from checkpoint\n"
            "  --perturb      Apply perturbations\n"
            "  --stimulus      Apply external stimuli\n"
            "  --analyze       Run detailed analysis\n"
            "  --simulate      Run simulation with custom settings\n\n"
            "Options:\n"
            "  --help          Show this help message",
            []() { return std::make_unique<ToolsCommand>(); });
    }
    
} // namespace nlm

// Initialize command registration when the library is loaded
void initializeCLI() {
    static bool initialized = false;
    if (!initialized) {
        nlm::registerAllCommands();
        initialized = true;
    }
}

// Entry point for command registration
void nlm_cli_register() {
    initializeCLI();
}