// Implementation of the extended VisualizationInterface
#pragma once

#include "VisualizationInterface.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::shared_ptr<class VisualizationSystem> visualizationSystem;
    std::shared_ptr<class Config> config;
    
    Impl() : active(false), updateRate(30.0), visualizationSystem(nullptr) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(const VisualizationSettings& settings) {
    // Create visualization system
    pImpl->visualizationSystem = std::make_shared<VisualizationSystem>();
    
    if (!pImpl->visualizationSystem->initialize(settings)) {
        pImpl->visualizationSystem.reset();
        return false;
    }
    
    pImpl->active = true;
    return true;
}

bool VisualizationInterface::initialize(std::shared_ptr<class Config> config) {
    pImpl->config = config;
    
    // Create visualization settings from config
    VisualizationSettings settings;
    
    // Get settings from config
    if (auto updateRateOpt = config->get<double>("visualization_update_rate")) {
        settings.networkUpdateRate = *updateRateOpt;
        settings.activityUpdateRate = *updateRateOpt;
    }
    
    if (auto enableRecordingOpt = config->get<bool>("visualization_recording_enabled")) {
        settings.enableRecording = *enableRecordingOpt;
    }
    
    if (auto recordingFormatOpt = config->get<std::string>("visualization_recording_format")) {
        settings.recordingFormat = *recordingFormatOpt;
    }
    
    if (auto recordingIntervalOpt = config->get<size_t>("visualization_recording_interval")) {
        settings.recordingInterval = *recordingIntervalOpt;
    }
    
    if (auto compressOpt = config->get<bool>("visualization_recording_compress")) {
        settings.compressRecording = *compressOpt;
    }
    
    if (auto maxFramesOpt = config->get<size_t>("visualization_recording_max_frames")) {
        settings.maxRecordedFrames = *maxFramesOpt;
    }
    
    if (auto backendOpt = config->get<std::string>("visualization_backend")) {
        settings.backend = *backendOpt;
    }
    
    if (auto headlessOpt = config->get<bool>("visualization_headless")) {
        settings.headless = *headlessOpt;
    }
    
    // Initialize visualization system
    return initialize(settings);
}

void VisualizationInterface::update() {
    if (pImpl->visualizationSystem && pImpl->visualizationSystem->isInitialized()) {
        pImpl->visualizationSystem->update();
    }
}

void VisualizationInterface::render() {
    if (pImpl->visualizationSystem && pImpl->visualizationSystem->isInitialized()) {
        pImpl->visualizationSystem->render();
    }
}

void VisualizationInterface::close() {
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem.reset();
    }
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active && pImpl->visualizationSystem && pImpl->visualizationSystem->isActive();
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->getSettings().networkUpdateRate = hz;
        pImpl->visualizationSystem->getSettings().activityUpdateRate = hz;
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const class Brain& brain) {
    // Delegate to visualization system
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setStepAndTime(0, 0.0);
        // Would update backend with network data
    }
}

void VisualizationInterface::visualizeActivity(const class Brain& brain) {
    // Delegate to visualization system
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setStepAndTime(0, 0.0);
        // Would update backend with activity data
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<class SpikeEvent>& spikes) {
    // Delegate to visualization system
    if (pImpl->visualizationSystem) {
        // Would update backend with spike data
    }
}

void VisualizationInterface::visualizeWeights(const class Brain& brain) {
    // Delegate to visualization system
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setStepAndTime(0, 0.0);
        // Would update backend with weight data
    }
}

void VisualizationInterface::setBrain(std::shared_ptr<class Brain> brain) {
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setBrain(brain);
    }
}

void VisualizationInterface::updateWithBrain(const class Brain& brain, size_t step, double time) {
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setStepAndTime(step, time);
        pImpl->visualizationSystem->updateWithBrain(brain);
    }
}

bool VisualizationInterface::startRecording(const std::string& filepath) {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->startRecording(filepath);
    }
    return false;
}

void VisualizationInterface::stopRecording() {
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->stopRecording();
    }
}

bool VisualizationInterface::isRecording() const {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->isRecording();
    }
    return false;
}

bool VisualizationInterface::loadReplay(const std::string& filepath) {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->loadReplay(filepath);
    }
    return false;
}

void VisualizationInterface::startReplay() {
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->startReplay();
    }
}

void VisualizationInterface::stopReplay() {
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->stopReplay();
    }
}

bool VisualizationInterface::isReplaying() const {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->isReplaying();
    }
    return false;
}

size_t VisualizationInterface::getCurrentReplayFrame() const {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getCurrentReplayFrame();
    }
    return 0;
}

size_t VisualizationInterface::getTotalFrames() const {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getTotalFrames();
    }
    return 0;
}

bool VisualizationInterface::exportData(const std::string& outputPath, const std::string& format) {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->exportData(outputPath, format);
    }
    return false;
}

bool VisualizationInterface::exportVisualizationData(const std::string& outputPath, const std::string& format) {
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->exportVisualizationData(outputPath, format);
    }
    return false;
}

const PerformanceMonitor::Metrics& VisualizationInterface::getMetrics() const {
    static const PerformanceMonitor::Metrics emptyMetrics {
        0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0.0f
    };
    
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getMetrics();
    }
    return emptyMetrics;
}

void VisualizationInterface::setSettings(const VisualizationSettings& settings) {
    if (pImpl->visualizationSystem) {
        pImpl->visualizationSystem->setSettings(settings);
    }
}

const VisualizationSettings& VisualizationInterface::getSettings() const {
    static VisualizationSettings emptySettings;
    if (pImpl->visualizationSystem) {
        return pImpl->visualizationSystem->getSettings();
    }
    return emptySettings;
}

bool VisualizationInterface::isInitialized() const {
    return pImpl->active && pImpl->visualizationSystem && pImpl->visualizationSystem->isInitialized();
}

bool VisualizationInterface::isActive() const {
    return isActive();
}

} // namespace nlm