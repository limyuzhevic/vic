// ===== NEURON CLASS DOCUMENTATION =====
/**
 * @class Neuron
 * @brief Implements Leaky Integrate-and-Fire (LIF) neuron model with spike-frequency adaptation
 * 
 * The Neuron class provides a biologically plausible spiking neuron model that forms the
 * fundamental computational unit of the NLM (Neural Learning Machine) system. This implementation
 * uses the Leaky Integrate-and-Fire dynamics with spike-frequency adaptation, refractory periods,
 * and configurable plasticity mechanisms. The neuron model supports both event-driven and
 * time-stepped simulation paradigms.
 * 
 * This class is part of the core neural processing system in NLM Phase 6: Final Integration,
 * representing individual computational units that participate in large-scale neural network
 * computations. The LIF model balances biological plausibility with computational efficiency,
 * making it suitable for both research and production deployments.
 * 
 * @note Uses Pimpl idiom for memory efficiency and performance
 * @warning Neuron instances must be properly initialized before simulation
 * @ingroup neural_components
 * 
 * @section neuron_architecture Neural Architecture
 * The neuron implements a conductance-based LIF model with the following components:
 * - Membrane potential integration with exponential decay
 * - Spike-frequency adaptation via adaptation variable
 * - Absolute refractory period after firing
 * - Synaptic integration (excitatory/inhibitory)
 * - Plasticity rule modulation (STDP, Hebbian, reward-modulated)
 * - Developmental parameter modulation
 * 
 * @section neuron_physics Biological Physics
 * The neuron model approximates biological dynamics using:
 * - Membrane capacitance ~1 nF (configurable)
 * - Time constant ~20 ms (configurable) 
 * - Resting potential ~-70 mV
 * - Threshold ~-55 mV
 * - Reset potential ~-75 mV
 * - Refractory period 2-10 ms
 * - Adaptation variable for spike-frequency dependence
 * 
 * @section neuron_usage Basic Usage Patterns
 * @subsection usage_initialization Initialization
 * @code{.cpp}
 *   NeuronId id(42);
 *   Neuron neuron(id);
 *   neuron.setThreshold(-55.0f);
 *   neuron.setRestingPotential(-70.0f);
 *   neuron.initializeRandom(rng);
 * @endcode
 * 
 * @subsection usage_simulation Simulation Loop
 * @code{.cpp}
 *   for (int step = 0; step < max_steps; ++step) {
 *       Timestamp time = step * 0.001;  // 1ms timestep
 *       bool fired = neuron.step(time);
 *       if (fired) {
 *           // Handle spike event
 *       }
 *       // Receive inputs
 *       neuron.receiveExcitatoryInput(input);
 *   }
 * @endcode
 * 
 * @subsection usage_plasticity Plasticity Control
 * @code{.cpp}
 *   // Enable plasticity mechanisms
 *   neuron.enablePlasticity(true, true, false);  // Hebbian, STDP, no reward
 *   
 *   // Configure structural plasticity
 *   neuron.enableStructuralPlasticity(true, 0.0001);
 * @endcode
 * 
 * @section neuron_relationships Class Relationships
 * - Brain manages collections of Neurons in regions and populations
 * - Synapse connects Neurons bidirectionally
 * - Config provides neuron parameters
 * - RandomGenerator used for initialization
 * - Plasticity classes modify synaptic weights based on neuron activity
 * 
 * @section neuron_performance Performance Characteristics
 * - Time complexity: O(1) per integration step
 * - Memory usage: Constant per neuron (excluding history)
 * - Spike processing: Event-driven with O(log N) lookups
 * - Synaptic integration: Linear in number of inputs
 * 
 * @section neuron_extensions Future Extensions
 * - Conductance-based models with dynamic synapses
 * - Dendritic compartmentalization
 * - Stochastic noise models
 * - Spike-timing dependent plasticity at axon level
 * - Neuromodulatory receptor dynamics
 * 
 * @todo Add stochastic membrane potential noise
 * @todo Implement conductance-based synaptic integration
 * @todo Add dendritic compartmentalization
 * @todo Implement local plasticity at axon initial segment
 * @todo Add spike-generating models beyond LIF
 * 
 * @see Brain.hpp for brain-level neuron management
 * @see Synapse.hpp for synaptic connections
 * @see Config.hpp for configuration parameters
 * @see constants.hpp for default values
 */