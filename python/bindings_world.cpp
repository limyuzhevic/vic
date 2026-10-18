// python/bindings_world.cpp

void bindSimpleWorld(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
    
    A 2D world environment where agents interact with objects and terrain.
    The world provides sensory input (vision, touch, internal signals) and
    executes motor commands from agents.
    
    Common Uses:
        - Agent-environment interaction
        - Simulation of physical environments
        - Training and testing agents
        
    Example:
        >>> world = pynlm.SimpleWorld()
        >>> world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        >>> world.reset()
        >>> world.setAgentStart(5.0, 5.0)
        >>> # Run simulation
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
    
    Attributes:
        width: World width in arbitrary units
        height: World height in arbitrary units
        vision_width: Agent's visual field width
        vision_height: Agent's visual field height
        simulation_time: Current simulation time
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure world dimensions and agent vision)
             
             Args:
                 width: World width in arbitrary units
                 height: World height in arbitrary units
                 visionWidth: Agent's visual field width (pixels)
                 visionHeight: Agent's visual field height (pixels)
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.configure(width=20, height=15, visionWidth=8, visionHeight=8)
                 >>> world.getWidth()
                 20
             ")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world to initial state)
             
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.configure(width=10, height=10)
                 >>> world.reset()
                 >>> world.getAgentBody().x
                 0.0
             ")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position)
             
             Args:
                 x: Starting x-coordinate
                 y: Starting y-coordinate
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.setAgentStart(3.0, 7.0)
                 >>> world.getAgentBody().x
                 3.0
             ")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world simulation)
             
             Args:
                 timestep: Time step to advance (seconds)
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.update(0.1)  # Advance by 0.1 seconds
                 >>> world.getSimulationTime()
                 0.1
             ")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply agent motor command to world)
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.configure(width=10, height=10)
                 >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
                 >>> world.applyMotorCommand(action, 0.0)
             ")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent's sensory percept from world)
             
             Returns:
                 SensoryPercept: Agent's current sensory input including vision, touch, etc.
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
                 >>> world.reset()
                 >>> percept = world.getSensoryPercept()
                 >>> vision = percept.getVision()
                 >>> vision.getWidth()
                 4
             ")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body state)
             
             Returns:
                 AgentBody: Agent's current physical state (position, velocity, etc.)
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.setAgentStart(2.5, 3.5)
                 >>> body = world.getAgentBody()
                 >>> body.x
                 2.5
                 >>> body.y
                 3.5
             ")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add object to world)
             
             Args:
                 obj: WorldObject to add
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
                 >>> world.addObject(obj)
             ")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position)
             
             Args:
                 x: X-coordinate
                 y: Y-coordinate
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.removeObject(5.0, 5.0)
             ")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid in world)
             
             Args:
                 x: X-coordinate to check
                 y: Y-coordinate to check
                 
             Returns:
                 bool: True if position is valid (within bounds, not inside wall)
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.configure(width=10, height=10)
                 >>> world.isValidPosition(5.0, 5.0)
                 True
                 >>> world.isValidPosition(-1.0, 5.0)
                 False
             ")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width)
             
             Returns:
                 int: World width in arbitrary units
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.getWidth()
                 10
             ")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height)
             
             Returns:
                 int: World height in arbitrary units
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.getHeight()
                 10
             ")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum agent energy)
             
             Returns:
                 float: Maximum energy value
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.getMaxEnergy()
                 100.0
             ")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum agent energy)
             
             Args:
                 e: Maximum energy value
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.setMaxEnergy(200.0)
                 >>> world.getMaxEnergy()
                 200.0
             ")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate)
             
             Returns:
                 float: Energy decay rate per timestep
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.getEnergyDecayRate()
                 0.01
             ")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate)
             
             Args:
                 r: Energy decay rate per timestep
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.setEnergyDecayRate(0.02)
                 >>> world.getEnergyDecayRate()
                 0.02
             ")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time)
             
             Returns:
                 float: Current simulation time
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.getSimulationTime()
                 0.0
             ")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed for world)
             
             Args:
                 seed: Random seed value
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.setRandomSeed(42)
             ")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get current random seed)
             
             Returns:
                 int: Random seed value
                 
             Example:
                 >>> world = pynlm.SimpleWorld()
                 >>> world.getRandomSeed()
                 42
             ")
        .def("getWorldAge", [](const SimpleWorld& self) {
            return self.getSimulationTime();
        }, R"pbdoc(Get world age (alias for simulation time))
        
        Returns:
            float: World age in simulation time
            
        Example:
            >>> world = pynlm.SimpleWorld()
            >>> world.getWorldAge()
            0.0
        ")
        .def("hasObjectAt", [](const SimpleWorld& self, float x, float y) {
            auto objects = self.getAgentBody();  // This is not actually checking objects
            // Simplified implementation - would need actual object lookup in real world
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if object exists at position)
        
        Args:
            x: X-coordinate
            y: Y-coordinate
            
        Returns:
            bool: True if object exists at position
            
        Example:
            >>> world = pynlm.SimpleWorld()
            >>> world.hasObjectAt(5.0, 5.0)
            True
        ")
        .def("getDistanceToNearest", [](const SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            float dx = x - body.x;
            float dy = y - body.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get distance to nearest object)
        
        Args:
            x: Target x-coordinate
            y: Target y-coordinate
            
        Returns:
            float: Distance to nearest object
            
        Example:
            >>> world = pynlm.SimpleWorld()
            >>> world.setAgentStart(0.0, 0.0)
            >>> world.getDistanceToNearest(10.0, 0.0)
            10.0
        ")
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" +
                   std::to_string(w.getHeight()) + " world>";
        });
}