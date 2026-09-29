#include "NLMCLI.hpp"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    // Create CLI instance
    auto cli = std::make_shared<nlm::NLMCLI>();
    
    // Register commands (this would typically be done by loading command plugins)
    // For now, we'll register a few example commands
    
    // Create command instances
    auto runCommand = std::make_unique<nlm::RunCommand>(cli);
    auto configCommand = std::make_unique<nlm::ConfigCommand>(cli);
    auto experimentCommand = std::make_unique<nlm::ExperimentCommand>(cli);
    auto benchmarkCommand = std::make_unique<nlm::BenchmarkCommand>(cli);
    auto analysisCommand = std::make_unique<nlm::AnalysisCommand>(cli);
    auto visualizationCommand = std::make_unique<nlm::VisualizationCommand>(cli);
    
    // Register commands
    cli->registerCommand("run", std::move(runCommand));
    cli->registerCommand("config", std::move(configCommand));
    cli->registerCommand("experiment", std::move(experimentCommand));
    cli->registerCommand("benchmark", std::move(benchmarkCommand));
    cli->registerCommand("analyze", std::move(analysisCommand));
    cli->registerCommand("visualize", std::move(visualizationCommand));
    
    // Run CLI
    return cli->run(argc, argv);
}
