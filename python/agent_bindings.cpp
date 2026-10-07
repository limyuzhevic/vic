#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "../src/agent/AgentBrain.hpp"
#include "../src/brain/Brain.hpp"

namespace py = pybind11;
namespace nlm {

// agent_bindings.cpp - AgentBrain class
// Purpose: Defines Python bindings for the AgentBrain interface connecting NLM brain to world

// Register AgentBrain class bindings to the given module
void register_agent(pybind11::module_& m) {

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain is the bridge between the neural processing of the brain and the
        simulated environment. It takes sensory input from the world, processes it
        through the brain, and generates motor commands to act in the world.
        
        This interface implements the complete agent control loop:
        1. Receive sensory information from the world
        2. Process sensory input through the brain
        3. Generate motor commands based on neural activity
        4. Return actions to the world
        5. Receive feedback and apply reward modulation
        
        The AgentBrain encapsulates the interaction between the cognitive system
        (brain) and the embodiment (world), implementing the fundamental
        architecture of embodied artificial intelligence.
        
        Examples:
            >>> import nlm
            >>> config = nlm.createDefaultConfig()
            >>> brain = nlm.createBrain(config)
            >>> brain.initialize()
            >>> world = nlm.createSimpleWorld()
            >>> agent = nlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> # Agent ready to interact with world
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Initialize AgentBrain with a brain instance.
             
             Args:
                 brain: The Brain instance to be controlled by this agent.
                       Must be properly initialized before use.
             
             Examples:
                 >>> brain = nlm.createBrain(nlm.createDefaultConfig())
                 >>> agent = nlm.createAgentBrain(brain)
                 >>> agent.initialize(world)
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize the agent with a world instance.
             
             Args:
                 world: The SimpleWorld instance where the agent will operate.
                       The world must be configured before initialization.
             
             This sets up the agent's sensory and motor interfaces with the world,
            making it ready to process sensory input and generate actions.
            
            Raises:
                ValueError: If world is None or not properly configured
                RuntimeError: If agent is already initialized
            
            Examples:
                >>> agent.initialize(world)
            )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
             Get the expected size of sensory input.
             
             Returns:
                 int: Number of expected sensory input dimensions
             
             This is used to validate that sensory input matches what the brain
            expects. Different sensory modalities (vision, audio, internal)
            contribute to the total input size.
            
            Examples:
                >>> size = agent.getSensoryInputSize()
                >>> size > 0
                True
            )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
             Get the expected size of motor output.
             
             Returns:
                 int: Number of expected motor output dimensions
             
            The motor output size corresponds to the brain's motor system
            and determines the complexity of actions the agent can generate.
            
            Examples:
                >>> size = agent.getMotorOutputSize()
                >>> size > 0
                True
            )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain.
             
             Args:
                 percept: SensoryPercept object containing all sensory information.
                         Must include vision, audio, and internal signals.
            
            This is the main input processing function. It extracts information
            from the percept and injects it into the appropriate neurons in the
            brain, allowing the brain to respond to the current environment.
            
            Raises:
                ValueError: If percept is None or missing required data
                RuntimeError: If brain is not initialized or not ready
            
            Examples:
                >>> percept = world.getSensoryPercept()
                >>> agent.processSensoryInput(percept)
            )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command.
             
             Returns:
                 Action: Motor action determined by the brain's current activity.
                        Represents the agent's intended behavior.
            
            This function reads the brain's motor neuron activity and converts
            it into a concrete motor command. The brain has learned patterns
            of activity that correspond to different actions through training.
            
            Examples:
                >>> action = agent.decodeMotorCommand()
                >>> action.getType()
                <ActionType.MoveForward: 0>
                >>> action.getName()
                'MoveForward'
            )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation to the brain.
             
             Args:
                 reward: Actual reward received from the world (e.g., +10 for food)
                 predictedReward: Expected reward predicted by the brain (-10 for hunger)
            
            This implements the reward prediction error signal that drives learning
            in the brain. When the actual reward differs from the predicted reward,
            the brain adjusts its expectations and learns from the discrepancy.
            
            Raises:
                RuntimeError: If brain is not initialized
            
            Examples:
                >>> reward = 10.0  # Found food
                >>> predicted = -5.0  # Expected hunger
                >>> agent.applyRewardModulation(reward, predicted)
            )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system based on current timestep.
             
             Args:
                 timestep: Current simulation timestep duration
            
            This advances the brain's developmental processes, allowing it
            to mature, change its plasticity rates, and adapt its architecture
            over time. Development affects learning capabilities and behavior.
            
            Examples:
                >>> agent.updateDevelopment(0.1)
            )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
             Get the current developmental stage of the agent.
             
             Returns:
                 DevelopmentalStage: Current stage of brain development
            
            Examples:
                >>> stage = agent.getDevelopmentalStage()
                >>> stage == nlm.DevelopmentalStage.Adult
                True
            )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
             Get the current neuromodulation level.
             
             Returns:
                 float: Current neuromodulation strength (0.0 to 1.0)
            
            Neuromodulation affects neural excitability, plasticity, and
            learning rates. Higher levels increase responsiveness and
            learning speed.
            
            Examples:
                >>> level = agent.getNeuromodulationLevel()
                >>> level >= 0.0
                True
            )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get the current curiosity level.
             
             Returns:
                 float: Current curiosity level (0.0 to 1.0)
            
            Curiosity drives exploration behavior, encouraging the agent
            to seek novel experiences and learn new information.
            
            Examples:
                >>> level = agent.getCuriosityLevel()
                >>> level >= 0.0
                True
            )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get the current novelty level.
             
             Returns:
                 float: Current novelty level (0.0 to 1.0)
            
            Novelty represents how novel the current environment or experience
            is to the agent, driving learning and attention.
            
            Examples:
                >>> level = agent.getNoveltyLevel()
                >>> level >= 0.0
                True
            )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get the current prediction error.
             
             Returns:
                 float: Current prediction error magnitude
            
            Prediction error measures the difference between expected and
            actual outcomes, driving learning and adaptation.
            
            Examples:
                >>> error = agent.getPredictionError()
                >>> error >= 0.0
                True
            )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
             Reset the agent for a new episode.
             
             This resets all agent state, clears sensory history, and prepares
            the agent for a new interaction episode. Useful for starting
            fresh simulations or testing.
            
            Examples:
                >>> agent.reset()
                >>> # Agent ready for new episode
            )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the underlying brain instance.
             
             Returns:
                 Brain: Reference to the brain's underlying neural system
            
            Examples:
                >>> brain = agent.getBrain()
                >>> brain.getTotalNeuronCount() > 0
                True
            )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable reward modulation.
             
             Args:
                 enable: True to enable, False to disable reward modulation
            
            Examples:
                >>> agent.enableRewardModulation(True)
                >>> agent.enableRewardModulation(False)
            )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable structural plasticity.
             
             Args:
                 enable: True to enable, False to disable structural plasticity
            
            Examples:
                >>> agent.enableStructuralPlasticity(True)
                >>> agent.enableStructuralPlasticity(False)
            )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable developmental processes.
             
             Args:
                 enable: True to enable, False to disable development
            
            Examples:
                >>> agent.enableDevelopment(True)
                >>> agent.enableDevelopment(False)
            )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable curiosity-driven exploration.
             
             Args:
                 enable: True to enable, False to disable curiosity
            
            Examples:
                >>> agent.enableCuriosity(True)
                >>> agent.enableCuriosity(False)
            )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(
             Check if reward modulation is enabled.
             
             Returns:
                 bool: True if reward modulation is enabled
            
            Examples:
                >>> enabled = agent.isRewardModulationEnabled()
                >>> isinstance(enabled, bool)
                True
            )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(
             Check if structural plasticity is enabled.
             
             Returns:
                 bool: True if structural plasticity is enabled
            
            Examples:
                >>> enabled = agent.isStructuralPlasticityEnabled()
                >>> isinstance(enabled, bool)
                True
            )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(
             Check if development is enabled.
             
             Returns:
                 bool: True if development is enabled
            
            Examples:
                >>> enabled = agent.isDevelopmentEnabled()
                >>> isinstance(enabled, bool)
                True
            )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(
             Check if curiosity is enabled.
             
             Returns:
                 bool: True if curiosity is enabled
            
            Examples:
                >>> enabled = agent.isCuriosityEnabled()
                >>> isinstance(enabled, bool)
                True
            )pbdoc");

} // namespace nlm