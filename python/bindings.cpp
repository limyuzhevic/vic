// ============================================================================
// Enhanced Config Class with Pythonic Interface
// ============================================================================

py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
    Configuration class for NLM system with enhanced Python interface
    
    Provides a Pythonic interface for configuring NLM parameters with
    type safety, property access, and convenient factory methods.
)pbdoc")
    .def(py::init<>())
    .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
         "Load configuration from a JSON file")
    .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
        return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
    }, py::arg("args"),
         "Load configuration from command line arguments")
    .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
         "Save configuration to a JSON file")
    .def("has", &Config::has, py::arg("key"),
         "Check if a configuration key exists")
    .def("get", &Config::get, py::arg("key"),
         "Get configuration value (may throw if key not found)")
    .def("getOr", py::overload_cast<const std::string&, int64_t>(&Config::getOr),
         py::arg("key"), py::arg("default"),
         "Get configuration value with default")
    .def("getOr", py::overload_cast<const std::string&, double>(&Config::getOr),
         py::arg("key"), py::arg("default"),
         "Get configuration value with default (double)")
    .def("getOr", py::overload_cast<const std::string&, float>(&Config::getOr),
         py::arg("key"), py::arg("default"),
         "Get configuration value with default (float)")
    .def("getOr", py::overload_cast<const std::string&, std::string>(&Config::getOr),
         py::arg("key"), py::arg("default"),
         "Get configuration value with default (string)")
    .def("getKeys", &Config::getKeys,
         "Get all configuration keys")
    .def("clear", &Config::clear,
         "Clear all configuration entries")
    .def("set", [](Config& self, const std::string& key, const py::object& value) {
        if (py::isinstance<py::int_>(value)) {
            self.set(key, value.cast<int64_t>(), nlm::ConfigSource::User);
        } else if (py::isinstance<py::float_>(value)) {
            self.set(key, value.cast<float>(), nlm::ConfigSource::User);
        } else if (py::isinstance<py::str>(value)) {
            self.set(key, value.cast<std::string>(), nlm::ConfigSource::User);
        } else {
            throw ConfigError("Unsupported type for configuration value: " + value.cast<std::string>());
        }
    }, py::arg("key"), py::arg("value"),
         "Set configuration value with type detection")
    .def("getSummary", &Config::summary,
         "Get a summary string of the configuration")
    .def("__repr__", [](const Config& cfg) {
        return "<Config: " + cfg.summary() + ">";
    })
    .def("__getitem__", [](Config& self, const std::string& key) {
        if (!self.has(key)) {
            throw py::key_error("Configuration key not found: " + key);
        }
        return self.get(key);
    }, "Get configuration value by key (supports [] operator)")
    .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
        self.set(key, value);
    }, "Set configuration value by key (supports [] operator)")
    .def("validate", [](Config& self, const py::object& validation_func) {
        // Simple validation - could be extended with more sophisticated validation
        return true;
    }, "Validate configuration (placeholder for advanced validation)")
    .def("to_dict", [](Config& self) {
        std::unordered_map<std::string, py::object> dict;
        auto keys = self.getKeys();
        for (const auto& key : keys) {
            dict[key] = py::object(); // Could extract actual values
        }
        return dict;
    }, "Convert configuration to dictionary (placeholder)")
    .def("from_dict", [](Config& self, const std::unordered_map<std::string, py::object>& dict) {
        // Could apply dictionary values to configuration
        return true;
    }, "Load configuration from dictionary (placeholder)")
    // Property accessors for common configuration patterns
    .def_property("random_seed", &Config::getOr<int64_t>,
                 nullptr, "Random seed for reproducible experiments")
    .def_property("simulation_timestep", &Config::getOr<double>,
                 nullptr, "Simulation timestep in seconds")
    .def_property("neuron_count", &Config::getOr<int64_t>,
                 nullptr, "Number of neurons in the brain")
    .def_property("region_count", &Config::getOr<int64_t>,
                 nullptr, "Number of brain regions")
    .def_property("connection_probability", &Config::getOr<float>,
                 nullptr, "Connection probability for random connectivity");

// ============================================================================
// Enhanced Sensory Input Classes
// ============================================================================

py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "SensoryInput", R"pbdoc(
    Base class for sensory input with enhanced Python interface
)pbdoc")
    .def("getType", &SensoryInput::getType, "Get the type of sensory input")
    .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
    .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
    .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
    .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
         "Set the timestamp")
    .def("__repr__", [](const SensoryInput& input) {
        return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) +
               ", dim=" + std::to_string(input.getDimensions()) + ">";
    });

// Enhanced Vision class with NumPy support
py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
    Vision sensory input with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
         py::arg("channels") = 3, "Create vision input with dimensions")
    .def("setData", [](Vision& self, const py::array_t<float>& data) {
        if (data.ndim() != 3) {
            throw NLMError("Vision data must be 3D array (height, width, channels)");
        }
        auto buf = data.request();
        std::vector<float> vec(static_cast<float*>(buf.ptr), 
                              static_cast<float*>(buf.ptr) + buf.size);
        self.setData(vec);
    }, py::arg("data"), "Set vision data from NumPy array")
    .def("getDataArray", [](Vision& self) {
        const auto& data = self.getData();
        return py::array_t<float>(data);
    }, "Get vision data as NumPy array")
    .def("getWidth", &Vision::getWidth)
    .def("getHeight", &Vision::getHeight)
    .def("getChannels", &Vision::getChannels)
    .def("normalize", [](Vision& self, float mean, float std) {
        auto& data = const_cast<std::vector<float>&>(self.getData());
        for (auto& val : data) {
            val = (val - mean) / std;
        }
    }, py::arg("mean"), py::arg("std"), "Normalize vision data")
    .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
        // Placeholder for actual resize implementation
        throw NLMError("Vision resize not yet implemented");
    }, py::arg("width"), py::arg("height"), "Resize vision data (placeholder)");

// Enhanced Audio class
py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
    Audio sensory input with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"),
         "Create audio input with sample rate and number of samples")
    .def("setData", [](Audio& self, const py::array_t<float>& data) {
        if (data.ndim() != 1) {
            throw NLMError("Audio data must be 1D array");
        }
        auto buf = data.request();
        std::vector<float> vec(static_cast<float*>(buf.ptr), 
                              static_cast<float*>(buf.ptr) + buf.size);
        self.setData(vec);
    }, py::arg("data"), "Set audio data from NumPy array")
    .def("getDataArray", [](Audio& self) {
        const auto& data = self.getData();
        return py::array_t<float>(data);
    }, "Get audio data as NumPy array")
    .def("setSampleRate", &Audio::setSampleRate, py::arg("sample_rate"),
         "Set sample rate")
    .def("getSampleRate", &Audio::getSampleRate)
    .def("getNumSamples", &Audio::getNumSamples)
    .def("normalize", [](Audio& self) {
        auto& data = const_cast<std::vector<float>&>(self.getData());
        float max_val = *std::max_element(data.begin(), data.end());
        float min_val = *std::min_element(data.begin(), data.end());
        float range = max_val - min_val;
        if (range > 0) {
            for (auto& val : data) {
                val = 2.0f * (val - min_val) / range - 1.0f;
            }
        }
    }, "Normalize audio data to [-1, 1] range");

// Enhanced InternalSignals class
py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
    Internal signals sensory input with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def("addSignal", [](InternalSignals& self, float value) {
        self.addSignal(value);
    }, py::arg("value"), "Add a signal value")
    .def("addSignals", [](InternalSignals& self, const py::array_t<float>& values) {
        auto buf = values.request();
        std::vector<float> vec(static_cast<float*>(buf.ptr), 
                              static_cast<float*>(buf.ptr) + buf.size);
        for (float val : vec) {
            self.addSignal(val);
        }
    }, py::arg("values"), "Add multiple signal values from array")
    .def("clearSignals", &InternalSignals::clearSignals)
    .def("getSignals", [](InternalSignals& self) {
        return self.getData();
    }, "Get all signal values")
    .def("getSignalsArray", [](InternalSignals& self) {
        return py::array_t<float>(self.getData());
    }, "Get all signal values as NumPy array")
    .def("getMean", [](InternalSignals& self) {
        const auto& data = self.getData();
        if (data.empty()) return 0.0f;
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        return sum / data.size();
    }, "Get mean of signals")
    .def("getStd", [](InternalSignals& self) {
        const auto& data = self.getData();
        if (data.size() <= 1) return 0.0f;
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        float mean = sum / data.size();
        float sq_sum = 0.0f;
        for (float val : data) {
            sq_sum += (val - mean) * (val - mean);
        }
        return std::sqrt(sq_sum / (data.size() - 1));
    }, "Get standard deviation of signals");

// ============================================================================
// Enhanced Action Class
// ============================================================================

py::class_<Action>(m, "Action", R"pbdoc(
    Action representation for motor output with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def(py::init<ActionType>(), py::arg("type"))
    .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
         "Create action with type and parameters")
    .def("getType", &Action::getType)
    .def("setType", &Action::setType, py::arg("type"))
    .def("getParameters", &Action::getParameters)
    .def("setParameters", &Action::setParameters, py::arg("params"))
    .def("getName", &Action::getName)
    .def("getParameter", [](Action& self, const std::string& name) {
        const auto& params = self.getParameters();
        auto it = std::find_if(params.begin(), params.end(), 
                               [](const std::pair<std::string, float>& p) { return p.first == name; });
        if (it != params.end()) {
            return it->second;
        }
        throw py::key_error("Parameter not found: " + name);
    }, "Get parameter value by name")
    .def("setParameter", [](Action& self, const std::string& name, float value) {
        auto& params = const_cast<std::vector<std::pair<std::string, float>>&>(self.getParameters());
        auto it = std::find_if(params.begin(), params.end(),
                               [&](const std::pair<std::string, float>& p) { return p.first == name; });
        if (it != params.end()) {
            it->second = value;
        } else {
            params.push_back({name, value});
        }
    }, py::arg("name"), py::arg("value"), "Set parameter value by name")
    .def("hasParameter", [](Action& self, const std::string& name) {
        const auto& params = self.getParameters();
        return std::any_of(params.begin(), params.end(),
                          [&](const std::pair<std::string, float>& p) { return p.first == name; });
    }, "Check if action has parameter by name")
    .def("clone", &Action::clone)
    .def("to_dict", [](const Action& self) {
        std::unordered_map<std::string, float> dict;
        for (const auto& param : self.getParameters()) {
            dict[param.first] = param.second;
        }
        return dict;
    }, "Convert action to dictionary")
    .def("from_dict", [](Action& self, const std::unordered_map<std::string, float>& dict) {
        auto& params = const_cast<std::vector<std::pair<std::string, float>>&>(self.getParameters());
        params.clear();
        for (const auto& item : dict) {
            params.push_back(item);
        }
    }, "Load action from dictionary")
    .def("__repr__", [](const Action& action) {
        std::ostringstream oss;
        oss << "<Action: " << action.getName();
        const auto& params = action.getParameters();
        if (!params.empty()) {
            oss << " (";
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << params[i].first << "=" << params[i].second;
            }
            oss << ")";
        }
        oss << ">";
        return oss.str();
    });

// ============================================================================
// Enhanced WorldObject Class
// ============================================================================

py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
    World object representation with enhanced Python interface
)pbdoc")
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
    .def("distanceTo", [](const WorldObject& self, const WorldObject& other) {
        float dx = self.x - other.x;
        float dy = self.y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }, py::arg("other"), "Calculate distance to another object")
    .def("isCollision", [](const WorldObject& self, const WorldObject& other) {
        float dx = self.x - other.x;
        float dy = self.y - other.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance < (self.radius + other.radius);
    }, py::arg("other"), "Check if this object collides with another")
    .def("to_dict", [](const WorldObject& obj) {
        std::unordered_map<std::string, float> dict;
        dict["x"] = obj.x;
        dict["y"] = obj.y;
        dict["radius"] = obj.radius;
        dict["value"] = obj.value;
        dict["type"] = static_cast<int>(obj.type);
        dict["active"] = obj.active ? 1.0f : 0.0f;
        return dict;
    }, "Convert object to dictionary")
    .def("from_dict", [](WorldObject& obj, const std::unordered_map<std::string, float>& dict) {
        obj.x = dict.at("x");
        obj.y = dict.at("y");
        obj.radius = dict.at("radius");
        obj.value = dict.at("value");
        obj.type = static_cast<WorldObjectType>(dict.at("type"));
        obj.active = dict.at("active") > 0.5f;
    }, "Load object from dictionary");

// ============================================================================
// Enhanced AgentBody Class
// ============================================================================

py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
    Agent body state with enhanced Python interface
)pbdoc")
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
    .def("distanceTo", [](const AgentBody& self, const AgentBody& other) {
        float dx = self.x - other.x;
        float dy = self.y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }, py::arg("other"), "Calculate distance to another agent")
    .def("isCollision", [](const AgentBody& self, const AgentBody& other) {
        float dx = self.x - other.x;
        float dy = self.y - other.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        float radius = 0.5f; // Default agent radius
        return distance < (radius + radius);
    }, py::arg("other"), "Check if this agent collides with another")
    .def("reset", &AgentBody::reset)
    .def("updatePosition", [](AgentBody& self, float dx, float dy, float dt) {
        self.x += self.velocityX * dt;
        self.y += self.velocityY * dt;
        self.orientation += self.angularVelocity * dt;
    }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Update position based on velocity")
    .def("setVelocity", [](AgentBody& self, float dx, float dy, float dt) {
        self.velocityX = dx / dt;
        self.velocityY = dy / dt;
    }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Set velocity from displacement")
    .def("to_dict", [](const AgentBody& body) {
        std::unordered_map<std::string, float> dict;
        dict["x"] = body.x;
        dict["y"] = body.y;
        dict["orientation"] = body.orientation;
        dict["velocityX"] = body.velocityX;
        dict["velocityY"] = body.velocityY;
        dict["angularVelocity"] = body.angularVelocity;
        dict["energy"] = body.energy;
        dict["health"] = body.health;
        dict["age"] = body.age;
        dict["isMoving"] = body.isMoving ? 1.0f : 0.0f;
        dict["isTurning"] = body.isTurning ? 1.0f : 0.0f;
        dict["lastActionTime"] = body.lastActionTime;
        return dict;
    }, "Convert agent body to dictionary")
    .def("from_dict", [](AgentBody& body, const std::unordered_map<std::string, float>& dict) {
        body.x = dict.at("x");
        body.y = dict.at("y");
        body.orientation = dict.at("orientation");
        body.velocityX = dict.at("velocityX");
        body.velocityY = dict.at("velocityY");
        body.angularVelocity = dict.at("angularVelocity");
        body.energy = dict.at("energy");
        body.health = dict.at("health");
        body.age = dict.at("age");
        body.isMoving = dict.at("isMoving") > 0.5f;
        body.isTurning = dict.at("isTurning") > 0.5f;
        body.lastActionTime = dict.at("lastActionTime");
    }, "Load agent body from dictionary");

// ============================================================================
// Enhanced SensoryPercept Class
// ============================================================================

py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
    Sensory percept data with enhanced Python interface
)pbdoc")
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
    .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
    .def("mergeWith", [](SensoryPercept& self, const SensoryPercept& other) {
        // Merge other percept into this one
        self.setVision(*other.getVision());
        self.setTouch(*other.getTouch());
        self.setInternal(*other.getInternal());
        self.setProprioception(*other.getProprioception());
        self.setAudio(*other.getAudio());
        self.setTimestamp(other.getTimestamp());
    }, py::arg("other"), "Merge another percept into this one")
    .def("hasVision", [](const SensoryPercept& self) {
        return self.getVision() != nullptr;
    }, "Check if percept has vision data")
    .def("hasTouch", [](const SensoryPercept& self) {
        return self.getTouch() != nullptr;
    }, "Check if percept has touch data")
    .def("hasInternal", [](const SensoryPercept& self) {
        return self.getInternal() != nullptr;
    }, "Check if percept has internal signals")
    .def("hasAudio", [](const SensoryPercept& self) {
        return self.getAudio() != nullptr;
    }, "Check if percept has audio data")
    .def("getAllData", [](SensoryPercept& self) {
        std::unordered_map<std::string, py::object> data;
        auto vision = self.getVision();
        if (vision) data["vision"] = py::cast(vision);
        auto touch = self.getTouch();
        if (touch) data["touch"] = py::cast(touch);
        auto internal = self.getInternal();
        if (internal) data["internal"] = py::cast(internal);
        auto audio = self.getAudio();
        if (audio) data["audio"] = py::cast(audio);
        return data;
    }, "Get all sensory data as dictionary")
    .def("setAllData", [](SensoryPercept& self, const std::unordered_map<std::string, py::object>& data) {
        for (const auto& item : data) {
            if (item.first == "vision") {
                self.setVision(py::cast<std::shared_ptr<Vision>>(item.second));
            } else if (item.first == "touch") {
                self.setTouch(py::cast<std::shared_ptr<InternalSignals>>(item.second));
            } else if (item.first == "internal") {
                self.setInternal(py::cast<std::shared_ptr<InternalSignals>>(item.second));
            } else if (item.first == "audio") {
                self.setAudio(py::cast<std::shared_ptr<Audio>>(item.second));
            }
        }
    }, "Set all sensory data from dictionary")
    .def("clear", [](SensoryPercept& self) {
        self.setVision(nullptr);
        self.setTouch(nullptr);
        self.setInternal(nullptr);
        self.setAudio(nullptr);
        self.setTimestamp(0.0);
    }, "Clear all sensory data")
    .def("to_dict", [](const SensoryPercept& self) {
        std::unordered_map<std::string, py::object> dict;
        auto vision = self.getVision();
        if (vision) dict["vision"] = py::cast(vision);
        auto touch = self.getTouch();
        if (touch) dict["touch"] = py::cast(touch);
        auto internal = self.getInternal();
        if (internal) dict["internal"] = py::cast(internal);
        auto audio = self.getAudio();
        if (audio) dict["audio"] = py::cast(audio);
        dict["timestamp"] = self.getTimestamp();
        return dict;
    }, "Convert sensory percept to dictionary")
    .def("from_dict", [](SensoryPercept& self, const std::unordered_map<std::string, py::object>& dict) {
        for (const auto& item : dict) {
            if (item.first == "vision") {
                self.setVision(py::cast<std::shared_ptr<Vision>>(item.second));
            } else if (item.first == "touch") {
                self.setTouch(py::cast<std::shared_ptr<InternalSignals>>(item.second));
            } else if (item.first == "internal") {
                self.setInternal(py::cast<std::shared_ptr<InternalSignals>>(item.second));
            } else if (item.first == "audio") {
                self.setAudio(py::cast<std::shared_ptr<Audio>>(item.second));
            } else if (item.first == "timestamp") {
                self.setTimestamp(py::cast<double>(item.second));
            }
        }
    }, "Load sensory percept from dictionary")
    .def("__repr__", [](const SensoryPercept& self) {
        std::ostringstream oss;
        oss << "<SensoryPercept: vision=" << (self.getVision() ? "yes" : "no")
            << ", touch=" << (self.getTouch() ? "yes" : "no")
            << ", internal=" << (self.getInternal() ? "yes" : "no")
            << ", audio=" << (self.getAudio() ? "yes" : "no")
            << ", timestamp=" << self.getTimestamp() << ">";
        return oss.str();
    });

// ============================================================================
// Enhanced SimpleWorld Class
// ============================================================================

py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
    Simple 2D world for NLM simulation with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
         py::arg("vision_width"), py::arg("vision_height"),
         "Configure the world dimensions and vision settings")
    .def("reset", &SimpleWorld::reset)
    .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
         "Set agent starting position")
    .def("update", &SimpleWorld::update, py::arg("timestep"),
         "Update world state")
    .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
         py::arg("cmd"), py::arg("current_time"),
         "Apply motor command to world")
    .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
         py::return_value_policy::reference_internal,
         "Get sensory percept from world")
    .def("getAgentBody", &SimpleWorld::getAgentBody,
         py::return_value_policy::reference_internal,
         "Get agent body from world")
    .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
         "Add a world object")
    .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
         "Remove object at position")
    .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
         "Check if position is valid")
    .def("getWidth", &SimpleWorld::getWidth)
    .def("getHeight", &SimpleWorld::getHeight)
    .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
    .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
         "Set maximum energy")
    .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
    .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
         "Set energy decay rate")
    .def("getSimulationTime", &SimpleWorld::getSimulationTime)
    .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
         "Set random seed")
    .def("getRandomSeed", &SimpleWorld::getRandomSeed)
    .def("__repr__", [](const SimpleWorld& world) {
        return "<SimpleWorld: " + std::to_string(world.getWidth()) + 
               "x" + std::to_string(world.getHeight()) + ">";
    });

// ============================================================================
// Enhanced AgentBrain Class
// ============================================================================

py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
    Agent brain interface connecting NLM brain to world with enhanced Python interface
)pbdoc")
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
         "Initialize with brain")
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
         py::arg("reward"), py::arg("predicted_reward"),
         "Apply reward-based neuromodulation")
    .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
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
    .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
    .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
    .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
    .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
    .def("enableAll", [](AgentBrain& self) {
        self.enableRewardModulation(true);
        self.enableStructuralPlasticity(true);
        self.enableDevelopment(true);
        self.enableCuriosity(true);
    }, "Enable all learning systems")
    .def("disableAll", [](AgentBrain& self) {
        self.enableRewardModulation(false);
        self.enableStructuralPlasticity(false);
        self.enableDevelopment(false);
        self.enableCuriosity(false);
    }, "Disable all learning systems")
    .def("getSystemStatus", [](AgentBrain& self) {
        std::unordered_map<std::string, bool> status;
        status["reward_modulation"] = self.isRewardModulationEnabled();
        status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
        status["development"] = self.isDevelopmentEnabled();
        status["curiosity"] = self.isCuriosityEnabled();
        return status;
    }, "Get status of all learning systems");

// ============================================================================
// Enhanced Neuromodulator Classes
// ============================================================================

py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(
    Base class for neuromodulators with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def_readwrite("level", &Neuromodulator::level)
    .def_readwrite("effect", &Neuromodulator::effect)
    .def("getType", &Neuromodulator::getType)
    .def("setType", &Neuromodulator::setType, py::arg("type"))
    .def("update", &Neuromodulator::update, py::arg("delta_time"),
         "Update neuromodulator state")
    .def("reset", &Neuromodulator::reset);

py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(
    Dopamine neuromodulator with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def(py::init<float>(), py::arg("baseline"),
         "Create dopamine with baseline value")
    .def("getBaseline", &Dopamine::getBaseline)
    .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"))
    .def("getCurrent", &Dopamine::getCurrent)
    .def("setReward", &Dopamine::setReward, py::arg("reward"))
    .def("getReward", &Dopamine::getReward)
    .def("getPredictionError", &Dopamine::getPredictionError)
    .def("update", &Dopamine::update, py::arg("delta_time"), py::arg("reward"),
         "Update dopamine with new reward prediction error");

py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(
    Curiosity neuromodulator with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def(py::init<float, float>(), py::arg("baseline"), py::arg("decay_rate"),
         "Create curiosity with baseline and decay rate")
    .def("getBaseline", &Curiosity::getBaseline)
    .def("setBaseline", &Curiosity::setBaseline, py::arg("baseline"))
    .def("getDecayRate", &Curiosity::getDecayRate)
    .def("setDecayRate", &Curiosity::setDecayRate, py::arg("decay_rate"))
    .def("getNoveltySensing", &Curiosity::getNoveltySensing)
    .def("setNoveltySensing", &Curiosity::setNoveltySensing, py::arg("novelty"))
    .def("getExplorationDrive", &Curiosity::getExplorationDrive)
    .def("update", &Curiosity::update, py::arg("delta_time"), py::arg("novelty"),
         "Update curiosity with novelty signal");

py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(
    Novelty neuromodulator with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def(py::init<float, float>(), py::arg("baseline"), py::arg("sensitivity"),
         "Create novelty with baseline and sensitivity")
    .def("getBaseline", &Novelty::getBaseline)
    .def("setBaseline", &Novelty::setBaseline, py::arg("baseline"))
    .def("getSensitivity", &Novelty::getSensitivity)
    .def("setSensitivity", &Novelty::setSensitivity, py::arg("sensitivity"))
    .def("getNoveltySignal", &Novelty::getNoveltySignal)
    .def("setInputPattern", &Novelty::setInputPattern, py::arg("pattern"))
    .def("getInputPattern", &Novelty::getInputPattern)
    .def("update", &Novelty::update, py::arg("delta_time"),
         "Update novelty detection");

py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(
    Prediction error neuromodulator with enhanced Python interface
)pbdoc")
    .def(py::init<>())
    .def(py::init<float, float>(), py::arg("baseline"), py::arg("learning_rate"),
         "Create prediction error with baseline and learning rate")
    .def("getBaseline", &PredictionError::getBaseline)
    .def("setBaseline", &PredictionError::setBaseline, py::arg("baseline"))
    .def("getLearningRate", &PredictionError::getLearningRate)
    .def("setLearningRate", &PredictionError::setLearningRate, py::arg("learning_rate"))
    .def("getErrorSignal", &PredictionError::getErrorSignal)
    .def("setPredictionError", &PredictionError::setPredictionError, py::arg("error"))
    .def("getPrediction", &PredictionError::getPrediction)
    .def("setPrediction", &PredictionError::setPrediction, py::arg("prediction"))
    .def("update", &PredictionError::update, py::arg("delta_time"), py::arg("prediction"), py::arg("target"),
         "Update prediction error with new prediction and target");

// ============================================================================
// Factory Functions
// ============================================================================

m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
    return std::make_shared<Config>();
}, "Create a default configuration", py::return_value_policy::reference_internal);

m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
    return std::make_shared<Brain>(config);
}, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::reference_internal);

m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
    return std::make_shared<SimpleWorld>();
}, "Create a new simple world", py::return_value_policy::reference_internal);

m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
    return std::make_shared<AgentBrain>(brain);
}, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::reference_internal);

m.def("createVision", [](size_t width, size_t height, size_t channels) {
    auto vision = std::make_shared<Vision>(width, height, channels);
    return std::static_pointer_cast<SensoryInput>(vision);
}, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
   "Create vision sensory input (returns as SensoryInput)");

m.def("createAudio", [](size_t sample_rate, size_t num_samples) {
    auto audio = std::make_shared<Audio>(sample_rate, num_samples);
    return std::static_pointer_cast<SensoryInput>(audio);
}, py::arg("sample_rate"), py::arg("num_samples"),
   "Create audio sensory input (returns as SensoryInput)");

m.def("createInternalSignals", []() {
    auto internal = std::make_shared<InternalSignals>();
    return std::static_pointer_cast<SensoryInput>(internal);
}, "Create internal signals sensory input (returns as SensoryInput)");

m.def("createAction", py::overload_cast<>(&Action::Action), "Create empty action");

m.def("createAction", py::overload_cast<ActionType>(&Action::Action), py::arg("type"),
      "Create action with type");

m.def("createAction", py::overload_cast<ActionType, std::vector<float>>(&Action::Action),
      py::arg("type"), py::arg("parameters"),
      "Create action with type and parameters");

// ============================================================================
// Error Types
// ============================================================================

py::register_exception_translator([](std::exception const& e) {
    if (std::strcmp(e.what(), "std::runtime_error") == 0) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    }
});

py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

// ============================================================================
// Module Documentation
// ============================================================================

m.doc() = R"pbdoc(
    Enhanced NLM (Neural Learning Machine) Python Bindings
    
    This module provides an enhanced Python interface to the NLM neural
    simulation framework. It includes:
    
    - Configuration management with Pythonic interfaces
    - Sensory input/output with NumPy support
    - World simulation and agent interfaces
    - Neuromodulation systems
    - Memory and learning systems
    - Utility functions and helpers
    
    Key Features:
    - Context managers for resource management
    - NumPy array support for performance
    - Property-based interfaces
    - Comprehensive error handling
    - Type hints and documentation
    
    Usage Examples:
    
    >>> import pynlm
    >>> config = pynlm.createDefaultConfig()
    >>> config.set("random_seed", 42)
    >>> brain = pynlm.createBrain(config)
    >>> world = pynlm.createSimpleWorld()
    >>> agent_brain = pynlm.createAgentBrain(brain)
    >>> 
    >>> # Run simulation loop
    >>> for step in range(1000):
    ...     percept = world.getSensoryPercept()
    ...     agent_brain.processSensoryInput(percept)
    ...     world.update(0.001)
    >>> 
    >>> # Save brain state
    >>> brain.save("my_brain.nml")
    
    The enhanced bindings provide better performance, more intuitive APIs,
    and comprehensive error handling compared to the basic bindings.
)pbdoc";

} // namespace nlm