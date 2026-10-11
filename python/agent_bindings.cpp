// Python bindings for NLM agent and world functionality
// This file contains bindings for AgentBrain, SimpleWorld, and related classes

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/WorldObject.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Action.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

void bindAgentBrain(py::module& m);
void bindAgentBody(py::module& m);
void bindSensoryPercept(py::module& m);
void bindSimpleWorld(py::module& m);
void bindWorldObject(py::module& m);
void bindAction(py::module& m);
void bindObservation(py::module& m);
void bindEnvironment(py::module& m);

void include_agent_bindings(py::module& m) {
    bindAgentBrain(m);
    bindAgentBody(m);
    bindSensoryPercept(m);
    bindSimpleWorld(m);
    bindWorldObject(m);
    bindAction(m);
    bindObservation(m);
    bindEnvironment(m);
}

void bindAgentBrain(py::module& m) {
    // AgentBrain class - Interface between NLM brain and world
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        The AgentBrain class provides the interface between the NLM brain and the
        external world. It handles:
        - Sensory transduction (processing world percepts)
        - Motor decoding (converting neural activity to actions)
        - Neuromodulation (reward-based learning)
        - Development (brain maturation over time)
        
        The agent follows this processing loop:
        1. Receive sensory percept from world
        2. Process percept and inject into brain
        3. Brain produces neural activity
        4. Decode neural activity to motor command
        5. Apply motor command to world
        6. Receive reward feedback
        7. Apply reward modulation to brain
        8. Update development system
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> agent = pynlm.createAgentBrain(brain)
            >>> 
            >>> # Create and configure world
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> world.reset()
            >>> 
            >>> # Initialize agent with world
            >>> agent.initialize(world)
            >>> 
            >>> # Enable learning subsystems
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
            >>> agent.enableDevelopment(True)
            >>> 
            >>> # Run simulation
            >>> for step in range(100):
            ...     world.update(0.1)  # Update world
            ...     percept = world.getSensoryPercept()  # What does the agent see?
            ...     agent.processSensoryInput(percept)  # Brain sees it
            ...     action = agent.decodeMotorCommand()  # Brain decides action
            ...     world.applyMotorCommand(action, world.getSimulationTime())  # Do action
            ...     
            >>> print(f"Simulation finished! Curiosity: {agent.getCuriosityLevel():.3f}")
        
        Methods:
            __init__(brain): Create agent with brain
            initialize(world): Initialize with world
            getSensoryInputSize(): Get expected sensory input size
            getMotorOutputSize(): Get expected motor output size
            processSensoryInput(percept): Process sensory percept and inject into brain
            decodeMotorCommand(): Decode brain motor activity into motor command
            applyRewardModulation(reward, predictedReward): Apply reward-based neuromodulation
            updateDevelopment(timestep): Update development system
            getDevelopmentalStage(): Get current developmental stage
            getNeuromodulationLevel(): Get current neuromodulation level
            getCuriosityLevel(): Get curiosity level
            getNoveltyLevel(): Get novelty level
            getPredictionError(): Get prediction error
            reset(): Reset agent for new episode
            getBrain(): Get the underlying brain
            enableRewardModulation(enabled): Enable/disable reward modulation
            enableStructuralPlasticity(enabled): Enable/disable structural plasticity
            enableDevelopment(enabled): Enable/disable development
            enableCuriosity(enabled): Enable/disable curiosity
            isRewardModulationEnabled(): Check if reward modulation is enabled
            isStructuralPlasticityEnabled(): Check if structural plasticity is enabled
            isDevelopmentEnabled(): Check if development is enabled
            isCuriosityEnabled(): Check if curiosity is enabled
            
        Attributes:
            brain: The underlying brain instance
            developmental_stage: Current developmental stage
            neuromodulation_level: Current neuromodulation level
            curiosity_level: Current curiosity level
            novelty_level: Current novelty level
            prediction_error: Current prediction error
            
        Raises:
            RuntimeError: If agent operations fail
            ValueError: If invalid parameters are provided
            
        Context Managers:
            The AgentBrain class can be used as a context manager:
            
            >>> with pynlm.createAgentBrain(brain) as agent:
            ...     agent.initialize(world)
            ...     agent.processSensoryInput(percept)
            ...     # Agent automatically reset on exit
            
        See Also:
            pynlm.createAgentBrain, pynlm.createSimpleWorld
            pynlm.createBrain, pynlm.createDefaultConfig
            Action, SensoryPercept, World
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent with brain")
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
             "Update development system")\n        // Development methods
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Neuromodulation methods
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        // State management
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        
        // Brain access
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // Subsystem enable/disable
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        
        // Subsystem status
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // Convenience methods
        .def("get_agent_stats", &AgentBrain::getAgentStats,
             "Get agent statistics as dictionary (convenience method)",
             py::return_value_policy::copy)
        
        .def("get_motor_stats", &AgentBrain::getMotorStats,
             "Get motor system statistics as dictionary",
             py::return_value_policy::copy)
        
        .def("get_sensory_stats", &AgentBrain::getSensoryStats,
             "Get sensory system statistics as dictionary",
             py::return_value_policy::copy)
        
        // Context manager support
        .def("__enter__", [](AgentBrain& self) {
            return &self;
        }, "Context manager entry - returns self")
        .def("__exit__", [](AgentBrain& self, py::object) {
            self.reset();
        }, "Context manager exit - resets agent")
        
        // Statistics
        .def("getStatistics", &AgentBrain::getStatistics,
             "Get agent statistics as dictionary",
             py::return_value_policy::copy)
        
        // Additional utility methods
        .def("run_episode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
                             size_t steps, double dt = 0.1) {
            world->reset();
            for (size_t step = 0; step < steps; ++step) {
                world->update(dt);
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                self.updateDevelopment(dt);
            }
        }, py::arg("world"), py::arg("steps"), py::arg("dt") = 0.1,
         "Run a complete episode with the given world")
        
        .def("get_action_stats", &AgentBrain::getActionStats,
             "Get action execution statistics",
             py::return_value_policy::copy)
        
        .def("get_performance_metrics", &AgentBrain::getPerformanceMetrics,
             "Get agent performance metrics",
             py::return_value_policy::copy);
}

void bindAgentBody(py::module& m) {
    // AgentBody class - Agent body state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation.
        
        The AgentBody class represents the physical state of an agent in the NLM world,
        including position, orientation, velocity, and physical properties.
        
        The body state is used by the world to track agent movement and
        interaction with the environment. It's updated by motor commands
        and affected by world physics and hazards.
        
        Example:
            >>> import pynlm
            >>> body = pynlm.AgentBody()
            >>> body.x = 10.0
            >>> body.y = 10.0
            >>> body.orientation = 0.0
            >>> body.velocity_x = 1.0
            >>> body.velocity_y = 0.0
            >>> body.energy = 100.0
            >>> body.health = 1.0
            >>> 
            >>> # Update body with movement
            >>> body.x += body.velocity_x * 0.1
            >>> body.y += body.velocity_y * 0.1
            >>> 
            >>> print(f"Agent position: ({body.x:.1f}, {body.y:.1f})")
        
        Attributes:
            x: X position
            y: Y position
            orientation: Facing direction (radians)
            velocity_x: X velocity
            velocity_y: Y velocity
            angular_velocity: Angular velocity
            energy: Current energy level
            health: Health level (0-1)
            age: Agent age (simulation steps)
            is_moving: Whether agent is currently moving
            is_turning: Whether agent is currently turning
            last_action_time: Time of last action
            
        Methods:
            __init__(): Create empty agent body
            reset(): Reset body to default state
            setPosition(x, y): Set position
            getPosition(): Get position as tuple
            setVelocity(vx, vy): Set velocity
            getVelocity(): Get velocity as tuple
            setOrientation(angle): Set orientation
            getOrientation(): Get orientation
            move(dx, dy): Move by relative displacement
            turn(angle): Turn by relative angle
            updatePosition(dt): Update position based on velocity
            updateOrientation(dt): Update orientation based on angular velocity
            isCollidingWith(other): Check collision with another body
            getDistanceTo(other): Get distance to another body
            getHeadingTo(target): Get heading to target position
        
        Properties:
            position: Get/set position (x, y)
            velocity: Get/set velocity (vx, vy)
            speed: Get current speed
            facing: Get/set orientation
            
        See Also:
            SimpleWorld, Action, SensoryPercept, AgentBrain
    )pbdoc")
        .def(py::init<>())
        .def("reset", &AgentBody::reset,
             "Reset body to default state")
        
        // Position
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction (radians)")
        
        // Velocity
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        
        // Physical properties
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Agent age (simulation steps)")
        
        // State flags
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        
        // Position utilities
        .def("setPosition", [](AgentBody& self, double x, double y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
         "Set position")
        
        .def("getPosition", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y);
        }, "Get position as tuple (x, y)")
        
        .def("move", [](AgentBody& self, double dx, double dy) {
            self.x += dx;
            self.y += dy;
            self.isMoving = (dx != 0.0 || dy != 0.0);
        }, py::arg("dx"), py::arg("dy"),
         "Move by relative displacement")
        
        // Velocity utilities
        .def("setVelocity", [](AgentBody& self, double vx, double vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0 || vy != 0.0);
        }, py::arg("vx"), py::arg("vy"),
         "Set velocity")
        
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as tuple (vx, vy)")
        
        // Orientation utilities
        .def("setOrientation", &AgentBody::setOrientation, py::arg("angle"),
             "Set orientation")
        .def("getOrientation", &AgentBody::getOrientation,
             "Get orientation")
        
        .def("turn", [](AgentBody& self, double angle) {
            self.orientation += angle;
            self.isTurning = (angle != 0.0);
            // Normalize orientation
            while (self.orientation > M_PI) self.orientation -= 2.0 * M_PI;
            while (self.orientation < -M_PI) self.orientation += 2.0 * M_PI;
        }, py::arg("angle"),
         "Turn by relative angle")
        
        // Physics updates
        .def("updatePosition", &AgentBody::updatePosition, py::arg("dt"),
             "Update position based on velocity")
        
        .def("updateOrientation", &AgentBody::updateOrientation, py::arg("dt"),
             "Update orientation based on angular velocity")
        
        // Distance calculations
        .def("getDistanceTo", [](const AgentBody& self, const AgentBody& other) {
            double dx = self.x - other.x;
            double dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"),
         "Get distance to another body")
        
        .def("getHeadingTo", [](const AgentBody& self, double target_x, double target_y) {
            double dx = target_x - self.x;
            double dy = target_y - self.y;
            return std::atan2(dy, dx);
        }, py::arg("target_x"), py::arg("target_y"),
         "Get heading to target position")
        
        .def("isCollidingWith", [](const AgentBody& self, const AgentBody& other) {
            double distance = self.getDistanceTo(other);
            return distance < (self.getRadius() + other.getRadius());
        }, py::arg("other"),
         "Check collision with another body")
        
        .def("getRadius", &AgentBody::getRadius,
             "Get agent radius for collision detection")
        
        // Convenience properties
        .def_property("position", [](const AgentBody& self) {
            return std::make_tuple(self.x, self.y);
        }, [](AgentBody& self, const std::tuple<double, double>& pos) {
            self.x = std::get<0>(pos);
            self.y = std::get<1>(pos);
        }, "Position as tuple")
        
        .def_property("velocity", [](const AgentBody& self) {
            return std::make_tuple(self.velocityX, self.velocityY);
        }, [](AgentBody& self, const std::tuple<double, double>& vel) {
            self.velocityX = std::get<0>(vel);
            self.velocityY = std::get<1>(vel);
            self.isMoving = (vel != std::make_tuple(0.0, 0.0));
        }, "Velocity as tuple")
        
        .def_property("facing", [](const AgentBody& self) {
            return self.orientation;
        }, [](AgentBody& self, double angle) {
            self.orientation = angle;
        }, "Facing direction (radians)")
        
        .def_property("speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, nullptr, "Current speed")
        
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos=(" + std::to_string(body.x) + ", " + std::to_string(body.y) +
                   "), vel=(" + std::to_string(body.velocityX) + ", " + std::to_string(body.velocityY) +
                   "), energy=" + std::to_string(body.energy) + ">";
        }, "String representation of agent body");
}

void bindSensoryPercept(py::module& m) {
    // SensoryPercept class - Processed sensory data
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Processed sensory data from the world.
        
        The SensoryPercept class represents processed sensory information
        that an agent receives from the environment. It contains all the
        sensory modalities processed and integrated by the agent:
        - Vision: Visual information from camera sensors
        - Touch: Contact and proximity information
        - Internal: Internal body state (energy, health, etc.)
        - Proprioception: Body position and orientation
        - Audio: Sound information
        
        The percept is the agent's representation of the world state,
        formed by integrating raw sensory inputs with internal knowledge
        and expectations.
        
        Example:
            >>> import pynlm
            >>> percept = pynlm.SensoryPercept()
            >>> 
            >>> # Set visual input
            >>> vision = pynlm.Vision(width=8, height=8)
            >>> for i in range(64):
            ...     vision.setData(i % 256 / 255.0)
            >>> percept.setVision(vision)
            >>> 
            >>> # Set internal signals
            >>> internal = pynlm.InternalSignals()
            >>> internal.addSignal(percept.getEnergy())
            >>> internal.addSignal(percept.getHealth())
            >>> percept.setInternal(internal)
            >>> 
            >>> # Get percept information
            >>> print(f"Vision dimensions: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
            >>> print(f"Internal signals: {len(percept.getInternal().getSignals())}")
        
        Attributes:
            vision_width: Width of visual input
            vision_height: Height of visual input
            touch_sensors: Number of touch sensors
            internal_signals: Number of internal signals
            proprioceptive_sensors: Number of proprioceptive sensors
            audio_channels: Number of audio channels
            
        Methods:
            __init__(): Create empty sensory percept
            getVision(): Get visual input
            setVision(vision): Set visual input
            getVisionWidth(): Get visual input width
            getVisionHeight(): Get visual input height
            getTouch(): Get touch input
            setTouch(touch): Set touch input
            getInternal(): Get internal signals
            setInternal(internal): Set internal signals
            getProprioception(): Get proprioceptive input
            setProprioception(proprioception): Set proprioceptive input
            getAudio(): Get audio input
            setAudio(audio): Set audio input
            getAllSignals(): Get all sensory signals as dictionary
            getTimestamp(): Get percept timestamp
            setTimestamp(timestamp): Set percept timestamp
            getSensoryInputSize(): Get expected sensory input size
            clear(): Clear all sensory inputs
        
        Properties:
            vision: Get/set visual input
            touch: Get/set touch input
            internal: Get/set internal signals
            proprioception: Get/set proprioceptive input
            audio: Get/set audio input
            timestamp: Get/set timestamp
            
        See Also:
            Vision, Audio, InternalSignals, SimpleWorld, AgentBrain
    )pbdoc")
        "def(py::init<>()
        .def("getVision", &SensoryPercept::getVision,
             py::return_value_policy::reference_internal,
             "Get visual input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get visual input width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get visual input height")
        .def("getTouch", &SensoryPercept::getTouch,
             py::return_value_policy::reference_internal,
             "Get touch input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch input")
        .def("getInternal", &SensoryPercept::getInternal,
             py::return_value_policy::reference_internal,
             "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
             py::return_value_policy::reference_internal,
             "Get proprioceptive input")
        .def("setProprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"),
             "Set proprioceptive input")
        .def("getAudio", &SensoryPercept::getAudio,
             py::return_value_policy::reference_internal,
             "Get audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp")
        .def("getSensoryInputSize", &SensoryPercept::getSensoryInputSize,
             "Get expected sensory input size")
        .def("clear", &SensoryPercept::clear,
             "Clear all sensory inputs")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) +
                   "x" + std::to_string(percept.getVisionHeight()) +
                   ", internal_signals=" + std::to_string(percept.getInternal().getSignals().size()) + ">";
        }, "String representation of sensory percept")
        
        .def_property("vision", [](const SensoryPercept& self) {
            return self.getVision();
        }, [](SensoryPercept& self, const Vision& vision) {
            self.setVision(vision);
        }, "Visual input")
        
        .def_property("touch", [](const SensoryPercept& self) {
            return self.getTouch();
        }, [](SensoryPercept& self, const Touch& touch) {
            self.setTouch(touch);
        }, "Touch input")
        
        .def_property("internal", [](const SensoryPercept& self) {
            return self.getInternal();
        }, [](SensoryPercept& self, const InternalSignals& internal) {
            self.setInternal(internal);
        }, "Internal signals")
        
        .def_property("proprioception", [](const SensoryPercept& self) {
            return self.getProprioception();
        }, [](SensoryPercept& self, const Proprioception& proprioception) {
            self.setProprioception(proprioception);
        }, "Proprioceptive input")
        
        .def_property("audio", [](const SensoryPercept& self) {
            return self.getAudio();
        }, [](SensoryPercept& self, const Audio& audio) {
            self.setAudio(audio);
        }, "Audio input")
        
        .def_property("timestamp", [](const SensoryPercept& self) {
            return self.getTimestamp();
        }, [](SensoryPercept& self, double timestamp) {
            self.setTimestamp(timestamp);
        }, "Percept timestamp");
}

void bindSimpleWorld(py::module& m) {
    // SimpleWorld class - World simulation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The SimpleWorld class implements a simple 2D environment for NLM agents.
        It provides:
        - World geometry and physics
        - Agent movement and interaction
        - Sensory perception
        - Object management
        - Reward and punishment systems
        
        The world is updated in discrete time steps, allowing agents to
        perceive their environment, take actions, and receive feedback.
        
        Example:
            >>> import pynlm
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> world.reset()
            >>> world.setAgentStart(10.0, 10.0)
            >>> 
            >>> # Add some resources
            >>> world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
            >>> world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=-5.0))
            >>> 
            >>> # Run simulation
            >>> for step in range(100):
            ...     world.update(0.1)  # Update world physics
            ...     percept = world.getSensoryPercept()  # Get what agent sees
            ...     body = world.getAgentBody()  # Get agent state
            ...     
            >>> print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), Energy: {body.energy:.1f}")
        
        Methods:
            __init__(): Create empty world
            configure(width, height, visionWidth, visionHeight): Configure world dimensions
            reset(): Reset world to initial state
            update(timestep): Update world by timestep seconds
            setAgentStart(x, y): Set agent starting position
            applyMotorCommand(cmd, currentTime): Apply motor command
            getSensoryPercept(): Get sensory percept
            getAgentBody(): Get agent body state
            addObject(obj): Add a world object
            removeObject(x, y): Remove object at position
            isValidPosition(x, y): Check if position is valid
            getWidth(): Get world width
            getHeight(): Get world height
            getSimulationTime(): Get current simulation time
            getMaxEnergy(): Get maximum energy
            setMaxEnergy(e): Set maximum energy
            getEnergyDecayRate(): Get energy decay rate
            setEnergyDecayRate(r): Set energy decay rate
            setRandomSeed(seed): Set random seed
            getRandomSeed(): Get random seed
        
        Attributes:
            width: World width
            height: World height
            simulation_time: Current simulation time
            agent_body: Current agent body state
            max_energy: Maximum agent energy
            energy_decay_rate: Energy decay rate per timestep
            
        Context Managers:
            The SimpleWorld class can be used as a context manager:
            
            >>> with pynlm.createSimpleWorld() as world:
            ...     world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            ...     world.reset()
            ...     # World automatically reset on exit
            
        See Also:
            pynlm.createSimpleWorld, pynlm.createAgentBrain
            AgentBody, SensoryPercept, WorldObject
            Action, MotorCommand
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
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
        
        // Convenience methods
        .def("run_episode", [](SimpleWorld& self, std::shared_ptr<AgentBrain> agent,
                             size_t steps, double dt = 0.1) {
            self.reset();
            for (size_t step = 0; step < steps; ++step) {
                self.update(dt);
                auto percept = self.getSensoryPercept();
                agent->processSensoryInput(percept);
                auto action = agent->decodeMotorCommand();
                self.applyMotorCommand(action, self.getSimulationTime());
            }
        }, py::arg("agent"), py::arg("steps"), py::arg("dt") = 0.1,
         "Run a complete episode with the given agent")
        
        .def("get_world_stats", &SimpleWorld::getWorldStats,
             "Get world statistics as dictionary",
             py::return_value_policy::copy)
        
        .def("get_object_stats", &SimpleWorld::getObjectStats,
             "Get world object statistics as dictionary",
             py::return_value_policy::copy)
        
        .def("get_agent_stats", &SimpleWorld::getAgentStats,
             "Get agent statistics as dictionary",
             py::return_value_policy::copy)
        
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        }, "Context manager entry - returns self")
        .def("__exit__", [](SimpleWorld& self, py::object) {
            self.reset();
        }, "Context manager exit - resets world")
        
        // Properties
        .def_property("width", &SimpleWorld::getWidth, nullptr,
                     "World width")
        .def_property("height", &SimpleWorld::getHeight, nullptr,
                     "World height")
        .def_property("simulation_time", &SimpleWorld::getSimulationTime, nullptr,
                     "Current simulation time")
        .def_property("max_energy", &SimpleWorld::getMaxEnergy,
                     &SimpleWorld::setMaxEnergy,
                     "Maximum energy")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
                     &SimpleWorld::setEnergyDecayRate,
                     "Energy decay rate")
        
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + ", " +
                   std::to_string(world.getSimulationTime()) + "s>";
        }, "String representation of the world");
}

void bindWorldObject(py::module& m) {
    // WorldObject class - Object in the world
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Object in the NLM world.
        
        The WorldObject class represents entities in the NLM world,
        including:
        - Resources: Consumable resources (food, water, energy)
        - Hazards: Dangerous objects (poison, traps, obstacles)
        - Walls: Solid barriers
        - Markers: Special locations or targets
        
        Objects can be added to the world and interact with the agent,
        providing rewards, penalties, or obstacles.
        
        Example:
            >>> import pynlm
            >>> # Create a resource object
            >>> resource = pynlm.WorldObject(
            ...     5.0, 5.0,  # Position (x, y)
            ...     pynlm.WorldObjectType.Resource,
            ...     value=10.0,
            ...     radius=0.5
            ... )
            >>> print(f"Resource at ({resource.x:.1f}, {resource.y:.1f}), value: {resource.value}")
            >>> 
            >>> # Create a hazard object
            >>> hazard = pynlm.WorldObject(
            ...     15.0, 15.0,
            ...     pynlm.WorldObjectType.Hazard,
            ...     value=-5.0,
            ...     radius=0.8
            ... )
            >>> print(f"Hazard at ({hazard.x:.1f}, {hazard.y:.1f}), penalty: {hazard.value}")
            >>> 
            >>> # Check if agent can interact
            >>> agent = pynlm.AgentBody()
            >>> agent.x = 5.0
            >>> agent.y = 5.0
            >>> agent.setRadius(0.3)
            >>> 
            >>> if agent.isCollidingWith(resource):
            ...     print("Agent collected resource!")
            >>>     if agent.isCollidingWith(hazard):
            ...     print("Agent hit hazard!")
        
        Attributes:
            x: X position
            y: Y position
            radius: Object radius
            type: Object type
            value: Object value (positive = reward, negative = penalty)
            active: Whether object is active
            
        Methods:
            __init__(x, y, type, value, radius): Create world object
            getX(): Get X position
            getY(): Get Y position
            setPosition(x, y): Set position
            getRadius(): Get radius
            setRadius(radius): Set radius
            getType(): Get object type
            setType(type): Get object type
            getValue(): Get object value
            setValue(value): Set object value
            isActive(): Check if object is active
            setActive(active): Set active state
            isCollidingWith(other): Check collision with another object
            getDistanceTo(other): Get distance to another object
            getRewardValue(): Get reward value (for resources)
            getPenaltyValue(): Get penalty value (for hazards)
            isCollectible(): Check if object can be collected
            isHazard(): Check if object is a hazard
            isWall(): Check if object is a wall
            isMarker(): Check if object is a marker
        
        Properties:
            position: Get/set position (x, y)
            
        See Also:
            SimpleWorld, AgentBody, WorldObjectType
            Action, SensoryPercept
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object")
        
        // Position
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        
        // Type and value
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Active state")
        
        // Position utilities
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
         "Set position")
        
        .def("getPosition", [](const WorldObject& self) {
            return std::make_tuple(self.x, self.y);
        }, "Get position as tuple (x, y)")
        
        // Distance and collision
        .def("getDistanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"),
         "Get distance to another object")
        
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float distance = self.getDistanceTo(other);
            return distance < (self.radius + other.radius);
        }, py::arg("other"),
         "Check collision with another object")
        
        // Type utilities
        .def("isCollectible", &WorldObject::isCollectible,
             "Check if object can be collected")
        .def("isHazard", &WorldObject::isHazard,
             "Check if object is a hazard")
        .def("isWall", &WorldObject::isWall,
             "Check if object is a wall")
        .def("isMarker", &WorldObject::isMarker,
             "Check if object is a marker")
        
        // Reward/penalty utilities
        .def("getRewardValue", &WorldObject::getRewardValue,
             "Get reward value (for resources)")
        
        .def("getPenaltyValue", &WorldObject::getPenaltyValue,
             "Get penalty value (for hazards)")
        
        // Convenience properties
        .def_property("position", [](const WorldObject& self) {
            return std::make_tuple(self.x, self.y);
        }, [](WorldObject& self, const std::tuple<float, float>& pos) {
            self.x = std::get<0>(pos);
            self.y = std::get<1>(pos);
        }, "Position as tuple")
        
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: type=" + std::to_string(static_cast<int>(obj.type)) +
                   ", pos=(" + std::to_string(obj.x) + ", " + std::to_string(obj.y) +
                   "), radius=" + std::to_string(obj.radius) + ">";
        }, "String representation of world object");
}

void bindAction(py::module& m) {
    // Action class - Motor command
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor command for NLM agents.
        
        The Action class represents a motor command that an agent can execute
        in the NLM world. Actions are generated by the brain and decoded by
        the agent, then applied to the world.
        
        Actions include movement, turning, looking, and interaction:
        - Movement: MoveForward, MoveBackward, MoveLeft, MoveRight
        - Rotation: TurnLeft, TurnRight
        - Perception: Look, LookUp, LookDown
        - Interaction: Interact, Eat, Drink
        - State: Rest, Wait, Custom
        
        Each action has:
        - Type: The action type (enumeration)
        - Parameters: Optional parameters (e.g., power, duration)
        - Name: Human-readable name
        
        Example:
            >>> import pynlm
            >>> # Create simple movement actions
            >>> move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
            >>> move_backward = pynlm.Action(pynlm.ActionType.MoveBackward)
            >>> turn_left = pynlm.Action(pynlm.ActionType.TurnLeft)
            >>> turn_right = pynlm.Action(pynlm.ActionType.TurnRight)
            >>> 
            >>> # Create actions with parameters
            >>> look_action = pynlm.Action(
            ...     pynlm.ActionType.Look,
            ...     [30.0]  # Look angle
            ... )
            >>> 
            >>> # Get action information
            >>> print(f"Action type: {move_forward.getType()}")
            >>> print(f"Action name: {move_forward.getName()}")
            >>> print(f"Action parameters: {move_forward.getParameters()}")
        
        Methods:
            __init__(type): Create action with type
            __init__(type, parameters): Create action with type and parameters
            getType(): Get action type
            setType(type): Set action type
            getParameters(): Get action parameters
            setParameters(parameters): Set action parameters
            getName(): Get action name
            clone(): Create copy of action
            isValid(): Check if action is valid
            toDict(): Convert to dictionary
            fromDict(dictionary): Create from dictionary
        
        Properties:
            type: Get/set action type
            parameters: Get/set action parameters
            name: Get action name (read-only)
            
        See Also:
            ActionType, MotorCommand, AgentBrain, SimpleWorld
            pynlm.ActionType, pynlm.MotorCommand
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with type")
        .def(py::init<ActionType, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        
        // Type
        .def("getType", &Action::getType,
             "Get action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        
        // Parameters
        .def("getParameters", &Action::getParameters,
             "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        
        // Utilities
        .def("getName", &Action::getName,
             "Get action name")
        .def("clone", &Action::clone,
             "Create copy of action")
        .def("isValid", &Action::isValid,
             "Check if action is valid")
        
        // Conversion methods
        .def("toDict", &Action::toDict,
             "Convert to dictionary")
        .def_static("fromDict", &Action::fromDict, py::arg("dict"),
             "Create action from dictionary")
        
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + 
                   " (type=" + std::to_string(static_cast<int>(action.getType())) + ")>";
        }, "String representation of action")
        
        // Convenience properties
        .def_property("type", [](const Action& self) {
            return self.getType();
        }, [](Action& self, ActionType type) {
            self.setType(type);
        }, "Action type")
        
        .def_property("parameters", [](const Action& self) {
            return self.getParameters();
        }, [](Action& self, const std::vector<float>& params) {
            self.setParameters(params);
        }, "Action parameters");
}

void bindObservation(py::module& m) {
    // Observation class - Result of applying action
    py::class_<Observation>(m, "Observation", R"pbdoc(
        Result of applying action to the world.
        
        The Observation class represents the result of applying an action
        to the world, including:
        - Reward: Immediate reward or penalty
        - Success: Whether action succeeded
        - Message: Descriptive message about the result
        - WorldState: Snapshot of world state after action
        
        Observations are used by agents to learn from their actions
        and update their internal models of the world.
        
        Example:
            >>> import pynlm
            >>> # Create a simple observation
            >>> observation = pynlm.Observation()
            >>> observation.reward = 5.0
            >>> observation.success = True
            >>> observation.message = "Collected resource"
            >>> 
            >>> # Get observation information
            >>> print(f"Reward: {observation.reward}")
            >>> print(f"Success: {observation.success}")
            >>> print(f"Message: {observation.message}")
            >>> 
            >>> # Use in reinforcement learning
            >>> if observation.success:
            ...     print(f"Agent learned from action!")
            ...     if observation.reward > 0:
            ...         print(f"Gained {observation.reward} reward points")
        
        Attributes:
            reward: Reward or penalty value
            success: Whether action succeeded
            message: Descriptive message
            
        Methods:
            __init__(): Create empty observation
            __init__(reward, success, message): Create observation with values
            getReward(): Get reward value
            setReward(reward): Set reward value
            isSuccess(): Check if action succeeded
            setSuccess(success): Set success flag
            getMessage(): Get message
            setMessage(message): Set message
            toDict(): Convert to dictionary
            fromDict(dictionary): Create from dictionary
        
        Properties:
            reward: Get/set reward value
            success: Get/set success flag
            message: Get/set message
            
        See Also:
            Action, SensoryPercept, AgentBrain, SimpleWorld
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, const std::string&>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create observation with values")
        
        // Reward
        .def_readwrite("reward", &Observation::reward, "Reward or penalty value")
        
        // Success
        .def("isSuccess", &Observation::isSuccess,
             "Check if action succeeded")
        .def_readwrite("success", &Observation::success, "Success flag")
        
        // Message
        .def_readwrite("message", &Observation::message, "Descriptive message")
        
        // Conversion methods
        .def("toDict", &Observation::toDict,
             "Convert to dictionary")
        .def_static("fromDict", &Observation::fromDict, py::arg("dict"),
             "Create observation from dictionary")
        
        // Convenience properties
        .def_property("reward", [](const Observation& self) {
            return self.reward;
        }, [](Observation& self, float reward) {
            self.reward = reward;
        }, "Reward value")
        
        .def_property("success", [](const Observation& self) {
            return self.success;
        }, [](Observation& self, bool success) {
            self.success = success;
        }, "Success flag")
        
        .def_property("message", [](const Observation& self) {
            return self.message;
        }, [](Observation& self, const std::string& message) {
            self.message = message;
        }, "Message")
        
        .def("__repr__", [](const Observation& obs) {
            return "<Observation: reward=" + std::to_string(obs.reward) +
                   ", success=" + std::to_string(obs.success) +
                   ", message='" + obs.message + "'>";
        }, "String representation of observation");
}

void bindEnvironment(py::module& m) {
    // Environment class - World-environment interface
    py::class_<Environment>(m, "Environment", R"pbdoc(
        World-environment interface for NLM agents.
        
        The Environment class provides the interface between NLM agents and the
        world, implementing:
        - State transition: Apply actions and get new state
        - Reward calculation: Compute rewards based on actions
        - Termination: Determine when episode ends
        - Information: Provide observations to agents
        
        The environment is the learning context for agents, providing
        feedback about the consequences of their actions.
        
        Example:
            >>> import pynlm
            >>> # Create simple grid world
            >>> env = pynlm.SimpleEnvironment(width=10, height=10)
            >>> env.setAgentStart(5, 5)
            >>> 
            >>> # Reset environment
            >>> state = env.reset()
            >>> 
            >>> # Run episode
            >>> for step in range(100):
            ...     action = agent.decodeMotorCommand()
            ...     next_state, reward, done, info = env.step(action)
            ...     agent.processSensoryInput(next_state)
            ...     
            ...     if done:
            ...         print("Episode finished!")
            ...         break
            ...     
            >>> print(f"Total reward: {sum(rewards)}")
        
        Methods:
            reset(): Reset environment to initial state
            step(action): Apply action and get next state
            render(): Render current state
            close(): Close environment
            getObservationSpace(): Get observation space
            getActionSpace(): Get action space
            getRewardRange(): Get reward range
            isTerminal(state): Check if state is terminal
        
        See Also:
            SimpleWorld, AgentBrain, Action, Observation
    )pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset,
             "Reset environment to initial state")
        .def("step", &Environment::step,
             py::arg("action"),
             "Apply action and get next state")
        .def("render", &Environment::render,
             "Render current state")
        .def("close", &Environment::close,
             "Close environment")
        .def("getObservationSpace", &Environment::getObservationSpace,
             "Get observation space")
        .def("getActionSpace", &Environment::getActionSpace,
             "Get action space")
        .def("getRewardRange", &Environment::getRewardRange,
             "Get reward range")
        .def("isTerminal", &Environment::isTerminal, py::arg("state"),
             "Check if state is terminal");
}
