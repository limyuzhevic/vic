// NLM (Neural Learning Machine) Python Bindings - AgentBrain Class
// This file contains bindings for the AgentBrain class

namespace py = pybind11;

void bindAgentBrainClass(py::module_& m) {
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        The AgentBrain class serves as the interface between the neural brain system
        and the external world, handling sensory input processing, motor command
        decoding, and neuromodulation. It provides:
        
        - Sensory perception processing (vision, touch, internal signals, audio)
        - Motor command decoding from neural activity
        - Reward-based learning and neuromodulation
        - Development system updates
        - System enable/disable flags for different capabilities
        
        This class enables the creation of autonomous agents that can interact
        with and learn from their environment.
        
        Example usage:
        ```python
        import pynlm
        
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Create agent brain interface
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Initialize with world
        agent_brain.initialize(world)
        
        # Configure world for agent
        world.configure(width=100, height=100, vision_width=20, vision_height=20)
        world.setAgentStart(50, 50)
        
        # Enable key systems
        agent_brain.enableRewardModulation(True)
        agent_brain.enableCuriosity(True)
        agent_brain.enableDevelopment(True)
        
        # Simulation loop
        for episode in range(100):
            # Reset agent for new episode
            agent_brain.reset()
            
            # Run episode
            for timestep in range(200):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent_brain.processSensoryInput(percept)
                
                # Decode motor command
                action = agent_brain.decodeMotorCommand()
                
                # Apply action in world
                result = world.applyMotorCommand(action, timestep)
                
                # Apply reward modulation
                agent_brain.applyRewardModulation(result.reward, 0.0)
                
                # Update development
                agent_brain.updateDevelopment(timestep)
                
                # Advance world
                world.update(timestep)
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
}