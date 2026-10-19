NLM (熙然) Phase 6 Integration - Command Line Interface

Usage: nlm [OPTIONS] [ARGS...]

This command-line interface provides access to all NLM functionality with comprehensive phase support.

Options:
    -h, --help              Show this help message
    -v, --version           Show version and build information
    --phase <PHASE>         Specify phase (3|4|5|6|demo|all)
    --config <FILE>         Load configuration from file
    --list-configs          List available configuration files
    --demo                  Run demonstration instead of main simulation
    --quiet, -q             Quiet mode (minimal output)
    --verbose, -v           Verbose mode (detailed output)
    --performance           Show performance metrics

Phase-specific options:
    For phase 6 (default):
        --brain-size <N>       Number of neurons (default: 1000)
        --steps <N>            Simulation steps (default: 10000)
        --checkpoint <FILE>    Checkpoint file path (default: ./checkpoint.bin)
        --dev-enable           Enable development system
        --replay-enable        Enable replay system

    For phases 3-5:
        These phases run legacy demonstrations

Arguments:
    ARGS...                 Additional arguments passed to the simulation

Phase Support:
    phase 3: Basic world interaction with spiking neurons
    phase 4: Integrated agent with cognitive capabilities  
    phase 5: Performance optimizations and memory systems
    phase 6: Final integration with all brain systems
    demo: Quick integration verification
    all: Run all phases sequentially

Examples:
    nlm                    # Run Phase 6 with default settings
    nlm --phase 3          # Run Phase 3 demonstration
    nlm --phase 6 --brain-size 2000 --steps 5000
    nlm --demo             # Quick integration test
    nlm --config myconfig.cfg

Features:
    - Automatic configuration loading and merging
    - Real-time progress reporting
    - Comprehensive error handling
    - Cross-platform compatibility
    - Python bindings support (pynlm)

Built with NLM (熙然) - Experimental Artificial Developmental Brain
Phase 6: Final Integration - All Systems Connected
