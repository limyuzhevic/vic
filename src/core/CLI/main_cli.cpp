// NLM Main CLI Application
// Entry point for the enhanced NLM command-line interface

#include "core/CLI/CLIFramework.hpp"
#include "core/CLI/CommandHandlers.hpp"
#include <iostream>
#include <string>
#include <memory>

// Forward declarations for command handlers
void brainConfigCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void stateManagerCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void performanceCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void diagnosticsCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void exportImportCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void batchCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void monitorCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void experimentCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void optimizeCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void visualizeCommand(const std::map<std::string, std::string>& args, CLIParser& parser);
void analyzeCommand(const std::map<std::string, std::string>& args, CLIParser& parser);

int main(int argc, char** argv) {
    // Create CLI parser
    CLIParser parser;
    
    // Register commands
    parser.addCommand("brain-config", "Configure brain parameters", 
                     "brain-config [--neuron-count N] [--region-count R] [--connection-probability P] [--random-seed S] [--output FILE]",
                     [&](const std::map<std::string, std::string>& args) { brainConfigCommand(args, parser); });
    
    parser.addCommand("state-manager", "Save/load/checkpoint brain state", 
                     "state-manager <save|load|checkpoint|list> <brain-file> [--brain-file FILE]",
                     [&](const std::map<std::string, std::string>& args) { stateManagerCommand(args, parser); });
    
    parser.addCommand("performance", "Performance monitoring and profiling", 
                     "performance [--duration D] [--output FILE]",
                     [&](const std::map<std::string, std::string>& args) { performanceCommand(args, parser); });
    
    parser.addCommand("diagnostics", "Diagnostic and debugging tools", 
                     "diagnostics [--output FILE]",
                     [&](const std::map<std::string, std::string>& args) { diagnosticsCommand(args, parser); });
    
    parser.addCommand("export-import", "Export/import brain configurations", 
                     "export-import <export|import> <source> <destination> [--source S] [--destination D]",
                     [&](const std::map<std::string, std::string>& args) { exportImportCommand(args, parser); });
    
    parser.addCommand("batch", "Batch processing for multiple simulations", 
                     "batch <mode> <count> [--mode M] [--count C] [--output FILE]",
                     [&](const std::map<std::string, std::string>& args) { batchCommand(args, parser); });
    
    parser.addCommand("monitor", "Real-time monitoring interfaces", 
                     "monitor [--output FILE] [--format JSON]",
                     [&](const std::map<std::string, std::string>& args) { monitorCommand(args, parser); });
    
    parser.addCommand("experiment", "Automated experiment running", 
                     "experiment <type> <config> [--type T] [--config C] [--output FILE]",
                     [&](const std::map<std::string, std::string>& args) { experimentCommand(args, parser); });
    
    parser.addCommand("optimize", "Parameter optimization tools", 
                     "optimize <method> [--method M] [--output FILE]",
                     [&](const std::map<std::string, std::string>& args) { optimizeCommand(args, parser); });
    
    parser.addCommand("visualize", "Visualization interfaces", 
                     "visualize <type> <output> [--type T] [--output O] [--format F]",
                     [&](const std::map<std::string, std::string>& args) { visualizeCommand(args, parser); });
    
    parser.addCommand("analyze", "Statistical analysis tools", 
                     "analyze <type> [--type T] [--output FILE] [--format F]",
                     [&](const std::map<std::string, std::string>& args) { analyzeCommand(args, parser); });
    
    // Parse command line arguments
    if (!parser.parse(argc, argv)) {
        return 1; // Error occurred
    }
    
    return 0;
}

// Command implementations

void brainConfigCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    BrainConfigCommand::handle(args, parser);
}

void stateManagerCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    StateManagerCommand::handle(args, parser);
}

void performanceCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    PerformanceCommand::handle(args, parser);
}

void diagnosticsCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    DiagnosticsCommand::handle(args, parser);
}

void exportImportCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    ExportImportCommand::handle(args, parser);
}

void batchCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    BatchCommand::handle(args, parser);
}

void monitorCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    MonitorCommand::handle(args, parser);
}

void experimentCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    ExperimentCommand::handle(args, parser);
}

void optimizeCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    OptimizeCommand::handle(args, parser);
}

void visualizeCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    VisualizeCommand::handle(args, parser);
}

void analyzeCommand(const std::map<std::string, std::string>& args, CLIParser& parser) {
    AnalyzeCommand::handle(args, parser);
}
