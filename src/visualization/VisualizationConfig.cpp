// Visualization configuration implementation
#pragma once

#include "VisualizationConfig.hpp"
#include <algorithm>
#include <fstream>

namespace nlm {

bool VisualizationSettings::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON-like configuration format
    file << "{" << std::endl;
    file << "  // Visualization Settings" << std::endl;
    file << "  \"enableNetworkView\": " << (enableNetworkView ? "true" : "false") << "," << std::endl;
    file << "  \"enableActivityView\": " << (enableActivityView ? "true" : "false") << "," << std::endl;
    file << "  \"enableSpikeView\": " << (enableSpikeView ? "true" : "false") << "," << std::endl;
    file << "  \"enableWeightsView\": " << (enableWeightsView ? "true" : "false") << "," << std::endl;
    file << "  \"enableMemoryView\": " << (enableMemoryView ? "true" : "false") << "," << std::endl;
    file << "  \"enableNeuromodulationView\": " << (enableNeuromodulationView ? "true" : "false") << "," << std::endl;
    file << "  \"enableDevelopmentView\": " << (enableDevelopmentView ? "true" : "false") << "," << std::endl;
    file << "  \"networkUpdateRate\": " << networkUpdateRate << "," << std::endl;
    file << "  \"activityUpdateRate\": " << activityUpdateRate << "," << std::endl;
    file << "  \"spikeUpdateRate\": " << spikeUpdateRate << "," << std::endl;
    file << "  \"backgroundColor\": \"" << backgroundColor << "\"," << std::endl;
    file << "  \"neuronColorActive\": \"" << neuronColorActive << "\"," << std::endl;
    file << "  \"neuronColorInactive\": \"" << neuronColorInactive << "\"," << std::endl;
    file << "  \"synapseColorExc\": \"" << synapseColorExc << "\"," << std::endl;
    file << "  \"synapseColorInh\": \"" << synapseColorInh << "," << std::endl;
    file << "  \"enableRecording\": " << (enableRecording ? "true" : "false") << "," << std::endl;
    file << "  \"recordingFormat\": \"" << recordingFormat << "\"," << std::endl;
    file << "  \"recordingInterval\": " << recordingInterval << "," << std::endl;
    file << "  \"compressRecording\": " << (compressRecording ? "true" : "false") << "," << std::endl;
    file << "  \"maxRecordedFrames\": " << maxRecordedFrames << "," << std::endl;
    file << "  \"enablePerformanceMonitoring\": " << (enablePerformanceMonitoring ? "true" : "false") << "," << std::endl;
    file << "  \"performanceMonitorUpdateRate\": " << performanceMonitorUpdateRate << "," << std::endl;
    file << "  \"backend\": \"" << backend << "\"," << std::endl;
    file << "  \"webPort\": \"" << webPort << "\"," << std::endl;
    file << "  \"headless\": " << (headless ? "true" : "false") << std::endl;
    file << "}" << std::endl;
    
    file.close();
    return true;
}

bool VisualizationSettings::loadFromFile(const std::string& filepath) {
    // For now, just use defaults. Could implement JSON parsing here.
    return false;
}

} // namespace nlm