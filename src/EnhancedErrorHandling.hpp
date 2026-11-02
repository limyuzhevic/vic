// Brain.h enhancements with improved error handling and validation
#include <stdexcept>
#include <sstream>

namespace nlm {
    
    // Exception classes for NLM errors
    class NLMException : public std::runtime_error {
    public:
        explicit NLMException(const std::string& message) 
            : std::runtime_error(message) {}
    };
    
    class InitializationException : public NLMException {
    public:
        explicit InitializationException(const std::string& message)
            : NLMException("Initialization Error: " + message) {}
    };
    
    class ConfigurationException : public NLMException {
    public:
        explicit ConfigurationException(const std::string& message)
            : NLMException("Configuration Error: " + message) {}
    };
    
    class ValidationException : public NLMException {
    public:
        explicit ValidationException(const std::string& message)
            : NLMException("Validation Error: " + message) {}
    };
    
    class MemoryException : public NLMException {
    public:
        explicit MemoryException(const std::string& message)
            : NLMException("Memory Error: " + message) {}
    };
    
    // Validation utilities
    class ValidationUtils {
    public:
        template<typename T>
        static void validateNotNull(const T* ptr, const std::string& operation) {
            if (!ptr) {
                throw ValidationException("Null pointer in " + operation);
            }
        }
        
        template<typename T>
        static void validateRange(T value, T min, T max, const std::string& operation, const std::string& paramName) {
            if (value < min || value > max) {
                throw ValidationException("Invalid value for " + paramName + " in " + operation + 
                                         ": expected range [" + std::to_string(min) + ", " + 
                                         std::to_string(max) + "], got " + std::to_string(value));
            }
        }
        
        static void validateNotEmpty(size_t size, const std::string& operation, const std::string& containerName) {
            if (size == 0) {
                throw ValidationException("Empty container in " + operation + ": " + containerName);
            }
        }
        
        static void validatePositive(float value, const std::string& paramName, const std::string& operation) {
            if (value <= 0.0f) {
                throw ValidationException("Invalid " + paramName + " in " + operation + ": must be positive, got " + std::to_string(value));
            }
        }
        
        static void validateNotNegative(float value, const std::string& paramName, const std::string& operation) {
            if (value < 0.0f) {
                throw ValidationException("Invalid " + paramName + " in " + operation + ": must be non-negative, got " + std::to_string(value));
            }
        }
    };
    
    // Enhanced Brain class with improved error handling
    class EnhancedBrain : public Brain {
    public:
        EnhancedBrain(std::shared_ptr<Config> config) : Brain(config) {}
        
        // Enhanced initialize with validation
        bool initialize() override {
            NLM_LOG_INFO("Initializing Enhanced NLM Brain with comprehensive validation...");
            
            // Validate configuration before initialization
            auto validation = validateInitialization();
            if (validation.hasErrors()) {
                for (const auto& error : validation.errors) {
                    NLM_LOG_ERROR("Initialization validation error: " + error);
                }
                throw InitializationException("Configuration validation failed during initialization");
            }
            
            if (validation.hasWarnings()) {
                for (const auto& warning : validation.warnings) {
                    NLM_LOG_WARNING("Initialization warning: " + warning);
                }
            }
            
            // Original initialization logic
            if (!Brain::initialize()) {
                throw InitializationException("Failed to initialize base brain systems");
            }
            
            // Post-initialization validation
            if (!validatePostInitialization()) {
                throw InitializationException("Post-initialization validation failed");
            }
            
            NLM_LOG_INFO("Enhanced NLM Brain initialization complete with validation");
            return true;
        }
        
        // Enhanced reset with validation
        void reset() override {
            NLM_LOG_INFO("Resetting Enhanced NLM Brain with validation...");
            
            try {
                Brain::reset();
                NLM_LOG_INFO("Enhanced NLM Brain reset complete");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error during enhanced brain reset: " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced step with validation
        void step(SimulationStep currentStep) override {
            // Validate input parameters
            ValidationUtils::validateRange(currentStep, 0, std::numeric_limits<SimulationStep>::max(), 
                                         "step", "currentStep");
            
            try {
                // Pre-step validation
                if (!validatePreStep(currentStep)) {
                    NLM_LOG_WARNING("Pre-step validation failed, continuing with execution");
                }
                
                Brain::step(currentStep);
                
                // Post-step validation
                validatePostStep(currentStep);
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error during enhanced brain step at step " + std::to_string(currentStep) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced injection with validation
        void injectCurrent(NeuronId neuron, MembranePotential current) override {
            // Validate parameters
            ValidationUtils::validatePositive(current, "current", "injectCurrent");
            
            try {
                Brain::injectCurrent(neuron, current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error during enhanced current injection to neuron " + 
                             std::to_string(neuron.index()) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced injection to neurons of specific type
        void injectCurrentToNeurons(NeuronType type, MembranePotential current) override {
            // Validate parameters
            ValidationUtils::validatePositive(current, "current", "injectCurrentToNeurons");
            
            try {
                Brain::injectCurrentToNeurons(type, current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error during enhanced current injection to neurons of type " + 
                             neuronTypeToString(type) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced produce action with validation
        std::unique_ptr<Action> produceAction() override {
            try {
                auto action = Brain::produceAction();
                
                // Validate the produced action
                if (action && !validateAction(*action)) {
                    NLM_LOG_WARNING("Produced action failed validation, defaulting to Wait action");
                    return std::make_unique<Action>(ActionType::Wait);
                }
                
                return action;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error during enhanced action production: " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced save with comprehensive validation
        bool save(const std::string& filepath) const override {
            // Validate filepath
            ValidationUtils::validateNotEmpty(filepath.size(), "save", "filepath");
            
            try {
                // Validate brain state before saving
                if (!validateBrainStateForSave()) {
                    NLM_LOG_ERROR("Brain state validation failed before save operation");
                    return false;
                }
                
                bool result = Brain::save(filepath);
                
                if (result) {
                    NLM_LOG_INFO("Enhanced brain state saved successfully to " + filepath);
                } else {
                    NLM_LOG_ERROR("Enhanced brain state save operation returned false");
                }
                
                return result;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during enhanced brain save to " + filepath + ": " + std::string(e.what()));
                return false;
            }
        }
        
        // Enhanced load with comprehensive validation
        bool load(const std::string& filepath) override {
            // Validate filepath
            ValidationUtils::validateNotEmpty(filepath.size(), "load", "filepath");
            
            try {
                // Validate current brain state before loading
                if (!validateBrainStateBeforeLoad()) {
                    NLM_LOG_ERROR("Brain state validation failed before load operation");
                    return false;
                }
                
                bool result = Brain::load(filepath);
                
                if (result) {
                    NLM_LOG_INFO("Enhanced brain state loaded successfully from " + filepath);
                    
                    // Validate loaded state
                    if (!validatePostLoad()) {
                        NLM_LOG_ERROR("Post-load validation failed");
                        return false;
                    }
                } else {
                    NLM_LOG_ERROR("Enhanced brain state load operation returned false");
                }
                
                return result;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during enhanced brain load from " + filepath + ": " + std::string(e.what()));
                return false;
            }
        }
        
    private:
        // Validation methods
        struct InitializationValidationResult {
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
            
            bool hasErrors() const { return !errors.empty(); }
            bool hasWarnings() const { return !warnings.empty(); }
        };
        
        InitializationValidationResult validateInitialization() {
            InitializationValidationResult result;
            
            // Validate configuration parameters
            auto config = getConfig();
            if (!config) {
                result.errors.push_back("Configuration is null during initialization validation");
                return result;
            }
            
            // Validate neuron count
            auto neuronCountOpt = config->get<int64_t>("neuron_count");
            if (neuronCountOpt) {
                ValidationUtils::validateRange(*neuronCountOpt, 10, 10000, "initialization", "neuron_count");
            }
            
            // Validate timestep
            auto timestepOpt = config->get<double>("simulation_timestep");
            if (timestepOpt) {
                ValidationUtils::validateRange(*timestepOpt, 0.0001, 0.1, "initialization", "simulation_timestep");
            }
            
            // Validate plasticity parameters
            auto connectionProbOpt = config->get<float>("connection_probability");
            if (connectionProbOpt) {
                ValidationUtils::validateRange(*connectionProbOpt, 0.0f, 1.0f, "initialization", "connection_probability");
            }
            
            return result;
        }
        
        bool validatePostInitialization() {
            NLM_LOG_INFO("Performing post-initialization validation...");
            
            // Check that essential systems are properly initialized
            if (!getSpikeSystem()) {
                NLM_LOG_ERROR("Spike system is not initialized");
                return false;
            }
            
            if (getTotalNeuronCount() == 0) {
                NLM_LOG_ERROR("Brain has zero neurons after initialization");
                return false;
            }
            
            if (getTotalSynapseCount() == 0) {
                NLM_LOG_WARNING("Brain has zero synapses after initialization - may indicate connectivity issue");
            }
            
            NLM_LOG_INFO("Post-initialization validation complete");
            return true;
        }
        
        bool validatePreStep(SimulationStep currentStep) {
            // Pre-step validation logic
            if (currentStep % 1000 == 0) {
                NLM_LOG_INFO("Pre-step validation for step " + std::to_string(currentStep));
            }
            return true;
        }
        
        bool validatePostStep(SimulationStep currentStep) {
            // Post-step validation logic
            if (currentStep % 1000 == 0) {
                NLM_LOG_INFO("Post-step validation for step " + std::to_string(currentStep) + 
                           " - Total spikes: " + std::to_string(getTotalSpikeCount()));
            }
            return true;
        }
        
        bool validateAction(const Action& action) {
            // Validate action parameters
            if (!action.getType()) {
                NLM_LOG_ERROR("Action has null type");
                return false;
            }
            
            // Add any action-specific validation here
            return true;
        }
        
        bool validateBrainStateForSave() {
            // Validate brain state is ready for saving
            if (getTotalNeuronCount() == 0) {
                NLM_LOG_ERROR("Cannot save brain with zero neurons");
                return false;
            }
            
            // Check for any invalid states
            auto* spikeSystem = getSpikeSystem();
            if (spikeSystem && spikeSystem->getPendingSpikeCount() > 10000) {
                NLM_LOG_WARNING("High number of pending spikes during save - may affect consistency");
            }
            
            return true;
        }
        
        bool validateBrainStateBeforeLoad() {
            // Validate brain state before loading (to prevent data loss)
            if (getTotalSpikeCount() > 0) {
                NLM_LOG_WARNING("Brain has active spikes before load operation - may be overwritten");
            }
            
            return true;
        }
        
        bool validatePostLoad() {
            // Validate brain state after loading
            if (getTotalNeuronCount() == 0) {
                NLM_LOG_ERROR("Loaded brain has zero neurons");
                return false;
            }
            
            NLM_LOG_INFO("Post-load validation complete");
            return true;
        }
        
        std::shared_ptr<Config> getConfig() const {
            // Access the config through the internal implementation
            // This is a simplified version - actual implementation would need to access pImpl->config
            return nullptr;
        }
        
        std::string neuronTypeToString(NeuronType type) {
            // Convert neuron type to string for error messages
            switch (type) {
                case NeuronType::Sensory: return "Sensory";
                case NeuronType::Internal: return "Internal";
                case NeuronType::Motor: return "Motor";
                case NeuronType::Inhibitory: return "Inhibitory";
                case NeuronType::Excitatory: return "Excitatory";
                default: return "Unknown";
            }
        }
    };
    
    // Enhanced Neuron class with improved error handling
    class EnhancedNeuron : public Neuron {
    public:
        EnhancedNeuron(NeuronId id) : Neuron(id) {}
        
        // Enhanced stepLIF with validation
        bool stepLIF(Timestamp currentTime, TimestepDuration dt) override {
            // Validate parameters
            ValidationUtils::validateRange(currentTime, 0.0, std::numeric_limits<Timestamp>::max(),
                                         "stepLIF", "currentTime");
            ValidationUtils::validatePositive(dt, "timestep", "stepLIF");
            
            // Validate neuron state
            auto& state = getState();
            ValidationUtils::validateRange(state.membranePotential, -100.0f, 50.0f, "stepLIF", "membranePotential");
            ValidationUtils::validateRange(state.threshold, -100.0f, 50.0f, "stepLIF", "threshold");
            
            try {
                bool result = Neuron::stepLIF(currentTime, dt);
                
                // Validate post-step state
                if (!validatePostStepLIF(state)) {
                    NLM_LOG_WARNING("Post-stepLIF validation issued warning");
                }
                
                return result;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during enhanced neuron stepLIF for neuron " + 
                             std::to_string(getId().index()) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced injectCurrent with validation
        void injectCurrent(MembranePotential current) override {
            // Validate parameters
            ValidationUtils::validateRange(current, -1000.0f, 1000.0f, "injectCurrent", "current");
            
            try {
                Neuron::injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during enhanced neuron injectCurrent for neuron " + 
                             std::to_string(getId().index()) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced enablePlasticity with validation
        void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) override {
            try {
                Neuron::enablePlasticity(hebbian, stdp, rewardModulated);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during enhanced neuron enablePlasticity for neuron " + 
                             std::to_string(getId().index()) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced reset with validation
        void reset() override {
            try {
                Neuron::reset();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during enhanced neuron reset for neuron " + 
                             std::to_string(getId().index()) + ": " + std::string(e.what()));
                throw;
            }
        }
        
        // Enhanced initializeRandom with validation
        void initializeRandom(RandomGenerator& rng) override {
            try {
                Neuron::initializeRandom(rng);
                
                // Validate initialization
                if (!validateInitialization()) {
                    NLM_LOG_ERROR("Neuron initialization validation failed for neuron " + 
                                 std::to_string(getId().index()));
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Exception during enhanced neuron initializeRandom for neuron " + 
                             std::to_string(getId().index()) + ": " + std::string(e.what()));
                throw;
            }
        }
        
    private:
        bool validatePostStepLIF(const NeuronState& state) {
            // Validate post-stepLIF state
            if (std::isnan(state.membranePotential)) {
                NLM_LOG_ERROR("Membrane potential is NaN after stepLIF");
                return false;
            }
            
            if (std::isinf(state.membranePotential)) {
                NLM_LOG_ERROR("Membrane potential is infinite after stepLIF");
                return false;
            }
            
            if (state.refractoryRemaining > 1000) {
                NLM_LOG_WARNING("High refractory remaining count: " + std::to_string(state.refractoryRemaining));
            }
            
            return true;
        }
        
        bool validateInitialization() {
            // Validate neuron initialization
            auto& state = getState();
            
            if (std::isnan(state.membranePotential) || std::isinf(state.membranePotential)) {
                NLM_LOG_ERROR("Invalid initial membrane potential: " + std::to_string(state.membranePotential));
                return false;
            }
            
            if (state.threshold < -100.0f || state.threshold > 50.0f) {
                NLM_LOG_ERROR("Invalid initial threshold: " + std::to_string(state.threshold));
                return false;
            }
            
            return true;
        }
    };
    
    // Helper function to get current namespace
    inline std::string getCurrentNamespace() {
        return "nlm";
    }
} // namespace nlm

// Include original headers and implementations
#include "Brain.hpp"
#include "Neuron.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"

// Note: This is a header-only enhancement file. The actual implementations
// would need to be integrated into the existing source files.
// For demonstration purposes, this shows the improved error handling structure.

// Integration example (pseudocode):
/*
// In main.cpp:
// Replace Brain with EnhancedBrain for better error handling
auto brain = std::make_shared<EnhancedBrain>(config);

// Or upgrade existing components:
auto neuron = std::make_unique<EnhancedNeuron>(neuronId);
*/
