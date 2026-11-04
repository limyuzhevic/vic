// World bindings
void bind_world(py::module_& m) {
    m.doc() = "World bindings for NLM"; 
    
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset the world state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        // Pythonic convenience methods
        .def("create_vision", [](SimpleWorld& self, size_t width, size_t height, size_t channels = 3) {
            // This would need Vision class to be exposed in sensory bindings
            // For now, placeholder
            return "Vision not yet exposed in this binding";
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           "Create vision sensor (placeholder)")
        .def("create_audio", [](SimpleWorld& self, size_t sampleRate, size_t numSamples) {
            // This would need Audio class to be exposed in sensory bindings
            // For now, placeholder
            return "Audio not yet exposed in this binding";
        }, py::arg("sampleRate"), py::arg("numSamples"),
           "Create audio sensor (placeholder)")
        .def("get_stats", [](SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            stats["simulation_time"] = self.getSimulationTime();
            stats["random_seed"] = self.getRandomSeed();
            return stats;
        }, "Get world statistics");
}

define_bind_world() {
    // This function exists for backward compatibility but is no longer needed
}