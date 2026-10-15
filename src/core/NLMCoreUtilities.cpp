#include "NLMCoreUtilities.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Brain.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdarg>

namespace nlm {

// Logging Utilities
namespace LoggingUtils {
    std::string formatTiming(double seconds) {
        if (seconds < 1.0) {
            return std::to_string(seconds * 1000) + "ms";
        } else if (seconds < 60.0) {
            return std::to_string(seconds) + "s";
        } else {
            int minutes = static_cast<int>(seconds / 60);
            int secs = static_cast<int>(seconds) % 60;
            return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
        }
    }
    
    std::string logStep(size_t step, size_t totalSteps) {
        std::stringstream ss;
        ss << "Step " << step << "/" << totalSteps 
           << " (" << (totalSteps > 0 ? (step * 100.0 / totalSteps) : 0) << "%)";
        return ss.str();
    }
    
    std::string logProgress(size_t completed, size_t total, const std::string& message) {
        std::stringstream ss;
        if (!message.empty()) {
            ss << message << ": ";
        }
        ss << completed << "/" << total << " (" 
           << (total > 0 ? (completed * 100.0 / total) : 0) << "%)";
        return ss.str();
    }
    
    std::string formatStats(size_t neurons, size_t synapses, size_t spikes, double time) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Neurons: " << neurons << ", Synapses: " << synapses 
           << ", Spikes: " << spikes << ", Time: " << time << "s";
        return ss.str();
    }
}

// Brain Initialization Utilities
namespace InitUtils {
    template<typename T>
    bool initializeComponent(std::shared_ptr<T> component, const std::string& componentName,
                            std::function<bool(std::shared_ptr<T>)> initFunc) {
        if (!component) {
            NLM_LOG_ERROR("Failed to initialize " + componentName + ": null pointer");
            return false;
        }
        
        try {
            if (initFunc(component)) {
                NLM_LOG_INFO(componentName + " initialized successfully");
                return true;
            } else {
                NLM_LOG_ERROR("Failed to initialize " + componentName);
                return false;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during " + componentName + " initialization: " + std::string(e.what()));
            return false;
        }
    }
    
    InitUtils::BrainInitResult safeBrainInitialize(std::shared_ptr<Config> config) {
        BrainInitResult result;
        result.success = false;
        result.errorMessage = "Unknown error";
        
        try {
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                result.success = true;
                result.brain = brain;
                NLM_LOG_INFO("Brain initialized successfully");
            } else {
                result.errorMessage = "Brain initialization failed";
                NLM_LOG_ERROR("Brain initialization failed");
            }
        } catch (const std::exception& e) {
            result.errorMessage = std::string("Exception during brain initialization: ") + e.what();
            NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        }
        
        return result;
    }
    
    bool validateComponentDependencies(const std::vector<std::string>& componentNames) {
        // Simple validation - check for null names and duplicates
        for (const auto& name : componentNames) {
            if (name.empty()) {
                NLM_LOG_ERROR("Empty component name found");
                return false;
            }
        }
        
        // Check for duplicates
        std::set<std::string> uniqueNames(componentNames.begin(), componentNames.end());
        if (uniqueNames.size() != componentNames.size()) {
            NLM_LOG_ERROR("Duplicate component names found");
            return false;
        }
        
        return true;
    }
}

// Current Injection Utilities
namespace InputUtils {
    InputPattern randomSensoryInput(size_t neuronCount, float current) {
        InputPattern pattern;
        pattern.name = "RandomSensoryInput";
        pattern.apply = [neuronCount, current](std::shared_ptr<Brain> brain) {
            for (size_t i = 0; i < neuronCount && i < brain->getTotalNeuronCount(); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, current);
            }
        };
        return pattern;
    }
    
    InputPattern correlatedInput(size_t preNeuron, size_t postNeuron, 
                                float preCurrent, float postCurrent, size_t delay) {
        InputPattern pattern;
        pattern.name = "CorrelatedInput";
        pattern.apply = [preNeuron, postNeuron, preCurrent, postCurrent, delay](std::shared_ptr<Brain> brain) {
            if (preNeuron < brain->getTotalNeuronCount()) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, preCurrent);
            }
            if (postNeuron < brain->getTotalNeuronCount()) {
                brain->injectCurrentToNeurons(NeuronType::Motor, postCurrent);
            }
        };
        return pattern;
    }
    
    InputPattern sustainedInput(float current, size_t duration) {
        InputPattern pattern;
        pattern.name = "SustainedInput";
        pattern.apply = [current, duration](std::shared_ptr<Brain> brain) {
            for (size_t i = 0; i < duration; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, current);
                brain->step(i, i * 0.001);
            }
        };
        return pattern;
    }
    
    void applyInputPattern(std::shared_ptr<Brain> brain, const InputPattern& pattern) {
        if (!brain) {
            NLM_LOG_ERROR("Cannot apply input pattern to null brain");
            return;
        }
        try {
            pattern.apply(brain);
            NLM_LOG_INFO("Applied input pattern: " + pattern.name);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to apply input pattern " + pattern.name + ": " + std::string(e.what()));
        }
    }
}

// Simulation Utilities
namespace SimulationUtils {
    SimulationUtils::SimulationStepResult safeSimulationStep(std::shared_ptr<Brain> brain, 
                                                           size_t step, double time) {
        SimulationStepResult result;
        result.success = false;
        result.stepCompleted = step;
        result.timeAtStep = time;
        result.errorMessage = "Unknown error";
        
        if (!brain) {
            result.errorMessage = "Null brain pointer";
            return result;
        }
        
        try {
            brain->step(step, time);
            result.success = true;
        } catch (const std::exception& e) {
            result.errorMessage = std::string("Exception during simulation step: ") + e.what();
            NLM_LOG_ERROR("Exception during simulation step: " + std::string(e.what()));
        }
        
        return result;
    }
    
    PerformanceMetrics calculatePerformance(const SimulationInfo& info) {
        PerformanceMetrics metrics;
        metrics.totalTime = info.currentTime;
        metrics.stepsCompleted = info.currentStep;
        metrics.spikesGenerated = 0;  // Would need to track from brain
        metrics.neuronsActive = 0;    // Would need to track from brain
        return metrics;
    }
}

// Experiment Utilities
namespace ExperimentUtils {
    ExperimentUtils::ExperimentResult runExperiment(std::function<void()> experimentFunc,
                                                   const std::string& name) {
        ExperimentResult result;
        result.experimentName = name;
        result.success = false;
        result.executionTime = 0.0;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            experimentFunc();
            result.success = true;
            NLM_LOG_INFO("Experiment '" + name + "' completed successfully");
        } catch (const std::exception& e) {
            result.errors.push_back(std::string("Exception: ") + e.what());
            NLM_LOG_ERROR("Experiment '" + name + "' failed: " + std::string(e.what()));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return result;
    }
    
    std::vector<ExperimentUtils::Metric> extractMetrics(const std::string& data) {
        std::vector<Metric> metrics;
        // Simple metric extraction from string data
        // In a real implementation, this would parse JSON or other structured data
        NLM_LOG_WARNING("Metric extraction from string data is a placeholder implementation");
        return metrics;
    }
}

// Configuration Utilities
namespace ConfigUtils {
    bool validateConfig(const Config& config, const std::vector<ValidationRule>& rules) {
        bool allValid = true;
        for (const auto& rule : rules) {
            if (!config.has(rule.key)) {
                if (!rule.errorMessage.empty()) {
                    NLM_LOG_WARNING(rule.errorMessage);
                }
                continue;
            }
            
            // Basic type validation (in a real implementation, would check actual types)
            if (rule.type == "int") {
                auto value = config.getOr<int>(rule.key, 0);
                if (rule.rule.substr(0, 4) == "min:") {
                    int minVal = std::stoi(rule.rule.substr(4));
                    if (value < minVal) {
                        NLM_LOG_ERROR("Configuration '" + rule.key + "': " + 
                                    std::to_string(value) + " is less than minimum " + 
                                    std::to_string(minVal));
                        allValid = false;
                    }
                } else if (rule.rule.substr(0, 4) == "max:") {
                    int maxVal = std::stoi(rule.rule.substr(4));
                    if (value > maxVal) {
                        NLM_LOG_ERROR("Configuration '" + rule.key + "': " + 
                                    std::to_string(value) + " exceeds maximum " + 
                                    std::to_string(maxVal));
                        allValid = false;
                    }
                }
            }
        }
        return allValid;
    }
    
    void setDefaultConfig(std::shared_ptr<Config> config) {
        if (!config) return;
        
        // Set sensible defaults if not already set
        if (!config->has("random_seed")) {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        }
        if (!config->has("log_level")) {
            config->set("log_level", "INFO", ConfigSource::Default);
        }
        if (!config->has("neuron_count")) {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        }
    }
    
    std::string generateConfigFileName(const std::string& baseName) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << baseName << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ".json";
        return ss.str();
    }
    
    bool isConfigFileValid(const std::string& filepath) {
        if (filepath.empty() || filepath.find('.json') == std::string::npos) {
            return false;
        }
        return FileUtils::fileExists(filepath);
    }
}

// Random Number Generation Utilities
namespace RandomUtils {
    RandomUtils::ThreadSafeRNG::ThreadSafeRNG(uint64_t seed) : generator(seed) {}
    
    int RandomUtils::ThreadSafeRNG::nextInt(int min, int max) {
        std::lock_guard<std::mutex> lock(mutex);
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator);
    }
    
    double RandomUtils::ThreadSafeRNG::nextDouble(double min, double max) {
        std::lock_guard<std::mutex> lock(mutex);
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator);
    }
    
    float RandomUtils::ThreadSafeRNG::nextFloat(float min, float max) {
        std::lock_guard<std::mutex> lock(mutex);
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator);
    }
    
    bool RandomUtils::ThreadSafeRNG::nextBool(double probability) {
        return nextDouble() < probability;
    }
    
    template<typename T>
    T RandomUtils::randomFromDistribution(const std::vector<std::pair<T, double>>& distribution) {
        double totalWeight = 0.0;
        for (const auto& pair : distribution) {
            totalWeight += pair.second;
        }
        
        double randomValue = ThreadSafeRNG().nextDouble() * totalWeight;
        double cumulativeWeight = 0.0;
        
        for (const auto& pair : distribution) {
            cumulativeWeight += pair.second;
            if (randomValue <= cumulativeWeight) {
                return pair.first;
            }
        }
        
        // Fallback to last element if rounding errors occur
        return distribution.back().first;
    }
}

// Memory Management Utilities
namespace MemoryUtils {
    void ObjectTracker::add(void* obj) {
        std::lock_guard<std::mutex> lock(mutex);
        objects.push_back(obj);
    }
    
    void ObjectTracker::remove(void* obj) {
        std::lock_guard<std::mutex> lock(mutex);
        objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
    }
    
    size_t ObjectTracker::count() const {
        std::lock_guard<std::mutex> lock(mutex);
        return objects.size();
    }
    
    template<typename T>
    void MemoryUtils::safeDelete(T*& ptr) {
        if (ptr != nullptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
}

// File I/O Utilities
namespace FileUtils {
    bool fileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    bool directoryExists(const std::string& path) {
        struct stat info;
        return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR) != 0;
    }
    
    std::vector<std::string> listFiles(const std::string& directory, 
                                       const std::string& extension) {
        std::vector<std::string> files;
        // Simplified implementation - in a real version would use proper directory traversal
        return files;
    }
    
    bool atomicWrite(const std::string& filepath, const std::string& content) {
        // Write to temporary file first, then rename for atomic operation
        std::string tempPath = filepath + ".tmp";
        std::ofstream file(tempPath);
        if (!file.is_open()) return false;
        
        file << content;
        file.close();
        
        // Atomic rename
        return std::rename(tempPath.c_str(), filepath.c_str()) == 0;
    }
    
    bool atomicRead(const std::string& filepath, std::string& content) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        return true;
    }
}

// String Manipulation Utilities
namespace StringUtils {
    std::string format(const std::string& format, ...) {
        va_list args;
        va_start(args, format);
        
        // Determine required buffer size
        va_list args_copy;
        va_copy(args_copy, args);
        int len = std::vsnprintf(nullptr, 0, format.c_str(), args_copy);
        va_end(args_copy);
        
        if (len < 0) {
            va_end(args);
            return "";
        }
        
        // Format string
        std::vector<char> buffer(len + 1);
        std::vsnprintf(buffer.data(), buffer.size(), format.c_str(), args);
        va_end(args);
        
        return std::string(buffer.data());
    }
    
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
    
    std::string getFileName(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return path;
        return path.substr(pos + 1);
    }
    
    std::string getFileExtension(const std::string& path) {
        size_t pos = path.find_last_of('.');
        if (pos == std::string::npos) return "";
        return path.substr(pos + 1);
    }
    
    std::string removeFileExtension(const std::string& path) {
        size_t pos = path.find_last_of('.');
        if (pos == std::string::npos) return path;
        return path.substr(0, pos);
    }
    
    std::string joinPaths(const std::string& path1, const std::string& path2) {
        if (path1.empty()) return path2;
        if (path2.empty()) return path1;
        
        char lastChar = path1.back();
        if (lastChar == '/' || lastChar == '\\') {
            if (path2.empty() || path2[0] == '/' || path2[0] == '\\') {
                return path1 + path2.substr(1);
            }
            return path1 + path2;
        } else {
            if (path2.empty() || path2[0] == '/' || path2[0] == '\\') {
                return path1 + path2;
            } else {
                return path1 + "/" + path2;
            }
        }
    }
}

// Explicit template instantiations for commonly used functions
namespace nlm {
    template bool InitUtils::initializeComponent<Brain>(std::shared_ptr<Brain> component, 
                                                       const std::string& componentName,
                                                       std::function<bool(std::shared_ptr<Brain>)> initFunc);
}

} // namespace nlm