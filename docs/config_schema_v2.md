# NLM Configuration Schema
# This file defines the configuration schema for NLM (Neural Learning Machine)
# Version: 2.0
# Phase: 2 - Real Neural Computation

[metadata]
version = "2.0"
phase = "2"
description = "Neural Learning Machine Phase 2 Configuration Schema"
created = "2026-10-18"
author = "NLM Development Team"

[sections]

[sections.simulation]
description = "Core simulation parameters"

[sections.simulation.timing]
description = "Timing and step control"
simulation_timestep = "float:0.0001:0.1|Simulation timestep in seconds"
max_simulation_steps = "int:1:1000000|Maximum number of simulation steps"
simulation_time_limit = "float:0.0:1000.0|Maximum simulation time in seconds"

[sections.simulation.topology]
description = "Brain topology and connectivity"
neuron_count = "int:1:1000000|Number of neurons in the brain"
region_count = "int:1:100|Number of brain regions"
connection_probability = "float:0.0:1.0|Probability of connection between neurons"

[sections.neuron]
description = "Neuron model and properties"

[sections.neuron.model]
description = "LIF neuron model parameters"
resting_potential = "float:-1.0:1.0|Resting membrane potential"
reset_potential = "float:-1.0:1.0|Membrane potential after spike"
threshold = "float:0.0:1.0|Spike threshold potential"

[sections.neuron.dynamics]
description = "Neuron dynamics parameters"
membrane_time_constant = "float:0.001:0.1|Membrane time constant in seconds"
refractory_period = "float:0.001:0.1|Refractory period in seconds"

[sections.plasticity]
description = "Synaptic plasticity mechanisms"

[sections.plasticity.stdp]
description = "Spike-Timing-Dependent Plasticity parameters"
stdp_ltp_weight = "float:0.0:1.0|LTP weight change for STDP"
stdp_ltd_weight = "float:0.0:1.0|LTD weight change for STDP"
stdp_time_constant = "float:1.0:100.0|STDP time constant in ms"

[sections.plasticity.hebbian]
description = "Hebbian plasticity parameters"
hebbian_learning_rate = "float:0.0:1.0|Hebbian learning rate"

[sections.plasticity.structural]
description = "Structural plasticity parameters"
synaptogenesis_rate = "float:0.0:1.0|Rate of new synapse formation"
pruning_rate = "float:0.0:1.0|Rate of synapse elimination"

[sections.neuromodulation]
description = "Neuromodulation and reward systems"

[sections.neuromodulation.dopamine]
description = "Dopamine neuromodulation"
dopamine_baseline = "float:-1.0:1.0|Baseline dopamine level"
dopamine_sensitivity = "float:0.0:1.0|Dopamine sensitivity factor"

[sections.neuromodulation.reward]
description = "Reward and reinforcement learning"
reward_discount_factor = "float:0.0:1.0|Reward discount factor for temporal difference"
reward_scale = "float:0.0:10.0|Reward scaling factor"

[sections.memory]
description = "Memory systems"

[sections.memory.working]
description = "Working memory parameters"
working_memory_capacity = "int:1:10000|Working memory capacity"
working_memory_decay_rate = "float:0.0:1.0|Working memory decay rate"

[sections.memory.episodic]
description = "Episodic memory parameters"
episodic_memory_capacity = "int:1:10000|Episodic memory capacity"
episodic_memory_consolidation_rate = "float:0.0:1.0|Episodic memory consolidation rate"

[sections.cognition]
description = "Cognitive systems"

[sections.cognition.planner]
description = "Neural planning system"
planning_horizon = "int:1:100|Planning horizon in steps"
planning_resolution = "float:0.0:1.0|Planning resolution"

[sections.cognition.concept_formation]
description = "Concept formation system"
concept_formation_rate = "float:0.0:1.0|Rate of new concept formation"
concept_generalization = "float:0.0:1.0|Concept generalization factor"

[sections.environment]
description = "Environment and interaction"

[sections.environment.world]
description = "World model parameters"
environment_name = "string:GridWorld|Name of the environment"
environment_width = "int:1:100|Environment width"
environment_height = "int:1:100|Environment height"

[sections.environment.sensory]
description = "Sensory system parameters"
sensory_noise_level = "float:0.0:1.0|Sensory input noise level"
sensory_receptive_field = "int:1:10|Sensory receptive field size"

[sections.environment.motor]
description = "Motor system parameters"
motor_output_range = "float:0.0:1.0|Motor output range"
motor_noise_level = "float:0.0:1.0|Motor output noise level"

[sections.logging]
description = "Logging and output"

[sections.logging.level]
description = "Log level configuration"
log_level = "string:DEBUG,INFO,WARNING,ERROR|Log level"
log_to_file = "bool:false|Whether to log to file"
log_filename = "string:nlm.log|Log file name"
log_file_rotation = "bool:true|Enable log file rotation"
log_rotation_size = "int:1048576:1073741824|Log file rotation size in bytes"

[sections.logging.monitoring]
description = "Monitoring and metrics"
enable_metrics = "bool:false|Enable metrics collection"
metrics_file = "string:metrics.json|Metrics output file"
metrics_interval = "int:100:10000|Metrics collection interval in steps"

[sections.visualization]
description = "Visualization parameters"
visualization_enabled = "bool:false|Enable visualization"
visualization_update_rate = "float:1.0:60.0|Visualization update rate in Hz"
visualization_output_format = "string:png,ascii,none|Visualization output format"

[sections.development]
description = "Development system parameters"

[sections.development.synaptogenesis]
description = "Synaptogenesis parameters"
development_synaptogenesis_rate = "float:0.0:1.0|Development synaptogenesis rate"

[sections.development.pruning]
description = "Pruning parameters"
development_pruning_rate = "float:0.0:1.0|Development pruning rate"

[sections.development.maturation]
description = "Maturation parameters"
development_maturation_rate = "float:0.0:1.0|Development maturation rate"

[sections.experimental]
description = "Experimental and research parameters"

[sections.experimental.debug]
description = "Debug and testing parameters"
debug_mode = "bool:false|Enable debug mode"
verbose_logging = "bool:false|Enable verbose logging"

[sections.experimental.performance]
description = "Performance optimization"
performance_profile = "string:none,release,debug,profile|Performance profile"
parallel_processing = "bool:false|Enable parallel processing"

[defaults]
description = "Default values for all sections"

[examples]
description = "Configuration examples for different scenarios"

[examples.simple]
title = "Simple Configuration"
description = "Basic configuration for quick testing"

[examples.simple.settings]
neuron_count = "100"
region_count = "1"
simulation_timestep = "0.001"
max_simulation_steps = "1000"

[examples.performance]
title = "High-Performance Configuration"
description = "Configuration optimized for performance"

[examples.performance.settings]
neuron_count = "5000"
region_count = "4"
simulation_timestep = "0.0001"
max_simulation_steps = "100000"
parallel_processing = "true"
performance_profile = "release"

[examples.research]
title = "Research Configuration"
description = "Configuration for research experiments"

[examples.research.settings]
neuron_count = "1000"
region_count = "2"
simulation_timestep = "0.001"
max_simulation_steps = "50000"
enable_metrics = "true"
verbose_logging = "true"
