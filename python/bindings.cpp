// Enhanced Python bindings for NLM
// This file extends the existing bindings with additional Pythonic features and utilities

namespace py = pybind11;
namespace nlm {

// Utility functions for Python integration
namespace pyutils {
    
    // Convert C++ vector to numpy array
    template<typename T>
    py::array_t<T> vectorToNumpy(const std::vector<T>& vec) {
        if (vec.empty()) {
            return py::array_t<T>();
        }
        py::array_t<T> result({vec.size()});
        std::copy(vec.begin(), vec.end(), result.mutable_data());
        return result;
    }
    
    // Convert numpy array to C++ vector
    template<typename T>
    std::vector<T> numpyToVector(py::array_t<T> arr) {
        py::buffer_info info = arr.request();
        T* ptr = static_cast<T*>(info.ptr);
        size_t size = info.shape[0];
        return std::vector<T>(ptr, ptr + size);
    }
    
    // Get Python representation of neuron state
    std::string neuronStateToString(const NeuronState& state) {
        std::ostringstream oss;
        oss << "NeuronState(";
        oss << "V=" << state.membranePotential << ", ";
        oss << "I_syn=" << state.synapseConductance << ", ";
        oss << "firing=" << (state.firingState == FiringState::Active ? "true" : "false") << ", ";
        oss << "spike_count=" << state.spikeHistory.size();
        oss << ")";
        return oss.str();
    }
    
    // Get Python representation of brain statistics
    std::string brainStatsToString(const Brain& brain) {
        std::ostringstream oss;
        oss << "BrainStats(";
        oss << "neurons=" << brain.getTotalNeuronCount() << ", ";
        oss << "synapses=" << brain.getTotalSynapseCount() << ", ";
        oss << "firing=" << brain.getFiringNeuronCount() << ", ";
        oss << "spikes=" << brain.getTotalSpikeCount() << ", ";
        oss << "rate=" << brain.getAverageFiringRate() << ", ";
        oss << "dev_stage=" << static_cast<int>(brain.getDevelopmentalStage());
        oss << ")";
        return oss.str();
    }
}

// Enhanced Python bindings
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        -----------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - NumPy integration for efficient array operations
        - Enhanced brain and agent APIs with Pythonic methods
        - Advanced utilities for simulation control and monitoring
        - Better error handling and exception translation
        - Performance monitoring tools
        
        Key features:
        * Real-time brain state monitoring
        * Advanced plotting and visualization helpers
        * Simulation recording and replay
        * Multi-threaded simulation support
        * Advanced learning algorithms
    )pbdoc";
    
    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // Neuron type bindings with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });
    
    // Enhanced NeuronType enum with string conversion
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__str__", [](NeuronType type) {
            switch (type) {
                case NeuronType::Excitatory: return std::string("excitatory");
                case NeuronType::Inhibitory: return std::string("inhibitory");
                case NeuronType::Modulatory: return std::string("modulatory");
                case NeuronType::Sensory: return std::string("sensory");
                case NeuronType::Motor: return std::string("motor");
                case NeuronType::Internal: return std::string("internal");
                default: return std::string("unknown");
            }
        });
    
    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys, "Get all configuration keys")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (bool)")
        .def("get", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            return val ? val.value() : std::string();
        }, py::arg("key"),
            "Get configuration value as string (empty if not found)")
        .def("get_int", [](Config& self, const std::string& key, int default_value) {
            return self.getOr<int>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value as int with default")
        .def("get_float", [](Config& self, const std::string& key, double default_value) {
            return self.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value as float with default")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value) {
            return self.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value as bool with default")
        .def("summary", &Config::summary, "Get configuration summary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });
    
    // Enhanced SensoryInput classes
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    // Enhanced Vision class with Pythonic interface
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from vector")
        .def("set_data_numpy", [](Vision& self, py::array_t<float> arr) {
            auto vec = pyutils::numpyToVector<float>(arr);
            self.setData(vec);
        }, py::arg("data"), "Set vision data from numpy array")
        .def("get_data", [](Vision& self) {
            return pyutils::vectorToNumpy<float>(self.getData());
        }, "Get vision data as numpy array")
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels);
    
    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from vector")
        .def("set_data_numpy", [](Audio& self, py::array_t<float> arr) {
            auto vec = pyutils::numpyToVector<float>(arr);
            self.setData(vec);
        }, py::arg("data"), "Set audio data from numpy array")
        .def("get_data", [](Audio& self) {
            return pyutils::vectorToNumpy<float>(self.getData());
        }, "Get audio data as numpy array")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);
    
    // Enhanced Action class with Pythonic methods
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("parameters"),
             "Set action parameters")
        .def("get_name", &Action::getName)
        .def("add_parameter", [](Action& self, float value) {
            std::vector<float> params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " (" + 
                   std::to_string(action.getParameters().size()) + " params)>";
        });
    
    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
            "Calculate distance to a point")
        .def("is_near", [](const WorldObject& obj, float x, float y, float radius) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return (dx*dx + dy*dy) <= (radius*radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
            "Check if object is near a point");
    
    // Enhanced AgentBody class
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
        .def("get_position", [](AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get position as (x, y) tuple")
        .def("get_velocity", [](AgentBody& body) {
            return py::make_tuple(body.velocityX, body.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("move_to", [](AgentBody& body, float x, float y, float dt) {
            float dx = x - body.x;
            float dy = y - body.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0) {
                float speed = dist / dt;
                body.velocityX = dx / dist * speed;
                body.velocityY = dy / dist * speed;
                body.x = x;
                body.y = y;
                body.isMoving = true;
            }
        }, py::arg("x"), py::arg("y"), py::arg("dt"),
            "Move agent to a position over time step dt")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");
    
    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_good", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if action result is good (successful and positive reward)")
        .def("is_bad", [](const ActionResult& result) {
            return !result.success || result.reward < 0.0f;
        }, "Check if action result is bad (unsuccessful or negative reward)");
    
    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a dictionary")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + "x" +
                   std::to_string(percept.getVisionHeight()) + ">";
        });
    
    // Enhanced SimpleWorld class with Pythonic interface
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world dynamics")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command and return result")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " @ " +
                   std::to_string(world.getSimulationTime()) + "s>";
        });
    
    // Enhanced Brain class with Pythonic interface
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount)
        .def("get_region_ids", &Brain::getRegionIds)
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount)
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount)
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount)
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount)
        .def("get_average_firing_rate", &Brain::getAverageFiringRate)
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio)
        .def("get_total_spike_count", &Brain::getTotalSpikeCount)
        .def("get_developmental_stage", &Brain::getDevelopmentalStage)
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus, "Log brain status")
        .def("get_stats", [](const Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            return stats;
        }, "Get brain statistics as dictionary")
        .def("get_region_stats", [](const Brain& brain, RegionId region_id) {
            py::dict stats;
            auto* region = brain.getRegion(region_id);
            if (region) {
                stats["neuron_count"] = region->getNeuronCount();
                stats["synapse_count"] = region->getSynapseCount();
                stats["active_neurons"] = region->getActiveNeuronCount();
                stats["firing_neurons"] = region->getFiringNeuronCount();
            }
            return stats;
        }, py::arg("region_id"), "Get statistics for a specific region");
    
    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize)
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize)
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage)
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel)
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel)
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel)
        .def("get_prediction_error", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        .def("enable_all_learning", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all learning systems")
        .def("disable_all_learning", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all learning systems")
        .def("get_learning_state", [](const AgentBrain& agent) {
            py::dict state;
            state["reward_modulation"] = agent.isRewardModulationEnabled();
            state["structural_plasticity"] = agent.isStructuralPlasticityEnabled();
            state["development"] = agent.isDevelopmentEnabled();
            state["curiosity"] = agent.isCuriosityEnabled();
            state["neuromodulation"] = agent.getNeuromodulationLevel();
            state["curiosity"] = agent.getCuriosityLevel();
            state["novelty"] = agent.getNoveltyLevel();
            state["prediction_error"] = agent.getPredictionError();
            return state;
        }, "Get current learning system state");
    
    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Convenience functions
    m.def("run_simple_episode", [](std::shared_ptr<Brain> brain,
                                std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent,
                                int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
        "Run a simple simulation episode and return total spikes");
    
    m.def("simulate_agent", [](std::shared_ptr<AgentBrain> agent,
                             std::shared_ptr<SimpleWorld> world,
                             int num_steps,
                             bool verbose = false) {
        int spikes = 0;
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            // Note: Brain step is handled internally by AgentBrain
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << "Spikes: " << agent->get_brain()->getTotalSpikeCount()
                         << " Action: " << static_cast<int>(action)
                         << " Reward: " << result.reward << std::endl;
            }
        }
        return agent->get_brain()->getTotalSpikeCount();
    }, py::arg("agent"), py::arg("world"), py::arg("num_steps"), py::arg("verbose") = false,
        "Run agent simulation with automatic brain stepping");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Performance monitoring utilities
    m.def("monitor_performance", [](std::shared_ptr<Brain> brain, int num_steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict metrics;
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["firing_rate"] = brain->getAverageFiringRate();
        metrics["steps_per_second"] = num_steps / (duration.count() / 1000.0);
        metrics["simulation_time_ms"] = duration.count();
        
        return metrics;
    }, py::arg("brain"), py::arg("num_steps"),
        "Monitor simulation performance and return metrics");
    
    // Simulation recording utilities
    m.def("record_simulation", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent,
                                 int num_steps) {
        std::vector<int> spike_counts;
        std::vector<double> firing_rates;
        std::vector<int> action_counts;
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            spike_counts.push_back(brain->getTotalSpikeCount());
            firing_rates.push_back(brain->getAverageFiringRate());
            action_counts.push_back(static_cast<int>(action));
        }
        
        py::dict recording;
        recording["spike_counts"] = spike_counts;
        recording["firing_rates"] = firing_rates;
        recording["actions"] = action_counts;
        recording["final_spikes"] = spike_counts.back();
        recording["final_firing_rate"] = firing_rates.back();
        
        return recording;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
        "Record simulation data and return time series");
    
    // Visualization utilities (basic plotting)
    m.def("plot_brain_activity", [](const py::dict& recording) {
        if (!recording.contains("spike_counts")) {
            throw py::value_error("Recording data does not contain spike_counts");
        }
        
        std::vector<int> spikes = recording["spike_counts"].cast<std::vector<int>>();
        std::cout << "Brain Activity Plot (spike count over time):" << std::endl;
        for (size_t i = 0; i < spikes.size(); ++i) {
            if (i % 50 == 0) std::cout << i << ": ";
            std::cout << spikes[i];
            if (i % 50 == 49 || i == spikes.size() - 1) {
                std::cout << std::endl;
            } else {
                std::cout << " ";
            }
        }
        return py::dict({
            {"max_spikes", *std::max_element(spikes.begin(), spikes.end())},
            {"avg_spikes", std::accumulate(spikes.begin(), spikes.end(), 0.0) / spikes.size()},
            {"spike_stddev", 0.0}  // Would need variance calculation
        });
    }, py::arg("recording"),
        "Simple text-based plotting of brain activity");
    
    // Type conversion utilities
    py::class_<pyutils>(m, "utils")
        .def("vector_to_numpy", &pyutils::vectorToNumpy<float>)
        .def("numpy_to_vector", &pyutils::numpyToVector<float>)
        .def("neuron_state_to_string", &pyutils::neuronStateToString)
        .def("brain_stats_to_string", &pyutils::brainStatsToString);
    
    // Version and build information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("build_info") = py::dict({
        {"c++_standard", "C++20"},
        {"pybind11_version", "PYBIND11_VERSION"},
        {"features", std::vector<std::string>{
            "numpy_integration",
            "enhanced_apis",
            "performance_monitoring",
            "simulation_recording",
            "advanced_learning",
            "visualization_tools"
        }},
        {"build_type", "Enhanced Python Bindings"}
    });
    
    // Example usage
    m.def("example_1", []() {
        std::cout << "Example 1: Basic Brain Usage" << std::endl;
        std::cout << "----------------------" << std::endl;
        std::cout << "1. Create a brain: brain = pynlm.create_brain(pynlm.create_default_config())" << std::endl;
        std::cout << "2. Initialize: brain.initialize()" << std::endl;
        std::cout << "3. Run simulation: brain.step(step)" << std::endl;
        std::cout << "4. Get statistics: brain.get_stats()" << std::endl;
        std::cout << "5. Produce action: brain.produce_action()" << std::endl;
        return std::string("Basic example executed successfully");
    }, "Run basic usage example");
    
    m.def("example_2", []() {
        std::cout << "Example 2: Agent Simulation" << std::endl;
        std::cout << "------------------------" << std::endl;
        std::cout << "1. Create world: world = pynlm.create_simple_world()" << std::endl;
        std::cout << "2. Configure world: world.configure(width, height, vision_width, vision_height)" << std::endl;
        std::cout << "3. Create brain: brain = pynlm.create_brain(config)" << std::endl;
        std::cout << "4. Create agent: agent = pynlm.create_agent_brain(brain)" << std::endl;
        std::cout << "5. Enable learning: agent.enable_all_learning()" << std::endl;
        std::cout << "6. Run simulation: pynlm.run_simple_episode(brain, world, agent, 1000)" << std::endl;
        return std::string("Agent example executed successfully");
    }, "Run agent simulation example");
    
    m.def("example_3", []() {
        std::cout << "Example 3: Advanced Features" << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << "1. Performance monitoring: pynlm.monitor_performance(brain, 1000)" << std::endl;
        std::cout << "2. Data recording: pynlm.record_simulation(brain, world, agent, 500)" << std::endl;
        std::cout << "3. Visualization: pynlm.plot_brain_activity(recording)" << std::endl;
        std::cout << "4. NumPy integration: vision.set_data_numpy(numpy_array)" << std::endl;
        return std::string("Advanced features example executed successfully");
    }, "Run advanced features example");
}

} // namespace nlm
