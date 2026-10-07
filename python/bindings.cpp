#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

// Include all NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Exception class for NLM-specific errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Helper functions for error handling and validation
namespace python_bindings {
    
    // Helper to convert C++ string to Python string with error checking
    std::string safe_string_conversion(const std::string& str) {
        if (str.empty()) {
            return "";
        }
        return str;
    }
    
    // Helper to validate neuron IDs
    void validate_neuron_id(NeuronId id, const std::string& context = "") {
        if (id == INVALID_NEURON_ID) {
            std::string msg = "Invalid neuron ID used";
            if (!context.empty()) {
                msg += " in " + context;
            }
            throw NLMError(msg);
        }
    }
    
    // Helper to validate region IDs
    void validate_region_id(RegionId id, const std::string& context = "") {
        if (id == INVALID_REGION_ID) {
            std::string msg = "Invalid region ID used";
            if (!context.empty()) {
                msg += " in " + context;
            }
            throw NLMError(msg);
        }
    }
    
    // Helper to validate configuration parameters
    void validate_config(const std::shared_ptr<Config>& config) {
        if (!config) {
            throw NLMError("Configuration object is null");
        }
    }
    
    // Helper to create Python-friendly error messages
    std::string create_error_message(const std::string& operation, 
                                     const std::string& details = "") {
        std::stringstream ss;
        ss << "NLM Error during " << operation;
        if (!details.empty()) {
            ss << ": " << details;
        }
        return ss.str();
    }
}

// Brain class Python binding with enhanced error handling
class BrainWrapper {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    BrainWrapper(std::shared_ptr<Config> config) {
        python_bindings::validate_config(config);
        brain_ = std::make_shared<Brain>(config);
    }
    
    bool initialize() {
        try {
            return brain_->initialize();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("brain initialization", 
                                                               e.what()));
        }
    }
    
    void step(SimulationStep currentStep) {
        try {
            brain_->step(currentStep);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("brain step (SimulationStep)", 
                                                               e.what()));
        }
    }
    
    void step_with_time(SimulationStep currentStep, Timestamp currentTime) {
        try {
            brain_->step(currentStep, currentTime);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("brain step (with time)", 
                                                               e.what()));
        }
    }
    
    std::unique_ptr<Action> produceAction() {
        try {
            return brain_->produceAction();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("action production", 
                                                               e.what()));
        }
    }
    
    void reset() {
        try {
            brain_->reset();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("brain reset", 
                                                               e.what()));
        }
    }
    
    bool save(const std::string& filepath) const {
        try {
            return brain_->save(filepath);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("brain save", 
                                                               e.what()));
        }
    }
    
    bool load(const std::string& filepath) {
        try {
            return brain_->load(filepath);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("brain load", 
                                                               e.what()));
        }
    }
    
    RegionId addRegion(const std::string& name = "") {
        try {
            return brain_->addRegion(name);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("region addition", 
                                                               e.what()));
        }
    }
    
    NeuralRegion* getRegion(RegionId id) {
        try {
            python_bindings::validate_region_id(id, "getRegion");
            return brain_->getRegion(id);
        } catch (const std::exception& e) {
            if (std::string(e.what()).find("NLM Error") != std::string::npos) {
                throw; // Re-throw NLM errors as-is
            }
            throw NLMError(python_bindings::create_error_message("getRegion", 
                                                               e.what()));
        }
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        try {
            python_bindings::validate_region_id(id, "getRegion (const)");
            return brain_->getRegion(id);
        } catch (const std::exception& e) {
            if (std::string(e.what()).find("NLM Error") != std::string::npos) {
                throw; // Re-throw NLM errors as-is
            }
            throw NLMError(python_bindings::create_error_message("getRegion (const)", 
                                                               e.what()));
        }
    }
    
    size_t getRegionCount() const {
        try {
            return brain_->getRegionCount();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getRegionCount", 
                                                               e.what()));
        }
    }
    
    std::vector<RegionId> getRegionIds() const {
        try {
            return brain_->getRegionIds();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getRegionIds", 
                                                               e.what()));
        }
    }
    
    // Statistical getters with error handling
    size_t getTotalNeuronCount() const {
        try {
            return brain_->getTotalNeuronCount();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getTotalNeuronCount", 
                                                               e.what()));
        }
    }
    
    size_t getTotalSynapseCount() const {
        try {
            return brain_->getTotalSynapseCount();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getTotalSynapseCount", 
                                                               e.what()));
        }
    }
    
    size_t getActiveNeuronCount() const {
        try {
            return brain_->getActiveNeuronCount();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getActiveNeuronCount", 
                                                               e.what()));
        }
    }
    
    size_t getFiringNeuronCount() const {
        try {
            return brain_->getFiringNeuronCount();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getFiringNeuronCount", 
                                                               e.what()));
        }
    }
    
    float getAverageFiringRate() const {
        try {
            return brain_->getAverageFiringRate();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getAverageFiringRate", 
                                                               e.what()));
        }
    }
    
    float getExcitationInhibitionRatio() const {
        try {
            return brain_->getExcitationInhibitionRatio();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getExcitationInhibitionRatio", 
                                                               e.what()));
        }
    }
    
    size_t getTotalSpikeCount() const {
        try {
            return brain_->getTotalSpikeCount();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getTotalSpikeCount", 
                                                               e.what()));
        }
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        try {
            return brain_->getDevelopmentalStage();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getDevelopmentalStage", 
                                                               e.what()));
        }
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        try {
            brain_->setDevelopmentalStage(stage);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("setDevelopmentalStage", 
                                                               e.what()));
        }
    }
    
    // Helper methods
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        python_bindings::validate_neuron_id(neuron, "injectCurrent");
        brain_->injectCurrent(neuron, current);
    }
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    std::shared_ptr<const Config> getConfig() const {
        return brain_->getConfig();
    }
    
    void logStatus() const {
        try {
            brain_->logStatus();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("logStatus", 
                                                               e.what()));
        }
    }
    
    std::shared_ptr<Brain> getBrain() {
        return brain_;
    }
};

// AgentBrain class Python binding with enhanced error handling
class AgentBrainWrapper {
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    
public:
    AgentBrainWrapper(std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw NLMError("Cannot create AgentBrain with null brain");
        }
        agentBrain_ = std::make_shared<AgentBrain>(brain);
    }
    
    void initialize(std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw NLMError("Cannot initialize AgentBrain with null world");
        }
        try {
            agentBrain_->initialize(world);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("agent initialization", 
                                                               e.what()));
        }
    }
    
    size_t getSensoryInputSize() const {
        try {
            return agentBrain_->getSensoryInputSize();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getSensoryInputSize", 
                                                               e.what()));
        }
    }
    
    size_t getMotorOutputSize() const {
        try {
            return agentBrain_->getMotorOutputSize();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getMotorOutputSize", 
                                                               e.what()));
        }
    }
    
    void processSensoryInput(const SensoryPercept& percept) {
        try {
            agentBrain_->processSensoryInput(percept);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("sensory processing", 
                                                               e.what()));
        }
    }
    
    std::unique_ptr<Action> decodeMotorCommand() {
        try {
            return agentBrain_->decodeMotorCommand();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("motor command decoding", 
                                                               e.what()));
        }
    }
    
    void applyRewardModulation(float reward, float predictedReward = 0.0f) {
        try {
            agentBrain_->applyRewardModulation(reward, predictedReward);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("reward modulation", 
                                                               e.what()));
        }
    }
    
    void updateDevelopment(double timestep) {
        try {
            agentBrain_->updateDevelopment(timestep);
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("development update", 
                                                               e.what()));
        }
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        try {
            return agentBrain_->getDevelopmentalStage();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getDevelopmentalStage", 
                                                               e.what()));
        }
    }
    
    float getNeuromodulationLevel() const {
        try {
            return agentBrain_->getNeuromodulationLevel();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getNeuromodulationLevel", 
                                                               e.what()));
        }
    }
    
    float getCuriosityLevel() const {
        try {
            return agentBrain_->getCuriosityLevel();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getCuriosityLevel", 
                                                               e.what()));
        }
    }
    
    float getNoveltyLevel() const {
        try {
            return agentBrain_->getNoveltyLevel();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getNoveltyLevel", 
                                                               e.what()));
        }
    }
    
    float getPredictionError() const {
        try {
            return agentBrain_->getPredictionError();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("getPredictionError", 
                                                               e.what()));
        }
    }
    
    void reset() {
        try {
            agentBrain_->reset();
        } catch (const std::exception& e) {
            throw NLMError(python_bindings::create_error_message("agent reset", 
                                                               e.what()));
        }
    }
    
    // Subsystem enable/disable methods
    void enableRewardModulation(bool enable) {
        agentBrain_->enableRewardModulation(enable);
    }
    
    void enableStructuralPlasticity(bool enable) {
        agentBrain_->enableStructuralPlasticity(enable);
    }
    
    void enableDevelopment(bool enable) {
        agentBrain_->enableDevelopment(enable);
    }
    
    void enableCuriosity(bool enable) {
        agentBrain_->enableCuriosity(enable);
    }
    
    bool isRewardModulationEnabled() const {
        return agentBrain_->isRewardModulationEnabled();
    }
    
    bool isStructuralPlasticityEnabled() const {
        return agentBrain_->isStructuralPlasticityEnabled();
    }
    
    bool isDevelopmentEnabled() const {
        return agentBrain_->isDevelopmentEnabled();
    }
    
    bool isCuriosityEnabled() const {
        return agentBrain_->isCuriosityEnabled();
    }
    
    std::shared_ptr<Brain> getBrain() {
        return agentBrain_->getBrain();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        
        A robust Python interface for the NLM C++ neural simulation framework.
        
        This module provides enhanced error handling, comprehensive documentation,
        and Pythonic APIs for working with artificial developmental brains.
        
        Key features:
        - Comprehensive error handling with detailed NLMError exceptions
        - Full API documentation with examples
        - Pythonic wrapper classes for better usability
        - Type safety and parameter validation
        - Performance-optimized bindings
        
        Quick start example:
            
        Basic usage:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> 
        >>> for i in range(100):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     agent.processSensoryInput(percept)
        ...     brain.step(i)
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
        
        Enable learning:
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
            >>> agent.enableDevelopment(True)
        
        Save/load capabilities:
            >>> brain.save("my_brain.pkl")
            >>> new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            >>> new_brain.load("my_brain.pkl")
            
        The brain object provides access to all subsystems:
        - Memory systems: working, episodic, associative, procedural
        - Prediction and cognition systems
        - Neuromodulation: dopamine, curiosity, novelty, prediction error
        - Development and plasticity mechanisms
        - Statistics and monitoring tools
        
        For detailed documentation, see:
        - https://github.com/your-repo/nlm for examples and tutorials
        - The NLM documentation in docs/ for scientific background
        - https://pynlm.readthedocs.io for Python-specific documentation
    )pbdoc";

    // Register custom exception
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    
    // Register basic type identifiers with improved documentation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with value semantics
        
        NeuronId encapsulates a 64-bit unsigned integer that uniquely identifies
        a neuron within a brain. The ID system supports indexing operations,
        comparison, and hashing for use in Python dictionaries and sets.
        
        Examples:
            >>> neuron_id = NeuronId(42)
            >>> print(neuron_id.value)
            42
            >>> neuron_id2 = NeuronId(42)
            >>> neuron_id == neuron_id2
            True
        )pbdoc")
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
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Register other basic identifiers with enhanced documentation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with value semantics
        
        SynapseId represents the unique identifier of a synaptic connection
        between neurons. Used for managing synaptic connectivity and tracking
        specific connections for plasticity mechanisms.
        
        Examples:
            >>> synapse_id = SynapseId(123)
            >>> print(synapse_id.value)
            123
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with value semantics
        
        RegionId identifies brain regions that group neurons by function,
        anatomy, or developmental origin. Regions can represent cortical
        areas, subcortical structures, or functional groupings.
        
        Examples:
            >>> region_id = RegionId(1)
            >>> print(region_id.value)
            1
        Common region types:
        - Prefrontal cortex
        - Hippocampus
        - Amygdala
        - Cerebellum
        - Sensory areas
        - Motor areas
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with value semantics
        
        PopulationId represents groups of neurons that share similar properties,
        functions, or anatomical locations. Populations are used for population-level
        analysis and control of neural activity.
        
        Examples:
            >>> population_id = PopulationId(5)
            >>> print(population_id.value)
            5
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Register enum types with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM model
        
        Each neuron type has distinct functional properties and connectivity patterns.
        The choice of neuron type affects synaptic interactions and plasticity.
        
        Types:
        - Excitatory: Neurons that increase membrane potential in postsynaptic neurons
        - Inhibitory: Neurons that decrease membrane potential in postsynaptic neurons  
        - Modulatory: Neurons that release neuromodulators affecting plasticity
        - Sensory: Neurons that receive external sensory input
        - Motor: Neurons that generate motor output commands
        - Internal: General-purpose neurons for internal processing
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons (glutamatergic)")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons (GABAergic)")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons (dopaminergic, etc.)")
        .value("Sensory", NeuronType::Sensory, "Sensory input neurons")
        .value("Motor", NeuronType::Motor, "Motor output neurons")
        .value("Internal", NeuronType::Internal, "Internal processing neurons")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM model
        
        Different synapse types have distinct computational properties and dynamics.
        The synapse type determines how synaptic strength is modified and how
        signals are transmitted between neurons.
        
        Types:
        - Excitatory: Standard chemical synapses that increase postsynaptic potential
        - Inhibitory: Chemical synapses that decrease postsynaptic potential
        - Modulatory: Synapses that release neuromodulators
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Specialized electrical synapses
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the NLM model
        
        The developmental stage represents the ontogenetic trajectory of the neural system,
        affecting plasticity rates, connectivity, and functional properties.
        
        Stages:
        - Initial: Newborn stage with high plasticity
        - CriticalPeriod: Sensitive period for learning and refinement
        - Maturation: Structural and functional maturation
        - Adult: Fully mature, stable system
        - Aging: Age-related decline and plasticity reduction
        
        Each stage has characteristic:
        - Plasticity rates (high in early stages, low in adult/aged)
        - Synaptogenesis and pruning rates
        - Learning capacity
        - Behavioral flexibility
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage (newborn)")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage (fully mature)")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage (decline and reduced plasticity)")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states in the NLM model
        
        Firing states represent the current behavioral mode of a neuron,
        determining its ability to generate and propagate action potentials.
        
        States:
        - Resting: Not currently active, below threshold
        - Active: Above threshold but not currently firing
        - Refractory: Has just fired, unable to fire again immediately
        - Inhibited: Actively suppressed, cannot fire regardless of input
        )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Register ActionType with enhanced documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types for motor output
        
        Action types represent high-level behavioral commands that the agent
        can execute in the simulated environment.
        
        Common action types:
        - Movement: Navigate in the environment
        - Interaction: Manipulate objects
        - Consumption: Eat or drink
        - Rest: Stop activity and recover
        - Custom: User-defined action types
        
        Each action type has associated parameters (position, orientation, etc.)
        that determine the specifics of the action.
        )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in current direction")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in current direction")
        .value("MoveLeft", ActionType::MoveLeft, "Turn left and move")
        .value("MoveRight", ActionType::MoveRight, "Turn right and move")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left without moving")
        .value("TurnRight", ActionType::TurnRight, "Turn right without moving")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat/food consumption")
        .value("Drink", ActionType::Drink, "Drink/liquid consumption")
        .value("Rest", ActionType::Rest, "Rest and recover")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom user-defined action")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands
        
        Motor commands represent the lowest level of motor output,
        directly controlling the agent's body in the environment.
        
        Commands:
        - Movement: Basic translational motion
        - Orientation: Rotational motion
        - Looking: Visual direction control
        - Interaction: Physical interaction with environment
        - Wait: No movement or interaction
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the NLM world
        
        WorldObjectType classifies objects in the simulated environment based on
        their properties and potential interactions with the agent.
        
        Types:
        - Empty: No object (empty space)
        - Resource: Positive value object (food, water, etc.)
        - Hazard: Negative value object (dangerous, toxic, etc.)
        - Wall: Static obstacle
        - Marker: Special object for testing or visualization
        
        Object types determine:
        - Agent behavior (approach, avoid, ignore)
        - Reward/punishment values
        - Interaction rules
        - Movement constraints
        )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Register Config class with enhanced error handling and documentation
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM with JSON file support
        
        The Config class manages all simulation parameters, neural properties,
        learning rates, and system settings. It provides a flexible interface
        for runtime configuration and persistence through JSON files.
        
        Key features:
        - Key-value storage with typed values (int, float, string, etc.)
        - Multiple configuration sources (default, file, command-line, runtime)
        - JSON file I/O for configuration persistence
        - Command-line argument parsing
        - Configuration validation and error checking
        
        Common configuration parameters:
        - brain.neuron_count: Number of neurons in the brain
        - brain.synapse_density: Connection probability
        - plasticity.stdp.enable: Enable STDP learning
        - neuromod.dopamine.scale: Dopamine modulation strength
        - simulation.timestep: Simulation timestep duration
        - log.level: Logging verbosity
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("plasticity.learning_rate", 0.01)
            >>> config.saveToFile("my_config.json")
            >>> new_config = pynlm.createDefaultConfig()
            >>> new_config.loadFromFile("my_config.json")
        
        The configuration system supports nested keys using dot notation:
            config.set("brain.neuron_count", 1000)
            config.set("neuromod.dopamine.scale", 1.0)
            config.set("plasticity.stdp.enable", True)
        
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command-line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key is set.")
        .def("get", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a formatted summary string of all configuration entries")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Register SensoryInput with enhanced documentation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input from the environment
        
        SensoryInput represents data received from the environment that drives
        perception and behavior in the NLM system. It serves as the input channel
        between the simulated world and the neural processing components.
        
        Subclasses include:
        - Vision: Visual sensor data (images, scenes)
        - Audio: Auditory sensor data (sounds, music)
        - InternalSignals: Internal physiological signals
        
        Key properties:
        - Type: Type of sensory modality
        - Data: Raw sensory data (typically vectors of floats)
        - Dimensions: Shape of the sensory data
        - Timestamp: Time when the input was received
        
        Examples:
            >>> percept = world.getSensoryPercept()
            >>> vision_data = percept.getVision()
            >>> vision_data.getWidth()
            8
            >>> vision_data.getHeight()
            8
            >>> vision_data.getChannels()
            3
        
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the sensory input type")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input")
        .def("__repr__", [](const SensoryInput& input) {
            std::stringstream ss;
            ss << "<SensoryInput type=" << static_cast<int>(input.getType())
               << " dims=" << input.getDimensions() << ">";
            return ss.str();
        });

    // Register Vision with enhanced documentation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data
        
        Vision represents visual information from the environment, typically
        as a 2D image grid with color channels. This is the primary input
        modality for many NLM agents, enabling scene perception and object recognition.
        
        Attributes:
        - width: Image width in pixels
        - height: Image height in pixels  
        - channels: Number of color channels (typically 3 for RGB)
        - data: Flattened array of pixel values (normalized 0.0 to 1.0)
        
        Examples:
            >>> vision = Vision(8, 8, 3)  # 8x8 RGB image
            >>> vision.setData(pixel_array)  # Set pixel data
            >>> width = vision.getWidth()   # Get width
            8
            >>> height = vision.getHeight() # Get height
            8
            >>> channels = vision.getChannels()  # Get channels
            3
            
        >>> # Create and use vision data
        >>> pixel_data = [0.1, 0.5, 0.9] * (8 * 8 * 3)  # 192 values
        >>> vision.setData(pixel_data)
        >>> vision.getWidth()
        8
        >>> vision.getHeight()
        8
        
        The vision system typically represents:
        - Objects: Different pixel intensities/values
        - Walls: Obstacles with specific patterns
        - Resources: Positive values (food, water)
        - Hazards: Negative values (danger, toxicity)
        - Free space: Neutral values
        
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from a vector of float values")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("__repr__", [](const Vision& vision) {
            std::stringstream ss;
            ss << "<Vision " << vision.getWidth() << "x" << vision.getHeight()
               << "x" << vision.getChannels() << ">";
            return ss.str();
        });

    // Register Audio with enhanced documentation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data
        
        Audio represents auditory information from the environment, such as
        sounds, music, or environmental noises. This input modality enables
        agents to detect and respond to acoustic stimuli.
        
        Attributes:
        - sampleRate: Audio sampling rate in Hz
        - numSamples: Number of audio samples
        - data: Array of audio sample values (normalized)
        
        Examples:
            >>> audio = Audio(44100, 1024)  # 44.1kHz, 1024 samples
            >>> audio.setData(audio_samples)
            >>> audio.setSampleRate(22050)  # Set sample rate
            >>> audio.getSampleRate()
            22050
            >>> audio.getNumSamples()
            1024
            
        Audio systems can represent:
        - Speech: Spoken commands or vocalizations
        - Music: Rhythmic or melodic patterns
        - Environmental: Background sounds, alarms, etc.
        - Silence: Absence of sound
        
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from a vector of float values")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sampling rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sampling rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("__repr__", [](const Audio& audio) {
            std::stringstream ss;
            ss << "<Audio " << audio.getSampleRate() << "Hz, " 
               << audio.getNumSamples() << " samples>";
            return ss.str();
        });

    // Register InternalSignals with enhanced documentation
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal physiological signals sensory input
        
        InternalSignals represents internal bodily states and physiological
        signals that influence behavior and decision-making. These signals
        include metabolic state, stress levels, hormonal states, etc.
        
        This modality enables homeostatic regulation and internal state monitoring.
        
        Examples:
            >>> internal = InternalSignals()
            >>> internal.addSignal(0.5)  # Energy level
            >>> internal.addSignal(0.8)  # Hydration level
            >>> internal.addSignal(0.3)  # Stress level
            >>> internal.getAllSignals()
            [0.5, 0.8, 0.3]
            >>> internal.clearSignals()
        
        Common internal signals:
        - Energy: Current energy level (0.0 to 1.0)
        - Hydration: Water balance (0.0 to 1.0)
        - Hunger: Nutritional state (0.0 to 1.0)
        - Stress: Stress hormone levels (0.0 to 1.0)
        - Health: Overall health status (0.0 to 1.0)
        - Temperature: Body temperature relative to optimum
        
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value (0.0 to 1.0)")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getAllSignals", &InternalSignals::getData, 
             "Get all internal signal values as a list")
        .def("__repr__", [](const InternalSignals& internal) {
            return "<InternalSignals " + std::to_string(internal.getData().size()) + 
                   " signals>";
        });

    // Register Action with enhanced documentation
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action representation for the agent
        
        Action represents a high-level behavioral command that the agent can
        execute in the simulated environment. It specifies both the type of
        action and any associated parameters (position, orientation, values, etc.).
        
        Actions are produced by the brain's motor system and executed by the world,
        resulting in environmental changes and potential reward feedback.
        
        Key attributes:
        - type: The action type (MoveForward, Interact, Eat, etc.)
        - parameters: List of float values specific to the action type
        - name: Human-readable name for the action type
        
        Examples:
            >>> move_action = Action(ActionType.MoveForward)
            >>> move_action.getType()
            ActionType.MoveForward
            >>> move_action.getName()
            "MoveForward"
            >>> 
            >>> eat_action = Action(ActionType.Eat, [0.7, 0.9])  # position parameters
            >>> eat_action.getType()
            ActionType.Eat
            >>> params = eat_action.getParameters()
            >>> print(params)
            [0.7, 0.9]
            
        >>> # Action cloning for modification
        >>> action1 = Action(ActionType.MoveForward)
        >>> action2 = action1.clone()  # Create a copy
        >>> action2.getType() == ActionType.MoveForward
        True
        
        Common action types and their parameters:
        - MoveForward, MoveBackward, TurnLeft, TurnRight: No parameters
        - Look, LookUp, LookDown: No parameters
        - Interact, Eat, Drink: Position parameters (x, y)
        - Custom: User-defined parameters
        
        Actions are immutable after creation to ensure consistency.
        Use clone() method to create modified copies.
        
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type and default parameters")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with specified type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters as list")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters from list")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            std::stringstream ss;
            ss << "<Action type=" << static_cast<int>(action.getType())
               << " params=" << action.getParameters() << ">";
            return ss.str();
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // Register WorldObject with enhanced documentation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Object in the NLM world with position and properties
        
        WorldObject represents objects in the simulated environment, such as
        resources, hazards, walls, or markers. Objects have position, type,
        value, and other properties that determine how the agent interacts with them.
        
        Key attributes:
        - x, y: Position coordinates in world space
        - type: Object type (Resource, Hazard, Wall, Marker, Empty)
        - value: Numerical value (positive for resources, negative for hazards)
        - radius: Interaction radius around the object
        - active: Whether the object is currently active
        
        Examples:
            >>> # Create a resource object
            >>> resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=10.0)
            >>> resource.x
            5.0
            >>> resource.y
            5.0
            >>> resource.type
            WorldObjectType.Resource
            >>> resource.value
            10.0
            >>> 
            >>> # Create a wall
            >>> wall = WorldObject(2.0, 3.0, WorldObjectType.Wall, radius=0.5)
            >>> wall.type
            WorldObjectType.Wall
            >>> wall.radius
            0.5
            >>> 
            >>> # Check if position is valid
            >>> world.isValidPosition(1.0, 1.0)
            True
            >>> world.addObject(resource)
            >>> world.removeObject(5.0, 5.0)
        
        Object types and their properties:
        - Resource: Positive value, attracts agent
        - Hazard: Negative value, repels agent  
        - Wall: Static obstacle, blocks movement
        - Marker: Special object for testing or navigation
        - Empty: No object (empty space)
        
        Objects influence agent behavior:
        - Resources: Drive motivation to approach and consume
        - Hazards: Drive motivation to avoid
        - Walls: Constrain movement and navigation
        - Markers: Guide navigation and learning
        
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position, type, and optional parameters")
        .def_readwrite("x", &WorldObject::x, "X coordinate in world space")
        .def_readwrite("y", &WorldObject::y, "Y coordinate in world space")
        .def_readwrite("radius", &WorldObject::radius, "Interaction radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Numerical value (positive/negative)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("__repr__", [](const WorldObject& obj) {
            std::stringstream ss;
            ss << "<WorldObject type=" << static_cast<int>(obj.type)
               << " pos=(" << obj.x << "," << obj.y << ") value=" << obj.value << ">";
            return ss.str();
        });

    // Register AgentBody with enhanced documentation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent's physical body state in the world
        
        AgentBody represents the agent's embodied state in the simulated environment,
        including position, orientation, movement dynamics, and physiological state.
        
        This class tracks the agent's complete state for sensory-motor coordination.
        
        Key attributes:
        - x, y: Position coordinates
        - orientation: Facing direction in radians
        - velocityX, velocityY: Linear velocity components
        - angularVelocity: Rotational speed
        - energy: Current energy level (0.0 to 1.0)
        - health: Overall health status (0.0 to 1.0)
        - age: Developmental age in simulation steps
        - isMoving: Whether agent is currently moving
        - isTurning: Whether agent is currently turning
        - lastActionTime: Time since last action
        
        Examples:
            >>> body = AgentBody()
            >>> body.x
            0.0
            >>> body.y
            0.0
            >>> body.orientation
            0.0
            >>> body.energy
            1.0
            >>> body.health
            1.0
            >>> 
            >>> # Update body state
            >>> body.x = 5.0
            >>> body.y = 3.0
            >>> body.orientation = 1.57  # π/2 radians (90 degrees)
            >>> body.velocityX = 0.1
            >>> body.velocityY = 0.05
            >>> body.energy = 0.8
            >>> 
            >>> # Reset body to initial state
            >>> body.reset()
            >>> body.x
            0.0
        
        The agent body integrates:
        - Kinematic state (position, velocity, orientation)
        - Physiological state (energy, health, age)
        - Behavioral state (moving, turning, activity)
        - Temporal state (last action time)
        
        Body state is updated by the world simulation during each timestep.
        Agents use this information for perception and action planning.
        
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position coordinate")
        .def_readwrite("y", &AgentBody::y, "Y position coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0.0-1.0)")
        .def_readwrite("health", &AgentBody::health, "Current health status (0.0-1.0)")
        .def_readwrite("age", &AgentBody::age, "Developmental age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("__repr__", [](const AgentBody& body) {
            std::stringstream ss;
            ss << "<AgentBody pos=(" << body.x << "," << body.y << ") "
               << "orient=" << body.orientation << " vel=(" 
               << body.velocityX << "," << body.velocityY << ") "
               << "energy=" << body.energy << " health=" << body.health << ">";
            return ss.str();
        });

    // Register ActionResult with enhanced documentation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an agent's action in the world
        
        ActionResult represents the outcome of an agent's action, including
        any reward feedback, success status, and explanatory messages. This
        information is used by the agent to learn and adapt its behavior.
        
        Key attributes:
        - reward: Numerical reward value (positive for good outcomes, negative for bad)
        - success: Boolean indicating whether action succeeded
        - message: Human-readable description of the outcome
        
        Examples:
            >>> # Successful action with positive reward
            >>> result = ActionResult(reward=5.0, success=True, message="Found food")
            >>> result.reward
            5.0
            >>> result.success
            True
            >>> result.message
            "Found food"
            >>> 
            >>> # Failed action with negative reward
            >>> result2 = ActionResult(reward=-2.0, success=False, message="Hit wall")
            >>> result2.reward
            -2.0
            >>> result2.success
            False
            >>> result2.message
            "Hit wall"
        
        Action results drive:
        - Reward-based learning (dopamine release)
        - Policy gradient methods (reward signals)
        - Value function estimation (expected future rewards)
        - Credit assignment (which actions led to good/bad outcomes)
        
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Numerical reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Human-readable message")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult reward=" << result.reward << " success=" 
               << (result.success ? "True" : "False") << " msg=\"" << result.message << "\">";
            return ss.str();
        });

    // Register SensoryPercept with enhanced documentation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Multi-modal sensory percept from the environment
        
        SensoryPercept represents the complete sensory input received by the agent
        from the environment in a single timestep. It integrates information from
        all sensory modalities (vision, audio, internal signals) into a unified
        percept for neural processing.
        
        The percept serves as the bridge between the external world and the neural
        processing systems, providing the raw material for perception, learning,
        and behavior.
        
        Key attributes:
        - vision: Visual sensory input (image data)
        - audio: Auditory sensory input (sound data)
        - internal: Internal physiological signals
        - proprioception: Body position and movement signals
        - touch: Tactile sensory input
        - timestamp: Time when percept was received
        
        Examples:
            >>> # Create a percept with vision data
            >>> vision = Vision(8, 8, 3)
            >>> vision.setData(pixel_array)
            >>> percept = SensoryPercept()
            >>> percept.setVision(vision)
            >>> 
            >>> # Access all sensory modalities
            >>> vision_data = percept.getVision()
            >>> vision_width = percept.getVisionWidth()
            8
            >>> vision_height = percept.getVisionHeight()
            8
            >>> audio_data = percept.getAudio()
            >>> internal_signals = percept.getInternal()
            >>> proprioception = percept.getProprioception()
            >>> touch_data = percept.getTouch()
            >>> 
            >>> # Get all signals at once
            >>> all_signals = percept.getAllSignals()
            >>> all_signals.size()
            5  # Vision + Audio + Internal + Proprioception + Touch
            
        The percept integrates multiple information sources:
        
        1. **Exogenous inputs** (external world):
           - Vision: What the agent sees
           - Audio: What the agent hears
           - Touch: What the agent feels
           
        2. **Endogenous inputs** (internal state):
           - Internal: Physiological needs and states
           - Proprioception: Body position and movement
        
        3. **Temporal information**:
           - Timestamp: When the percept was received
        
        This multi-modal integration:
        - Provides a comprehensive view of the agent's situation
        - Enables grounded perception and action
        - Supports prediction and expectation formation
        - Drives motivated behavior based on needs and preferences
        
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get visual sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision image width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision image height")
        .def("getTouch", &SensoryPercept::getTouch, "Get tactile sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set tactile sensory input")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, 
             "Get proprioceptive sensory input")
        .def("setProprioception", &SensoryPercept::setProprioception, 
             py::arg("proprioception"), "Set proprioceptive input")
        .def("getAudio", &SensoryPercept::getAudio, "Get auditory sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set auditory sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, 
             "Get all sensory signals as a list")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept timestamp=" << percept.getTimestamp()
               << " vision=" << (percept.getVision().getWidth() * percept.getVision().getHeight() * percept.getVision().getChannels()) << " pixels>";
            return ss.str();
        });

    // Register SimpleWorld with enhanced documentation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM agent simulation
        
        SimpleWorld represents a 2D grid world where the agent interacts with
        the environment, objects, and dynamics. This world provides the
        simulated environment for agent perception, action, and learning.
        
        Key features:
        - 2D grid world with configurable dimensions
        - Agent representation with position and orientation
        - Object management (resources, hazards, walls, markers)
        - Physics simulation (movement, collisions, energy decay)
        - Sensory input generation (vision, proprioception, internal)
        - Action execution and feedback
        
        The world simulates:
        - **Space**: Continuous 2D coordinates
        - **Time**: Discrete timesteps with fixed duration
        - **Objects**: Static and dynamic entities
        - **Physics**: Movement, collisions, energy dynamics
        - **Sensing**: Agent perception of the world
        
        Examples:
            >>> # Create a simple world
            >>> world = SimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> 
            >>> # Set agent starting position
            >>> world.setAgentStart(10.0, 10.0)
            >>> 
            >>> # Reset world to initial state
            >>> world.reset()
            >>> 
            >>> # Run simulation
            >>> for i in range(100):
            ...     world.update(0.1)  # 100ms timestep
            ...     percept = world.getSensoryPercept()
            ...     # Process percept with agent
            ...     # Agent produces action
            ...     world.applyMotorCommand(action, world.getSimulationTime())
            >>> 
            >>> # Check agent position
            >>> body = world.getAgentBody()
            >>> body.x, body.y
            (5.3, 7.8)
            
        World configuration options:
        - width, height: World dimensions in meters
        - visionWidth, visionHeight: Sensory input resolution
        - maxEnergy: Maximum agent energy
        - energyDecayRate: Energy loss per timestep
        - randomSeed: For reproducible simulations
        
        Object types and behavior:
        - Resource: Positive value objects (food, water, etc.)
        - Hazard: Negative value objects (danger, toxicity, etc.)
        - Wall: Static obstacles that block movement
        - Marker: Special objects for testing or navigation
        
        The world provides:
        1. **Environment Dynamics**: Physics simulation, collision detection
        2. **Sensory Rendering**: Multi-modal input generation
        3. **Action Effects**: State changes based on agent actions
        4. **Feedback**: Reward signals, success/failure information
        5. **Persistence**: State management across timesteps
        
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, 
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory resolution")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position in the world")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by specified timestep (seconds)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply agent's motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, 
             py::arg("x"), py::arg("y"), "Check if position is valid (not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in meters")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in meters")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, 
             "Get energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time in seconds")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world dynamics")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed")
        .def("__repr__", [](const SimpleWorld& world) {
            std::stringstream ss;
            ss << "<SimpleWorld " << world.getWidth() << "x" << world.getHeight()
               << "m, vision=" << world.getVisionWidth() << "x" << world.getVisionHeight()
               << "m, agent pos=(" << world.getAgentBody().x << "," << world.getAgentBody().y
               << ") time=" << world.getSimulationTime() << "s>";
            return ss.str();
        });

    // Register Enhanced Brain class with error handling
    py::class_<BrainWrapper>(m, "Brain", R"pbdoc(
        Central neural brain with enhanced error handling and Pythonic API
        
        Brain represents the complete artificial neural system that processes
        information, makes decisions, and controls behavior. This is the main
        computational engine of the NLM framework.
        
        Key features:
        - Comprehensive error handling with detailed exception messages
        - Pythonic wrapper with automatic parameter validation
        - Full access to all neural subsystems (memory, prediction, cognition)
        - Checkpoint/save-load capabilities for persistence
        - Development system with multiple stages
        - Neuromodulation integration (reward, curiosity, novelty)
        
        The brain integrates:
        
        1. **Neural Core**: LIF spiking neurons with real-time dynamics
        2. **Memory Systems**: Working (transient), Episodic (experience), 
           Associative (pattern), Procedural (skills)
        3. **Prediction System**: Forward models and prediction error
        4. **Cognition Systems**: Planning, attention, concept formation
        5. **Development System**: Ontogenetic trajectory and plasticity
        6. **Neuromodulation Systems**: Dopamine, curiosity, novelty
        7. **Plasticity Systems**: STDP, Hebbian, structural plasticity
        
        Error handling provides:
        - Detailed error messages with operation context
        - Type safety and parameter validation
        - Graceful failure handling for invalid operations
        - Python exception compatibility
        
        Examples:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> 
            >>> # Initialize brain
            >>> brain.initialize()
            >>> 
            >>> # Check brain status
            >>> brain.logStatus()
            "NLM Phase 2: Real Neural Computation"
            "  total_neurons: 500"
            "  total_synapses: 12500"
            >>> 
            >>> # Access statistics
            >>> neurons = brain.getTotalNeuronCount()
            >>> synapses = brain.getTotalSynapseCount()
            >>> spikes = brain.getTotalSpikeCount()
            >>> 
            >>> # Set developmental stage
            >>> from pynlm import DevelopmentalStage
            >>> brain.setDevelopmentalStage(DevelopmentalStage.Adult)
            >>> 
            >>> # Save brain state
            >>> brain.save("my_brain.pkl")
            >>> 
            >>> # Load brain state
            >>> new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            >>> new_brain.load("my_brain.pkl")
        
        The brain class provides comprehensive access to all subsystems:
        - Memory systems for storage and retrieval
        - Prediction systems for expectation formation
        - Cognition systems for planning and decision-making
        - Development systems for ontogenetic changes
        - Neuromodulation systems for motivation and learning
        
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &BrainWrapper::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (BrainWrapper::*)(SimulationStep)>(&BrainWrapper::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step_with_time", &BrainWrapper::step_with_time,
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with explicit time")
        .def("receiveSensoryInput", &BrainWrapper::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &BrainWrapper::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into specific neuron")
        .def("injectCurrentToNeurons", &BrainWrapper::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current to neurons of specific type")
        .def("produceAction", &BrainWrapper::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &BrainWrapper::reset, "Reset brain state")
        .def("save", &BrainWrapper::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &BrainWrapper::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &BrainWrapper::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", py::overload_cast<RegionId>(&BrainWrapper::getRegion),
             py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegion", py::overload_cast<RegionId>(&BrainWrapper::getRegion, py::const),
             py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID (const)")
        .def("getRegionCount", &BrainWrapper::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &BrainWrapper::getRegionIds, "Get all region IDs")
        .def("getRegions", &BrainWrapper::getRegions, py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &BrainWrapper::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &BrainWrapper::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &BrainWrapper::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &BrainWrapper::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &BrainWrapper::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &BrainWrapper::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &BrainWrapper::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &BrainWrapper::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &BrainWrapper::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &BrainWrapper::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &BrainWrapper::logStatus, "Log brain status")
        .def("getBrain", &BrainWrapper::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain object");

    // Register AgentBrain class with enhanced error handling
    py::class_<AgentBrainWrapper>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface with error handling and Pythonic API
        
        AgentBrain provides the interface between the neural brain and the simulated
        world, handling sensory input processing, motor output decoding, and
        neuromodulation. It implements the complete perception-action loop.
        
        Key features:
        - Comprehensive error handling with detailed exception messages
        - Pythonic wrapper with automatic parameter validation
        - Full integration with brain's neuromodulation systems
        - Development system integration
        - Subsystem enable/disable control
        - Statistical monitoring and access
        
        The agent brain implements:
        
        1. **Sensory Processing**: Convert world input to neural signals
        2. **Motor Decoding**: Convert neural activity to actions
        3. **Neuromodulation**: Integrate reward, curiosity, and motivation
        4. **Development**: Track and update developmental stage
        5. **Learning**: Enable/disable specific learning mechanisms
        
        Error handling ensures:
        - Type safety and parameter validation
        - Graceful failure on invalid operations
        - Detailed error messages for debugging
        - Python exception compatibility
        
        Examples:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> 
            >>> # Enable learning subsystems
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
            >>> agent.enableDevelopment(True)
            >>> 
            >>> # Run agent simulation
            >>> for i in range(100):
            ...     world.update(0.1)
            ...     percept = world.getSensoryPercept()
            ...     agent.processSensoryInput(percept)
            ...     brain.step(i)
            ...     action = agent.decodeMotorCommand()
            ...     world.applyMotorCommand(action, world.getSimulationTime())
            ...     
            >>> # Check agent statistics
            >>> curiosity = agent.getCuriosityLevel()
            >>> novelty = agent.getNoveltyLevel()
            >>> prediction_error = agent.getPredictionError()
            >>> neuromodulation = agent.getNeuromodulationLevel()
            >>> 
            >>> # Access brain through agent
            >>> underlying_brain = agent.getBrain()
            >>> neurons = underlying_brain.getTotalNeuronCount()
        
        The agent brain provides:
        - Sensory integration and processing
        - Motor command generation and decoding
        - Neuromodulatory signal processing
        - Developmental stage tracking and updates
        - Learning mechanism control
        - Statistical monitoring and access
        - Error handling and validation
        
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &AgentBrainWrapper::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrainWrapper::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrainWrapper::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrainWrapper::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrainWrapper::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrainWrapper::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrainWrapper::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrainWrapper::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrainWrapper::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrainWrapper::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrainWrapper::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrainWrapper::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrainWrapper::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrainWrapper::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrainWrapper::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrainWrapper::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrainWrapper::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrainWrapper::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrainWrapper::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrainWrapper::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrainWrapper::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrainWrapper::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Register convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with standard NLM settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with the specified configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for agent simulation");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface for world interaction");

    // Register constant values for easy access
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register version information
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Enhanced NLM Python bindings with comprehensive error handling and documentation";
}

} // namespace nlm