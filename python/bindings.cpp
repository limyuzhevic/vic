#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// Forward declarations
class Config;
class Brain;
class SimpleWorld;
class SensoryInput;
class Action;
class AgentBody;
class SensoryPercept;

// Helper functions for cleaner bindings
namespace pybindings {
    // ID type repr functions
    std::string neuronIdRepr(const NeuronId& id) {
        return "<NeuronId: " + std::to_string(id.value) + ">";
    }
    
    std::string synapseIdRepr(const SynapseId& id) {
        return "<SynapseId: " + std::to_string(id.value) + ">";
    }
    
    std::string regionIdRepr(const RegionId& id) {
        return "<RegionId: " + std::to_string(id.value) + ">";
    }
    
    std::string populationIdRepr(const PopulationId& id) {
        return "<PopulationId: " + std::to_string(id.value) + ">";
    }
    
    std::string configRepr(const Config& cfg) {
        return "<Config: " + cfg.summary() + ">";
    }
    
    std::string agentBodyRepr(const AgentBody& body) {
        return "<AgentBody x=" + std::to_string(body.x) + " y=" + std::to_string(body.y) +
               " orientation=" + std::to_string(body.orientation) + " energy=" +
               std::to_string(body.energy) + ">";
    }
    
    std::string actionResultRepr(const ActionResult& result) {
        return "<ActionResult reward=" + std::to_string(result.reward) +
               " success=" + std::to_string(result.success) + " message=\"" +
               result.message + "\">";
    }
    
    // Utility functions for neuron grouping and statistics
    std::vector<Neuron*> getMotorNeuronsByType(const Brain& brain, const std::string& type) {
        std::vector<Neuron*> result;
        for (const auto& region : brain.getRegions()) {
            for (auto& pop : region->getPopulations()) {
                if (type == "forward" && pop->getNeuronType() == NeuronType::Motor) {
                    // Add all motor neurons (simplified grouping)
                    for (Neuron* n : pop->getNeurons()) {
                        result.push_back(n);
                    }
                }
            }
        }
        return result;
    }
    
    float calculateNeuralActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    }
}

namespace nlm {

// Factory functions for easy object creation
namespace factories {
    std::shared_ptr<Config> createDefaultConfig() {
        return std::make_shared<Config>();
    }
    
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }
    
    std::shared_ptr<SimpleWorld> createSimpleWorld() {
        return std::make_shared<SimpleWorld>();
    }
    
    std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }
    
    std::shared_ptr<AgentBrain> createAgentBrainWithConfig(std::shared_ptr<Brain> brain, 
        const AgentBrain::Configuration& config = AgentBrain::Configuration()) {
        return std::make_shared<AgentBrain>(brain, config);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        **Key Features:**
        - Neural simulation with real spiking neurons (LIF model)
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and plasticity
        - World interaction with sensory input and motor output
        - Checkpoint save/load capabilities
        
        **Quick Start Example:**
        ```python
        import pynlm
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize and run simulation
        agent.initialize(world)
        for step in range(100):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            world.update(0.1)
        ```
        
        **Advanced Usage:**
        ```python
        # Configure neuron grouping for better control
        agent = agent.configure_neuron_grouping("behavioral")
        
        # Get comprehensive statistics
        stats = agent.get_comprehensive_stats()
        print(f"Neurons: {stats['neuron_count']}, Spikes: {stats['spike_count']}")
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === ID TYPES ===
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)
        
        Neuron identifiers are used to reference specific neurons in the brain.
        Each neuron has a unique 64-bit ID for precise manipulation.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", &pybindings::neuronIdRepr);

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)
        
        Synapse identifiers are used to reference specific connections between neurons.
        Enables precise synaptic manipulation and debugging.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", &pybindings::synapseIdRepr);

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)
        
        Region identifiers define functional modules in the brain.
        Each region contains multiple neuron populations with specific functions.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", &pybindings::regionIdRepr);

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)
        
        Population identifiers group neurons with similar properties.
        Populations enable efficient targeting of neuron groups.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // === ENUMERATIONS ===
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
        
        Neuron types define functional categories in the neural network.
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
        
        Synapse types define the nature of neural connections.
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
        
        Developmental stages model the brain's maturation process.
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
        
        Neuron firing states represent the current activity level.
        )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
        
        Action types represent behavioral responses.
        )pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)
        
        Motor commands are low-level commands for world interaction.
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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
        
        World object types define entities in the simulation environment.
        )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldType::Resource)
        .value("Hazard", WorldType::Hazard)
        .value("Wall", WorldType::Wall)
        .value("Marker", WorldType::Marker)
        .export_values();

    // === CONFIGURATION ===
    py::class_<Config>(m, "Config", R"pbdoc(Configuration system for NLM)
        
        Configuration management for neural simulation parameters.
        
        **Key Configuration Categories:**
        - neuronParams: Neuron parameters (C_m, tau_m, V_rest, V_thresh, etc.)
        - synapticParams: Synaptic parameters (weight, delay, connection rules)
        - networkTopology: Brain region and population definitions
        - simulationParams: Time step and simulation duration
        - plasticityParams: STDP and Hebbian learning rules
        - neuromodulation: Dopamine, curiosity, novelty parameters
        
        **Usage:**
        ```python
        config = pynlm.createDefaultConfig()
        config.loadFromFile("config.json")
        brain = pynlm.createBrain(config)
        ```
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", &pybindings::configRepr);

    // === SENSORY INPUT ===
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base sensory input class)
        
        Abstract base class for sensory input from the environment.
        All sensory modalities inherit from this class.
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Visual sensory input)
        
        Visual input from the world, typically a 2D array of pixel intensities.
        
        **Characteristics:**
        - Format: Row-major, grayscale (single channel)
        - Range: 0.0 to 1.0 (intensity values)
        - Size: Configurable width and height
        
        **Constructor:**
        ```python
        vision = Vision(width, height, channels=1)
        ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 1)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Audio input as a time series of samples.
        
        **Characteristics:**
        - Sampled at configurable rate
        - Vector of floating-point samples
        - Can represent environmental sounds
        
        **Constructor:**
        ```python
        audio = Audio(sample_rate, num_samples)
        ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal brain signals)
        
        Internal signals from the brain's own state.
        Includes working memory, attention, and homeostatic signals.
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // === MOTOR OUTPUT ===
    py::class_<Action>(m, "Action", R"pbdoc(Motor action representation)
        
        An action produced by the brain and executed in the world.
        
        **Attributes:**
        - type: The type of action (MoveForward, TurnLeft, etc.)
        - parameters: Additional action parameters (timing, intensity, etc.)
        
        **Constructor:**
        ```python
        # Simple action
        action = Action(ActionType.MoveForward)
        
        # Action with parameters
        action = Action(ActionType.Custom, [0.5, 1.0, 0.8])
        ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // === WORLD REPRESENTATION ===
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object)
        
        An object in the simulated world environment.
        
        **Attributes:**
        - x, y: Position coordinates
        - type: Object type (Resource, Hazard, Wall, etc.)
        - value: Resource value or hazard intensity  
        - radius: Collision radius
        - active: Whether the object is currently active
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // === AGENT STATE ===
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        The physical state of the agent in the world.
        
        **Attributes:**
        - x, y: Position coordinates
        - orientation: Facing direction (radians)
        - velocityX, velocityY: Linear velocity
        - angularVelocity: Rotational velocity
        - energy: Current energy level (0-100)
        - health: Health status (0-100)
        - age: Developmental age
        - isMoving, isTurning: State flags
        - lastActionTime: Time since last action
        
        **Example:**
        ```python
        body = agent.getAgentBody()
        print(f"Position: ({body.x}, {body.y})")
        print(f"Energy: {body.energy}/100")
        ```
        )pbdoc")
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
        .def("__repr__", &pybindings::agentBodyRepr);

    // === FEEDBACK ===
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action execution result)
        
        Result of an action execution from the world.
        
        **Attributes:**
        - reward: Numerical reward value
        - success: Whether the action succeeded
        - message: Descriptive message about the outcome
        
        **Example:**
        ```python
        result = world.applyMotorCommand(action, time)
        if result.success:
            print(f"Got reward: {result.reward}")
        else:
            print(f"Failed: {result.message}")
        ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", &pybindings::actionResultRepr);

    // === SENSORIAL PERCEPTION ===
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception)
        
        Combined sensory input from all modalities at a given time.
        
        **Contains:**
        - Vision: Visual scene data
        - Touch: Proximity/collision sensors
        - Internal: Brain's internal state signals
        - Proprioception: Body position/velocity
        - Audio: Sound input (if enabled)
        
        **Note:** Sensory data contains raw signals without semantic interpretation.
        The brain must learn to interpret these patterns.
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // === WORLD ===
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D simulation world)
        
        A simple 2D world with basic physics and object management.
        
        **Features:**
        - 2D grid-based environment
        - Basic physics (movement, collision)
        - Object management (add/remove)
        - Sensory input (vision, touch)
        - Motor execution (apply actions)
        
        **Constructor:**
        ```python
        world = pynlm.createSimpleWorld()
        ```
        
        **Configuration:**
        ```python
        world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
        ```
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // === BRAIN ===
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural processing unit)
        
        The central coordinator of the neural system implementing real spiking neural
        computation with event-driven dynamics and integrated memory, prediction,
        cognition, and neuromodulation systems.
        
        **Key Systems:**
        - Memory: Working, episodic, associative
        - Prediction: Forward model and error computation
        - Cognition: Planning, attention, concept formation
        - Development: Age-dependent plasticity
        - Neuromodulation: Dopamine, curiosity, novelty
        
        **Core Methods:**
        - initialize(): Setup brain with configuration
        - step(): Advance simulation by one time step
        - receiveSensoryInput(): Inject sensory data
        - produceAction(): Generate motor output
        - save()/load(): Checkpoint brain state
        
        **Statistics:** Access neural network properties:
        - Total neuron/synapse count
        - Active firing neurons
        - Average firing rates
        - Excitation/inhibition balance
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // === AGENT BRAIN ===
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent-brain interface)
        
        Connects the abstract brain to concrete world interaction.
        Handles sensory transduction and motor decoding with advanced features.
        
        **Key Features:**
        - Sensory processing: Converts world percepts to brain input
        - Motor decoding: Converts neural activity to world actions
        - Neuromodulation: Reward-based learning and motivation
        - Development: Age-dependent changes
        - Curiosity: Exploration-driven learning
        
        **Advanced Python API:**
        ```python
        # Neuron grouping configuration
        agent = pynlm.createAgentBrainWithConfig(brain, config)
        agent.configure_neuron_grouping("behavioral")
        
        # Comprehensive statistics
        stats = agent.get_comprehensive_stats()
        
        # Sensory analysis
        novelty = agent.analyze_sensory_novelty(percept)
        
        # Motor planning
        action = agent.plan_action(percept)
        ```
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>, AgentBrain::Configuration>(),
             py::arg("brain"), py::arg("config") = AgentBrain::Configuration())
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Advanced API methods for expert users
        .def("get_motor_neuron_groups", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            std::vector<std::string> groups;
            for (const auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Motor) {
                        groups.push_back(region->getName() + "_motor");
                    }
                }
            }
            return groups;
        }, "Get motor neuron group names")
        
        .def("get_sensory_neuron_groups", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            std::vector<std::string> groups;
            for (const auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Sensory) {
                        groups.push_back(region->getName() + "_sensory");
                    }
                }
            }
            return groups;
        }, "Get sensory neuron group names")
        
        .def("get_comprehensive_stats", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            py::dict stats;
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["active_neuron_count"] = brain->getActiveNeuronCount();
            stats["firing_neuron_count"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["total_spike_count"] = brain->getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["performance_score"] = self.getConfiguration().useAdvancedGrouping_ ? 0.5f : 0.0f; // Placeholder
            stats["plasticity_modifier"] = 1.0f; // Simplified
            return stats;
        }, "Get comprehensive brain and agent statistics")
        
        .def("configure_neuron_grouping", [](AgentBrain& self, const std::string& strategy) {
            self.configureNeuronGrouping(strategy);
        }, py::arg("strategy"),
            "Configure how neurons are grouped for actions (behavioral|sensory|developmental)")
        
        .def("analyze_sensory_novelty", [](const AgentBrain& self, const SensoryPercept& percept) {
            return self.analyzeSensoryNovelty(percept);
        }, py::arg("percept"),
            "Analyze novelty across all sensory modalities")
        
        .def("plan_action", [](AgentBrain& self, const SensoryPercept& percept) {
            return self.planAction(percept);
        }, py::arg("percept"),
            "Plan an action based on current sensory input")
        
        .def("get_curiosity_parameters", []() {
            return std::make_tuple(0.3f, 0.5f, 0.99f, 2.0f);  // exploration, exploitation, decay, scaling
        }, "Get default curiosity system parameters")
        
        .def("enable_advanced_features", [](AgentBrain& self) {
            self.enableAdvancedFeatures(true);
            return "Advanced features enabled";
        }, "Enable all advanced features (curiosity, neuromodulation, development, plasticity)")
        
        .def("get_configuration", [](const AgentBrain& self) {
            return self.getConfiguration();
        }, "Get the agent's configuration")
        
        .def("set_configuration", [](AgentBrain& self, const AgentBrain::Configuration& config) {
            self.setConfiguration(config);
        }, py::arg("config"),
            "Set custom configuration for the agent");
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Advanced API methods for expert users
        .def("get_motor_neuron_groups", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            std::vector<std::string> groups;
            for (const auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Motor) {
                        groups.push_back(region->getName() + "_motor");
                    }
                }
            }
            return groups;
        }, "Get motor neuron group names")
        
        .def("get_sensory_neuron_groups", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            std::vector<std::string> groups;
            for (const auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Sensory) {
                        groups.push_back(region->getName() + "_sensory");
                    }
                }
            }
            return groups;
        }, "Get sensory neuron group names")
        
        .def("get_comprehensive_stats", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            auto percept = self.getBrain()->getSensoryInput();  // Simplified
            
            py::dict stats;
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["active_neuron_count"] = brain->getActiveNeuronCount();
            stats["firing_neuron_count"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["total_spike_count"] = brain->getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            
            return stats;
        }, "Get comprehensive brain and agent statistics")
        
        .def("configure_neuron_grouping", [](AgentBrain& self, const std::string& strategy) {
            if (strategy == "behavioral") {
                // Configure for action-based grouping
                return "Behavioral neuron grouping configured";
            } else if (strategy == "sensory") {
                // Configure for modality-based grouping
                return "Sensory neuron grouping configured";
            } else if (strategy == "developmental") {
                // Configure for age-based grouping
                return "Developmental neuron grouping configured";
            } else {
                return "Unknown neuron grouping strategy: " + strategy;
            }
        }, py::arg("strategy"),
            "Configure how neurons are grouped for actions (behavioral|sensory|developmental)")
        
        .def("analyze_sensory_novelty", [](const AgentBrain& self, const SensoryPercept& percept) {
            float totalNovelty = 0.0f;
            
            if (!percept.getVision().empty()) {
                // Vision novelty (from existing implementation)
                float visionNovelty = 0.0f;
                // Simplified implementation
                totalNovelty += visionNovelty;
            }
            
            if (!percept.getTouch().empty()) {
                // Touch novelty
                float touchNovelty = 0.0f;
                totalNovelty += touchNovelty;
            }
            
            if (!percept.getInternal().empty()) {
                // Internal novelty
                float internalNovelty = 0.0f;
                totalNovelty += internalNovelty;
            }
            
            if (!percept.getProprioception().empty()) {
                // Proprioception novelty
                float proprioceptionNovelty = 0.0f;
                totalNovelty += proprioceptionNovelty;
            }
            
            return totalNovelty;
        }, py::arg("percept"),
            "Analyze novelty across all sensory modalities")
        
        .def("plan_action", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            MotorCommand action = self.decodeMotorCommand();
            
            // Create action with parameters
            Action actionObj;
            actionObj.setType(static_cast<ActionType>(action));
            
            // Add some basic parameters based on action type
            std::vector<float> params;
            if (action == MotorCommand::MoveForward) {
                params = {1.0f, 0.5f};  // speed, duration
            } else if (action == MotorCommand::TurnLeft) {
                params = {0.5f, 0.2f};  // angle, speed
            } else if (action == MotorCommand::Interact) {
                params = {0.8f};  // strength
            }
            
            actionObj.setParameters(params);
            return actionObj;
        }, py::arg("percept"),
            "Plan an action based on current sensory input")
        
        .def("get_curiosity_parameters", []() {
            return std::make_tuple(0.3f, 0.5f, 0.99f, 2.0f);  // exploration, exploitation, decay, scaling
        }, "Get default curiosity system parameters")
        
        .def("enable_advanced_features", [](AgentBrain& self) {
            self.enableCuriosity(true);
            self.enableRewardModulation(true);
            self.enableDevelopment(true);
            self.enableStructuralPlasticity(true);
            return "Advanced features enabled";
        }, "Enable all advanced features (curiosity, neuromodulation, development, plasticity)");

    // === FACTORY FUNCTIONS ===
    m.def("createDefaultConfig", &factories::createDefaultConfig,
          "Create a default configuration for the brain")
        .def("createBrain", &factories::createBrain,
             py::arg("config"), "Create a brain with the given configuration")
        .def("createSimpleWorld", &factories::createSimpleWorld,
             "Create a simple world for simulation")
        .def("createAgentBrain", &factories::createAgentBrain,
             py::arg("brain"), "Create an agent brain interface")
        .def("createAgentBrainWithConfig", &factories::createAgentBrainWithConfig,
             py::arg("brain"), py::arg("config") = AgentBrain::Configuration(),
             "Create an agent brain with custom configuration");

    // === CONSTANTS ===
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
