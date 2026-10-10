#include "NeuralMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <cmath>
#include <iomanip>

namespace nlm {

class NeuralMonitor::Impl {
public:
    Impl() : isMonitoring_(false), totalSpikes_(0), activeNeurons_(0), 
             refractoryNeurons_(0), totalConnections_(0), 
             currentFiringRate_(0.0), networkActivityLevel_(0.0) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Impl() = default;
    
    void startMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isMonitoring_) {
            isMonitoring_ = true;
            startTime_ = std::chrono::high_resolution_clock::now();
            activityHistory_.clear();
        }
    }
    
    void stopMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        isMonitoring_ = false;
    }
    
    bool isMonitoring() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isMonitoring_;
    }
    
    void recordSpike(NeuronId neuronId, Timestamp timestamp, double amplitude) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        NeuralActivity activity;
        activity.neurons = {neuronId};
        activity.activityLevel = amplitude;
        activity.timestamp = timestamp;
        activity.activityType = "spike";
        
        activityHistory_.push_back(activity);
        totalSpikes_++;
        
        // Update neuron states (simplified)
        neuronSpikes_[neuronId]++;
        neuronLastSpike_[neuronId] = timestamp;
        
        // Update network metrics
        updateNetworkMetrics();
    }
    
    void recordSpike(NeuronId neuronId, Timestamp timestamp) {
        recordSpike(neuronId, timestamp, 1.0);
    }
    
    void recordPopulationActivity(const std::vector<NeuronId>& neurons, double activity, const std::string& populationName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        NeuralActivity populationActivity;
        populationActivity.neurons = neurons;
        populationActivity.activityLevel = activity;
        populationActivity.timestamp = getCurrentTimestamp();
        populationActivity.activityType = "population:" + populationName;
        
        activityHistory_.push_back(populationActivity);
        
        // Update population statistics
        for (const auto& neuronId : neurons) {
            populationActivity_[neuronId][populationName] = activity;
        }
        
        // Update network metrics
        updateNetworkMetrics();
    }
    
    void recordPopulationActivity(const std::vector<NeuronId>& neurons, double activity) {
        recordPopulationActivity(neurons, activity, "default");
    }
    
    void recordSynapticActivity(const SynapticActivity& activity) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        synapticActivity_.push_back(activity);
        
        // Update network node connections
        if (networkNodes_.find(activity.preNeuronId) == networkNodes_.end()) {
            NetworkNode node;
            node.neuronId = activity.preNeuronId;
            node.nodeType = "neuron";
            networkNodes_[activity.preNeuronId] = node;
        }
        
        if (networkNodes_.find(activity.postNeuronId) == networkNodes_.end()) {
            NetworkNode node;
            node.neuronId = activity.postNeuronId;
            node.nodeType = "neuron";
            networkNodes_[activity.postNeuronId] = node;
        }
        
        // Update synaptic connections
        if (networkNodes_.find(activity.preNeuronId) != networkNodes_.end()) {
            networkNodes_[activity.preNeuronId].outgoingSynapses.push_back(activity.synapseId);
        }
        
        if (networkNodes_.find(activity.postNeuronId) != networkNodes_.end()) {
            networkNodes_[activity.postNeuronId].incomingSynapses.push_back(activity.synapseId);
        }
        
        totalConnections_++;
        updateNetworkMetrics();
    }
    
    void recordSynapticWeightChange(SynapseId synapseId, NeuronId preNeuronId, 
                                    NeuronId postNeuronId, double weightChange) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        SynapticActivity activity;
        activity.synapseId = synapseId;
        activity.preNeuronId = preNeuronId;
        activity.postNeuronId = postNeuronId;
        activity.weightChange = weightChange;
        activity.timestamp = getCurrentTimestamp();
        activity.efficacy = std::abs(weightChange);
        
        recordSynapticActivity(activity);
    }
    
    void updateNetworkNode(const NetworkNode& node) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (node.nodeType == "neuron") {
            networkNodes_[node.neuronId] = node;
        } else if (node.nodeType == "region") {
            regionNodes_[node.regionId] = node;
        } else if (node.nodeType == "population") {
            populationNodes_[node.populationId] = node;
        }
    }
    
    void addSynapseConnection(SynapseId synapseId, NeuronId preNeuronId, NeuronId postNeuronId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Add connection to pre-synaptic neuron
        if (networkNodes_.find(preNeuronId) != networkNodes_.end()) {
            if (std::find(networkNodes_[preNeuronId].outgoingSynapses.begin(), 
                         networkNodes_[preNeuronId].outgoingSynapses.end(), synapseId) == 
                networkNodes_[preNeuronId].outgoingSynapses.end()) {
                networkNodes_[preNeuronId].outgoingSynapses.push_back(synapseId);
            }
        }
        
        // Add connection to post-synaptic neuron
        if (networkNodes_.find(postNeuronId) != networkNodes_.end()) {
            if (std::find(networkNodes_[postNeuronId].incomingSynapses.begin(), 
                         networkNodes_[postNeuronId].incomingSynapses.end(), synapseId) == 
                networkNodes_[postNeuronId].incomingSynapses.end()) {
                networkNodes_[postNeuronId].incomingSynapses.push_back(synapseId);
            }
        }
        
        totalConnections_++;
        updateNetworkMetrics();
    }
    
    void removeSynapseConnection(SynapseId synapseId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Remove connection from all neurons
        for (auto& pair : networkNodes_) {
            auto& outgoing = pair.second.outgoingSynapses;
            outgoing.erase(std::remove(outgoing.begin(), outgoing.end(), synapseId), outgoing.end());
            
            auto& incoming = pair.second.incomingSynapses;
            incoming.erase(std::remove(incoming.begin(), incoming.end(), synapseId), incoming.end());
        }
        
        totalConnections_ = std::max(0, totalConnections_ - 1);
        updateNetworkMetrics();
    }
    
    size_t getTotalSpikes() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalSpikes_;
    }
    
    size_t getFiringNeurons() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return static_cast<size_t>(neuronSpikes_.size());
    }
    
    double getAverageFiringRate() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (activityHistory_.empty()) return 0.0;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        
        if (duration <= 0.0) return 0.0;
        
        return static_cast<double>(totalSpikes_) / duration;
    }
    
    size_t getActiveNeuronCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return activeNeurons_;
    }
    
    size_t getRefractoryNeuronCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return refractoryNeurons_;
    }
    
    std::vector<NeuralActivity> getRecentActivity(size_t limit) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<NeuralActivity> result;
        size_t startIndex = std::max(0LL, static_cast<long long>(activityHistory_.size() - limit));
        
        for (size_t i = startIndex; i < activityHistory_.size(); ++i) {
            result.push_back(activityHistory_[i]);
        }
        
        return result;
    }
    
    std::vector<NeuralActivity> getActivityByTimeRange(Timestamp startTime, 
                                                         Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<NeuralActivity> result;
        for (const auto& activity : activityHistory_) {
            if (activity.timestamp >= startTime && activity.timestamp <= endTime) {
                result.push_back(activity);
            }
        }
        
        return result;
    }
    
    std::map<std::string, double> analyzeNetworkTopology() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, double> results;
        
        if (networkNodes_.empty()) {
            return results;
        }
        
        // Calculate network metrics
        size_t totalNodes = networkNodes_.size();
        size_t totalEdges = totalConnections_;
        
        // Average degree
        double sumDegree = 0.0;
        for (const auto& pair : networkNodes_) {
            size_t degree = pair.second.outgoingSynapses.size() + pair.second.incomingSynapses.size();
            sumDegree += degree;
        }
        
        double averageDegree = totalNodes > 0 ? sumDegree / totalNodes : 0.0;
        results["average_degree"] = averageDegree;
        
        // Network density (for undirected graph)
        double maxEdges = totalNodes * (totalNodes - 1) / 2.0;
        double density = maxEdges > 0 ? totalEdges / maxEdges : 0.0;
        results["network_density"] = density;
        
        // Clustering coefficient (simplified)
        double clusteringSum = 0.0;
        size_t clusteringCount = 0;
        
        for (const auto& pair : networkNodes_) {
            auto& outgoing = pair.second.outgoingSynapses;
            auto& incoming = pair.second.incomingSynapses;
            
            if (outgoing.size() >= 2) {
                // Simplified clustering coefficient calculation
                clusteringSum += outgoing.size() * (outgoing.size() - 1) / 2.0;
                clusteringCount++;
            }
        }
        
        double clusteringCoefficient = clusteringCount > 0 ? clusteringSum / clusteringCount : 0.0;
        results["clustering_coefficient"] = clusteringCoefficient;
        
        return results;
    }
    
    double calculateNetworkEfficiency() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (networkNodes_.size() < 2) {
            return 0.0;
        }
        
        // Simplified network efficiency calculation
        double totalEfficiency = 0.0;
        size_t nodeCount = 0;
        
        for (const auto& pair1 : networkNodes_) {
            for (const auto& pair2 : networkNodes_) {
                if (pair1.first != pair2.first) {
                    // Calculate shortest path distance (simplified)
                    double distance = calculateDistance(pair1.first, pair2.first);
                    if (distance > 0.0) {
                        totalEfficiency += 1.0 / distance;
                        nodeCount++;
                    }
                }
            }
        }
        
        return nodeCount > 0 ? totalEfficiency / (nodeCount * (nodeCount - 1)) : 0.0;
    }
    
    std::vector<NetworkNode> getHighDegreeNodes(size_t minDegree) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<NetworkNode> result;
        
        for (const auto& pair : networkNodes_) {
            size_t degree = pair.second.outgoingSynapses.size() + pair.second.incomingSynapses.size();
            if (degree >= minDegree) {
                result.push_back(pair.second);
            }
        }
        
        // Sort by degree (descending)
        std::sort(result.begin(), result.end(), 
                  [](const NetworkNode& a, const NetworkNode& b) {
                      size_t degreeA = a.outgoingSynapses.size() + a.incomingSynapses.size();
                      size_t degreeB = b.outgoingSynapses.size() + b.incomingSynapses.size();
                      return degreeA > degreeB;
                  });
        
        return result;
    }
    
    std::map<std::string, double> analyzeSynapticDynamics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, double> results;
        
        if (synapticActivity_.empty()) {
            return results;
        }
        
        // Calculate synaptic dynamics metrics
        double totalWeightChange = 0.0;
        double maxWeightChange = 0.0;
        double averageEfficacy = 0.0;
        
        for (const auto& activity : synapticActivity_) {
            totalWeightChange += std::abs(activity.weightChange);
            maxWeightChange = std::max(maxWeightChange, std::abs(activity.weightChange));
            averageEfficacy += activity.efficacy;
        }
        
        results["total_weight_change"] = totalWeightChange;
        results["max_weight_change"] = maxWeightChange;
        results["average_efficacy"] = averageEfficacy / synapticActivity_.size();
        
        return results;
    }
    
    double getAverageSynapticStrength() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (synapticActivity_.empty()) {
            return 0.0;
        }
        
        double totalStrength = 0.0;
        for (const auto& activity : synapticActivity_) {
            totalStrength += std::abs(activity.weightChange);
        }
        
        return totalStrength / synapticActivity_.size();
    }
    
    size_t getStrongestSynapse() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (synapticActivity_.empty()) {
            return 0;
        }
        
        size_t strongestSynapseId = 0;
        double maxStrength = 0.0;
        
        for (const auto& activity : synapticActivity_) {
            if (activity.efficacy > maxStrength) {
                maxStrength = activity.efficacy;
                strongestSynapseId = activity.synapseId.index();
            }
        }
        
        return strongestSynapseId;
    }
    
    double getCurrentFiringRate() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentFiringRate_;
    }
    
    size_t getTotalConnections() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalConnections_;
    }
    
    double getNetworkActivityLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return networkActivityLevel_;
    }
    
    std::vector<std::vector<NeuronId>> detectSynchronizedGroups(double correlationThreshold) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::vector<NeuronId>> groups;
        
        // Simplified synchronized group detection
        // In a real implementation, this would use proper correlation analysis
        if (activityHistory_.size() < 2) {
            return groups;
        }
        
        // Group neurons by recent activity patterns
        std::map<NeuronId, std::vector<Timestamp>> neuronActivityHistory;
        for (const auto& activity : activityHistory_) {
            for (const auto& neuronId : activity.neurons) {
                neuronActivityHistory[neuronId].push_back(activity.timestamp);
            }
        }
        
        // Find neurons with similar activity patterns (simplified)
        std::vector<NeuronId> processedNeurons;
        
        for (const auto& pair1 : neuronActivityHistory) {
            if (std::find(processedNeurons.begin(), processedNeurons.end(), pair1.first) != 
                processedNeurons.end()) {
                continue;
            }
            
            std::vector<NeuronId> group;
            group.push_back(pair1.first);
            processedNeurons.push_back(pair1.first);
            
            for (const auto& pair2 : neuronActivityHistory) {
                if (pair1.first == pair2.first || 
                    std::find(processedNeurons.begin(), processedNeurons.end(), pair2.first) != 
                    processedNeurons.end()) {
                    continue;
                }
                
                // Simple similarity check (in real implementation, use proper correlation)
                if (haveSimilarActivity(pair1.second, pair2.second, correlationThreshold)) {
                    group.push_back(pair2.first);
                    processedNeurons.push_back(pair2.first);
                }
            }
            
            if (group.size() >= 2) { // Only consider groups with at least 2 neurons
                groups.push_back(group);
            }
        }
        
        return groups;
    }
    
    std::vector<NeuronId> detectBurstNeurons(Timestamp windowDuration) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<NeuronId> burstNeurons;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto windowStart = now - std::chrono::duration<double>(windowDuration);
        
        for (const auto& pair : neuronLastSpike_) {
            auto spikeTime = std::chrono::time_point<std::chrono::high_resolution_clock>(
                std::chrono::duration<double>(pair.second));
            
            if (spikeTime >= windowStart) {
                burstNeurons.push_back(pair.first);
            }
        }
        
        return burstNeurons;
    }
    
    void clearActivityHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        activityHistory_.clear();
        synapticActivity_.clear();
        neuronSpikes_.clear();
        neuronLastSpike_.clear();
        networkNodes_.clear();
        populationActivity_.clear();
        totalSpikes_ = 0;
        activeNeurons_ = 0;
        refractoryNeurons_ = 0;
        totalConnections_ = 0;
        currentFiringRate_ = 0.0;
        networkActivityLevel_ = 0.0;
    }
    
    void setActivityRetentionTime(Timestamp retentionTime) {
        std::lock_guard<std::mutex> lock(mutex_);
        activityRetentionTime_ = retentionTime;
        cleanOldActivity();
    }
    
private:
    Timestamp getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        return duration;
    }
    
    double calculateDistance(NeuronId id1, NeuronId id2) const {
        // Simplified distance calculation
        // In a real implementation, this would use actual network topology
        return std::abs(static_cast<double>(id1.index() - id2.index())) / 1000.0;
    }
    
    bool haveSimilarActivity(const std::vector<Timestamp>& history1, 
                            const std::vector<Timestamp>& history2, 
                            double threshold) const {
        // Simplified similarity check
        // In a real implementation, this would use proper statistical analysis
        if (history1.empty() || history2.empty()) {
            return false;
        }
        
        // Simple check based on activity frequency
        double rate1 = static_cast<double>(history1.size()) / 100.0; // Normalized
        double rate2 = static_cast<double>(history2.size()) / 100.0; // Normalized
        
        double correlation = 1.0 - std::abs(rate1 - rate2);
        return correlation >= threshold;
    }
    
    void updateNetworkMetrics() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        
        if (duration > 0.0) {
            currentFiringRate_ = static_cast<double>(totalSpikes_) / duration;
        }
        
        // Calculate network activity level (simplified)
        networkActivityLevel_ = 0.0;
        if (!activityHistory_.empty()) {
            double totalActivity = 0.0;
            for (const auto& activity : activityHistory_) {
                totalActivity += activity.activityLevel;
            }
            networkActivityLevel_ = totalActivity / activityHistory_.size();
        }
        
        // Update active and refractory neuron counts (simplified)
        activeNeurons_ = std::min(networkNodes_.size(), 1000ULL); // Placeholder
        refractoryNeurons_ = 0; // Placeholder
    }
    
    void cleanOldActivity() {
        auto now = std::chrono::high_resolution_clock::now();
        double currentTime = getCurrentTimestamp();
        
        // Remove old activity from history
        auto it = std::remove_if(activityHistory_.begin(), activityHistory_.end(),
                                 [this, currentTime](const NeuralActivity& activity) {
                                     return (currentTime - activity.timestamp) > activityRetentionTime_;
                                 });
        activityHistory_.erase(it, activityHistory_.end());
    }
    
    mutable std::mutex mutex_;
    bool isMonitoring_;
    std::chrono::high_resolution_clock::time_point startTime_;
    Timestamp activityRetentionTime_ = 3600.0; // 1 hour default
    
    size_t totalSpikes_;
    size_t activeNeurons_;
    size_t refractoryNeurons_;
    size_t totalConnections_;
    double currentFiringRate_;
    double networkActivityLevel_;
    
    std::vector<NeuralActivity> activityHistory_;
    std::vector<SynapticActivity> synapticActivity_;
    
    std::map<NeuronId, size_t> neuronSpikes_;
    std::map<NeuronId, Timestamp> neuronLastSpike_;
    std::map<NeuronId, std::map<std::string, double>> populationActivity_;
    
    std::map<NeuronId, NetworkNode> networkNodes_;
    std::map<RegionId, NetworkNode> regionNodes_;
    std::map<PopulationId, NetworkNode> populationNodes_;
};

NeuralMonitor::NeuralMonitor() : pImpl(new Impl()) {}

NeuralMonitor::~NeuralMonitor() {
    delete pImpl;
}

void NeuralMonitor::startMonitoring() {
    pImpl->startMonitoring();
}

void NeuralMonitor::stopMonitoring() {
    pImpl->stopMonitoring();
}

bool NeuralMonitor::isMonitoring() const {
    return pImpl->isMonitoring();
}

void NeuralMonitor::recordSpike(NeuronId neuronId, Timestamp timestamp, double amplitude) {
    pImpl->recordSpike(neuronId, timestamp, amplitude);
}

void NeuralMonitor::recordSpike(NeuronId neuronId, Timestamp timestamp) {
    pImpl->recordSpike(neuronId, timestamp);
}

void NeuralMonitor::recordPopulationActivity(const std::vector<NeuronId>& neurons, double activity, const std::string& populationName) {
    pImpl->recordPopulationActivity(neurons, activity, populationName);
}

void NeuralMonitor::recordPopulationActivity(const std::vector<NeuronId>& neurons, double activity) {
    pImpl->recordPopulationActivity(neurons, activity);
}

void NeuralMonitor::recordSynapticActivity(const SynapticActivity& activity) {
    pImpl->recordSynapticActivity(activity);
}

void NeuralMonitor::recordSynapticWeightChange(SynapseId synapseId, NeuronId preNeuronId, 
                                                NeuronId postNeuronId, double weightChange) {
    pImpl->recordSynapticWeightChange(synapseId, preNeuronId, postNeuronId, weightChange);
}

void NeuralMonitor::updateNetworkNode(const NetworkNode& node) {
    pImpl->updateNetworkNode(node);
}

void NeuralMonitor::addSynapseConnection(SynapseId synapseId, NeuronId preNeuronId, NeuronId postNeuronId) {
    pImpl->addSynapseConnection(synapseId, preNeuronId, postNeuronId);
}

void NeuralMonitor::removeSynapseConnection(SynapseId synapseId) {
    pImpl->removeSynapseConnection(synapseId);
}

size_t NeuralMonitor::getTotalSpikes() const {
    return pImpl->getTotalSpikes();
}

size_t NeuralMonitor::getFiringNeurons() const {
    return pImpl->getFiringNeurons();
}

double NeuralMonitor::getAverageFiringRate() const {
    return pImpl->getAverageFiringRate();
}

size_t NeuralMonitor::getActiveNeuronCount() const {
    return pImpl->getActiveNeuronCount();
}

size_t NeuralMonitor::getRefractoryNeuronCount() const {
    return pImpl->getRefractoryNeuronCount();
}

std::vector<NeuralActivity> NeuralMonitor::getRecentActivity(size_t limit) const {
    return pImpl->getRecentActivity(limit);
}

std::vector<NeuralActivity> NeuralMonitor::getActivityByTimeRange(Timestamp startTime, 
                                                                     Timestamp endTime) const {
    return pImpl->getActivityByTimeRange(startTime, endTime);
}

std::map<std::string, double> NeuralMonitor::analyzeNetworkTopology() const {
    return pImpl->analyzeNetworkTopology();
}

double NeuralMonitor::calculateNetworkEfficiency() const {
    return pImpl->calculateNetworkEfficiency();
}

std::vector<NetworkNode> NeuralMonitor::getHighDegreeNodes(size_t minDegree) const {
    return pImpl->getHighDegreeNodes(minDegree);
}

std::map<std::string, double> NeuralMonitor::analyzeSynapticDynamics() const {
    return pImpl->analyzeSynapticDynamics();
}

double NeuralMonitor::getAverageSynapticStrength() const {
    return pImpl->getAverageSynapticStrength();
}

size_t NeuralMonitor::getStrongestSynapse() const {
    return pImpl->getStrongestSynapse();
}

double NeuralMonitor::getCurrentFiringRate() const {
    return pImpl->getCurrentFiringRate();
}

size_t NeuralMonitor::getTotalConnections() const {
    return pImpl->getTotalConnections();
}

double NeuralMonitor::getNetworkActivityLevel() const {
    return pImpl->getNetworkActivityLevel();
}

std::vector<std::vector<NeuronId>> NeuralMonitor::detectSynchronizedGroups(double correlationThreshold) const {
    return pImpl->detectSynchronizedGroups(correlationThreshold);
}

std::vector<NeuronId> NeuralMonitor::detectBurstNeurons(Timestamp windowDuration) const {
    return pImpl->detectBurstNeurons(windowDuration);
}

void NeuralMonitor::clearActivityHistory() {
    pImpl->clearActivityHistory();
}

void NeuralMonitor::setActivityRetentionTime(Timestamp retentionTime) {
    pImpl->setActivityRetentionTime(retentionTime);
}

} // namespace nlm