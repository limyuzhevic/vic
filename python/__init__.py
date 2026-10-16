"""
NLM (Neural Learning Machine) Python Bindings

This package provides Python bindings for the NLM C++ neural simulation framework.

Main modules:
- pynlm: Core neural simulation functionality (C++ bindings via pybind11)
- unified_cli: Advanced CLI tool for power users with comprehensive command support

The unified CLI provides 8 command categories with 40+ advanced commands:
1. Debug and Introspection (--inspect, --debug, --trace, --benchmark, --validate)
2. Configuration Management (--export-config, --import-config, --merge-config, --compare-config, --generate-config)
3. Brain State Management (--checkpoint, --restore, --snapshot, --replay, --diff)
4. Performance and Profiling (--profile, --analyze, --monitor, --stats, --diagnose)
5. Batch Processing and Automation (--batch, --schedule, --loop, --script, --pipeline)
6. Export/Import (--export-brain, --import-brain, --export-data, --import-data, --export-visualization)
7. System Administration (--system-info, --memory, --processes, --logs, --security)
8. Research and Analysis (--experiment-design, --parameter-sweep, --cross-validation, --ensemble, --sensitivity-analysis)

Usage:
    python -m pynlm --help          # Show basic help
    python -m pynlm info            # Display system information
    python -m pynlm unified_cli.py  # Start advanced CLI with all commands
"""

from .bindings import *
from .unified_cli import main, CLIParser

__all__ = [
    'create_default_config',
    'create_brain', 
    'create_simple_world',
    'create_agent_brain',
    'create_simple_brain',
    'run_simulation',
    'setup_experiment',
    'save_brain_state',
    'load_brain_state',
    'benchmark_brain',
    'benchmark_simulation',
    'get_version',
    'main',
    'CLIParser'
]
