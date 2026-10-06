#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace py = pybind11;
namespace nlm {

// Python wrapper for Config class with enhanced error handling
void bindConfig(py::module_& m) {
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                bool success = self.loadFromFile(filepath);
                if (!success) {
                    throw std::runtime_error("Failed to load configuration from file: " + filepath);
                }
                return success;
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Config loadFromFile failed: ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Load configuration from a JSON file)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                bool success = self.loadFromArgs(argc, argv);
                if (!success) {
                    throw std::runtime_error("Failed to load configuration from command line arguments");
                }
                return success;
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Config loadFromArgs failed: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"), R"pbdoc(Load configuration from command line arguments)pbdoc")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                bool success = self.saveToFile(filepath);
                if (!success) {
                    throw std::runtime_error("Failed to save configuration to file: " + filepath);
                }
                return success;
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Config saveToFile failed: ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Save configuration to a JSON file)pbdoc")
        .def("has", [](Config& self, const std::string& key) {
            try {
                return self.has(key);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Config has failed: ") + e.what());
            }
        }, py::arg("key"), R"pbdoc(Check if a configuration key exists)pbdoc")
        .def("getKeys", [](Config& self) {
            try {
                return self.getKeys();
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Config getKeys failed: ") + e.what());
            }
        }, R"pbdoc(Get all configuration keys)pbdoc")
        .def("clear", [](Config& self) {
            try {
                self.clear();
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Config clear failed: ") + e.what());
            }
        }, R"pbdoc(Clear all configuration entries)pbdoc")
        .def("summary", [](Config& self) {
            try {
                return self.summary();
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Config summary failed: ") + e.what());
            }
        }, R"pbdoc(Get a summary string of the configuration)pbdoc")
        .def("__repr__", [](const Config& self) {
            return "<Config: " + self.summary() + ">";
        });
}

// Python wrapper for SensoryInput class with enhanced error handling
void bindSensoryInput(py::module_& m) {
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input)pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a vector)pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality)pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp)pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), R"pbdoc(Set the timestamp)pbdoc");
}

// Python wrapper for Vision class with enhanced error handling
void bindVision(py::module_& m) {
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                self.setData(data);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Vision setData failed: ") + e.what());
            }
        }, py::arg("data"), R"pbdoc(Set data for vision sensory input)pbdoc")
        .def("getWidth", &Vision::getWidth, R"pbdoc(Get vision width)pbdoc")
        .def("getHeight", &Vision::getHeight, R"pbdoc(Get vision height)pbdoc")
        .def("getChannels", &Vision::getChannels, R"pbdoc(Get vision channels)pbdoc");
}

// Python wrapper for Audio class with enhanced error handling
void bindAudio(py::module_& m) {
    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                self.setData(data);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Audio setData failed: ") + e.what());
            }
        }, py::arg("data"), R"pbdoc(Set data for audio sensory input)pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), R"pbdoc(Set audio sample rate)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(Get audio sample rate)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(Get number of audio samples)pbdoc");
}

// Python wrapper for InternalSignals class with enhanced error handling
void bindInternalSignals(py::module_& m) {
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), R"pbdoc(Add a signal value)pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(Clear all signals)pbdoc");
}

// Python wrapper for Action class with enhanced error handling
void bindAction(py::module_& m) {
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, R"pbdoc(Get action type)pbdoc")
        .def("setType", &Action::setType, py::arg("type"), R"pbdoc(Set action type)pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(Get action parameters)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"), R"pbdoc(Set action parameters)pbdoc")
        .def("getName", &Action::getName, R"pbdoc(Get action name)pbdoc")
        .def("clone", &Action::clone, R"pbdoc(Clone this action)pbdoc");
}

// Python wrapper for WorldObject class with enhanced error handling
void bindWorldObject(py::module_& m) {
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, R"pbdoc(X position)pbdoc")
        .def_readwrite("y", &WorldObject::y, R"pbdoc(Y position)pbdoc")
        .def_readwrite("radius", &WorldObject::radius, R"pbdoc(Object radius)pbdoc")
        .def_readwrite("type", &WorldObject::type, R"pbdoc(Object type)pbdoc")
        .def_readwrite("value", &WorldObject::value, R"pbdoc(Object value)pbdoc")
        .def_readwrite("active", &WorldObject::active, R"pbdoc(Object active state)pbdoc");
}

// Python wrapper for AgentBody class with enhanced error handling
void bindAgentBody(py::module_& m) {
    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, R"pbdoc(X position)pbdoc")
        .def_readwrite("y", &AgentBody::y, R"pbdoc(Y position)pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation, R"pbdoc(Heading angle in radians)pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX, R"pbdoc(X velocity)pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY, R"pbdoc(Y velocity)pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, R"pbdoc(Angular velocity)pbdoc")
        .def_readwrite("energy", &AgentBody::energy, R"pbdoc(Current energy)pbdoc")
        .def_readwrite("health", &AgentBody::health, R"pbdoc(Current health)pbdoc")
        .def_readwrite("age", &AgentBody::age, R"pbdoc(Age in simulation steps)pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving, R"pbdoc(Moving state)pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning, R"pbdoc(Turning state)pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, R"pbdoc(Time since last action)pbdoc")
        .def("reset", &AgentBody::reset, R"pbdoc(Reset agent body to initial state)pbdoc");
}

// Python wrapper for ActionResult class with enhanced error handling
void bindActionResult(py::module_& m) {
    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, R"pbdoc(Reward received)pbdoc")
        .def_readwrite("success", &ActionResult::success, R"pbdoc(Whether action was successful)pbdoc")
        .def_readwrite("message", &ActionResult::message, R"pbdoc(Result message)pbdoc");
}

// Python wrapper for SensoryPercept class with enhanced error handling
void bindSensoryPercept(py::module_& m) {
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, R"pbdoc(Get vision data)pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), R"pbdoc(Set vision data)pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, R"pbdoc(Get vision width)pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, R"pbdoc(Get vision height)pbdoc")
        .def("getTouch", &SensoryPercept::getTouch, R"pbdoc(Get touch data)pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), R"pbdoc(Set touch data)pbdoc")
        .def("getInternal", &SensoryPercept::getInternal, R"pbdoc(Get internal signals)pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), R"pbdoc(Set internal signals)pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception, R"pbdoc(Get proprioception)pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), R"pbdoc(Set proprioception)pbdoc")
        .def("getAudio", &SensoryPercept::getAudio, R"pbdoc(Get audio data)pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), R"pbdoc(Set audio data)pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals, R"pbdoc(Get all sensory signals)pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp, R"pbdoc(Get percept timestamp)pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), R"pbdoc(Set percept timestamp)pbdoc");
}

// Python wrapper for SimpleWorld class with enhanced error handling
void bindSimpleWorld(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), R"pbdoc(Configure world dimensions and agent vision)pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), R"pbdoc(Set agent starting position)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"), R"pbdoc(Update world state)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), R"pbdoc(Apply motor command to world)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, R"pbdoc(Get sensory percept for agent)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, R"pbdoc(Get agent body state)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), R"pbdoc(Add object to world)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), R"pbdoc(Remove object from world)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid)pbdoc")
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(Get world width)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(Get world height)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get maximum agent energy)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), R"pbdoc(Set maximum agent energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), R"pbdoc(Set energy decay rate)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get simulation time)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), R"pbdoc(Set random seed)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed)pbdoc");
}

// Python wrapper for Brain class with enhanced error handling
void bindBrain(py::module_& m) {
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), R"pbdoc(Create a brain with configuration)pbdoc")
        .def("initialize", &Brain::initialize, R"pbdoc(Initialize the brain with configuration)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"), R"pbdoc(Perform a simulation step)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), R"pbdoc(Perform a simulation step with timestamp)pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), R"pbdoc(Inject sensory input into the brain)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type)pbdoc")
        .def("produceAction", &Brain::produceAction, R"pbdoc(Produce motor action based on neural activity)pbdoc")
        .def("reset", &Brain::reset, R"pbdoc(Reset brain state)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", R"pbdoc(Add a new neural region)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get a region by ID)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount, R"pbdoc(Get the number of regions)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds, R"pbdoc(Get all region IDs)pbdoc")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, R"pbdoc(Get all regions)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, R"pbdoc(Get total neuron count across all regions)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, R"pbdoc(Get total synapse count across all regions)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, R"pbdoc(Get count of active neurons)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, R"pbdoc(Get count of currently firing neurons)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, R"pbdoc(Get average firing rate across all neurons)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, R"pbdoc(Get excitation/inhibition balance ratio)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, R"pbdoc(Get total spike count)pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, R"pbdoc(Get number of pending spike events)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), R"pbdoc(Set developmental stage)pbdoc")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, R"pbdoc(Get the configuration)pbdoc")
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log brain status)pbdoc");
}

// Python wrapper for AgentBrain class with enhanced error handling
void bindAgentBrain(py::module_& m) {
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), R"pbdoc(Create an agent brain interface)pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), R"pbdoc(Initialize with world)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, R"pbdoc(Get expected sensory input size)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, R"pbdoc(Get expected motor output size)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, R"pbdoc(Decode brain motor activity into motor command)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), R"pbdoc(Apply reward-based neuromodulation)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), R"pbdoc(Update development system)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, R"pbdoc(Get current neuromodulation level)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent for new episode)pbdoc")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, R"pbdoc(Get the underlying brain)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), R"pbdoc(Enable/disable reward modulation)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), R"pbdoc(Enable/disable structural plasticity)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), R"pbdoc(Enable/disable development)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), R"pbdoc(Enable/disable curiosity)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, R"pbdoc(Check if reward modulation is enabled)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, R"pbdoc(Check if structural plasticity is enabled)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, R"pbdoc(Check if development is enabled)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, R"pbdoc(Check if curiosity is enabled)pbdoc");
}

// Helper functions to create default objects
void bindHelperFunctions(py::module_& m) {
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)pbdoc");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world)pbdoc");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Main Python module initialization
PYBIND11_MODULE(pynlm, m) {
    // Bind all classes
    bindConfig(m);
    bindSensoryInput(m);
    bindVision(m);
    bindAudio(m);
    bindInternalSignals(m);
    bindAction(m);
    bindWorldObject(m);
    bindAgentBody(m);
    bindActionResult(m);
    bindSensoryPercept(m);
    bindSimpleWorld(m);
    bindBrain(m);
    bindAgentBrain(m);
    
    // Bind helper functions
    bindHelperFunctions(m);
    
    // Add module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Event-driven spiking neural computation
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction and planning systems
        - Cognitive development stages
        - Structural plasticity
        - Checkpoint saving and loading
        - Comprehensive error handling with descriptive messages
    )pbdoc";
}

} // namespace nlm
