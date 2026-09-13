// ============================================================================
// WorkingMemoryVisualizer Implementation
// ============================================================================

#include "WorkingMemoryVisualizer.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct WorkingMemoryVisualizer::Impl {
    float traceDecayRate;
    float interferenceThreshold;
    uint32_t maxActiveTraces;
    bool enableChunking;
    
    Impl() : traceDecayRate(0.95f), interferenceThreshold(0.8f), maxActiveTraces(20),
             enableChunking(true) {}
};

WorkingMemoryVisualizer::WorkingMemoryVisualizer() : pImpl(std::make_unique<Impl>()) {
    name_ = "WorkingMemoryVisualizer";
}

void WorkingMemoryVisualizer::attachToBrain(class Brain* brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    brain_ = brain;
}

void WorkingMemoryVisualizer::attachToMemory(class MemorySystem* memory) {
    std::lock_guard<std::mutex> lock(mutex_);
    memory_ = memory;
}

void WorkingMemoryVisualizer::attachToPlasticity(class PlasticitySystem* plasticity) {
    std::lock_guard<std::mutex> lock(mutex_);
    plasticity_ = plasticity;
}

bool WorkingMemoryVisualizer::initializeImpl(const VisualizationConfig& config) {
    if (auto* vizSettings = config.getSettings()) {
        if (vizSettings->enableLogging) {
            NLM_LOG_INFO("WorkingMemoryVisualizer: Initializing working memory visualization");
        }
        
        // Configure working memory visualization parameters
        pImpl->traceDecayRate = 0.95f;
        pImpl->interferenceThreshold = 0.8f;
        pImpl->maxActiveTraces = 20;
        pImpl->enableChunking = vizSettings->enableAnalysis;
    }
    
    // Initialize trace data
    traceStrengths_.resize(100, 0.0);
    
    NLM_LOG_INFO("WorkingMemoryVisualizer: Working memory visualization ready");
    return true;
}

void WorkingMemoryVisualizer::updateImpl() {
    if (!memory_) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Collect working memory data
    collectWorkingMemoryData();
    
    // Calculate working memory metrics
    calculateWorkingMemoryMetrics();
    
    // Update trace strengths
    updateTraceStrengths();
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Track performance
    double frameTime = duration.count() / 1000000.0;
    pImpl->updateCount++;
    pImpl->totalUpdateTime += frameTime;
    
    if (pImpl->updateCount > 0) {
        fps_ = 1.0 / (pImpl->totalUpdateTime / pImpl->updateCount);
    }
}

void WorkingMemoryVisualizer::renderImpl() {
    switch (getConfiguration().getSettings()->outputFormat) {
        case VisualizationOutputFormat::Terminal:
            renderTerminalView();
            break;
        case VisualizationOutputFormat::OpenGL:
            renderOpenGLView();
            break;
        case VisualizationOutputFormat::WebGL:
            renderWebGLView();
            break;
        default:
            renderBasicView();
            break;
    }
}

void WorkingMemoryVisualizer::closeImpl() {
    NLM_LOG_INFO("WorkingMemoryVisualizer: Shutting down working memory visualization");
    
    traceStrengths_.clear();
    
    NLM_LOG_INFO("WorkingMemoryVisualizer: Working memory visualization stopped");
}

void WorkingMemoryVisualizer::resetImpl() {
    NLM_LOG_INFO("WorkingMemoryVisualizer: Resetting working memory data");
    
    activeTraceCount_ = 0;
    workingMemoryLoad_ = 0.0;
    memorySpan_ = 0.0;
    chunkCount_ = 0;
    
    traceStrengths_.assign(100, 0.0);
}

void WorkingMemoryVisualizer::collectWorkingMemoryData() {
    if (!memory_) return;
    
    // Get working memory system data
    auto* workingMemory = memory_->getWorkingMemory();
    if (!workingMemory) return;
    
    // Collect active traces
    activeTraceCount_ = workingMemory->getActiveTraceCount();
    workingMemoryLoad_ = workingMemory->getMemoryLoad();
    memorySpan_ = workingMemory->getMemorySpan();
    
    // Collect trace strengths
    traceStrengths_.clear();
    auto traces = workingMemory->getActiveTraces();
    for (const auto& trace : traces) {
        traceStrengths_.push_back(trace.strength);
    }
    
    // Collect chunk information
    chunkCount_ = workingMemory->getChunkCount();
}

void WorkingMemoryVisualizer::calculateWorkingMemoryMetrics() {
    // Calculate load based on active traces and their strengths
    if (activeTraceCount_ > 0) {
        double totalStrength = 0.0;
        for (float strength : traceStrengths_) {
            totalStrength += strength;
        }
        workingMemoryLoad_ = totalStrength / activeTraceCount_;
    }
    
    // Calculate memory span based on decay rate
    memorySpan_ = std::log(pImpl->traceDecayRate) * -1.0;
    
    // Calculate chunk efficiency
    if (activeTraceCount_ > 0) {
        chunkCount_ = static_cast<uint64_t>(activeTraceCount_ * 0.3);  // Assume 30% of traces are chunked
    }
}

void WorkingMemoryVisualizer::updateTraceStrengths() {
    // Decay trace strengths
    for (auto& strength : traceStrengths_) {
        strength *= pImpl->traceDecayRate;
    }
}

void WorkingMemoryVisualizer::renderTerminalView() {
    std::cout << "Working Memory Visualization:" << std::endl;
    std::cout << "  Active Traces: " << activeTraceCount_ << "/" << pImpl->maxActiveTraces << std::endl;
    std::cout << "  Memory Load: " << (workingMemoryLoad_ * 100.0) << "%" << std::endl;
    std::cout << "  Memory Span: " << memorySpan_ << " items" << std::endl;
    std::cout << "  Chunks: " << chunkCount_ << std::endl;
    
    if (!traceStrengths_.empty()) {
        std::cout << "  Trace Strengths: [";
        for (size_t i = 0; i < std::min<size_t>(traceStrengths_.size(), 20); ++i) {
            float strength = traceStrengths_[i];
            size_t barPos = static_cast<size_t>(strength * 20);
            std::cout << std::string(barPos, '=');
            if (i < traceStrengths_.size() - 1) std::cout << " ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;
}

void WorkingMemoryVisualizer::renderOpenGLView() {
    // OpenGL rendering for 3D working memory visualization
}

void WorkingMemoryVisualizer::renderWebGLView() {
    // WebGL rendering for browser-based working memory visualization
}

void WorkingMemoryVisualizer::renderBasicView() {
    std::cout << "Working Memory: " << activeTraceCount_ << " traces, " 
              << (workingMemoryLoad_ * 100.0) << "% load" << std::endl;
    std::cout << "Span: " << memorySpan_ << " items, Chunks: " << chunkCount_ << std::endl;
}

} // namespace nlm
