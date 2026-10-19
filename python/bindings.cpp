#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Key improvements:
        - Python-friendly API with snake_case method names
        - Property-style accessors for common properties
        - Enhanced error handling with Python exceptions
        - Convenience methods for common operations
        - Neural planner integration
        - Concept formation integration
        - Checkpoint system
        - Development system support
        - Example scripts and templates
    )pbdoc";

    // Register Python exception for runtime errors
    py::register_exception<std::runtime_error>(m, "NLMError", PyExc_RuntimeError);

    // =====================================================
    // Basic Type Definitions
    // =====================================================
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); });

    py::enum_<NeuronType>(m, "NeuronType")
        .value("EXCITATORY", NeuronType::Excitatory)
        .value("INHIBITORY", NeuronType::Inhibitory)
        .value("MODULATORY", NeuronType::Modulatory)
        .value("SENSORY", NeuronType::Sensory)
        .value("MOTOR", NeuronType::Motor)
        .value("INTERNAL", NeuronType::Internal)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
        .value("MOVE_FORWARD", ActionType::MoveForward)
        .value("MOVE_BACKWARD", ActionType::MoveBackward)
        .value("MOVE_LEFT", ActionType::MoveLeft)
        .value("MOVE_RIGHT", ActionType::MoveRight)
        .value("TURN_LEFT", ActionType::TurnLeft)
        .value("TURN_RIGHT", ActionType::TurnRight)
        .value("LOOK", ActionType::Look)
        .value("INTERACT", ActionType::Interact)
        .value("EAT", ActionType::Eat)
        .value("DRINK", ActionType::Drink)
        .value("REST", ActionType::Rest)
        .value("WAIT", ActionType::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("EMPTY", WorldObjectType::Empty)
        .value("RESOURCE", WorldObjectType::Resource)
        .value("HAZARD", WorldObjectType::Hazard)
        .value("WALL", WorldObjectType::Wall)
        .value("MARKER", WorldObjectType::Marker)
        .export_values();

    // =====================================================
    // Configuration System
    // =====================================================
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
        
        Configuration management for the NLM brain system.
        
        Example:
        ```python
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", 1000)
        brain = pynlm.create_brain(config)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"))
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"))
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"))
        .def("has", &Config::has, py::arg("key"))
        .def("get_keys", &Config::getKeys)
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            if (key.find("neuron_count") != std::string::npos || 
                key.find("synapse_count") != std::string::npos) {
                int intValue = std::stoi(value);
                return self.set(key, intValue);
            } else if (key.find("rate") != std::string::npos || 
                       key.find("scale") != std::string::npos ||
                       key.find("learning") != std::string::npos) {
                float floatValue = std::stof(value);
                return self.set(key, floatValue);
            } else if (key.find("enable") != std::string::npos) {
                bool boolValue = (value == "true" || value == "1");
                return self.set(key, boolValue);
            }
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("get", [](const Config& self, const std::string& key) {
            try {
                auto variant = self.get(key);
                if (std::holds_alternative<int>(variant)) {
                    return py::cast(std::get<int>(variant));
                } else if (std::holds_alternative<float>(variant)) {
                    return py::cast(std::get<float>(variant));
                } else if (std::holds_alternative<bool>(variant)) {
                    return py::cast(std::get<bool>(variant));
                } else if (std::holds_alternative<std::string>(variant)) {
                    return py::cast(std::get<std::string>(variant));
                }
            } catch (...) {
                throw py::index_error("Configuration key not found: " + key);
            }
            throw py::index_error("Configuration key not found: " + key);
        }, py::arg("key"))
        .def("clear", &Config::clear)
        .def("summary", &Config::summary)
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // =====================================================
    // Sensory and Motor Systems
    // =====================================================
    
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 0, py::arg("height") = 0,
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("get_data", &Vision::getData);

    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate") = 0, py::arg("num_samples") = 0)
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("get_data", &Audio::getData);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("type") = WorldObjectType::Empty,
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward") = 0.0f, py::arg("success") = false, py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_good", [](const ActionResult& result) {
            return result.success && result.reward > 0;
        });

    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // =====================================================
    // World Simulation
    // =====================================================
    
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_object_at", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        // World manipulation helpers
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f)
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float damage = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 1.0f);

    // =====================================================
    // Core Brain - Enhanced Python API
    // =====================================================
    
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("current_step"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"))
        .def("run_simulation", [](Brain& self, int num_steps, bool log_stats = false) {
            for (int i = 0; i < num_steps; ++i) {
                self.step(i);
                if (log_stats && (i % 100 == 0 || i == num_steps - 1)) {
                    self.logStatus();
                }
            }
        }, py::arg("num_steps"), py::arg("log_stats") = false)
        .def("reset_brain", &Brain::reset)
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"))
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron_id"), py::arg("current"))
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("neuron_type"), py::arg("current"))
        .def("produce_action", &Brain::produceAction)
        .def("reset", &Brain::reset)
        .def("save", &Brain::save, py::arg("filepath"))
        .def("load", &Brain::load, py::arg("filepath"))
        .def("add_region", &Brain::addRegion, py::arg("name") = "")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal)
        .def("get_region_count", &Brain::getRegionCount)
        .def("get_region_ids", &Brain::getRegionIds)
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal)
        // Property-style accessors for common properties
        .def_property_readonly("total_neuron_count", &Brain::getTotalNeuronCount)
        .def_property_readonly("total_synapse_count", &Brain::getTotalSynapseCount)
        .def_property_readonly("active_neuron_count", &Brain::getActiveNeuronCount)
        .def_property_readonly("firing_neuron_count", &Brain::getFiringNeuronCount)
        .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount)
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate)
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio)
        .def("get_plasticity_systems", [](Brain& self) {
            return std::make_tuple(self.getSTDP(), self.getHebbian(), self.getStructuralPlasticity());
        })
        .def("get_memory_systems", [](Brain& self) {
            return std::make_tuple(self.getWorkingMemory(), self.getEpisodicMemory(), self.getAssociativeMemory());
        })
        .def("get_prediction_system", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal)
        .def("get_planner", &Brain::getPlanner,
             py::return_value_policy::reference_internal)
        .def("get_concept_formation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal)
        .def("get_development_system", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal)
        .def("get_neuromodulators", [](Brain& self) {
            return std::make_tuple(self.getDopamine(), self.getCuriosity(), self.getNovelty(), self.getPredictionErrorSignal());
        })
        .def("get_spike_system", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal)
        .def("get_current_brain_state", &Brain::getCurrentBrainState)
        .def("get_last_planned_action", &Brain::getLastPlannedAction)
        .def("get_last_concept_id", &Brain::getLastConceptId)
        .def("get_developmental_stage", &Brain::getDevelopmentalStage)
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"))
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal)
        .def("get_random_generator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal)
        .def("log_status", &Brain::logStatus)
        .def("__repr__", [](const Brain& brain) {
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) + ">";
        });

    // =====================================================
    // AgentBrain Interface
    // =====================================================
    
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize)
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize)
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand)
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"))
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage)
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel)
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel)
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel)
        .def("get_prediction_error", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal)
        // Subsystem enable/disable methods
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        // Subsystem status queries
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        // Agent helper methods
        .def("train_agent", [](AgentBrain& self, SimpleWorld& world, int num_steps) {
            for (int i = 0; i < num_steps; ++i) {
                world.update(0.1);
                self.processSensoryInput(world.getSensoryPercept());
                self.getBrain()->step(i);
                auto motor_cmd = self.decodeMotorCommand();
                auto result = world.applyMotorCommand(motor_cmd, world.getSimulationTime());
                self.applyRewardModulation(result.reward, 0.0);
                self.updateDevelopment(0.1);
            }
        }, py::arg("world"), py::arg("num_steps"))
        .def("evaluate_agent", [](AgentBrain& self, SimpleWorld& world, int num_steps) {
            float total_reward = 0.0f;
            for (int i = 0; i < num_steps; ++i) {
                world.update(0.1);
                self.processSensoryInput(world.getSensoryPercept());
                self.getBrain()->step(i);
                auto motor_cmd = self.decodeMotorCommand();
                auto result = world.applyMotorCommand(motor_cmd, world.getSimulationTime());
                total_reward += result.reward;
                self.applyRewardModulation(result.reward, 0.0);
                self.updateDevelopment(0.1);
            }
            return total_reward / num_steps;
        }, py::arg("world"), py::arg("num_steps"))
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain connected to " + std::to_string(agent.getBrain()->getTotalNeuronCount()) + " neurons>";
        });

    // =====================================================
    // Neural Planner Integration
    // =====================================================
    
    py::class_<NeuralPlanner>(m, "NeuralPlanner")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("plan_action", &NeuralPlanner::planAction,
             py::arg("current_state"), py::arg("target_reward") = 0.5f)
        .def("evaluate_sequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("start_state"))
        .def("get_planning_depth", &NeuralPlanner::getPlanningDepth)
        .def("set_planning_depth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("get_planning_confidence", &NeuralPlanner::getPlanningConfidence)
        .def("update_plan_quality", &NeuralPlanner::updatePlanQuality,
             py::arg("planned_actions"), py::arg("actual_actions"), py::arg("actual_reward"))
        .def("clear_cache", &NeuralPlanner::clearCache)
        .def("set_action_quality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"))
        .def("get_current_goal", &NeuralPlanner::getCurrentGoal)
        .def("set_current_goal", &NeuralPlanner::setCurrentGoal, py::arg("goal"))
        .def("was_recent_plan_successful", &NeuralPlanner::wasRecentPlanSuccessful)
        .def("__repr__", [](const NeuralPlanner& planner) {
            return "<NeuralPlanner depth=" + std::to_string(planner.getPlanningDepth()) +
                   " confidence=" + std::to_string(planner.getPlanningConfidence()) + ">";
        });

    // =====================================================
    // Concept Formation Integration
    // =====================================================
    
    py::class_<ConceptFormation>(m, "ConceptFormation")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("present_experience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("current_time"))
        .def("get_matching_concept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarity_threshold") = 0.7f)
        .def("get_concepts", &ConceptFormation::getConcepts)
        .def("get_concept", &ConceptFormation::getConcept, py::arg("concept_id"),
             py::return_value_policy::reference_internal)
        .def("get_concept_prototype", &ConceptFormation::getConceptPrototype,
             py::arg("concept_id"))
        .def("get_concept_instances", &ConceptFormation::getConceptInstances,
             py::arg("concept_id"))
        .def("get_concept_stability", &ConceptFormation::getConceptStability,
             py::arg("concept_id"))
        .def("get_concept_count", &ConceptFormation::getConceptCount)
        .def("merge_concepts", &ConceptFormation::mergeConcepts,
             py::arg("concept_a"), py::arg("concept_b"))
        .def("update_concept", &ConceptFormation::updateConcept,
             py::arg("concept_id"), py::arg("new_pattern"), py::arg("features"), py::arg("reward"))
        .def("compute_similarity", &ConceptFormation::computeSimilarity,
             py::arg("a"), py::arg("b"))
        .def("is_novel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarity_threshold") = 0.7f)
        .def("find_concept_for_pattern", &ConceptFormation::findConceptForPattern,
             py::arg("pattern"))
        .def("clear", &ConceptFormation::clear)
        .def("get_generalization_ability", &ConceptFormation::getGeneralizationAbility,
             py::arg("concept_id"))
        .def("set_formation_threshold", &ConceptFormation::setFormationThreshold,
             py::arg("threshold"))
        .def("set_stability_window", &ConceptFormation::setStabilityWindow,
             py::arg("window"))
        .def("__repr__", [](const ConceptFormation& cf) {
            return "<ConceptFormation concepts=" + std::to_string(cf.getConceptCount()) + ">";
        });

    // =====================================================
    // Development System
    // =====================================================
    
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem")
        .def(py::init<>())
        .def("get_stage", &DevelopmentSystem::getStage)
        .def("set_stage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advance_stage", &DevelopmentSystem::advanceStage)
        .def("get_stage_name", &DevelopmentSystem::getStageName)
        .def("update", [](DevelopmentSystem& self, Brain* brain, SimulationStep currentStep) {
            self.update(brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"))
        .def("update", [](DevelopmentSystem& self, Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
            self.update(brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("get_plasticity_modifier", &DevelopmentSystem::getPlasticityModifier)
        .def("is_critical_period", &DevelopmentSystem::isCriticalPeriod)
        .def("get_critical_period_progress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("get_developmental_age", &DevelopmentSystem::getDevelopmentalAge)
        .def("set_developmental_age", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"))
        .def("__repr__", [](const DevelopmentSystem& dev) {
            return "<DevelopmentSystem stage=" + std::to_string(static_cast<int>(dev.getStage())) +
                   " age=" + std::to_string(dev.getDevelopmentalAge()) + ">";
        });

    // =====================================================
    // Factory Functions
    // =====================================================
    
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    });

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"));

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    });

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"));

    m.def("create_planner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    });

    m.def("create_concept_formation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    });

    m.def("create_development_system", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    });

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // =====================================================
    // Examples and Templates
    // =====================================================
    
    py::module_ examples_module = m.def_submodule("examples", "Example scripts and templates");
    
    examples_module.def("simple_brain_example", [](int num_steps) {
        Config config;
        Brain brain(std::make_shared<Config>(config));
        brain.initialize();
        
        for (int i = 0; i < num_steps; ++i) {
            brain.step(i);
        }
        
        return std::make_tuple(brain.getTotalNeuronCount(), brain.getTotalSynapseCount());
    }, py::arg("num_steps") = 100);
    
    examples_module.def("agent_in_world_example", [](int num_steps) {
        Config config;
        Brain brain(std::make_shared<Config>(config));
        brain.initialize();
        
        SimpleWorld world;
        world.configure(20, 20, 8, 8);
        world.reset();
        world.setAgentStart(10.0, 10.0);
        
        AgentBrain agent(brain);
        agent.initialize(world);
        
        float total_reward = 0.0f;
        for (int i = 0; i < num_steps; ++i) {
            world.update(0.1);
            agent.processSensoryInput(world.getSensoryPercept());
            brain.step(i);
            auto motor_cmd = agent.decodeMotorCommand();
            auto result = world.applyMotorCommand(motor_cmd, world.getSimulationTime());
            total_reward += result.reward;
            agent.applyRewardModulation(result.reward, 0.0);
            agent.updateDevelopment(0.1);
        }
        
        return total_reward / num_steps;
    }, py::arg("num_steps") = 500);
    
    examples_module.def("learning_example", [](int num_steps) {
        Config config;
        Brain brain(std::make_shared<Config>(config));
        brain.initialize();
        
        auto& dev_system = *brain.getDevelopmentSystem();
        dev_system.setStage(DevelopmentalStage::CriticalPeriod);
        
        for (int i = 0; i < num_steps; ++i) {
            brain.step(i);
        }
        
        return brain.getTotalSpikeCount();
    }, py::arg("num_steps") = 1000);
    
    examples_module.def("advanced_planner_example", [](int num_steps) {
        Config config;
        Brain brain(std::make_shared<Config>(config));
        brain.initialize();
        
        NeuralPlanner planner;
        planner.initialize(&brain);
        
        float current_reward = 0.0f;
        for (int i = 0; i < num_steps; ++i) {
            brain.step(i);
            auto action = planner.planAction(brain.getCurrentBrainState());
        }
        
        return std::make_tuple(current_reward, num_steps);
    }, py::arg("num_steps") = 100);

    // =====================================================
    // Advanced Features for Experienced Users
    // =====================================================
    
    // Configuration helpers
    m.def("config_get_int", [](const Config& config, const std::string& key) {
        auto variant = config.get(key);
        if (std::holds_alternative<int>(variant)) {
            return std::get<int>(variant);
        }
        throw py::index_error("Key is not an int: " + key);
    }, py::arg("config"), py::arg("key"));

    m.def("config_get_float", [](const Config& config, const std::string& key) {
        auto variant = config.get(key);
        if (std::holds_alternative<float>(variant)) {
            return std::get<float>(variant);
        }
        throw py::index_error("Key is not a float: " + key);
    }, py::arg("config"), py::arg("key"));

    m.def("config_get_bool", [](const Config& config, const std::string& key) {
        auto variant = config.get(key);
        if (std::holds_alternative<bool>(variant)) {
            return std::get<bool>(variant);
        }
        throw py::index_error("Key is not a bool: " + key);
    }, py::arg("config"), py::arg("key"));

    // Debug utilities
    m.def("debug_log_brain_state", [](Brain& brain) {
        brain.logStatus();
    }, py::arg("brain"));

    m.def("debug_validate_world", [](SimpleWorld& world) {
        bool valid = world.isValidPosition(world.getAgentBody().x, world.getAgentBody().y);
        return valid;
    }, py::arg("world"));

    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";

} // namespace nlm
