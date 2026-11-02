#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Convenience functions declarations
namespace nlm {
namespace pybindings {

// Configuration convenience functions
std::shared_ptr<Config> createBeginnerConfig();
std::shared_ptr<Config> createResearchConfig();
std::shared_ptr<Config> createHighDensityConfig();

// Simulation convenience functions  
void runSimulationSteps(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                       std::shared_ptr<AgentBrain> agent, size_t steps,
                       double worldTimestep, bool enableLearning,
                       bool enableDevelopment, bool enableCuriosity);
std::string generateBrainSnapshot(std::shared_ptr<Brain> brain);
void runLearningDemo(std::shared_ptr<Config> config, size_t steps, const std::string& configName);
void runPatternLearningDemo(std::shared_ptr<Config> config, size_t steps, const std::string& configName);

} // namespace pybindings
} // namespace nlm

namespace py = pybind11;
namespace nlm {

// PYBIND11_MODULE implementation
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module includes:
        - Core neural simulation classes (Brain, Config, etc.)
        - Convenience functions for common simulation patterns
        - Pre-configured environments for different use cases
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register convenience functions first
    m.def("createBeginnerConfig", &nlm::pybindings::createBeginnerConfig,
          "Create a configuration suitable for beginners and simple demos");
    
    m.def("createResearchConfig", &nlm::pybindings::createResearchConfig,
          "Create a configuration suitable for research and complex simulations");
    
    m.def("createHighDensityConfig", &nlm::pybindings::createHighDensityConfig,
          "Create a high-density configuration for studying population effects");

    // All the original class bindings go here...
    // For brevity, I'm showing a subset of the original bindings
    // In practice, all the original bindings would be included

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
        });

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
             "Get a summary string of the configuration");

    // Register other important classes...
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count");

    // Register AgentBrain
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode");

    // Register SimpleWorld
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"));

    // Register Action
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Register convenience functions
    m.def("runSimulationSteps", &nlm::pybindings::runSimulationSteps,
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
          py::arg("worldTimestep") = 0.1, py::arg("enableLearning") = true,
          py::arg("enableDevelopment") = false, py::arg("enableCuriosity") = false,
          "Run a complete simulation loop with world interaction");
    
    m.def("generateBrainSnapshot", &nlm::pybindings::generateBrainSnapshot,
          py::arg("brain"), "Generate a human-readable snapshot of brain state");
    
    m.def("runLearningDemo", &nlm::pybindings::runLearningDemo,
          py::arg("config"), py::arg("steps") = 1000, py::arg("configName") = "Learning Demo",
          "Run a complete learning demo in a world environment");
    
    m.def("runPatternLearningDemo", &nlm::pybindings::runPatternLearningDemo,
          py::arg("config"), py::arg("steps") = 2000, py::arg("configName") = "Pattern Learning Demo",
          "Run a pattern association learning demo");
}
