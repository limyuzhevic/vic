#include "VisualizationFramework.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <ctime>

namespace nlm {

using json = nlohmann::json;

struct JSONVisualizationBackend::Impl {
    bool prettyPrint;
    int indentation;
    std::unordered_map<std::string, std::any> metadata;
    json cachedData;
    
    Impl() : prettyPrint(false), indentation(2) {}
};

JSONVisualizationBackend::JSONVisualizationBackend() : pImpl(std::make_unique<Impl>()) {}
JSONVisualizationBackend::~JSONVisualizationBackend() = default;

bool JSONVisualizationBackend::initialize(const std::unordered_map<std::string, std::any>& config) {
    try {
        if (config.find("pretty_print") != config.end()) {
            pImpl->prettyPrint = std::any_cast<bool>(config.at("pretty_print"));
        }
        if (config.find("indentation") != config.end()) {
            pImpl->indentation = std::any_cast<int>(config.at("indentation"));
        }
        
        pImpl->cachedData = json::object();
        return true;
    } catch (...) {
        return false;
    }
}

void JSONVisualizationBackend::shutdown() {
    pImpl->cachedData.clear();
    pImpl->metadata.clear();
}

bool JSONVisualizationBackend::isActive() const {
    return !pImpl->cachedData.empty();
}

void JSONVisualizationBackend::update(const VisualizationData& data) {
    if (data.format == "json" || data.format.empty()) {
        try {
            pImpl->cachedData = json::parse(data.content);
        } catch (...) {
            pImpl->cachedData["error"] = "Failed to parse JSON data";
            pImpl->cachedData["content"] = data.content;
        }
    }
}

bool JSONVisualizationBackend::render(const std::string& output) {
    try {
        std::string jsonString;
        if (pImpl->prettyPrint) {
            jsonString = pImpl->cachedData.dump(pImpl->indentation);
        } else {
            jsonString = pImpl->cachedData.dump();
        }
        
        if (output.empty()) {
            std::cout << jsonString << std::endl;
        } else {
            std::ofstream file(output);
            if (file.is_open()) {
                file << jsonString << std::endl;
                file.close();
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

const char* JSONVisualizationBackend::getName() const {
    return "JSONVisualizationBackend";
}

std::vector<VisualizationFormat> JSONVisualizationBackend::getSupportedFormats() const {
    return {VisualizationFormat::JSON, VisualizationFormat::HTML, VisualizationFormat::CSV};
}

void JSONVisualizationBackend::addMetadata(const std::string& key, const std::any& value) {
    pImpl->metadata[key] = value;
    pImpl->cachedData["metadata"][key] = value;
}

void JSONVisualizationBackend::setPrettyPrint(bool enable) {
    pImpl->prettyPrint = enable;
}

void JSONVisualizationBackend::setIndentation(int level) {
    pImpl->indentation = level;
}

} // namespace nlm
