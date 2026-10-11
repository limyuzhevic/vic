// CommandParser implementation
#include "nlm_cli.cpp"
#include <algorithm>
#include <cctype>

std::string CommandParser::Command::aliasesToString() const {
    std::string result;
    for (size_t i = 0; i < aliases.size(); ++i) {
        if (i > 0) result += ", ";
        result += aliases[i];
    }
    return result;
}

void CommandParser::initializeCommands() {
    // Basic Simulation Control
    registerCommand("start", "Start the simulation", "start [duration] [steps]", 
                   "Begin or resume neural simulation. Without arguments, runs in auto-mode. "
                   "With duration/steps, runs for specified time or step count.", 
                   {"run", "begin"}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->startSimulation(args); 
                   });
    
    registerCommand("stop", "Stop the simulation", "stop", 
                   "Stop the currently running simulation. Saves current state as last checkpoint.", 
                   {"pause", "halt"}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->stopSimulation(); 
                   });
    
    registerCommand("step", "Execute one simulation step", "step [steps]", 
                   "Execute one or more simulation steps manually. Useful for debugging and observation.", 
                   {"tick"}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->stepSimulation(args); 
                   });
    
    registerCommand("run", "Run automated simulation", "run <duration> [steps]", 
                   "Run simulation for a specified duration (e.g., '1s', '5m', '2h') or step count. "
                   "Can be interrupted with 'stop'.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->runAutoSimulation(args); 
                   });
    
    // Advanced Configuration
    registerCommand("configure", "Configure brain parameters", "configure <key> <value>", 
                   "Set brain configuration parameters. Common keys: neuron_count, connection_probability, "
                   "stdp_ltp_weight, stdp_ltd_weight, simulation_timestep, region_count.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->configureBrain(args); 
                   });
    
    registerCommand("inspect", "Inspect brain state and components", "inspect <component>", 
                   "Inspect brain components: neurons, synapses, memory, prediction, neuromodulation. "
                   "Without component shows overall status.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->inspectBrain(args); 
                   });
    
    registerCommand("save", "Save brain state to checkpoint", "save <filename>", 
                   "Save current brain state to a checkpoint file. Use 'load <filename>' to restore.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->saveBrainState(args); 
                   });
    
    registerCommand("load", "Load brain state from checkpoint", "load <filename>", 
                   "Load brain state from a checkpoint file. Resets current simulation state.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->loadBrainState(args); 
                   });
    
    registerCommand("neuromodulate", "Control neuromodulation signals", "neuromodulate <type> <intensity>", 
                   "Apply neuromodulatory signals: dopamine, curiosity, novelty, prediction_error. "
                   "Intensity ranges from 0.0 to 1.0.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->applyNeuromodulation(args); 
                   });
    
    registerCommand("memory", "Manipulate working memory", "memory <action> [data]", 
                   "Working memory operations: store, recall, clear, list, capacity. "
                   "Data can be string or numeric values.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->manipulateWorkingMemory(args); 
                   });
    
    // Performance and Monitoring
    registerCommand("stats", "Display real-time statistics", "stats [refresh_interval]", 
                   "Display live simulation statistics including neuron counts, firing rates, "
                   "synaptic activity, and memory usage. Press Ctrl+C to stop monitoring.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->displayStatistics(args); 
                   });
    
    registerCommand("profile", "Performance profiling", "profile <duration> [output_file]", 
                   "Profile simulation performance over time. Collects timing data, "
                   "memory usage, and event queue statistics for optimization analysis.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->profilePerformance(args); 
                   });
    
    registerCommand("monitor", "Monitor system resources", "monitor [interval]", 
                   "Monitor system resources: memory usage, CPU utilization, event queue depth. "
                   "Provides real-time performance metrics.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->monitorSystem(args); 
                   });
    
    // Configuration Management
    registerCommand("config", "Configuration management", "config <action> <file> [name]", 
                   "Manage configuration files: load, save, list, switch between profiles. "
                   "Useful for reproducible experiments.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->manageConfiguration(args); 
                   });
    
    registerCommand("set", "Set runtime parameter", "set <key> <value>", 
                   "Set runtime configuration parameter that affects current simulation. "
                   "Changes take effect immediately.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->setRuntimeParameter(args); 
                   });
    
    registerCommand("profile", "Profile a brain configuration", "profile <config_name>", 
                   "Run simulation with a named configuration to compare performance "
                   "and determine optimal settings.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->profilePerformance(args); 
                   });
    
    // Development and Testing
    registerCommand("test", "Run unit tests", "test [test_name]", 
                   "Run NLM unit tests. Without argument runs all tests. "
                   "With test_name runs specific test suite.", {}, false, true,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->runTests(args); 
                   });
    
    registerCommand("benchmark", "Run performance benchmarks", "benchmark <neuron_count> <steps>", 
                   "Run performance benchmarks with specified network size. Measures "
                   "steps/sec, memory usage, and computational efficiency.", {}, false, true,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->runBenchmark(args); 
                   });
    
    registerCommand("leaktest", "Memory leak detection", "leaktest <duration> [threshold]", 
                   "Detect memory leaks during simulation. Monitors memory allocation/deallocation "
                   "patterns to identify resource leaks over time.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->detectMemoryLeaks(args); 
                   });
    
    registerCommand("integrate", "Integration testing", "integrate <test_type>", 
                   "Run integration tests to verify component interactions: memory_systems, "
                   "neuromodulation, prediction, all_systems. Ensures complete brain functionality.", {}, false, true,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<SimulationController*>(controller);
                       return ctrl->runIntegrationTests(args); 
                   });
    
    // Utility Commands
    registerCommand("help", "Show command help", "help [command]", 
                   "Display help information for commands or general CLI usage. "
                   "Without argument shows all available commands.", {}, false, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<NLMCLIApp*>(controller);
                       ctrl->displayHelp((args.empty() ? "" : args[0]));
                       return true; 
                   });
    
    registerCommand("exit", "Exit the application", "exit", 
                   "Save current state and exit the NLM CLI. Use 'quit' or Ctrl+D as well.", {}, false, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<NLMCLIApp*>(controller);
                       return !ctrl->exitApplication(); 
                   });
    
    registerCommand("quit", "Exit the application", "quit", 
                   "Alias for 'exit'. Save current state and exit the NLM CLI.", {}, false, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<NLMCLIApp*>(controller);
                       return !ctrl->exitApplication(); 
                   });
    
    registerCommand("clear", "Clear the screen", "clear", 
                   "Clear the terminal screen. Useful for keeping output manageable during long simulations.", {}, false, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<NLMCLIApp*>(controller);
                       return ctrl->clearScreen(); 
                   });
    
    registerCommand("version", "Show version information", "version", 
                   "Display NLM version and build information, including compilation date and features.", {}, false, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<NLMCLIApp*>(controller);
                       return ctrl->displayVersion(); 
                   });
    
    registerCommand("status", "Show current simulation status", "status", 
                   "Display current simulation state including brain status, configuration, "
                   "performance metrics, and active processes.", {}, true, false,
                   [](const std::vector<std::string>& args, void* controller) { 
                       auto ctrl = static_cast<NLMCLIApp*>(controller);
                       return ctrl->showStatus(); 
                   });
}