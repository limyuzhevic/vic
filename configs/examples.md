# NLM Configuration Examples
# This file contains example configuration files for different use cases and experiments

# Example 1: Small-scale learning experiment
# Good for rapid prototyping and testing

[small_scale_learning]
random_seed = 123
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.2
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.01
max_simulation_steps = 5000
log_level = INFO
log_to_file = false

# Example 2: Large-scale network simulation
# Good for studying network dynamics and properties

[large_scale_network]
random_seed = 456
simulation_timestep = 0.0005
neuron_count = 5000
region_count = 3
connection_probability = 0.05
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
max_simulation_steps = 20000
log_level = WARNING
log_to_file = true
log_filename = nlm_large.log
checkpoint_dir = ./checkpoints_large

# Example 3: Development study
# Good for investigating developmental processes

[development_study]
random_seed = 789
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
plasticity_learning_rate = 0.01
development_enabled = true
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.00005
max_simulation_steps = 15000
log_level = INFO
log_to_file = true

# Example 4: Neuromodulation focus
# Good for studying reward and curiosity systems

[neuromodulation_focus]
random_seed = 999
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
curiosity_enabled = true
novelty_threshold = 0.05
reward_discount_factor = 0.95
reward_learning_rate = 0.03
max_simulation_steps = 8000
log_level = INFO
log_to_file = true
