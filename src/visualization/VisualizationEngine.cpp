#include "VisualizationEngine.hpp"

namespace nlm {

struct VisualizationData::Impl {
    DataType type;
    std::vector<std::pair<double, float>> timeSeriesData;
    std::vector<std::tuple<int64_t, double, float>> spikeData;
    std::vector<std::tuple<int64_t, double, float>> membranePotentialData;
    std::vector<std::tuple<int64_t, int64_t, float, double>> synapticWeightData;
    mutable std::mutex mutex;
    
    Impl() : type(DataType::TimeSeries) {}
};

VisualizationData::VisualizationData() : pImpl(std::make_unique<Impl>()) {}

VisualizationData::~VisualizationData() = default;

VisualizationData::VisualizationData(VisualizationData&&) noexcept = default;
VisualizationData& VisualizationData::operator=(VisualizationData&&) noexcept = default;

void VisualizationData::setType(DataType type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->type = type;
}

VisualizationData::DataType VisualizationData::getType() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->type;
}

template<typename T>
void VisualizationData::addTimeSeriesData(double timestamp, T value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timeSeriesData.emplace_back(timestamp, static_cast<float>(value));
}

void VisualizationData::addSpikeData(int64_t neuronId, double timestamp, float synapticInput) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->spikeData.emplace_back(neuronId, timestamp, synapticInput);
}

void VisualizationData::addMembranePotentialData(int64_t neuronId, double timestamp, float potential) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->membranePotentialData.emplace_back(neuronId, timestamp, potential);
}

void VisualizationData::addSynapticWeightData(int64_t sourceNeuronId, int64_t targetNeuronId,
                                              float weight, double timestamp) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->synapticWeightData.emplace_back(sourceNeuronId, targetNeuronId, weight, timestamp);
}

std::vector<std::pair<double, float>> VisualizationData::getTimeSeriesData() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->timeSeriesData;
}

std::vector<std::tuple<int64_t, double, float>> VisualizationData::getSpikeData() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->spikeData;
}

std::vector<std::tuple<int64_t, double, float>> VisualizationData::getMembranePotentialData() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->membranePotentialData;
}

std::vector<std::tuple<int64_t, int64_t, float, double>> VisualizationData::getSynapticWeightData() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->synapticWeightData;
}

void VisualizationData::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timeSeriesData.clear();
    pImpl->spikeData.clear();
    pImpl->membranePotentialData.clear();
    pImpl->synapticWeightData.clear();
}

size_t VisualizationData::size() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->timeSeriesData.size() + pImpl->spikeData.size() + 
           pImpl->membranePotentialData.size() + pImpl->synapticWeightData.size();
}

bool VisualizationData::empty() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return size() == 0;
}

bool VisualizationData::exportToJSON(const std::string& filepath) const {
    // Implementation would use JSON library to export data
    return false; // Placeholder
}

bool VisualizationData::exportToCSV(const std::string& filepath, const std::string& format) const {
    // Implementation would use CSV library to export data
    return false; // Placeholder
}

VisualizationData::VisualizationData(VisualizationData&&) noexcept = default;
VisualizationData& VisualizationData::operator=(VisualizationData&&) noexcept = default;

// Template instantiation

template void VisualizationData::addTimeSeriesData<double>(double timestamp, double value);
template void VisualizationData::addTimeSeriesData<float>(double timestamp, float value);

struct Visualizer::Impl {
    VisualizationConfig config;
    bool active;
    std::function<void(const std::string&)> interactionCallback;
    double lastRenderTime;
    uint64_t renderCount;
    
    Impl() : active(false), lastRenderTime(0.0), renderCount(0) {}
};

Visualizer::Visualizer() : active_(false), pImpl(std::make_unique<Impl>()) {}

Visualizer::~Visualizer() = default;

Visualizer::Visualizer(Visualizer&&) noexcept = default;
Visualizer& Visualizer::operator=(Visualizer&&) noexcept = default;

bool Visualizer::initialize(const VisualizationConfig& config) {
    config_ = config;
    active_ = true;
    pImpl->active = true;
    pImpl->config = config;
    return true;
}

void Visualizer::update(const VisualizationData& data) {
    // Base implementation - derived classes should override
    pImpl->lastRenderTime = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    pImpl->renderCount++;
}

void Visualizer::render(bool output) {
    if (!active_) return;
    // Base implementation - derived classes should override
    if (output) {
        // Output rendering logic
    }
}

void Visualizer::configure(const VisualizationConfig& config) {
    config_ = config;
    pImpl->config = config;
}

bool Visualizer::isActive() const {
    return active_ && pImpl->active;
}

void Visualizer::setInteractionCallback(std::function<void(const std::string&)> callback) {
    interactionCallback_ = callback;
    pImpl->interactionCallback = callback;
}

std::unordered_map<std::string, double> Visualizer::getPerformanceStats() const {
    std::unordered_map<std::string, double> stats;
    stats["last_render_time"] = pImpl->lastRenderTime;
    stats["render_count"] = static_cast<double>(pImpl->renderCount);
    stats["active"] = active_ ? 1.0 : 0.0;
    return stats;
}

bool Visualizer::exportData(const std::string& format, const std::string& filepath) {
    return false; // Base implementation - derived classes should override
}

// Singleton implementation for VisualizationEngine
namespace {
    std::unique_ptr<VisualizationEngine> globalEngine;
    std::mutex engineMutex;
}

VisualizationEngine& VisualizationEngine::getInstance() {
    std::lock_guard<std::mutex> lock(engineMutex);
    if (!globalEngine) {
        globalEngine = std::unique_ptr<VisualizationEngine>(new VisualizationEngine());
    }
    return *globalEngine;
}

struct VisualizationEngine::Impl {
    VisualizationConfig config;
    std::vector<std::unique_ptr<Visualizer>> visualizers;
    std::unordered_map<std::string, VisualizationType> visualizerTypes;
    std::unordered_map<std::string, bool> visualizerEnabled;
    VisualizationData currentData;
    bool active;
    bool realTimeMonitoring;
    const Brain* monitoringBrain;
    double updateInterval;
    std::thread monitoringThread;
    std::chrono::steady_clock::time_point lastUpdate;
    std::mutex dataMutex;
    double totalUpdateTime;
    uint64_t totalUpdates;
    
    Impl() : active(false), realTimeMonitoring(false), monitoringBrain(nullptr), 
             updateInterval(0.1), totalUpdateTime(0.0), totalUpdates(0) {}
};

VisualizationEngine::VisualizationEngine() : pImpl(std::make_unique<Impl>()) {}

VisualizationEngine::~VisualizationEngine() {
    if (pImpl->realTimeMonitoring) {
        stopRealTimeMonitoring();
    }
}

VisualizationEngine::VisualizationEngine(VisualizationEngine&&) noexcept = default;
VisualizationEngine& VisualizationEngine::operator=(VisualizationEngine&&) noexcept = default;

bool VisualizationEngine::initialize(const VisualizationConfig& config) {
    pImpl->config = config;
    pImpl->active = true;
    
    NLM_LOG_INFO("VisualizationEngine initialized with " + std::to_string(config.updateFrequency) + " Hz update rate");
    return true;
}

void VisualizationEngine::update(const VisualizationData& data) {
    auto startTime = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->currentData.clear();
    pImpl->currentData = data;
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    
    for (auto& visualizer : pImpl->visualizers) {
        if (visualizer->isActive() && pImpl->visualizerEnabled[visualizer->getName()]) {
            visualizer->update(data);
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    pImpl->totalUpdateTime += duration;
    pImpl->totalUpdates++;
}

void VisualizationEngine::render(bool output) {
    for (auto& visualizer : pImpl->visualizers) {
        if (visualizer->isActive() && pImpl->visualizerEnabled[visualizer->getName()]) {
            visualizer->render(output);
        }
    }
}

void VisualizationEngine::configure(const VisualizationConfig& config) {
    pImpl->config = config;
    
    for (auto& visualizer : pImpl->visualizers) {
        if (visualizer->isActive()) {
            visualizer->configure(config);
        }
    }
}

bool VisualizationEngine::registerVisualizer(std::unique_ptr<Visualizer> visualizer, VisualizationType type) {
    if (!visualizer) return false;
    
    std::string name = visualizer->getName();
    
    if (pImpl->visualizerTypes.find(name) != pImpl->visualizerTypes.end()) {
        NLM_LOG_WARNING("Visualizer already registered: " + name);
        return false;
    }
    
    if (!visualizer->initialize(pImpl->config)) {
        NLM_LOG_ERROR("Failed to initialize visualizer: " + name);
        return false;
    }
    
    pImpl->visualizers.push_back(std::move(visualizer));
    pImpl->visualizerTypes[name] = type;
    pImpl->visualizerEnabled[name] = true;
    
    NLM_LOG_INFO("Registered visualizer: " + name + " (type: " + std::to_string(static_cast<int>(type)) + ")");
    return true;
}

bool VisualizationEngine::unregisterVisualizer(const std::string& name) {
    auto it = pImpl->visualizerTypes.find(name);
    if (it == pImpl->visualizerTypes.end()) {
        return false;
    }
    
    for (auto it_viz = pImpl->visualizers.begin(); it_viz != pImpl->visualizers.end(); ++it_viz) {
        if ((*it_viz)->getName() == name) {
            pImpl->visualizers.erase(it_viz);
            pImpl->visualizerTypes.erase(it);
            pImpl->visualizerEnabled.erase(name);
            NLM_LOG_INFO("Unregistered visualizer: " + name);
            return true;
        }
    }
    
    return false;
}

std::vector<Visualizer*> VisualizationEngine::getVisualizers(VisualizationType type) const {
    std::vector<Visualizer*> result;
    
    for (auto& visualizer : pImpl->visualizers) {
        if (visualizer->isActive() && pImpl->visualizerEnabled[visualizer->getName()]) {
            if (type == VisualizationType::Custom || pImpl->visualizerTypes.at(visualizer->getName()) == type) {
                result.push_back(visualizer.get());
            }
        }
    }
    
    return result;
}

bool VisualizationEngine::setVisualizerEnabled(const std::string& name, bool enabled) {
    auto it = pImpl->visualizerTypes.find(name);
    if (it == pImpl->visualizerTypes.end()) {
        return false;
    }
    
    pImpl->visualizerEnabled[name] = enabled;
    return true;
}

VisualizationConfig VisualizationEngine::getConfig() const {
    return pImpl->config;
}

void VisualizationEngine::addBrainData(const Brain& brain, double timestamp) {
    VisualizationData data;
    data.setType(VisualizationData::DataType::TimeSeries);
    
    // Extract neural activity data
    for (const auto& region : brain.getRegions()) {
        for (const auto& neuron : region->getNeurons()) {
            data.addMembranePotentialData(
                neuron->getId(),
                timestamp,
                neuron->getMembranePotential()
            );
        }
    }
    
    // Extract spike data
    for (const auto& spike : brain.getSpikeSystem()->getRecentSpikes()) {
        data.addSpikeData(
            spike->getNeuronId(),
            spike->getTimestamp(),
            spike->getSynapticInput()
        );
    }
    
    update(data);
}

void VisualizationEngine::addSpikeData(const std::vector<SpikeEvent>& spikes) {
    VisualizationData data;
    data.setType(VisualizationData::DataType::SpikeTrains);
    
    for (const auto& spike : spikes) {
        data.addSpikeData(spike.neuronId, spike.timestamp, spike.synapticInput);
    }
    
    update(data);
}

void VisualizationEngine::addPerformanceData(const std::unordered_map<std::string, double>& metrics) {
    VisualizationData data;
    data.setType(VisualizationData::DataType::Performance);
    
    for (const auto& [name, value] : metrics) {
        data.addTimeSeriesData(std::stod(name.substr(6)), value);
    }
    
    update(data);
}

std::unordered_map<std::string, double> VisualizationEngine::getPerformanceStats() const {
    std::unordered_map<std::string, double> stats = pImpl->currentData.getTimeSeriesData().size() > 0 ? 
        getVisualizers(VisualizationType::Performance).size() : 
        std::unordered_map<std::string, double>{};
    
    stats["engine_active"] = pImpl->active ? 1.0 : 0.0;
    stats["total_updates"] = static_cast<double>(pImpl->totalUpdates);
    stats["average_update_time"] = pImpl->totalUpdates > 0 ? pImpl->totalUpdateTime / pImpl->totalUpdates : 0.0;
    stats["visualizers_count"] = static_cast<double>(pImpl->visualizers.size());
    
    return stats;
}

bool VisualizationEngine::exportAllData(const std::string& format, const std::string& filepath) {
    for (auto& visualizer : pImpl->visualizers) {
        if (visualizer->isActive() && pImpl->visualizerEnabled[visualizer->getName()]) {
            if (!visualizer->exportData(format, filepath)) {
                NLM_LOG_ERROR("Failed to export data from visualizer: " + visualizer->getName());
                return false;
            }
        }
    }
    return true;
}

bool VisualizationEngine::startRealTimeMonitoring(const Brain& brain, double updateInterval) {
    if (pImpl->realTimeMonitoring) {
        return false;
    }
    
    pImpl->monitoringBrain = &brain;
    pImpl->updateInterval = updateInterval;
    pImpl->realTimeMonitoring = true;
    
    pImpl->monitoringThread = std::thread([this]() {
        while (pImpl->realTimeMonitoring && pImpl->monitoringBrain) {
            auto startTime = std::chrono::steady_clock::now();
            
            addBrainData(*pImpl->monitoringBrain, 
                        std::chrono::duration<double>(
                            std::chrono::steady_clock::now().time_since_epoch()).count());
            
            render(false); // Render without output during monitoring
            
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            double sleepTime = std::max(0.0, pImpl->updateInterval - duration);
            if (sleepTime > 0.0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    });
    
    NLM_LOG_INFO("Real-time monitoring started with " + std::to_string(updateInterval) + "s interval");
    return true;
}

bool VisualizationEngine::stopRealTimeMonitoring() {
    if (!pImpl->realTimeMonitoring) {
        return false;
    }
    
    pImpl->realTimeMonitoring = false;
    
    if (pImpl->monitoringThread.joinable()) {
        pImpl->monitoringThread.join();
    }
    
    pImpl->monitoringBrain = nullptr;
    
    NLM_LOG_INFO("Real-time monitoring stopped");
    return true;
}

bool VisualizationEngine::isActive() const {
    return pImpl->active;
}

VisualizationInterface& VisualizationEngine::getVisualizationInterface() {
    // This would return a reference to the global visualization interface
    // For now, return a dummy reference
    static VisualizationInterface dummyInterface;
    return dummyInterface;
}

} // namespace nlm