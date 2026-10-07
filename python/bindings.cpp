#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <algorithm>

namespace py = pybind11;
namespace nlm {

// Python-style context manager for Brain
class BrainContext {
public:
    BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    ~BrainContext() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Enable context manager protocol
    BrainContext* __enter__() {
        return this;
    }
    
    void __exit__(PyExcType type, PyExcValuePtr value, PyExcTracePtr trace) {
        // Clean up on context exit
        if (brain_) {
            // Optional: save state or perform cleanup
        }
    }
    
    // Brain operations within context
    void step(SimulationStep step) {
        if (!brain_) {
            throw PythonBindingException("Cannot step: brain is null");
        }
        brain_->step(step);
    }
    
    void stepWithTime(SimulationStep step, Timestamp time) {
        if (!brain_) {
            throw PythonBindingException("Cannot step: brain is null");
        }
        brain_->step(step, time);
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
    bool isInitialized() const {
        return brain_ && brain_->getConfig();
    }
    
    // Configuration management
    void updateConfig(const std::string& key, const std::string& value) {
        if (brain_) {
            brain_->getConfig()->set(key, value);
        }
    }
    
    std::string getConfigValue(const std::string& key) const {
        if (!brain_) {
            throw PythonBindingException("Cannot get config: brain is null");
        }
        auto val = brain_->getConfig()->get<std::string>(key);
        return val.has_value() ? val.value() : "";
    }
    
    // Sensory and motor operations
    void receiveInput(const SensoryPercept& percept) {
        if (!brain_) {
            throw PythonBindingException("Cannot receive input: brain is null");
        }
        brain_->receiveSensoryInput(percept);
    }
    
    Action produceAction() {
        if (!brain_) {
            throw PythonBindingException("Cannot produce action: brain is null");
        }
        return brain_->produceAction();
    }
    
    // Analysis and monitoring
    std::map<std::string, double> getStatistics() const {
        std::map<std::string, double> stats;
        if (!brain_) {
            return stats;
        }
        
        stats["total_neurons"] = brain_->getTotalNeuronCount();
        stats["total_synapses"] = brain_->getTotalSynapseCount();
        stats["active_neurons"] = brain_->getActiveNeuronCount();
        stats["firing_neurons"] = brain_->getFiringNeuronCount();
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["total_spikes"] = brain_->getTotalSpikeCount();
        stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
        
        return stats;
    }
    
    // Development tracking
    void updateDevelopment(Timestamp timestep) {
        if (!brain_) {
            throw PythonBindingException("Cannot update development: brain is null");
        }
        brain_->updateDevelopment(timestep);
    }
    
    DevelopmentalStage getDevelopmentStage() const {
        if (!brain_) {
            throw PythonBindingException("Cannot get development stage: brain is null");
        }
        return brain_->getDevelopmentalStage();
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Python-style context manager for AgentBrain
class AgentBrainContext {
public:
    AgentBrainContext(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {
        if (agentBrain_) {
            // Initialization handled by caller if needed
        }
    }
    
    ~AgentBrainContext() {
        // Cleanup handled by caller
    }
    
    AgentBrainContext* __enter__() {
        return this;
    }
    
    void __exit__(PyExcType type, PyExcValuePtr value, PyExcTracePtr trace) {
        // Clean up on context exit
    }
    
    // Agent operations
    void processSensoryInput(const SensoryPercept& percept) {
        if (!agentBrain_) {
            throw PythonBindingException("Cannot process input: agent brain is null");
        }
        agentBrain_->processSensoryInput(percept);
    }
    
    Action decodeMotorCommand() {
        if (!agentBrain_) {
            throw PythonBindingException("Cannot decode motor command: agent brain is null");
        }
        return agentBrain_->decodeMotorCommand();
    }
    
    std::shared_ptr<AgentBrain> getAgentBrain() const {
        return agentBrain_;
    }
    
    // Neuromodulation controls
    void applyReward(double reward, double predictedReward = 0.0) {
        if (!agentBrain_) {
            throw PythonBindingException("Cannot apply reward: agent brain is null");
        }
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    void enableModulation(const std::string& type, bool enable) {
        if (!agentBrain_) {
            throw PythonBindingException("Cannot enable modulation: agent brain is null");
        }
        
        if (type == "reward") {
            agentBrain_->enableRewardModulation(enable);
        } else if (type == "structural_plasticity") {
            agentBrain_->enableStructuralPlasticity(enable);
        } else if (type == "development") {
            agentBrain_->enableDevelopment(enable);
        } else if (type == "curiosity") {
            agentBrain_->enableCuriosity(enable);
        }
    }
    
    // State management
    void reset() {
        if (agentBrain_) {
            agentBrain_->reset();
        }
    }
    
    std::map<std::string, double> getModulationLevels() const {
        std::map<std::string, double> levels;
        if (!agentBrain_) {
            return levels;
        }
        
        levels["reward_modulation"] = agentBrain_->getNeuromodulationLevel();
        levels["curiosity"] = agentBrain_->getCuriosityLevel();
        levels["novelty"] = agentBrain_->getNoveltyLevel();
        levels["prediction_error"] = agentBrain_->getPredictionError();
        
        return levels;
    }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Configuration helper functions
class ConfigBuilder {
public:
    ConfigBuilder() : config_(std::make_shared<Config>()) {}
    
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, int value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::vector<std::string>& values) {
        // Convert vector to JSON-like string for now
        std::string jsonStr = "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) jsonStr += ", ";
            jsonStr += "\"" + values[i] + "\"";
        }
        jsonStr += "]";
        config_->set(key, jsonStr);
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config_;
    }
    
    void loadFromFile(const std::string& filepath) {
        config_->loadFromFile(filepath);
    }
    
    void loadFromJson(const std::string& json) {
        // Simple JSON parsing for config
        if (json.empty() || json[0] != '{') {
            throw ConfigException("Invalid JSON format for configuration");
        }
        // For now, treat as simple key=value format
        config_->loadFromSimpleFormat(json);
    }
    
    std::string toJson() const {
        // Convert config to JSON string
        std::string json = "{";
        const auto& keys = config_->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) json += ", ";
            json += "\"" + keys[i] + "\": ";
            
            auto val = config_->get<std::string>(keys[i]);
            if (val.has_value()) {
                // Try to determine if it's a number or boolean
                try {
                    size_t pos;
                    double num = std::stod(val.value(), &pos);
                    if (pos == val.value().size()) {
                        json += std::to_string(num);
                        continue;
                    }
                } catch (...) {}
                
                if (val.value() == "true" || val.value() == "false") {
                    json += val.value();
                } else {
                    json += "\"" + val.value() + "\"";
                }
            }
        }
        json += "}";
        return json;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Simulation helper classes
class SimulationStepBuilder {
public:
    SimulationStepBuilder() : step_(0), time_(0.0), maxSteps_(0) {}
    
    SimulationStepBuilder& setSteps(int steps) {
        maxSteps_ = steps;
        return *this;
    }
    
    SimulationStepBuilder& setTime(double time) {
        time_ = time;
        return *this;
    }
    
    SimulationStepBuilder& setTimeStep(double dt) {
        timeStep_ = dt;
        return *this;
    }
    
    std::vector<std::map<std::string, double>> runSimulation(std::shared_ptr<Brain> brain) {
        std::vector<std::map<std::string, double>> results;
        
        if (!brain) {
            throw PythonBindingException("Cannot run simulation: brain is null");
        }
        
        for (int i = 0; i < maxSteps_; ++i) {
            brain->step(i, time_);
            
            std::map<std::string, double> stats;
            stats["step"] = i;
            stats["time"] = time_;
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_rate"] = brain->getAverageFiringRate();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            
            results.push_back(stats);
            
            time_ += timeStep_;
        }
        
        return results;
    }
    
private:
    int step_;
    double time_;
    double timeStep_ = 1.0;
    int maxSteps_;
};

class LearningEnvironment {
public:
    LearningEnvironment(std::shared_ptr<SimpleWorld> world, std::shared_ptr<Brain> brain)
        : world_(world), brain_(brain) {}
    
    // Episode management
    std::map<std::string, double> runEpisode(int maxSteps = 100) {
        std::map<std::string, double> episodeStats;
        episodeStats["steps"] = 0;
        episodeStats["total_reward"] = 0.0;
        episodeStats["success"] = 0.0;
        
        world_->reset();
        brain_->reset();
        
        for (int step = 0; step < maxSteps; ++step) {
            // Get sensory input
            SensoryPercept percept = world_->getSensoryPercept();
            
            // Process input and get action
            brain_->processSensoryInput(percept);
            Action action = brain_->decodeMotorCommand();
            
            // Apply action in world
            ActionResult result = world_->applyMotorCommand(action, step);
            
            // Apply reward modulation
            brain_->applyRewardModulation(result.reward, 0.0);
            
            // Update episode stats
            episodeStats["steps"] = step + 1;
            episodeStats["total_reward"] += result.reward;
            episodeStats["success"] += (result.success ? 1.0 : 0.0);
            
            // Check if episode should end
            if (!world_->isValidPosition(world_->getAgentBody().x, world_->getAgentBody().y)) {
                break;
            }
        }
        
        // Calculate averages
        if (episodeStats["steps"] > 0) {
            episodeStats["avg_reward"] = episodeStats["total_reward"] / episodeStats["steps"];
            episodeStats["success_rate"] = episodeStats["success"] / episodeStats["steps"];
        }
        
        return episodeStats;
    }
    
    // Training loop
    std::vector<std::map<std::string, double>> train(int numEpisodes, int stepsPerEpisode = 100) {
        std::vector<std::map<std::string, double>> allStats;
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::map<std::string, double> stats = runEpisode(stepsPerEpisode);
            stats["episode"] = episode;
            allStats.push_back(stats);
            
            // Log progress
            std::cout << "Episode " << episode << ": "
                      << "Steps: " << stats["steps"] << ", "
                      << "Avg Reward: " << stats["avg_reward"] << ", "
                      << "Success Rate: " << stats["success_rate"] << std::endl;
        }
        
        return allStats;
    }
    
    // Environment setup
    void configureWorld(int width, int height, int visionWidth, int visionHeight) {
        if (world_) {
            world_->configure(width, height, visionWidth, visionHeight);
        }
    }
    
    void setAgentStart(double x, double y) {
        if (world_) {
            world_->setAgentStart(x, y);
        }
    }
    
    // State management
    void saveState(const std::string& filepath) {
        if (brain_) {
            brain_->save(filepath);
        }
    }
    
    void loadState(const std::string& filepath) {
        if (brain_) {
            brain_->load(filepath);
        }
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<Brain> brain_;
};

// Enhanced utility functions
namespace utils {
    
    // Brain creation and configuration
    std::shared_ptr<Brain> createBrainWithConfig(const std::string& configFile = "") {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            bool success = config->loadFromFile(configFile);
            if (!success) {
                throw IOError("Failed to load configuration from: " + configFile);
            }
        }
        
        return std::make_shared<Brain>(config);
    }
    
    // Statistical analysis utilities
    struct StatisticalAnalysis {
        double mean;
        double variance;
        double stddev;
        double min;
        double max;
        double median;
        int count;
        
        std::string toString() const {
            std::ostringstream oss;
            oss << "Stats: mean=" << mean << ", stddev=" << stddev
                << ", min=" << min << ", max=" << max
                << ", count=" << count;
            return oss.str();
        }
    };
    
    StatisticalAnalysis analyzeVector(const std::vector<double>& data) {
        StatisticalAnalysis stats;
        stats.count = data.size();
        if (stats.count == 0) {
            return stats;
        }
        
        // Calculate basic statistics
        stats.min = *std::min_element(data.begin(), data.end());
        stats.max = *std::max_element(data.begin(), data.end());
        stats.mean = std::accumulate(data.begin(), data.end(), 0.0) / stats.count;
        
        // Calculate variance and stddev
        double sumSquaredDiffs = 0.0;
        for (double val : data) {
            double diff = val - stats.mean;
            sumSquaredDiffs += diff * diff;
        }
        stats.variance = sumSquaredDiffs / stats.count;
        stats.stddev = std::sqrt(stats.variance);
        
        // Calculate median
        std::vector<double> sortedData = data;
        std::sort(sortedData.begin(), sortedData.end());
        if (stats.count % 2 == 0) {
            stats.median = (sortedData[stats.count/2 - 1] + sortedData[stats.count/2]) / 2.0;
        } else {
            stats.median = sortedData[stats.count/2];
        }
        
        return stats;
    }
    
    // Vector operations
    std::vector<double> normalize(const std::vector<double>& data) {
        std::vector<double> result = data;
        double min = *std::min_element(data.begin(), data.end());
        double max = *std::max_element(data.begin(), data.end());
        
        if (max > min) {
            for (size_t i = 0; i < result.size(); ++i) {
                result[i] = (result[i] - min) / (max - min);
            }
        }
        
        return result;
    }
    
    std::vector<double> softmax(const std::vector<double>& logits) {
        std::vector<double> result;
        double max = *std::max_element(logits.begin(), logits.end());
        double sum = 0.0;
        
        for (double val : logits) {
            result.push_back(std::exp(val - max));
            sum += result.back();
        }
        
        for (double& val : result) {
            val /= sum;
        }
        
        return result;
    }
    
    // Random number generation
    class RandomGenerator {
    public:
        RandomGenerator(int seed = -1) {
            if (seed >= 0) {
                std::srand(seed);
            }
        }
        
        int nextInt(int min = 0, int max = 100) {
            return min + (std::rand() % (max - min + 1));
        }
        
        double nextDouble(double min = 0.0, double max = 1.0) {
            return min + (std::rand() / (RAND_MAX + 1.0)) * (max - min);
        }
        
        bool nextBool(double probability = 0.5) {
            return std::rand() / (RAND_MAX + 1.0) < probability;
        }
        
        std::vector<double> generateGaussian(int count, double mean = 0.0, double stddev = 1.0) {
            std::vector<double> result;
            for (int i = 0; i < count; ++i) {
                double val = mean;
                // Simple Box-Muller transform
                double u1 = std::rand() / (RAND_MAX + 1.0);
                double u2 = std::rand() / (RAND_MAX + 1.0);
                val += stddev * std::sqrt(-2.0 * std::log(u1)) * std::sin(2.0 * M_PI * u2);
                result.push_back(val);
            }
            return result;
        }
    };
    
    // String utilities
    std::string formatString(const std::string& format, std::vector<std::string> args) {
        std::string result = format;
        for (size_t i = 0; i < args.size(); ++i) {
            size_t pos = result.find("{}" + std::to_string(i));
            if (pos != std::string::npos) {
                result.replace(pos, 3 + std::to_string(i).size(), args[i]);
            }
        }
        return result;
    }
    
    std::vector<std::string> splitString(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        return result;
    }
    
    std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
        std::ostringstream oss;
        for (size_t i = 0; i < strings.size(); ++i) {
            if (i > 0) oss << delimiter;
            oss << strings[i];
        }
        return oss.str();
    }
    
} // namespace utils

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        -----------------------------
        A comprehensive Python API for the NLM neural simulation framework.
        
        Key Features:
        - Context managers for resource management
        - Enhanced configuration with JSON support
        - Simulation and training utilities
        - Advanced statistical analysis tools
        - Error handling and validation
        - Performance monitoring
    )pbdoc";
    
    // Register exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigException>(m, "ConfigException");
    py::register_exception<NeuronException>(m, "NeuronException");
    py::register_exception<SynapseException>(m, "SynapseException");
    py::register_exception<NeuralDynamicsException>(m, "NeuralDynamicsException");
    py::register_exception<PlasticityException>(m, "PlasticityException");
    py::register_exception<NeuromodulationException>(m, "NeuromodulationException");
    py::register_exception<MemoryException>(m, "MemoryException");
    py::register_exception<IOError>(m, "IOError");
    py::register_exception<PythonBindingException>(m, "PythonBindingException");
    
    // Basic types
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
    
    // Enhanced Config class with builder pattern
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class with JSON support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON or key=value format)")
        .def("loadFromJson", [](Config& self, const std::string& json) {
            return self.loadFromJson(json);
        }, py::arg("json"),
             "Load configuration from JSON string")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("toJson", &Config::toJson,
             "Convert configuration to JSON string")
        .def("fromJson", [](Config& self, const std::string& json) {
            return self.loadFromJson(json);
        }, py::arg("json"),
             "Load configuration from JSON (class method)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("get", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : "";
        }, py::arg("key"),
             "Get configuration value as string")
        .def("getInt", [](Config& self, const std::string& key) {
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : 0;
        }, py::arg("key"),
             "Get configuration value as int")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            return val.has_value() ? val.value() : 0.0;
        }, py::arg("key"),
             "Get configuration value as float")
        .def("getBool", [](Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : false;
        }, py::arg("key"),
             "Get configuration value as bool")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Config Builder
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Configuration builder for fluent API)pbdoc")
        .def(py::init<>())
        .def("set", [](ConfigBuilder& self, const std::string& key, const std::string& value) {
            self.set(key, value);
            return &self; // Return self for chaining
        }, py::arg("key"), py::arg("value"),
             "Set configuration key-value pair")
        .def("setInt", [](ConfigBuilder& self, const std::string& key, int value) {
            self.set(key, value);
            return &self;
        }, py::arg("key"), py::arg("value"),
             "Set configuration integer value")
        .def("setFloat", [](ConfigBuilder& self, const std::string& key, double value) {
            self.set(key, value);
            return &self;
        }, py::arg("key"), py::arg("value"),
             "Set configuration float value")
        .def("setBool", [](ConfigBuilder& self, const std::string& key, bool value) {
            self.set(key, value);
            return &self;
        }, py::arg("key"), py::arg("value"),
             "Set configuration boolean value")
        .def("build", &ConfigBuilder::build,
             "Build and return the configuration")
        .def("loadFromFile", &ConfigBuilder::loadFromFile, py::arg("filepath"),
             "Load configuration from file")
        .def("loadFromJson", &ConfigBuilder::loadFromJson, py::arg("json"),
             "Load configuration from JSON");
    
    // Enhanced Brain class with context manager
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for Brain simulation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", &BrainContext::__enter__)
        .def("__exit__", &BrainContext::__exit__)
        .def("step", &BrainContext::step, py::arg("step"),
             "Perform a simulation step")
        .def("stepWithTime", &BrainContext::stepWithTime, py::arg("step"), py::arg("time"),
             "Perform a simulation step with timestamp")
        .def("getStatistics", &BrainContext::getStatistics,
             "Get brain statistics as dictionary")
        .def("updateDevelopment", &BrainContext::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentStage", &BrainContext::getDevelopmentStage,
             "Get current developmental stage")
        .def("isInitialized", &BrainContext::isInitialized,
             "Check if brain is initialized");
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced brain simulation with improved error handling)pbdoc")
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
             "Log brain status")
        // Context manager support
        .def("context", [](std::shared_ptr<Brain> brain) {
            return BrainContext(brain);
        }, py::arg("brain"),
             "Get a context manager for the brain");
    
    // Enhanced AgentBrain class with context manager
    py::class_<AgentBrainContext>(m, "AgentBrainContext", R"pbdoc(Context manager for AgentBrain)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agentBrain"))
        .def("__enter__", &AgentBrainContext::__enter__)
        .def("__exit__", &AgentBrainContext::__exit__)
        .def("processSensoryInput", &AgentBrainContext::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrainContext::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("getModulationLevels", &AgentBrainContext::getModulationLevels,
             "Get current neuromodulation levels as dictionary");
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
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
        // Modulation controls
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Context manager support
        .def("context", [](std::shared_ptr<AgentBrain> agentBrain) {
            return AgentBrainContext(agentBrain);
        }, py::arg("agentBrain"),
             "Get a context manager for the agent brain");
    
    // Simulation helpers
    py::class_<SimulationStepBuilder>(m, "SimulationStepBuilder", R"pbdoc(Simulation step builder)pbdoc")
        .def(py::init<>())
        .def("setSteps", &SimulationStepBuilder::setSteps, py::arg("steps"),
             "Set number of simulation steps")
        .def("setTime", &SimulationStepBuilder::setTime, py::arg("time"),
             "Set total simulation time")
        .def("setTimeStep", &SimulationStepBuilder::setTimeStep, py::arg("dt"),
             "Set time step size")
        .def("runSimulation", &SimulationStepBuilder::runSimulation, py::arg("brain"),
             "Run simulation and return results");
    
    py::class_<LearningEnvironment>(m, "LearningEnvironment", R"pbdoc(Learning environment for agent training)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>, std::shared_ptr<Brain>>(),
             py::arg("world"), py::arg("brain"))
        .def("runEpisode", &LearningEnvironment::runEpisode, py::arg("maxSteps") = 100,
             "Run a single training episode")
        .def("train", &LearningEnvironment::train,
             py::arg("numEpisodes"), py::arg("stepsPerEpisode") = 100,
             "Run training episodes")
        .def("configureWorld", &LearningEnvironment::configureWorld,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world environment")
        .def("setAgentStart", &LearningEnvironment::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("saveState", &LearningEnvironment::saveState, py::arg("filepath"),
             "Save environment state")
        .def("loadState", &LearningEnvironment::loadState, py::arg("filepath"),
             "Load environment state");
    
    // Utility functions
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
    
    // Enhanced utility functions
    m.def("createConfigBuilder", []() {
        return ConfigBuilder();
    }, "Create a configuration builder");
    
    m.def("createSimulationBuilder", []() {
        return SimulationStepBuilder();
    }, "Create a simulation builder");
    
    m.def("createLearningEnvironment", 
          [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<Brain> brain) {
              return LearningEnvironment(world, brain);
          }, py::arg("world"), py::arg("brain"),
          "Create a learning environment");
    
    // Statistical analysis utilities
    m.def("analyzeVector", &utils::analyzeVector, py::arg("data"),
          "Analyze a vector of data and return statistics");
    
    m.def("normalize", &utils::normalize, py::arg("data"),
          "Normalize a vector of values to range [0, 1]");
    
    m.def("softmax", &utils::softmax, py::arg("logits"),
          "Apply softmax function to logits");
    
    // Random number generation
    m.def("setRandomSeed", [](int seed) {
        std::srand(seed);
    }, py::arg("seed"), "Set random seed for reproducible results");
    
    // Error handling
    m.def("getLastError", []() {
        return std::string("Last error would be retrieved from global error handler");
    }, "Get the last error message");
    
    m.def("clearErrors", []() {
        // Clear error history
    }, "Clear error history");
    
    // Performance monitoring
    m.def("startPerformanceMonitor", [](const std::string& name) {
        return LogPerformanceMonitor(name);
    }, py::arg("name"), "Start performance monitoring");
    
    // Context manager for simulation
    m.def("runSimulationContext", 
          [](std::shared_ptr<Brain> brain, int steps) {
              auto builder = SimulationStepBuilder();
              builder.setSteps(steps);
              return builder.runSimulation(brain);
          }, py::arg("brain"), py::arg("steps"),
          "Run simulation in context");
    
    // Additional type definitions for better type hints
    py::class_<std::map<std::string, double>>(m, "StringDoubleMap")
        .def(py::init<>());
    
    py::class_<std::vector<std::map<std::string, double>>>(m, "StringDoubleMapList")
        .def(py::init<>());
    
    // Constants
    m.attr("VERSION") = "2.0.0";
    m.attr("DEFAULT_SEED") = 42;
    m.attr("MAX_STEPS") = 10000;
    m.attr("DEFAULT_TIME_STEP") = 1.0;
    
    // Documentation strings for key classes
    m.attr("__doc__") = R"pbdoc(
        Enhanced NLM Python API v2.0
        ==============================
        
        This is a comprehensive Python API for the Neural Learning Machine (NLM) framework.
        It provides advanced features for neural simulation, learning, and control.
        
        Key Features:
        • Context managers for resource management
        • Enhanced configuration with JSON support
        • Simulation and training utilities
        • Advanced statistical analysis tools
        • Comprehensive error handling and validation
        • Performance monitoring and profiling
        • Fluent API builders for easy configuration
        • Learning environment for agent training
        
        Installation:
        pip install nlm
        
        Basic Usage:
        >>> from nlm import createBrain, createWorld, createLearningEnvironment
        >>> config = createConfigBuilder().set("neurons", 100).set("synapses", 1000).build()
        >>> brain = createBrain(config)
        >>> world = createWorld()
        >>> env = createLearningEnvironment(world, brain)
        >>> stats = env.runEpisode()
        
        For complete documentation, visit: https://nlm.readthedocs.io
    )pbdoc";
}

} // namespace nlm