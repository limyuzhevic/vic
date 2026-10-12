// Doxygen-style comment templates for NLM codebase
// ================================================
// This file contains standardized comment templates for various code elements
// Copy and paste these templates into your source files as needed

/**
 * FILE: [filename]
 * DESCRIPTION: High-level module description
 * AUTHORS: [author list]
 * DATE: [creation date]
 * VERSION: [version number]
 * COPYRIGHT: [copyright notice]
 * 
 * This module implements [brief functionality description].
 * 
 * Key features:
 * - Feature 1
 * - Feature 2
 * - Feature 3
 * 
 * Dependencies:
 *   - Library dependency 1
 *   - Library dependency 2
 * 
 * File structure:
 * - Class/function definitions
 * - Helper utilities
 * - Implementation details
 * 
 * ile
 */

/**
 * CLASS: [ClassName]
 * DESCRIPTION: Brief class description
 * 
 * Detailed class description providing:
 * - Purpose and motivation
 * - Key responsibilities
 * - Relationship to other classes
 * - Usage patterns
 * 
 * Class inheritance:
 * @inherits [ParentClass] "if inheriting from another class"
 * 
 * Attributes:
 * @attribute [access] [type] [name] [description]
 * 
 * Methods:
 * @method [access] [return_type] [method_name] [brief_description]
 * 
 * Examples:
 * @code
 * [Example code snippet]
 * @endcode
 * 
 * @class
 */

/**
 * STRUCT: [StructName]
 * DESCRIPTION: Data structure purpose and meaning
 * 
 * Used for:
 * - [Use case 1]
 * - [Use case 2]
 * 
 * Member variables:
 * @field [type] [name] [description]
 * @field [type] [name] [description]
 * 
 * @struct
 */

/**
 * ENUM: [EnumName]
 * DESCRIPTION: Enumeration purpose and semantic meaning
 * 
 * Used to represent [domain-specific concepts].
 * 
 * @enum [EnumName]
 * @enumerator [value] "Description of this enumeration value"
 * @enumerator [value] "Description of this enumeration value"
 * 
 * The values typically represent:
 * - Value 1: [semantic meaning]
 * - Value 2: [semantic meaning]
 * - etc.
 */

/**
 * FUNCTION: [function_signature]
 * DESCRIPTION: Function purpose and what it accomplishes
 * 
 * Detailed explanation of:
 * - What the function does
 * - Why it exists
 * - When/how to use it
 * - Special considerations
 * 
 * Parameters:
 * @param [parameter_name] [description]
 *   - More details about the parameter
 *   - Constraints or requirements
 *   - Default values if applicable
 * 
 * Return value:
 * @return [description of return value]
 *   - Return codes meaning
 *   - What the value represents
 *   - Special return cases
 * 
 * Throws:
 * @throws [exception_type] [condition]
 *   - Exception thrown when [condition]
 * 
 * Examples:
 * @code
 * [Example usage showing function call]
 * @endcode
 * 
 * See also:
 * @see related_function() "Related function with similar purpose"
 * 
 * @function
 */

/**
 * FUNCTION (overloaded): [function_signature]
 * DESCRIPTION: Overloaded function version
 * 
 * Differentiator from other overloads:
 * - Which parameter types distinguish this version
 * - Why this overload exists
 * 
 * @overload
 */

/**
 * VARIABLE: [variable_name]
 * DESCRIPTION: Variable purpose and meaning
 * 
 * @var [type] [name] [description]
 */

/**
 * NAMESPACE: nlm
 * DESCRIPTION: Neural Learning Machine framework namespace
 * 
 * This namespace encompasses the entire NLM cognitive architecture
 * for implementing autonomous learning agents.
 * 
 * Key components:
 * - Brain simulation (neurons, synapses, plasticity)
 * - Sensory processing and perception
 * - Motor control and action selection
 * - Learning and memory systems
 * - Environmental interaction
 * 
 * Usage: All classes and functions in this codebase are contained
 * within the nlm namespace to avoid naming conflicts.
 * 
 * @namespace nlm
 */

/**
 * CONCEPT: [concept_name]
 * DESCRIPTION: Abstract concept or interface
 * 
 * Defines the contract for [concept].
 * 
 * Requirements:
 * - Requirement 1
 * - Requirement 2
 * 
 * Typical implementations:
 * - Implementation 1
 * - Implementation 2
 * 
 * @concept
 */

// USAGE EXAMPLES IN DOCUMENTATION
// ================================

/**
 * EXAMPLE: Creating and using a Neuron
 * 
 * @code
 * #include "Neuron.hpp"
 * #include "../core/Random/Random.hpp"
 * 
 * // Create neuron with unique ID
 * nlm::NeuronId neuronId(42);
 * nlm::Neuron neuron(neuronId);
 * 
 * // Initialize with biologically plausible parameters
 * nlm::RandomGenerator rng;
 * rng.seed(12345);
 * neuron.initializeRandom(rng);
 * 
 * // Configure neuron properties
 * neuron.setType(nlm::NeuronType::Excitatory);
 * neuron.setThreshold(-50.0f); // Spike at -50mV
 * neuron.setFiringRate(10.0f); // 10Hz target rate
 * 
 * // Simulate for 100ms at 1kHz timestep
 * for (int step = 0; step < 100; ++step) {
 *     float time = step * 0.001f; // seconds
 *     bool fired = neuron.stepLIF(time, 0.001f);
 *     
 *     if (fired) {
 *         std::cout << "Neuron " << neuron.getId().index()
 *                   << " fired at time " << time << "s" << std::endl;
 *     }
 *     
 *     // Add random synaptic input
 *     neuron.receiveExcitatoryInput(2.5f);
 * }
 * @endcode
 * 
 * Output:
 * Neuron 42 fired at time 0.123s
 * Neuron 42 fired at time 0.456s
 * 
 * @example
 */

/**
 * EXAMPLE: Configuration usage
 * 
 * @code
 * #include "Config.hpp"
 * 
 * int main() {
 *     nlm::Config config;
 *     
 *     // Load from JSON configuration file
 *     if (!config.loadFromFile("agent_config.json")) {
 *         std::cerr << "Failed to load config: " 
 *                   << nlm::Config::getLastError() << std::endl;
 *         return 1;
 *     }
 *     
 *     // Set runtime parameters
 *     config.set("random_seed", 12345LL);
 *     config.set("simulation_timestep", 0.001);
 *     config.set("neuron_count", 100);
 *     
 *     // Retrieve configuration values with type safety
 *     auto timestep = config.get<double>("simulation_timestep");
 *     if (timestep) {
 *         std::cout << "Using timestep: " << *timestep << "s" << std::endl;
 *     }
 *     
 *     // Get default value when key doesn't exist
 *     auto max_neurons = config.getOr<int>("neuron_count", 50);
 *     std::cout << "Max neurons: " << max_neurons << std::endl;
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * @example
 */

/**
 * EXAMPLE: STDP learning example
 * 
 * @code
 * #include "STDP.hpp"
 * #include "Synapse.hpp"
 * 
 * // Create synapse for STDP learning
 * nlm::SynapseId synId(1);
 * nlm::Synapse synapse(synId, nlm::NeuronId(100), nlm::NeuronId(200));
 * 
 * // Create STDP learning rule
 * nlm::STDP stdp;
 * 
 * // Configure STDP parameters
 * stdp.configure(0.01f,   // LTP weight (A+)
 *               0.012f,  // LTD weight (A-)
 *               20.0f);  // Time constant (τ)
 * 
 * // Simulate spike timing
 * std::vector<nlm::Timestamp> preSpikes{0.0f, 5.0f, 10.0f};  // Pre-synaptic spikes
 * std::vector<nlm::Timestamp> postSpikes{2.5f, 7.5f};       // Post-synaptic spikes
 * 
 * // Apply STDP learning
 * nlm::TimestepDuration dt = 0.001f; // 1ms timestep
 * stdp.update(&synapse, preSpikes, postSpikes, dt);
 * 
 * // Check resulting synaptic weight
 * float weight = synapse.getWeight();
 * std::cout << "Synaptic weight after STDP: " << weight << std::endl;
 * @endcode
 * 
 * This example demonstrates how spike timing affects synaptic strength:
 * - Pre-before-post spikes → potentiation (weight increases)
 * - Post-before-pre spikes → depression (weight decreases)
 * - Simultaneous spikes → no change
 * 
 * @example
 */

/**
 * EXAMPLE: Environment interaction
 * 
 * @code
 * #include "Environment.hpp"
 * #include "MotorSystem.hpp"
 * 
 * // Create specific environment (grid world for testing)
 * nlm::GridWorldEnvironment env(10, 10);  // 10x10 grid
 * 
 * // Reset environment to initial state
 * auto observation = env.reset();
 * 
 * bool episode_done = false;
 * int step_count = 0;
 * float total_reward = 0.0f;
 * 
 * while (!episode_done && step_count < 1000) {
 *     // Get available actions from motor system
 *     MotorSystem motor;
 *     auto available_actions = motor.getAvailableActions();
 *     
 *     // For simplicity, use first available action
 *     if (!available_actions.empty()) {
 *         ActionType action = available_actions[0];
 *         
 *         // Create action
 *         Action action_obj(action);
 *         
 *         // Take action in environment
 *         observation = env.step(action_obj);
 *         
 *         // Get reward for this action
 *         float reward = env.getLastReward();
 *         total_reward += reward;
 *         
 *         // Check if episode is complete
 *         episode_done = env.isDone();
 *     }
 *     
 *     step_count++;
 * }
 * 
 * std::cout << "Episode completed after " << step_count << " steps" << std::endl;
 * std::cout << "Total reward: " << total_reward << std::endl;
 * @endcode
 * 
 * This example shows how an agent interacts with an environment:
 * - Reset: Initialize environment state
 * - Step: Take action and receive new observation
 * - Reward: Accumulate rewards for learning
 * - Done: Detect episode termination
 * 
 * @example
 */

// TEMPLATE FOR COMPLEX MATHEMATICAL FORMULAS
// ============================================

/**
 * Mathematical formula documentation template
 * 
 * Use this template for documenting complex mathematical operations:
 * 
 * @code
 * /**
 *  * Leaky Integrate-and-Fire (LIF) neuron dynamics
 *  * 
 *  * Mathematical model:
 *  * dV/dt = (V_rest - V)/τ + I/C
 *  * 
 *  * Where:
 *  *   V: membrane potential (mV)
 *  *   V_rest: resting potential (mV)
 *  *   τ: membrane time constant (ms)
 *  *   C: membrane capacitance (nF)
 *  *   I: total synaptic current (nA)
 *  * 
 *  * Discretized form (Euler integration):
 *  * V(t+dt) = V(t) + dt × [(V_rest - V(t))/τ + I/C]
 *  * 
 *  * Biological interpretation:
 *  * - Models passive membrane properties
 *  * - Neurons "leak" charge toward resting potential
 *  * - Synaptic inputs push potential toward threshold
 *  * 
 *  * @param currentTime Current simulation time in seconds
 *  * @param dt Time step duration in seconds
 *  * @return true if neuron fired, false otherwise
 *  * @note The factor 1000.0f converts seconds to milliseconds for biological realism
 *  * @see Neuron::stepLIF()
 *  */
 * 
 * float V_new = V_old + dt * ((V_rest - V_old) / tau + I / C);
 * @endcode
 */

// TEMPLATE FOR CONFIGURATION CLASSES
// ====================================

/**
 * Configuration template for key-value storage
 * 
 * Use this template for configuration management classes:
 * 
 * @code
 * /**
 *  * Configuration system for agent parameters
 *  * 
 *  * Supports multiple configuration sources:
 *  * - Default: Values from code (hardcoded)
 *  * - File: Configuration files (JSON, INI, key=value)
 *  * - Command line: Command line arguments
 *  * - Runtime: Set programmatically during execution
 *  * - Environment: Environment variables
 *  * - Schema: Validation against configuration schema
 *  * 
 *  * Key features:
 *  * - Type-safe value access
 *  * - Configuration validation
 *  * - Change notification system
 *  * - Persistence to disk
 *  * 
 *  * Usage example:
 *  * @code
 *  * nlm::Config config;
 *  * config.loadFromFile("agent_config.json");
 *  * auto seed = config.getOr<long>("random_seed", 12345LL);
 *  * @endcode
 *  * 
 *  * @file
 *  * @class Config
 *  */
 */

// TEMPLATE FOR ABSTRACT BASE CLASSES
// ====================================

/**
 * Abstract base class template
 * 
 * Use this template for abstract base classes defining interfaces:
 * 
 * @code
 * /**
 *  * Abstract base class for plasticity rules
 *  * 
 *  * Defines the interface for synaptic plasticity mechanisms in the brain.
 *  * Concrete implementations include STDP, Hebbian, BCM, and anti-Hebbian rules.
 *  * 
 *  * Plasticity is the process by which synaptic strengths change based on
 *  * neural activity, enabling learning and memory.
 *  * 
 *  * Key responsibilities:
 *  * - Detect pre- and post-synaptic activity patterns
 *  * - Compute appropriate weight changes
 *  * - Apply changes to synaptic connections
 *  * 
 *  * The learning rule must be implemented by all concrete classes.
 *  * 
 *  * Examples:
 *  * @code
 *  * class STDP : public PlasticityRule {
 *  * public:
 *  *     void update(Synapse* synapse,
 *  *                  const std::vector<Timestamp>& preSpikes,
 *  *                  const std::vector<Timestamp>& postSpikes,
 *  *                  TimestepDuration dt) override;
 *  *     
 *  *     void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
 *  *     
 *  *     const char* getName() const override;
 *  * };
 *  * @endcode
 *  * 
 *  * @abstract
 *  * @class PlasticityRule
 *  */
 */

// BEST PRACTICES FOR DOCUMENTATION
// =================================

/**
 * Guidelines for effective code documentation:
 * 
 * 1. Be concise but complete:
 *    - Explain WHAT the code does
 *    - Explain WHY it does it (motivation)
 *    - Explain HOW it works (technical details)
 *    - Explain WHEN/WHERE to use it
 *    
 * 2. Use consistent formatting:
 *    - Use Doxygen comment style /** ... */
 *    - Use @param for parameters
 *    - Use @return for return values
 *    - Use @code / @endcode for examples
 *    
 * 3. Keep documentation up-to-date:
 *    - Update documentation when code changes
 *    - Review documentation with code reviews
 *    - Use automated documentation tools
 *    
 * 4. Focus on the user:
 *    - Assume user has basic domain knowledge
 *    - Provide examples for common use cases
 *    - Document edge cases and error conditions
 *    - Point to related functionality
 *    
 * 5. Follow project standards:
 *    - Use organization's coding standards
 *    - Follow existing documentation patterns
 *    - Use project-specific terminology
 *    - Reference project documentation
 *    
 * @note These guidelines should be incorporated into your team's
 *        documentation standards and code review process.
 */
