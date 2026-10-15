// PerformanceProfiler.hpp - Implementation of Performance Monitoring

#ifndef NLM_PERFORMANCE_PROFILER_HPP
#define NLM_PERFORMANCE_PROFILER_HPP

#include <chrono>
#include <iostream>
#include <vector>
#include <mutex>

namespace nlm {

class PerformanceProfiler {
public:
    struct ProfilerData {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastFrameTime;
        double totalTime;
        double frameTime;
        size_t frames;
        size_t spikeProcessTime;
        size_t neuronStepTime;
        size_t memoryAllocationTime;
        size_t plasticityUpdateTime;
        
        ProfilerData() : totalTime(0.0), frameTime(0.0), frames(0),
                        spikeProcessTime(0), neuronStepTime(0),
                        memoryAllocationTime(0), plasticityUpdateTime(0) {}
    };
    
    static ProfilerData& getData() {
        static ProfilerData data;
        return data;
    }
    
    static void startFrame() {
        auto& data = getData();
        data.startTime = std::chrono::steady_clock::now();
    }
    
    static void endFrame() {
        auto& data = getData();
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - data.startTime).count();
        data.totalTime += duration;
        data.frameTime = duration;
        data.frames++;
        
        // Log performance every 100 frames
        if (data.frames % 100 == 0) {
            reportPerformance();
        }
    }
    
    static void recordSpikeProcessing(size_t timeNs) {
        getData().spikeProcessTime += timeNs;
    }
    
    static void recordNeuronStepping(size_t timeNs) {
        getData().neuronStepTime += timeNs;
    }
    
    static void recordMemoryAllocation(size_t timeNs) {
        getData().memoryAllocationTime += timeNs;
    }
    
    static void recordPlasticityUpdate(size_t timeNs) {
        getData().plasticityUpdateTime += timeNs;
    }
    
    static void reportPerformance() {
        auto& data = getData();
        if (data.frames == 0) return;
        
        double avgFrameTime = data.totalTime / data.frames;
        double avgSpikeTime = static_cast<double>(data.spikeProcessTime) / data.frames / 1e6;
        double avgNeuronTime = static_cast<double>(data.neuronStepTime) / data.frames / 1e6;
        double avgMemoryTime = static_cast<double>(data.memoryAllocationTime) / data.frames / 1e6;
        double avgPlasticityTime = static_cast<double>(data.plasticityUpdateTime) / data.frames / 1e6;
        
        std::cout << "[PERFORMANCE] "
                  << "Frame: " << avgFrameTime * 1000 << "ms, "
                  << "FPS: " << 1000.0 / avgFrameTime << ", "
                  << "Spike: " << avgSpikeTime << "ms, "
                  << "Neuron: " << avgNeuronTime << "ms, "
                  << "Memory: " << avgMemoryTime << "ms, "
                  << "Plasticity: " << avgPlasticityTime << "ms"
                  << std::endl;
    }
    
    static void printDetailedReport() {
        auto& data = getData();
        if (data.frames == 0) {
            std::cout << "[PERFORMANCE] No performance data available" << std::endl;
            return;
        }
        
        std::cout << "========================================" << std::endl;
        std::cout << "PERFORMANCE DETAILED REPORT" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Total frames processed: " << data.frames << std::endl;
        std::cout << "Average frame time: " << data.totalTime / data.frames * 1000 << "ms" << std::endl;
        std::cout << "Average FPS: " << 1000.0 / (data.totalTime / data.frames) << std::endl;
        std::cout << "Total time: " << data.totalTime << "s" << std::endl;
        std::cout << std::endl;
        std::cout << "Operation Times:" << std::endl;
        std::cout << "  - Spike processing: " << static_cast<double>(data.spikeProcessTime) / data.frames / 1e6 << "ms/frame" << std::endl;
        std::cout << "  - Neuron stepping: " << static_cast<double>(data.neuronStepTime) / data.frames / 1e6 << "ms/frame" << std::endl;
        std::cout << "  - Memory allocation: " << static_cast<double>(data.memoryAllocationTime) / data.frames / 1e6 << "ms/frame" << std::endl;
        std::cout << "  - Plasticity update: " << static_cast<double>(data.plasticityUpdateTime) / data.frames / 1e6 << "ms/frame" << std::endl;
        std::cout << std::endl;
        std::cout << "Performance Summary:" << std::endl;
        double totalOperationTime = static_cast<double>(data.spikeProcessTime + data.neuronStepTime + 
                                                       data.memoryAllocationTime + data.plasticityUpdateTime) / data.frames / 1e6;
        double avgFrameTime = data.totalTime / data.frames;
        double overheadPercent = (totalOperationTime / avgFrameTime) * 100.0;
        std::cout << "  - Total operation overhead: " << overheadPercent << "% of frame time" << std::endl;
        std::cout << "  - Spike processing overhead: " << (static_cast<double>(data.spikeProcessTime) / data.frames / 1e6) / avgFrameTime * 100.0 << "% of frame time" << std::endl;
        std::cout << "  - Neuron stepping overhead: " << (static_cast<double>(data.neuronStepTime) / data.frames / 1e6) / avgFrameTime * 100.0 << "% of frame time" << std::endl;
        std::cout << "  - Memory allocation overhead: " << (static_cast<double>(data.memoryAllocationTime) / data.frames / 1e6) / avgFrameTime * 100.0 << "% of frame time" << std::endl;
        std::cout << "  - Plasticity update overhead: " << (static_cast<double>(data.plasticityUpdateTime) / data.frames / 1e6) / avgFrameTime * 100.0 << "% of frame time" << std::endl;
        std::cout << "========================================" << std::endl;
    }
};

} // namespace nlm

#endif // NLM_PERFORMANCE_PROFILER_HPP
