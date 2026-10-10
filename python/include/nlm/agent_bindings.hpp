// NLM Python Bindings - Agent and World Classes
// SensoryPercept, SimpleWorld, Brain, and AgentBrain class bindings

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "exceptions.hpp"
#include "validation.hpp"

namespace py = pybind11;
namespace nlm {

void bind_agent(py::module_& m) {
    // SensoryPercept class binding
    py::class_<SensoryPercept, std::shared_ptr<SensoryPercept>>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
    
    Represents the processed sensory information received by the agent.
    Contains vision, touch, internal, and audio signals from the world.
    "pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, R"pbdoc(Get vision data)
        
        Returns:
            Vision sensory input
        )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), R"pbdoc(Set vision data)
        
        Args:
            vision: Vision sensory input
        )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, R"pbdoc(Get vision width)
        
        Returns:
            Vision width in pixels
        )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, R"pbdoc(Get vision height)
        
        Returns:
            Vision height in pixels
        )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch, R"pbdoc(Get touch data)
        
        Returns:
            Touch sensory input
        )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), R"pbdoc(Set touch data)
        
        Args:
            touch: Touch sensory input
        )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal, R"pbdoc(Get internal signals)
        
        Returns:
            Internal signals sensory input
        )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), R"pbdoc(Set internal signals)
        
        Args:
            internal: Internal signals sensory input
        )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception, R"pbdoc(Get proprioception data)
        
        Returns:
            Proprioception sensory input
        )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), R"pbdoc(Set proprioception data)
        
        Args:
            proprioception: Proprioception sensory input
        )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio, R"pbdoc(Get audio data)
        
        Returns:
            Audio sensory input
        )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), R"pbdoc(Set audio data)
        
        Args:
            audio: Audio sensory input
        )pbdoc")
        .def("getAllSignals", [](const SensoryPercept& self) {
            // Helper to get all signals as a combined vector
            std::vector<float> allSignals;
            
            // Get vision data
            auto vision = self.getVision();
            if (vision) {
                auto& data = vision->getData();
                allSignals.insert(allSignals.end(), data.begin(), data.end());
            }
            
            // Get touch data
            auto touch = self.getTouch();
            if (touch) {
                auto& data = touch->getData();
                allSignals.insert(allSignals.end(), data.begin(), data.end());
            }
            
            // Get internal data
            auto internal = self.getInternal();
            if (internal) {
                auto& data = internal->getData();
                allSignals.insert(allSignals.end(), data.begin(), data.end());
            }
            
            // Get audio data
            auto audio = self.getAudio();
            if (audio) {
                auto& data = audio->getData();
                allSignals.insert(allSignals.end(), data.begin(), data.end());
            }
            
            return allSignals;
        }, R"pbdoc(Get all sensory signals as a combined vector)
        
        Returns:
            Vector containing all sensory signals (vision, touch, internal, audio)
        )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp, R"pbdoc(Get sensory percept timestamp)
        
        Returns:
            Timestamp when this percept was generated
        )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), R"pbdoc(Set sensory percept timestamp)
        
        Args:
            timestamp: Timestamp to set
        )pbdoc")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision_width=" + std::to_string(percept.getVisionWidth()) +
                   ", vision_height=" + std::to_string(percept.getVisionHeight()) +
                   ", signals=" + std::to_string(percept.getAllSignals().size()) + ">";
        });

    // SimpleWorld class binding with improved error handling
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
    
    The SimpleWorld class represents a 2D grid-based world where the agent
    interacts with objects and receives sensory input. It handles physics,
    collisions, and object management.
    "pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height, 
                            size_t visionWidth, size_t visionHeight) {
            try {
                validation::check_bounds("world width", width, 10, 10000);
                validation::check_bounds("world height", height, 10, 10000);
                validation::check_bounds("vision width", visionWidth, 1, width);
                validation::check_bounds("vision height", visionHeight, 1, height);
                self.configure(width, height, visionWidth, visionHeight);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw WorldError(std::string("Failed to configure world: ") + e.what());
            }
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), R"pbdoc(Configure the world dimensions)
        
        Args:
            width: World width in cells
            height: World height in cells
            visionWidth: Vision sensor width in cells
            visionHeight: Vision sensor height in cells
        
        Raises:
            WorldError: If world cannot be configured
            ValidationError: If dimensions are invalid
        )pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset the world to initial state)
        
        Resets the world to its initial configuration.
        )pbdoc")
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            try {
                validation::check_bounds("agent start x", x, 0.0f, 1000.0f);
                validation::check_bounds("agent start y", y, 0.0f, 1000.0f);
                self.setAgentStart(x, y);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw WorldError(std::string("Failed to set agent start: ") + e.what());
            }
        }, py::arg("x"), py::arg("y"), R"pbdoc(Set agent starting position)
        
        Args:
            x: Starting X coordinate
            y: Starting Y coordinate
        
        Raises:
            WorldError: If start position cannot be set
            ValidationError: If coordinates are invalid
        )pbdoc")
        .def("update", [](SimpleWorld& self, double timestep) {
            try {
                validation::check_bounds("timestep", timestep, 0.001, 1.0);
                self.update(timestep);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw WorldError(std::string("Failed to update world: ") + e.what());
            }
        }, py::arg("timestep"), R"pbdoc(Update world simulation)
        
        Args:
            timestep: Time step for update
        
        Raises:
            WorldError: If world cannot be updated
            ValidationError: If timestep is invalid
        )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), R"pbdoc(Apply a motor command to the world)
        
        Args:
            cmd: Motor command to apply
            currentTime: Current simulation time
        
        Returns:
            ActionResult from applying the command
        
        Raises:
            WorldError: If command cannot be applied
        )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, R"pbdoc(Get current sensory percept)
        
        Returns:
            Current sensory percept from agent
        )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, R"pbdoc(Get agent body state)
        
        Returns:
            Current agent body state
        )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), R"pbdoc(Add an object to the world)
        
        Args:
            obj: World object to add
        
        Raises:
            WorldError: If object cannot be added
        )pbdoc")
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            try {
                validation::check_bounds("object x", x, 0.0f, 1000.0f);
                validation::check_bounds("object y", y, 0.0f, 1000.0f);
                self.removeObject(x, y);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw WorldError(std::string("Failed to remove object: ") + e.what());
            }
        }, py::arg("x"), py::arg("y"), R"pbdoc(Remove object at position)
        
        Args:
            x: X coordinate
            y: Y coordinate
        
        Raises:
            WorldError: If object cannot be removed
            ValidationError: If coordinates are invalid
        )pbdoc")
        .def("isValidPosition", [](SimpleWorld& self, float x, float y) {
            try {
                validation::check_bounds("position x", x, 0.0f, 1000.0f);
                validation::check_bounds("position y", y, 0.0f, 1000.0f);
                return self.isValidPosition(x, y);
            } catch (const ValidationError& e) {
                throw;
            }
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid)
        
        Args:
            x: X coordinate
            y: Y coordinate
        
        Returns:
            True if position is valid, False otherwise
        
        Raises:
            ValidationError: If coordinates are invalid
        )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(Get world width)
        
        Returns:
            World width in cells
        )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(Get world height)
        
        Returns:
            World height in cells
        )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get maximum agent energy)
        
        Returns:
            Maximum energy level
        )pbdoc")
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            try {
                validation::check_bounds("max energy", e, 0.0f, 1000.0f);
                self.setMaxEnergy(e);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw WorldError(std::string("Failed to set max energy: ") + e.what());
            }
        }, py::arg("e"), R"pbdoc(Set maximum agent energy)
        
        Args:
            e: Maximum energy level
        
        Raises:
            WorldError: If energy cannot be set
            ValidationError: If energy is invalid
        )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate)
        
        Returns:
            Energy decay rate per timestep
        )pbdoc")
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            try {
                validation::check_bounds("energy decay rate", r, 0.0f, 1.0f);
                self.setEnergyDecayRate(r);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw WorldError(std::string("Failed to set energy decay rate: ") + e.what());
            }
        }, py::arg("r"), R"pbdoc(Set energy decay rate)
        
        Args:
            r: Energy decay rate (0.0 to 1.0)
        
        Raises:
            WorldError: If decay rate cannot be set
            ValidationError: If rate is invalid
        )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get current simulation time)
        
        Returns:
            Current simulation time
        )pbdoc")
        .def("setRandomSeed", [](SimpleWorld& self, uint64_t seed) {
            try {
                validation::check_bounds("random seed", seed, 0ULL, UINT64_MAX);
                self.setRandomSeed(seed);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw WorldError(std::string("Failed to set random seed: ") + e.what());
            }
        }, py::arg("seed"), R"pbdoc(Set random seed for reproducible simulation)
        
        Args:
            seed: Random seed value
        
        Raises:
            WorldError: If seed cannot be set
            ValidationError: If seed is invalid
        )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed)
        
        Returns:
            Current random seed
        )pbdoc")
        .def("getObjectCount", [](const SimpleWorld& self) {
            // Get all objects and count them (if available)
            // This is a helper method since SimpleWorld doesn't expose getObjects directly
            return 0; // Placeholder - would need to be implemented in SimpleWorld
        }, R"pbdoc(Get number of objects in world)
        
        Returns:
            Number of objects
        )pbdoc")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, R"pbdoc(Get agent position)
        
        Returns:
            Tuple of (x, y) coordinates
        )pbdoc")
        .def("getAgentEnergy", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.energy;
        }, R"pbdoc(Get agent energy)
        
        Returns:
            Current energy level
        )pbdoc")
        .def("isAgentAlive", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.health > 0;
        }, R"pbdoc(Check if agent is alive)
        
        Returns:
            True if agent health > 0
        )pbdoc")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " cells, time=" +
                   std::to_string(world.getSimulationTime()) + ">";
        });

    // Brain class binding with improved error handling
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)
    
    The Brain class is the core neural simulation engine that processes sensory
    input, maintains internal state, and produces motor actions. It contains
    neurons, synapses, regions, and integrates all neural systems.
    "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), R"pbdoc(Create a Brain with configuration)
        
        Args:
            config: Configuration object
        
        Raises:
            ConfigError: If config is invalid
            BrainError: If brain cannot be initialized
        )pbdoc")
        .def("initialize", &Brain::initialize, R"pbdoc(Initialize the brain with configuration)
        
        Raises:
            BrainError: If brain cannot be initialized
        )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), R"pbdoc(Perform a simulation step)
        
        Args:
            currentStep: Current simulation step
        
        Raises:
            BrainError: If step fails
        )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), R"pbdoc(Perform a simulation step with timestamp)
        
        Args:
            currentStep: Current simulation step
            currentTime: Current simulation time
        
        Raises:
            BrainError: If step fails
        )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), R"pbdoc(Inject sensory input into the brain)
        
        Args:
            input: Sensory input to inject
        
        Raises:
            BrainError: If input cannot be processed
            ValidationError: If input is invalid
        )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron)
        
        Args:
            neuron: Neuron ID
            current: Current to inject
        
        Raises:
            BrainError: If current cannot be injected
            ValidationError: If parameters are invalid
        )pbdoc")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, float current) {
            try {
                validation::check_allowed("neuron type", type,
                                         {NeuronType::Excitatory, NeuronType::Inhibitory,
                                          NeuronType::Modulatory, NeuronType::Sensory,
                                          NeuronType::Motor, NeuronType::Internal});
                validation::validate_synaptic_weight(current);
                self.injectCurrentToNeurons(type, current);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw BrainError(std::string("Failed to inject current: ") + e.what());
            }
        }, py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type)
        
        Args:
            type: Neuron type
            current: Current to inject
        
        Raises:
            BrainError: If current cannot be injected
            ValidationError: If parameters are invalid
        )pbdoc")
        .def("produceAction", &Brain::produceAction, R"pbdoc(Produce motor action based on neural activity)
        
        Returns:
            Motor action based on brain activity
        
        Raises:
            BrainError: If action cannot be produced
        )pbdoc")
        .def("reset", &Brain::reset, R"pbdoc(Reset brain state)
        
        Resets the brain to its initial state.
        
        Raises:
            BrainError: If brain cannot be reset
        )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"), R"pbdoc(Save brain state to file)
        
        Args:
            filepath: Path where to save brain state
        
        Raises:
            BrainError: If brain cannot be saved
            ValidationError: If filepath is invalid
        )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"), R"pbdoc(Load brain state from file)
        
        Args:
            filepath: Path to load brain state from
        
        Raises:
            BrainError: if brain cannot be loaded
            ValidationError: If filepath is invalid
        )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", R"pbdoc(Add a new neural region)
        
        Args:
            name: Region name (optional)
        
        Returns:
            Region ID
        
        Raises:
            BrainError: If region cannot be added
        )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, R"pbdoc(Get a region by ID)
        
        Args:
            id: Region ID
        
        Returns:
            Neural region
        
        Raises:
            BrainError: If region cannot be found
        )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount, R"pbdoc(Get the number of regions)
        
        Returns:
            Number of regions
        )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds, R"pbdoc(Get all region IDs)
        
        Returns:
            List of region IDs
        )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, R"pbdoc(Get all regions)
        
        Returns:
            List of all regions
        )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, R"pbdoc(Get total neuron count across all regions)
        
        Returns:
            Total number of neurons
        )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, R"pbdoc(Get total synapse count across all regions)
        
        Returns:
            Total number of synapses
        )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, R"pbdoc(Get count of active neurons)
        
        Returns:
            Number of active neurons
        )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, R"pbdoc(Get count of currently firing neurons)
        
        Returns:
            Number of firing neurons
        )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, R"pbdoc(Get average firing rate across all neurons)
        
        Returns:
            Average firing rate
        )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, R"pbdoc(Get excitation/inhibition balance ratio)
        
        Returns:
            Excitation/inhibition ratio
        )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, R"pbdoc(Get total spike count)
        
        Returns:
            Total number of spikes
        )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)
        
        Returns:
            Current developmental stage
        )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), R"pbdoc(Set developmental stage)
        
        Args:
            stage: Developmental stage
        
        Raises:
            ValidationError: If stage is invalid
        )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, R"pbdoc(Get the configuration)
        
        Returns:
            Brain configuration
        )pbdoc")
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log brain status)
        
        Logs brain status information.
        )pbdoc")
        .def("getNeuronCountByType", [](const Brain& self, NeuronType type) {
            try {
                validation::check_allowed("neuron type", type,
                                         {NeuronType::Excitatory, NeuronType::Inhibitory,
                                          NeuronType::Modulatory, NeuronType::Sensory,
                                          NeuronType::Motor, NeuronType::Internal});
                // This is a helper since Brain doesn't expose getNeuronCountByType directly
                // Would need to be implemented in Brain class
                return 0;
            } catch (const ValidationError& e) {
                throw;
            }
        }, py::arg("type"), R"pbdoc(Get neuron count by type)
        
        Args:
            type: Neuron type
        
        Returns:
            Number of neurons of the specified type
        
        Raises:
            ValidationError: If type is invalid
        )pbdoc")
        .def("getMemoryUsage", [](const Brain& self) {
            // Get memory usage statistics (helper)
            // Would need to be implemented in Brain class
            return 0.0;
        }, R"pbdoc(Get memory usage)
        
        Returns:
            Memory usage in MB
        )pbdoc")
        .def("getSimulationStats", [](const Brain& self) {
            // Get simulation statistics (helper)
            // Would need to be implemented in Brain class
            std::map<std::string, double> stats;
            return stats;
        }, R"pbdoc(Get simulation statistics)
        
        Returns:
            Dictionary of simulation statistics
        )pbdoc")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses, " +
                   std::to_string(brain.getRegionCount()) + " regions>";
        });

    // AgentBrain class binding with improved error handling
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
    
    The AgentBrain class provides an interface between the NLM brain and the
    world, handling sensory input processing and motor command generation.
    "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), R"pbdoc(Create an AgentBrain with brain)
        
        Args:
            brain: Brain instance to interface with
        
        Raises:
            BrainError: If brain is invalid
        )pbdoc")
        .def("initialize", [](AgentBrain& self, const SimpleWorld& world) {
            try {
                self.initialize(world);
            } catch (const std::exception& e) {
                throw AgentError(std::string("Failed to initialize agent brain: ") + e.what());
            }
        }, py::arg("world"), R"pbdoc(Initialize with world)
        
        Args:
            world: World to initialize with
        
        Raises:
            AgentError: If initialization fails
        )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, R"pbdoc(Get expected sensory input size)
        
        Returns:
            Expected sensory input size
        )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, R"pbdoc(Get expected motor output size)
        
        Returns:
            Expected motor output size
        )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)
        
        Args:
            percept: Sensory percept to process
        
        Raises:
            AgentError: If percept cannot be processed
            ValidationError: If percept is invalid
        )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, R"pbdoc(Decode brain motor activity into motor command)
        
        Returns:
            Motor command derived from brain activity
        )pbdoc")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            try {
                validation::validate_synaptic_weight(reward);
                validation::validate_synaptic_weight(predictedReward);
                self.applyRewardModulation(reward, predictedReward);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw AgentError(std::string("Failed to apply reward modulation: ") + e.what());
            }
        }, py::arg("reward"), py::arg("predictedReward"), R"pbdoc(Apply reward-based neuromodulation)
        
        Args:
            reward: Actual reward received
            predictedReward: Predicted reward
        
        Raises:
            AgentError: If modulation cannot be applied
            ValidationError: If rewards are invalid
        )pbdoc")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            try {
                validation::check_bounds("timestep", timestep, 0.001, 1.0);
                self.updateDevelopment(timestep);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw AgentError(std::string("Failed to update development: ") + e.what());
            }
        }, py::arg("timestep"), R"pbdoc(Update development system)
        
        Args:
            timestep: Time step for update
        
        Raises:
            AgentError: If development cannot be updated
            ValidationError: If timestep is invalid
        )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)
        
        Returns:
            Current developmental stage
        )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, R"pbdoc(Get current neuromodulation level)
        
        Returns:
            Current neuromodulation level
        )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, R"pbdoc(Get curiosity level)
        
        Returns:
            Current curiosity level
        )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, R"pbdoc(Get novelty level)
        
        Returns:
            Current novelty level
        )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError, R"pbdoc(Get prediction error)
        
        Returns:
            Current prediction error
        )pbdoc")
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent for new episode)
        
        Resets the agent for a new episode.
        
        Raises:
            AgentError: If agent cannot be reset
        )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, R"pbdoc(Get the underlying brain)
        
        Returns:
            Underlying Brain instance
        )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), R"pbdoc(Enable or disable reward modulation)
        
        Args:
            enable: Whether to enable reward modulation
        )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), R"pbdoc(Enable or disable structural plasticity)
        
        Args:
            enable: Whether to enable structural plasticity
        )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), R"pbdoc(Enable or disable development)
        
        Args:
            enable: Whether to enable development
        )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), R"pbdoc(Enable or disable curiosity)
        
        Args:
            enable: Whether to enable curiosity
        )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, R"pbdoc(Check if reward modulation is enabled)
        
        Returns:
            True if reward modulation is enabled
        )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, R"pbdoc(Check if structural plasticity is enabled)
        
        Returns:
            True if structural plasticity is enabled
        )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, R"pbdoc(Check if development is enabled)
        
        Returns:
            True if development is enabled
        )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, R"pbdoc(Check if curiosity is enabled)
        
        Returns:
            True if curiosity is enabled
        )pbdoc")
        .def("getLearningStats", [](const AgentBrain& self) {
            // Get learning statistics (helper)
            // Would need to be implemented in AgentBrain class
            std::map<std::string, double> stats;
            return stats;
        }, R"pbdoc(Get learning statistics)
        
        Returns:
            Dictionary of learning statistics
        )pbdoc")
        .def("getPerformanceMetrics", [](const AgentBrain& self) {
            // Get performance metrics (helper)
            // Would need to be implemented in AgentBrain class
            std::map<std::string, double> metrics;
            return metrics;
        }, R"pbdoc(Get performance metrics)
        
        Returns:
            Dictionary of performance metrics
        )pbdoc")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: " + std::to_string(agent.getSensoryInputSize()) +
                   " sensory input, " + std::to_string(agent.getMotorOutputSize()) +
                   " motor output>";
        });

} // namespace nlm
