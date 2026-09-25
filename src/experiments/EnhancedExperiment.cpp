#include "EnhancedExperiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct EnhancedExperiment::Impl {
    std::string name;
    std::string description;
    ExperimentState state;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    uint64_t seed;
    
    std::shared_ptr<Config> config;
    std::unordered_map<std::string, std::string> parameters;
    
    struct MetricEntry {
        std::string name;
        double value;
        std::string category;
        std::unordered_map<std::string, std::string> metadata;
        std::chrono::system_clock::time_point timestamp;
    };
    std::vector<MetricEntry> metrics;
    
    std::vector<EnhancedStepData> stepData;
    
    struct NoteEntry {
        std::string category;
        std::string content;
        std::chrono::system_clock::time_point timestamp;
    };
    std::vector<NoteEntry> notes;
    
    std::string templateName;
    
    // For checkpointing
    std::string checkpointPath;
    
    Impl() : state(ExperimentState::UNINITIALIZED), seed(0) {
        setStartTime();
    }
};

EnhancedExperiment::EnhancedExperiment() : pImpl(new Impl) {}

EnhancedExperiment::EnhancedExperiment(const std::string& name) : pImpl(new Impl) {
    pImpl->name = name;
}

EnhancedExperiment::EnhancedExperiment(const std::string& name, const std::string& description) : pImpl(new Impl) {
    pImpl->name = name;
    pImpl->description = description;
}

EnhancedExperiment::~EnhancedExperiment() = default;

const std::string& EnhancedExperiment::getName() const {
    return pImpl->name;
}

void EnhancedExperiment::setName(const std::string& name) {
    pImpl->name = name;
}

const std::string& EnhancedExperiment::getDescription() const {
    return pImpl->description;
}

void EnhancedExperiment::setDescription(const std::string& description) {
    pImpl->description = description;
}

ExperimentState EnhancedExperiment::getState() const {
    return pImpl->state;
}

bool EnhancedExperiment::isRunning() const {
    return pImpl->state == ExperimentState::RUNNING;
}

bool EnhancedExperiment::isCompleted() const {
    return pImpl->state == ExperimentState::COMPLETED;
}

bool EnhancedExperiment::isFailed() const {
    return pImpl->state == ExperimentState::FAILED;
}

void EnhancedExperiment::initialize() {
    if (pImpl->state == ExperimentState::UNINITIALIZED) {
        pImpl->state = ExperimentState::INITIALIZED;
        NLM_LOG_INFO("Experiment \"" + pImpl->name + "\" initialized");
    }
}

void EnhancedExperiment::start() {
    if (pImpl->state == ExperimentState::INITIALIZED) {
        pImpl->state = ExperimentState::RUNNING;
        setStartTime();
        NLM_LOG_INFO("Experiment \"" + pImpl->name + "\" started");
    }
}

void EnhancedExperiment::pause() {
    if (pImpl->state == ExperimentState::RUNNING) {
        pImpl->state = ExperimentState::PAUSED;
        setEndTime();
        NLM_LOG_INFO("Experiment \"" + pImpl->name + "\" paused");
    }
}

void EnhancedExperiment::resume() {
    if (pImpl->state == ExperimentState::PAUSED) {
        // In a real implementation, we would track pause duration
        pImpl->state = ExperimentState::RUNNING;
        NLM_LOG_INFO("Experiment \"" + pImpl->name + "\" resumed");
    }
}

void EnhancedExperiment::complete() {
    if (pImpl->state == ExperimentState::RUNNING || pImpl->state == ExperimentState::PAUSED) {
        pImpl->state = ExperimentState::COMPLETED;
        setEndTime();
        NLM_LOG_INFO("Experiment \"" + pImpl->name + "\" completed successfully");
    }
}

void EnhancedExperiment::fail(const std::string& errorMessage) {
    pImpl->state = ExperimentState::FAILED;
    setEndTime();
    addNote("error", "Experiment failed: " + errorMessage);
    NLM_LOG_ERROR("Experiment \"" + pImpl->name + "\" failed: " + errorMessage);
}

void EnhancedExperiment::setStartTime() {
    pImpl->startTime = std::chrono::system_clock::now();
}

void EnhancedExperiment::setEndTime() {
    pImpl->endTime = std::chrono::system_clock::now();
}

const std::chrono::system_clock::time_point& EnhancedExperiment::getStartTime() const {
    return pImpl->startTime;
}

const std::chrono::system_clock::time_point& EnhancedExperiment::getEndTime() const {
    return pImpl->endTime;
}

double EnhancedExperiment::getDuration() const {
    if (pImpl->endTime.time_since_epoch().count() == 0) {
        return std::chrono::duration<double>(std::chrono::system_clock::now() - pImpl->startTime).count();
    }
    return std::chrono::duration<double>(pImpl->endTime - pImpl->startTime).count();
}

bool EnhancedExperiment::saveState(const std::string& filepath) const {
    try {
        json j;
        
        j["name"] = pImpl->name;
        j["description"] = pImpl->description;
        j["state"] = static_cast<int>(pImpl->state);
        j["startTime"] = std::chrono::system_clock::to_time_t(pImpl->startTime);
        j["endTime"] = std::chrono::system_clock::to_time_t(pImpl->endTime);
        j["seed"] = pImpl->seed;
        j["templateName"] = pImpl->templateName;
        j["checkpointPath"] = pImpl->checkpointPath;
        
        // Save config
        if (pImpl->config) {
            j["config"] = pImpl->config->toString();
        }
        
        // Save parameters
        json paramsJson = json::object();
        for (const auto& pair : pImpl->parameters) {
            paramsJson[pair.first] = pair.second;
        }
        j["parameters"] = paramsJson;
        
        // Save metrics
        json metricsJson = json::array();
        for (const auto& metric : pImpl->metrics) {
            json metricJson;
            metricJson["name"] = metric.name;
            metricJson["value"] = metric.value;
            metricJson["category"] = metric.category;
            metricJson["timestamp"] = std::chrono::system_clock::to_time_t(metric.timestamp);
            
            json metadataJson = json::object();
            for (const auto& metaPair : metric.metadata) {
                metadataJson[metaPair.first] = metaPair.second;
            }
            metricJson["metadata"] = metadataJson;
            
            metricsJson.push_back(metricJson);
        }
        j["metrics"] = metricsJson;
        
        // Save step data
        json stepsJson = json::array();
        for (const auto& step : pImpl->stepData) {
            json stepJson;
            stepJson["step"] = step.step;
            stepJson["timestamp"] = std::chrono::system_clock::to_time_t(step.timestamp);
            stepJson["neuronCount"] = step.neuronCount;
            stepJson["synapseCount"] = step.synapseCount;
            stepJson["firingCount"] = step.firingCount;
            stepJson["avgFiringRate"] = step.avgFiringRate;
            stepJson["reward"] = step.reward;
            
            json additionalJson = json::object();
            for (const auto& addPair : step.additionalMetrics) {
                additionalJson[addPair.first] = addPair.second;
            }
            stepJson["additionalMetrics"] = additionalJson;
            
            stepsJson.push_back(stepJson);
        }
        j["stepData"] = stepsJson;
        
        // Save notes
        json notesJson = json::array();
        for (const auto& note : pImpl->notes) {
            json noteJson;
            noteJson["category"] = note.category;
            noteJson["content"] = note.content;
            noteJson["timestamp"] = std::chrono::system_clock::to_time_t(note.timestamp);
            notesJson.push_back(noteJson);
        }
        j["notes"] = notesJson;
        
        // Write to file
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            pImpl->checkpointPath = filepath;
            NLM_LOG_INFO("Experiment state saved to " + filepath);
            return true;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save experiment state: " + std::string(e.what()));
    }
    
    return false;
}

bool EnhancedExperiment::loadState(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open experiment state file: " + filepath);
            return false;
        }
        
        json j;
        file >> j;
        
        pImpl->name = j.value("name", "");
        pImpl->description = j.value("description", "");
        pImpl->state = static_cast<ExperimentState>(j.value("state", 0));
        pImpl->startTime = std::chrono::system_clock::from_time_t(j.value("startTime", 0));
        pImpl->endTime = std::chrono::system_clock::from_time_t(j.value("endTime", 0));
        pImpl->seed = j.value("seed", 0);
        pImpl->templateName = j.value("templateName", "");
        pImpl->checkpointPath = j.value("checkpointPath", "");
        
        // Load config (simplified)
        // In real implementation, would use Config class properly
        
        // Load parameters
        pImpl->parameters.clear();
        if (j.contains("parameters")) {
            for (const auto& pair : j["parameters"].items()) {
                pImpl->parameters[pair.key()] = pair.value();
            }
        }
        
        // Load metrics
        pImpl->metrics.clear();
        if (j.contains("metrics")) {
            for (const auto& metricJson : j["metrics"]) {
                Impl::MetricEntry metric;
                metric.name = metricJson.value("name", "");
                metric.value = metricJson.value("value", 0.0);
                metric.category = metricJson.value("category", "general");
                metric.timestamp = std::chrono::system_clock::from_time_t(metricJson.value("timestamp", 0));
                
                if (metricJson.contains("metadata")) {
                    for (const auto& metaPair : metricJson["metadata"].items()) {
                        metric.metadata[metaPair.key()] = metaPair.value();
                    }
                }
                
                pImpl->metrics.push_back(metric);
            }
        }
        
        // Load step data
        pImpl->stepData.clear();
        if (j.contains("stepData")) {
            for (const auto& stepJson : j["stepData"]) {
                EnhancedStepData step;
                step.step = stepJson.value("step", 0);
                step.timestamp = std::chrono::system_clock::from_time_t(stepJson.value("timestamp", 0));
                step.neuronCount = stepJson.value("neuronCount", 0);
                step.synapseCount = stepJson.value("synapseCount", 0);
                step.firingCount = stepJson.value("firingCount", 0);
                step.avgFiringRate = stepJson.value("avgFiringRate", 0.0f);
                step.reward = stepJson.value("reward", 0.0);
                
                if (stepJson.contains("additionalMetrics")) {
                    for (const auto& addPair : stepJson["additionalMetrics"].items()) {
                        step.additionalMetrics[addPair.key()] = addPair.value();
                    }
                }
                
                pImpl->stepData.push_back(step);
            }
        }
        
        // Load notes
        pImpl->notes.clear();
        if (j.contains("notes")) {
            for (const auto& noteJson : j["notes"]) {
                Impl::NoteEntry note;
                note.category = noteJson.value("category", "general");
                note.content = noteJson.value("content", "");
                note.timestamp = std::chrono::system_clock::from_time_t(noteJson.value("timestamp", 0));
                pImpl->notes.push_back(note);
            }
        }
        
        NLM_LOG_INFO("Experiment state loaded from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load experiment state: " + std::string(e.what()));
    }
    
    return false;
}

bool EnhancedExperiment::hasCheckpoint() const {
    return !pImpl->checkpointPath.empty() && std::filesystem::exists(pImpl->checkpointPath);
}

bool EnhancedExperiment::clearCheckpoint() {
    if (hasCheckpoint()) {
        std::error_code ec;
        std::filesystem::remove(pImpl->checkpointPath, ec);
        if (ec) {
            NLM_LOG_ERROR("Failed to remove checkpoint: " + ec.message());
            return false;
        }
    }
    pImpl->checkpointPath.clear();
    NLM_LOG_INFO("Checkpoint cleared");
    return true;
}

void EnhancedExperiment::setConfig(const std::shared_ptr<Config>& config) {
    pImpl->config = config;
}

const std::shared_ptr<Config>& EnhancedExperiment::getConfig() const {
    return pImpl->config;
}

void EnhancedExperiment::setParameter(const std::string& key, const std::string& value) {
    pImpl->parameters[key] = value;
}

std::string EnhancedExperiment::getParameter(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    return it != pImpl->parameters.end() ? it->second : "";
}

bool EnhancedExperiment::hasParameter(const std::string& key) const {
    return pImpl->parameters.find(key) != pImpl->parameters.end();
}

std::vector<std::string> EnhancedExperiment::getParameterKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->parameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

void EnhancedExperiment::recordMetric(const std::string& name, double value,
                                     const std::string& category,
                                     const std::unordered_map<std::string, std::string>& metadata) {
    Impl::MetricEntry metric;
    metric.name = name;
    metric.value = value;
    metric.category = category;
    metric.metadata = metadata;
    metric.timestamp = std::chrono::system_clock::now();
    
    pImpl->metrics.push_back(metric);
}

void EnhancedExperiment::recordMetric(const std::string& name, float value,
                                     const std::string& category,
                                     const std::unordered_map<std::string, std::string>& metadata) {
    recordMetric(name, static_cast<double>(value), category, metadata);
}

void EnhancedExperiment::recordMetric(const std::string& name, int value,
                                     const std::string& category,
                                     const std::unordered_map<std::string, std::string>& metadata) {
    recordMetric(name, static_cast<double>(value), category, metadata);
}

const std::vector<std::pair<std::string, double>>& EnhancedExperiment::getMetrics() const {
    static std::vector<std::pair<std::string, double>> allMetrics;
    allMetrics.clear();
    
    for (const auto& metric : pImpl->metrics) {
        allMetrics.emplace_back(metric.name, metric.value);
    }
    
    return allMetrics;
}

const std::vector<std::pair<std::string, double>>& EnhancedExperiment::getMetrics(const std::string& category) const {
    static std::vector<std::pair<std::string, double>> categoryMetrics;
    categoryMetrics.clear();
    
    for (const auto& metric : pImpl->metrics) {
        if (metric.category == category) {
            categoryMetrics.emplace_back(metric.name, metric.value);
        }
    }
    
    return categoryMetrics;
}

double EnhancedExperiment::getMetric(const std::string& name) const {
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name) {
            return metric.value;
        }
    }
    return 0.0;
}

double EnhancedExperiment::getMetric(const std::string& name, const std::string& category) const {
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name && metric.category == category) {
            return metric.value;
        }
    }
    return 0.0;
}

bool EnhancedExperiment::hasMetric(const std::string& name) const {
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name) {
            return true;
        }
    }
    return false;
}

bool EnhancedExperiment::hasMetric(const std::string& name, const std::string& category) const {
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name && metric.category == category) {
            return true;
        }
    }
    return false;
}

double EnhancedExperiment::getMetricMean(const std::string& name) const {
    std::vector<double> values;
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double EnhancedExperiment::getMetricStdDev(const std::string& name) const {
    std::vector<double> values;
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.size() < 2) return 0.0;
    
    double mean = getMetricMean(name);
    double sumSquares = 0.0;
    for (double v : values) {
        double diff = v - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (values.size() - 1));
}

double EnhancedExperiment::getMetricMin(const std::string& name) const {
    std::vector<double> values;
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) return 0.0;
    
    return *std::min_element(values.begin(), values.end());
}

double EnhancedExperiment::getMetricMax(const std::string& name) const {
    std::vector<double> values;
    for (const auto& metric : pImpl->metrics) {
        if (metric.name == name) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) return 0.0;
    
    return *std::max_element(values.begin(), values.end());
}

void EnhancedExperiment::recordStep(const EnhancedStepData& stepData) {
    pImpl->stepData.push_back(stepData);
}

void EnhancedExperiment::recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                                   size_t firingCount, float avgFiringRate, double reward) {
    EnhancedStepData data;
    data.step = step;
    data.timestamp = std::chrono::system_clock::now();
    data.neuronCount = neuronCount;
    data.synapseCount = synapseCount;
    data.firingCount = firingCount;
    data.avgFiringRate = avgFiringRate;
    data.reward = reward;
    
    recordStep(data);
}

const std::vector<EnhancedExperiment::EnhancedStepData>& EnhancedExperiment::getStepData() const {
    return pImpl->stepData;
}

void EnhancedExperiment::addNote(const std::string& note) {
    Impl::NoteEntry noteEntry;
    noteEntry.category = "general";
    noteEntry.content = note;
    noteEntry.timestamp = std::chrono::system_clock::now();
    pImpl->notes.push_back(noteEntry);
}

void EnhancedExperiment::addNote(const std::string& category, const std::string& note) {
    Impl::NoteEntry noteEntry;
    noteEntry.category = category;
    noteEntry.content = note;
    noteEntry.timestamp = std::chrono::system_clock::now();
    pImpl->notes.push_back(noteEntry);
}

const std::vector<std::pair<std::string, std::string>>& EnhancedExperiment::getNotes() const {
    static std::vector<std::pair<std::string, std::string>> allNotes;
    allNotes.clear();
    
    for (const auto& note : pImpl->notes) {
        allNotes.emplace_back(note.category, note.content);
    }
    
    return allNotes;
}

std::string EnhancedExperiment::getResultsSummary() const {
    std::stringstream ss;
    ss << "Experiment: " << pImpl->name << "\n";
    ss << "Description: " << pImpl->description << "\n";
    ss << "State: " << static_cast<int>(pImpl->state) << "\n";
    ss << "Duration: " << getDuration() << " seconds\n";
    ss << "Seed: " << pImpl->seed << "\n";
    ss << "Template: " << pImpl->templateName << "\n";
    ss << "\nMetrics Summary:" << "\n";
    
    std::unordered_map<std::string, std::vector<double>> metricsByName;
    for (const auto& metric : pImpl->metrics) {
        metricsByName[metric.name].push_back(metric.value);
    }
    
    for (const auto& pair : metricsByName) {
        ss << "  " << pair.first << ": mean=" << getMetricMean(pair.first) 
           << ", min=" << getMetricMin(pair.first)
           << ", max=" << getMetricMax(pair.first) << "\n";
    }
    
    ss << "\nSteps recorded: " << pImpl->stepData.size();
    ss << "\nNotes: " << pImpl->notes.size();
    
    return ss.str();
}

std::string EnhancedExperiment::getResultsJSON() const {
    json j;
    
    j["name"] = pImpl->name;
    j["description"] = pImpl->description;
    j["state"] = static_cast<int>(pImpl->state);
    j["startTime"] = std::chrono::system_clock::to_time_t(pImpl->startTime);
    j["endTime"] = std::chrono::system_clock::to_time_t(pImpl->endTime);
    j["duration"] = getDuration();
    j["seed"] = pImpl->seed;
    j["templateName"] = pImpl->templateName;
    
    // Convert metrics to JSON
    json metricsJson = json::array();
    for (const auto& metric : pImpl->metrics) {
        json metricJson;
        metricJson["name"] = metric.name;
        metricJson["value"] = metric.value;
        metricJson["category"] = metric.category;
        metricsJson.push_back(metricJson);
    }
    j["metrics"] = metricsJson;
    
    // Convert step data
    json stepsJson = json::array();
    for (const auto& step : pImpl->stepData) {
        json stepJson;
        stepJson["step"] = step.step;
        stepJson["timestamp"] = std::chrono::system_clock::to_time_t(step.timestamp);
        stepJson["neuronCount"] = step.neuronCount;
        stepJson["synapseCount"] = step.synapseCount;
        stepJson["firingCount"] = step.firingCount;
        stepJson["avgFiringRate"] = step.avgFiringRate;
        stepJson["reward"] = step.reward;
        stepsJson.push_back(stepJson);
    }
    j["stepData"] = stepsJson;
    
    return j.dump(2);
}

std::string EnhancedExperiment::getResultsCSV() const {
    std::stringstream ss;
    
    // Header
    ss << "Step,Timestamp,NeuronCount,SynapseCount,FiringCount,AvgFiringRate,Reward\n";
    
    // Step data
    for (const auto& step : pImpl->stepData) {
        auto time_t = std::chrono::system_clock::to_time_t(step.timestamp);
        ss << step.step << ","
           << std::to_string(time_t) << ","
           << step.neuronCount << ","
           << step.synapseCount << ","
           << step.firingCount << ","
           << step.avgFiringRate << ","
           << step.reward << "\n";
    }
    
    ss << "\n\nMetric Summary\n";
    ss << "Name,Category,Value\n";
    
    // Metrics
    for (const auto& metric : pImpl->metrics) {
        ss << metric.name << ","
           << metric.category << ","
           << metric.value << "\n";
    }
    
    return ss.str();
}

uint64_t EnhancedExperiment::getSeed() const {
    return pImpl->seed;
}

void EnhancedExperiment::setSeed(uint64_t seed) {
    pImpl->seed = seed;
}

bool EnhancedExperiment::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> EnhancedExperiment::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (pImpl->name.empty()) {
        errors.push_back("Experiment name is required");
    }
    
    if (pImpl->state == ExperimentState::UNINITIALIZED) {
        errors.push_back("Experiment must be initialized before validation");
    }
    
    if (pImpl->state == ExperimentState::FAILED) {
        errors.push_back("Experiment has failed");
    }
    
    // Check required metrics
    if (pImpl->metrics.empty()) {
        errors.push_back("No metrics recorded");
    }
    
    // Check for duplicate metric names
    std::unordered_map<std::string, int> metricCounts;
    for (const auto& metric : pImpl->metrics) {
        metricCounts[metric.name]++;
    }
    
    for (const auto& pair : metricCounts) {
        if (pair.second > 1) {
            errors.push_back("Duplicate metric \"" + pair.first + "\" found");
        }
    }
    
    return errors;
}

void EnhancedExperiment::setTemplate(const std::string& templateName) {
    pImpl->templateName = templateName;
}

const std::string& EnhancedExperiment::getTemplate() const {
    return pImpl->templateName;
}

std::vector<std::string> EnhancedExperiment::getAvailableTemplates() {
    return {
        "memory_integration",
        "prediction_system", 
        "cognitive_benchmark",
        "neuromodulation",
        "development",
        "social_learning"
    };
}

} // namespace nlm
