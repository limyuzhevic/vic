# NLM Doxygen Documentation Implementation Plan

## Phase 1: Foundation and Templates (Weeks 1-2)

### 1.1 Create Documentation Templates Directory
```bash
mkdir -p docs/templates doxygen
mkdir -p scripts/validation
```

### 1.2 Copy Reference Headers for Documentation Standards
We'll use Neuron.hpp, Config.hpp, and Brain.hpp as the gold standard for documentation.

### 1.3 Create Documentation Validation Script

```python
# scripts/validation/check_doc_coverage.py
#!/usr/bin/env python3

import os
import re
import sys
from pathlib import Path

def check_doxygen_comments(filepath):
    """Check if a header file has adequate Doxygen documentation."""
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Check for class documentation
    class_pattern = r'class\s+\w+\s*\{'
    if re.search(class_pattern, content):
        # Check for /** after class definition
        class_pos = content.find('class')
        class_section = content[class_pos:class_pos + 2000]
        if '/**' not in class_section:
            return False, "Missing class documentation"
    
    # Check for struct documentation
    struct_pattern = r'struct\s+\w+\s*\{'
    if re.search(struct_pattern, content):
        struct_pos = content.find('struct')
        struct_section = content[struct_pos:struct_pos + 2000]
        if '/**' not in struct_section:
            return False, "Missing struct documentation"
    
    return True, "Documentation adequate"

def analyze_documentation_coverage(base_path):
    """Analyze documentation coverage across all headers."""
    headers = list(Path(base_path).glob("*.hpp"))
    
    total_headers = len(headers)
    documented_headers = 0
    undocumented_files = []
    
    for header in headers:
        is_documented, details = check_doxygen_comments(header)
        if is_documented:
            documented_headers += 1
        else:
            undocumented_files.append((header, details))
    
    coverage_percentage = (documented_headers / total_headers) * 100 if total_headers > 0 else 0
    
    return {
        'total_headers': total_headers,
        'documented_headers': documented_headers,
        'undocumented_files': undocumented_files,
        'coverage_percentage': coverage_percentage
    }

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='Check Doxygen documentation coverage')
    parser.add_argument('--path', default='./src', help='Path to source directory')
    
    args = parser.parse_args()
    
    results = analyze_documentation_coverage(args.path)
    
    print(f"Total Headers: {results['total_headers']}")
    print(f"Documented Headers: {results['documented_headers']}")
    print(f"Documentation Coverage: {results['coverage_percentage']:.1f}%")
    
    if results['undocumented_files']:
        print("\nUndocumented Headers:")
        for header, details in results['undocumented_files']:
            print(f"  - {header}: {details}")
    
    sys.exit(0 if results['coverage_percentage'] >= 90 else 1)
```

### 1.4 Create Doxygen Configuration

```xml
<!-- doxygen/Doxyfile -->
<!DOCTYPE doxygen PUBLIC "-//Doxygen//DTD Doxygen 1.9.1//EN" "https://www.doxygen.org/doxygen-1.9.1.dtd">
<doxygenversion>1.9.1</doxygenversion>

PROJECT_NAME           = "NLM Documentation"
PROJECT_NUMBER         = "Phase 6"
OUTPUT_DIRECTORY       = docs/output
GENERATE_LATEX         = NO
GENERATE_RTF           = NO
GENERATE_HTML          = YES
GENERATE_XML           = YES
GENERATE_MD            = YES
GENERATE_MAN           = NO
GENERATE_TESTGEN       = NO

INPUT                  = src/
INPUT                  = docs/templates
EXCLUDE                = */build/*
                     */tests/*
                     */experiments/*
                     */third_party/*

ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = YES
EXPAND_AS_DEFINED      = nlm

GENERATE_BRIEF_MEMBER_DESC = YES
BRIEF_MEMBER_DESC      = YES
SORT_BRIEF_DOCS        = YES
CLASS_DIAGRAMS         = YES
HIDE_UNPUBLIC_MEMBERS   = YES

ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_COLORSTYLE_HUE    = 220
HTML_COLORSTYLE_SAT    = 100
HTML_EXTRA_STYLESHEET  = style.css

SEARCHENGINE           = YES
SEARCHENGINE_URL       = https://www.google.com/search
```

## Phase 2: Create Implementation Templates (Weeks 2-3)

### 2.1 Neuron.hpp Documentation Template (Updated)

```cpp
/**
 * @brief Individual neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * This class represents a single biological neuron model with realistic biophysical
 * properties and learning capabilities. Each neuron maintains its own state
 * and responds to synaptic inputs, generating spikes according to integrate-and-fire
 * dynamics. Neurons are the fundamental computational units of the NLM brain.
 * 
 * The neuron operates with realistic biophysical parameters:
 * - Membrane potential (-70mV resting, -55mV threshold)
 * - Refractory period after spike generation
 * - Synaptic input integration (excitatory, inhibitory, modulatory)
 * - Activity-dependent firing rate modulation
 * 
 * Key features:
 * - Realistic LIF dynamics with membrane potential integration
 * - Spike generation and refractory period handling
 * - Synaptic input processing (excitatory, inhibitory, modulatory)
 * - Plasticity rule application (STDP, Hebbian, reward-modulated)
 * - Developmental stage awareness and adaptation
 * - Region and population membership tracking
 * - Activity-dependent firing rate modulation
 * 
 * Neuron's role in the brain architecture:
 * - Receives synaptic inputs from other neurons
 * - Processes inputs through integrate-and-fire dynamics
 * - Generates spikes for communication with postsynaptic neurons
 * - Participates in plasticity learning mechanisms
 * - Contributes to population-level computations
 * - Responds to neuromodulatory signals
 * 
 * @note This implementation represents a simplified biological neuron model.
 * Real neurons have more complex dynamics including ion channel states,
 * dendritic processing, and detailed morphology.
 * 
 * @see Brain.hpp for brain-wide coordination
 * @see Synapse.hpp for synaptic connections
 * @see Config.hpp for neuron parameters
 */
class Neuron {
public:
    /**
     * @brief Construct a new Neuron with specified identifier
     * 
     * Creates a new neuron instance with the given identifier and initializes
     * it to a physiologically realistic resting state. The neuron is ready to
     * receive synaptic inputs and participate in brain computations.
     * 
     * @param id Unique identifier for this neuron (must be unique within the brain)
     * 
     * @note The neuron is initialized to resting state with default
     *       physiological parameters. Further initialization (random parameters,
     *       synaptic connections) should be done through initialization methods.
     * @see initializeRandom() for parameter randomization
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the Neuron instance and clean up resources
     * 
     * Ensures proper cleanup of neuron state and resources. The neuron should
     * be in a clean state (typically reset) before destruction to maintain
     * brain system consistency.
     * 
     * @note This destructor is noexcept to prevent exceptions during cleanup,
     *       which could occur during brain shutdown or error conditions.
     */
    ~Neuron();
    
    // Disable copying, enable moving for efficient resource management
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // -------------------------------------------------------------------------
    // NEURON IDENTIFICATION AND CLASSIFICATION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * Returns the neuron's unique identifier, which is used for neuron identification
     * in brain-wide operations and to maintain the type-safe neural reference system.
     * 
     * @return NeuronId The neuron's identifier
     * 
     * @note This ID is immutable and can be used to reference the neuron
     *       throughout the brain system. Use getId() for identification,
     *       getRegionId() for regional classification.
     * @see getRegionId() for regional context
     * @see getPopulationId() for population context
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type (Excitatory, Inhibitory, Modulatory, etc.)
     * 
     * Returns the neuron's functional type, which determines its behavior and role
     * in the network. Neuron type influences synaptic transmission, plasticity eligibility,
     * and computational role.
     * 
     * @return NeuronType The neuron's functional type
     * 
     * @note Neuron type determines:
     *       - How the neuron affects postsynaptic membrane potential
     *       - Which plasticity rules apply to its synapses
     *       - Its role in network computation
     *       - How it responds to neuromodulatory signals
     * 
     * @see setType() to modify neuron type
     * @see NeuronType enum for available types
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * Changes the neuron's functional type, which affects its behavior and interaction
     * patterns. Type changes should be used with caution as they may disrupt network dynamics.
     * 
     * @param type New neuron type to assign
     * 
     * @note This operation affects:
     *       - Synaptic transmission properties
     *       - Plasticity rule eligibility
     *       - Neuromodulation response
     *       - Computational role in the network
     *       
     * @warning Changing neuron type may create inconsistent network behavior.
     * Consider this operation carefully, especially in running simulations.
     * @see getType() for current type
     * @see NeuronType enum for valid values
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's complete state structure
     * 
     * Returns a reference to the neuron's internal state structure containing all
     * biophysical parameters and dynamic variables. This provides read-only access
     * to the neuron's state, allowing inspection and statistical analysis.
     * 
     * @return const NeuronState& Reference to the neuron's state
     * 
     * @note The state structure includes:
     *       - Membrane potential and ionic concentrations
     *       - Firing history and refractory period
     *       - Synaptic conductances and plasticity flags
     *       - Developmental and adaptation variables
     *       
     * @warning This is a const reference. For state modification, use getState()
     *       overload or specific setter methods for controlled changes.
     * @see getState() for mutable access
     * @see NeuronState struct for field descriptions
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to the neuron's state structure
     * 
     * Returns a mutable reference to the neuron's internal state structure, allowing
     * direct modification of neuron's biophysical parameters. Use with caution to
     * avoid corrupting neuron state or creating inconsistent neural configurations.
     * 
     * @return NeuronState& Mutable reference to the neuron's state
     * 
     * @warning Direct state modification bypasses validation and consistency checks.
     * Use this method only when:
     *       - Implementing advanced simulation techniques
     *       - Testing edge cases and error conditions
     *       - Implementing custom control or debugging procedures
     *       
     * @note For typical neuron operation, prefer specific setter methods:
     *       - setMembranePotential()
     *       - setFiringState()
     *       - setType()
     *       - enablePlasticity()
     *       
     * @see getState() for read-only access
     * @see NeuronState struct for field descriptions
     */
    NeuronState& getState();
    
    // -------------------------------------------------------------------------
    // BASIC STATE MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the current membrane potential
     * 
     * Returns the neuron's current membrane potential, which represents the
     * electrical charge difference across the neuronal membrane. This is the
     * primary variable that determines neuron's firing behavior.
     * 
     * @return MembranePotential Current membrane potential (mV)
     * 
     * @note Range: typically -80mV to +40mV. Firing threshold is usually around -55mV.
     *       Positive values indicate depolarization (excitatory), negative values indicate
     *       hyperpolarization (inhibitory). Values outside physiological range may
     *       indicate simulation errors or extreme conditions.
     *       
     * @see getThreshold() for firing threshold
     * @see setMembranePotential() for modification
     * @see isFiring() for firing determination
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * Sets the neuron's membrane potential to the specified value. This provides
     * direct control over neuronal state, useful for controlled experiments,
     * testing specific responses, or implementing external control protocols.
     * 
     * @param potential New membrane potential (mV)
     * 
     * @note Use this method for:
     *       - Controlled experimental protocols
     *       - Testing neuron responses to specific inputs
     *       - Implementing external stimulation or control
     *       - Setting initial conditions for simulations
     *       
     * @warning This bypasses normal synaptic integration mechanisms.
     * For typical neuron operation, use synaptic input methods instead:
     *       - receiveExcitatoryInput()
     *       - receiveInhibitoryInput()
     *       - receiveModulatoryInput()
     *       - step() for dynamic integration
     *       
     * @see getMembranePotential() for current value
     * @see addToMembranePotential() for incremental changes
     * @see stepLIF() for dynamic integration
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a delta to the membrane potential (synaptic current input)
     * 
     * Increments the neuron's membrane potential by the specified delta value,
     * representing synaptic current injection. This is the primary mechanism for
     * neuron response to synaptic inputs and the core of neural integration.
     * 
     * @param delta Change in membrane potential (mV)
     * 
     * @note This method represents the fundamental synaptic input mechanism:
     *       - Positive deltas cause depolarization (excitation)
     *       - Negative deltas cause hyperpolarization (inhibition)
     *       - Magnitude determines input strength
     *       - This is how neurons integrate information
     *       
     * @see receiveExcitatoryInput() for positive input
     * @see receiveInhibitoryInput() for negative input
     * @see receiveModulatoryInput() for modulatory input
     * @see step() for complete integration cycle
     */
    void addToMembranePotential(MembranePotential delta);
    
    // -------------------------------------------------------------------------
    // THRESHOLD AND FIRING CONTROL
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the neuron's firing threshold
     * 
     * Returns the neuron's firing threshold, which is the membrane potential level
     * that must be exceeded for the neuron to generate a spike. This parameter
     * controls neuron excitability and firing patterns.
     * 
     * @return MembranePotential Firing threshold (mV)
     * 
     * @note Threshold typically ranges from -55mV to -40mV:
     *       - Higher thresholds make neurons harder to fire
     *       - Lower thresholds make neurons more excitable
     *       - Threshold affects firing rate and synchrony
     *       - Can be modulated by neuromodulatory signals
     *       
     * @see getMembranePotential() for current value
     * @see setThreshold() for modification
     * @see isFiring() for firing determination
     * @see checkThreshold() for threshold checking
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the neuron's firing threshold
     * 
     * Adjusts the neuron's firing threshold, which affects neuron excitability and
     * firing patterns. Threshold modulation is an important mechanism for neural
     * adaptation and learning.
     * 
     * @param threshold New firing threshold (mV)
     * 
     * @note Threshold modification affects:
     *       - Neuron excitability (higher threshold = less excitable)
     *       - Firing rate and patterns (lower threshold = more firing)
     *       - Network synchrony (threshold affects spike timing)
     *       - Learning dynamics (threshold affects plasticity)
     *       
     * @warning Threshold changes can create inconsistent network dynamics.
     * Use this method carefully, especially in running simulations.
     * 
     * @see getThreshold() for current threshold
     * @see isFiring() for firing determination
     * @note Some neurons have threshold that adapts with activity
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing (above threshold)
     * 
     * Determines whether the neuron should fire based on its current membrane potential
     * relative to the firing threshold. This method checks without modifying neuron state.
     * 
     * @return bool true if neuron is firing, false otherwise
     * 
     * @note This method:
     *       - Performs a threshold comparison (getMembranePotential() >= getThreshold())
     *       - Does not modify neuron state
     *       - Can be called repeatedly without side effects
     *       - Is used by spike detection and firing control
     *       
     * @see getMembranePotential() for current value
     * @see getThreshold() for threshold value
     * @see checkThreshold() for detailed threshold checking
     * @see step() for complete firing process
     * @see recordSpike() for spike recording
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * Determines whether the neuron is currently in its refractory period after a spike.
     * During refractory period, the neuron cannot fire spikes regardless of membrane potential.
     
     * @return bool true if neuron is refractory, false otherwise
     * 
     * @note Refractory period:
     *       - Prevents unrealistic high-frequency firing
     *       - Allows ion channel recovery
     *       - Contributes to temporal coding
     *       - Is typically 2-5 simulation steps
     *       
     * @see setRefractoryPeriod() for modification
     * @see decrementRefractory() for refractory progression
     * @see isFiring() for firing capability
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state directly
     * 
     * Sets the neuron's firing state directly, bypassing normal integration mechanisms.
     * This provides direct control over neuron's firing state for experimental or testing purposes.
     * 
     * @param state New firing state
     * 
     * @note Use this method for:
     *       - Controlled experimental protocols
     *       - Testing specific neural states
     *       - Implementing custom control signals
     *       - Debugging and testing procedures
     *       
     * @warning Direct state modification bypasses validation and consistency checks.
     * For normal neuron operation, allow state changes to occur through normal
     * integration and spike detection mechanisms.
     *       
     * @see getFiringState() for current state
     * @see isFiring() for firing determination
     * @see setMembranePotential() for membrane potential control
     * @note Firing state transitions normally occur through step() methods
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the neuron's refractory period duration
     * 
     * Adjusts the neuron's refractory period duration, which controls how long the neuron
     * remains unable to fire after a spike. This parameter affects firing patterns and
     * temporal coding capabilities.
     * 
     * @param steps Number of simulation steps for refractory period
     * 
     * @note Refractory period effects:
     *       - Higher values prevent high-frequency firing
     *       - Longer periods allow more complete recovery
     *       - Affects temporal coding precision
     *       - Influences network synchrony
     *       
     * @warning Refractory period affects network dynamics significantly.
     * Use this method carefully, especially in running simulations where it may
     * create inconsistent firing patterns.
     * 
     * @see getRefractoryPeriod() for current duration
     * @see isRefractory() for current state
     * @see decrementRefractory() for refractory progression
     * @note Typical values range from 1 to 10 simulation steps
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter by one step
     * 
     * Decrements the neuron's refractory period counter by one simulation step, allowing
     * the neuron to transition from refractory to active state. This method is typically
     * called by the brain system during simulation steps.
     * 
     * @note This method:
     *       - Reduces refractoryRemaining by 1
     *       - May enable future firing if counter reaches zero
     *       - Is called automatically by step() methods
     *       - Allows controlled refractory period progression
     *       
     * @warning This method should be called once per simulation step.
     * Multiple calls may create inconsistent refractory states.
     * 
     * @see isRefractory() for current state
     * @see setRefractoryPeriod() for duration setting
     * @see step() for automatic refractory management
     */
    void decrementRefractory();
    
    // ... Additional template methods would continue with similar documentation
};
```

### 2.2 Config.hpp Documentation Template (Enhanced)

```cpp
/**
 * @brief Main configuration class for the NLM brain system
 * 
 * Manages all configuration parameters including default values, file loading,
 * command-line parsing, and runtime modifications. Provides type-safe value access
 * through templated getter methods.
 * 
 * Configuration follows a precedence hierarchy:
 * 1. Command line arguments (highest precedence)
 * 2. Configuration file (JSON format)
 * 3. Runtime set() calls
 * 4. Default values (lowest precedence)
 * 
 * The configuration system supports multiple data types for flexible specification:
 * - Basic types: int, int64_t, double, bool, std::string
 * - Collections: std::vector<int>, std::vector<double>, std::vector<std::string>
 * - Composite: ConfigValue variant for type-safe storage
 * 
 * Thread safety: Not thread-safe for concurrent modification. Configuration should
 * be set during initialization or when the brain is not running to avoid race conditions.
 * 
 * Error handling: Graceful handling of missing keys and type mismatches through
 * optional returns and default value fallbacks.
 * 
 * Example usage:
 * @code
 * Config config;
 * 
 * // Set configuration values
 * config.set("neuron_count", 1000u);
 * config.set("simulation_timestep", 0.001);
 * config.set("learning_rate", 0.01);
 * 
 * // Get configuration values with type safety
 * auto neuron_count = config.get<uint64_t>("neuron_count");
 * auto timestep = config.get<double>("simulation_timestep");
 * 
 * // Load from file or command line
 * config.loadFromFile("config.json");
 * config.loadFromArgs(argc, argv);
 * @endcode
 * 
 * @see Brain.hpp for brain configuration integration
 * @see Neuron.hpp for neuron-specific parameters
 * @see Experiment.hpp for experimental parameters
 */
class Config {
public:
    /**
     * @brief Default constructor creates an empty configuration
     * 
     * Creates a new configuration instance with no settings. The configuration will
     * have access to default values only. To load specific settings, use loadFromFile(),
     * loadFromArgs(), or direct set() calls.
     * 
     * @note This constructor is typically used when configuration is managed
     *       entirely through configuration files or command-line arguments.
     *       For programmatic configuration, consider using the default constructor
     *       followed by set() calls for all desired parameters.
     */
    Config();
    
    /**
     * @brief Destructor releases configuration resources
     * 
     * Cleans up any resources held by the configuration object. This includes
     * any loaded configuration data, file handles, and internal parsing state.
     * 
     * @note This destructor is noexcept to ensure safe cleanup during error conditions
     *       or program termination. Configuration objects should not throw exceptions
     *       during destruction.
     */
    ~Config();
    
    // Disable copying, enable moving for efficient resource management
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // -------------------------------------------------------------------------
    // CONFIGURATION LOADING AND PERSISTENCE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Load configuration from JSON file
     * 
     * Loads configuration parameters from a JSON file, merging with existing
     * configuration values according to the precedence hierarchy. File loading
     * is idempotent and can be called multiple times to update configuration.
     * 
     * @param filepath Path to JSON configuration file
     * @return true if file was successfully loaded and parsed, false otherwise
     * 
     * @note File loading respects configuration hierarchy and merges with existing values:
     *       - File values override default values
     *       - Later loadFromFile() calls override earlier ones
     *       - Command-line parsing takes highest precedence
     *       
     * @throws std::runtime_error for severe file I/O or parsing errors
     * @throws std::invalid_argument for malformed JSON or invalid parameter values
     * 
     * @see loadFromArgs() for command-line loading
     * @see saveToFile() for configuration saving
     * @see getOr() for default value fallback
     * 
     * @code
     * Config config;
     * if (!config.loadFromFile("brain_config.json")) {
     *     std::cerr << "Failed to load configuration" << std::endl;
     *     return 1;
     * }
     * @endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments to extract configuration parameters. Command line
     * arguments take highest precedence in the configuration hierarchy and can override
     * values from configuration files and defaults.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array (typically from main())
     * @return true if command line arguments were successfully parsed, false otherwise
     * 
     * @note Command line arguments use the format: --key value [--key value ...]
     *       Multiple occurrences of the same key use the last value provided.
     *       Valid types: integers, floating-point numbers, strings, boolean flags.
     *       
     * @see loadFromFile() for file loading
     * @see get() for value retrieval
     * @see saveToFile() for configuration saving
     * 
     * @code
     * int main(int argc, char* argv[]) {
     *     Config config;
     *     if (!config.loadFromArgs(argc, argv)) {
     *         std::cerr << "Invalid command line arguments" << std::endl;
     *         return 1;
     *     }
     *     
     *     // Configuration is now ready for brain initialization
     *     Brain brain(std::make_shared<Config>(config));
     * }
     * @endcode
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save current configuration to JSON file
     * 
     * Saves the current configuration to a JSON file. Only non-default values may be saved,
     * depending on implementation. File format is JSON for interoperability with other tools.
     * 
     * @param filepath Path where configuration will be saved
     * @return true if configuration was successfully saved, false otherwise
     * 
     * @note Saving behavior:
     *       - Only modified (non-default) values are typically saved
     *       - Can be used to persist configuration changes
     *       - File format is standard JSON for compatibility
     *       - Serialization preserves type information
     *       
     * @see loadFromFile() for loading configuration
     * @see clear() for resetting to defaults
     * @see getKeys() for available keys
     * 
     * @code
     * Config config;
     * config.set("brain_size", 1000u);
     * config.set("learning_rate", 0.01);
     * 
     * if (config.saveToFile("my_config.json")) {
     *     std::cout << "Configuration saved successfully" << std::endl;
     * } else {
     *     std::cerr << "Failed to save configuration" << std::endl;
     * }
     * @endcode
     */
    bool saveToFile(const std::string& filepath) const;
    
    // -------------------------------------------------------------------------
    // CONFIGURATION ACCESS AND MODIFICATION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Retrieve a configuration value with type safety
     * 
     * Retrieves a configuration value by key with compile-time type checking. The method
     * automatically handles type conversion and validation, ensuring that the returned
     * value matches the requested type.
     * 
     * @tparam T Type to retrieve (must be one of ConfigValue supported types)
     * @param key Configuration key to look up
     * @return std::optional<T> Value if key exists and matches type, nullopt otherwise
     * 
     * @note Type support includes:
     *       - Basic types: int, int64_t, double, bool, std::string
     *       - Collections: std::vector<int>, std::vector<double>, std::vector<std::string>
     *       
     * @note Automatic type conversion and validation:
     *       - JSON strings are converted to appropriate types
     *       - Type mismatches result in nullopt return
     *       - Missing keys result in nullopt return
     *       
     * @see getOr() for default value fallback
     * @see has() for key existence checking
     * @see set() for value setting
     * 
     * @code
     * Config config;
     * config.set("max_neurons", 1000u);
     * 
     * auto neurons = config.get<uint64_t>("max_neurons");
     * if (neurons.has_value()) {
     *     std::cout << "Max neurons: " << neurons.value() << std::endl;
     * } else {
     *     std::cout << "Max neurons not configured" << std::endl;
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Retrieve a configuration value with type safety and default fallback
     * 
     * Retrieves a configuration value by key with compile-time type checking and
     * automatic fallback to a default value if the key is not found or type mismatches.
     * This method is more efficient than get() + optional checking for default cases.
     * 
     * @tparam T Type to retrieve (must be one of ConfigValue supported types)
     * @param key Configuration key to look up
     * @param defaultValue Value to return if key doesn't exist or type mismatch
     * @return T Requested value or default if key not found
     * 
     * @note Template specialization enables compile-time type checking and
     *       efficient code generation. No runtime type checking is performed.
     *       
     * @see get() for optional return without defaults
     * @see set() for value setting
     * @see has() for key existence checking
     * 
     * @code
     * Config config;
     * 
     * // With default value
     * uint64_t neurons = config.getOr<uint64_t>("max_neurons", 1000u);
     * std::cout << "Max neurons: " << neurons << std::endl;
     * 
     * // String configuration
     * std::string mode = config.getOr<std::string>("mode", "default");
     * @endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value with explicit source
     * 
     * Sets a configuration value by key, storing the source of the configuration
     * for precedence resolution and debugging. Runtime sources override file and
     * command-line values, following the configuration hierarchy.
     * 
     * @param key Configuration key to set
     * @param value Configuration value
     * @param source Origin of this configuration (defaults to Runtime)
     * 
     * @note Source tracking helps with debugging and precedence resolution:
     *       - Default: Built-in system defaults
     *       - File: Loaded from configuration file
     *       - CommandLine: Set via command line arguments
     *       - Runtime: Set programmatically during execution
     *       
     * @note Runtime sources (Runtime, CommandLine) override file and default values.
     *       This allows dynamic configuration changes during execution.
     *       
     * @see ConfigSource enum for source types
     * @see get() for value retrieval
     * @see has() for key existence checking
     * @see remove() for value removal
     * 
     * @code
     * Config config;
     * 
     * // Set with different sources
     * config.set("learning_rate", 0.01, ConfigSource::Default);
     * config.set("neuron_count", 1000, ConfigSource::File);
     * config.set("debug_mode", true, ConfigSource::CommandLine);
     * config.set("temp_override", 42, ConfigSource::Runtime);
     * @endcode
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // ... Additional Config templates continue with similar documentation
};
```

## Phase 3: Implementation Tools and Scripts

### 3.1 Create Documentation Validation Script

```python
# scripts/validation/validate_documentation.py
#!/usr/bin/env python3

import os
import re
import sys
from pathlib import Path
from typing import List, Dict, Tuple

def check_doxygen_structure(filepath: Path) -> Tuple[bool, List[str]]:
    """Check if a header file has proper Doxygen documentation structure."""
    issues = []
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Check for class/struct documentation
    class_pattern = r'^\s*class\s+\w+\s*\{'
    struct_pattern = r'^\s*struct\s+\w+\s*\{'
    
    lines = content.split('\n')
    
    for i, line in enumerate(lines):
        if re.match(class_pattern, line):
            # Check for Doxygen comment before class
            j = i - 1
            while j >= 0 and (lines[j].strip() == '' or lines[j].strip().startswith('//')):
                j -= 1
            if j < 0 or not lines[j].strip().startswith('/**'):
                issues.append(f"Class '{re.search(class_pattern, line).group()}' at line {i+1}: Missing Doxygen documentation")
        
        elif re.match(struct_pattern, line):
            # Check for Doxygen comment before struct
            j = i - 1
            while j >= 0 and (lines[j].strip() == '' or lines[j].strip().startswith('//')):
                j -= 1
            if j < 0 or not lines[j].strip().startswith('/**'):
                issues.append(f"Struct '{re.search(struct_pattern, line).group()}' at line {i+1}: Missing Doxygen documentation")
        
        # Check for method documentation
        method_pattern = r'^\s*[a-zA-Z_]\w*\s+\w+\s*\([^)]*\)\s*(?:const)?\s*;'
        if re.match(method_pattern, line) and not line.strip().startswith('//'):
            # Check if it's in class/struct context and not a constructor/destructor
            j = i - 1
            doxygen_found = False
            while j >= 0:
                if lines[j].strip().startswith('/**'):
                    doxygen_found = True
                    break
                elif lines[j].strip().startswith('public:') or lines[j].strip().startswith('private:') or lines[j].strip().startswith('protected:'):
                    break
                j -= 1
            
            if not doxygen_found:
                method_name = re.search(method_pattern, line).group().split('(')[0].strip()
                issues.append(f"Method '{method_name}' at line {i+1}: Missing Doxygen documentation")
    
    return len(issues) == 0, issues

def generate_documentation_report(headers: List[Path]) -> Dict:
    """Generate a comprehensive documentation report."""
    report = {
        'total_files': len(headers),
        'documented_files': 0,
        'undocumented_files': [],
        'total_methods': 0,
        'documented_methods': 0,
        'coverage_percentage': 0.0
    }
    
    for header in headers:
        is_documented, issues = check_doxygen_structure(header)
        
        if is_documented:
            report['documented_files'] += 1
        else:
            report['undocumented_files'].append({
                'file': header,
                'issues': issues
            })
        
        # Count methods in the file
        with open(header, 'r') as f:
            content = f.read()
        
        # Simple method counting
        method_pattern = r'^\s*[a-zA-Z_]\w*\s+\w+\s*\([^)]*\)\s*(?:const)?\s*;'
        methods = re.findall(method_pattern, content, re.MULTILINE)
        report['total_methods'] += len(methods)
        
        # Count documented methods (simplified)
        documented_methods = len([m for m in methods if '/**' in content])
        report['documented_methods'] += documented_methods
    
    if report['total_methods'] > 0:
        report['coverage_percentage'] = (report['documented_methods'] / report['total_methods']) * 100
    
    return report

def main():
    parser = argparse.ArgumentParser(description='Validate NLM Doxygen documentation')
    parser.add_argument('--path', default='./src', help='Path to source directory')
    parser.add_argument('--format', choices=['json', 'text', 'summary'], 
                       default='text', help='Output format')
    parser.add_argument('--priority', choices=['high', 'medium', 'low'], 
                       default='high', help='Documentation priority level')
    
    args = parser.parse_args()
    
    # Find all header files
    base_path = Path(args.path)
    headers = list(base_path.glob("*.hpp"))
    
    print(f"Analyzing {len(headers)} header files in {base_path}")
    print()
    
    # Generate report
    report = generate_documentation_report(headers)
    
    if args.format == 'json':
        import json
        print(json.dumps(report, indent=2))
    elif args.format == 'summary':
        print(f"Documentation Coverage: {report['coverage_percentage']:.1f}%")
        print(f"Files: {report['documented_files']}/{report['total_files']}")
        print(f"Methods: {report['documented_methods']}/{report['total_methods']}")
    else:  # text
        print("=== NLM Doxygen Documentation Validation ===\n")
        print(f"Total Headers: {report['total_files']}")
        print(f"Documented Headers: {report['documented_files']}")
        print(f"Documentation Coverage: {report['coverage_percentage']:.1f}%\n")
        
        if report['undocumented_files']:
            print("=== Undocumented Headers ===\n")
            for item in report['undocumented_files']:
                print(f"File: {item['file']}")
                for issue in item['issues']:
                    print(f"  - {issue}")
                print()
    
    # Generate prioritized TODO list
    print("=== Prioritized TODO List ===\n")
    
    # High priority files (from our strategy)
    high_priority = [
        'Logger.hpp', 'Random.hpp', 'SimulationClock.hpp',
        'MotorSystem.hpp', 'Action.hpp', 'Observation.hpp'
    ]
    
    for priority_file in high_priority:
        if any(item['file'].name == priority_file for item in report['undocumented_files']):
            for item in report['undocumented_files']:
                if item['file'].name == priority_file:
                    print(f"HIGH PRIORITY: {priority_file}")
                    for issue in item['issues'][:3]:  # Show first 3 issues
                        print(f"  - {issue}")
                    print()
    
    return report

if __name__ == "__main__":
    sys.exit(main())
```

### 3.2 Create Template Generator Script

```python
# scripts/generate/templates.py
#!/usr/bin/env python3

import os
import re
from pathlib import Path

def generate_header_template(class_name: str, brief: str, detailed: str) -> str:
    """Generate a Doxygen template for a header file."""
    return f'''/**
 * @brief {brief}
 * 
 * {detailed}
 * 
 * Key features:
 * - Feature 1 description
 * - Feature 2 description
 * - Feature 3 description
 * 
 * Example usage:
 * @code
 * // Example code snippet
 * {class_name} obj;
 * @endcode
 */
class {class_name} {{
public:
    /**
     * @brief Constructor description
     * 
     * @param [parameter_name] [Description]
     */
    {class_name}([parameters]);
    
    /**
     * @brief Destructor description
     */
    ~{class_name}();
    
    // Public methods
    
    /**
     * @brief Method description
     * 
     * @param [parameter_name] [Description]
     * @return [Return type] [Description]
     */
    [return_type] [method_name]([parameters]);
}};
'''

def main():
    # Create templates directory
    templates_dir = Path("docs/templates")
    templates_dir.mkdir(parents=True, exist_ok=True)
    
    # Create class template
    class_template = templates_dir / "class.template"
    class_template.write_text(generate_header_template(
        "ClassName",
        "Brief description of the class",
        "Detailed explanation of the class purpose and functionality."
    ))
    
    # Create struct template
    struct_template = templates_dir / "struct.template"
    struct_template.write_text(generate_header_template(
        "StructName", 
        "Brief description of the struct",
        "Detailed explanation of the struct purpose and functionality."
    ))
    
    # Create function template
    func_template = templates_dir / "function.template"
    func_template.write_text('''/**
 * @brief Function description
 * 
 * Detailed explanation of the function purpose and behavior.
 * 
 * @param [parameter_name] [Description including units and constraints]
 * @param ... Additional parameters
 * 
 * @return [Return type] Description of return value
 * 
 * @throws [Exception] When this exception is thrown
 * @note Important implementation details
 * @see Related functions
 * 
 * @code
 * // Example usage
 * function_name(param1, param2);
 * @endcode
 */
[return_type] function_name([parameters]);
''')
    
    print("Generated documentation templates in docs/templates/")

if __name__ == "__main__":
    main()
```

## Phase 4: Priority Implementation Plan

### 4.1 High Priority Files (Week 1-2)

#### 4.1.1 Logger.hpp
```cpp
/**
 * @brief Central logging system for the NLM brain
 * 
 * The Logger class provides comprehensive logging infrastructure for the NLM brain system,
 * supporting multiple log levels, message formatting, and output destinations. It serves
 * as the primary debugging and monitoring tool for brain operation and development.
 * 
 * Logger features:
 * - Multiple log levels (Debug, Info, Warning, Error, Critical)
 * - Thread-safe concurrent logging operations
 * - Flexible output destinations (console, file, multiple sinks)
 * - Color-coded console output for improved readability
 * - Structured log entries with file, line, and function information
 * - Global logger instance for system-wide logging
 * 
 * The logging system is essential for:
 * - Debugging simulation issues and unexpected behavior
 * - Monitoring brain performance and system statistics
 * - Tracking neural activity patterns and learning processes
 * - Analyzing experimental results and performance metrics
 * - Providing feedback for development and testing
 * 
 * @note Logging is primarily used for debugging and monitoring. Production systems
 * should configure appropriate log levels to balance information and performance.
 * 
 * @see ConsoleLogger for console-specific logging
 * @see LogEntry for structured log data
 * @see LogLevel enum for available levels
 */
class Logger {
public:
    /**
     * @brief Default constructor creates a global logger instance
     * 
     * Creates the global logger instance with default configuration (Info level,
     * console output). The global instance is accessible through getGlobal().
     * 
     * @note This constructor is typically not called directly. Use the global
     * logger interface through Logger::getGlobal() for system-wide logging.
     */
    Logger();
    
    /**
     * @brief Destructor cleans up logger resources
     * 
     * Ensures proper cleanup of all registered logger sinks and associated resources.
     * The destructor is noexcept to prevent exceptions during shutdown.
     */
    ~Logger();
    
    // Disable copying, enable moving for efficient resource management
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    // -------------------------------------------------------------------------
    // LOGGER MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Add a logger sink for output
     * 
     * Registers a logger implementation to receive log messages. The sink will
     * receive all log messages at or above the configured minimum level. Multiple
     * sinks can be added for different output destinations or processing.
     * 
     * @param logger Shared pointer to logger implementation
     * 
     * @note Logger sinks are called synchronously during log operations.
     * Consider thread safety when implementing custom logger sinks.
     * 
     * @see ILogger for the logger interface
     * @see clearLoggers() for removing all sinks
     * @see getGlobal() for global logger access
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove all logger sinks
     * 
     * Clears all registered logger sinks and resets the logger to an empty state.
     * This is useful for reconfiguration or when the logger is no longer needed.
     * 
     * @note After calling clearLoggers(), no log messages will be output until
     * new logger sinks are added. This is useful for temporarily disabling logging.
     * 
     * @see addLogger() for adding sinks
     * @see setLevel() for controlling output level
     */
    void clearLoggers();
    
    // -------------------------------------------------------------------------
    // LOGGING OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Log a message at specified level
     * 
     * Outputs a log message with the specified severity level. The message includes
     * file, line, and function information when available for improved debugging.
     * Log messages are distributed to all registered logger sinks.
     * 
     * @param level Log severity level (Debug, Info, Warning, Error, Critical)
     * @param message Log message text
     * @param file Source file name (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     * 
     * @note File, line, and function parameters are automatically populated by
     * the NLM_LOG_STREAM macro. For manual logging, provide source information
     * for better debugging context.
     * 
     * @see debug() for Debug level convenience method
     * @see info() for Info level convenience method
     * @see warning() for Warning level convenience method
     * @see error() for Error level convenience method
     * @see critical() for Critical level convenience method
     * @see isEnabled() for level checking
     */
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods for common log levels
    /**
     * @brief Log a debug-level message
     * 
     * Outputs a debug-level log message. Debug messages are typically used for
     * detailed tracing of program flow and intermediate states. They are only
     * output when the logger's level is set to Debug or lower.
     * 
     * @param message Debug message text
     * @param file Source file name (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     * 
     * @note Use debug logging for:
     *       - Detailed tracing of algorithm execution
     *       - Intermediate state information
     *       - Fine-grained control flow tracking
     *       - Performance profiling data
     *       
     * @see NLM_LOG_DEBUG macro for automatic source location
     * @see log() for general logging
     * @see setLevel() for controlling output
     */
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an info-level message
     * 
     * Outputs an informational log message. Info messages provide general
     * information about program execution, system state, and important events.
     * They are output when the logger's level is Info or lower.
     * 
     * @param message Info message text
     * @param file Source file name (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     * 
     * @note Use info logging for:
     *       - System initialization and startup
     *       - Important configuration changes
     *       - Major state transitions
     *       - Regular operation status updates
     *       
     * @see NLM_LOG_INFO macro for automatic source location
     * @see log() for general logging
     */
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a warning-level message
     * 
     * Outputs a warning-level log message. Warning messages indicate potentially
     * problematic conditions that don't prevent program execution but may require
     * attention. They are output when the logger's level is Warning or lower.
     * 
     * @param message Warning message text
     * @param file Source file name (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     * 
     * @note Use warning logging for:
     *       - Potential issues that don't prevent execution
     *       - Deprecated API usage warnings
     *       - Configuration mismatches
     *       - Performance warnings
     *       
     * @see NLM_LOG_WARNING macro for automatic source location
     * @see log() for general logging
     */
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "");
    
    /**
     * @brief Log an error-level message
     * 
     * Outputs an error-level log message. Error messages indicate conditions that
     * prevent normal program execution or indicate serious problems. They are output
     * when the logger's level is Error or lower.
     * 
     * @param message Error message text
     * @param file Source file name (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     * 
     * @note Use error logging for:
     *       - File I/O errors
     *       - Memory allocation failures
     *       - Invalid parameter values
     *       - System resource exhaustion
     *       
     * @see NLM_LOG_ERROR macro for automatic source location
     * @see log() for general logging
     */
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    
    /**
     * @brief Log a critical-level message
     * 
     * Outputs a critical-level log message. Critical messages indicate severe
     * conditions that may prevent program continuation. They are output when the
     * logger's level is Critical or lower.
     * 
     * @param message Critical message text
     * @param file Source file name (defaults to empty string)
     * @param line Source line number (defaults to 0)
     * @param function Source function name (defaults to empty string)
     * 
     * @note Use critical logging for:
     *       - System crashes
     *       - Irrecoverable errors
     *       - Security violations
     *       - Fatal system failures
     *       
     * @see NLM_LOG_CRITICAL macro for automatic source location
     * @see log() for general logging
     */
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "");
    
    // -------------------------------------------------------------------------
    // LOGGING CONTROL
    // -------------------------------------------------------------------------
    
    /**
     * @brief Set minimum log level for all sinks
     * 
     * Configures the minimum severity level for log messages. Messages below this
     * level will not be output to any logger sink. This provides a global control
     * mechanism for filtering log output.
     * 
     * @param level Minimum severity level (Debug, Info, Warning, Error, Critical)
     * 
     * @note Level setting affects all registered logger sinks.
     * Different sinks may have their own independent level settings in addition
     * to this global minimum.
     * 
     * @see LogLevel enum for available levels
     * @see isEnabled() for level checking
     * @see debug() for Debug level logging
     * @see info() for Info level logging
     * @see warning() for Warning level logging
     * @see error() for Error level logging
     * @see critical() for Critical level logging
     */
    void setLevel(LogLevel level);
    
    /**
     * @brief Check if log level is enabled
     * 
     * Determines whether log messages at the specified level will be output.
     * This is useful for checking if a particular level of logging is active
     * before performing expensive log message formatting or operations.
     * 
     * @param level Log level to check
     * @return bool true if level is enabled (message will be output), false otherwise
     * 
     * @note This method is useful for:
     *       - Conditional logging to avoid string formatting overhead
     *       - Dynamic log level configuration
     *       - Implementing custom logging logic
     *       
     * @see setLevel() for level configuration
     * @see log() for actual logging
     */
    bool isEnabled(LogLevel level) const;
    
    // -------------------------------------------------------------------------
    // GLOBAL LOGGER INTERFACE
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the global logger instance
     * 
     * Returns a reference to the global logger instance, which is the primary
     * interface for system-wide logging. The global instance is shared across
     * all brain components and subsystems.
     * 
     * @return Logger& Reference to the global logger instance
     * 
     * @note This provides the main logging interface used throughout the NLM brain:
     *       - Brain components use this for debugging and monitoring
     *       - Experiments use this for recording results
     *       - Performance profiling uses this for metrics
     *       - Development tools use this for diagnostics
     *       
     * @see setGlobal() for replacing the global logger
     * @see addLogger() for adding output sinks
     * @see getGlobal() const for constant version
     */
    static Logger& getGlobal();
    
    /**
     * @brief Set the global logger instance
     * 
     * Replaces the current global logger instance with a new one. This is useful
     * for testing, custom logging configurations, or system-wide logger replacement.
     * 
     * @param logger Shared pointer to new global logger instance
     * 
     * @note After setting a new global logger, all components will use the
     *       new instance for logging operations. The previous global logger will
     *       no longer be used.
     *       
     * @see getGlobal() for getting the current global logger
     * @see addLogger() for adding sinks to the global logger
     */
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // -------------------------------------------------------------------------
    // SYSTEM OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Log the current status of the brain system
     * 
     * Generates a comprehensive status report including system statistics,
     * memory usage, and current operational metrics. This method is useful for
     * monitoring brain operation and debugging system issues.
     * 
     * @note This generates a detailed report including:
     *       - Total neuron and synapse counts
     *       - Active neuron statistics
     *       - Spike generation statistics
     *       - Memory usage information
     *       - Performance metrics
     *       
     * @see debug() for debug-level status messages
     * @see info() for informational status messages
     * @see NLM_LOG_INFO macro for automatic source location
     */
    void logStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation for PIMPL pattern
};
```

#### 4.1.2 Random.hpp
```cpp
/**
 * @brief Deterministic random number generator for the NLM brain
 * 
 * The RandomGenerator class provides reproducible stochastic computation for the
 * NLM brain system. It supports multiple probability distributions and ensures
 * deterministic results when using explicit seeds, which is crucial for
 * reproducible experiments and scientific computing.
 * 
 * Random generation features:
 * - Deterministic with explicit seeds for reproducible experiments
 * - Multiple probability distributions (uniform, normal, Bernoulli, Poisson)
 * - Template-based container operations (shuffle, pickRandom)
 * - Efficient byte-level random data generation
 * - Integration with standard C++ random engine for advanced usage
 * - Thread-safe operations for parallel processing
 * 
 * The generator is used throughout the NLM brain for:
 * - Initialization with random parameters
 * - Plasticity rule parameter sampling
 * - Behavioral decision making
 * - Experimental condition randomization
 * - Monte Carlo simulations
 * - Statistical analysis and validation
 * 
 * @note Determinism is crucial for scientific reproducibility:
 *       - Use explicit seeds for experimental control
 *       - Avoid time-based seeds for reproducible results
 *       - Document seed values in experimental records
 *       - Use the same seed for repeated experiments
 *       
 * @see Brain.hpp for random initialization integration
 * @see PlasticityRule.hpp for random parameter generation
 * @see Experiment.hpp for experimental randomization
 */
class RandomGenerator {
public:
    using result_type = uint32_t;
    
    /**
     * @brief Create random generator with explicit seed
     * 
     * Creates a new random number generator with the specified seed, ensuring
     * reproducible results. The seed value should be chosen carefully for
     * experimental control and reproducibility.
     * 
     * @param seed Random seed value (uint64_t)
     * 
     * @note Use explicit seeds for:
     *       - Reproducible experiments
     *       - Scientific validation
     *       - Debugging random-related issues
     *       - Testing and regression testing
     *       
     * @warning Do not use time-based seeds (like std::time(nullptr)) for
     *         scientific experiments where reproducibility is required.
     *         Use RandomGenerator() for non-deterministic generation.
     *       
     * @see RandomGenerator() for time-based seed (non-deterministic)
     * @see seed() for changing seed after creation
     * @see getSeed() for retrieving the current seed
     */
    explicit RandomGenerator(uint64_t seed);
    
    /**
     * @brief Create random generator with time-based seed (non-deterministic)
     * 
     * Creates a random number generator with a time-based seed, producing
     * different random sequences each time the program runs. Use this constructor
     * only when randomness (not reproducibility) is the goal.
     * 
     * @note Use time-based seeds for:
     *       - Production systems where variance is desired
     *       - User interaction randomization
     *       - Simulation exploration
     *       - Training and reinforcement learning
     *       
     * @warning This constructor is NOT suitable for:
     *         - Scientific experiments requiring reproducibility
     *         - Testing and validation
     *         - Debugging and analysis
     *         
     * @see RandomGenerator(uint64_t) for deterministic generation
     * @see seed() for changing seed after creation
     */
    RandomGenerator();
    
    /**
     * @brief Destructor cleans up random generator resources
     * 
     * Ensures proper cleanup of any resources held by the random generator.
     * This destructor is noexcept to prevent exceptions during cleanup.
     */
    ~RandomGenerator();
    
    // Disable copying, enable moving for efficient resource management
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    // -------------------------------------------------------------------------
    // SEED MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Set the random seed
     * 
     * Changes the random seed, affecting all subsequent random number generation.
     * This allows reusing the same random generator for different random sequences.
     * 
     * @param seed New random seed value (uint64_t)
     * 
     * @note Seed changes affect:
     *       - All subsequent random number generation
     *       - Any container operations that use randomization
     *       - Thread-local random state if applicable
     *       
     * @see getSeed() for retrieving current seed
     * @see RandomGenerator(uint64_t) for initial seeding
     */
    void seed(uint64_t seed);
    
    /**
     * @brief Get the current random seed
     * 
     * Returns the current seed value used by the random number generator.
     * This is useful for documentation, debugging, and reproducibility.
     * 
     * @return uint64_t Current seed value
     * 
     * @note The seed value ensures deterministic reproduction of random sequences.
     *       Same seed with same operations produces identical results.
     *       
     * @see seed() for seed modification
     * @see RandomGenerator(uint64_t) for constructor seeding
     */
    uint64_t getSeed() const;
    
    // -------------------------------------------------------------------------
    // UNIFORM DISTRIBUTIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate uniform random integer in specified range
     * 
     * Produces a uniformly distributed random integer between min and max (inclusive).
     * This is the fundamental operation for discrete random sampling.
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return uint32_t Random integer in [min, max]
     * 
     * @note Uniform distribution properties:
     *       - Each integer in [min, max] has equal probability
     *       - Expected value = (min + max) / 2
     *       - Variance = ((max - min + 1)^2 - 1) / 12
     *       
     * @see uniformInt(uint64_t, uint64_t) for 64-bit integers
     * @see uniformReal() for continuous uniform distribution
     * @see bernoulli() for boolean sampling
     */
    uint32_t uniformInt(uint32_t min, uint32_t max);
    
    /**
     * @brief Generate uniform random 64-bit integer in specified range
     * 
     * Produces a uniformly distributed random 64-bit integer between min and max (inclusive).
     * Useful for large-scale simulations and high-precision sampling.
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return uint64_t Random integer in [min, max]
     * 
     * @note Similar to uniformInt() but for 64-bit range.
     *       Use when working with large arrays or large population counts.
     *       
     * @see uniformInt(uint32_t, uint32_t) for 32-bit integers
     */
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    /**
     * @brief Generate uniform random real number in specified range
     * 
     * Produces a uniformly distributed random double-precision floating-point number
     * between min (inclusive) and max (exclusive). This is used for continuous
     * random sampling and probability-based operations.
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return double Random real in [min, max)
     * 
     * @note Continuous uniform distribution:
     *       - Range length = max - min
     *       - Probability density = 1 / (max - min)
     *       - Expected value = (min + max) / 2
     *       
     * @see uniformReal(float, float) for single precision
     * @see uniformInt() for integer sampling
     * @see normal() for normal distribution sampling
     */
    double uniformReal(double min, double max);
    
    /**
     * @brief Generate uniform random single-precision real number
     * 
     * Produces a uniformly distributed random single-precision floating-point number
     * between min (inclusive) and max (exclusive). More memory-efficient than double
     * precision for applications where precision requirements are lower.
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return float Random real in [min, max)
     * 
     * @note Use single precision when:
     *       - Memory bandwidth is a concern
     *       - Precision requirements are lower
     *       - SIMD vectorization benefits from single precision
     *       
     * @see uniformReal(double, double) for double precision
     */
    float uniformReal(float min, float max);
    
    // -------------------------------------------------------------------------
    // NORMAL DISTRIBUTION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate normally distributed random number
     * 
     * Produces a random number from a normal (Gaussian) distribution with specified
     * mean and standard deviation. Used for modeling natural variation and noise.
     * 
     * @param mean Distribution mean (center)
     * @param stddev Distribution standard deviation (spread)
     * @return double Random number from N(mean, stddev^2)
     * 
     * @note Normal distribution properties:
     *       - Approximately 68% of values in [mean-stddev, mean+stddev]
     *       - Approximately 95% of values in [mean-2*stddev, mean+2*stddev]
     *       - Approximately 99.7% of values in [mean-3*stddev, mean+3*stddev]
     *       
     * @see normal(float, float) for single precision
     * @see bernoulli() for binary approximation
     * @see exponential() for related exponential distribution
     */
    double normal(double mean, double stddev);
    
    /**
     * @brief Generate normally distributed single-precision random number
     * 
     * Produces a random number from a normal distribution using single-precision
     * floating-point arithmetic. More efficient for vectorized operations and
     * when precision requirements are lower.
     * 
     * @param mean Distribution mean (center)
     * @param stddev Distribution standard deviation (spread)
     * @return float Random number from N(mean, stddev^2)
     * 
     * @note Similar to normal(double, double) but uses single precision.
     *       May introduce additional rounding error but is faster for SIMD.
     */
    float normal(float mean, float stddev);
    
    // -------------------------------------------------------------------------
    // BERNOULLI DISTRIBUTION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate Bernoulli distributed random boolean
     * 
     * Produces a random boolean value where true occurs with probability p
     * and false with probability (1-p). Used for binary decision making and
     * probabilistic sampling.
     * 
     * @param p Probability of true (default: 0.5)
     * @return bool Random boolean where P(true) = p
     * 
     * @note Bernoulli distribution applications:
     *       - Binary decision making
     *       - Coin flipping simulations
     *       - Success/failure trials
     *       - Network activation/deactivation
     *       
     * @see uniformReal() for probability threshold generation
     * @see bernoulli(double) for parameterized version
     */
    bool bernoulli(double p = 0.5);
    
    // -------------------------------------------------------------------------
    // POISSON DISTRIBUTION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate Poisson-distributed random integer
     * 
     * Produces a random integer from a Poisson distribution with specified
     * rate parameter lambda. Models the number of events occurring in a fixed
     * time interval, useful for spike train generation and event counting.
     * 
     * @param lambda Rate parameter (expected number of events)
     * @return uint32_t Random integer from Poisson(lambda)
     * 
     * @note Poisson distribution properties:
     *       - Mean = lambda
     *       - Variance = lambda
     *       - P(k events) = (lambda^k * e^(-lambda)) / k!
     *       
     * @see exponential() for related exponential distribution
     * @see uniformInt() for integer range sampling
     */
    uint32_t poisson(double lambda);
    
    // -------------------------------------------------------------------------
    // EXPONENTIAL DISTRIBUTION
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate exponentially distributed random number
     * 
     * Produces a random number from an exponential distribution with specified
     * rate parameter lambda. Models the time between events in a Poisson process,
     * useful for synaptic delay modeling and inter-spike interval generation.
     * 
     * @param lambda Rate parameter (events per unit time)
     * @return double Random number from Exponential(lambda)
     * 
     * @note Exponential distribution properties:
     *       - Mean = 1/lambda
     *       - Variance = 1/lambda^2
     *       - Memoryless property
     *       
     * @see poisson() for related Poisson process
     * @see uniformReal() for uniform alternative
     */
    double exponential(double lambda);
    
    // -------------------------------------------------------------------------
    // BOOLEAN AND BIT OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Generate random boolean value
     * 
     * Produces a random boolean with 50% probability for true and false.
     * Equivalent to bernoulli(0.5) but more efficient.
     * 
     * @return bool Random boolean value
     * 
     * @note This is a convenience method for common binary random sampling.
     *       For asymmetric probabilities, use bernoulli(p) instead.
     *       
     * @see bernoulli(double) for parameterized probability
     * @see randomBit() for bit-level random sampling
     */
    bool randomBool();
    
    /**
     * @brief Generate random bit
     * 
     * Produces a random bit (0 or 1) with equal probability. Useful for
     * bitwise operations, random sampling, and low-level random generation.
     * 
     * @return uint32_t Random bit (0 or 1)
     * 
     * @note This method is optimized for bit-level random generation.
     *       For higher precision, consider uniformInt(0, 1) or randomBool().
     *       
     * @see randomBool() for boolean random sampling
     * @see fillBytes() for random byte generation
     */
    uint32_t randomBit();
    
    // -------------------------------------------------------------------------
    // CONTAINER OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Shuffle container elements randomly
     * 
     * Randomly permutes the elements of a container in place, implementing the
     * Fisher-Yates shuffle algorithm. Used for random sampling, randomization
     * tests, and creating random orderings.
     * 
     * @tparam Container Type of container (supports random access iterators)
     * @param container Container to be shuffled (modified in place)
     * 
     * @note Time complexity: O(n) where n is container size.
     *       All permutations are equally likely.
     *       
     * @see pickRandom() for selecting random elements
     * @see shuffle() for algorithm details
     * 
     * @code
     * std::vector<int> numbers = {1, 2, 3, 4, 5};
     * RandomGenerator rng(42);
     * rng.shuffle(numbers); // Now numbers in random order
     * @endcode
     */
    template<typename Container>
    void shuffle(Container& container);
    
    /**
     * @brief Pick random element from container
     * 
     * Selects a random element from a container with uniform probability.
     * The container must not be empty. Returns a const reference to avoid copying.
     * 
     * @tparam Container Type of container (supports size() and iterator)
     * @param container Container to sample from (must not be empty)
     * @return typename Container::const_reference Random element from container
     * 
     * @note Throws std::runtime_error if container is empty.
     *       Each element has equal probability of being selected.
     *       
     * @see shuffle() for randomization
     * @see pickRandomIndex() for index-based selection
     * @see empty() for checking container state
     * 
     * @code
     * std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
     * RandomGenerator rng(42);
     * auto random_name = rng.pickRandom(names); // Random name
     * @endcode
     */
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    /**
     * @brief Pick random index from 0 to n-1
     * 
     * Generates a random index in the range [0, n-1] where n is the specified
     * number of elements. Useful for random access into arrays, vectors, or
     * other sized collections.
     * 
     * @param n Number of elements (size of range)
     * @return uint64_t Random index in [0, n-1]
     * 
     * @note Throws std::invalid_argument if n <= 0.
     *       All indices have equal probability of being selected.
     *       
     * @see pickRandom() for element selection
     * @see uniformInt() for integer generation
     */
    uint64_t pickRandomIndex(uint64_t n);
    
    // -------------------------------------------------------------------------
    // MEMORY OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Fill buffer with random bytes
     * 
     * Fills a memory buffer with cryptographically secure random bytes using
     * the underlying random engine's bit generation capabilities. Useful for
     * random seed generation, initialization, and testing.
     * 
     * @param data Pointer to buffer to fill
     * @param size Size of buffer in bytes
     * 
     * @note This method is useful for:
     *       - Generating random seeds
     *       - Creating random initialization data
     *       - Testing random number generation
     *       
     * @see randomBit() for bit-level generation
     * @see engine() for direct engine access
     */
    void fillBytes(void* data, size_t size);
    
    // -------------------------------------------------------------------------
    // ENGINE ACCESS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get mutable reference to underlying random engine
     * 
     * Returns a reference to the underlying mt19937_64 random engine for
     * advanced users who need direct control over random number generation.
     * This allows using the engine with standard C++ algorithms and functions.
     * 
     * @return std::mt19937_64& Reference to the random engine
     * 
     * @note Use this method for:
     *       - Integration with standard C++ algorithms
     *       - Advanced random number generation techniques
     *       - Testing and debugging of random generation
     *       
     * @warning Direct engine access bypasses RandomGenerator's interface
     *         and may break the generator's state management.
     *         Use with caution.
     *       
     * @see engine() const for constant version
     * @see std::shuffle for algorithm integration
     */
    std::mt19937_64& engine();
    
    /**
     * @brief Get constant reference to underlying random engine
     * 
     * Returns a constant reference to the underlying random engine for read-only
     * access. This allows inspection of the engine's state without modification.
     * 
     * @return const std::mt19937_64& Constant reference to the random engine
     * 
     * @see engine() for mutable access
     */
    const std::mt19937_64& engine() const;
    
    // -------------------------------------------------------------------------
    // COMPATIBILITY
    // -------------------------------------------------------------------------
    
    /**
     * @brief Static minimum for mt19937_64 compatibility
     * 
     * Provides the minimum value that can be generated by the underlying
     * mt19937_64 random engine. This enables compatibility with standard
     * C++ random number generator interface requirements.
     * 
     * @return result_type Minimum possible random value
     */
    static constexpr result_type min() { return std::mt19937_64::min(); }
    
    /**
     * @brief Static maximum for mt19937_64 compatibility
     * 
     * Provides the maximum value that can be generated by the underlying
     * mt19937_64 random engine. This enables compatibility with standard
     * C++ random number generator interface requirements.
     * 
     * @return result_type Maximum possible random value
     */
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    /**
     * @brief Operator for compatibility with std functions
     * 
     * Provides operator() for compatibility with standard C++ random number
     * generator requirements. This allows RandomGenerator to be used with
     * std::uniform_int_distribution, std::normal_distribution, and other
     * standard algorithms.
     * 
     * @return result_type Random value from operator()
     */
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

// Template implementations
template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    std::shuffle(container.begin(), container.end(), engine());
}

template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (container.empty()) {
        throw std::runtime_error("Cannot pick from empty container");
    }
    auto it = container.begin();
    std::advance(it, pickRandomIndex(container.size()));
    return *it;
}
```

## 4.1 Create Documentation Templates Directory Structure

```bash
# Create documentation templates and utilities
mkdir -p docs/templates
mkdir -p scripts/validation
mkdir -p docs/output
mkdir -p docs/checklists

# Create core documentation templates
cat > docs/templates/class.template << 'EOF'
/**
 * @brief [Brief class description]
 * 
 * [Detailed class description]
 * 
 * Key features:
 * - Feature 1
 * - Feature 2
 * 
 * Example usage:
 * @code
 * [Example code]
 * @endcode
 */
class [ClassName] {
public:
    /**
     * @brief [Constructor description]
     * 
     * @param [param] [Description]
     */
    [ClassName]([parameters]);
    
    /**
     * @brief [Method description]
     * 
     * @param [param] [Description]
     * @return [Return type] [Description]
     */
    [ReturnType] [methodName]([parameters]);
};
EOF

cat > docs/templates/struct.template << 'EOF'
/**
 * @brief [Brief struct description]
 * 
 * [Detailed struct description]
 * 
 * Member variables:
 * - [Type] [member]: [Description]
 */
struct [StructName] {
    [Type] [member];  ///< [Description]
    // Additional members...
};
EOF

cat > docs/templates/function.template << 'EOF'
/**
 * @brief [Function description]
 * 
 * [Detailed description]
 * 
 * @param [param] [Description]
 * @return [Return type] [Description]
 * @throws [Exception] [When]
 * @note [Important details]
 * @see [Related functions]
 * 
 * @code
 * [Example usage]
 * @endcode
 */
[ReturnType] functionName([parameters]);
EOF
```

## 4.2 Create Documentation Validation Script

```python
# scripts/validation/validate_documentation.py
#!/usr/bin/env python3

import os
import re
import sys
import argparse
from pathlib import Path
from typing import List, Dict, Tuple, Set

class DocumentationValidator:
    def __init__(self, source_path: Path):
        self.source_path = source_path
        self.headers = list(source_path.glob("*.hpp"))
        self.required_patterns = {
            'class': r'^\s*class\s+\w+\s*\{',
            'struct': r'^\s*struct\s+\w+\s*\{',
            'method': r'^\s*[a-zA-Z_]\w*\s+\w+\s*\([^)]*\)\s*(?:const)?\s*;'
        }
        
    def find_doxygen_comments(self, filepath: Path) -> Dict[str, List[int]]:
        """Find all Doxygen comments in a file."""
        with open(filepath, 'r') as f:
            content = f.read()
        
        doxygen_positions = {
            'class': [],
            'struct': [],
            'method': []
        }
        
        lines = content.split('\n')
        
        for i, line in enumerate(lines):
            stripped = line.strip()
            
            # Check for class/struct documentation
            if stripped.startswith('/**'):
                # Look ahead for class/struct definition within reasonable distance
                for j in range(i + 1, min(i + 50, len(lines))):
                    if re.match(self.required_patterns['class'], lines[j].strip()):
                        doxygen_positions['class'].append(i)
                        break
                    elif re.match(self.required_patterns['struct'], lines[j].strip()):
                        doxygen_positions['struct'].append(i)
                        break
                    elif lines[j].strip().startswith('class ') or lines[j].strip().startswith('struct '):
                        break
            
            # Check for method documentation (simplified)
            if re.match(self.required_patterns['method'], stripped):
                # Look backward for Doxygen comment
                doxygen_found = False
                for j in range(i - 1, max(i - 30, -1), -1):
                    if lines[j].strip().startswith('/**'):
                        doxygen_positions['method'].append(i)
                        doxygen_found = True
                        break
                    elif lines[j].strip().startswith('public:') or lines[j].strip().startswith('private:') or lines[j].strip().startswith('protected:'):
                        break
        
        return doxygen_positions
    
    def validate_file(self, filepath: Path) -> Dict:
        """Validate a single header file."""
        result = {
            'file': str(filepath),
            'total_classes': 0,
            'total_structs': 0,
            'total_methods': 0,
            'documented_classes': 0,
            'documented_structs': 0,
            'documented_methods': 0,
            'issues': [],
            'missing_doxygen': []
        }
        
        with open(filepath, 'r') as f:
            content = f.read()
        
        # Count elements
        result['total_classes'] = len(re.findall(self.required_patterns['class'], content, re.MULTILINE))
        result['total_structs'] = len(re.findall(self.required_patterns['struct'], content, re.MULTILINE))
        
        # Count methods more accurately
        lines = content.split('\n')
        in_class_or_struct = False
        method_count = 0
        
        for line in lines:
            stripped = line.strip()
            
            if stripped.startswith('class ') or stripped.startswith('struct '):
                in_class_or_struct = True
            elif stripped.startswith('public:') or stripped.startswith('private:') or stripped.startswith('protected:'):
                in_class_or_struct = True
            elif stripped and not stripped.startswith(' ') and not stripped.startswith('\t'):
                in_class_or_struct = False
            
            if in_class_or_struct and re.match(self.required_patterns['method'], stripped):
                method_count += 1
        
        result['total_methods'] = method_count
        
        # Find documented elements
        doxygen_data = self.find_doxygen_comments(filepath)
        
        # Map elements to their line numbers for validation
        lines = content.split('\n')
        
        for i, line in enumerate(lines):
            stripped = line.strip()
            
            if re.match(self.required_patterns['class'], stripped):
                class_name = re.search(r'class\s+(\w+)', stripped).group(1)
                if i in doxygen_data['class']:
                    result['documented_classes'] += 1
                else:
                    result['missing_doxygen'].append(f"Class '{class_name}' at line {i + 1}")
            
            elif re.match(self.required_patterns['struct'], stripped):
                struct_name = re.search(r'struct\s+(\w+)', stripped).group(1)
                if i in doxygen_data['struct']:
                    result['documented_structs'] += 1
                else:
                    result['missing_doxygen'].append(f"Struct '{struct_name}' at line {i + 1}")
            
            elif re.match(self.required_patterns['method'], stripped) and i in doxygen_data['method']:
                method_match = re.search(r'([a-zA-Z_]\w*)\s+(\w+)\s*\(', stripped)
                if method_match:
                    method_name = method_match.group(2)
                    result['documented_methods'] += 1
        
        # Generate recommendations for missing documentation
        if result['missing_doxygen']:
            result['issues'].append(f"Missing Doxygen documentation for {len(result['missing_doxygen'])} elements")
        
        if result['documented_classes'] < result['total_classes']:
            missing_classes = result['total_classes'] - result['documented_classes']
            result['issues'].append(f"Class documentation incomplete: {missing_classes}/{result['total_classes']}")
        
        if result['documented_structs'] < result['total_structs']:
            missing_structs = result['total_structs'] - result['documented_structs']
            result['issues'].append(f"Struct documentation incomplete: {missing_structs}/{result['total_structs']}")
        
        if result['documented_methods'] < result['total_methods']:
            missing_methods = result['total_methods'] - result['documented_methods']
            result['issues'].append(f"Method documentation incomplete: {missing_methods}/{result['total_methods']}")
        
        return result
    
    def validate_all(self) -> Dict:
        """Validate all header files."""
        results = {
            'total_files': len(self.headers),
            'files_validated': 0,
            'overall_issues': [],
            'file_results': [],
            'summary': {
                'total_classes': 0,
                'total_structs': 0,
                'total_methods': 0,
                'documented_classes': 0,
                'documented_structs': 0,
                'documented_methods': 0,
                'coverage_percentage': 0.0
            }
        }
        
        for header in self.headers:
            file_result = self.validate_file(header)
            results['file_results'].append(file_result)
            results['files_validated'] += 1
            
            # Update summary
            summary = results['summary']
            summary['total_classes'] += file_result['total_classes']
            summary['total_structs'] += file_result['total_structs']
            summary['total_methods'] += file_result['total_methods']
            summary['documented_classes'] += file_result['documented_classes']
            summary['documented_structs'] += file_result['documented_structs']
            summary['documented_methods'] += file_result['documented_methods']
            
            if file_result['issues']:
                results['overall_issues'].append({
                    'file': header.name,
                    'issues': file_result['issues'],
                    'missing_doxygen': file_result['missing_doxygen']
                })
        
        # Calculate overall coverage
        total_elements = (summary['total_classes'] + summary['total_structs'] + 
                         summary['total_methods'])
        if total_elements > 0:
            documented_elements = (summary['documented_classes'] + 
                                 summary['documented_structs'] + 
                                 summary['documented_methods'])
            results['summary']['coverage_percentage'] = (
                (documented_elements / total_elements) * 100
            )
        
        return results

def main():
    parser = argparse.ArgumentParser(description='Validate NLM Doxygen documentation')
    parser.add_argument('--path', default='./src', help='Path to source directory')
    parser.add_argument('--format', choices=['json', 'text', 'summary'], 
                       default='text', help='Output format')
    parser.add_argument('--priority', choices=['high', 'medium', 'low'], 
                       default='high', help='Documentation priority level')
    parser.add_argument('--output', help='Output file path')
    
    args = parser.parse_args()
    
    source_path = Path(args.path)
    if not source_path.exists():
        print(f"Error: Source path '{args.path}' does not exist")
        sys.exit(1)
    
    validator = DocumentationValidator(source_path)
    results = validator.validate_all()
    
    # Output results
    if args.format == 'json':
        import json
        output_data = {
            'validation_results': results,
            'timestamp': datetime.now().isoformat()
        }
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(output_data, f, indent=2)
            print(f"Results written to {args.output}")
        else:
            print(json.dumps(output_data, indent=2))
    
    elif args.format == 'summary':
        print(f"=== NLM Documentation Validation Summary ===\n")
        print(f"Files Analyzed: {results['files_validated']}/{results['total_files']}")
        print(f"Class Documentation: {results['summary']['documented_classes']}/{results['summary']['total_classes']} ({results['summary']['coverage_percentage']:.1f}%)")
        print(f"Struct Documentation: {results['summary']['documented_structs']}/{results['summary']['total_structs']}")
        print(f"Method Documentation: {results['summary']['documented_methods']}/{results['summary']['total_methods']}")
        print(f"\nOverall Coverage: {results['summary']['coverage_percentage']:.1f}%")
        
        if results['overall_issues']:
            print(f"\n=== Issues Found ({len(results['overall_issues'])}) ===")
            for issue in results['overall_issues'][:10]:  # Limit output
                print(f"\nFile: {issue['file']}")
                for i, issue_text in enumerate(issue['issues'], 1):
                    print(f"  {i}. {issue_text}")
                if issue['missing_doxygen']:
                    print(f"  Missing Doxygen: {', '.join(issue['missing_doxygen'][:3])}")
        
        # Generate prioritized TODO list
        print(f"\n=== Priority Action Plan ===")
        
        # High priority files based on our strategy
        high_priority_files = [
            'Logger.hpp', 'Random.hpp', 'SimulationClock.hpp',
            'MotorSystem.hpp', 'Action.hpp', 'Observation.hpp',
            'SimpleWorld.hpp', 'VisualizationInterface.hpp'
        ]
        
        for file_result in results['file_results']:
            if any(priority_file in file_result['file'] for priority_file in high_priority_files):
                if file_result['issues']:
                    print(f"\n🔴 HIGH PRIORITY: {Path(file_result['file']).name}")
                    for issue in file_result['issues']:
                        print(f"  - {issue}")
        
    else:  # text format
        print(f"=== NLM Doxygen Documentation Validation ===\n")
        print(f"Files Analyzed: {results['files_validated']}/{results['total_files']}")
        print(f"Total Elements: {results['summary']['total_classes']} classes, "
              f"{results['summary']['total_structs']} structs, "
              f"{results['summary']['total_methods']} methods")
        print(f"Documented Elements: {results['summary']['documented_classes']} classes, "
              f"{results['summary']['documented_structs']} structs, "
              f"{results['summary']['documented_methods']} methods")
        print(f"\nOverall Documentation Coverage: {results['summary']['coverage_percentage']:.1f}%")
        
        if results['overall_issues']:
            print(f"\n=== Issues Found ({len(results['overall_issues'])}) ===")
            for issue in results['overall_issues'][:20]:
                print(f"\nFile: {issue['file']}")
                for i, issue_text in enumerate(issue['issues'], 1):
                    print(f"  {i}. {issue_text}")
                if issue['missing_doxygen']:
                    print(f"  Missing Doxygen: {', '.join(issue['missing_doxygen'])}")
    
    # Exit with appropriate code
    if results['summary']['coverage_percentage'] >= 90:
        print(f"\n✅ Documentation coverage meets target (>=90%)")
        sys.exit(0)
    else:
        print(f"\n❌ Documentation coverage below target (<90%)")
        sys.exit(1)

if __name__ == "__main__":
    import argparse
    import datetime
    main()
```

## 5. Create Automated Documentation Generation

### 5.1 Doxygen Configuration (Doxyfile)

```xml
<!DOCTYPE doxygen PUBLIC "-//Doxygen//DTD Doxygen 1.9.1//EN" "https://www.doxygen.org/doxygen-1.9.1.dtd">
<doxygenversion>1.9.1</doxygenversion>

PROJECT_NAME           = "NLM Documentation"
PROJECT_NUMBER         = "Phase 6"
OUTPUT_DIRECTORY       = docs/output
GENERATE_LATEX         = NO
GENERATE_RTF           = NO
GENERATE_HTML          = YES
GENERATE_XML           = YES
GENERATE_MD            = YES
GENERATE_MAN           = NO
GENERATE_TESTGEN       = NO

INPUT                  = src/
INPUT                  = docs/templates
EXCLUDE                = */build/*
                     */tests/*
                     */third_party/*

ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = YES
EXPAND_AS_DEFINED      = nlm

GENERATE_BRIEF_MEMBER_DESC = YES
BRIEF_MEMBER_DESC      = YES
SORT_BRIEF_DOCS        = YES
CLASS_DIAGRAMS         = YES
HIDE_UNPUBLIC_MEMBERS   = YES

ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_COLORSTYLE_HUE    = 220
HTML_COLORSTYLE_SAT    = 100
HTML_EXTRA_STYLESHEET  = style.css

SEARCHENGINE           = YES
SEARCHENGINE_URL       = https://www.google.com/search

GENERATE_TAGFILE       = docs/output/nlm.tag
QUIET                  = YES

USE_MDFILE_AS_MAINPAGE = README.md
```

### 5.2 Documentation Building Script

```bash
#!/bin/bash
# scripts/build_docs.sh

set -e

echo "Building NLM Documentation..."

echo "1. Running documentation validation..."
python3 scripts/validation/validate_documentation.py --format text --priority high

if [ $? -ne 0 ]; then
    echo "WARNING: Documentation coverage below target. Continuing with generation."
fi

echo "2. Generating Doxygen documentation..."
doxygen Doxyfile

if [ $? -eq 0 ]; then
    echo "✅ Documentation generated successfully in docs/output/"
    
    echo "3. Creating documentation summary..."
    python3 scripts/validation/generate_summary.py
    
    echo "✅ Documentation build complete."
else
    echo "❌ Documentation generation failed"
    exit 1
fi
```

### 5.3 Documentation Summary Generator

```python
# scripts/validation/generate_summary.py
#!/usr/bin/env python3

import os
import json
from pathlib import Path
from datetime import datetime

def generate_documentation_summary():
    """Generate a comprehensive documentation summary."""
    
    summary = {
        'generated_at': datetime.now().isoformat(),
        'project': 'NLM (熙然) - Artificial Developmental Brain',
        'phase': 'Phase 6: Final Integration',
        'documentation_status': {
            'overall_coverage': 'Below target',
            'files_analyzed': 0,
            'headers_documented': 0,
            'classes_documented': 0,
            'structs_documented': 0,
            'methods_documented': 0,
            'missing_elements': []
        },
        'prioritized_action_plan': [],
        'quality_checks': {
            'doxygen_syntax': True,
            'cross_references': True,
            'examples_present': False,
            'parameter_documentation': True,
            'return_documentation': True
        },
        'files': []
    }
    
    # Analyze generated documentation
    output_dir = Path("docs/output")
    if output_dir.exists():
        html_index = output_dir / "index.html"
        if html_index.exists():
            summary['html_documentation'] = str(html_index.absolute())
        
        xml_index = output_dir / "index.xml"
        if xml_index.exists():
            summary['xml_documentation'] = str(xml_index.absolute())
        
        tagfile = output_dir / "nlm.tag"
        if tagfile.exists():
            summary['tagfile'] = str(tagfile.absolute())
        
        # Extract statistics from Doxygen output
        compoundindex = output_dir / "compoundindex.html"
        if compoundindex.exists():
            with open(compoundindex, 'r') as f:
                content = f.read()
                # Extract statistics (simplified)
                if 'class' in content.lower():
                    summary['quality_checks']['classes_present'] = True
                if 'struct' in content.lower():
                    summary['quality_checks']['structs_present'] = True
                if 'function' in content.lower():
                    summary['quality_checks']['functions_present'] = True
    
    # Generate prioritized action plan
    high_priority = [
        {
            'file': 'Logger.hpp',
            'priority': 'HIGH',
            'action': 'Add comprehensive class and method documentation',
            'estimated_lines': 150,
            'effort': 'High - Complex multi-method class'
        },
        {
            'file': 'Random.hpp',
            'priority': 'HIGH',
            'action': 'Document all probability distribution methods',
            'estimated_lines': 100,
            'effort': 'Medium - Well-structured but needs details'
        },
        {
            'file': 'SimulationClock.hpp',
            'priority': 'HIGH',
            'action': 'Add time system documentation and examples',
            'estimated_lines': 80,
            'effort': 'Medium - Simple but important'
        }
    ]
    
    summary['prioritized_action_plan'] = high_priority
    
    # Generate recommendations
    recommendations = [
        "Use existing documentation in Neuron.hpp, Config.hpp, and Brain.hpp as templates",
        "Focus on public API methods first - private implementation can be documented later",
        "Include examples for complex APIs (container operations, distribution methods)",
        "Document thread safety and exception guarantees",
        "Add cross-references between related components"
    ]
    
    summary['recommendations'] = recommendations
    
    # Write summary to file
    summary_file = Path("docs/documentation_summary.json")
    with open(summary_file, 'w') as f:
        json.dump(summary, f, indent=2)
    
    print("Documentation summary generated:")
    print(f"  - Generated at: {summary['generated_at']}")
    print(f"  - HTML docs: {summary.get('html_documentation', 'Not available')}")
    print(f"  - Priority files: {len(summary['prioritized_action_plan'])}")
    print(f"  - Recommendations: {len(summary['recommendations'])}")
    
    return summary

if __name__ == "__main__":
    generate_documentation_summary()
```

## 6. Implementation Timeline

### 6.1 Week 1: Foundation
- [ ] Create documentation templates and validation tools
- [ ] Run initial documentation audit
- [ ] Establish documentation standards and templates
- [ ] Create Doxygen configuration

### 6.2 Week 2: Core Systems Documentation
- [ ] Document Logger.hpp (Phase 1 completion)
- [ ] Document Random.hpp (Phase 2 completion)
- [ ] Document SimulationClock.hpp (Phase 3 completion)
- [ ] Run validation and collect metrics

### 6.3 Week 3: Essential Interfaces
- [ ] Document MotorSystem.hpp
- [ ] Document Action.hpp
- [ ] Document Observation.hpp
- [ ] Document SimpleWorld.hpp

### 6.4 Week 4: Neural Processing
- [ ] Document Synapse.hpp
- [ ] Document NeuralPopulation.hpp
- [ ] Document NeuralRegion.hpp
- [ ] Document SpikeSystem.hpp

### 6.5 Week 5: Learning Systems
- [ ] Document STDP.hpp
- [ ] Document Hebbian.hpp
- [ ] Document StructuralPlasticity.hpp
- [ ] Document PlasticityRule.hpp

### 6.6 Week 6: Neuromodulation
- [ ] Document Neuromodulator.hpp
- [ ] Document Dopamine.hpp
- [ ] Document Curiosity.hpp
- [ ] Document Novelty.hpp
- [ ] Document PredictionError.hpp

### 6.7 Week 7-8: Advanced Systems
- [ ] Document remaining memory systems
- [ ] Document prediction and cognition systems
- [ ] Document experimental and performance systems
- [ ] Final validation and integration

## 7. Success Metrics

### 7.1 Documentation Coverage Targets
- **Overall Coverage**: 95% of public APIs documented
- **High Priority Files**: 100% (Logger.hpp, Random.hpp, SimulationClock.hpp)
- **Core Systems**: 90% coverage for all Tier 1 and Tier 2 files
- **Method Documentation**: 85% of methods documented
- **Class Documentation**: 95% of classes documented

### 7.2 Quality Metrics
- **Doxygen Compliance**: All generated documentation follows Doxygen standards
- **Cross-References**: 80% of internal and external references present
- **Examples**: 70% of complex APIs include usage examples
- **Parameter Documentation**: 95% of parameters documented
- **Return Documentation**: 90% of returns documented
- **Thread Safety**: All thread safety guarantees documented
- **Exception Documentation**: All exceptions documented

### 7.3 Process Metrics
- **Validation Runs**: Weekly documentation audits
- **Coverage Improvement**: Minimum 10% improvement per sprint
- **Documentation Quality**: Automated validation passes with <5% errors
- **Template Compliance**: 100% template adherence for new documentation

This comprehensive strategy provides a systematic, scalable approach to documenting the NLM project's public APIs while leveraging the excellent existing documentation standards established in Neuron.hpp, Config.hpp, and Brain.hpp. The phased implementation ensures manageable progress while maintaining high quality and consistency across the entire codebase.
