#ifndef NLM_PLUGIN_CORE_H
#define NLM_PLUGIN_CORE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Config.hpp"
#include "Logger.hpp"

namespace nlm {

// Forward declarations
class IPlugin;
class ISensorPlugin;
class INeuralNetworkPlugin;
class IWorldPlugin;

// Plugin system version
const std::string PLUGIN_SYSTEM_VERSION = "1.0.0";

// ============================================================================
// Plugin Base Interface
// ============================================================================

/**
 * @interface IPlugin
 * @brief Base interface for all NLM plugins
 * 
 * The IPlugin interface defines the contract that all plugins must implement
 * to be loaded and managed by the NLM plugin system.
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    /**
     * @brief Get the plugin name
     * 
     * @return Unique name identifying the plugin
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get the plugin version
     * 
     * @return Version string in semantic versioning format (major.minor.patch)
     */
    virtual std::string getVersion() const = 0;
    
    /**
     * @brief Get the plugin description
     * 
     * @return Human-readable description of the plugin's purpose
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief Initialize the plugin
     * 
     * This method is called when the plugin is enabled. The plugin should
     * set up any necessary resources and configuration.
     * 
     * @param config Global configuration for this plugin
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool initialize(const Config& config) = 0;
    
    /**
     * @brief Shutdown the plugin
     * 
     * This method is called when the plugin is being disabled or unloaded.
     * The plugin should clean up any resources and save any necessary state.
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Check if the plugin is enabled
     * 
     * @return true if the plugin is currently enabled
     */
    virtual bool isEnabled() const = 0;
    
    /**
     * @brief Enable or disable the plugin
     * 
     * @param enabled Whether to enable the plugin
     */
    virtual void setEnabled(bool enabled) = 0;
    
    /**
     * @brief Get plugin metadata
     * 
     * @return Map of metadata key-value pairs
     */
    virtual std::unordered_map<std::string, std::string> getMetadata() const {
        return {};
    }
    
    /**
     * @brief Get plugin type category
     * 
     * @return String identifying the plugin type category
     */
    virtual std::string getType() const {
        return "generic";
    }
};

// ============================================================================
// Sensor Plugin Interface
// ============================================================================

/**
 * @interface ISensorPlugin
 * @brief Interface for sensory input plugins
 * 
 * Sensor plugins provide custom sensory input modalities for the agent.
 * Examples include vision, touch, internal state, proprioception, and audio sensors.
 */
class ISensorPlugin : public virtual IPlugin {
public:
    virtual ~ISensorPlugin() = default;
    
    /**
     * @brief Sensor data structure
     */
    struct SensorData {
        std::vector<float> values;           //!< Sensor readings as floating-point values
        std::string name;                    //!< Human-readable name of the sensor
        double timestamp;                     //!< Simulation time when data was collected
        std::unordered_map<std::string, std::string> metadata; //!< Additional metadata
        bool isValid;                         //!< Whether the sensor data is valid
        
        SensorData() : values(), name(), timestamp(0.0), isValid(false) {}
    };
    
    /**
     * @brief Read sensor data
     * 
     * This method should be called each simulation step to get the latest
     * sensor readings.
     * 
     * @return SensorData containing the latest readings
     */
    virtual SensorData read() = 0;
    
    /**
     * @brief Get the expected size of sensor data
     * 
     * @return Number of values expected from this sensor
     */
    virtual size_t getExpectedSize() const = 0;
    
    /**
     * @brief Get the sensor type
     * 
     * @return String identifying the sensor type (e.g., "vision", "touch", "internal")
     */
    virtual std::string getSensorType() const = 0;
    
    /**
     * @brief Check if the sensor is active
     * 
     * @return true if the sensor is currently active and providing data
     */
    virtual bool isActive() const = 0;
    
    /**
     * @brief Enable or disable the sensor
     * 
     * @param active Whether to activate the sensor
     */
    virtual void setActive(bool active) = 0;
    
    /**
     * @brief Get sensor-specific configuration
     * 
     * @return Configuration specific to this sensor type
     */
    virtual std::unordered_map<std::string, std::string> getSensorConfig() const {
        return {};
    }
    
    /**
     * @brief Validate sensor data
     * 
     * @param data The sensor data to validate
     * @return true if the data is valid, false otherwise
     */
    virtual bool validateData(const SensorData& data) const {
        return !data.values.empty();
    }
};

// ============================================================================
// Neural Network Plugin Interface
// ============================================================================

/**
 * @interface INeuralNetworkPlugin
 * @brief Interface for neural network plugins
 * 
 * Neural network plugins provide custom implementations of neural computation
 * and learning. They can implement different neuron models, connectivity patterns,
 * and learning algorithms.
 */
class INeuralNetworkPlugin : public virtual IPlugin {
public:
    virtual ~INeuralNetworkPlugin() = default;
    
    /**
     * @brief Neural network neuron state structure
     */
    struct NeuronState {
        double membranePotential;          //!< Current membrane potential
        double restingPotential;           //!< Resting membrane potential
        double threshold;                   //!< Firing threshold
        double refractoryTime;              //!< Refractory period remaining
        double refractoryRemaining;         //!< Time until neuron can fire again
        std::vector<double> synapticWeights; //!< Synaptic weights to other neurons
        std::vector<double> eligibilityTraces; //!< Eligibility traces for learning
        double bias;                        //!< Neural bias
        double activation;                  //!< Current activation value
        
        NeuronState()
            : membranePotential(0.0), restingPotential(-70.0), threshold(-50.0),
              refractoryTime(0.0), refractoryRemaining(0.0), bias(0.0), activation(0.0) {}
    };
    
    /**
     * @brief Neural network synapse structure
     */
    struct Synapse {
        size_t sourceNeuron;                //!< Index of source neuron
        size_t targetNeuron;                //!< Index of target neuron
        double weight;                      //!< Synaptic weight
        double delay;                       //!< Transmission delay
        double eligibilityTrace;            //!< Current eligibility trace
        std::string connectionType;         //!< Type of connection (e.g., "excitatory", "inhibitory")
        
        Synapse() : sourceNeuron(0), targetNeuron(0), weight(0.0), delay(0.0), eligibilityTrace(0.0) {}
    };
    
    /**
     * @brief Neural network topology
     */
    struct NetworkTopology {
        std::vector<size_t> layerSizes;      //!< Number of neurons in each layer
        std::vector<std::string> layerTypes; //!< Type of each layer (e.g., "dense", "conv")
        std::vector<std::vector<size_t>> connections; //!< Connection matrix
        double learningRate;                //!< Learning rate for this network
        
        NetworkTopology() : learningRate(0.001) {}
    };
    
    /**
     * @brief Process neural network state
     * 
     * This method should process the neural network state and return updated
     * neuron states. It should implement the neural dynamics, spike generation,
     * and synaptic transmission.
     * 
     * @param inputStates Current neuron states
     * @return Updated neuron states after processing
     */
    virtual std::vector<NeuronState> process(const std::vector<NeuronState>& inputStates) = 0;
    
    /**
     * @brief Reset neural network to initial state
     */
    virtual void reset() = 0;
    
    /**
     * @brief Get the number of neurons in the network
     * 
     * @return Total number of neurons
     */
    virtual size_t getNeuronCount() const = 0;
    
    /**
     * @brief Get the network architecture
     * 
     * @return String describing the network architecture
     */
    virtual std::string getArchitecture() const = 0;
    
    /**
     * @brief Get the learning rate
     * 
     * @return Current learning rate
     */
    virtual double getLearningRate() const = 0;
    
    /**
     * @brief Set the learning rate
     * 
     * @param rate New learning rate
     */
    virtual void setLearningRate(double rate) = 0;
    
    /**
     * @brief Apply learning to the network
     * 
     * This method should implement the learning algorithm using available
     * eligibility traces and neuromodulatory signals.
     */
    virtual void learn() = 0;
    
    /**
     * @brief Get synaptic connections
     * 
     * @return Vector of all synaptic connections
     */
    virtual std::vector<Synapse> getSynapses() const {
        return {};
    }
    
    /**
     * @brief Add a new synapse
     * 
     * @param synapse The synapse to add
     */
    virtual void addSynapse(const Synapse& synapse) {
        // Default implementation - do nothing
    }
    
    /**
     * @brief Remove a synapse
     * 
     * @param source Source neuron index
     * @param target Target neuron index
     */
    virtual void removeSynapse(size_t source, size_t target) {
        // Default implementation - do nothing
    }
    
    /**
     * @brief Get network statistics
     * 
     * @return Map of network statistics
     */
    virtual std::unordered_map<std::string, double> getStatistics() const {
        return {};
    }
};

// ============================================================================
// World Plugin Interface
// ============================================================================

/**
 * @interface IWorldPlugin
 * @brief Interface for world simulation plugins
 * 
 * World plugins provide custom implementations of the simulated environment
 * and its dynamics. They handle agent physics, object interactions, and
 * environmental dynamics.
 */
class IWorldPlugin : public virtual IPlugin {
public:
    virtual ~IWorldPlugin() = default;
    
    /**
     * @brief World object type
     */
    enum class WorldObjectType {
        EMPTY = 0,
        RESOURCE = 1,
        HAZARD = 2,
        WALL = 3,
        MARKER = 4,
        AGENT = 5,
        SENSOR = 6
    };
    
    /**
     * @brief World object structure
     */
    struct WorldObject {
        double x;                         //!< X position
        double y;                         //!< Y position
        WorldObjectType type;             //!< Type of object
        double value;                     //!< Value for resources, penalty for hazards
        std::string name;                 //!< Human-readable name
        std::unordered_map<std::string, std::string> metadata; //!< Additional metadata
        
        WorldObject() : x(0.0), y(0.0), type(WorldObjectType::EMPTY), value(0.0) {}
        WorldObject(double x, double y, WorldObjectType type, double value = 0.0)
            : x(x), y(y), type(type), value(value) {}
    };
    
    /**
     * @brief Agent state structure
     */
    struct AgentState {
        double x;                         //!< X position
        double y;                         //!< Y position
        double orientation;               //!< Orientation in radians
        double velocityX;                 //!< X velocity
        double velocityY;                 //!< Y velocity
        double angularVelocity;           //!< Angular velocity
        double energy;                    //!< Energy level (0-1)
        double health;                    //!< Health level (0-1)
        double age;                       //!< Age in simulation steps
        bool isMoving;                    //!< Whether agent is moving
        bool isTurning;                   //!< Whether agent is turning
        double lastActionTime;            //!< Time since last action
        
        AgentState()
            : x(0.0), y(0.0), orientation(0.0), velocityX(0.0), velocityY(0.0),
              angularVelocity(0.0), energy(1.0), health(1.0), age(0.0),
              isMoving(false), isTurning(false), lastActionTime(0.0) {}
    };
    
    /**
     * @brief World configuration
     */
    struct WorldConfig {
        size_t width;                      //!< World width
        size_t height;                     //!< World height
        double gravityX;                   //!< Gravity in X direction
        double gravityY;                   //!< Gravity in Y direction
        double friction;                   //!< Friction coefficient
        bool enableCollisionDetection;     //!< Whether collision detection is enabled
        double timeStep;                   //!< Simulation time step
        std::string worldType;            //!< Type of world simulation
        
        WorldConfig()
            : width(100), height(100), gravityX(0.0), gravityY(-9.8),
              friction(0.5), enableCollisionDetection(true), timeStep(0.1),
              worldType("standard") {}
    };
    
    /**
     * @brief Update world simulation
     * 
     * This method should advance the world simulation by the specified
     * time step, updating all entities and their states.
     * 
     * @param timestep Time step to advance (in seconds)
     */
    virtual void update(double timestep) = 0;
    
    /**
     * @brief Get current agent state
     * 
     * @return Current agent state
     */
    virtual AgentState getAgentState() const = 0;
    
    /**
     * @brief Get sensory input for the agent
     * 
     * @return Sensory input vector
     */
    virtual std::vector<double> getSensoryInput() const = 0;
    
    /**
     * @brief Apply motor command to the world
     * 
     * @param action Motor command to apply
     * @param currentTime Current simulation time
     */
    virtual void applyAction(const std::string& action, double currentTime) = 0;
    
    /**
     * @brief Get current simulation time
     * 
     * @return Current simulation time
     */
    virtual double getSimulationTime() const = 0;
    
    /**
     * @brief Reset world to initial state
     */
    virtual void reset() = 0;
    
    /**
     * @brief Get world configuration
     * 
     * @return World configuration
     */
    virtual WorldConfig getConfig() const = 0;
    
    /**
     * @brief Set world configuration
     * 
     * @param config New world configuration
     */
    virtual void setConfig(const WorldConfig& config) = 0;
    
    /**
     * @brief Get world type
     * 
     * @return String identifying the world type
     */
    virtual std::string getWorldType() const = 0;
    
    /**
     * @brief Get all objects in the world
     * 
     * @return Vector of all world objects
     */
    virtual std::vector<WorldObject> getObjects() const {
        return {};
    }
    
    /**
     * @brief Add an object to the world
     * 
     * @param object Object to add
     */
    virtual void addObject(const WorldObject& object) {
        // Default implementation - do nothing
    }
    
    /**
     * @brief Remove an object from the world
     * 
     * @param x X position
     * @param y Y position
     */
    virtual void removeObject(double x, double y) {
        // Default implementation - do nothing
    }
    
    /**
     * @brief Check if position is valid
     * 
     * @param x X position
     * @param y Y position
     * @return true if position is valid (not occupied by wall, etc.)
     */
    virtual bool isValidPosition(double x, double y) const {
        return true;
    }
    
    /**
     * @brief Get world statistics
     * 
     * @return Map of world statistics
     */
    virtual std::unordered_map<std::string, double> getStatistics() const {
        return {};
    }
};

// Plugin version
static const std::string PLUGIN_VERSION = "1.0.0";

// Plugin type categories
static const std::string PLUGIN_TYPE_SENSOR = "sensor";
static const std::string PLUGIN_TYPE_NEURAL_NETWORK = "neural_network";
static const std::string PLUGIN_TYPE_WORLD = "world";
static const std::string PLUGIN_TYPE_GENERIC = "generic";

} // namespace nlm

#endif // NLM_PLUGIN_CORE_H