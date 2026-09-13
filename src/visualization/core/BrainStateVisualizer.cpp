// ============================================================================
// BrainStateVisualizer Implementation
// ============================================================================

#include "BrainStateVisualizer.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../plasticity/PlasticitySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct BrainStateVisualizer::Impl {
    std::string visualizationMode;
    float heatmapScale;
    uint32_t spikeHistorySize;
    float activityThreshold;
    bool enableAnimations;
    
    Impl() : visualizationMode("heatmap"), heatmapScale(1.0f), spikeHistorySize(100), 
             activityThreshold(0.5f), enableAnimations(true) {}
};

BrainStateVisualizer::BrainStateVisualizer() : pImpl(std::make_unique<Impl>()) {
    name_ = "BrainStateVisualizer";
}

void BrainStateVisualizer::attachToBrain(class Brain* brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    brain_ = brain;
}

void BrainStateVisualizer::attachToMemory(class MemorySystem* memory) {
    std::lock_guard<std::mutex> lock(mutex_);
    memory_ = memory;
}

void BrainStateVisualizer::attachToPlasticity(class PlasticitySystem* plasticity) {
    std::lock_guard<std::mutex> lock(mutex_);
    plasticity_ = plasticity;
}

bool BrainStateVisualizer::initializeImpl(const VisualizationConfig& config) {
    // Initialize visualization-specific parameters from config
    if (auto* vizSettings = config.getSettings()) {
        if (vizSettings->enableLogging) {
            NLM_LOG_INFO("BrainStateVisualizer: Initializing neural activity monitoring");
        }
        
        // Configure visualization mode
        if (vizSettings->outputFormat == VisualizationOutputFormat::Terminal) {
            pImpl->visualizationMode = "terminal";
        } else if (vizSettings->outputFormat == VisualizationOutputFormat::WebGL) {
            pImpl->visualizationMode = "webgl";
        } else {
            pImpl->visualizationMode = "heatmap";
        }
        
        // Set heatmap scale based on activity range
        pImpl->heatmapScale = vizSettings->targetFPS > 0 ? (60.0f / vizSettings->targetFPS) : 1.0f;
        pImpl->spikeHistorySize = std::min(vizSettings->dataPointsPerFrame, 1000u);
        pImpl->activityThreshold = 0.5f;
        pImpl->enableAnimations = vizSettings->enableAnimations;
    }
    
    // Initialize spike history
    spikeHistory_.resize(pImpl->spikeHistorySize, 0);
    
    NLM_LOG_INFO("BrainStateVisualizer: Neural activity monitoring ready");
    return true;
}

void BrainStateVisualizer::updateImpl() {
    if (!brain_) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Collect neural activity data
    collectNeuralActivityData();
    
    // Calculate activity metrics
    calculateActivityMetrics();
    
    // Update spike history
    if (!spikeHistory_.empty()) {
        spikeHistory_.erase(spikeHistory_.begin());
        spikeHistory_.push_back(brain_->getTotalSpikeCount());
    }
    
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

void BrainStateVisualizer::renderImpl() {
    // Implementation depends on output format
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

void BrainStateVisualizer::closeImpl() {
    NLM_LOG_INFO("BrainStateVisualizer: Shutting down neural activity monitoring");
    
    // Clean up resources
    spikeHistory_.clear();
    
    NLM_LOG_INFO("BrainStateVisualizer: Neural activity monitoring stopped");
}

void BrainStateVisualizer::resetImpl() {
    NLM_LOG_INFO("BrainStateVisualizer: Resetting neural activity data");
    
    // Reset all visualization data
    currentActivity_ = NeuralActivityData();
    activityLevel_ = 0.0;
    totalSpikeCount_ = 0;
    activeRegionCount_ = 0;
    
    // Clear history
    spikeHistory_.assign(pImpl->spikeHistorySize, 0);
}

void BrainStateVisualizer::collectNeuralActivityData() {
    if (!brain_) return;
    
    currentActivity_.timestamp = brain_->getCurrentTime();
    currentActivity_.step = brain_->getCurrentStep();
    
    // Collect spike data
    currentActivity_.totalSpikes = brain_->getTotalSpikeCount();
    currentActivity_.firingNeuronCount = brain_->getFiringNeuronCount();
    
    // Calculate firing rate
    if (currentActivity_.firingNeuronCount > 0) {
        currentActivity_.averageFiringRate = 
            static_cast<double>(currentActivity_.totalSpikes) / 
            static_cast<double>(brain_->getCurrentStep() + 1);
    }
    
    // Collect neuron potentials
    if (auto* region = brain_->getRegion(RegionId(1))) {
        auto neurons = region->getAllNeurons();
        currentActivity_.neuronPotentials.clear();
        currentActivity_.neuronPotentials.reserve(neurons.size());
        
        for (auto* neuron : neurons) {
            currentActivity_.neuronPotentials.push_back(
                neuron->getMembranePotential()
            );
        }
    }
    
    // Collect regional activity
    for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
        auto regionId = RegionId(i + 1);
        if (auto* region = brain_->getRegion(regionId)) {
            currentActivity_.regionalActivity[regionId] = region->getRegionalActivity();
            currentActivity_.regionalSpikeCount[regionId] = region->getTotalSpikes();
        }
    }
    
    // Collect plasticity data
    currentActivity_.stdpEvents = 0;
    currentActivity_.hebbianEvents = 0;
    currentActivity_.structuralPlasticityEvents = 0;
    
    // Collect neuromodulation data
    currentActivity_.dopamineLevel = 0.0;
    currentActivity_.curiosityLevel = 0.0;
    currentActivity_.noveltyLevel = 0.0;
    currentActivity_.predictionError = 0.0;
}

void BrainStateVisualizer::calculateActivityMetrics() {
    if (!brain_) return;
    
    // Calculate overall activity level based on multiple factors
    double spikeActivity = static_cast<double>(currentActivity_.totalSpikes) / 1000.0;
    double firingActivity = currentActivity_.averageFiringRate / 100.0;
    
    // Calculate regional activity variance
    double regionalVariance = 0.0;
    size_t regionCount = currentActivity_.regionalActivity.size();
    if (regionCount > 1) {
        double meanActivity = 0.0;
        for (const auto& pair : currentActivity_.regionalActivity) {
            meanActivity += pair.second;
        }
        meanActivity /= regionCount;
        
        double sumSquaredDiff = 0.0;
        for (const auto& pair : currentActivity_.regionalActivity) {
            double diff = pair.second - meanActivity;
            sumSquaredDiff += diff * diff;
        }
        regionalVariance = sumSquaredDiff / regionCount;
    }
    
    // Combine metrics with weights
    activityLevel_ = spikeActivity * 0.4 + firingActivity * 0.3 + regionalVariance * 0.3;
    
    // Clamp activity level
    activityLevel_ = std::min(std::max(activityLevel_, 0.0), 1.0);
    
    // Calculate other metrics
    totalSpikeCount_ = currentActivity_.totalSpikes;
    activeRegionCount_ = static_cast<uint32_t>(
        std::count_if(currentActivity_.regionalActivity.begin(), 
                     currentActivity_.regionalActivity.end(),
                     [this](const auto& pair) { return pair.second > pImpl->activityThreshold; })
    );
}

void BrainStateVisualizer::renderTerminalView() {
    // Simple text-based visualization for terminal output
    std::cout << "Brain State Visualization:" << std::endl;
    std::cout << "  Step: " << currentActivity_.step << std::endl;
    std::cout << "  Total Spikes: " << currentActivity_.totalSpikes << std::endl;
    std::cout << "  Active Neurons: " << currentActivity_.firingNeuronCount << std::endl;
    std::cout << "  Average Firing Rate: " << currentActivity_.averageFiringRate << " Hz" << std::endl;
    std::cout << "  Activity Level: " << (activityLevel_ * 100.0) << "%" << std::endl;
    std::cout << "  Active Regions: " << activeRegionCount_ << std::endl;
    std::cout << std::endl;
}

void BrainStateVisualizer::renderOpenGLView() {
    // OpenGL rendering would go here
    // This is a placeholder for hardware-accelerated 3D visualization
}

void BrainStateVisualizer::renderWebGLView() {
    // WebGL rendering would go here
    // This is a placeholder for browser-based visualization
}

void BrainStateVisualizer::renderBasicView() {
    // Basic ASCII visualization
    std::cout << "Brain State: Step " << currentActivity_.step << std::endl;
    std::cout << "Spikes: " << currentActivity_.totalSpikes << "  ";
    for (size_t i = 0; i < std::min<size_t>(spikeHistory_.size(), 20); ++i) {
        std::cout << (spikeHistory_[i] > 0 ? "*" : "-");
    }
    std::cout << std::endl;
    std::cout << "Activity: [";
    for (size_t i = 0; i < 20; ++i) {
        size_t idx = spikeHistory_.size() - 1 - i;
        if (idx < spikeHistory_.size()) {
            float normalized = static_cast<float>(spikeHistory_[idx]) / 1000.0f;
            size_t barPos = static_cast<size_t>(normalized * 20);
            std::cout << (i < barPos ? "=" : " ");
        }
    }
    std::cout << "] " << (activityLevel_ * 100.0) << "%" << std::endl;
}

} // namespace nlm
