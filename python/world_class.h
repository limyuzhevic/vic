// NLM (Neural Learning Machine) Python Bindings - World Class
// This file contains bindings for the SimpleWorld class

namespace py = pybind11;

void bindWorldClass(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The SimpleWorld class provides a 2D simulation environment with agents,
        objects, and sensory-motor interfaces. It implements the world interface
        for NLM brain simulation, handling:
        
        - Agent body state (position, orientation, velocity, health)
        - Object management (resources, hazards, walls, markers)
        - Sensory input (vision, touch, proprioception, audio)
        - Motor output (actions and their results)
        - World dynamics and state management
        
        Example usage:
        ```python
        import pynlm
        
        # Create world
        world = SimpleWorld()
        
        # Configure world dimensions
        world.configure(width=100, height=100, vision_width=20, vision_height=20)
        
        # Create agent brain
        brain = createBrain(createDefaultConfig())
        agent_brain = createAgentBrain(brain)
        
        # Initialize brain with world
        agent_brain.initialize(world)
        
        # Set agent start position
        world.setAgentStart(50, 50)
        
        # Run simulation loop
        for timestep in range(1000):
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process with agent brain
            action = agent_brain.decodeMotorCommand()
            
            # Apply action in world
            result = world.applyMotorCommand(action, timestep)
            
            # Update world state
            world.update(timestep)
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
}