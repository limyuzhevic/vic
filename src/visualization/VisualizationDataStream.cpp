// ============================================================================
// VisualizationDataStream Implementation
// ============================================================================

#include "VisualizationDataStream.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../plasticity/PlasticitySystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisualizationDataStream::Impl {
    std::queue<VisualizationDataFrame> frameQueue;
    std::queue<VisualizationDataFrame> processedQueue;
    VisualizationDataFrame lastProcessedFrame;
    
    // Data compression
    uint32_t compressionLevel;
    double compressionRatio;
    
    // Timing and performance
    std::chrono::steady_clock::time_point lastFrameTime;
    double frameInterval;
    uint64_t framesProduced;
    uint64_t framesConsumed;
    double totalFrameSize;
    double averageFrameSize;
    
    // Latency tracking
    std::chrono::steady_clock::time_point productionStartTime;
    std::chrono::steady_clock::time_point processingStartTime;
    double accumulatedLatency;
    uint32_t latencySamples;
    
    // Throughput monitoring
    double lastThroughputCheck;
    uint64_t framesSinceLastCheck;
    double throughput;
    
    // Brain integration
    const Brain* attachedBrain;
    bool productionActive;
    
    // Filtering
    std::function<bool(const VisualizationDataFrame&)> filter;
    
    Impl() : compressionLevel(1), compressionRatio(1.0), 
             framesProduced(0), framesConsumed(0), totalFrameSize(0.0),
             averageFrameSize(0.0), attachedBrain(nullptr), productionActive(false),
             lastThroughputCheck(0.0), framesSinceLastCheck(0), throughput(0.0),
             accumulatedLatency(0.0), latencySamples(0) {}
};

VisualizationDataStream::VisualizationDataStream() : pImpl(std::make_unique<Impl>()) {
    pImpl->frameInterval = 1.0 / 60.0;  // Default 60 Hz
    pImpl->lastFrameTime = std::chrono::steady_clock::now();
}

VisualizationDataStream::~VisualizationDataStream() = default;

void VisualizationDataStream::pushFrame(const VisualizationDataFrame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->frameQueue.push(frame);
    
    // Apply compression if needed
    if (pImpl->compressionLevel > 0) {
        // Placeholder for compression logic
        double originalSize = calculateFrameSize(frame);
        pImpl->totalFrameSize += originalSize;
        pImpl->framesProduced++;
        
        double compressedSize = originalSize * (1.0 / (1.0 + pImpl->compressionLevel * 0.1));
        pImpl->compressionRatio = (pImpl->totalFrameSize / pImpl->framesProduced) / compressedSize;
    } else {
        pImpl->totalFrameSize += calculateFrameSize(frame);
        pImpl->framesProduced++;
    }
    
    // Update throughput
    pImpl->framesSinceLastCheck++;
    auto currentTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(currentTime - std::chrono::steady_clock::time_point(std::chrono::microseconds(pImpl->lastFrameTime))).count();
    
    if (elapsed >= 1.0) {
        pImpl->throughput = static_cast<double>(pImpl->framesSinceLastCheck) / elapsed;
        pImpl->framesSinceLastCheck = 0;
        pImpl->lastFrameTime = currentTime;
    }
}

void VisualizationDataStream::startProduction(const class Brain* brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->attachedBrain = brain;
    pImpl->productionActive = true;
    pImpl->productionStartTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("VisualizationDataStream: Production started");
}

void VisualizationDataStream::stopProduction() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pImpl->productionActive) {
        pImpl->productionActive = false;
        pImpl->attachedBrain = nullptr;
        
        // Calculate average latency
        if (pImpl->latencySamples > 0) {
            pImpl->processingLatency = pImpl->accumulatedLatency / pImpl->latencySamples;
        }
        
        NLM_LOG_INFO("VisualizationDataStream: Production stopped");
    }
}

bool VisualizationDataStream::hasData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !pImpl->frameQueue.empty() || !pImpl->processedQueue.empty();
}

bool VisualizationDataStream::getFrame(VisualizationDataFrame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pImpl->processedQueue.empty() && pImpl->frameQueue.empty()) {
        return false;
    }
    
    // Move from raw queue to processed queue
    if (pImpl->processedQueue.empty() && !pImpl->frameQueue.empty()) {
        auto& rawFrame = pImpl->frameQueue.front();
        
        // Apply filter if present
        if (pImpl->filter && !pImpl->filter(rawFrame)) {
            pImpl->frameQueue.pop();
            return getFrame(frame);  // Try next frame
        }
        
        // Process the frame
        VisualizationDataFrame processed = processFrame(rawFrame);
        
        auto processingStart = std::chrono::steady_clock::now();
        pImpl->processedQueue.push(std::move(processed));
        auto processingEnd = std::chrono::steady_clock::now();
        
        // Track latency
        auto latency = std::chrono::duration<double>(processingEnd - processingStart).count() * 1000.0;  // Convert to milliseconds
        pImpl->accumulatedLatency += latency;
        pImpl->latencySamples++;
        
        pImpl->frameQueue.pop();
    }
    
    // Get the processed frame
    frame = std::move(pImpl->processedQueue.front());
    pImpl->processedQueue.pop();
    pImpl->framesConsumed++;
    
    return true;
}

bool VisualizationDataStream::isHealthy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check basic health conditions
    if (pImpl->framesProduced == 0 && pImpl->framesConsumed == 0) {
        return true;  // Nothing produced/consumed yet, but that's OK initially
    }
    
    // Check for data loss
    if (pImpl->framesConsumed > pImpl->framesProduced) {
        return false;
    }
    
    // Check throughput
    if (pImpl->throughput < 1.0) {
        return false;
    }
    
    // Check latency
    if (pImpl->processingLatency > pImpl->maxLatency_) {
        return false;
    }
    
    return true;
}

std::string VisualizationDataStream::getHealthReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "VisualizationDataStream Health Report\n";
    oss << "====================================\n";
    oss << "Frames produced: " << pImpl->framesProduced << "\n";
    oss << "Frames consumed: " << pImpl->framesConsumed << "\n";
    oss << "Frames in buffer: " << pImpl->frameQueue.size() << " (raw), " 
        << pImpl->processedQueue.size() << " (processed)\n";
    oss << "Throughput: " << pImpl->throughput << " frames/sec\n";
    oss << "Average frame size: " << pImpl->averageFrameSize << " bytes\n";
    oss << "Compression ratio: " << pImpl->compressionRatio << ":1\n";
    oss << "Processing latency: " << pImpl->processingLatency << " ms\n";
    oss << "Max allowed latency: " << pImpl->maxLatency_ << " ms\n";
    oss << "Buffer size: " << pImpl->bufferSize_ << " frames\n";
    oss << "Update frequency: " << pImpl->updateFrequency_ << " Hz\n";
    
    if (pImpl->filter) {
        oss << "Filter: Active\n";
    } else {
        oss << "Filter: None\n";
    }
    
    if (pImpl->productionActive) {
        oss << "Production: Active\n";
    } else {
        oss << "Production: Inactive\n";
    }
    
    if (!isHealthy()) {
        oss << "\nWARNING: DataStream is not healthy!\n";
        if (pImpl->framesConsumed > pImpl->framesProduced) {
            oss << "  - Data loss detected\n";
        }
        if (pImpl->throughput < 1.0) {
            oss << "  - Low throughput\n";
        }
        if (pImpl->processingLatency > pImpl->maxLatency_) {
            oss << "  - High latency\n";
        }
    }
    
    return oss.str();
}

double VisualizationDataStream::calculateFrameSize(const VisualizationDataFrame& frame) const {
    double size = 0.0;
    
    // Estimate size based on data content
    size += sizeof(frame.step) + sizeof(frame.timestamp) + sizeof(frame.systemTime);
    size += frame.neuronStates.size() * sizeof(NeuronState);
    size += frame.synapseWeights.size() * sizeof(SynapticWeight);
    size += frame.activeNeurons.size() * sizeof(NeuronId);
    size += frame.activeSynapses.size() * sizeof(std::pair<NeuronId, NeuronId>);
    size += frame.memoryEntries.size() * sizeof(MemoryEntry);
    size += frame.workingMemoryIds.size() * sizeof(MemoryId);
    size += frame.episodicMemoryIds.size() * sizeof(MemoryId);
    size += frame.plasticityEvents.size() * sizeof(PlasticityEvent);
    size += frame.stdpConnections.size() * sizeof(std::pair<NeuronId, NeuronId>);
    size += frame.hebbianConnections.size() * sizeof(NeuronId);
    size += frame.neuromodulatorLevels.size() * sizeof(NeuromodulatorLevel);
    size += frame.dopamineLevels.size() * sizeof(float);
    size += frame.curiosityLevels.size() * sizeof(float);
    size += frame.noveltyLevels.size() * sizeof(float);
    
    return size;
}

VisualizationDataStream::VisualizationDataFrame VisualizationDataStream::processFrame(const VisualizationDataFrame& frame) {
    VisualizationDataFrame processed = frame;
    
    // Apply compression if needed
    if (pImpl->compressionLevel > 0) {
        // Placeholder for actual compression logic
        // For now, just return the frame as-is
    }
    
    return processed;
}

} // namespace nlm
