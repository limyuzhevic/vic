============================================
NLM - Neural Learning Machine CLI
============================================

Basic Usage:
  ./nlm_cli run [options]
  ./nlm_cli info
  ./nlm_cli config
  ./nlm_cli check
  ./nlm_cli benchmark

Commands:
  run        Run an NLM simulation with specified configuration
  info        Display system information and capabilities
  config      Manage configuration settings
  check       Check system status and validate installation
  benchmark   Run performance benchmarks

Examples:
  ./nlm_cli run --neurons 1000 --steps 5000
  ./nlm_cli config set default_neurons 2000
  ./nlm_cli benchmark run

============================================

Run Phase 2 - Basic Neural Computation:
  ./nlm_cli run phase2 --neurons 1000 --steps 1000

Run Phase 3 - World Interaction Loop:
  ./nlm_cli run phase3 --neurons 500 --steps 2000

Run Phase 4 - Advanced Cognition:
  ./nlm_cli run phase4 --neurons 1000 --steps 3000

Run Phase 5 - Brain-Personality & Sandbox:
  ./nlm_cli run phase5

Run Phase 6 - Final Integration:
  ./nlm_cli run phase6 --neurons 1000 --steps 5000 --checkpoint

Run Phase 6 with Text-as-a-Sense:
  ./nlm_cli run phase6_text --neurons 1000 --steps 5000

============================================

Configuration Commands:
  ./nlm_cli config show                    - Show current settings
  ./nlm_cli config set default_neurons N    - Set default neuron count
  ./nlm_cli config set default_steps S      - Set default simulation steps
  ./nlm_cli config set world_width W        - Set world width
  ./nlm_cli config set world_height H       - Set world height
  ./nlm_cli config set vision_size V        - Set vision sensor count
  ./nlm_cli config set reward_mod true|false - Enable/disable reward modulation
  ./nlm_cli config set curiosity true|false - Enable/disable curiosity
  ./nlm_cli config set checkpoint true|false - Enable/disable checkpointing
  ./nlm_cli config set development true|false - Enable/disable development
  ./nlm_cli config reset                   - Reset to defaults

============================================

Information Commands:
  ./nlm_cli info                           - Display system info and capabilities
  ./nlm_cli check                          - Check system status

============================================

Benchmark Commands:
  ./nlm_cli benchmark run                  - Run performance benchmarks

============================================

Advanced Options (available with --help):
  --help                                 - Show this help message
  --verbose                              - Enable verbose output
  --checkpoint-path PATH                 - Set checkpoint file path
  --config CONFIG_FILE                   - Use custom configuration file

============================================

The NLM CLI provides easy access to all NLM demos and advanced configuration
options for users who want more control over their neural simulations.
