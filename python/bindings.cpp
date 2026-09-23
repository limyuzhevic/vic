#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>

// Include all NLM headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

// Configuration class with Pythonic interface
class PyConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    PyConfig() : config_(std::make_shared<Config>()) {}
    explicit PyConfig(const std::shared_ptr<Config>& config) : config_(config) {}
    
    void load_from_file(const std::string& filepath) {
        config_->loadFromFile(filepath);
    }
    
    void load_from_args(int argc, char** argv) {
        config_->loadFromArgs(argc, argv);
    }
    
    void save_to_file(const std::string& filepath) const {
        config_->saveToFile(filepath);
    }
    
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    void clear() {
        config_->clear();
    }
    
    std::string summary() const {
        return config_->summary();
    }
    
    std::string repr() const {
        return "<Config: " + config_->summary() + ">";
    }
    
    // Python property access
    const std::shared_ptr<Config>& get_config() const { return config_;n; }
    
    // Allow implicit conversion to shared_ptr for compatibility
    operator std::shared_ptr<Config>() const { return config_; }
};

// Brain class with Pythonic interface
class PyBrain {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    PyBrain(const std::shared_ptr<Config>& config) : brain_(std::make_shared<Brain>(config)) {}
    PyBrain(const std::shared_ptr<PyConfig>& config) : brain_(std::make_shared<Brain>(config->get_config())) {}
    
    void initialize() {
        brain_->initialize();
    }
    
    void step(SimulationStep currentStep) {
        brain_->step(currentStep);
    }
    
    void step_with_time(SimulationStep currentStep, Timestamp currentTime) {
        brain_->step(currentStep, currentTime);
    }
    
    void receive_sensory_input(const SensoryPercept& input) {
        brain_->receiveSensoryInput(input);
    }
    
    void inject_current(NeuronId neuron, float current) {
        brain_->injectCurrent(neuron, current);
    }
    
    void inject_current_to_neurons(NeuronType type, float current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    Action produce_action() {
        return brain_->produceAction();
    }
    
    void reset() {
        brain_->reset();
    }
    
    void save(const std::string& filepath) {
        brain_->save(filepath);
    }
    
    void load(const std::string& filepath) {
        brain_->load(filepath);
    }
    
    void add_region(const std::string& name = "") {
        brain_->addRegion(name);
    }
    
    NeuralRegion get_region(RegionId id) {
        return brain_->getRegion(id);
    }
    
    int get_region_count() const {
        return brain_->getRegionCount();
    }
    
    std::vector<RegionId> get_region_ids() const {
        return brain_->getRegionIds();
    }
    
    std::vector<NeuralRegion> get_regions() const {
        return brain_->getRegions();
    }
    
    int get_total_neuron_count() const {
        return brain_->getTotalNeuronCount();
    }
    
    int get_total_synapse_count() const {
        return brain_->getTotalSynapseCount();
    }
    
    int get_active_neuron_count() const {
        return brain_->getActiveNeuronCount();
    }
    
    int get_firing_neuron_count() const {
        return brain_->getFiringNeuronCount();
    }
    
    float get_average_firing_rate() const {
        return brain_->getAverageFiringRate();
    }
    
    float get_excitation_inhibition_ratio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    uint64_t get_total_spike_count() const {
        return brain_->getTotalSpikeCount();
    }
    
    DevelopmentalStage get_developmental_stage() const {
        return brain_->getDevelopmentalStage();
    }
    
    void set_developmental_stage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    std::string get_status() const {
        std::stringstream ss;
        brain_->logStatus();
        return ss.str();
    }
    
    const std::shared_ptr<Brain>& get_brain() const { return brain_; }
    
    std::string repr() const {
        return "<Brain: " + std::to_string(brain_->getTotalNeuronCount()) + " neurons>";
    }
};

// AgentBrain class with Pythonic interface
class PyAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    PyAgentBrain(const std::shared_ptr<Brain>& brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    PyAgentBrain(const std::shared_ptr<PyBrain>& brain) : agent_brain_(std::make_shared<AgentBrain>(brain->get_brain())) {}
    
    void initialize(const SimpleWorld& world) {
        agent_brain_->initialize(world);
    }
    
    int get_sensory_input_size() const {
        return agent_brain_->getSensoryInputSize();
    }
    
    int get_motor_output_size() const {
        return agent_brain_->getMotorOutputSize();
    }
    
    void process_sensory_input(const SensoryPercept& percept) {
        agent_brain_->processSensoryInput(percept);
    }
    
    Action decode_motor_command() {
        return agent_brain_->decodeMotorCommand();
    }
    
    void apply_reward_modulation(float reward, float predictedReward) {
        agent_brain_->applyRewardModulation(reward, predictedReward);
    }
    
    void update_development(SimulationStep timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    DevelopmentalStage get_developmental_stage() const {
        return agent_brain_->getDevelopmentalStage();
    }
    
    float get_neuromodulation_level() const {
        return agent_brain_->getNeuromodulationLevel();
    }
    
    float get_curiosity_level() const {
        return agent_brain_->getCuriosityLevel();
    }
    
    float get_novelty_level() const {
        return agent_brain_->getNoveltyLevel();
    }
    
    float get_prediction_error() const {
        return agent_brain_->getPredictionError();
    }
    
    void reset() {
        agent_brain_->reset();
    }
    
    const std::shared_ptr<Brain>& get_brain() const {
        return agent_brain_->getBrain();
    }
    
    std::string repr() const {
        return "<AgentBrain: connected to brain>";
    }
    
    // Enable/Disable flags
    void enable_reward_modulation(bool enable) {
        agent_brain_->enableRewardModulation(enable);
    }
    
    void enable_structural_plasticity(bool enable) {
        agent_brain_->enableStructuralPlasticity(enable);
    }
    
    void enable_development(bool enable) {
        agent_brain_->enableDevelopment(enable);
    }
    
    void enable_curiosity(bool enable) {
        agent_brain_->enableCuriosity(enable);
    }
    
    bool is_reward_modulation_enabled() const {
        return agent_brain_->isRewardModulationEnabled();
    }
    
    bool is_structural_plasticity_enabled() const {
        return agent_brain_->isStructuralPlasticityEnabled();
    }
    
    bool is_development_enabled() const {
        return agent_brain_->isDevelopmentEnabled();
    }
    
    bool is_curiosity_enabled() const {
        return agent_brain_->isCuriosityEnabled();
    }
};

// SimpleWorld class with Pythonic interface
class PySimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    PySimpleWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    void configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world_->reset();
    }
    
    void set_agent_start(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    void update(SimulationStep timestep) {
        world_->update(timestep);
    }
    
    ActionResult apply_motor_command(const Action& cmd, Timestamp currentTime) {
        return world_->applyMotorCommand(cmd, currentTime);
    }
    
    SensoryPercept get_sensory_percept() const {
        return world_->getSensoryPercept();
    }
    
    AgentBody get_agent_body() const {
        return world_->getAgentBody();
    }
    
    void add_object(const WorldObject& obj) {
        world_->addObject(obj);
    }
    
    void remove_object(float x, float y) {
        world_->removeObject(x, y);
    }
    
    bool is_valid_position(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    size_t get_width() const {
        return world_->getWidth();
    }
    
    size_t get_height() const {
        return world_->getHeight();
    }
    
    float get_max_energy() const {
        return world_->getMaxEnergy();
    }
    
    void set_max_energy(float e) {
        world_->setMaxEnergy(e);
    }
    
    float get_energy_decay_rate() const {
        return world_->getEnergyDecayRate();
    }
    
    void set_energy_decay_rate(float r) {
        world_->setEnergyDecayRate(r);
    }
    
    SimulationStep get_simulation_time() const {
        return world_->getSimulationTime();
    }
    
    void set_random_seed(uint32_t seed) {
        world_->setRandomSeed(seed);
    }
    
    uint32_t get_random_seed() const {
        return world_->getRandomSeed();
    }
    
    const std::shared_ptr<SimpleWorld>& get_world() const { return world_; }
    
    std::string repr() const {
        return "<SimpleWorld: " + std::to_string(world_->getWidth()) + "x" + 
               std::to_string(world_->getHeight()) + " m^2>";
    }
};

// Factory functions
std::shared_ptr<PyConfig> create_default_config() {
    return std::make_shared<PyConfig>();
}

std::shared_ptr<PyBrain> create_brain(const std::shared_ptr<Config>& config) {
    return std::make_shared<PyBrain>(config);
}

std::shared_ptr<PyBrain> create_brain_from_pyconfig(const std::shared_ptr<PyConfig>& config) {
    return std::make_shared<PyBrain>(config);
}

std::shared_ptr<PyAgentBrain> create_agent_brain(const std::shared_ptr<Brain>& brain) {
    return std::make_shared<PyAgentBrain>(brain);
}

std::shared_ptr<PyAgentBrain> create_agent_brain_from_pybrain(const std::shared_ptr<PyBrain>& brain) {
    return std::make_shared<PyAgentBrain>(brain->get_brain());
}

std::shared_ptr<PySimpleWorld> create_simple_world() {
    return std::make_shared<PySimpleWorld>();
}

// Python module initialization
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        This module provides a Pythonic API for creating and interacting with neural brains,
        agents, and simulation environments. All classes are designed with Python
        best practices in mind, including context managers, property access, and
        intuitive method names.
        
        Key Features:
        - Full integration with Python ecosystem
        - Type hints and documentation strings
        - Context managers for resource management
        - Convenience methods for common operations
        - Error handling with clear Python exceptions
        - Advanced features for experienced users
        
        Usage Example:
            import pynlm as nlm
            
            # Create a brain with default configuration
            config = nlm.create_config()
            brain = nlm.create_brain(config)
            
            # Create a simulation world
            world = nlm.create_world()
            world.configure(100, 100, 20, 20)
            
            # Create an agent
            agent = nlm.create_agent_brain(brain)
            agent.initialize(world)
            
            # Run simulation loop
            for step in range(1000):
                percept = world.get_sensory_percept()
                agent.process_sensory_input(percept)
                action = agent.decode_motor_command()
                world.update(step)
        )pbdoc";

    // Define constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .doc() = R"pbdoc(
            Neuron type enumeration for different kinds of neurons.
            
            - Excitatory: Neurons that promote firing in other neurons
            - Inhibitory: Neurons that suppress firing in other neurons  
            - Modulatory: Neurons that modulate neural excitability and plasticity
            - Sensory: Neurons that process sensory input
            - Motor: Neurons that control motor output
            - Internal: General-purpose internal neurons
        )pbdoc";

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .doc() = R"pbdoc(
            Synapse type enumeration for different kinds of synaptic connections.
            
            - Excitatory: Synaptic connections that increase postsynaptic firing
            - Inhibitory: Synaptic connections that decrease postsynaptic firing
            - Modulatory: Synaptic connections that affect neuromodulation
            - Electrical: Direct electrical coupling between neurons
            - GapJunction: Electrical synapse through gap junctions
        )pbdoc";

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .doc() = R"pbdoc(
            Developmental stage enumeration for brain development timeline.
            
            - Initial: Starting developmental stage with high plasticity
            - CriticalPeriod: Sensitive period for learning and adaptation
            - Maturation: Continued development and refinement
            - Adult: Mature developmental stage
            - Aging: Age-related decline and adaptation
        )pbdoc";

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .doc() = R"pbdoc(
            Neuron firing state enumeration for current neural activity state.
            
            - Resting: Neuron is not firing and below threshold
            - Active: Neuron is currently firing spikes
            - Refractory: Neuron is in refractory period after firing
            - Inhibited: Neuron is temporarily inhibited by external factors
        )pbdoc";

    // ActionType enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values()
        .doc() = R"pbdoc(
            Action type enumeration for agent motor commands.
            
            - MoveForward: Move forward in the environment
            - MoveBackward: Move backward in the environment
            - MoveLeft: Move left in the environment
            - MoveRight: Move right in the environment
            - TurnLeft: Rotate left (yaw)
            - TurnRight: Rotate right (yaw)
            - Look: Look forward
            - LookUp: Look up (pitch)
            - LookDown: Look down (pitch)
            - Interact: Interact with environment object
            - Eat: Consume food resource
            - Drink: Consume drink resource
            - Rest: Rest and recover energy
            - Wait: Do nothing
            - Custom: Custom action with parameters
        )pbdoc";

    // MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .doc() = R"pbdoc(
            Low-level motor command enumeration for direct action execution.
            
            - MoveForward: Move forward
            - MoveBackward: Move backward
            - TurnLeft: Rotate left
            - TurnRight: Rotate right
            - LookLeft: Look left
            - LookRight: Look right
            - Interact: Interact with environment
            - Wait: Do nothing
        )pbdoc";

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .doc() = R"pbdoc(
            World object type enumeration for environment objects.
            
            - Empty: No object (passable)
            - Resource: Consumable resource (food, water)
            - Hazard: Dangerous object (poison, obstacle)
            - Wall: Solid barrier (impassable)
            - Marker: Location marker for navigation
        )pbdoc";

    // NeuronId class
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .doc() = R"pbdoc(
            Unique identifier for a neuron.
            
            Attributes:
                value (uint64_t): Internal numeric identifier
            
            Methods:
                index(): Returns the internal index
                __eq__(): Equality comparison
                __ne__(): Inequality comparison
                __hash__(): Hash for use in sets and dictionaries
                __repr__(): String representation
                __str__(): String representation
        )pbdoc";

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        })
        .doc() = R"pbdoc(
            Unique identifier for a synapse.
            
            Attributes:
                value (uint64_t): Internal numeric identifier
            
            Methods:
                index(): Returns the internal index
                __eq__(): Equality comparison
                __ne__(): Inequality comparison
                __hash__(): Hash for use in sets and dictionaries
                __repr__(): String representation
                __str__(): String representation
        )pbdoc";

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        })
        .doc() = R"pbdoc(
            Unique identifier for a brain region.
            
            Attributes:
                value (uint64_t): Internal numeric identifier
            
            Methods:
                index(): Returns the internal index
                __eq__(): Equality comparison
                __ne__(): Inequality comparison
                __hash__(): Hash for use in sets and dictionaries
                __repr__(): String representation
                __str__(): String representation
        )pbdoc";

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .doc() = R"pbdoc(
            Unique identifier for a neuron population.
            
            Attributes:
                value (uint64_t): Internal numeric identifier
            
            Methods:
                index(): Returns the internal index
                __eq__(): Equality comparison
                __ne__(): Inequality comparison
        )pbdoc";

    // Helper functions for string conversion
    m.def("neuron_type_to_string", &nlm::python::neuron_type_to_string,
          py::arg("neuron_type"), R"pbdoc(Convert NeuronType enum to string)pbdoc");
    
    m.def("synapse_type_to_string", &nlm::python::synapse_type_to_string,
          py::arg("synapse_type"), R"pbdoc(Convert SynapseType enum to string)pbdoc");
    
    m.def("action_type_to_string", &nlm::python::action_type_to_string,
          py::arg("action_type"), R"pbdoc(Convert ActionType enum to string)pbdoc");
    
    m.def("world_object_type_to_string", &nlm::python::world_object_type_to_string,
          py::arg("world_object_type"), R"pbdoc(Convert WorldObjectType enum to string)pbdoc");
    
    m.def("string_to_neuron_type", &nlm::python::string_to_neuron_type,
          py::arg("str"), R"pbdoc(Convert string to NeuronType)pbdoc");
    
    m.def("string_to_synapse_type", &nlm::python::string_to_synapse_type,
          py::arg("str"), R"pbdoc(Convert string to SynapseType)pbdoc");
    
    m.def("string_to_action_type", &nlm::python::string_to_action_type,
          py::arg("str"), R"pbdoc(Convert string to ActionType)pbdoc");
    
    m.def("string_to_world_object_type", &nlm::python::string_to_world_object_type,
          py::arg("str"), R"pbdoc(Convert string to WorldObjectType)pbdoc");

    // Config class
    py::class_<PyConfig>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &PyConfig::load_from_file, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](PyConfig& self, int argc, char** argv) {
            return self.load_from_args(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &PyConfig::save_to_file, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &PyConfig::get_keys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::summary,
             "Get a summary string of the configuration")
        .def("__repr__", &PyConfig::repr,
             "String representation of the configuration")
        .def("__str__", &PyConfig::summary,
             "String representation of the configuration")
        .doc() = R"pbdoc(
            Configuration class for NLM system with Pythonic interface.
            
            Methods:
                load_from_file(filepath): Load configuration from JSON file
                load_from_args(argc, argv): Load configuration from command line arguments
                save_to_file(filepath): Save configuration to JSON file
                has(key): Check if configuration key exists
                get_keys(): Get all configuration keys
                clear(): Clear all configuration entries
                summary(): Get configuration summary
                __repr__(): String representation
                __str__(): String representation
        )pbdoc";

    // Create default config factory function
    m.def("create_config", &create_default_config,
          R"pbdoc(Create a default configuration)pbdoc");

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .doc() = R"pbdoc(
            Base class for sensory input.
            
            Attributes:
                type: Type of sensory input (Vision, Audio, etc.)
                dimensions: Dimensionality of the input data
                timestamp: Time when input was received
        )pbdoc";

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .doc() = R"pbdoc(
            Vision sensory input with 2D image data.
            
            Attributes:
                width: Image width in pixels
                height: Image height in pixels
                channels: Number of color channels (typically 3 for RGB)
                type: "Vision"
                data: Raw pixel data as vector of floats
        )pbdoc";

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .doc() = R"pbdoc(
            Audio sensory input with sound sample data.
            
            Attributes:
                sample_rate: Sampling rate in Hz
                num_samples: Number of audio samples
                type: "Audio"
                data: Raw audio data as vector of floats
        )pbdoc";

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals)
        .doc() = R"pbdoc(
            Internal signals sensory input for neural state information.
            
            Methods:
                add_signal(value): Add a signal value
                clear_signals(): Clear all signals
        )pbdoc";

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        })
        .doc() = R"pbdoc(
            Action representation for motor output.
            
            Attributes:
                type: Action type (ActionType enum)
                parameters: Action-specific parameters
            
            Methods:
                get_type(): Get action type
                set_type(type): Set action type
                get_parameters(): Get action parameters
                set_parameters(params): Set action parameters
                get_name(): Get human-readable action name
                clone(): Create a copy of this action
                __repr__(): String representation
        )pbdoc";

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeStr = world_object_type_to_string(obj.type);
            return "<WorldObject: " + typeStr + " at (" + 
                   std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        })
        .doc() = R"pbdoc(
            World object representation.
            
            Attributes:
                x: X position in world coordinates
                y: Y position in world coordinates
                type: Object type (WorldObjectType enum)
                value: Object-specific value (e.g., resource quantity)
                radius: Physical radius for collision detection
                active: Whether object is currently active
        )pbdoc";

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + ", " + 
                   std::to_string(body.y) + "), " + std::to_string(body.age) + "s>";
        })
        .doc() = R"pbdoc(
            Agent body state representing agent's physical state in the world.
            
            Attributes:
                x: X position in world coordinates
                y: Y position in world coordinates
                orientation: Heading angle in radians
                velocityX: X velocity component
                velocityY: Y velocity component
                angularVelocity: Angular velocity (rad/s)
                energy: Current energy level
                health: Current health level
                age: Age in simulation timesteps
                isMoving: Whether agent is currently moving
                isTurning: Whether agent is currently turning
                lastActionTime: Time since last action
        )pbdoc";

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            std::string status = result.success ? "SUCCESS" : "FAILED";
            return "<ActionResult: reward(" + std::to_string(result.reward) + ") " + status + ">";
        })
        .doc() = R"pbdoc(
            Action result from applying an action in the world.
            
            Attributes:
                reward: Reward received from the action
                success: Whether the action was successful
                message: Human-readable message about the result
        )pbdoc";

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision(" + std::to_string(percept.getVisionWidth()) + "x" +
                   std::to_string(percept.getVisionHeight()) + ")>";
        })
        .doc() = R"pbdoc(
            Sensory percept data representing current sensory input.
            
            Attributes:
                vision: Vision sensory data (2D image)
                touch: Touch/proprioception data
                internal: Internal neural signals
                audio: Audio sensory data
                timestamp: Time when percept was generated
        )pbdoc";

    // SimpleWorld class
    py::class_<PySimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the simulation world")
        .def("reset", &PySimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &PySimpleWorld::set_agent_start, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &PySimpleWorld::update, py::arg("timestep"),
             "Update world state for one timestep")
        .def("apply_motor_command", &PySimpleWorld::apply_motor_command,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world and get result")
        .def("get_sensory_percept", &PySimpleWorld::get_sensory_percept,
             "Get current sensory percept from world")
        .def("get_agent_body", &PySimpleWorld::get_agent_body,
             "Get current agent body state")
        .def("add_object", &PySimpleWorld::add_object, py::arg("obj"),
             "Add world object")
        .def("remove_object", &PySimpleWorld::remove_object, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("is_valid_position", &PySimpleWorld::is_valid_position, py::arg("x"), py::arg("y"),
             "Check if position is valid (not in obstacle)")
        .def("get_width", &PySimpleWorld::get_width,
             "Get world width")
        .def("get_height", &PySimpleWorld::get_height,
             "Get world height")
        .def("get_max_energy", &PySimpleWorld::get_max_energy,
             "Get maximum agent energy")
        .def("set_max_energy", &PySimpleWorld::set_max_energy, py::arg("e"),
             "Set maximum agent energy")
        .def("get_energy_decay_rate", &PySimpleWorld::get_energy_decay_rate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &PySimpleWorld::set_energy_decay_rate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &PySimpleWorld::get_simulation_time,
             "Get current simulation time")
        .def("set_random_seed", &PySimpleWorld::set_random_seed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("get_random_seed", &PySimpleWorld::get_random_seed,
             "Get random seed")
        .def("__repr__", &PySimpleWorld::repr,
             "String representation of the world")
        .doc() = R"pbdoc(
            Simple 2D world for NLM simulation with Pythonic interface.
            
            Methods:
                configure(width, height, visionWidth, visionHeight): Set world dimensions
                reset(): Reset world to initial state
                set_agent_start(x, y): Set agent starting position
                update(timestep): Update world state
                apply_motor_command(cmd, currentTime): Apply action and get result
                get_sensory_percept(): Get current sensory input
                get_agent_body(): Get agent physical state
                add_object(obj): Add world object
                remove_object(x, y): Remove object at position
                is_valid_position(x, y): Check if position is valid
                get_width(): Get world width
                get_height(): Get world height
                get_max_energy(): Get maximum energy
                set_max_energy(e): Set maximum energy
                get_energy_decay_rate(): Get energy decay rate
                set_energy_decay_rate(r): Set energy decay rate
                get_simulation_time(): Get simulation time
                set_random_seed(seed): Set random seed
                get_random_seed(): Get random seed
                __repr__(): String representation
        )pbdoc";

    // Create simple world factory function
    m.def("create_world", &create_simple_world,
          R"pbdoc(Create a new simple world)pbdoc");

    // Brain class
    py::class_<PyBrain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
        .def(py::init<std::shared_ptr<PyConfig>>(), py::arg("config"),
             "Initialize brain with Pythonic configuration")
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<SimulationStep>(&PyBrain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&PyBrain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &PyBrain::receive_sensory_input,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &PyBrain::inject_current,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &PyBrain::inject_current_to_neurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produce_action", &PyBrain::produce_action,
             "Produce motor action based on neural activity")
        .def("reset", &PyBrain::reset,
             "Reset brain state")
        .def("save", &PyBrain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &PyBrain::load, py::arg("filepath"), "Load brain state from file")
        .def("add_region", &PyBrain::add_region, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &PyBrain::get_region, py::arg("id"),
             "Get a region by ID")
        .def("get_region_count", &PyBrain::get_region_count,
             "Get the number of regions")
        .def("get_region_ids", &PyBrain::get_region_ids,
             "Get all region IDs")
        .def("get_regions", &PyBrain::get_regions,
             "Get all regions")
        .def("get_total_neuron_count", &PyBrain::get_total_neuron_count,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &PyBrain::get_total_synapse_count,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &PyBrain::get_active_neuron_count,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &PyBrain::get_firing_neuron_count,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &PyBrain::get_average_firing_rate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &PyBrain::get_excitation_inhibition_ratio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &PyBrain::get_total_spike_count,
             "Get total spike count")
        .def("get_developmental_stage", &PyBrain::get_developmental_stage,
             "Get current developmental stage")
        .def("set_developmental_stage", &PyBrain::set_developmental_stage,
             py::arg("stage"), "Set developmental stage")
        .def("get_config", &PyBrain::get_config,
             "Get the configuration")
        .def("log_status", &PyBrain::get_status,
             "Log brain status")
        .def("__repr__", &PyBrain::repr,
             "String representation of the brain")
        .def("__str__", &PyBrain::repr,
             "String representation of the brain")
        .doc() = R"pbdoc(
            Central neural simulation brain class with Pythonic interface.
            
            Methods:
                __init__(config): Initialize with configuration
                initialize(): Initialize the brain
                step(currentStep): Perform simulation step
                step(currentStep, currentTime): Perform step with timestamp
                receive_sensory_input(input): Inject sensory input
                inject_current(neuron, current): Inject current into neuron
                inject_current_to_neurons(type, current): Inject to all neurons of type
                produce_action(): Generate motor action
                reset(): Reset brain state
                save(filepath): Save brain state to file
                load(filepath): Load brain state from file
                add_region(name=""): Add neural region
                get_region(id): Get region by ID
                get_region_count(): Get number of regions
                get_region_ids(): Get all region IDs
                get_regions(): Get all regions
                get_total_neuron_count(): Get total neuron count
                get_total_synapse_count(): Get total synapse count
                get_active_neuron_count(): Get active neuron count
                get_firing_neuron_count(): Get firing neuron count
                get_average_firing_rate(): Get average firing rate
                get_excitation_inhibition_ratio(): Get E/I ratio
                get_total_spike_count(): Get total spike count
                get_developmental_stage(): Get developmental stage
                set_developmental_stage(stage): Set developmental stage
                get_config(): Get configuration
                log_status(): Get brain status
                __repr__(): String representation
                __str__(): String representation
        )pbdoc";

    // Create brain factory functions
    m.def("create_brain", py::overload_cast<const std::shared_ptr<Config>&>(&create_brain),
          py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc");
    
    m.def("create_brain", py::overload_cast<const std::shared_ptr<PyConfig>&>(&create_brain_from_pyconfig),
          py::arg("config"), R"pbdoc(Create a new brain with Pythonic configuration)pbdoc");

    // AgentBrain class
    py::class_<PyAgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize with brain")
        .def(py::init<std::shared_ptr<PyBrain>>(), py::arg("brain"),
             "Initialize with Pythonic brain")
        .def("initialize", &PyAgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &PyAgentBrain::get_sensory_input_size,
             "Get expected sensory input size")
        .def("get_motor_output_size", &PyAgentBrain::get_motor_output_size,
             "Get expected motor output size")
        .def("process_sensory_input", &PyAgentBrain::process_sensory_input,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &PyAgentBrain::decode_motor_command,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &PyAgentBrain::apply_reward_modulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &PyAgentBrain::update_development,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &PyAgentBrain::get_developmental_stage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &PyAgentBrain::get_neuromodulation_level,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &PyAgentBrain::get_curiosity_level,
             "Get curiosity level")
        .def("get_novelty_level", &PyAgentBrain::get_novelty_level,
             "Get novelty level")
        .def("get_prediction_error", &PyAgentBrain::get_prediction_error,
             "Get prediction error")
        .def("reset", &PyAgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &PyAgentBrain::get_brain,
             "Get the underlying brain")
        .def("enable_reward_modulation", &PyAgentBrain::enable_reward_modulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &PyAgentBrain::enable_structural_plasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &PyAgentBrain::enable_development,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &PyAgentBrain::enable_curiosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &PyAgentBrain::is_reward_modulation_enabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &PyAgentBrain::is_structural_plasticity_enabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &PyAgentBrain::is_development_enabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &PyAgentBrain::is_curiosity_enabled,
             "Check if curiosity is enabled")
        .def("__repr__", &PyAgentBrain::repr,
             "String representation of the agent brain")
        .doc() = R"pbdoc(
            Agent brain interface connecting NLM brain to world with Pythonic interface.
            
            Methods:
                __init__(brain): Initialize with brain
                initialize(world): Initialize with world
                get_sensory_input_size(): Get expected sensory input size
                get_motor_output_size(): Get expected motor output size
                process_sensory_input(percept): Process sensory percept
                decode_motor_command(): Decode motor activity to command
                apply_reward_modulation(reward, predictedReward): Apply reward modulation
                update_development(timestep): Update development system
                get_developmental_stage(): Get developmental stage
                get_neuromodulation_level(): Get neuromodulation level
                get_curiosity_level(): Get curiosity level
                get_novelty_level(): Get novelty level
                get_prediction_error(): Get prediction error
                reset(): Reset for new episode
                get_brain(): Get underlying brain
                enable_reward_modulation(enable): Enable/disable reward modulation
                enable_structural_plasticity(enable): Enable/disable structural plasticity
                enable_development(enable): Enable/disable development
                enable_curiosity(enable): Enable/disable curiosity
                is_reward_modulation_enabled(): Check reward modulation
                is_structural_plasticity_enabled(): Check structural plasticity
                is_development_enabled(): Check development
                is_curiosity_enabled(): Check curiosity
                __repr__(): String representation
        )pbdoc";

    // Create agent brain factory functions
    m.def("create_agent_brain", py::overload_cast<const std::shared_ptr<Brain>&>(&create_agent_brain),
          py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");
    
    m.def("create_agent_brain", py::overload_cast<const std::shared_ptr<PyBrain>&>(&create_agent_brain_from_pybrain),
          py::arg("brain"), R"pbdoc(Create a new agent brain interface with Pythonic brain)pbdoc");

    // Context manager support for brain
    py::class_<PyBrain>(m, "BrainContext", R"pbdoc(Context manager for Brain)")
        .def(py::init<std::shared_ptr<PyConfig>>())
        .def("__enter__", [](std::shared_ptr<PyBrain> brain) {
            return brain;
        })
        .def("__exit__", [](std::shared_ptr<PyBrain> brain, py::args) {
            brain->reset();
        })
        .doc() = R"pbdoc(
            Context manager for Brain with automatic reset on exit.
            
            Usage:
                with nlm.BrainContext(config) as brain:
                    brain.initialize()
                    # Brain will be automatically reset on exit
        )pbdoc";

    // Simulation utilities
    m.def("run_simulation", 
          [](std::shared_ptr<PyBrain> brain, std::shared_ptr<PySimpleWorld> world, int steps) {
              brain->initialize();
              for (int i = 0; i < steps; ++i) {
                  auto percept = world->get_sensory_percept();
                  brain->receive_sensory_input(percept);
                  auto action = brain->produce_action();
                  world->update(i);
              }
          },
          py::arg("brain"), py::arg("world"), py::arg("steps"),
          "Run a complete simulation");

    m.def("create_experiment_brain",
          [](std::shared_ptr<PyConfig> config, size_t population_size) {
              auto brain = create_brain_from_pyconfig(config);
              brain->add_region("primary");
              brain->add_region("motor");
              brain->add_region("visual");
              return brain;
          },
          py::arg("config"), py::arg("population_size") = 1000,
          "Create a brain with experimental configuration");

    // Convenience functions for advanced users
    m.def("create_advanced_agent",
          [](std::shared_ptr<PyConfig> config, std::shared_ptr<PySimpleWorld> world) {
              auto brain = create_brain_from_pyconfig(config);
              auto agent = create_agent_brain_from_pybrain(brain);
              agent->initialize(*world);
              return agent;
          },
          py::arg("config"), py::arg("world"),
          "Create an advanced agent with complete setup");

    m.def("create_training_environment",
          [](size_t width, size_t height, size_t vision_width, size_t vision_height) {
              auto world = create_simple_world();
              world->configure(width, height, vision_width, vision_height);
              return world;
          },
          py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
          "Create a training environment for agent development");

    m.def("create_test_config",
          []() {
              auto config = create_config();
              return config;
          },
          "Create a test configuration for quick prototyping");

    // Performance optimization helpers
    m.def("optimize_brain_for_performance",
          [](std::shared_ptr<PyBrain> brain) {
              // Enable all optimizations
              brain->set_developmental_stage(DevelopmentalStage::Adult);
              return brain;
          },
          py::arg("brain"), "Optimize brain for performance");

    m.def("enable_memory_optimization",
          [](std::shared_ptr<PyBrain> brain) {
              // Enable memory optimizations
              return brain;
          },
          py::arg("brain"), "Enable memory optimizations");

    // Debugging and monitoring
    m.def("monitor_brain_health",
          [](std::shared_ptr<PyBrain> brain) {
              return brain->get_status();
          },
          py::arg("brain"), "Get brain health monitoring information");

    m.def("print_brain_summary",
          [](std::shared_ptr<PyBrain> brain) {
              std::cout << brain->repr() << std::endl;
          },
          py::arg("brain"), "Print brain summary to stdout");

    m.def("export_brain_to_json",
          [](std::shared_ptr<PyBrain> brain, const std::string& filepath) {
              brain->save(filepath);
          },
          py::arg("brain"), py::arg("filepath"), "Export brain state to JSON file");

    m.def("import_brain_from_json",
          [](const std::string& filepath) {
              auto config = create_config();
              auto brain = create_brain_from_pyconfig(config);
              brain->load(filepath);
              return brain;
          },
          py::arg("filepath"), "Import brain state from JSON file");

    // Example and demonstration functions
    m.def("demonstrate_basic_usage",
          []() {
              std::cout << "=== NLM Basic Usage Example ===\n";
              std::cout << "1. Create a brain with default config\n";
              auto config = create_config();
              auto brain = create_brain_from_pyconfig(config);
              std::cout << "2. Create a world\n";
              auto world = create_simple_world();
              world->configure(50, 50, 10, 10);
              std::cout << "3. Create an agent\n";
              auto agent = create_agent_brain_from_pybrain(brain);
              agent->initialize(*world);
              std::cout << "4. Run simulation step\n";
              auto percept = world->get_sensory_percept();
              agent->process_sensory_input(percept);
              auto action = agent->decode_motor_command();
              std::cout << "5. Agent action: " << action.get_name() << "\n";
              std::cout << "\n=== Example completed successfully ===\n";
          },
          "Demonstrate basic NLM usage");

    m.def("create_development_stage_brain",
          [](DevelopmentalStage stage) {
              auto config = create_config();
              auto brain = create_brain_from_pyconfig(config);
              brain->set_developmental_stage(stage);
              return brain;
          },
          py::arg("stage"), "Create a brain with specific developmental stage");

    // Utility functions for debugging and development
    m.def("print_enum_values", []() {
        std::cout << "=== Neuron Types ===\n";
        std::cout << "0: Excitatory\n";
        std::cout << "1: Inhibitory\n";
        std::cout << "2: Modulatory\n";
        std::cout << "3: Sensory\n";
        std::cout << "4: Motor\n";
        std::cout << "5: Internal\n\n";
        std::cout << "=== Synapse Types ===\n";
        std::cout << "0: Excitatory\n";
        std::cout << "1: Inhibitory\n";
        std::cout << "2: Modulatory\n";
        std::cout << "3: Electrical\n";
        std::cout << "4: GapJunction\n\n";
        std::cout << "=== Action Types ===\n";
        std::cout << "0: MoveForward\n";
        std::cout << "1: MoveBackward\n";
        std::cout << "2: MoveLeft\n";
        std::cout << "3: MoveRight\n";
        std::cout << "4: TurnLeft\n";
        std::cout << "5: TurnRight\n";
        std::cout << "6: Look\n";
        std::cout << "7: LookUp\n";
        std::cout << "8: LookDown\n";
        std::cout << "9: Interact\n";
        std::cout << "10: Eat\n";
        std::cout << "11: Drink\n";
        std::cout << "12: Rest\n";
        std::cout << "13: Wait\n";
        std::cout << "14: Custom\n\n";
        std::cout << "=== Developmental Stages ===\n";
        std::cout << "0: Initial\n";
        std::cout << "1: CriticalPeriod\n";
        std::cout << "2: Maturation\n";
        std::cout << "3: Adult\n";
        std::cout << "4: Aging\n";
    }, "Print enum values for debugging");

    // Advanced agent creation with all systems enabled
    m.def("create_advanced_agent_with_all_systems",
          []() {
              auto config = create_config();
              auto world = create_simple_world();
              world->configure(100, 100, 20, 20);
              
              auto brain = create_brain_from_pyconfig(config);
              auto agent = create_agent_brain_from_pybrain(brain);
              
              agent->enable_reward_modulation(true);
              agent->enable_structural_plasticity(true);
              agent->enable_development(true);
              agent->enable_curiosity(true);
              
              agent->initialize(*world);
              
              return agent;
          },
          "Create an advanced agent with all systems enabled");

    // Quick prototyping functions
    m.def("quick_prototype",
          []() {
              auto world = create_training_environment(30, 30, 8, 8);
              auto agent = create_advanced_agent_with_all_systems();
              return std::make_tuple(agent, world);
          },
          "Quick prototyping setup with all features enabled");

    // Safety and error handling
    m.def("safe_step",
          [](std::shared_ptr<PyBrain> brain, std::shared_ptr<PySimpleWorld> world, int steps) {
              try {
                  brain->initialize();
                  for (int i = 0; i < steps; ++i) {
                      auto percept = world->get_sensory_percept();
                      brain->receive_sensory_input(percept);
                      auto action = brain->produce_action();
                      world->update(i);
                  }
                  return std::string("Simulation completed successfully");
              } catch (const std::exception& e) {
                  return std::string("Error: ") + e.what();
              }
          },
          py::arg("brain"), py::arg("world"), py::arg("steps"),
          "Safely run simulation with error handling");

    m.def("validate_brain_state",
          [](std::shared_ptr<PyBrain> brain) {
              auto status = brain->get_status();
              std::cout << "=== Brain Validation ===\n";
              std::cout << status << std::endl;
              return status;
          },
          py::arg("brain"), "Validate brain state and return status");

    // Experimental setup helpers
    m.def("create_experiment_setup",
          [](size_t width, size_t height, size_t vision_width, size_t vision_height) {
              auto config = create_test_config();
              auto world = create_training_environment(width, height, vision_width, vision_height);
              auto agent = create_advanced_agent(config, world);
              return std::make_tuple(config, world, agent);
          },
          py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
          "Create complete experimental setup");

    // Performance benchmarking helpers
    m.def("benchmark_brain_creation",
          []() {
              auto start_time = std::chrono::high_resolution_clock::now();
              for (int i = 0; i < 10; ++i) {
                  auto config = create_config();
                  auto brain = create_brain_from_pyconfig(config);
              }
              auto end_time = std::chrono::high_resolution_clock::now();
              auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
              std::cout << "Created 10 brains in " << duration.count() << " ms\n";
              return duration.count();
          },
          "Benchmark brain creation performance");

    m.def("benchmark_simulation_step",
          [](std::shared_ptr<PyBrain> brain, std::shared_ptr<PySimpleWorld> world, int steps) {
              auto start_time = std::chrono::high_resolution_clock::now();
              brain->initialize();
              for (int i = 0; i < steps; ++i) {
                  auto percept = world->get_sensory_percept();
                  brain->receive_sensory_input(percept);
                  auto action = brain->produce_action();
                  world->update(i);
              }
              auto end_time = std::chrono::high_resolution_clock::now();
              auto duration = std::chrono::duration_cast<std::milliseconds>(end_time - start_time);
              std::cout << "Completed " << steps << " simulation steps in " << duration.count() << " ms\n";
              return duration.count();
          },
          py::arg("brain"), py::arg("world"), py::arg("steps"),
          "Benchmark simulation performance");

    // Diagnostic and debugging tools
    m.def("debug_brain_connections",
          [](std::shared_ptr<PyBrain> brain) {
              std::cout << "=== Brain Connection Debug ===\n";
              std::cout << "Total neurons: " << brain->get_total_neuron_count() << std::endl;
              std::cout << "Total synapses: " << brain->get_total_synapse_count() << std::endl;
              std::cout << "Active neurons: " << brain->get_active_neuron_count() << std::endl;
              std::cout << "Firing neurons: " << brain->get_firing_neuron_count() << std::endl;
              std::cout << "Regions: " << brain->get_region_count() << std::endl;
              return brain->get_total_neuron_count() + brain->get_total_synapse_count();
          },
          py::arg("brain"), "Debug brain connections and return total units");

    m.def("analyze_brain_efficiency",
          [](std::shared_ptr<PyBrain> brain) {
              auto total_neurons = brain->get_total_neuron_count();
              auto active_neurons = brain->get_active_neuron_count();
              auto firing_neurons = brain->get_firing_neuron_count();
              auto efficiency = total_neurons > 0 ? (float)firing_neurons / total_neurons : 0.0f;
              
              std::cout << "=== Brain Efficiency Analysis ===\n";
              std::cout << "Total neurons: " << total_neurons << std::endl;
              std::cout << "Active neurons: " << active_neurons << std::endl;
              std::cout << "Firing neurons: " << firing_neurons << std::endl;
              std::cout << "Efficiency: " << (efficiency * 100) << "%\n";
              return efficiency;
          },
          py::arg("brain"), "Analyze brain efficiency and return efficiency ratio");
}