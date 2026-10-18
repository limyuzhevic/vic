# python/bindings.cpp
// Main pybind11 module for NLM - Includes all modular bindings

#include "bindings_core_types.cpp"
#include "bindings_config.cpp"
#include "bindings_sensory.cpp"
#include "bindings_action.cpp"
#include "bindings_world.cpp"
#include "bindings_agent.cpp"
#include "bindings_brain.cpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides a complete Python interface to the NLM brain simulator,
        enabling creation and simulation of artificial neural systems with:
        
        Features:
        - Complete brain simulation with spiking neurons
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction and planning systems
        - Cognitive functions (attention, concept formation, planning)
        - Developmental and aging processes
        - World interaction and agent systems
        - Advanced plasticity mechanisms (STDP, Hebbian, structural)
        
        Key Classes:
        - Brain: Central neural simulation brain class
        - Config: Configuration management
        - AgentBrain: Agent brain interface connecting NLM brain to world
        - SimpleWorld: Simple 2D world for NLM simulation
        - Vision, Audio, InternalSignals: Sensory input types
        - Action: Motor action representation
        - SensoryPercept: Sensory percept data
        - WorldObject, AgentBody: State representations
        
        Usage Examples:
        
        Basic Brain Simulation:
        ```python
        import pynlm
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
        # Get statistics
        print(f"Neurons: {brain.getTotalNeuronCount()}")
        print(f"Spikes: {brain.getTotalSpikeCount()}")
        ```
        
        Complete Agent Example:
        ```python
        import pynlm
        
        def run_agent_simulation():
            # Setup
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run episode
            for step in range(500):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            return brain, agent, world
        ```
        
        See Also:
        - easy_usage.md: Beginner-friendly usage guide
        - HOW_TO_USE.md: Comprehensive usage documentation
        - docs/: Full technical documentation
    )pbdoc";

    // Register exception types from core types
    bindExceptionTypes(m);

    // Bind all classes and functions
    bindCoreTypes(m);
    bindConfig(m);
    bindSensoryInput(m);
    bindVision(m);
    bindAudio(m);
    bindInternalSignals(m);
    bindAction(m);
    bindAgentBody(m);
    bindActionResult(m);
    bindSensoryPercept(m);
    bindSimpleWorld(m);
    bindAgentBrain(m);
    bindBrain(m);

    // Register factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Register invalid IDs for backward compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add useful constants and utilities
    m.def("version", []() {
        return "pynlm 0.1.0 - NLM Python Bindings";
    }, "Get pynlm version information");

    // Advanced utility functions for experienced users
    m.def("batchBrainStep", [](std::shared_ptr<Brain> brain, size_t steps, 
                             bool withTimestamps = false) {
        for (size_t i = 0; i < steps; ++i) {
            if (withTimestamps) {
                brain->step(i, i * 0.001);
            } else {
                brain->step(i);
            }
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("with_timestamps") = false,
       "Efficiently run multiple brain steps");

    m.def("validateSensoryPercept", [](const SensoryPercept& percept) {
        // Basic validation - check for required fields
        auto vision = percept.getVision();
        auto internal = percept.getInternal();
        
        return !vision.getData().empty() || !internal.empty();
    }, py::arg("percept"), "Validate sensory percept structure");

    m.def("createValidatedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        // Basic validation
        if (!config->has("brain.neuron_count")) {
            throw nlm::ConfigurationError("Configuration must include brain.neuron_count");
        }
        if (!config->has("brain.synapse_density")) {
            throw nlm::ConfigurationError("Configuration must include brain.synapse_density");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw nlm::ConfigurationError("Brain initialization failed");
        }
        
        return brain;
    }, py::arg("config"), "Create and initialize brain with validation");

    // Performance and profiling utilities
    m.def("benchmarkBrainStep", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return std::make_tuple(
            duration.count(),  // total time in milliseconds
            steps / (duration.count() / 1000.0),  // steps per second
            brain->getTotalSpikeCount()  // total spikes
        );
    }, py::arg("brain"), py::arg("steps"), 
       "Benchmark brain step performance");

    // Configuration utilities
    m.def("getConfigurationSchema", []() {
        std::map<std::string, std::string> schema;
        schema["brain.neuron_count"] = "int: number of neurons (e.g., 100, 1000)";
        schema["brain.synapse_density"] = "float: connectivity density (0.0 to 1.0)";
        schema["brain.connection_probability"] = "float: connection probability (0.0 to 1.0)";
        schema["brain.initial_weight_mean"] = "float: mean initial synaptic weight";
        schema["brain.initial_weight_std"] = "float: std dev of initial weights";
        schema["plasticity.stdp.enable"] = "bool: enable STDP plasticity";
        schema["plasticity.hebbian.enable"] = "bool: enable Hebbian plasticity";
        schema["plasticity.structural.enable"] = "bool: enable structural plasticity";
        
        return schema;
    }, "Get configuration schema documentation");

    // World setup utilities
    m.def("createStandardWorld", [](size_t width, size_t height, 
                                   size_t visionWidth, size_t visionHeight,
                                   float maxEnergy) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
       py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
       py::arg("maxEnergy") = 100.0,
       "Create a standard world with default settings");

    // Agent setup utilities
    m.def("createCompleteAgent", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
             bool enableAllSystems = true) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        if (enableAllSystems) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enable_all_systems") = true,
       "Create a complete agent with all systems enabled");

    // Save/load utilities
    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, 
                              const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), 
       "Save brain state to file");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain,
                              const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from file");

    // Debug and diagnostic utilities
    m.def("getBrainDiagnostics", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> diagnostics;
        diagnostics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        diagnostics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        diagnostics["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
        diagnostics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        diagnostics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        diagnostics["avg_firing_rate"] = brain->getAverageFiringRate();
        diagnostics["ei_ratio"] = brain->getExcitationInhibitionRatio();
        
        return diagnostics;
    }, py::arg("brain"), "Get comprehensive brain diagnostics");

    m.def("simulateWorldEpisode", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
             std::shared_ptr<SimpleWorld> world, size_t steps,
             bool verbose = false) {
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                // Log progress (in real implementation, would use proper logging)
            }
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
       py::arg("verbose") = false,
       "Run a complete world simulation episode");

    // Register enumerations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .value("Modulatory", NeuronType::Modulatory)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Register helper classes for advanced users
    py::class_<std::pair<float, float>>(m, "Position2D")
        .def(py::init<>())
        .def(py::init<float, float>())
        .def_readwrite("x", &std::pair<float, float>::first)
        .def_readwrite("y", &std::pair<float, float>::second);

    // Version and info
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM Python Bindings - Neural Learning Machine";

} // namespace nlm

// Helper function implementation
double getExpectedSize(const Vision& vision) {
    return vision.getWidth() * vision.getHeight() * vision.getChannels();
}

// Custom comparator for sorting (if needed)
struct VisionDataComparator {
    bool operator()(const std::vector<float>& a, const std::vector<float>& b) const {
        return a.size() < b.size();
    }
};