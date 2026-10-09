// BatchProcessor implementation
#include "interfaces/AdvancedInterface.hpp"
#include "experiments/Experiment.hpp"
#include "core/Config/Config.hpp"
#include <thread>
#include <chrono>
#include <sstream>
#include <random>

namespace nlm {

class BatchProcessor::Impl {
public:
    std::unordered_map<std::string, BatchResult> batches;
    std::unordered_map<std::string, std::thread> batchThreads;
    std::unordered_map<std::string, std::atomic<bool>> batchStops;
    std::mutex mutex;
    
    std::default_random_engine rng;
    std::mutex rngMutex;
};

BatchProcessor::BatchProcessor() : pImpl(std::make_unique<Impl>()) {}
BatchProcessor::~BatchProcessor() {
    for (auto& pair : pImpl->batchThreads) {
        if (pair.second.joinable()) {
            pair.second.join();
        }
    }
}

std::string BatchProcessor::startBatch(const BatchConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Generate batch ID
    std::stringstream ss;
    ss << "batch_" << std::chrono::system_clock::now().time_since_epoch().count();
    std::string batchId = ss.str();
    
    // Initialize batch result
    BatchResult result;
    result.batchId = batchId;
    result.status = "running";
    result.startTime = std::chrono::system_clock::now();
    result.totalExperiments = config.experimentFiles.size();
    result.completedExperiments = 0;
    result.failedExperiments = 0;
    
    pImpl->batches[batchId] = result;
    pImpl->batchStops[batchId] = false;
    
    // Start batch processing thread
    pImpl->batchThreads[batchId] = std::thread([this, config, batchId]() {
        processBatch(config, batchId);
    });
    
    return batchId;
}

bool BatchProcessor::stopBatch(const std::string& batchId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->batchStops.find(batchId);
    if (it == pImpl->batchStops.end()) {
        return false;
    }
    
    it->second = true;
    return true;
}

BatchProcessor::BatchResult BatchProcessor::getBatchStatus(const std::string& batchId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->batches.find(batchId);
    if (it == pImpl->batches.end()) {
        BatchResult result;
        result.batchId = batchId;
        result.status = "not_found";
        return result;
    }
    
    return it->second;
}

std::vector<std::string> BatchProcessor::listBatches() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> batchIds;
    for (const auto& pair : pImpl->batches) {
        batchIds.push_back(pair.first);
    }
    
    return batchIds;
}

bool BatchProcessor::cancelBatch(const std::string& batchId) {
    return stopBatch(batchId);
}

bool BatchProcessor::exportBatchResults(const std::string& batchId, const std::string& outputFile) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->batches.find(batchId);
    if (it == pImpl->batches.end()) {
        return false;
    }
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    const auto& result = it->second;
    file << "Batch Results Report" << std::endl;
    file << "====================" << std::endl;
    file << std::endl;
    file << "Batch ID: " << result.batchId << std::endl;
    file << "Status: " << result.status << std::endl;
    file << "Start Time: " << getTimeString(result.startTime) << std::endl;
    if (result.endTime.time_since_epoch().count() > 0) {
        file << "End Time: " << getTimeString(result.endTime) << std::endl;
        file << "Duration: " << getDurationString(result.startTime, result.endTime) << std::endl;
    }
    file << "Total Experiments: " << result.totalExperiments << std::endl;
    file << "Completed: " << result.completedExperiments << std::endl;
    file << "Failed: " << result.failedExperiments << std::endl;
    file << std::endl;
    file << "Experiment Results:" << std::endl;
    for (const auto& pair : result.experimentResults) {
        file << "  - " << pair.first << ": " << pair.second << std::endl;
    }
    
    if (!result.errorMessages.empty()) {
        file << std::endl;
        file << "Error Messages:" << std::endl;
        for (const auto& error : result.errorMessages) {
            file << "  - " << error << std::endl;
        }
    }
    
    return true;
}

std::string BatchProcessor::getBatchResultsSummary(const std::string& batchId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->batches.find(batchId);
    if (it == pImpl->batches.end()) {
        return "Batch not found";
    }
    
    const auto& result = it->second;
    std::stringstream ss;
    ss << "Batch " << batchId << ": " << result.status << " (" 
       << result.completedExperiments << " completed, " << result.failedExperiments << " failed)";
    return ss.str();
}

bool BatchProcessor::isBatchRunning(const std::string& batchId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->batches.find(batchId);
    if (it == pImpl->batches.end()) {
        return false;
    }
    
    return it->second.status == "running";
}

void BatchProcessor::processBatch(const BatchConfig& config, const std::string& batchId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    BatchResult& result = pImpl->batches[batchId];
    
    std::mt19937& rng = pImpl->rng;
    std::uniform_int_distribution<int> dist(1, 100);
    
    for (size_t i = 0; i < config.experimentFiles.size(); ++i) {
        if (pImpl->batchStops[batchId]) {
            result.status = "stopped";
            break;
        }
        
        std::string experimentFile = config.experimentFiles[i];
        std::string experimentId = "exp_" + std::to_string(i);
        
        // Simulate experiment execution
        try {
            // Simulate random success/failure
            int randomValue = dist(rng);
            bool success = (randomValue % 3 != 0); // ~66% success rate
            
            if (success) {
                result.completedExperiments++;
                result.experimentResults[experimentId] = "success";
                
                if (config.saveResults) {
                    // Save results
                }
            } else {
                result.failedExperiments++;
                result.experimentResults[experimentId] = "failed";
                result.errorMessages.push_back("Experiment failed due to random simulation");
            }
            
            // Simulate processing time
            std::this_thread::sleep_for(std::chrono::milliseconds(100 + randomValue * 10));
            
        } catch (const std::exception& e) {
            result.failedExperiments++;
            result.experimentResults[experimentId] = "error";
            result.errorMessages.push_back(std::string("Error: ") + e.what());
        }
    }
    
    result.endTime = std::chrono::system_clock::now();
    result.status = result.failedExperiments > 0 ? "completed_with_errors" : "completed";
    
    if (config.cleanupAfterCompletion) {
        // Cleanup
    }
}

std::string BatchProcessor::getTimeString(std::chrono::system_clock::time_point time) const {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string BatchProcessor::getDurationString(std::chrono::system_clock::time_point start, std::chrono::system_clock::time_point end) const {
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::stringstream ss;
    ss << duration << " seconds";
    return ss.str();
}

} // namespace nlm