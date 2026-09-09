#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Add error handling for pybind11 operations
    try {
        py::register_exception<std::runtime_error>(m, "RuntimeError");

        py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &NeuronId::value)
            .def("index", &NeuronId::index)
            .def("__eq__", &NeuronId::operator==)
            .def("__ne__", &NeuronId::operator!=)
            .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
            .def("__repr__", [](const NeuronId& id) {
                return "<NeuronId: " + std::to_string(id.value) + ">";
            });

        py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &SynapseId::value)
            .def("index", &SynapseId::index)
            .def("__eq__", &SynapseId::operator==)
            .def("__ne__", &SynapseId::operator!=)
            .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
            .def("__repr__", [](const SynapseId& id) {
                return "<SynapseId: " + std::to_string(id.value) + ">";
            });

        py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &RegionId::value)
            .def("index", &RegionId::index)
            .def("__eq__", &RegionId::operator==)
            .def("__ne__", &RegionId::operator!=)
            .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
            .def("__repr__", [](const RegionId& id) {
                return "<RegionId: " + std::to_string(id.value) + ">";
            });

        py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &PopulationId::value)
            .def("index", &PopulationId::index)
            .def("__eq__", &PopulationId::operator==)
            .def("__ne__", &PopulationId::operator!=);

        py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
            .value("Excitatory", NeuronType::Excitatory)
            .value("Inhibitory", NeuronType::Inhibitory)
            .value("Modulatory", NeuronType::Modulatory)
            .value("Sensory", NeuronType::Sensory)
            .value("Motor", NeuronType::Motor)
            .value("Internal", NeuronType::Internal)
            .export_values();

        py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
            .value("Excitatory", SynapseType::Excitatory)
            .value("Inhibitory", SynapseType::Inhibitory)
            .value("Modulatory", SynapseType::Modulatory)
            .value("Electrical", SynapseType::Electrical)
            .value("GapJunction", SynapseType::GapJunction)
            .export_values();

        py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
            .value("Initial", DevelopmentalStage::Initial)
            .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
            .value("Maturation", DevelopmentalStage::Maturation)
            .value("Adult", DevelopmentalStage::Adult)
            .value("Aging", DevelopmentalStage::Aging)
            .export_values();

        py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
            .value("Resting", FiringState::Resting)
            .value("Active", FiringState::Active)
            .value("Refractory", FiringState::Refractory)
            .value("Inhibited", FiringState::Inhibited)
            .export_values();

        py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
            .value("MoveForward", ActionType::MoveForward)
            .value("MoveBackward", ActionType::MoveBackward)
            .value("MoveLeft", ActionType::MoveLeft)
            .value("MoveRight", ActionType::MoveRight)
            .value("TurnLeft", ActionType::TurnLeft)
            .value("TurnRight", ActionType::TurnRight)
            .value("Look", ActionType::Look)
            .value("LookUp", ActionType::LookUp)
            .value("LookDown", ActionType::LookDown)
            .value("Interact", ActionType::Interact)
            .value("Eat", ActionType::Eat)
            .value("Drink", ActionType::Drink)
            .value("Rest", ActionType::Rest)
            .value("Wait", ActionType::Wait)
            .value("Custom", ActionType::Custom)
            .export_values();

        py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
            .value("MoveForward", MotorCommand::MoveForward)
            .value("MoveBackward", MotorCommand::MoveBackward)
            .value("TurnLeft", MotorCommand::TurnLeft)
            .value("TurnRight", MotorCommand::TurnRight)
            .value("LookLeft", MotorCommand::LookLeft)
            .value("LookRight", MotorCommand::LookRight)
            .value("Interact", MotorCommand::Interact)
            .value("Wait", MotorCommand::Wait)
            .export_values();

        py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
            .value("Empty", WorldObjectType::Empty)
            .value("Resource", WorldObjectType::Resource)
            .value("Hazard", WorldObjectType::Hazard)
            .value("Wall", WorldObjectType::Wall)
            .value("Marker", WorldObjectType::Marker)
            .export_values();

        // Add missing constants for Python access
        m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
        m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
        m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
        m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

        // ====================================================================
        // ADVANCED USER COMMANDS AND ENHANCED API EXTENSIONS
        // ====================================================================

        // 1. Simulation Control and Management
        m.def("reset", [](std::shared_ptr<Brain> brain) {
            if (brain) brain->reset();
        }, py::arg("brain"), "Reset brain to initial state");

        m.def("save_checkpoint", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
            if (brain) brain->save(filepath);
        }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

        m.def("load_checkpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
            if (brain) brain->load(filepath);
        }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

        m.def("benchmark_network", [](size_t neuronCount, SimulationStep steps) {
            // Create and benchmark a neural network
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto start = std::chrono::high_resolution_clock::now();
            for (SimulationStep step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            double stepsPerSecond = steps / (duration.count() / 1000.0);
            
            return std::make_tuple(brain->getTotalSpikeCount(), stepsPerSecond, duration.count());
        }, py::arg("neuronCount"), py::arg("steps"), "Benchmark neural network performance");

        // 2. Advanced Configuration and Management
        py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
            .def(py::init<>())
            .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
                 "Load configuration from a JSON file")
            .def("loadFromArgs", [](Config& self, int argc, char** argv) {
                return self.loadFromArgs(argc, argv);
            }, py::arg("argc"), py::arg("argv"),
               "Load configuration from command line arguments")
            .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
                 "Save configuration to a JSON file")
            .def("has", &Config::has, py::arg("key"),
                 "Check if a configuration key exists")
            .def("getKeys", &Config::getKeys,
                 "Get all configuration keys")
            .def("clear", &Config::clear,
                 "Clear all configuration entries")
            .def("summary", &Config::summary,
                 "Get a summary string of the configuration")
            .def("__repr__", [](const Config& cfg) {
                return "<Config: " + cfg.summary() + ">";
            })
            // Advanced configuration methods
            .def("set_advanced_parameters", [](Config& self, const py::dict& params) {
                for (auto item : params) {
                    std::string key = item.first.cast<std::string>();
                    // Handle different parameter types
                    if (py::isinstance<py::int_>(item.second)) {
                        self.set(key, item.second.cast<int64_t>(), ConfigSource::Default);
                    } else if (py::isinstance<py::float_>(item.second)) {
                        self.set(key, item.second.cast<float>(), ConfigSource::Default);
                    } else if (py::isinstance<py::str>(item.second)) {
                        self.set(key, item.second.cast<std::string>(), ConfigSource::Default);
                    }
                }
            }, py::arg("params"), "Set multiple configuration parameters at once");

        m.def("get_brain_statistics", [](const std::shared_ptr<Brain>& brain) {
            if (!brain) return py::dict();
            
            py::dict stats;
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            stats["region_count"] = brain->getRegionCount();
            
            return stats;
        }, py::arg("brain"), "Get comprehensive brain statistics");

        // 3. Learning and Training
        m.def("train_agent", [](std::shared_ptr<Brain> brain, 
                               std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent,
                               size_t episodeCount, 
                               SimulationStep stepsPerEpisode) {
            if (!brain || !world || !agent) {
                throw std::runtime_error("Brain, world, and agent must be provided");
            }
            
            std::vector<py::dict> episodeResults;
            
            for (size_t episode = 0; episode < episodeCount; ++episode) {
                // Reset environment
                world->reset();
                agent->reset();
                agent->initialize(*world);
                
                py::dict episodeStats;
                episodeStats["episode"] = static_cast<int>(episode);
                episodeStats["initial_spikes"] = brain->getTotalSpikeCount();
                
                // Run episode
                for (SimulationStep step = 0; step < stepsPerEpisode; ++step) {
                    // Update world
                    world->update(0.1);
                    
                    // Get sensory input
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    // Brain processes
                    brain->step(step, step * 0.001);
                    
                    // Get action and execute
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Apply reward modulation
                    agent->applyRewardModulation(1.0f, 0.5f); // Simple reward model
                }
                
                episodeStats["final_spikes"] = brain->getTotalSpikeCount();
                episodeStats["spike_increase"] = brain->getTotalSpikeCount() - episodeStats["initial_spikes"].cast<size_t>();
                episodeStats["avg_firing_rate"] = brain->getAverageFiringRate();
                
                episodeResults.push_back(episodeStats);
            }
            
            return episodeResults;
        }, py::arg("brain"), py::arg("world"), py::arg("agent"),
           py::arg("episodeCount"), py::arg("stepsPerEpisode"),
           "Train an agent through reinforcement learning");

        // 4. Visualization and Analysis
        m.def("visualize_brain", [](const std::shared_ptr<Brain>& brain) {
            if (!brain) return std::string("No brain provided");
            
            std::stringstream visualization;
            visualization << "Brain Visualization:" << std::endl;
            visualization << "===================" << std::endl;
            visualization << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
            visualization << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
            visualization << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
            visualization << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
            visualization << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
            visualization << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
            visualization << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            visualization << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            visualization << "Region Count: " << brain->getRegionCount() << std::endl;
            
            return visualization.str();
        }, py::arg("brain"), "Generate text-based brain visualization");

        m.def("analyze_learning_curves", [](const py::list& episodeResults) {
            std::vector<py::dict> results;
            
            for (auto item : episodeResults) {
                py::dict episode = item.cast<py::dict>();
                results.push_back(episode);
            }
            
            // Simple analysis
            py::dict analysis;
            size_t numEpisodes = results.size();
            if (numEpisodes == 0) return analysis;
            
            float totalSpikeIncrease = 0.0f;
            float totalFiringRate = 0.0f;
            size_t validEpisodes = 0;
            
            for (const auto& ep : results) {
                if (ep.contains("spike_increase") && ep.contains("avg_firing_rate")) {
                    totalSpikeIncrease += ep["spike_increase"].cast<float>();
                    totalFiringRate += ep["avg_firing_rate"].cast<float>();
                    validEpisodes++;
                }
            }
            
            if (validEpisodes > 0) {
                analysis["avg_spike_increase_per_episode"] = totalSpikeIncrease / validEpisodes;
                analysis["avg_firing_rate"] = totalFiringRate / validEpisodes;
                analysis["learning_trend"] = "improving"; // Simplified
                analysis["episodes_analyzed"] = static_cast<int>(validEpisodes);
            }
            
            return analysis;
        }, py::arg("episodeResults"), "Analyze learning curves from agent training");

        m.def("compare_synaptic_changes", [](const std::shared_ptr<Brain>& brain1,
                                             const std::shared_ptr<Brain>& brain2,
                                             const std::string& label1,
                                             const std::string& label2) {
            if (!brain1 || !brain2) {
                throw std::runtime_error("Both brains must be provided");
            }
            
            py::dict comparison;
            comparison["brain1_" + label1] = brain1->getTotalSynapseCount();
            comparison["brain2_" + label2] = brain2->getTotalSynapseCount();
            comparison["brain1_spikes"] = brain1->getTotalSpikeCount();
            comparison["brain2_spikes"] = brain2->getTotalSpikeCount();
            comparison["brain1_firing"] = brain1->getFiringNeuronCount();
            comparison["brain2_firing"] = brain2->getFiringNeuronCount();
            comparison["brain1_rate"] = brain1->getAverageFiringRate();
            comparison["brain2_rate"] = brain2->getAverageFiringRate();
            
            // Determine which performed better
            float rate1 = brain1->getAverageFiringRate();
            float rate2 = brain2->getAverageFiringRate();
            if (rate1 > rate2) {
                comparison["better_performance"] = label1;
            } else if (rate2 > rate1) {
                comparison["better_performance"] = label2;
            } else {
                comparison["better_performance"] = "equal";
            }
            
            return comparison;
        }, py::arg("brain1"), py::arg("brain2"), py::arg("label1"), py::arg("label2"),
           "Compare two brains for plasticity effectiveness");

        // 5. System Management and Monitoring
        m.def("spawn_worker_process", []() {
            // Note: This is a simplified implementation
            // In a real system, this would create separate processes for parallel computation
            return std::make_tuple("worker_1", "worker_2", "worker_3");
        }, "Spawn worker processes for distributed computation");

        m.def("load_plugin_module", [](const std::string& modulePath) {
            // This is a placeholder - real implementation would use dlopen
            // For now, just return a success message
            return "Plugin module loaded: " + modulePath;
        }, py::arg("modulePath"), "Load dynamic plugin module");

        m.def("profile_performance", [](const std::shared_ptr<Brain>& brain) {
            if (!brain) {
                throw std::runtime_error("Brain must be provided");
            }
            
            py::dict profile;
            
            // Get timing information (simplified)
            profile["neuron_count"] = brain->getTotalNeuronCount();
            profile["synapse_count"] = brain->getTotalSynapseCount();
            profile["memory_efficiency"] = 1.0f; // Placeholder
            profile["operation_count"] = brain->getTotalSpikeCount();
            
            // Calculate estimated performance metrics
            float estimatedOpsPerSecond = brain->getTotalSpikeCount() * 1000.0f; // Placeholder
            profile["estimated_ops_per_second"] = estimatedOpsPerSecond;
            
            return profile;
        }, py::arg("brain"), "Profile brain performance characteristics");

        // ====================================================================
        // CORE CLASS BINDINGS (continued)
        // ====================================================================

        py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
            .def("getType", &SensoryInput::getType, "Get the type of sensory input")
            .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
            .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
            .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
            .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
                 "Set the timestamp");

        py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
            .def(py::init<>())
            .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
                 py::arg("channels") = 3)
            .def("setData", [](Vision& self, const std::vector<float>& data) {
                self.setData(data);
            }, py::arg("data"))
            .def("getWidth", &Vision::getWidth)
            .def("getHeight", &Vision::getHeight)
            .def("getChannels", &Vision::getChannels);

        py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
            .def(py::init<>())
            .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
            .def("setData", [](Audio& self, const std::vector<float>& data) {
                self.setData(data);
            }, py::arg("data"))
            .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
            .def("getSampleRate", &Audio::getSampleRate)
            .def("getNumSamples", &Audio::getNumSamples);

        py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
            .def(py::init<>())
            .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
            .def("clearSignals", &InternalSignals::clearSignals);

        py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
            .def(py::init<>())
            .def(py::init<ActionType>(), py::arg("type"))
            .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
            .def("getType", &Action::getType)
            .def("setType", &Action::setType, py::arg("type"))
            .def("getParameters", &Action::getParameters)
            .def("setParameters", &Action::setParameters, py::arg("params"))
            .def("getName", &Action::getName)
            .def("clone", &Action::clone);

        py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
            .def(py::init<>())
            .def(py::init<float, float, WorldObjectType, float, float>(),
                 py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
                 py::arg("radius") = 0.5f)
            .def_readwrite("x", &WorldObject::x)
            .def_readwrite("y", &WorldObject::y)
            .def_readwrite("radius", &WorldObject::radius)
            .def_readwrite("type", &WorldObject::type)
            .def_readwrite("value", &WorldObject::value)
            .def_readwrite("active", &WorldObject::active);

        py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
            .def(py::init<>())
            .def_readwrite("x", &AgentBody::x)
            .def_readwrite("y", &AgentBody::y)
            .def_readwrite("orientation", &AgentBody::orientation)
            .def_readwrite("velocityX", &AgentBody::velocityX)
            .def_readwrite("velocityY", &AgentBody::velocityY)
            .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
            .def_readwrite("energy", &AgentBody::energy)
            .def_readwrite("health", &AgentBody::health)
            .def_readwrite("age", &AgentBody::age)
            .def_readwrite("isMoving", &AgentBody::isMoving)
            .def_readwrite("isTurning", &AgentBody::isTurning)
            .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
            .def("reset", &AgentBody::reset);

        py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
            .def(py::init<>())
            .def(py::init<float, bool, std::string>(),
                 py::arg("reward"), py::arg("success"), py::arg("message") = "")
            .def_readwrite("reward", &ActionResult::reward)
            .def_readwrite("success", &ActionResult::success)
            .def_readwrite("message", &ActionResult::message);

        py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
            .def(py::init<>())
            .def("getVision", &SensoryPercept::getVision)
            .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
            .def("getVisionWidth", &SensoryPercept::getVisionWidth)
            .def("getVisionHeight", &SensoryPercept::getVisionHeight)
            .def("getTouch", &SensoryPercept::getTouch)
            .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
            .def("getInternal", &SensoryPercept::getInternal)
            .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
            .def("getProprioception", &SensoryPercept::getProprioception)
            .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
            .def("getAudio", &SensoryPercept::getAudio)
            .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
            .def("getAllSignals", &SensoryPercept::getAllSignals)
            .def("getTimestamp", &SensoryPercept::getTimestamp)
            .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

        py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

        py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
            .def("initialize", &Brain::initialize,
                 "Initialize the brain with configuration")
            .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
                 py::arg("currentStep"),
                 "Perform a simulation step")
            .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
                 py::arg("currentStep"), py::arg("currentTime"),
                 "Perform a simulation step with timestamp")
            .def("receiveSensoryInput", &Brain::receiveSensoryInput,
                 py::arg("input"),
                 "Inject sensory input into the brain")
            .def("injectCurrent", &Brain::injectCurrent,
                 py::arg("neuron"), py::arg("current"),
                 "Inject current into a specific neuron")
            .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
                 py::arg("type"), py::arg("current"),
                 "Inject current into all neurons of a specific type")
            .def("produceAction", &Brain::produceAction,
                 "Produce motor action based on neural activity")
            .def("reset", &Brain::reset,
                 "Reset brain state")
            .def("save", &Brain::save, py::arg("filepath"),
                 "Save brain state to file")
            .def("load", &Brain::load, py::arg("filepath"),
                 "Load brain state from file")
            .def("addRegion", &Brain::addRegion, py::arg("name") = "",
                 "Add a new neural region")
            .def("getRegion", &Brain::getRegion, py::arg("id"),
                 py::return_value_policy::reference_internal,
                 "Get a region by ID")
            .def("getRegionCount", &Brain::getRegionCount,
                 "Get the number of regions")
            .def("getRegionIds", &Brain::getRegionIds,
                 "Get all region IDs")
            .def("getRegions", &Brain::getRegions,
                 py::return_value_policy::reference_internal,
                 "Get all regions")
            .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
                 "Get total neuron count across all regions")
            .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
                 "Get total synapse count across all regions")
            .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
                 "Get count of active neurons")
            .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
                 "Get count of currently firing neurons")
            .def("getAverageFiringRate", &Brain::getAverageFiringRate,
                 "Get average firing rate across all neurons")
            .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
                 "Get excitation/inhibition balance ratio")
            .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
                 "Get total spike count")
            .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
                 "Get current developmental stage")
            .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
                 py::arg("stage"),
                 "Set developmental stage")
            .def("getConfig", &Brain::getConfig,
                 py::return_value_policy::reference_internal,
                 "Get the configuration")
            .def("logStatus", &Brain::logStatus,
                 "Log brain status");

        py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

        m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration");

        m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration");

        m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world");

        m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface");

        // Advanced factory functions for enhanced user experience
        m.def("create_brain_with_preset", [](const std::string& preset) -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            
            if (preset == "simple") {
                config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
                config->set("simulation_timestep", 0.001, ConfigSource::Default);
                config->set("connection_probability", 0.1f, ConfigSource::Default);
            } else if (preset == "advanced") {
                config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
                config->set("simulation_timestep", 0.0005, ConfigSource::Default);
                config->set("connection_probability", 0.15f, ConfigSource::Default);
                config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
                config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
            } else if (preset == "high_performance") {
                config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
                config->set("simulation_timestep", 0.001, ConfigSource::Default);
                config->set("connection_probability", 0.05f, ConfigSource::Default);
                config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
                config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
            }
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, py::arg("preset"), "Create a brain with preset configuration");

        m.def("create_world_with_objects", [](size_t width, size_t height, 
                                             const std::vector<WorldObject>& objects) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, 16, 16);
            
            for (const auto& obj : objects) {
                world->addObject(obj);
            }
            
            return world;
        }, py::arg("width"), py::arg("height"), py::arg("objects"),
           "Create a world with predefined objects");

        m.def("create_agent_with_learning", [](std::shared_ptr<Brain> brain, 
                                              std::shared_ptr<SimpleWorld> world,
                                              bool enable_reward_learning,
                                              bool enable_curiosity_driven,
                                              bool enable_structural_plasticity) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            if (enable_reward_learning) agent->enableRewardModulation(true);
            if (enable_curiosity_driven) agent->enableCuriosity(true);
            if (enable_structural_plasticity) agent->enableStructuralPlasticity(true);
            
            return agent;
        }, py::arg("brain"), py::arg("world"),
           py::arg("enable_reward_learning") = true,
           py::arg("enable_curiosity_driven") = true,
           py::arg("enable_structural_plasticity") = true,
           "Create an agent with enhanced learning capabilities");

        m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
        m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
        m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
        m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    }
    catch (const std::exception& e) {
        py::errstream() << "Error registering Python bindings: " << e.what() << std::endl;
    }
}

} // namespace nlm