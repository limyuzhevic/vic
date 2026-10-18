// python/bindings_agent.cpp

void bindAgentBody(py::module_& m) {
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
    
    Represents the physical state of an agent in the world.
    Includes position, velocity, orientation, and other body properties.
    
    Common Uses:
        - Agent physics simulation
        - State tracking and monitoring
        - Motion planning input
        
    Example:
        >>> agent_body = pynlm.AgentBody()
        >>> agent_body.x = 5.0
        >>> agent_body.y = 3.0
        >>> agent_body.orientation = 0.5
        >>> agent_body.energy = 80.0
    
    Attributes:
        x: X-coordinate position
        y: Y-coordinate position
        orientation: Heading angle in radians
        velocityX: Velocity in x-direction
        velocityY: Velocity in y-direction
        angularVelocity: Angular velocity (rad/s)
        energy: Current energy level
        health: Health status (0.0 to 1.0)
        age: Agent age in simulation time
        isMoving: Whether agent is currently moving
        isTurning: Whether agent is currently turning
        lastActionTime: Time since last action
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
             R"pbdoc(X-coordinate position)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.x = 10.0
                 >>> body.x
                 10.0
             ")
        .def_readwrite("y", &AgentBody::y,
             R"pbdoc(Y-coordinate position)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.y = 5.0
                 >>> body.y
                 5.0
             ")
        .def_readwrite("orientation", &AgentBody::orientation,
             R"pbdoc(Heading angle in radians)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.orientation = 1.57  # π/2 radians (90 degrees)
                 >>> body.orientation
                 1.57
             ")
        .def_readwrite("velocityX", &AgentBody::velocityX,
             R"pbdoc(Velocity in x-direction)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.velocityX = 2.0
                 >>> body.velocityX
                 2.0
             ")
        .def_readwrite("velocityY", &AgentBody::velocityY,
             R"pbdoc(Velocity in y-direction)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.velocityY = -1.5
                 >>> body.velocityY
                 -1.5
             ")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
             R"pbdoc(Angular velocity in radians per second)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.angularVelocity = 0.5
                 >>> body.angularVelocity
                 0.5
             ")
        .def_readwrite("energy", &AgentBody::energy,
             R"pbdoc(Current energy level)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.energy = 75.0
                 >>> body.energy
                 75.0
             ")
        .def_readwrite("health", &AgentBody::health,
             R"pbdoc(Health status (0.0 to 1.0))
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.health = 0.8
                 >>> body.health
                 0.8
             ")
        .def_readwrite("age", &AgentBody::age,
             R"pbdoc(Agent age in simulation time)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.age = 10.5
                 >>> body.age
                 10.5
             ")
        .def_readwrite("isMoving", &AgentBody::isMoving,
             R"pbdoc(Whether agent is currently moving)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.isMoving = True
                 >>> body.isMoving
                 True
             ")
        .def_readwrite("isTurning", &AgentBody::isTurning,
             R"pbdoc(Whether agent is currently turning)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.isTurning = False
                 >>> body.isTurning
                 False
             ")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
             R"pbdoc(Time since last action)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.lastActionTime = 2.5
                 >>> body.lastActionTime
                 2.5
             ")
        .def("reset", &AgentBody::reset,
             R"pbdoc(Reset agent body to initial state)
             
             Example:
                 >>> body = pynlm.AgentBody()
                 >>> body.x = 10.0
                 >>> body.reset()
                 >>> body.x
                 0.0
             ")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, R"pbdoc(Get agent position as (x, y) tuple)
        
        Returns:
            tuple: (x, y) position coordinates
            
        Example:
            >>> body = pynlm.AgentBody()
            >>> body.x = 3.0
            >>> body.y = 4.0
            >>> pos = body.getPosition()
            >>> pos[0]
            3.0
            >>> pos[1]
            4.0
        ")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Set agent position)
        
        Args:
            x: X-coordinate
            y: Y-coordinate
            
        Example:
            >>> body = pynlm.AgentBody()
            >>> body.setPosition(5.0, 7.0)
            >>> body.x
            5.0
            >>> body.y
            7.0
        ")
        .def("getDistanceTo", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get distance to target position)
        
        Args:
            x: Target x-coordinate
            y: Target y-coordinate
            
        Returns:
            float: Distance to target
            
        Example:
            >>> body = pynlm.AgentBody()
            >>> body.x = 0.0
            >>> body.y = 0.0
            >>> body.getDistanceTo(3.0, 4.0)
            5.0
        ")
        .def("isNear", [](const AgentBody& self, float x, float y, float threshold) {
            return self.getDistanceTo(x, y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), R"pbdoc(Check if near target position)
        
        Args:
            x: Target x-coordinate
            y: Target y-coordinate
            threshold: Distance threshold
            
        Returns:
            bool: True if within threshold distance
            
        Example:
            >>> body = pynlm.AgentBody()
            >>> body.x = 0.0
            >>> body.y = 0.0
            >>> body.isNear(2.0, 0.0, 3.0)  # Within 3 units
            True
            >>> body.isNear(10.0, 0.0, 3.0)  # Beyond threshold
            False
        ")
        .def("moveToward", [](AgentBody& self, float x, float y, float speed) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance > 0.0f) {
                float scale = speed / distance;
                self.x += dx * scale;
                self.y += dy * scale;
                self.velocityX = dx * scale;
                self.velocityY = dy * scale;
                self.isMoving = true;
            }
        }, py::arg("x"), py::arg("y"), py::arg("speed"), R"pbdoc(Move toward target position)
        
        Args:
            x: Target x-coordinate
            y: Target y-coordinate
            speed: Movement speed per timestep
            
        Example:
            >>> body = pynlm.AgentBody()
            >>> body.x = 0.0
            >>> body.y = 0.0
            >>> body.moveToward(10.0, 0.0, 1.0)  # Move 1 unit toward target
            >>> body.x
            1.0
        ")
        .def("__repr__", [](const AgentBody& b) {
            return "<AgentBody: pos(" + std::to_string(b.x) + "," + std::to_string(b.y) + ") energy=" + std::to_string(b.energy) + ">";
        });
}

void bindActionResult(py::module_& m) {
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
    
    Result of applying an agent action in the world.
    Contains reward information, success status, and any feedback messages.
    
    Common Uses:
        - Agent learning from actions
        - Reward tracking
        - Action evaluation
        
    Example:
        >>> result = pynlm.ActionResult(reward=1.0, success=True, message="Moved successfully")
        >>> result.reward
        1.0
        >>> result.success
        True
        >>> result.message
        'Moved successfully'
    
    Attributes:
        reward: Reward value received (positive = good, negative = bad)
        success: Whether action was successful
        message: Human-readable feedback message
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(Create action result)
             
             Args:
                 reward: Reward value
                 success: Whether action was successful
                 message: Optional feedback message
                 
             Example:
                 >>> result = pynlm.ActionResult(1.0, True, "Success!")
                 >>> result.reward
                 1.0
            ")
        .def_readwrite("reward", &ActionResult::reward,
             R"pbdoc(Reward value)
             
             Example:
                 >>> result = pynlm.ActionResult()
                 >>> result.reward = 2.5
                 >>> result.reward
                 2.5
             ")
        .def_readwrite("success", &ActionResult::success,
             R"pbdoc(Success status)
             
             Example:
                 >>> result = pynlm.ActionResult()
                 >>> result.success = False
                 >>> result.success
                 False
             ")
        .def_readwrite("message", &ActionResult::message,
             R"pbdoc(Feedback message)
             
             Example:
                 >>> result = pynlm.ActionResult()
                >>> result.message = "Collected resource"
                 >>> result.message
                 'Collected resource'
             ")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, R"pbdoc(Check if reward is positive)
        
        Returns:
            bool: True if reward > 0
            
        Example:
            >>> good_result = pynlm.ActionResult(1.0, True)
            >>> good_result.isPositive()
            True
            >>> bad_result = pynlm.ActionResult(-1.0, False)
            >>> bad_result.isPositive()
            False
        ")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, R"pbdoc(Check if reward is negative)
        
        Returns:
            bool: True if reward < 0
            
        Example:
            >>> good_result = pynlm.ActionResult(1.0, True)
            >>> good_result.isNegative()
            False
            >>> bad_result = pynlm.ActionResult(-1.0, False)
            >>> bad_result.isNegative()
            True
        ")
        .def("getMagnitude", [](const ActionResult& self) {
            return std::abs(self.reward);
        }, R"pbdoc(Get absolute reward magnitude)
        
        Returns:
            float: Absolute value of reward
            
        Example:
            >>> result = pynlm.ActionResult(-3.5, False)
            >>> result.getMagnitude()
            3.5
        ")
        .def("__repr__", [](const ActionResult& r) {
            return "<ActionResult: reward=" + std::to_string(r.reward) + ", success=" + (r.success ? "True" : "False") + ">";
        });
}

void bindSensoryPercept(py::module_& m) {
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
    
    Container for all sensory inputs from the world.
    Represents what an agent perceives in its environment, including
    vision, touch, internal signals, and other sensory modalities.
    
    Common Uses:
        - Agent perception and awareness
        - Sensor fusion
        - Decision making input
        
    Example:
        >>> percept = pynlm.SensoryPercept()
        >>> # Set various sensory inputs
        >>> percept.setVision(vision_data)
        >>> percept.setTouch(touch_data)
        >>> percept.setInternal(internal_signals)
        >>> # Access sensory data
        >>> vision = percept.getVision()
        >>> touch = percept.getTouch()
        >>> internal = percept.getInternal()
    
    Attributes:
        vision: Visual sensory input
        touch: Proprioceptive/touch input
        internal: Internal brain signals
        audio: Auditory input (if available)
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(Get visual sensory input)
             
             Returns:
                 Vision: Visual input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> vision = percept.getVision()
                 >>> vision.getWidth()
                 8
             ")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set visual sensory input)
             
             Args:
                 vision: Vision sensory input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> vision = pynlm.Vision(8, 8, 3)
                 >>> percept.setVision(vision)
             ")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(Get vision width)
             
             Returns:
                 int: Vision width
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> percept.setVision(pynlm.Vision(10, 15, 3))
                 >>> percept.getVisionWidth()
                 10
             ")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(Get vision height)
             
             Returns:
                 int: Vision height
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> percept.setVision(pynlm.Vision(10, 15, 3))
                 >>> percept.getVisionHeight()
                 15
             ")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(Get touch/pro proprioceptive input)
             
             Returns:
                 SensoryInput: Touch input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> touch = percept.getTouch()
             ")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch/pro proprioceptive input)
             
             Args:
                 touch: Touch sensory input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> touch = pynlm.InternalSignals()
                 >>> percept.setTouch(touch)
             ")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(Get internal signals)
             
             Returns:
                 list: Internal signal values
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> internal = percept.getInternal()
                 >>> len(internal)
                 0
             ")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal signals)
             
             Args:
                 internal: List of internal signal values
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> percept.setInternal([0.5, 0.8, 0.3])
                 >>> percept.getInternal()
                 [0.5, 0.8, 0.3]
             ")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(Get proprioceptive input)
             
             Returns:
                 SensoryInput: Proprioceptive input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> proprio = percept.getProprioception()
             ")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioceptive input)
             
             Args:
                 proprioception: Proprioceptive sensory input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> percept.setProprioception(pynlm.InternalSignals())
             ")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(Get auditory input)
             
             Returns:
                 Audio: Audio input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> audio = percept.getAudio()
             ")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set auditory input)
             
             Args:
                 audio: Audio sensory input
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> audio = pynlm.Audio(16000, 100)
                 >>> percept.setAudio(audio)
             ")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(Get all sensory signals as a dictionary)
             
             Returns:
                 dict: Dictionary of all sensory signals
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> signals = percept.getAllSignals()
                 >>> 'vision' in signals
                 True
             ")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(Get percept timestamp)
             
             Returns:
                 float: Timestamp
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> percept.setTimestamp(1.5)
                 >>> percept.getTimestamp()
                 1.5
             ")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set percept timestamp)
             
             Args:
                 timestamp: Timestamp
                 
             Example:
                 >>> percept = pynlm.SensoryPercept()
                 >>> percept.setTimestamp(2.0)
                 >>> percept.getTimestamp()
                 2.0
             ")
        .def("getSignalCount", [](const SensoryPercept& self) {
            auto all_signals = self.getAllSignals();
            return static_cast<int>(all_signals.size());
        }, R"pbdoc(Get total number of sensory signals)
        
        Returns:
            int: Number of sensory signals
            
        Example:
            >>> percept = pynlm.SensoryPercept()
            >>> count = percept.getSignalCount()
            >>> count >= 0
            True
        ")
        .def("__repr__", [](const SensoryPercept& p) {
            return "<SensoryPercept: vision=" + std::to_string(p.getVisionWidth()) + 
                   "x" + std::to_string(p.getVisionHeight()) + " vision=" + 
                   std::to_string(p.getVision().getChannels()) + 
                   " channels, internal=" + std::to_string(p.getInternal().size()) + ">";
        });
}

void bindAgentBrain(py::module_& m) {
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
    
    Interface that connects the NLM brain to the world environment.
    Handles sensory input processing, motor command decoding, and
    neuromodulation for learning and adaptation.
    
    Common Uses:
        - Agent-environment interaction
        - Perception-action loop
        - Learning and adaptation
        
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Process sensory input
        >>> percept = world.getSensoryPercept()
        >>> agent.processSensoryInput(percept)
        >>> 
        >>> # Get motor command
        >>> action = agent.decodeMotorCommand()
        >>> 
        >>> # Apply learning
        >>> reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        >>> agent.applyRewardModulation(reward, 0.0)
    
    Attributes:
        brain: The underlying NLM brain
        world: The connected world
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create agent brain interface)
             
             Args:
                 brain: The underlying NLM brain
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
             ")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize agent with world)
             
             Args:
                 world: The world to connect to
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.initialize(world)
             ")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size)
             
             Returns:
                 int: Expected sensory input size
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.getSensoryInputSize()
                 192  # 8x8x3 vision data
             ")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size)
             
             Returns:
                 int: Expected motor output size
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.getMotorOutputSize()
                 10  # Number of motor neurons
             ")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain)
             
             Args:
                 percept: Sensory percept from world
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> percept = world.getSensoryPercept()
                 >>> agent.processSensoryInput(percept)
             ")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command)
             
             Returns:
                 Action: Motor command decoded from brain activity
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> action = agent.decodeMotorCommand()
                 >>> action.getType()
                 'move_forward'
             ")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation)
             
             Args:
                 reward: Actual reward received
                 predictedReward: Predicted reward
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.applyRewardModulation(1.0, 0.5)
             ")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(Update development system)
             
             Args:
                 timestep: Time step for development
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.updateDevelopment(0.1)
             ")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)
             
             Returns:
                 str: Developmental stage name
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> stage = agent.getDevelopmentalStage()
                 >>> stage
                 'adult'
             ")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level)
             
             Returns:
                 float: Neuromodulation level (0.0 to 1.0)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> level = agent.getNeuromodulationLevel()
                 >>> level >= 0.0 and level <= 1.0
                 True
             ")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level)
             
             Returns:
                 float: Curiosity level (0.0 to 1.0)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> curiosity = agent.getCuriosityLevel()
                 >>> curiosity >= 0.0 and curiosity <= 1.0
                 True
             ")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level)
             
             Returns:
                 float: Novelty level (0.0 to 1.0)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> novelty = agent.getNoveltyLevel()
                 >>> novelty >= 0.0 and novelty <= 1.0
                 True
             ")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error)
             
             Returns:
                 float: Prediction error (0.0 to 1.0)
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> error = agent.getPredictionError()
                 >>> error >= 0.0 and error <= 1.0
                 True
             ")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode)
             
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.reset()
             ")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain)
             
             Returns:
                 Brain: The underlying NLM brain
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> brain = agent.getBrain()
                 >>> brain.getTotalNeuronCount()
                 1000
             ")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), R"pbdoc(Enable/disable reward modulation)
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.enableRewardModulation(True)
                 >>> agent.isRewardModulationEnabled()
                 True
             ")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), R"pbdoc(Enable/disable structural plasticity)
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.enableStructuralPlasticity(True)
                 >>> agent.isStructuralPlasticityEnabled()
                 True
             ")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), R"pbdoc(Enable/disable development)
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.enableDevelopment(True)
                 >>> agent.isDevelopmentEnabled()
                 True
             ")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), R"pbdoc(Enable/disable curiosity)
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.enableCuriosity(True)
                 >>> agent.isCuriosityEnabled()
                 True
             ")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled)
             
             Returns:
                 bool: True if enabled
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.isRewardModulationEnabled()
                 True
             ")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled)
             
             Returns:
                 bool: True if enabled
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.isStructuralPlasticityEnabled()
                 True
             ")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled)
             
             Returns:
                 bool: True if enabled
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.isDevelopmentEnabled()
                 True
             ")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled)
             
             Returns:
                 bool: True if enabled
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> brain = pynlm.createBrain(config)
                 >>> agent = pynlm.createAgentBrain(brain)
                 >>> agent.isCuriosityEnabled()
                 True
             )
        .def("__repr__", [](const AgentBrain& a) {
            return "<AgentBrain: connected to world>";
        });
}