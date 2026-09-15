#include "BrainCLI.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // Create CLI application
    nlm::cli::CLIApplication app("NLM Brain CLI", "2.0.0");
    
    // Add core brain operations commands
    app.addCommand(new nlm::cli::CreateBrainCommand());
    app.addCommand(new nlm::cli::InitializeBrainCommand());
    app.addCommand(new nlm::cli::StepBrainCommand());
    app.addCommand(new nlm::cli::SaveBrainCommand());
    app.addCommand(new nlm::cli::LoadBrainCommand());
    app.addCommand(new nlm::cli::BrainStatusCommand());
    
    // Add advanced brain management commands
    app.addCommand(new nlm::cli::CloneBrainCommand());
    app.addCommand(new nlm::cli::CompareBrainsCommand());
    app.addCommand(new nlm::cli::BatchBrainCommand());
    
    // Add visualization commands
    app.addCommand(new nlm::cli::VisualizeNeuralNetworkCommand());
    app.addCommand(new nlm::cli::ExportBrainDataCommand());
    app.addCommand(new nlm::cli::ProgressMonitorCommand());
    
    // Add analysis commands
    app.addCommand(new nlm::cli::PatternRecognitionCommand());
    app.addCommand(new nlm::cli::LearningTrajectoryCommand());
    app.addCommand(new nlm::cli::PerformanceBenchmarkCommand());
    app.addCommand(new nlm::cli::StatisticalAnalysisCommand());
    
    // Add collaboration commands
    app.addCommand(new nlm::cli::ShareBrainCommand());
    app.addCommand(new nlm::cli::ImportExportConfigCommand());
    app.addCommand(new nlm::cli::CommunityMarketplaceCommand());
    
    // Add plugin commands
    app.addCommand(new nlm::cli::PluginListCommand());
    app.addCommand(new nlm::cli::PluginLoadCommand());
    app.addCommand(new nlm::cli::PluginMarketplaceCommand());
    
    // Handle command line arguments
    bool batchMode = false;
    std::string scriptFile;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--batch" || arg == "-b") {
            batchMode = true;
        } else if (arg.find("--script=") == 0) {
            scriptFile = arg.substr(9);
        } else if (arg == "--help" || arg == "-h") {
            app.showHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            app.showVersion();
            return 0;
        } else if (arg == "--quiet" || arg == "-q") {
            app.enableLogging(false);
        } else {
            // Pass remaining args to the application for parsing
            // In a real implementation, these would be parsed
        }
    }
    
    // Run in batch or interactive mode
    if (batchMode) {
        if (scriptFile.empty()) {
            std::cout << "Error: No script file specified for batch mode" << std::endl;
            app.showHelp();
            return 1;
        }
        return app.runBatch(scriptFile) ? 0 : 1;
    } else {
        return app.runInteractive() ? 0 : 1;
    }
}
