// python/bindings_core_types.cpp
// Core types and ID classes - Eliminates duplication with template
template<typename IDClass, typename IndexFunc>
void bindIDClass(py::module_& m, const std::string& name) {
    py::class_<IDClass>(m, name.c_str(), R"pbdoc(Unique identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDClass::value)
        .def("index", IndexFunc())
        .def("__eq__", &IDClass::operator==)
        .def("__ne__", &IDClass::operator!=)
        .def("__hash__", [](const IDClass& id) {
            return std::hash<uint64_t>{}(id.value);
        })
        .def("__repr__", [](const IDClass& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

void bindCoreTypes(py::module_& m) {
    // ID classes using template to eliminate duplication
    bindIDClass<NeuronId>(m, "NeuronId");
    bindIDClass<SynapseId>(m, "SynapseId");
    bindIDClass<RegionId>(m, "RegionId");
    bindIDClass<PopulationId>(m, "PopulationId");

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

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
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Additional basic types
    py::class_<SimulationStep>(m, "SimulationStep", R"pbdoc(Simulation step identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t>(), py::arg("value"))
        .def_readwrite("value", &SimulationStep::value);

    py::class_<Timestamp>(m, "Timestamp", R"pbdoc(Simulation timestamp)pbdoc")
        .def(py::init<>())
        .def(py::init<double>(), py::arg("value"))
        .def_readwrite("value", &Timestamp::value);
}

// Register exception types
void bindExceptionTypes(py::module_& m) {
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Custom NLM exceptions
    py::class_<nlm::NLMException>(m, "NLMError", R"pbdoc(Base NLM exception)pbdoc")
        .def(py::init<const std::string&>());

    py::class_<nlm::InvalidParameterError>(m, "InvalidParameterError", R"pbdoc(Invalid parameter error)pbdoc")
        .def(py::init<const std::string&, const std::string&>());

    py::class_<nlm::ConfigurationError>(m, "ConfigurationError", R"pbdoc(Configuration error)pbdoc")
        .def(py::init<const std::string&>());
}