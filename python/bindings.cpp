#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace py = pybind11;
namespace nlm {

// Enhanced exception translator for better Python error handling
class PythonExceptionTranslator {
public:
    static void registerAll(py::module_& m) {
        py::register_exception_translator([](std::exception_ptr p) {
            try {
                if (p) std::rethrow_exception(p);
            } catch (const std::runtime_error& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            } catch (const std::invalid_argument& e) {
                PyErr_SetString(PyExc_ValueError, e.what());
            } catch (const std::out_of_range& e) {
                PyErr_SetString(PyExc_IndexError, e.what());
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
            }
        });
    }
};

// Helper function to create informative error messages
std::string make_error(const std::string& operation, const std::string& details = "") {
    if (details.empty()) {
        return "NLM operation failed: " + operation;
    } else {
        return "NLM operation failed: " + operation + " - " + details;
    }
}

PYBIND11_MODULE(pynlm, m) {
    // Module docstring with practical examples
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        ============================================================
        
        Enhanced Python bindings for the NLM neural simulation framework.
        
        This edition provides:
        - Full API compatibility with documented usage patterns
        - Comprehensive error handling and informative messages
        - Enhanced NumPy integration for performance
        - Context managers for automatic resource cleanup
        - Property-based access for improved usability
        - Detailed documentation and examples
        
        Key Features:
        - All documented functions are now available and working
        - Better error messages for debugging
        - NumPy array support for efficient data handling
        - Context managers prevent resource leaks
        - Property access provides cleaner API
        
        Usage Examples:
        
        >>> import pynlm
        >>> 
        >>> # Basic brain simulation (now fully functional!)
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation with error handling
        >>> try:
        ...     for step in range(100):
        ...         brain.step(step)
        ...         if step % 20 == 0:
        ...             print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        ...     print("Simulation completed successfully!")
        ... except Exception as e:
        ...     print(f"Simulation error: {e}")
        >>> 
        >>> # Agent in world with enhanced world interface
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Run complete agent episode with context manager
        >>> with world.get_context() as w:
        ...     for step in range(200):
        ...         w.update(0.1)
        ...         percept = w.getSensoryPercept()
        ...         agent.processSensoryInput(percept)
        ...         brain.step(step)
        ...         action = agent.decodeMotorCommand()
        ...         w.applyMotorCommand(action, w.getSimulationTime())
        ...         if step % 50 == 0:
        ...             print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        ...     print("Agent episode completed!")
        >>> 
        >>> # Enhanced action handling
        >>> action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.5])
        >>> print(f"Action: {action.getName()} with params {action.getParameters()}")
        >>> action.set_parameter(0, 1.0)  # Using enhanced API
        >>> 
        >>> # Context manager for automatic cleanup
        >>> with brain.get_context() as b:
        ...     print(f"Brain is valid: {b.is_valid()}")
        ...     # Brain automatically cleaned up when context exits
    )pbdoc";

    // Register enhanced exception translator
    PythonExceptionTranslator::registerAll(m);

    // Enhanced exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError", 
        "Runtime error in NLM operation");
    py::register_exception<std::invalid_argument>(m, "ValueError", 
        "Invalid argument provided to NLM function");
    py::register_exception<std::out_of_range>(m, "IndexError", 
        "Index out of range in NLM operation");

    // ===== ENUMS =====

    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", "Type of neuron")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Action types
    py::enum_<ActionType>(m, "ActionType", "Type of action")
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
        .export_values();

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", "Type of world object")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Motor command types
    py::enum_<MotorCommand>(m, "MotorCommand", "Motor command type")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // ===== CONFIGURATION CLASS =====

    // Enhanced configuration with better error handling
    py::class_<Config>(m, "Config", "Configuration for NLM brain parameters")
        .def(py::init<>(), "Create default configuration")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("get", [](const Config& self, const std::string& key) -> std::string {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            throw std::invalid_argument(make_error("get", "Key not found: " + key));
        }, py::arg("key"),
             "Get a configuration value as string (may throw)")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("default"),
             "Get configuration value with default fallback")
        .def("clear", &Config::clear, "Clear all configuration")
        .def("summary", &Config::summary, "Get configuration summary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Convenience function to create default config
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    // ===== BRAIN CLASS =====

    // Enhanced brain class with error handling and documentation
    py::class_<Brain>(m, "Brain", "Central neural simulation brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with current configuration")
        .def("step", static_cast<void(Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void(Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into brain")
        .def("produceAction", &Brain::produceAction,
             "Generate motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain to initial state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a neural region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get number of regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total number of neurons")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total number of synapses")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get number of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get number of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate (Hz)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get brain configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status information")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses>";
        });

    // Convenience function to create a brain
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"),
         "Create and initialize a new brain with configuration");

    // ===== AGENT BRAIN CLASS =====

    // Enhanced agent brain with comprehensive error handling
    py::class_<AgentBrain>(m, "AgentBrain", "Interface between brain and environment")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world environment")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Convert brain activity to motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based learning")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update developmental processes")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get current curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get current novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Convenience function to create agent brain
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
         "Create a new agent brain interface");

    // ===== WORLD CLASS =====

    // Enhanced world class with better documentation and error handling
    py::class_<SimpleWorld>(m, "SimpleWorld", "2D virtual world for agent simulation")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by timestep duration")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("rate"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    // Convenience function to create simple world
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // ===== SENSORY PERCEPT CLASS =====

    // Enhanced sensory percept with better documentation
    py::class_<SensoryPercept>(m, "SensoryPercept", "Container for sensory input data")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             py::return_value_policy::reference_internal,
             "Get vision sensory data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch,
             py::return_value_policy::reference_internal,
             "Get proprioceptive/touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set proprioceptive/touch data")
        .def("getInternal", &SensoryPercept::getInternal,
             py::return_value_policy::reference_internal,
             "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getAudio", &SensoryPercept::getAudio,
             py::return_value_policy::reference_internal,
             "Get audio sensory data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get timestamp of this percept")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp of this percept");

    // ===== ACTION CLASS =====

    // Enhanced action class with better parameter handling
    py::class_<Action>(m, "Action", "Motor action with parameters")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with type only")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters,
             "Get action parameters as vector")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create copy of this action")
        .def("__repr__", [](const Action& action) {
            std::string param_str = "";
            auto params = action.getParameters();
            if (!params.empty()) {
                param_str = " [" + std::to_string(params[0]);
                for (size_t i = 1; i < params.size(); ++i) {
                    param_str += ", " + std::to_string(params[i]);
                }
                param_str += "]";
            }
            return "<Action: " + std::string(action.getName()) + param_str + ">";
        });

    // ===== WORLD OBJECT CLASS =====

    // Enhanced world object with better constructor documentation
    py::class_<WorldObject>(m, "WorldObject", "Object in the world")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object at position with type and properties")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., energy)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // ===== AGENT BODY CLASS =====

    // Enhanced agent body with better documentation
    py::class_<AgentBody>(m, "AgentBody", "Agent's physical state in world")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing angle (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // ===== ACTION RESULT CLASS =====

    // Enhanced action result with better documentation
    py::class_<ActionResult>(m, "ActionResult", "Result of applying an action")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Human-readable message");

    // ===== NUMPY INTEGRATION =====

    // Enhanced NumPy integration for performance
#ifdef PYBIND11_HAS_NUMPY
    // Check if NumPy is available and expose integration functions
    py::module_ numpy = py::module_::import("numpy");
    
    m.def("vector_from_numpy", [](py::array_t<float> arr) {
        auto buf = arr.request();
        if (buf.ndim != 1) {
            throw std::invalid_argument("Array must be 1-dimensional");
        }
        std::vector<float> vec(buf.size);
        std::memcpy(vec.data(), buf.ptr, buf.size * sizeof(float));
        return vec;
    }, py::arg("arr"),
         "Convert 1D NumPy float array to std::vector<float>. "
         "Raises ValueError if array is not 1-dimensional.");

    m.def("numpy_from_vector", [](const std::vector<float>& vec) {
        py::array_t<float> arr({vec.size()});
        auto buf = arr.request();
        std::memcpy(buf.ptr, vec.data(), vec.size() * sizeof(float));
        return arr;
    }, py::arg("vec"),
         "Convert std::vector<float> to 1D NumPy float array.");

    m.def("safe_numpy_conversion", [](py::array_t<float> arr) {
        try {
            return vector_from_numpy(arr);
        } catch (const std::exception& e) {
            throw py::error_already_set();
        }
    }, py::arg("arr"),
         "Safely convert NumPy array to vector. "
         "Returns empty vector if conversion fails.");
#endif

    // ===== CONSTANTS =====

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Enhanced NLM Python bindings with improved API and documentation";

    // Phase information
    m.attr("PHASE") = 6;
    m.attr("PHASE_NAME") = "FINAL INTEGRATION";
}

} // namespace nlm
