// CommandRegistry.cpp implementation
CommandRegistry commands::commandRegistry;

void registerAllCommands() {
    // Register all available commands with their dependencies
    
    // Create a default config for commands that need it
    auto defaultConfig = std::make_shared<Config>();
    defaultConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    defaultConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    defaultConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    defaultConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Create a default brain for commands that need it (will be re-initialized when needed)
    auto defaultBrain = std::make_shared<Brain>(defaultConfig);
    
    // Register BrainRunnerCommand (has its own config and brain)
    commands::commandRegistry.registerCommand<BrainRunnerCommand>(
        "brainrunner",
        "Run all default tests: connectivity, plasticity, and STDP verification",
        defaultConfig
    );
    
    // Register ConnectivityTestCommand
    commands::commandRegistry.registerCommand<ConnectivityTestCommand>(
        "connectivity",
        "Run basic neural connectivity test",
        defaultBrain
    );
    
    // Register PlasticityExperimentCommand
    commands::commandRegistry.registerCommand<PlasticityExperimentCommand>(
        "plasticity",
        "Run plasticity learning experiment",
        defaultBrain
    );
    
    // Register StdpVerificationCommand
    commands::commandRegistry.registerCommand<StdpVerificationCommand>(
        "stdp",
        "Verify STDP plasticity rule works",
        defaultBrain
    );
    
    // Register ExportCommand (requires filename argument)
    commands::commandRegistry.registerCommand<ExportCommand>(
        "export",
        "Export simulation data to file",
        std::string("simulation_data.json") // Default filename
    );
    
    // Register ImportCommand (requires filename argument)
    commands::commandRegistry.registerCommand<ImportCommand>(
        "import",
        "Import simulation data from file",
        std::string("simulation_data.json") // Default filename
    );
    
    // Register ConfigCommand (no arguments)
    commands::commandRegistry.registerCommand<ConfigCommand>(
        "config",
        "Display or modify configuration"
    );
    
    // Register HelpCommand (requires registry reference)
    commands::commandRegistry.registerCommand<HelpCommand>(
        "help",
        "Show help information about available commands",
        &commands::commandRegistry
    );
}