#include "VisualizationBase.hpp"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

namespace nlm::visualization {

std::vector<std::string> VisualizationBase::getAvailableFormats() {
    return {
        "terminal",
        "opengl",
        "webgl",
        "html",
        "csv",
        "json",
        "png",
        "video"
    };
}

bool VisualizationBase::validateSettings(const VisualizationSettings& settings) {
    bool valid = true;
    
    // Validate output format
    auto formats = getAvailableFormats();
    auto formatStr = std::to_string(static_cast<int>(settings.outputFormat));
    if (std::find(formats.begin(), formats.end(), formatStr) == formats.end()) {
        std::cerr << "Invalid output format: " << formatStr << std::endl;
        valid = false;
    }
    
    // Validate frequency
    if (settings.updateFrequency <= 0.0f) {
        std::cerr << "Update frequency must be positive" << std::endl;
        valid = false;
    }
    
    // Validate dimensions
    if (settings.width <= 0 || settings.height <= 0) {
        std::cerr << "Invalid dimensions" << std::endl;
        valid = false;
    }
    
    // Validate size factors
    if (settings.neuronSize <= 0.0f || settings.synapseThickness <= 0.0f) {
        std::cerr << "Size factors must be positive" << std::endl;
        valid = false;
    }
    
    // Validate speed
    if (settings.animationSpeed <= 0.0f) {
        std::cerr << "Animation speed must be positive" << std::endl;
        valid = false;
    }
    
    // Validate color scheme
    std::vector<std::string> validSchemes = {"default", "scientific", "artistic", "monochrome"};
    if (std::find(validSchemes.begin(), validSchemes.end(), settings.colorScheme) == validSchemes.end()) {
        std::cerr << "Invalid color scheme: " << settings.colorScheme << std::endl;
        valid = false;
    }
    
    // Validate history size
    if (settings.maxHistoryPoints <= 0) {
        std::cerr << "Max history points must be positive" << std::endl;
        valid = false;
    }
    
    return valid;
}

} // namespace nlm::visualization