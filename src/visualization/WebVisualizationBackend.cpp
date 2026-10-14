#include "VisualizationFramework.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace nlm {

struct WebVisualizationBackend::Impl {
    int port;
    std::string host;
    bool autoOpen;
    std::vector<std::string> javaScript;
    std::vector<std::string> css;
    std::string htmlContent;
    
    Impl() : port(8080), host("localhost"), autoOpen(true) {}
};

WebVisualizationBackend::WebVisualizationBackend() : pImpl(std::make_unique<Impl>()) {}
WebVisualizationBackend::~WebVisualizationBackend() = default;

bool WebVisualizationBackend::initialize(const std::unordered_map<std::string, std::any>& config) {
    try {
        if (config.find("port") != config.end()) {
            pImpl->port = std::any_cast<int>(config.at("port"));
        }
        if (config.find("host") != config.end()) {
            pImpl->host = std::any_cast<std::string>(config.at("host"));
        }
        if (config.find("auto_open") != config.end()) {
            pImpl->autoOpen = std::any_cast<bool>(config.at("auto_open"));
        }
        
        // Build HTML content
        std::stringstream html;
        html << "<!DOCTYPE html>" << std::endl;
        html << "<html>" << std::endl;
        html << "<head>" << std::endl;
        html << "<title>NLM Visualization</title>" << std::endl;
        html << "<style>" << std::endl;
        
        // Add CSS
        for (const auto& style : pImpl->css) {
            html << style << std::endl;
        }
        
        html << "</style>" << std::endl;
        html << "</head>" << std::endl;
        html << "<body>" << std::endl;
        html << "<h1>NLM Neural Network Visualization</h1>" << std::endl;
        html << "<div id=\"visualization\">Loading...</div>" << std::endl;
        html << "<script>" << std::endl;
        
        // Add JavaScript
        for (const auto& script : pImpl->javaScript) {
            html << script << std::endl;
        }
        
        html << "</script>" << std::endl;
        html << "</body>" << std::endl;
        html << "</html>" << std::endl;
        
        pImpl->htmlContent = html.str();
        return true;
    } catch (...) {
        return false;
    }
}

void WebVisualizationBackend::shutdown() {
    pImpl->javaScript.clear();
    pImpl->css.clear();
    pImpl->htmlContent.clear();
}

bool WebVisualizationBackend::isActive() const {
    return !pImpl->htmlContent.empty();
}

void WebVisualizationBackend::update(const VisualizationData& data) {
    // Web backend doesn't need to update data, it's rendered on demand
}

bool WebVisualizationBackend::render(const std::string& output) {
    if (output.empty()) {
        std::cout << pImpl->htmlContent << std::endl;
        return true;
    }
    
    // Write to file
    std::ofstream file(output);
    if (file.is_open()) {
        file << pImpl->htmlContent << std::endl;
        file.close();
        
        if (pImpl->autoOpen) {
            // In a real implementation, this would open the default browser
            std::cout << "Visualization saved to " + output + std::endl;
            std::cout << "Open in browser to view visualization" << std::endl;
        }
        
        return true;
    }
    
    return false;
}

const char* WebVisualizationBackend::getName() const {
    return "WebVisualizationBackend";
}

std::vector<VisualizationFormat> WebVisualizationBackend::getSupportedFormats() const {
    return {VisualizationFormat::HTML, VisualizationFormat::JSON, VisualizationFormat::CSV};
}

void WebVisualizationBackend::setPort(int port) {
    pImpl->port = port;
}

void WebVisualizationBackend::setHost(const std::string& host) {
    pImpl->host = host;
}

void WebVisualizationBackend::setAutoOpen(bool enable) {
    pImpl->autoOpen = enable;
}

void WebVisualizationBackend::addJavaScript(const std::string& js) {
    pImpl->javaScript.push_back(js);
}

void WebVisualizationBackend::addCSS(const std::string& css) {
    pImpl->css.push_back(css);
}

} // namespace nlm
