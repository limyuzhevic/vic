# Configuration parameter documentation
# This file documents all available configuration parameters for NLM

# Simulation parameters
simulation.neuron_count
  Description: Number of neurons in the brain
  Type: int64
  Default: 1000
  Range: 100 - 10000
  
simulation.region_count
  Description: Number of brain regions
  Type: int64
  Default: 1
  Range: 1 - 10
  

simulation.connection_probability
  Description: Probability of synapse formation between neurons
  Type: float
  Default: 0.1
  Range: 0.01 - 0.5
  
# Plasticity parameters
plasticity.stdp_ltp_weight
  Description: Spike-timing-dependent plasticity long-term potentiation weight
  Type: float
  Default: 0.01
  Range: 0.001 - 0.1
  
plasticity.stdp_ltd_weight
  Description: Spike-timing-dependent plasticity long-term depression weight
  Type: float
  Default: 0.012
  Range: 0.001 - 0.1
  
plasticity.stdp_tau
  Description: STDP time constant (in simulation steps)
  Type: float
  Default: 20.0
  Range: 5.0 - 100.0
  
plasticity.synaptogenesis_rate
  Description: Rate of new synapse formation
  Type: float
  Default: 0.0001
  Range: 0.00001 - 0.001
  
plasticity.pruning_rate
  Description: Rate of synapse elimination
  Type: float
  Default: 0.00001
  Range: 0.000001 - 0.0001
  
# Development parameters
development.enable_development
  Description: Enable developmental processes
  Type: bool
  Default: true
  
# Memory parameters
memory.working_memory_capacity
  Description: Capacity of working memory in neuron traces
  Type: int64
  Default: 100
  Range: 10 - 1000
  
memory.max_episodes
  Description: Maximum number of episodic memories to store
  Type: int64
  Default: 1000
  Range: 100 - 10000
  
# Neuromodulation parameters
neuromodulation.enable_curiosity
  Description: Enable curiosity-driven exploration
  Type: bool
  Default: true
  
neuromodulation.enable_novelty
  Description: Enable novelty detection
  Type: bool
  Default: true
  
# Visualization parameters
visualization.enable_viz
  Description: Enable visualization output
  Type: bool
  Default: false
  
visualization.viz_update_rate
  Description: Rate of visualization updates (Hz)
  Type: float
  Default: 0.1
  Range: 0.01 - 10.0
  
# Checkpoint parameters
checkpoint.enable_checkpointing
  Description: Enable checkpoint save/load functionality
  Type: bool
  Default: true
  
checkpoint.checkpoint_interval
  Description: Steps between checkpoints
  Type: int64
  Default: 1000
  Range: 100 - 10000
  
checkpoint.checkpoint_dir
  Description: Directory for checkpoint files
  Type: string
  Default: "./checkpoints"
  
checkpoint.max_checkpoints
  Description: Maximum number of checkpoints to keep
  Type: int64
  Default: 10
  Range: 1 - 100
  
# Logging parameters
logging.log_level
  Description: Logging verbosity level
  Type: string
  Default: "info"
  Values: "error", "warning", "info", "debug"
  
logging.log_file
  Description: File for log output
  Type: string
  Default: "nlm.log"
  
logging.console_output
  Description: Enable console logging
  Type: bool
  Default: true
  
# Experiment presets
preset.cognitive
  Description: Cognitive learning experiment configuration
  simulation.neuron_count = 1000
  simulation.connection_probability = 0.15
  plasticity.stdp_ltp_weight = 0.02
  plasticity.stdp_ltd_weight = 0.018
  development.enable_development = true
  memory.working_memory_capacity = 150
  

preset.social
  Description: Social learning experiment configuration
  simulation.neuron_count = 800
  simulation.connection_probability = 0.12
  plasticity.stdp_ltp_weight = 0.015
  plasticity.stdp_ltd_weight = 0.014
  development.enable_development = true
  memory.max_episodes = 500
  neuromodulation.enable_curiosity = true
  

preset.developmental
  Description: Developmental maturation experiment configuration
  simulation.neuron_count = 600
  simulation.connection_probability = 0.1
  plasticity.stdp_ltp_weight = 0.01
  plasticity.stdp_ltd_weight = 0.012
  development.enable_development = true
  checkpoint.enable_checkpointing = true
  checkpoint.checkpoint_interval = 500
  

preset.performance
  Description: High-performance simulation configuration
  simulation.neuron_count = 2000
  simulation.connection_probability = 0.05
  plasticity.stdp_ltp_weight = 0.008
  plasticity.stdp_ltd_weight = 0.009
  development.enable_development = false
  checkpoint.enable_checkpointing = false
  logging.log_level = "warning"
  
# Command-line parameter shortcuts
param.shortcuts
  Description: Common parameter shortcuts
  neuron-count=500 (quick test)
  max-steps=1000 (short simulation)
  enable-dev=false (disable development)
  no-viz=true (disable visualization)
  
# Advanced usage notes
notes.advanced
  Description: For advanced users
  
  Use --param to override any configuration parameter from command line
  Example: nlm --experiment cognitive --param neuron-count=2000 --param logging-level=debug
  
  Configuration files can be extended with custom sections for user-defined parameters
  Use checkpoint system to save/restore brain state
  
  For complex batch processing, use batch files with multiple experiment configurations
  
  Performance tips: Use preset.performance for large-scale simulations
  Use --quiet for reduced output in production runs
  
  For debugging, use --log file.log and --verbose flags
  
# Example batch file format (for --batch option)
# Each line: experiment_name,parameter1=value,parameter2=value
#cognitive,neuron-count=1000,steps=2000
#social,neuron-count=800,enable-curiosity=true
#developmental,neuron-count=600,enable-dev=true
  
# Custom preset examples
custom.quick_test
  simulation.neuron_count = 100
  simulation.connection_probability = 0.2
  max_steps = 500
  

custom.large_scale
  simulation.neuron_count = 5000
  simulation.connection_probability = 0.05
  development.enable_development = false
  checkpoint.enable_checkpointing = true
  checkpoint.checkpoint_interval = 5000
  logging.log_level = "warning"
  

custom.research
  simulation.neuron_count = 2000
  simulation.connection_probability = 0.15
  plasticity.stdp_ltp_weight = 0.025
  plasticity.stdp_ltd_weight = 0.023
  development.enable_development = true
  memory.working_memory_capacity = 300
  memory.max_episodes = 3000
  neuromodulation.enable_curiosity = true
  neuromodulation.enable_novelty = true
  checkpoint.enable_checkpointing = true
  checkpoint.checkpoint_interval = 2000
  visualization.enable_viz = true
  
# Performance optimization hints
hints.performance
  
  For faster execution:
  - Reduce neuron_count (e.g., 100 for quick tests)
  - Set enable_development=false for stable phases
  - Use preset.performance for production
  
  For more detailed output:
  - Set logging.log_level=debug
  - Add --verbose flags
  - Enable visualization for complex experiments
  
  For long-running experiments:
  - Use checkpointing with regular intervals
  - Monitor progress with --verbose output
  - Consider using batch mode for parameter sweeps
  
# Troubleshooting tips
troubleshooting
  
  "No spikes detected" errors:
  - Increase neuron-count or connection-probability
  - Check plasticity parameters are enabled
  - Verify configuration file is loaded correctly
  
  "Memory exceeded" errors:
  - Reduce neuron-count or max_episodes
  - Disable checkpointing if not needed
  - Increase available system memory
  
  "Configuration file not found":
  - Ensure config file path is correct
  - Use --config to specify alternative location
  - Run with --help for usage information
  
  For experimental logs and issues:
  - Enable --log to file
  - Use --verbose for detailed output
  - Check nlm_cli.log for CLI-specific issues
  
# File format notes
notes.format
  
  Configuration files use simple key=value format
  Sections use [section_name] notation for organization
  Comments start with #
  Empty lines are ignored
  Values can use scientific notation (e.g., 1.5e-3)
  Boolean values can be: true/false, 1/0, yes/no
  
  Command-line parameters override configuration file values
  Command-line parameters take precedence over defaults
  
  The format is designed for both human readability and easy parsing
  