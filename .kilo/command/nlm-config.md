# Kilo Command System
# Provides command-line interface to NLM for easier use and automation

# Command configuration template
command_config = {
    name = "nlm",
    version = "1.0.0",
    author = "NLM Research Team",
    description = "Neural Learning Machine - Brain Simulator",
    
    commands = [
        "run", "simulate", "experiment", "config", "status", "help", "version"
    ],
    
    flags = {
        "--config": "Configuration file path",
        "--neurons": "Number of neurons",
        "--steps": "Number of simulation steps",
        "--headless": "Run without GUI",
        "--verbose": "Enable verbose output",
        "--debug": "Enable debug mode",
        "--integration": "Run integration tests",
        "--quick": "Quick test run"
    },
    
    subcommands = {
        "run": "Run a simulation with specified parameters",
        "simulate": "Alias for run",
        "experiment": "Run specific experiment",
        "config": "Manage configuration files",
        "status": "Show system status",
        "help": "Show help information",
        "version": "Show version information"
    }
}
