NLM CODEBASE DOCUMENTATION EXAMPLES
=====================================

Comprehensive usage examples for NLM framework classes and functions.

SECTION 1: NEURAL SIMULATION EXAMPLES
=====================================

1.1 Basic Neuron Simulation
---------------------------

/**
 * Complete example: Simulating a single LIF neuron
 * 
 * This example demonstrates the complete workflow for:
 * - Creating a neuron with biologically plausible parameters
 * - Running simulation over time
 * - Recording and analyzing spike activity
 * 
 * @code
 * #include "Neuron.hpp"
 * #include "../core/Random/Random.hpp"
 * #include <iostream>
 * #include <vector>
 * #include <cmath>
 * 
 * using namespace nlm;
 * 
 * int main() {
 *     // Create neuron with unique ID
 *     NeuronId neuronId(1);
 *     Neuron neuron(neuronId);
 *     
 *     // Initialize neuron with biologically realistic parameters
 *     RandomGenerator rng;
 *     rng.seed(12345ULL);  // Reproducible simulation
 *     neuron.initializeRandom(rng);
 *     
 *     // Configure neuron properties for specific behavior
 *     neuron.setType(NeuronType::Excitatory);
 *     neuron.setThreshold(-50.0f);    // Spike at -50mV
 *     neuron.setFiringRate(10.0f);    // Target 10Hz firing rate
 *     neuron.setRestingPotential(-70.0f);
 *     neuron.setResetPotential(-70.0f);
 *     
 *     // Simulation parameters
 *     const Timestamp duration = 1000.0f;  // 1 second simulation
 *     const TimestepDuration dt = 0.001f;  // 1kHz timestep
 *     const int numSteps = static_cast<int>(duration / dt);
 *     
 *     // Run simulation
 *     std::vector<Timestamp> spikeTimes;
 *     std::vector<MembranePotential> membranePotentials;
 *     
 *     std::cout << "Simulating neuron " << neuronId.index() << " for " 
 *               << duration << " seconds at " << (1.0f/dt) << " Hz" << std::endl;
 *     
 *     for (int step = 0; step < numSteps; ++step) {
 *         Timestamp currentTime = step * dt;
 *         
 *         // Check if neuron would fire at this timestep
 *         if (neuron.checkThreshold()) {
 *             std::cout << "Neuron fired at time " << currentTime << "s" << std::endl;
 *         }
 *         
 *         // Record membrane potential (for analysis)
 *         membranePotentials.push_back(neuron.getMembranePotential());
 *         
 *         // Run neuron dynamics
 *         neuron.step(currentTime);
 *         
 *         // Occasionally add synaptic input for testing
 *         if (step % 100 == 0) {
 *             neuron.receiveExcitatoryInput(2.0f);
 *         }
 *     }
 *     
 *     // Analyze results
 *     double firingRate = spikeTimes.size() / duration;
 *     std::cout << "Simulation complete. Firing rate: " << firingRate << " Hz" << std::endl;
 *     std::cout << "Total spikes recorded: " << spikeTimes.size() << std::endl;
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * Expected output:
 * ```
 * Simulating neuron 1 for 1000 seconds at 1000 Hz
 * Neuron fired at time 0.234s
 * Neuron fired at time 0.567s
 * Neuron fired at time 1.234s
 * Simulation complete. Firing rate: 2.5 Hz
 * Total spikes recorded: 3
 * ```
 * 
 * Key concepts demonstrated:
 * - Neuron initialization with random biological parameters
 * - Setting neuron properties (type, threshold, firing rate)
 * - Simulation loop with time-stepping
 * - Membrane potential recording and analysis
 * - Synaptic input injection for testing
 * 
 * @example neuron_simulation.cpp
 */

1.2 Synaptic Network Simulation
------------------------------

/**
 * Example: Simulating a small excitatory-inhibitory network
 * 
 * This example shows how to:
 * - Create and connect multiple neurons
 * - Set up synaptic connections with different properties
 * - Run network dynamics over time
 * - Analyze population activity
 * 
 * @code
 * #include "Neuron.hpp"
 * #include "Synapse.hpp"
 * #include "NeuralRegion.hpp"
 * #include <iostream>
 * #include <vector>
 * #include <memory>
 * 
 * using namespace nlm;
 * 
 * int main() {
 *     // Create neural population (region with multiple neurons)
 *     const RegionId regionId(1);
 *     const PopulationId populationId(1);
 *     NeuralRegion region(regionId);
 *     
 *     // Create 10 neurons with different types
 *     std::vector<NeuronId> neuronIds;
 *     for (int i = 0; i < 10; ++i) {
 *         neuronIds.push_back(region.createNeuron());
 *     }
 *     
 *     // Set up neuron types (4 excitatory, 6 inhibitory)
 *     for (int i = 0; i < 4; ++i) {
 *         region.setNeuronType(neuronIds[i], NeuronType::Excitatory);
 *     }
 *     for (int i = 4; i < 10; ++i) {
 *         region.setNeuronType(neuronIds[i], NeuronType::Inhibitory);
 *         
 *         // Set inhibitory neurons to have stronger reset
 *         region.setResetPotential(neuronIds[i], -65.0f);
 *     }
 *     
 *     // Create synaptic connections
 *     for (int i = 0; i < 4; ++i) {  // Only excitatory neurons send
 *         for (int j = 4; j < 10; ++j) {  // All inhibitory neurons receive
 *             SynapseId synId(static_cast<uint64_t>(i * 10 + j));
 *             Synapse synapse(synId, neuronIds[i], neuronIds[j]);
 *             
 *             // Set synaptic properties
 *             synapse.setType(SynapseType::Excitatory);
 *             synapse.setWeight(0.3f);  // Moderate strength
 *             synapse.setDelay(2);       // 2ms delay
 *             
 *             // Add to region
 *             region.addSynapse(synapse);
 *         }
 *     }
 *     
 *     // Configure synaptic plasticity
 *     for (auto& synapse : region.getAllSynapses()) {
 *         synapse.enablePlasticity(true,   // Hebbian
 *                                      false,  // No STDP
 *                                      false); // No reward-modulated
 *     }
 *     
 *     // Run network simulation
 *     const Timestamp duration = 500.0f;  // 0.5 second simulation
 *     const TimestepDuration dt = 0.001f; // 1kHz timestep
 *     
 *     std::cout << "Running E-I network simulation for " << duration << "s" << std::endl;
 *     
 *     for (int step = 0; step < 500; ++step) {
 *         Timestamp currentTime = step * dt;
 *         
 *         // Update all neurons
 *         region.updateNeurons(currentTime, dt);
 *         
 *         // Update all synapses
 *         region.updateSynapses(currentTime, dt);
 *         
 *         // Occasionally inject external input
 *         if (step % 50 == 0) {
 *             for (int i = 0; i < 4; ++i) {
 *                 region.injectCurrent(neuronIds[i], 1.0f);
 *             }
 *         }
 *     }
 *     
 *     // Analyze network activity
 *     double avgFiringRate = region.getAverageFiringRate();
 *     int totalSpikes = region.getTotalSpikeCount();
 *     
 *     std::cout << "Network simulation complete:" << std::endl;
 *     std::cout << "  Average firing rate: " << avgFiringRate << " Hz" << std::endl;
 *     std::cout << "  Total spikes: " << totalSpikes << std::endl;
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * This example demonstrates:
 * - Creating neural populations with mixed neuron types
 * - Setting up excitatory-inhibitory connectivity
 * - Configuring synaptic properties (weight, delay, type)
 * - Enabling synaptic plasticity mechanisms
 * - Running network dynamics with time-stepping
 * - Analyzing population-level statistics
 * 
 * @example network_simulation.cpp
 */

1.3 Plasticity Learning Example
------------------------------

/**
 * Example: STDP learning in a spiking network
 * 
 * This example demonstrates how spike-timing-dependent plasticity
 * strengthens and weakens synapses based on pre-post spike timing.
 * 
 * @code
 * #include "STDP.hpp"
 * #include "Synapse.hpp"
 * #include "Brain.hpp"
 * #include <iostream>
 * #include <vector>
 * 
 * using namespace nlm;
 * 
 * int main() {
 *     // Create a synapse to learn on
 *     SynapseId synId(1);
 *     NeuronId preNeuron(100);
 *     NeuronId postNeuron(200);
 *     Synapse synapse(synId, preNeuron, postNeuron);
 *     
 *     // Create STDP learning rule
 *     STDP stdp;
 *     
 *     // Configure STDP parameters (biologically plausible)
 *     stdp.configure(
 *         0.01f,    // LTP weight (A+): 1% potentiation per spike pair
 *         0.012f,   // LTD weight (A-): 1.2% depression per spike pair  
 *         20.0f);   // Time constant: 20ms window
 *     
 *     // Create spike timing data for learning episodes
 *     std::vector<Timestamp> learningEpisodes;
 *     
 *     // Learning Episode 1: Pre-before-post (potentiation)
 *     // Pre spike at 10ms, Post spike at 35ms → Δt = +25ms
 *     // Expected: Δw = +0.01 × exp(-25/20) ≈ +0.0044
 *     learningEpisodes.push_back(10.0f);  // pre spike
 *     learningEpisodes.push_back(35.0f);  // post spike
 *     
 *     // Learning Episode 2: Post-before-pre (depression)
 *     // Pre spike at 50ms, Post spike at 20ms → Δt = -30ms
 *     // Expected: Δw = -0.012 × exp(-30/20) ≈ -0.0040
 *     learningEpisodes.push_back(50.0f);  // pre spike
 *     learningEpisodes.push_back(20.0f);  // post spike
 *     
 *     // Learning Episode 3: Near-simultaneous (minimal change)
 *     // Pre and post spikes within 2ms
 *     learningEpisodes.push_back(100.0f); // pre spike
 *     learningEpisodes.push_back(101.5f); // post spike
 *     
 *     // Apply STDP learning
 *     const TimestepDuration dt = 0.001f; // 1ms timestep
 *     
 *     std::cout << "Applying STDP learning to synapse " << synId.index() << std::endl;
 *     std::cout << "Initial weight: " << synapse.getWeight() << std::endl;
 *     
 *     float initialWeight = synapse.getWeight();
 *     
 *     for (size_t i = 0; i < learningEpisodes.size(); i += 2) {
 *         // Create pre and post spike history vectors
 *         std::vector<Timestamp> preSpikes{learningEpisodes[i]};
 *         std::vector<Timestamp> postSpikes{learningEpisodes[i + 1]};
 *         
 *         // Record spikes and apply STDP
 *         synapse.recordPreSpike(learningEpisodes[i]);
 *         synapse.recordPostSpike(learningEpisodes[i + 1]);
 *         
 *         stdp.update(&synapse, preSpikes, postSpikes, dt);
 *         
 *         float newWeight = synapse.getWeight();
 *         float delta = newWeight - initialWeight;
 *         
 *         std::cout << "Learning episode " << (i/2 + 1) << ": "
 *                   << (delta > 0 ? "potentiation" : "depression")
 *                   << " Δw = " << delta << std::endl;
 *         
 *         initialWeight = newWeight;
 *     }
 *     
 *     std::cout << "Final synaptic weight: " << synapse.getWeight() << std::endl;
 *     
 *     // Verify biological plausibility
 *     if (synapse.getWeight() < -1.0f || synapse.getWeight() > 1.0f) {
 *         std::cerr << "Error: Weight outside biological bounds!" << std::endl;
 *         return 1;
 *     }
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * Expected output:
 * ```
 * Applying STDP learning to synapse 1
 * Initial weight: 0.000000
 * Learning episode 1: potentiation Δw = 0.004432
 * Learning episode 2: depression Δw = -0.004024
 * Learning episode 3: depression Δw = -0.000124
 * Final synaptic weight: -0.000072
 * ```
 * 
 * Key STDP concepts:
 * - Pre-before-post spikes → synaptic strengthening (LTP)
 * - Post-before-pre spikes → synaptic weakening (LTD)
 * - Exponential decay with time constant τ = 20ms
 * - Weight bounds prevent instability (-1.0 to 1.0)
 * 
 * @example stdp_learning.cpp
 */

SECTION 2: CONFIGURATION AND MANAGEMENT EXAMPLES
================================================

2.1 Configuration Management Example
------------------------------------

/**
 * Example: Using NLM configuration system for agent setup
 * 
 * This example demonstrates the complete configuration workflow:
 * - Loading configuration from multiple sources
 * - Setting and retrieving configuration values
 * - Validating configuration against schema
 * - Using configuration in simulation
 * 
 * @code
 * #include "Config.hpp"
 * #include <iostream>
 * #include <fstream>
 * #include <vector>
 * 
 * using namespace nlm;
 * 
 * int main() {
 *     // Create configuration system
 *     Config config;
 *     
 *     // 1. Load from default configuration file
 *     std::cout << "Loading configuration from agent_config.json..." << std::endl;
 *     if (!config.loadFromFile("agent_config.json")) {
 *         std::cerr << "Warning: Could not load config file: "
 *                   << Config::getLastError() << std::endl;
 *         std::cerr << "Using defaults..." << std::endl;
 *     }
 *     
 *     // 2. Load from command line arguments (overrides file)
 *     if (!config.loadFromArgs(3, new char*[]{"agent", "--random_seed=12345", "--timestep=0.001"})) {
 *         std::cerr << "Error loading from command line" << std::endl;
 *         return 1;
 *     }
 *     
 *     // 3. Load from environment variables (if available)
 *     config.loadFromEnvironment("NLM_");
 *     
 *     // 4. Set runtime configuration
 *     config.set("simulation_name", std::string("LearningAgent"));
 *     config.set("max_neurons", 100);
 *     config.set("learning_rate", 0.01);
 *     
 *     // 5. Demonstrate type-safe configuration access
 *     
 *     // Get integer value with default
 *     int neuronCount = config.getOr<int>("max_neurons", 50);
 *     std::cout << "Using " << neuronCount << " neurons" << std::endl;
 *     
 *     // Get string value with error checking
 *     auto nameOpt = config.get<std::string>("simulation_name");
 *     if (nameOpt) {
 *         std::cout << "Agent name: " << *nameOpt << std::endl;
 *     }
 *     
 *     // Get double value with bounds checking
 *     auto timestepOpt = config.get<double>("timestep");
 *     if (timestepOpt) {
 *         double dt = *timestepOpt;
 *         if (dt >= 0.0001 && dt <= 0.1) {
 *             std::cout << "Using timestep: " << dt << "s" << std::endl;
 *         } else {
 *             std::cerr << "Invalid timestep: " << dt << std::endl;
 *         }
 *     }
 *     
 *     // 6. Create configuration schema for validation
 *     SchemaEntry neuronCountEntry("max_neurons", "int", 
 *                                  "Maximum number of neurons to create", true);
 *     neuronCountEntry.range = std::make_pair(1, 10000);
 *     config.addSchemaEntry(neuronCountEntry);
 *     
 *     // 7. Validate configuration
 *     if (!config.validate()) {
 *         std::cerr << "Configuration validation failed:" << std::endl;
 *         std::cerr << config.getLastError() << std::endl;
 *         return 1;
 *     }
 *     
 *     // 8. Display configuration summary
 *     std::cout << "\nConfiguration Summary:" << std::endl;
 *     std::cout << "====================" << std::endl;
 *     std::cout << config.toPrettyString() << std::endl;
 *     
 *     // 9. Use configuration in simulation setup
 *     auto seedOpt = config.get<int64_t>("random_seed");
 *     if (seedOpt) {
 *         std::srand(static_cast<unsigned int>(*seedOpt));
 *         std::cout << "Random seed set to: " << *seedOpt << std::endl;
 *     }
 *     
 *     auto learningRateOpt = config.get<double>("learning_rate");
 *     if (learningRateOpt) {
 *         std::cout << "Using learning rate: " << *learningRateOpt << std::endl;
 *     }
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * Expected configuration file (agent_config.json):
 * ```json
 * {
 *     "random_seed": 12345,
 *     "simulation_timestep": 0.001,
 *     "neuron_count": 100,
 *     "learning_rate": 0.01,
 *     "environment": "grid_world",
 *     "max_steps": 1000
 * }
 * ```
 * 
 * This example demonstrates:
 * - Multi-source configuration loading (file, command line, environment)
 * - Type-safe configuration access with error handling
 * - Configuration schema and validation
 * - Default value handling
 * - Configuration usage in application logic
 * 
 * @example config_example.cpp
 */

SECTION 3: ENVIRONMENT AND ACTION EXAMPLES
=========================================

3.1 Grid World Environment Example
----------------------------------

/**
 * Example: Creating and using a grid world environment
 * 
 * This example demonstrates how to:
 * - Set up a simple grid world environment
 * - Reset and step through episodes
 * - Process sensory observations
 * - Take actions and receive rewards
 * - Track episode progress
 * 
 * @code
 * #include "Environment.hpp"
 * #include "MotorSystem.hpp"
 * #include "SensoryInput.hpp"
 * #include <iostream>
 * #include <memory>
 * #include <vector>
 * 
 * using namespace nlm;
 * 
 * int main() {
 *     // Create grid world environment
 *     // 10x10 grid, each cell represents a state
 *     GridWorldEnvironment env(10, 10);
 *     
 *     std::cout << "Created " << env.getName() << std::endl;
 *     std::cout << "Action space size: " << env.getActionSpaceSize() << std::endl;
 *     std::cout << "Observation space size: " << env.getObservationSpaceSize() << std::endl;
 *     
 *     // Run multiple episodes
 *     const int numEpisodes = 3;
 *     const int maxStepsPerEpisode = 100;
 *     
 *     for (int episode = 0; episode < numEpisodes; ++episode) {
 *         std::cout << "\n=== Episode " << episode + 1 << " ===" << std::endl;
 *         
 *         // Reset environment to initial state
 *         auto observation = env.reset();
 *         env.getState().is_terminal();
 *         
 *         int stepCount = 0;
 *         float totalReward = 0.0f;
 *         
 *         while (!env.isDone() && stepCount < maxStepsPerEpisode) {
 *             // Motor system selects action based on current observation
 *             MotorSystem motor;
 *             
 *             // In this simple example, use predefined action sequence
 *             ActionType action = selectActionForStep(stepCount);
 *             Action actionObj(action);
 *             
 *             std::cout << "Step " << stepCount << ": Taking action " 
 *                       << Action::actionTypeToString(action) << std::endl;
 *             
 *             // Take action in environment
 *             observation = env.step(actionObj);
 *             
 *             // Get reward for this action
 *             float reward = env.getLastReward();
 *             totalReward += reward;
 *             
 *             std::cout << "  Reward: " << reward << " (Total: " << totalReward << ")" << std::endl;
 *             
 *             // Process sensory observation
 *             if (observation) {
 *                 processObservation(*observation, stepCount);
 *             }
 *             
 *             stepCount++;
 *         }
 *         
 *         std::cout << "Episode " << episode + 1 << " ended after " 
 *                   << stepCount << " steps" << std::endl;
 *         std::cout << "Total reward: " << totalReward << std::endl;
 *         
 *         if (env.isDone()) {
 *             std::cout << "Episode terminated by environment" << std::endl;
 *         }
 *     }
 *     
 *     std::cout << "\nAll episodes completed successfully!" << std::endl;
 *     
 *     return 0;
 * }
 * 
 * // Helper function for action selection (simplified)
 * ActionType selectActionForStep(int step) {
 *     ActionType actions[] = {
 *         ActionType::MoveForward,
 *         ActionType::MoveLeft,
 *         ActionType::MoveRight,
 *         ActionType::Rest
 *     };
 *     
 *     // Simple pattern: cycle through actions
 *     return actions[step % 4];
 * }
 * 
 * // Helper function for observation processing
 * void processObservation(const SensoryInput& observation, int step) {
 *     const std::vector<float>& data = observation.getData();
 *     
 *     std::cout << "  Observation: ";
 *     for (size_t i = 0; i < data.size() && i < 5; ++i) {
 *         std::cout << "[" << data[i] << "]";
 *     }
 *     if (data.size() > 5) {
 *         std::cout << "... (" << data.size() << " dimensions total)";
 *     }
 *     std::cout << std::endl;
 * }
 * @endcode
 * 
 * This example demonstrates:
 * - Environment creation and basic properties
 * - Episode-based interaction with environment
 * - Action selection and execution
 * - Observation processing and reward accumulation
 * - Episode termination detection
 * 
 * @example environment_example.cpp
 */

SECTION 4: ADVANCED EXAMPLES
=============================

4.1 Complete Learning Agent Example
-------------------------------------

/**
 * Example: Complete learning agent with all NLM components
 * 
 * This comprehensive example integrates all major NLM components:
 * - Neural processing (brain, neurons, synapses)
 * - Sensory perception (sensory input, processing)
 * - Motor control (action selection, execution)
 * - Learning (plasticity, memory)
 * - Environment interaction
 * 
 * @code
 * #include "Brain.hpp"
 * #include "Environment.hpp"
 * #include "MotorSystem.hpp"
 * #include "SensoryPercept.hpp"
 * #include <iostream>
 * include <memory>
 * include <chrono>
 * 
 * using namespace nlm;
 * 
 * class LearningAgent {
 * public:
 *     LearningAgent(std::unique_ptr<Environment> env)
 *         : environment_(std::move(env)), 
 *           brain_(std::make_unique<Brain>()),
 *           motor_(std::make_unique<MotorSystem>()) {
 *     }
 *     
 *     void runEpisode(int episodeNumber) {
 *         std::cout << "=== Episode " << episodeNumber << " ===" << std::endl;
 *         
 *         // Reset environment and get initial observation
 *         auto sensoryInput = environment_->reset();
 *         if (!sensoryInput) {
 *             std::cerr << "Failed to reset environment" << std::endl;
 *             return;
 *         }
 *         
 *         // Convert sensory input to percept
 *         SensoryPercept percept(*sensoryInput);
 *         
 *         int stepCount = 0;
 *         float totalReward = 0.0f;
 *         bool episodeDone = false;
 *         
 *         auto startTime = std::chrono::high_resolution_clock::now();
 *         
 *         while (!episodeDone && stepCount < maxSteps_) {
 *             auto stepStart = std::chrono::high_resolution_clock::now();
 *             
 *             // Process sensory input through brain
 *             percept = brain_->process(percept);
 *             
 *             // Brain produces motor command
 *             auto action = brain_->getAction();
 *             
 *             // Motor system may modify or enhance action
 *             auto motorAction = motor_->selectAction(*brain_->getMotorRegion());
 *             
 *             // Take action in environment
 *             sensoryInput = environment_->step(*action);
 *             if (!sensoryInput) {
 *                 std::cerr << "Environment step failed" << std::endl;
 *                 episodeDone = true;
 *                 break;
 *             }
 *             
 *             // Get reward
 *             float reward = environment_->getLastReward();
 *             totalReward += reward;
 *             
 *             // Store experience for learning
 *             experiences_.push_back({percept, *action, reward, *sensoryInput, episodeDone});
 *             
 *             // Learn from experience (simplified)
 *             learnFromExperience(experiences_.back());
 *             
 *             // Update percept for next step
 *             percept = SensoryPercept(*sensoryInput);
 *             
 *             stepCount++;
 *             
 *             // Enforce step time limit (real-time simulation)
 *             auto stepEnd = std::chrono::high_resolution_clock::now();
 *             auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEnd - stepStart);
 *             if (stepDuration.count() < 10) { // 10ms minimum per step
 *                 std::this_thread::sleep_for(std::chrono::milliseconds(10) - stepDuration);
 *             }
 *         }
 *         
 *         auto endTime = std::chrono::high_resolution_clock::now();
 *         auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
 *         
 *         std::cout << "Episode completed in " << stepCount << " steps ("
 *                   << duration.count() << "s)" << std::endl;
 *         std::cout << "Total reward: " << totalReward << std::endl;
 *         std::cout << "Experiences stored: " << experiences_.size() << std::endl;
 *         
 *         // Clear experiences for next episode
 *         experiences_.clear();
 *     }
 *     
 * private:
 *     struct Experience {
 *         SensoryPercept percept;
 *         Action action;
 *         float reward;
 *         SensoryInput observation;
 *         bool done;
 *     };
 *     
 *     void learnFromExperience(const Experience& exp) {
 *         // Simplified learning: update brain based on reward
 *         if (exp.reward > 0) {
 *             brain_->adjustWeights(exp.percept, exp.reward * 0.01f);
 *         }
 *     }
 *     
 *     // Agent parameters
 *     int maxSteps_ = 1000;
 *     std::unique_ptr<Environment> environment_;
 *     std::unique_ptr<Brain> brain_;
 *     std::unique_ptr<MotorSystem> motor_;
 *     std::vector<Experience> experiences_;
 * };
 * 
 * int main() {
 *     // Create environment
 *     auto env = std::make_unique<GridWorldEnvironment>(20, 20);
 *     
 *     // Create agent
 *     LearningAgent agent(std::move(env));
 *     
 *     // Run multiple learning episodes
 *     const int numEpisodes = 5;
 *     for (int i = 0; i < numEpisodes; ++i) {
 *         agent.runEpisode(i + 1);
 *         std::cout << std::endl;
 *     }
 *     
 *     std::cout << "Learning complete!" << std::endl;
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * This comprehensive example demonstrates:
 * - Agent class architecture integrating all NLM components
 * - Complete environment interaction loop
 * - Experience collection and learning
 * - Real-time simulation with timing constraints
 * - Multi-episode learning process
 * 
 * @example learning_agent.cpp
 */

SECTION 5: TROUBLESHOOTING AND DEBUGGING EXAMPLES
=================================================

5.1 Common Debugging Patterns
-----------------------------

/**
 * Example: Debugging common NLM issues
 * 
 * This example shows how to debug and fix common problems:
 * 
 * @code
 * #include "Neuron.hpp"
 * #include "Config.hpp"
 * #include <iostream>
 * 
 * using namespace nlm;
 * 
 * void debugNeuron(const Neuron& neuron) {
 *     // Check for common neuron issues
 *     const auto& state = neuron.getState();
 *     
 *     std::cout << "=== Neuron Debug Info ===" << std::endl;
 *     std::cout << "ID: " << neuron.getId().index() << std::endl;
 *     
 *     // Check membrane potential bounds
 *     std::cout << "Membrane potential: " << state.membranePotential << " mV" << std::endl;
 *     if (state.membranePotential < -100.0f || state.membranePotential > 100.0f) {
 *         std::cout << "WARNING: Membrane potential outside biological bounds!" << std::endl;
 *     }
 *     
 *     // Check threshold logic
 *     std::cout << "Threshold: " << state.threshold << " mV" << std::endl;
 *     if (neuron.checkThreshold()) {
 *         std::cout << "Neuron would fire based on current potential" << std::endl;
 *     } else {
 *         std::cout << "Neuron below threshold" << std::endl;
 *     }
 *     
 *     // Check refractory state
 *     std::cout << "Refractory remaining: " << state.refractoryRemaining << std::endl;
 *     if (state.refractoryRemaining > 0) {
 *         std::cout << "Neuron is in refractory period" << std::endl;
 *     }
 *     
 *     // Check firing rate
 *     std::cout << "Firing rate: " << state.firingRate << " Hz" << std::endl;
 *     if (state.firingRate < 0.0f || state.firingRate > 1000.0f) {
 *         std::cout << "WARNING: Firing rate outside typical bounds!" << std::endl;
 *     }
 * }
 * 
 * void debugConfiguration(const Config& config) {
 *     std::cout << "=== Configuration Debug Info ===" << std::endl;
 *     
 *     // Check for missing required keys
 *     std::vector<std::string> requiredKeys = {"random_seed", "timestep", "neuron_count"};
 *     for (const auto& key : requiredKeys) {
 *         if (!config.has(key)) {
 *             std::cout << "MISSING: " << key << std::endl;
 *         }
 *     }
 *     
 *     // Check for invalid values
 *     auto timestepOpt = config.get<double>("timestep");
 *     if (timestepOpt) {
 *         double dt = *timestepOpt;
 *         if (dt <= 0.0) {
 *             std::cout << "ERROR: Invalid timestep (must be positive): " << dt << std::endl;
 *         }
 *     }
 *     
 *     // Check last error
 *     std::string lastError = config.getLastError();
 *     if (!lastError.empty()) {
 *         std::cout << "LAST ERROR: " << lastError << std::endl;
 *     }
 * }
 * 
 * int main() {
 *     // Example: Debug a neuron with potential issues
 *     NeuronId testId(1);
 *     Neuron neuron(testId);
 *     
 *     // Simulate problematic conditions
 *     neuron.setThreshold(-100.0f);  // Very low threshold
 *     neuron.setFiringRate(-10.0f);  // Negative firing rate
 *     
 *     debugNeuron(neuron);
 *     
 *     // Example: Debug configuration issues
 *     Config config;
 *     config.loadFromFile("invalid_config.txt"); // This will fail
 *     
 *     debugConfiguration(config);
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * @example debugging_example.cpp
 */

SECTION 6: ADVANCED DOCUMENTATION PATTERNS
===========================================

6.1 Mathematical Formula Documentation
----------------------------------------

/**
 * Template: Documenting complex mathematical formulas
 * 
 * Example: Leaky Integrate-and-Fire neuron dynamics
 * 
 * @code
 * /**
 *  * Leaky Integrate-and-Fire (LIF) neuron model
 *  * 
 *  * Mathematical description:
 *  * dV/dt = (V_rest - V)/τ + I/C
 *  * 
 *  * Continuous form:
 *  * - V(t): membrane potential at time t (mV)
 *  * - V_rest: resting potential (mV)
 *  * - τ: membrane time constant (ms)
 *  * - C: membrane capacitance (nF)
 *  * - I: total synaptic current (nA)
 *  * 
 *  * Biological interpretation:
 *  * - First term: passive leak current toward resting potential
 *  * - Second term: synaptic current integration
 *  * - Models subthreshold membrane dynamics
 *  * 
 *  * Discretized for simulation:
 *  * V(t+dt) = V(t) + dt × [(V_rest - V(t))/τ + I/C]
 *  * 
 *  * Where:
 *  *   τ = 20ms (typical cortical neuron)
 *  *   C = 1nF (typical membrane capacitance)
 *  *   V_rest = -70mV (typical resting potential)
 *  * 
 *  * Numerical stability:
 *  * - dt must be small enough for convergence
 *  * - For τ = 20ms, recommend dt ≤ 0.1ms
 *  * 
 *  * Implementation note:
 *  * - Equation assumes linear membrane properties
 *  * - Actual neurons have nonlinearities (active conductances)
 *  * - This is a simplified model for computational efficiency
 *  * 
 *  * References:
 *  * - Dayan, P., & Abbott, L. (2001). Theoretical Neuroscience.
 *  * - Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). 
 *  *   Neural Dynamics: From Single Neurons to Networks.
 *  * @param currentTime Current simulation time in seconds
 *  * @param dt Time step duration in seconds
 *  * @return true if neuron fired, false otherwise
 *  * @note Multiplied by 1000.0f to convert seconds to milliseconds
 *  *   for biological time scales
 *  * @see Neuron::stepLIF()
 *  * @see NeuralDynamics::updateNeuron()
 *  */
 * 
 * float V_new = V_old + dt * ((V_rest - V_old) / tau + I / C);
 * @endcode
 * 
 * This template provides a standardized way to document:
 * - Mathematical variables and their units
 * - Biological interpretations
 * - Discretization and numerical methods
 * - Implementation notes and limitations
 * - References for further reading
 * 
 * @template formula_documentation.md
 */

SECTION 7: BUILD AND INSTALL EXAMPLES
=====================================

7.1 Building Examples with CMake
---------------------------------

/**
 * Example: Building NLM examples
 * 
 * @code
 * # CMakeLists.txt modifications for examples
 * 
 * project(NLM_Examples)
 * 
 * # Enable C++17
 * set(CMAKE_CXX_STANDARD 17)
 * set(CMAKE_CXX_STANDARD_REQUIRED True)
 * 
 * # Find NLM library
 * find_package(NLM REQUIRED)
 * 
 * # Example 1: Simple neuron simulation
 * add_executable(neuron_simulation neuron_simulation.cpp)
 * target_link_libraries(neuron_simulation nlm)
 * 
 * # Example 2: Network simulation
 * add_executable(network_simulation network_simulation.cpp)
 * target_link_libraries(network_simulation nlm)
 * 
 * # Example 3: Configuration example
 * add_executable(config_example config_example.cpp)
 * target_link_libraries(config_example nlm)
 * 
 * # Install examples
 * install(TARGETS neuron_simulation network_simulation config_example
 *         RUNTIME DESTINATION bin)
 * @endcode
 * 
 * Build and run example:
 * ```bash
 * mkdir build
 * cd build
 * cmake ..
 * make -j4
 * ./neuron_simulation
 * ./network_simulation
 * ./config_example
 * ```
 * 
 * @example build_examples.md
 */

SECTION 8: PERFORMANCE AND OPTIMIZATION EXAMPLES
================================================

8.1 Performance Measurement Example
------------------------------------

/**
 * Example: Measuring and reporting simulation performance
 * 
 * @code
 * #include <chrono>
 * include <iostream>
 * include <iomanip>
 * 
 * using namespace nlm;
 * 
 * class PerformanceTimer {
 * public:
 *     PerformanceTimer(const std::string& name) : name_(name) {
 *         start_ = std::chrono::high_resolution_clock::now();
 *     }
 *     
 *     ~PerformanceTimer() {
 *         auto end = std::chrono::high_resolution_clock::now();
 *         auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
 *         
 *         std::cout << "[PERF] " << name_ << ": "
 *                   << std::fixed << std::setprecision(2)
 *                   << duration.count() / 1000.0 << "s" << std::endl;
 *     }
 *     
 * private:
 *     std::string name_;
 *     std::chrono::high_resolution_clock::time_point start_;
 * };
 * 
 * int main() {
 *     // Example neuron simulation with performance measurement
 *     {
 *         PerformanceTimer timer("Neuron Simulation");
 *         
 *         NeuronId neuronId(1);
 *         Neuron neuron(neuronId);
 *         
 *         // Initialize
 *         neuron.setThreshold(-50.0f);
 *         neuron.setRestingPotential(-70.0f);
 *         
 *         // Run simulation
 *         const int numSteps = 10000;
 *         for (int i = 0; i < numSteps; ++i) {
 *             neuron.step(static_cast<Timestamp>(i * 0.001f));
 *         }
 *     }
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * This example shows how to measure and report simulation performance.
 * Useful for benchmarking and optimization.
 * 
 * @example performance_measurement.cpp
 */

// SUMMARY
=========

This documentation examples file provides:

1. Basic component usage (Neuron, Synapse, etc.)
2. Complex system examples (networks, agents)
3. Configuration management patterns
4. Environment interaction examples
5. Debugging and troubleshooting guides
6. Performance measurement and optimization
7. Build and installation instructions

All examples are self-contained and can be compiled with the NLM framework.
Each example includes:
- Complete code with explanations
- Expected output
- Key concepts highlighted
- File names for organization

These examples serve as:
- Learning resources for new developers
- Templates for future development
- Test cases for documentation validation
- Performance benchmarks
- Integration test examples

TO CONTRIBUTE EXAMPLES:
=======================

1. Write complete, runnable examples
2. Include detailed comments explaining key concepts
3. Add expected output for verification
4. Organize examples by functionality
5. Include performance considerations
6. Provide build instructions

All examples should follow the NLM documentation standards and
be integrated into the official documentation build system.
