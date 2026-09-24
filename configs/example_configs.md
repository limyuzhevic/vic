# NLM Configuration File Examples

# This file contains example configuration files for the NLM (Neural Learning Machine)
# Each configuration serves a different purpose and demonstrates various settings.

configs/default.cfg:
    # Default configuration for basic testing
    # Suitable for quick validation and development
    
    [brain]
    neuron_count = 500
    region_count = 1
    connection_probability = 0.15
    initial_weight_mean = 0.5
    initial_weight_std = 0.1
    
    [brain.neurons]
    v_thresh = -50.0      # Neuron threshold potential (mV)
    v_rest = -70.0        # Resting potential (mV)
    v_reset = -75.0       # Reset potential after spike (mV)
    tau_mem = 20.0        # Membrane time constant (ms)
    tau_ref = 2.0         # Refractory period (ms)
    
    [plasticity.stdp]
    enable = true
    learning_rate = 0.001
    tau_plus = 20.0
    tau_minus = 20.0
    
    [plasticity.hebbian]
    enable = true
    learning_rate = 0.001
    
    [plasticity.structural]
    enable = true
    synaptogenesis_rate = 0.0001
    pruning_rate = 0.00001
    
    [neuromodulation.dopamine]
    scale = 1.0
    
    [neuromodulation.curiosity]
    enable = true
    
    [neuromodulation.novelty]
    enable = true
    
    [simulation]
    timestep = 0.001      # Simulation timestep (s)
    
    [logging]
    level = "info"
    file = "nlm.log"
    max_file_size = 10485760  # 10MB

configs/large.cfg:
    # Configuration for large-scale simulations
    # Good for performance testing and studying emergent behaviors
    
    [brain]
    neuron_count = 5000
    region_count = 4
    connection_probability = 0.05
    initial_weight_mean = 0.5
    initial_weight_std = 0.1
    
    [brain.neurons]
    v_thresh = -50.0
    v_rest = -70.0
    v_reset = -75.0
    tau_mem = 20.0
    tau_ref = 5.0           # Longer refractory period
    
    [plasticity.stdp]
    enable = true
    learning_rate = 0.001
    tau_plus = 20.0
    tau_minus = 20.0
    
    [plasticity.hebbian]
    enable = true
    learning_rate = 0.0005    # Slower Hebbian learning
    
    [plasticity.structural]
    enable = true
    synaptogenesis_rate = 0.00005
    pruning_rate = 0.00001
    
    [neuromodulation.dopamine]
    scale = 1.5              # Stronger neuromodulation
    
    [neuromodulation.curiosity]
    enable = true
    
    [neuromodulation.novelty]
    enable = true
    
    [simulation]
    timestep = 0.001
    
    [logging]
    level = "info"
    file = "nlm_large.log"
    max_file_size = 52428800  # 50MB

configs/learning.cfg:
    # Configuration optimized for learning and plasticity experiments
    # Emphasizes synaptic modification and memory formation
    
    [brain]
    neuron_count = 1000
    region_count = 2
    connection_probability = 0.2    # Higher connectivity for learning
    initial_weight_mean = 0.3
    initial_weight_std = 0.2
    
    [brain.neurons]
    v_thresh = -55.0        # More excitable neurons
    v_rest = -70.0
    v_reset = -80.0         # Lower reset potential
    tau_mem = 15.0          # Faster membrane dynamics
    tau_ref = 3.0
    
    [plasticity.stdp]
    enable = true
    learning_rate = 0.002      # Stronger STDP
    tau_plus = 30.0            # Longer LTP window
    tau_minus = 30.0           # Longer LTD window
    
    [plasticity.hebbian]
    enable = true
    learning_rate = 0.001
    
    [plasticity.structural]
    enable = true
    synaptogenesis_rate = 0.0002    # Higher synaptogenesis
    pruning_rate = 0.00005
    
    [neuromodulation.dopamine]
    scale = 2.0              # Strong reward modulation
    
    [neuromodulation.curiosity]
    enable = true
    curiosity_threshold = 0.5
    
    [neuromodulation.novelty]
    enable = true
    novelty_threshold = 0.7
    
    [simulation]
    timestep = 0.001
    
    [logging]
    level = "debug"          # More detailed logging for learning analysis
    file = "nlm_learning.log"
    max_file_size = 10485760

configs/debug.cfg:
    # Debug configuration with verbose logging and validation
    # Ideal for development and troubleshooting
    
    [brain]
    neuron_count = 100       # Small network for quick debugging
    region_count = 1
    connection_probability = 0.3
    initial_weight_mean = 0.5
    initial_weight_std = 0.1
    
    [brain.neurons]
    v_thresh = -50.0
    v_rest = -70.0
    v_reset = -75.0
    tau_mem = 20.0
    tau_ref = 2.0
    
    [plasticity.stdp]
    enable = true
    learning_rate = 0.001
    tau_plus = 20.0
    tau_minus = 20.0
    
    [plasticity.hebbian]
    enable = true
    learning_rate = 0.001
    
    [plasticity.structural]
    enable = false           # Disable structural plasticity for cleaner debugging
    synaptogenesis_rate = 0.0001
    pruning_rate = 0.00001
    
    [neuromodulation.dopamine]
    scale = 1.0
    
    [neuromodulation.curiosity]
    enable = false
    
    [neuromodulation.novelty]
    enable = false
    
    [simulation]
    timestep = 0.001
    
    [logging]
    level = "debug"          # Maximum verbosity
    file = "nlm_debug.log"
    max_file_size = 5242880   # 5MB

configs/performance.cfg:
    # Performance-focused configuration
    # Optimized for speed and efficiency
    
    [brain]
    neuron_count = 2000
    region_count = 3
    connection_probability = 0.1
    initial_weight_mean = 0.5
    initial_weight_std = 0.1
    
    [brain.neurons]
    v_thresh = -50.0
    v_rest = -70.0
    v_reset = -75.0
    tau_mem = 20.0
    tau_ref = 2.0
    
    [plasticity.stdp]
    enable = true            # STDP is computationally efficient
    learning_rate = 0.001
    tau_plus = 20.0
    tau_minus = 20.0
    
    [plasticity.hebbian]
    enable = false           # Disabled for performance
    learning_rate = 0.001
    
    [plasticity.structural]
    enable = false           # Disabled for performance
    synaptogenesis_rate = 0.0001
    pruning_rate = 0.00001
    
    [neuromodulation.dopamine]
    scale = 1.0
    
    [neuromodulation.curiosity]
    enable = false           # Disabled for performance
    
    [neuromodulation.novelty]
    enable = false           # Disabled for performance
    
    [simulation]
    timestep = 0.001
    
    [logging]
    level = "info"           # Minimal logging for performance
    file = "nlm_performance.log"
    max_file_size = 10485760

configs/experiment.cfg:
    # Comprehensive configuration for scientific experiments
    # Designed for reproducibility and scientific analysis
    
    [brain]
    neuron_count = 3000
    region_count = 5
    connection_probability = 0.08
    initial_weight_mean = 0.5
    initial_weight_std = 0.1
    
    [brain.neurons]
    v_thresh = -50.0
    v_rest = -70.0
    v_reset = -75.0
    tau_mem = 20.0
    tau_ref = 2.0
    
    [plasticity.stdp]
    enable = true
    learning_rate = 0.001
    tau_plus = 20.0
    tau_minus = 20.0
    
    [plasticity.hebbian]
    enable = true
    learning_rate = 0.001
    
    [plasticity.structural]
    enable = true
    synaptogenesis_rate = 0.00005
    pruning_rate = 0.00005
    
    [neuromodulation.dopamine]
    scale = 1.0
    
    [neuromodulation.curiosity]
    enable = true
    curiosity_threshold = 0.5
    
    [neuromodulation.novelty]
    enable = true
    novelty_threshold = 0.7
    
    [simulation]
    timestep = 0.001
    
    [logging]
    level = "info"
    file = "nlm_experiment.log"
    max_file_size = 104857600  # 100MB
    append = true
    
    [experiment]
    random_seed = 42          # Fixed for reproducibility
    log_detailed = true
    save_intermediate = true
    intermediate_file = "nlm_experiment_intermediate.bin"