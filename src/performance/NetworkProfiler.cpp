// NetworkProfiler.cpp
#include "NetworkProfiler.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

NetworkProfiler::NetworkProfiler() : running_(false), stopSampling_(false) {
    prevBytesSent_ = 0;
    prevBytesReceived_ = 0;
    prevTime_ = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

NetworkProfiler::~NetworkProfiler() {
    stopSampling();
}

void NetworkProfiler::start() {
    if (running_) return;
    
    running_ = true;
    stopSampling_ = false;
    
    samplingThread_ = std::thread(&NetworkProfiler::samplingLoop, this);
}

void NetworkProfiler::stop() {
    if (!running_) return;
    
    running_ = false;
    stopSampling_ = true;
    
    if (samplingThread_.joinable()) {
        samplingThread_.join();
    }
}

void NetworkProfiler::samplingLoop() {
    while (!stopSampling_) {
        updateSamples();
        
        std::this_thread::sleep_for(std::chrono::duration<double>(0.1)); // 100ms interval
    }
}

void NetworkProfiler::updateSamples() {
    auto now = std::chrono::high_resolution_clock::now();
    double currentTime = std::chrono::duration<double>(now.time_since_epoch()).count();
    double timeDelta = currentTime - prevTime_;
    
    NetworkSample sample(currentTime);
    
    // Calculate throughput
    if (timeDelta > 0) {
        sample.sendThroughput = double(prevBytesSent_) / timeDelta;
        sample.receiveThroughput = double(prevBytesReceived_) / timeDelta;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.push_back(sample);
    
    // Keep only last hour of samples
    double cutoffTime = currentTime - 3600.0; // 1 hour
    while (!samples_.empty() && samples_.front().timestamp < cutoffTime) {
        samples_.erase(samples_.begin());
    }
    
    // Update previous values
    prevBytesSent_ = 0;
    prevBytesReceived_ = 0;
    prevTime_ = currentTime;
}

void NetworkProfiler::recordNetworkActivity(uint64_t bytesSent, uint64_t bytesReceived,
                                          uint64_t packetsSent, uint64_t packetsReceived) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    prevBytesSent_ += bytesSent;
    prevBytesReceived_ += bytesReceived;
}

double NetworkProfiler::getSendThroughput() const {
    if (samples_.empty()) return 0.0;
    
    double totalBytes = 0;
    double totalTime = 0;
    
    for (size_t i = 1; i < samples_.size(); ++i) {
        const auto& current = samples_[i];
        const auto& previous = samples_[i - 1];
        
        double timeDelta = current.timestamp - previous.timestamp;
        if (timeDelta > 0) {
            totalBytes += current.sendThroughput * timeDelta;
            totalTime += timeDelta;
        }
    }
    
    if (totalTime <= 0) return 0.0;
    
    return totalBytes / totalTime;
}

double NetworkProfiler::getReceiveThroughput() const {
    if (samples_.empty()) return 0.0;
    
    double totalBytes = 0;
    double totalTime = 0;
    
    for (size_t i = 1; i < samples_.size(); ++i) {
        const auto& current = samples_[i];
        const auto& previous = samples_[i - 1];
        
        double timeDelta = current.timestamp - previous.timestamp;
        if (timeDelta > 0) {
            totalBytes += current.receiveThroughput * timeDelta;
            totalTime += timeDelta;
        }
    }
    
    if (totalTime <= 0) return 0.0;
    
    return totalBytes / totalTime;
}

double NetworkProfiler::getPacketLossRate() const {
    if (samples_.empty()) return 0.0;
    
    // Simplified packet loss calculation
    return 0.0; // Would need more detailed tracking
}

std::string NetworkProfiler::generateNetworkReport() const {
    std::stringstream ss;
    ss << "Network Profiler Report\n";
    ss << "========================\n\n";
    
    if (samples_.empty()) {
        ss << "No network samples collected.\n";
        return ss.str();
    }
    
    ss << "Total samples: " << samples_.size() << "\n";
    ss << "Average send throughput: " << getSendThroughput() << " bytes/sec\n";
    ss << "Average receive throughput: " << getReceiveThroughput() << " bytes/sec\n";
    ss << "Packet loss rate: " << getPacketLossRate() * 100 << "%\n\n";
    
    // Recent activity
    ss << "Recent network activity:\n";
    size_t recentCount = std::min(size_t(5), samples_.size());
    for (size_t i = 0; i < recentCount; ++i) {
        size_t idx = samples_.size() - 1 - i;
        const auto& sample = samples_[idx];
        ss << "  " << sample.timestamp << "s: "
           << "send: " << sample.sendThroughput << " bytes/sec, "
           << "receive: " << sample.receiveThroughput << " bytes/sec\n";
    }
    
    return ss.str();
}

bool NetworkProfiler::saveData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    j["samples"] = json::array();
    
    for (const auto& sample : samples_) {
        json jSample;
        jSample["timestamp"] = sample.timestamp;
        jSample["bytesSent"] = sample.bytesSent;
        jSample["bytesReceived"] = sample.bytesReceived;
        jSample["packetsSent"] = sample.packetsSent;
        jSample["packetsReceived"] = sample.packetsReceived;
        jSample["sendThroughput"] = sample.sendThroughput;
        jSample["receiveThroughput"] = sample.receiveThroughput;
        
        j["samples"].push_back(jSample);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool NetworkProfiler::loadData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        return false;
    }
    
    clearSamples();
    
    for (const auto& jSample : j["samples"]) {
        NetworkSample sample(jSample["timestamp"].get<double>());
        sample.bytesSent = jSample["bytesSent"].get<uint64_t>();
        sample.bytesReceived = jSample["bytesReceived"].get<uint64_t>();
        sample.packetsSent = jSample.value("packetsSent", 0ULL);
        sample.packetsReceived = jSample.value("packetsReceived", 0ULL);
        sample.sendThroughput = jSample.value("sendThroughput", 0.0);
        sample.receiveThroughput = jSample.value("receiveThroughput", 0.0);
        
        samples_.push_back(sample);
    }
    
    file.close();
    return true;
}

void NetworkProfiler::clearSamples() {
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.clear();
}

} // namespace nlm
