# NLM Configuration File
# Advanced configuration with all system features enabled

# Basic brain configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Simulation parameters
simulation_timestep = 0.001
max_steps = 10000
random_seed = 12345

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation parameters
dopamine_strength = 0.8
curiosity_strength = 0.6
novelty_sensitivity = 0.5
reward_scaling = 1.0

# Memory system parameters
working_memory_capacity = 500
working_memory_decay = 0.01
episodic_memory_capacity = 1000
episodic_memory_decay = 0.001

# Prediction system
prediction_horizon = 5
prediction_accuracy_threshold = 0.8

# Cognitive system
planner_max_actions = 10
concept_learning_rate = 0.1
attention_focus_strength = 0.9

# Development parameters
plasticity_rate = 0.5
development_maturation_rate = 0.1
neural_excitability_base = 0.5

# Input/output
sensory_input_scale = 1.0
motor_output_scale = 1.0

# Checkpointing
enable_checkpointing = true
checkpoint_interval = 100
checkpoint_path = "checkpoints/"

# Replay and consolidation
enable_replay = true
replay_strength = 0.8
consolidation_threshold = 0.7

# Performance
enable_performance_monitoring = true
performance_log_interval = 50

# Visualization
enable_visualization = false
viz_update_interval = 10
viz_output_file = "visualization.json"

# configs/benchmark.cfg
# Configuration optimized for performance benchmarking

neuron_count = 2000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001

# Lightweight configuration for fast benchmarking
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 10.0

# Minimal plasticity for stable performance
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005

# Disable memory systems to focus on core performance
enable_memory = false
enable_prediction = false
enable_cognition = false

# Input pattern for consistent benchmarking
pattern_repeats = 20
input_intensity = 30.0

# configs/development.cfg
# Configuration optimized for developmental studies

neuron_count = 500
region_count = 2
connection_probability = 0.2
simulation_timestep = 0.001

# High plasticity for developmental changes
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.04
stdp_tau = 50.0

# Strong structural plasticity
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

# Developmental parameters
plasticity_rate = 0.8
initial_plasticity = 1.0
maturation_threshold = 0.9
development_maturation_rate = 0.05

# Age-dependent effects
age_plasticity_decay = 0.01
age_excitability_change = 0.3

# Enable all systems for comprehensive development
enable_memory = true
enable_prediction = true
enable_cognition = true

# Input for developmental learning
developmental_inputs = true
input_variation = 0.2

# configs/experiment.cfg
# Configuration for scientific experiments

neuron_count = 1000
region_count = 3
connection_probability = 0.12
simulation_timestep = 0.001

# Precise STDP parameters for experiments
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 25.0

# Controlled plasticity
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

# Experimental controls
experimental_subject_id = "exp_001"
experimental_condition = "control"
experimental_replicates = 5

# Measurement parameters
record_spike_times = true
record_synaptic_weights = true
record_membrane_potentials = true
record_neuron_types = true

# Analysis parameters
analysis_window_size = 100
analysis_output_format = "json"
analysis_metrics = "spike_rate,stability,learning_rate"

# Data collection
enable_data_logging = true
data_log_interval = 10
data_output_directory = "experimental_data/"

# Visualization for experiments
enable_visualization = true
viz_plot_spikes = true
viz_plot_weights = true
viz_plot_activity = true

# configs/custom.cfg
# Example of custom configuration for power users

# Override brain configuration
neuron_count = 3000
region_count = 4
connection_probability = 0.18

# Custom simulation parameters
simulation_timestep = 0.0005
max_steps = 20000
random_seed = 99999

# Unusual but interesting parameters
stdp_ltp_weight = 0.15  # Stronger potentiation
stdp_ltd_weight = 0.01  # Weaker depression
stdp_tau = 100.0        # Very long memory

# Complex structural plasticity
synaptogenesis_rate = 0.002
pruning_rate = 0.00005

# Advanced neuromodulation
dopamine_strength = 1.2
curiosity_strength = 0.8
novelty_sensitivity = 0.3
reward_scaling = 2.0

# Large memory systems
working_memory_capacity = 2000
episodic_memory_capacity = 5000

# Advanced prediction
prediction_horizon = 10
prediction_accuracy_threshold = 0.9

# Sophisticated cognition
planner_max_actions = 20
concept_learning_rate = 0.3
attention_focus_strength = 0.95

# Development
plasticity_rate = 0.7
initial_plasticity = 1.5

# Input patterns
multi_sensory_input = true
input_patterns = "visual,audio,internal"
input_modulation = "adaptive"

# Output control
motor_output_mode = "precise"
output_noise_level = 0.05

# Checkpointing with custom path
enable_checkpointing = true
checkpoint_interval = 500
checkpoint_path = "custom_checkpoints/experiment_99/"
checkpoint_compression = "gzip"

# Replay with consolidation
enable_replay = true
replay_strength = 0.9
consolidation_threshold = 0.8
consolidation_method = "hebbian"

# Performance monitoring
always_monitor_performance = true
performance_log_interval = 100
performance_target_fps = 1000
performance_adaptive_scaling = true

# Visualization
enable_visualization = true
viz_output_format = "high_resolution"
viz_update_interval = 5
viz_data_compression = true
viz_output_file = "visualization/experiment_99/final.viz"

# Experimental notes
experimental_notes = "Custom configuration for power user testing with enhanced plasticity and memory systems"
experimental_researcher = "NLM Developer"
experimental_date = "2026-11-10"

# configs/zero_plasticity.cfg
# Configuration with minimal plasticity for stability testing

neuron_count = 500
region_count = 1
connection_probability = 0.1

# Zero or minimal plasticity
stdp_ltp_weight = 0.0
stdp_ltd_weight = 0.0
stdp_tau = 1.0

synaptogenesis_rate = 0.0
pruning_rate = 0.0

# Disable all plasticity-dependent systems
enable_development = false
enable_memory = false
enable_prediction = false
enable_cognition = false

# Very stable input
input_stability = 0.99
input_variance = 0.001

# Focus on stable maintenance rather than learning
maintenance_mode = true
stability_objective = 0.999