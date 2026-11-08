// DemoFramework.cpp - Implementation of the demo framework utilities
// This file contains the implementation of all the classes declared in DemoFramework.hpp

#include "demo/DemoFramework.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <sys/stat.h> // For file stats
#include <unistd.h>    // For system calls

namespace nlm {

// DemoCLI implementation
DemoCLI::Options DemoCLI::parseArguments(int argc, char** argv) {
    Options options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--json-output" || arg == "--json") {
            options.jsonOutput = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                options.logLevel = argv[++i];
            }
        } else if (arg == "--test-filter") {
            if (i + 1 < argc) {
                options.testFilter = argv[++i];
            }
        } else if (arg == "--time-step") {
            if (i + 1 < argc) {
                options.timeStep = std::stod(argv[++i]);
            }
        } else if (arg == "--neuron-count") {
            if (i + 1 < argc) {
                options.neuronCount = std::stoi(argv[++i]);
            }
        } else if (arg == "--max-steps") {
            if (i + 1 < argc) {
                options.maxSteps = std::stoi(argv[++i]);
            }
        } else if (arg == "--enable-checkpoint") {
            options.enableCheckpoint = true;
        } else if (arg == "--enable-replay") {
            options.enableReplay = true;
        } else if (arg == "--enable-development") {
            options.enableDevelopment = true;
        } else {
            options.customArgs.push_back(arg);
        }
    }
    
    return options;
}

void DemoCLI::printHelp() {
    std::cout << "=== NLM Demo Framework Help ===" << std::endl << std::endl;
    std::cout << "General Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet             Suppress non-essential output" << std::endl;
    std::cout << "  --json-output, --json   Output results in JSON format" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config <file>          Load configuration from JSON file" << std::endl;
    std::cout << "  --output <file>          Write output to file" << std::endl;
    std::cout << "  --log-level <level>      Set logging level (debug, info, warning, error, critical)" << std::endl;
    std::cout << "  --test-filter <pattern>  Filter tests by name pattern" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Brain Configuration Options:" << std::endl;
    std::cout << "  --time-step <seconds>    Set simulation timestep" << std::endl;
    std::cout << "  --neuron-count <count>   Set number of neurons" << std::endl;
    std::cout << "  --max-steps <steps>      Set maximum simulation steps" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Feature Flags:" << std::endl;
    std::cout << "  --enable-checkpoint      Enable checkpointing" << std::endl;
    std::cout << "  --enable-replay          Enable replay mode" << std::endl;
    std::cout << "  --enable-development     Enable development mode" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --verbose --neuron-count 1000 --max-steps 5000" << std::endl;
    std::cout << "  ./nlm --json-output --output results.json --config demo_config.json" << std::endl;
    std::cout << "  ./nlm --enable-checkpoint --enable-replay --enable-development" << std::endl;
}

std::string DemoCLI::formatDuration(double seconds) {
    std::stringstream ss;
    if (seconds >= 60.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double secs = seconds - (minutes * 60.0);
        ss << minutes << "m " << std::fixed << std::setprecision(3) << secs << "s";
    } else {
        ss << std::fixed << std::setprecision(3) << seconds << "s";
    }
    return ss.str();
}

std::string DemoCLI::formatMemorySize(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < 3) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(unitIndex == 0 ? 0 : 2) << size << " " << units[unitIndex];
    return ss.str();
}

nlohmann::json DemoCLI::exportResults(const std::vector<DemoResult>& results) {
    nlohmann::json jsonArray = nlohmann::json::array();
    
    for (const auto& result : results) {
        nlohmann::json resultJson;
        resultJson["demo_name"] = result.demoName;
        resultJson["success"] = result.success;
        resultJson["execution_time"] = result.executionTime;
        resultJson["error_message"] = result.errorMessage;
        resultJson["metrics"] = result.metrics;
        
        jsonArray.push_back(resultJson);
    }
    
    return jsonArray;
}

// MetricsCollector implementation
void MetricsCollector::recordMetric(const std::string& name, double value,
                                   const std::string& unit,
                                   const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.emplace_back(name, value, unit);
    metrics_.back().tags = tags;
}

std::vector<MetricsCollector::Metric> MetricsCollector::getMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_;
}

void MetricsCollector::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.clear();
}

double MetricsCollector::getAverage(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<double> values;
    for (const auto& metric : metrics_) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double MetricsCollector::getMin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    double min = std::numeric_limits<double>::max();
    
    for (const auto& metric : metrics_) {
        if (metric.name == name && metric.value < min) {
            min = metric.value;
        }
    }
    
    return (min == std::numeric_limits<double>::max()) ? 0.0 : min;
}

double MetricsCollector::getMax(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    double max = std::numeric_limits<double>::lowest();
    
    for (const auto& metric : metrics_) {
        if (metric.name == name && metric.value > max) {
            max = metric.value;
        }
    }
    
    return (max == std::numeric_limits<double>::lowest()) ? 0.0 : max;
}

size_t MetricsCollector::getCount(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& metric : metrics_) {
        if (metric.name == name) {
            count++;
        }
    }
    return count;
}

// AdvancedConfig implementation
std::shared_ptr<Config> AdvancedConfig::loadJsonConfig(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Config file not found: " + filepath);
            return config;
        }
        
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Load configuration from JSON
        if (jsonConfig.contains("random_seed")) {
            config->set("random_seed", jsonConfig["random_seed"].get<int64_t>(), ConfigSource::File);
        }
        if (jsonConfig.contains("simulation_timestep")) {
            config->set("simulation_timestep", jsonConfig["simulation_timestep"].get<double>(), ConfigSource::File);
        }
        if (jsonConfig.contains("neuron_count")) {
            config->set("neuron_count", jsonConfig["neuron_count"].get<int64_t>(), ConfigSource::File);
        }
        if (jsonConfig.contains("region_count")) {
            config->set("region_count", jsonConfig["region_count"].get<int64_t>(), ConfigSource::File);
        }
        if (jsonConfig.contains("connection_probability")) {
            config->set("connection_probability", jsonConfig["connection_probability"].get<float>(), ConfigSource::File);
        }
        if (jsonConfig.contains("stdp_ltp_weight")) {
            config->set("stdp_ltp_weight", jsonConfig["stdp_ltp_weight"].get<float>(), ConfigSource::File);
        }
        if (jsonConfig.contains("stdp_ltd_weight")) {
            config->set("stdp_ltd_weight", jsonConfig["stdp_ltd_weight"].get<float>(), ConfigSource::File);
        }
        if (jsonConfig.contains("stdp_tau")) {
            config->set("stdp_tau", jsonConfig["stdp_tau"].get<float>(), ConfigSource::File);
        }
        if (jsonConfig.contains("synaptogenesis_rate")) {
            config->set("synaptogenesis_rate", jsonConfig["synaptogenesis_rate"].get<float>(), ConfigSource::File);
        }
        if (jsonConfig.contains("pruning_rate")) {
            config->set("pruning_rate", jsonConfig["pruning_rate"].get<float>(), ConfigSource::File);
        }
        if (jsonConfig.contains("enable_checkpointing")) {
            config->set("enable_checkpointing", jsonConfig["enable_checkpointing"].get<bool>(), ConfigSource::File);
        }
        if (jsonConfig.contains("enable_replay")) {
            config->set("enable_replay", jsonConfig["enable_replay"].get<bool>(), ConfigSource::File);
        }
        if (jsonConfig.contains("enable_development")) {
            config->set("enable_development", jsonConfig["enable_development"].get<bool>(), ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load JSON configuration: " + std::string(e.what()));
    }
    
    return config;
}

std::shared_ptr<Config> AdvancedConfig::loadCommandLineConfig(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    
    DemoCLI::Options options = DemoCLI::parseArguments(argc, argv);
    
    // Apply command line options to config
    if (options.neuronCount > 0) {
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    }
    if (options.maxSteps > 0) {
        config->set("max_steps", static_cast<int64_t>(options.maxSteps), ConfigSource::CommandLine);
    }
    if (options.timeStep > 0.0) {
        config->set("simulation_timestep", options.timeStep, ConfigSource::CommandLine);
    }
    if (options.enableCheckpoint) {
        config->set("enable_checkpointing", true, ConfigSource::CommandLine);
    }
    if (options.enableReplay) {
        config->set("enable_replay", true, ConfigSource::CommandLine);
    }
    if (options.enableDevelopment) {
        config->set("enable_development", true, ConfigSource::CommandLine);
    }
    
    return config;
}

std::shared_ptr<Config> AdvancedConfig::mergeConfigs(const std::shared_ptr<Config>& base,
                                                     const std::map<std::string, std::string>& overrides) {
    auto merged = std::make_shared<Config>(*base);
    
    for (const auto& [key, value] : overrides) {
        if (key == "random_seed") {
            try { merged->set("random_seed", std::stoll(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "simulation_timestep") {
            try { merged->set("simulation_timestep", std::stod(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "neuron_count") {
            try { merged->set("neuron_count", std::stoll(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "region_count") {
            try { merged->set("region_count", std::stoll(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "connection_probability") {
            try { merged->set("connection_probability", std::stof(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "stdp_ltp_weight") {
            try { merged->set("stdp_ltp_weight", std::stof(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "stdp_ltd_weight") {
            try { merged->set("stdp_ltd_weight", std::stof(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "stdp_tau") {
            try { merged->set("stdp_tau", std::stof(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "synaptogenesis_rate") {
            try { merged->set("synaptogenesis_rate", std::stof(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "pruning_rate") {
            try { merged->set("pruning_rate", std::stof(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "enable_checkpointing") {
            try { merged->set("enable_checkpointing", std::stoi(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "enable_replay") {
            try { merged->set("enable_replay", std::stoi(value), ConfigSource::Override); } catch (...) {}
        } else if (key == "enable_development") {
            try { merged->set("enable_development", std::stoi(value), ConfigSource::Override); } catch (...) {}
        }
    }
    
    return merged;
}

void AdvancedConfig::saveResultsJson(const std::string& filepath, const DemoResult& result) {
    nlohmann::json resultJson;
    resultJson["demo_name"] = result.demoName;
    resultJson["success"] = result.success;
    resultJson["execution_time"] = result.executionTime;
    resultJson["error_message"] = result.errorMessage;
    resultJson["metrics"] = result.metrics;
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << std::setw(2) << resultJson << std::endl;
        NLM_LOG_INFO("Results saved to JSON: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to save results to: " + filepath);
    }
}

std::vector<DemoResult> AdvancedConfig::loadResultsJson(const std::string& filepath) {
    std::vector<DemoResult> results;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Results file not found: " + filepath);
            return results;
        }
        
        nlohmann::json jsonArray;
        file >> jsonArray;
        
        for (const auto& jsonItem : jsonArray) {
            DemoResult result(jsonItem["demo_name"].get<std::string>());
            result.success = jsonItem["success"].get<bool>();
            result.executionTime = jsonItem["execution_time"].get<double>();
            result.errorMessage = jsonItem["error_message"].get<std::string>();
            result.metrics = jsonItem["metrics"].get<std::map<std::string, std::string>>();
            
            results.push_back(result);
        }
        
        NLM_LOG_INFO("Loaded results from JSON: " + filepath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load results from JSON: " + std::string(e.what()));
    }
    
    return results;
}

std::string AdvancedConfig::formatConfigForDisplay(const std::shared_ptr<Config>& config) {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "  random_seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
    ss << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "s" << std::endl;
    ss << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
    ss << "  region_count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
    ss << "  connection_probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
    ss << "  stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << std::endl;
    ss << "  stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << std::endl;
    ss << "  stdp_tau: " << config->getOr<float>("stdp_tau", 0.0f) << std::endl;
    ss << "  synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0f) << std::endl;
    ss << "  pruning_rate: " << config->getOr<float>("pruning_rate", 0.0f) << std::endl;
    ss << "  enable_checkpointing: " << (config->getOr<bool>("enable_checkpointing", false) ? "true" : "false") << std::endl;
    ss << "  enable_replay: " << (config->getOr<bool>("enable_replay", false) ? "true" : "false") << std::endl;
    ss << "  enable_development: " << (config->getOr<bool>("enable_development", false) ? "true" : "false") << std::endl;
    
    return ss.str();
}

// ProgressTracker implementation
ProgressTracker::ProgressTracker(const std::string& description, int totalSteps)
    : description_(description), totalSteps_(totalSteps), currentStep_(0) {
    startTime_ = std::chrono::system_clock::now();
}

void ProgressTracker::update(int currentStep, const std::string& status) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentStep_ = currentStep;
}

void ProgressTracker::increment(const std::string& status) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentStep_++;
}

void ProgressTracker::complete() {
    std::lock_guard<std::mutex> lock(mutex_);
    currentStep_ = totalSteps_;
}

std::string ProgressTracker::getProgressBar(double progress) const {
    std::stringstream ss;
    int barWidth = 50;
    
    ss << "[";
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) ss << "=";
        else if (i == pos) ss << ">";
        else ss << " ";
    }
    ss << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
    
    return ss.str();
}

std::string ProgressTracker::getStatusLine() const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
    
    std::stringstream ss;
    ss << "Progress: " << getProgressBar(static_cast<double>(currentStep_) / totalSteps_)
       << " (Step " << currentStep_ << "/" << totalSteps_ << ")"
       << " - " << description_
       << " - Elapsed: " << duration.count() << "s";
    
    return ss.str();
}

// DemoBrainWrapper implementation
DemoBrainWrapper::DemoBrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {
}

bool DemoBrainWrapper::step(SimulationStep step, double time) {
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = brain_->step(step, time);
    
    // Collect metrics
    metrics_.recordMetric("spike_count", brain_->getTotalSpikeCount(), "spikes");
    metrics_.recordMetric("firing_neuron_count", brain_->getFiringNeuronCount(), "neurons");
    metrics_.recordMetric("average_firing_rate", brain_->getAverageFiringRate(), "Hz");
    
    if (brain_->getWorkingMemory()) {
        metrics_.recordMetric("working_memory_neurons", brain_->getWorkingMemory()->getMemoryNeuronCount(), "neurons");
    }
    
    if (brain_->getEpisodicMemory()) {
        metrics_.recordMetric("episodic_memory_episodes", brain_->getEpisodicMemory()->getEpisodeCount(), "episodes");
    }
    
    return success;
}

size_t DemoBrainWrapper::getTotalSpikeCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return brain_->getTotalSpikeCount();
}

size_t DemoBrainWrapper::getFiringNeuronCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return brain_->getFiringNeuronCount();
}

double DemoBrainWrapper::getAverageFiringRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return brain_->getAverageFiringRate();
}

size_t DemoBrainWrapper::getWorkingMemoryNeurons() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return brain_->getWorkingMemory() ? brain_->getWorkingMemory()->getMemoryNeuronCount() : 0;
}

size_t DemoBrainWrapper::getEpisodicMemoryCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return brain_->getEpisodicMemory() ? brain_->getEpisodicMemory()->getEpisodeCount() : 0;
}

std::map<std::string, double> DemoBrainWrapper::getSystemMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<std::string, double> systemMetrics;
    
    systemMetrics["spike_count"] = brain_->getTotalSpikeCount();
    systemMetrics["firing_neuron_count"] = brain_->getFiringNeuronCount();
    systemMetrics["average_firing_rate"] = brain_->getAverageFiringRate();
    
    if (brain_->getWorkingMemory()) {
        systemMetrics["working_memory_neurons"] = brain_->getWorkingMemory()->getMemoryNeuronCount();
    }
    
    if (brain_->getEpisodicMemory()) {
        systemMetrics["episodic_memory_episodes"] = brain_->getEpisodicMemory()->getEpisodeCount();
    }
    
    return systemMetrics;
}

double DemoBrainWrapper::getMemoryUsage() const {
    // Simple memory usage approximation
    return static_cast<double>(getTotalSpikeCount()) * 100.0 + getWorkingMemoryNeurons() * 50.0;
}

double DemoBrainWrapper::getCPULoad() const {
    // Placeholder for CPU load estimation
    return 0.5; // Assume 50% CPU load
}

void DemoBrainWrapper::enableRecording(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    recording_ = true;
    recordingFile_ = filepath;
    NLM_LOG_INFO("Recording enabled: " + filepath);
}

void DemoBrainWrapper::disableRecording() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (recording_) {
        NLM_LOG_INFO("Recording disabled");
    }
    recording_ = false;
    recordingFile_ = "";
}

bool DemoBrainWrapper::isRecording() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return recording_;
}

void DemoBrainWrapper::applyConfig(const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Apply configuration to brain (implementation depends on brain interface)
    brain_->applyConfig(config);
}

// OutputFormatter implementation
std::string OutputFormatter::format(const DemoResult& result, Format format) {
    switch (format) {
        case Format::Text:
            return formatTextResult(result);
        case Format::Json:
            return formatJsonResult(result);
        case Format::Csv:
            return formatCsvResult(result);
        case Format::Html:
            return formatHtmlResult(result);
        default:
            return formatTextResult(result);
    }
}

std::string OutputFormatter::format(const std::vector<DemoResult>& results, Format format) {
    switch (format) {
        case Format::Text:
            return formatTextResults(results);
        case Format::Json:
            return formatJsonResults(results);
        case Format::Csv:
            return formatCsvResults(results);
        case Format::Html:
            return formatHtmlResults(results);
        default:
            return formatTextResults(results);
    }
}

std::string OutputFormatter::formatMetrics(const MetricsCollector& metrics, Format format) {
    switch (format) {
        case Format::Text:
            return formatTextMetrics(metrics);
        case Format::Json:
            return formatJsonMetrics(metrics);
        case Format::Csv:
            return formatCsvMetrics(metrics);
        case Format::Html:
            return formatHtmlMetrics(metrics);
        default:
            return formatTextMetrics(metrics);
    }
}

void OutputFormatter::writeToFile(const std::string& content, const std::string& filepath) {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << content;
        NLM_LOG_INFO("Output written to file: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to write output to: " + filepath);
    }
}

std::string OutputFormatter::escapeHtml(const std::string& text) {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '&', "&amp;");
    std::replace(escaped.begin(), escaped.end(), '<', "&lt;");
    std::replace(escaped.begin(), escaped.end(), '>', "&gt;");
    std::replace(escaped.begin(), escaped.end(), '"', "&quot;");
    std::replace(escaped.begin(), escaped.end(), "'", "&apos;");
    return escaped;
}

// Format implementation details (private helper functions)
std::string OutputFormatter::formatTextResult(const DemoResult& result) {
    std::stringstream ss;
    ss << "=== Demo: " << result.demoName << " ===" << std::endl;
    ss << "Status: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
    ss << "Execution Time: " << DemoCLI::formatDuration(result.executionTime) << std::endl;
    ss << "Error Message: " << result.errorMessage << std::endl;
    ss << "Metrics:" << std::endl;
    for (const auto& [key, value] : result.metrics) {
        ss << "  " << key << ": " << value << std::endl;
    }
    return ss.str();
}

std::string OutputFormatter::formatJsonResult(const DemoResult& result) {
    nlohmann::json json;
    json["demo_name"] = result.demoName;
    json["success"] = result.success;
    json["execution_time"] = result.executionTime;
    json["error_message"] = result.errorMessage;
    json["metrics"] = result.metrics;
    
    return json.dump(2);
}

// Additional format implementations (Csv, Html, etc.) would continue here...

} // namespace nlm
