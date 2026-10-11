// AnimationGenerator.cpp - Animation generation implementation

#include "AnimationGenerator.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "SpikeEvent.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <queue>

namespace nlm {

struct AnimationGenerator::Impl {
    AnimationConfig config;
    AnimationStats stats;
    bool active;
    bool complete;
    AnimationCallback callback;
    std::vector<FrameRenderer> frameRenderers;
    std::function<std::vector<std::vector<double>>(double time)> dataSource;
    std::unique_ptr<Brain> brainData;
    std::vector<AnimationFrame> frames;
    std::chrono::steady_clock::time_point startTime;
    std::atomic<double> progress;
    std::string currentStatus;
    std::mutex mutex;
    
    Impl() : active(false), complete(false), progress(0.0), currentStatus("Initialized") {
    }
};

AnimationGenerator::AnimationGenerator() : pImpl(std::make_unique<Impl>()) {
}

AnimationGenerator::AnimationGenerator(const AnimationConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

AnimationGenerator::~AnimationGenerator() {
    stop();
}

void AnimationGenerator::configure(const AnimationConfig& config) {
    pImpl->config = config;
    // Clear existing data
    pImpl->frames.clear();
    pImpl->progress = 0.0;
    pImpl->currentStatus = "Configured";
}

bool AnimationGenerator::initialize(const Brain& brain) {
    pImpl->brainData = std::make_unique<Brain>(brain);
    pImpl->frames.clear();
    pImpl->progress = 0.0;
    pImpl->complete = false;
    pImpl->active = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->currentStatus = "Initialized";
    
    // Initialize statistics
    pImpl->stats.totalFrames = 0;
    pImpl->stats.totalDuration = pImpl->config.duration;
    pImpl->stats.fileSizeMB = 0.0;
    pImpl->stats.averageFrameTime = 0.0;
    pImpl->stats.maxFrameTime = 0.0;
    pImpl->stats.minFrameTime = 0.0;
    pImpl->stats.generationTime = 0.0;
    pImpl->stats.fps = static_cast<int>(pImpl->config.frameRate);
    pImpl->stats.completed = false;
    
    NLM_LOG_INFO("AnimationGenerator initialized: " + std::to_string(pImpl->config.width) + "x" + std::to_string(pImpl->config.height) + 
                " @ " + std::to_string(pImpl->config.frameRate) + " fps, " + std::to_string(pImpl->config.duration) + "s duration");
    return true;
}

bool AnimationGenerator::generate(const Brain& brain, const std::vector<SpikeEvent>& spikes, const AnimationCallback& callback) {
    if (!pImpl->active) {
        NLM_LOG_ERROR("AnimationGenerator not active");
        return false;
    }
    
    pImpl->callback = callback;
    pImpl->brainData = std::make_unique<Brain>(brain);
    
    // Clear existing data
    pImpl->frames.clear();
    pImpl->progress = 0.0;
    
    int totalFrames = static_cast<int>(pImpl->config.frameRate * pImpl->config.duration);
    pImpl->stats.totalFrames = totalFrames;
    pImpl->stats.generationTime = 0.0;
    
    NLM_LOG_INFO("Starting animation generation: " + std::to_string(totalFrames) + " frames");
    
    // Generate frames
    for (int frameIndex = 0; frameIndex < totalFrames && pImpl->active; ++frameIndex) {
        double time = (frameIndex / pImpl->config.frameRate);
        
        // Update progress
        pImpl->progress = (static_cast<double>(frameIndex) / totalFrames) * 100.0;
        pImpl->currentStatus = "Rendering frame " + std::to_string(frameIndex + 1) + "/" + std::to_string(totalFrames);
        
        if (pImpl->callback) {
            pImpl->callback(pImpl->progress, pImpl->currentStatus);
        }
        
        // Render frame
        std::vector<uint8_t> frameData;
        auto frameStart = std::chrono::steady_clock::now();
        
        if (!pImpl->frameRenderers.empty()) {
            // Use first available frame renderer
            frameData = pImpl->frameRenderers[0](frameIndex, time);
        } else {
            // Generate default frame
            frameData = generateDefaultFrame(time);
        }
        
        // Calculate frame time
        auto frameEnd = std::chrono::steady_clock::now();
        double frameTime = std::chrono::duration<double>(frameEnd - frameStart).count();
        
        // Update statistics
        pImpl->stats.totalFrames++;
        pImpl->stats.generationTime += frameTime;
        
        if (frameTime > pImpl->stats.maxFrameTime) {
            pImpl->stats.maxFrameTime = frameTime;
        }
        if (frameTime < pImpl->stats.minFrameTime || pImpl->stats.minFrameTime == 0.0) {
            pImpl->stats.minFrameTime = frameTime;
        }
        
        // Create frame
        AnimationFrame frame;
        frame.frameIndex = frameIndex;
        frame.timeStamp = time;
        frame.frameData = std::move(frameData);
        frame.format = "PNG";
        frame.sizeBytes = frame.frameData.size();
        frame.captured = std::chrono::system_clock::now();
        frame.metadata["frameIndex"] = std::to_string(frameIndex);
        frame.metadata["time"] = std::to_string(time);
        
        pImpl->frames.push_back(std::move(frame));
        
        // Small delay to maintain frame rate (in real implementation)
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000 / pImpl->config.frameRate));
    }
    
    // Animation generation complete
    pImpl->complete = true;
    pImpl->active = false;
    pImpl->progress = 100.0;
    pImpl->currentStatus = "Animation generation completed";
    
    if (pImpl->callback) {
        pImpl->callback(100.0, pImpl->currentStatus);
    }
    
    // Calculate final statistics
    if (pImpl->stats.totalFrames > 0) {
        pImpl->stats.averageFrameTime = pImpl->stats.generationTime / pImpl->stats.totalFrames;
        pImpl->stats.fileSizeMB = calculateTotalFileSize() / (1024.0 * 1024.0);
        pImpl->stats.fps = static_cast<int>(pImpl->config.frameRate);
    }
    
    NLM_LOG_INFO("Animation generation completed: " + std::to_string(pImpl->frames.size()) + " frames generated, " + 
                std::to_string(pImpl->stats.fileSizeMB) + " MB total size");
    
    return true;
}

void AnimationGenerator::addFrameRenderer(const FrameRenderer& renderer) {
    pImpl->frameRenderers.push_back(renderer);
}

void AnimationGenerator::setDataSource(std::function<std::vector<std::vector<double>>(double time)> dataSource) {
    pImpl->dataSource = dataSource;
}

AnimationGenerator::AnimationStats AnimationGenerator::getStats() const {
    return pImpl->stats;
}

bool AnimationGenerator::isComplete() const {
    return pImpl->complete;
}

std::vector<AnimationGenerator::AnimationFrame> AnimationGenerator::getFrames() const {
    return pImpl->frames;
}

bool AnimationGenerator::exportFrames(const std::string& directory) const {
    if (!pImpl->complete) {
        NLM_LOG_ERROR("Animation generation not complete, cannot export");
        return false;
    }
    
    std::error_code ec;
    std::filesystem::create_directories(directory, ec);
    if (ec) {
        NLM_LOG_ERROR("Failed to create output directory: " + ec.message());
        return false;
    }
    
    NLM_LOG_INFO("Exporting " + std::to_string(pImpl->frames.size()) + " frames to " + directory);
    
    for (const auto& frame : pImpl->frames) {
        std::string filename = directory + "/frame_" + std::to_string(frame.frameIndex) + ".png";
        
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filename);
            return false;
        }
        
        // Write frame data
        file.write(reinterpret_cast<const char*>(frame.frameData.data()), frame.frameData.size());
        file.close();
    }
    
    NLM_LOG_INFO("Frames exported successfully");
    return true;
}

bool AnimationGenerator::compileAnimation(const std::string& outputPath) const {
    if (!pImpl->complete) {
        NLM_LOG_ERROR("Animation generation not complete, cannot compile");
        return false;
    }
    
    NLM_LOG_INFO("Compiling animation to " + outputPath);
    
    // In real implementation, this would use FFmpeg or similar tools
    // For now, just log a placeholder message
    NLM_LOG_WARNING("Animation compilation not yet implemented");
    
    return true;
}

void AnimationGenerator::stop() {
    pImpl->active = false;
    pImpl->currentStatus = "Stopped";
    NLM_LOG_INFO("Animation generation stopped");
}

// Private helper methods
std::vector<uint8_t> AnimationGenerator::generateDefaultFrame(double time) const {
    // Generate a simple placeholder frame
    // In real implementation, this would use actual visualization rendering
    
    // Simple gradient background
    int pixelCount = pImpl->config.width * pImpl->config.height;
    std::vector<uint8_t> frameData(pixelCount * 3); // RGB
    
    double intensity = 0.5 + 0.5 * std::sin(time * 2.0);
    
    for (int i = 0; i < pixelCount; ++i) {
        frameData[i * 3] = static_cast<uint8_t>(intensity * 255);     // Red
        frameData[i * 3 + 1] = static_cast<uint8_t>(0.3 * 255);     // Green
        frameData[i * 3 + 2] = static_cast<uint8_t>(intensity * 200); // Blue
    }
    
    // Add time text (simplified)
    std::string timeText = "Time: " + std::to_string(time) + "s";
    
    return frameData;
}

double AnimationGenerator::calculateTotalFileSize() const {
    double totalSize = 0.0;
    for (const auto& frame : pImpl->frames) {
        totalSize += frame.sizeBytes;
    }
    return totalSize;
}

} // namespace nlm
