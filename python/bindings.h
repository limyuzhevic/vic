#include <pybind11/pybind11.h>

// Simple modular binding that delegates to individual binding files
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Modular binding organization
        - Pythonic API design with type hints
        - Context managers and resource management
        - Better error handling and documentation
        - Factory methods for common use cases
    )pbdoc";

    // Version information
    m.attr("__version__") = "0.2.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM neural simulation framework";
    m.attr("__license__") = "MIT";
    m.attr("__website__") = "https://github.com/nlm-project/nlm";

    // Basic bindings - enhanced versions will be added when files are compiled
    bind_basic_types(m);
    bind_enumerations(m);
    bind_config(m);
    bind_sensory(m);
    bind_motor(m);
    bind_world(m);
    bind_brain(m);
    bind_agent(m);
    bind_factory(m);
    
    // Register enhanced exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "LogicError");
    
    // Module-level helper functions
    m.def("get_version_info", []() {
        return py::dict(
            "version", m.attr("__version__"),
            "author", m.attr("__author__"),
            "description", m.attr("__description__"),
            "license", m.attr("__license__"),
            "website", m.attr("__website__")
        );
    }, "Get comprehensive version information");
    
    m.def("quick_start", []() {
        std::cout << "=== NLM Quick Start Guide ===\n";
        std::cout << "1. Create brain: brain = pynlm.createBrain(pynlm.createDefaultConfig())\n";
        std::cout << "2. Initialize: brain.initialize()\n";
        std::cout << "3. Configure world: world.configure(width, height, vision_width, vision_height)\n";
        std::cout << "4. Create agent: agent = pynlm.createAgentBrain(brain)\n";
        std::cout << "5. Run simulation: agent.run_episode(world, steps)\n";
        std::cout << "\nFor detailed documentation, see HOW_TO_USE.md and docs/\n";
        return py::dict();
    }, "Get quick start guide for NLM usage");
}

void bind_basic_types(py::module_ &m) {
    // Basic ID types - simplified for now
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value);
    
    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value);
    
    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value);
}

void bind_enumerations(py::module_ &m) {
    // Basic enumerations - will be enhanced with more details
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Interact", ActionType::Interact)
        .value("Wait", ActionType::Wait)
        .export_values();
}

void bind_config(py::module_ &m) {
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("set", &Config::set, py::arg("key"), py::arg("value"))
        .def("get", &Config::get, py::arg("key"), py::arg("default"), py::arg("nullptr"))
        .def("has", &Config::has, py::arg("key"))
        .def("summary", &Config::summary);
}

void bind_sensory(py::module_ &m) {
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions);
    
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight);
}

void bind_motor(py::module_ &m) {
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def("getType", &Action::getType)
        .def("getName", &Action::getName);
}

void bind_world(py::module_ &m) {
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure)
        .def("reset", &SimpleWorld::reset)
        .def("update", &SimpleWorld::update)
        .def("getAgentBody", &SimpleWorld::getAgentBody);
}

void bind_brain(py::module_ &m) {
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", &Brain::step)
        .def("produceAction", &Brain::produceAction)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount);
}

void bind_agent(py::module_ &m) {
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput)
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("run_episode", &AgentBrain::run_episode);
}

void bind_factory(py::module_ &m) {
    m.def("createDefaultConfig", []() { return std::make_shared<Config>(); });
    m.def("createBrain", [](std::shared_ptr<Config> config) { return std::make_shared<Brain>(config); });
    m.def("createSimpleWorld", []() { return std::make_shared<SimpleWorld>(); });
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) { return std::make_shared<AgentBrain>(brain); });
}
