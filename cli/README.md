// NLM CLI Directory
// Contains all command-line tools for NLM interaction

// Main CLI structure:
// - nlm-cli.cpp: Main command-line interface dispatcher
// - nlm-debug.cpp: Advanced debugging and profiling tools
// - nlm-config.cpp: Configuration management tools (can be merged with main CLI)
// - nlm-experiments.cpp: Experiment-specific commands (can be merged with main CLI)
// - nlm-checkpoint.cpp: Checkpoint management tools (can be merged with main CLI)
// - nlm-visualize.cpp: Visualization tools (can be merged with main CLI)

// The current main CLI (nlm-cli.cpp) already includes most functionality.
// The debug tool (nlm-debug.cpp) provides advanced debugging capabilities.
// Other tools can be created by extending the CLI command structure.

// Usage:
// nlm <command> [options]
// nlm help          # Show general help
// nlm version       # Show version information
// nlm experiment    # Run NLM experiments
// nlm checkpoint    # Manage checkpoints
// nlm visualize     # Launch visualization interface
// nlm config        # Manage configuration
// nlm debug         # Launch advanced debugging tools
// nlm help <command> # Get help for specific command

// Key Features:
// - Comprehensive command-line interface with subcommands
// - Advanced debugging and profiling tools
// - Configuration management with JSON/YAML support
// - Checkpoint save/load functionality
// - Experiment running (Phases 2-6)
// - Visualization interface
// - Error handling and logging
// - Extensible architecture for custom commands

// The NLM CLI is designed to provide a unified interface for:
// 1. Running NLM experiments and simulations
// 2. Managing brain checkpoints and saved states
// 3. Configuring NLM parameters and settings
// 4. Debugging and profiling NLM systems
// 5. Visualizing NLM network dynamics
// 6. Analyzing performance and behavior
//
// All commands follow a consistent pattern:
// nlm <command> [options]
// Options are command-specific and can be viewed with nlm help <command>
