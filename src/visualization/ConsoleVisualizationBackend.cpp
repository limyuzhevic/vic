#include "VisualizationFramework.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <ctime>

namespace nlm {

struct ConsoleVisualizationBackend::Impl {
    size_t width;
    size_t height;
    std::string style;
    bool colorsEnabled;
    double updateInterval;
    double lastUpdateTime;
    std::string lastOutput;
    
    Impl() : width(80), height(24), style("default"), colorsEnabled(true),
             updateInterval(1.0), lastUpdateTime(0.0) {}
};

ConsoleVisualizationBackend::ConsoleVisualizationBackend() : pImpl(std::make_unique<Impl>()) {}
ConsoleVisualizationBackend::~ConsoleVisualizationBackend() = default;

bool ConsoleVisualizationBackend::initialize(const std::unordered_map<std::string, std::any>& config) {
    try {
        if (config.find("width") != config.end()) {
            pImpl->width = std::any_cast<size_t>(config.at("width"));
        }
        if (config.find("height") != config.end()) {
            pImpl->height = std::any_cast<size_t>(config.at("height"));
        }
        if (config.find("style") != config.end()) {
            pImpl->style = std::any_cast<std::string>(config.at("style"));
        }
        if (config.find("colors") != config.end()) {
            pImpl->colorsEnabled = std::any_cast<bool>(config.at("colors"));
        }
        if (config.find("update_interval") != config.end()) {
            pImpl->updateInterval = std::any_cast<double>(config.at("update_interval"));
        }
        
        pImpl->lastUpdateTime = 0.0;
        return true;
    } catch (...) {
        return false;
    }
}

void ConsoleVisualizationBackend::shutdown() {
    pImpl->lastOutput.clear();
}

bool ConsoleVisualizationBackend::isActive() const {
    return !pImpl->lastOutput.empty();
}

void ConsoleVisualizationBackend::update(const VisualizationData& data) {
    // Console backend doesn't need to update data, it's rendered on demand
}

bool ConsoleVisualizationBackend::render(const std::string& output) {
    if (output.empty()) {
        std::cout << pImpl->lastOutput << std::endl;
    } else {
        std::ofstream file(output);
        if (file.is_open()) {
            file << pImpl->lastOutput << std::endl;
            file.close();
            return true;
        }
    }
    return true;
}

const char* ConsoleVisualizationBackend::getName() const {
    return "ConsoleVisualizationBackend";
}

std::vector<VisualizationFormat> ConsoleVisualizationBackend::getSupportedFormats() const {
    return {VisualizationFormat::Console, VisualizationFormat::HTML, VisualizationFormat::JSON};
}

void ConsoleVisualizationBackend::setConsoleSize(size_t width, size_t height) {
    pImpl->width = width;
    pImpl->height = height;
}

void ConsoleVisualizationBackend::setStyle(const std::string& style) {
    pImpl->style = style;
}

void ConsoleVisualizationBackend::setColors(bool enable) {
    pImpl->colorsEnabled = enable;
}

void ConsoleVisualizationBackend::setUpdateInterval(double interval) {
    pImpl->updateInterval = interval;
}

} // namespace nlm
