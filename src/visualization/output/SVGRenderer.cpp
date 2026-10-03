#include "SVGRenderer.hpp"

namespace nlm {

struct SVGRenderer::Impl {
    int width;
    int height;
    float viewBoxMinX;
    float viewBoxMinY;
    float viewBoxWidth;
    float viewBoxHeight;
    std::array<float, 4> backgroundColor;
    std::string documentTitle;
    std::string description;
    bool compressionEnabled;
    int compressionLevel;
    
    Impl() : width(1024), height(768), viewBoxMinX(0.0f), viewBoxMinY(0.0f), 
             viewBoxWidth(1024.0f), viewBoxHeight(768.0f), backgroundColor({1.0f, 1.0f, 1.0f, 1.0f}),
             compressionEnabled(false), compressionLevel(6) {}
};

SVGRenderer::SVGRenderer() : pImpl(std::make_unique<Impl>()) {}
SVGRenderer::~SVGRenderer() = default;

bool SVGRenderer::initialize() {
    // Initialize SVG rendering
    return true;
}

void SVGRenderer::shutdown() {
    // Shutdown SVG rendering
}

void SVGRenderer::update(double deltaTime) {
    // Update SVG rendering
}

bool SVGRenderer::render(const std::string& filepath, const std::map<std::string, VisualizationData>& data) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    writeSVGHeader(file);
    
    // Render different data types
    if (data.find("network") != data.end()) {
        renderNetworkToSVG(file, data.at("network"));
    }
    
    if (data.find("activity") != data.end()) {
        renderActivityToSVG(file, data.at("activity"));
    }
    
    if (data.find("spikes") != data.end()) {
        renderSpikesToSVG(file, data.at("spikes"));
    }
    
    if (data.find("weights") != data.end()) {
        renderWeightsToSVG(file, data.at("weights"));
    }
    
    if (data.find("timeSeries") != data.end()) {
        renderTimeSeriesToSVG(file, data.at("timeSeries"));
    }
    
    if (data.find("memory") != data.end()) {
        renderMemoryToSVG(file, data.at("memory"));
    }
    
    if (data.find("prediction") != data.end()) {
        renderPredictionToSVG(file, data.at("prediction"));
    }
    
    writeSVGFooter(file);
    
    return true;
}

void SVGRenderer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

int SVGRenderer::getWidth() const {
    return pImpl->width;
}

int SVGRenderer::getHeight() const {
    return pImpl->height;
}

void SVGRenderer::setViewBox(float minX, float minY, float width, float height) {
    pImpl->viewBoxMinX = minX;
    pImpl->viewBoxMinY = minY;
    pImpl->viewBoxWidth = width;
    pImpl->viewBoxHeight = height;
}

void SVGRenderer::setBackgroundColor(float r, float g, float b, float a = 1.0f) {
    pImpl->backgroundColor = {r, g, b, a};
}

std::array<float, 4> SVGRenderer::getBackgroundColor() const {
    return pImpl->backgroundColor;
}

void SVGRenderer::setDocumentTitle(const std::string& title) {
    pImpl->documentTitle = title;
}

const std::string& SVGRenderer::getDocumentTitle() const {
    return pImpl->documentTitle;
}

void SVGRenderer::setDescription(const std::string& description) {
    pImpl->description = description;
}

const std::string& SVGRenderer::getDescription() const {
    return pImpl->description;
}

void SVGRenderer::enableCompression(bool enabled) {
    pImpl->compressionEnabled = enabled;
}

bool SVGRenderer::isCompressionEnabled() const {
    return pImpl->compressionEnabled;
}

void SVGRenderer::setCompressionLevel(int level) {
    pImpl->compressionLevel = level;
}

int SVGRenderer::getCompressionLevel() const {
    return pImpl->compressionLevel;
}

void SVGRenderer::writeSVGHeader(std::ofstream& svg) {
    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" " << std::endl;
    svg << "     xmlns:xlink=\"http://www.w3.org/1999/xlink\" " << std::endl;
    svg << "     width=\"" << pImpl->width << "px\" height=\"" << pImpl->height << "px\" " << std::endl;
    svg << "     viewBox=\"" << pImpl->viewBoxMinX << " " << pImpl->viewBoxMinY << " " << pImpl->viewBoxWidth << " " << pImpl->viewBoxHeight << "\" " << std::endl;
    svg << "     version=\"1.1\" baseProfile=\"full\">" << std::endl;
    
    if (!pImpl->documentTitle.empty()) {
        svg << "<title>" << pImpl->documentTitle << "</title>" << std::endl;
    }
    
    if (!pImpl->description.empty()) {
        svg << "<desc>" << pImpl->description << "</desc>" << std::endl;
    }
    
    svg << "<rect width=\"100%\" height=\"100%\" fill=\"rgb(" << static_cast<int>(pImpl->backgroundColor[0] * 255) << ",";
    svg << static_cast<int>(pImpl->backgroundColor[1] * 255) << ",";
    svg << static_cast<int>(pImpl->backgroundColor[2] * 255) << ",";
    svg << static_cast<int>(pImpl->backgroundColor[3] * 255) << ")\"/>" << std::endl;
}

void SVGRenderer::writeSVGFooter(std::ofstream& svg) {
    svg << "</svg>" << std::endl;
}

void SVGRenderer::writeSVGG(std::ofstream& svg, float x, float y, float width, float height) {
    svg << "<g transform=\"translate(" << x << "," << y << ")\">" << std::endl;
}

void SVGRenderer::writeSVGCircle(std::ofstream& svg, float cx, float cy, float r, const std::string& fill, float opacity) {
    svg << "<circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << r << "\" fill=\"" << fill << "\" opacity=\"" << opacity << "\"/>" << std::endl;
}

void SVGRenderer::writeSVGRect(std::ofstream& svg, float x, float y, float width, float height, const std::string& fill, float opacity) {
    svg << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\"" << height << "\" fill=\"" << fill << "\" opacity=\"" << opacity << "\"/>" << std::endl;
}

void SVGRenderer::writeSVGLine(std::ofstream& svg, float x1, float y1, float x2, float y2, const std::string& stroke, float strokeWidth) {
    svg << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\" stroke=\"" << stroke << "\" stroke-width=\"" << strokeWidth << "\"/>" << std::endl;
}

void SVGRenderer::writeSVGText(std::ofstream& svg, float x, float y, const std::string& text, const std::string& fontFamily, float fontSize, const std::string& fill) {
    svg << "<text x=\"" << x << "\" y=\"" << y << "\" font-family=\"" << fontFamily << "\" font-size=\"" << fontSize << "\" fill=\"" << fill << "\">" << text << "</text>" << std::endl;
}

void SVGRenderer::renderNetworkToSVG(std::ofstream& svg, const VisualizationData::Network& network) {
    // Render network nodes
    writeSVGG(svg, 0, 0, 1, 1);
    
    for (size_t i = 0; i < network.neurons.size(); ++i) {
        float x = static_cast<float>(i % 10) * 50.0f + 50.0f;
        float y = static_cast<float>(i / 10) * 50.0f + 50.0f;
        
        std::string color = network.neuronTypes[i] == NeuronType::Excitatory ? "red" : "blue";
        writeSVGCircle(svg, x, y, 5.0f, color, 0.8f);
        
        if (!network.neuronLabels.empty() && network.neuronLabels.find(network.neurons[i]) != network.neuronLabels.end()) {
            writeSVGText(svg, x + 10, y, network.neuronLabels.at(network.neurons[i]), "Arial", 10.0f, "black");
        }
    }
    
    // Render connections
    for (const auto& connection : network.connections) {
        float x1 = static_cast<float>(connection.first % 10) * 50.0f + 50.0f;
        float y1 = static_cast<float>(connection.first / 10) * 50.0f + 50.0f;
        float x2 = static_cast<float>(connection.second % 10) * 50.0f + 50.0f;
        float y2 = static_cast<float>(connection.second / 10) * 50.0f + 50.0f;
        
        std::string strokeColor = "gray";
        writeSVGLine(svg, x1, y1, x2, y2, strokeColor, 1.0f);
    }
    
    svg << "</g>" << std::endl;
}

void SVGRenderer::renderActivityToSVG(std::ofstream& svg, const VisualizationData::Activity& activity) {
    writeSVGG(svg, 200, 0, 1, 1);
    
    // Render heatmap
    for (size_t i = 0; i < activity.heatmaps.size(); ++i) {
        for (size_t j = 0; j < activity.heatmaps[i].size(); ++j) {
            float x = static_cast<float>(j) * 2.0f + 50.0f;
            float y = static_cast<float>(i) * 2.0f + 50.0f;
            
            float value = activity.heatmaps[i][j];
            std::string color = getColorFromValue(value);
            writeSVGRect(svg, x, y, 2.0f, 2.0f, color, value);
        }
    }
    
    svg << "</g>" << std::endl;
}

void SVGRenderer::renderSpikesToSVG(std::ofstream& svg, const VisualizationData::Spikes& spikes) {
    writeSVGG(svg, 400, 0, 1, 1);
    
    // Render spike raster plot
    for (size_t i = 0; i < spikes.neuronSpikes.size(); ++i) {
        for (const auto& timestamp : spikes.neuronSpikes[i]) {
            float y = static_cast<float>(i) * 2.0f + 50.0f;
            float x = static_cast<float>(timestamp / 1000.0) * 100.0f + 50.0f;  // Convert to milliseconds
            
            writeSVGCircle(svg, x, y, 1.0f, "black", 0.5f);
        }
    }
    
    svg << "</g>" << std::endl;
}

void SVGRenderer::renderWeightsToSVG(std::ofstream& svg, const VisualizationData::Weights& weights) {
    writeSVGG(svg, 600, 0, 1, 1);
    
    // Render weight matrix
    for (size_t i = 0; i < weights.weightMatrix.size(); ++i) {
        const auto& weight = weights.weightMatrix[i];
        float x = static_cast<float>(std::get<0>(weight) % 10) * 20.0f + 50.0f;
        float y = static_cast<float>(std::get<0>(weight) / 10) * 20.0f + 50.0f;
        
        float weightValue = std::get<2>(weight);
        std::string color = getColorFromWeight(weightValue);
        writeSVGRect(svg, x, y, 15.0f, 15.0f, color, 0.8f);
    }
    
    svg << "</g>" << std::endl;
}

void SVGRenderer::render3DViewToSVG(std::ofstream& svg, const VisualizationData& data) {
    writeSVGG(svg, 0, 300, 1, 1);
    
    // Render 3D view (simplified)
    for (size_t i = 0; i < data.network.neurons.size(); ++i) {
        float x = static_cast<float>(i % 10) * 30.0f + 50.0f;
        float z = static_cast<float>(i / 10) * 30.0f + 50.0f;
        float y = 50.0f + sin(static_cast<float>(data.simulation.currentTime * 0.01) * i) * 20.0f;
        
        std::string color = network.neuronTypes[i] == NeuronType::Excitatory ? "red" : "blue";
        writeSVGCircle(svg, x, y, 5.0f, color, 0.8f);
        
        if (i < data.network.connections.size()) {
            float x2 = static_cast<float>(data.network.connections[i].first % 10) * 30.0f + 50.0f;
            float z2 = static_cast<float>(data.network.connections[i].first / 10) * 30.0f + 50.0f;
            float y2 = 50.0f;
            
            writeSVGLine(svg, x, y, x2, y2, "gray", 1.0f);
        }
    }
    
    svg << "</g>" << std::endl;
}

void SVGRenderer::renderTimeSeriesToSVG(std::ofstream& svg, const VisualizationData::TimeSeries& timeSeries) {
    writeSVGG(svg, 200, 300, 1, 1);
    
    // Render time series
    for (size_t trace = 0; trace < timeSeries.neuronActivity.size(); ++trace) {
        svg << "<polyline points=\"";
        for (size_t point = 0; point < timeSeries.timestamps.size(); ++point) {
            float x = static_cast<float>(point) * 2.0f + 50.0f;
            float y = static_cast<float>(timeSeries.neuronActivity[trace].find(data.network.neurons[trace]) != timeSeries.neuronActivity[trace].end() ?
                                      timeSeries.neuronActivity[trace].at(data.network.neurons[trace]) : 0.0f) * -100.0f + 350.0f;
            svg << x << "," << y << " ";
        }
        svg << "\" fill=\"none\" stroke=\"blue\" stroke-width=\"2\"/>" << std::endl;
    }
    
    svg << "</g>" << std::endl;
}

void SVGRenderer::renderMemoryToSVG(std::ofstream& svg, const VisualizationData::Memory& memory) {
    writeSVGG(svg, 400, 300, 1, 1);
    
    // Render memory heatmap
    for (size_t i = 0; i < memory.memoryHeatmaps.size(); ++i) {
        for (size_t j = 0; j < memory.memoryHeatmaps[i].size(); ++j) {
            float x = static_cast<float>(j) * 2.0f + 50.0f;
            float y = static_cast<float>(i) * 2.0f + 50.0f;
            
            float value = memory.memoryHeatmaps[i][j];
            std::string color = getColorFromValue(value);
            writeSVGRect(svg, x, y, 2.0f, 2.0f, color, value);
        }
    }
    
    svg << "</g>" << std::endl;
}

void SVGRenderer::renderPredictionToSVG(std::ofstream& svg, const VisualizationData::Prediction& prediction) {
    writeSVGG(svg, 600, 300, 1, 1);
    
    // Render prediction heatmap
    for (size_t i = 0; i < prediction.confidenceMaps.size(); ++i) {
        for (size_t j = 0; j < prediction.confidenceMaps[i].size(); ++j) {
            float x = static_cast<float>(j) * 2.0f + 50.0f;
            float y = static_cast<float>(i) * 2.0f + 50.0f;
            
            float value = prediction.confidenceMaps[i][j];
            std::string color = getColorFromConfidence(value);
            writeSVGRect(svg, x, y, 2.0f, 2.0f, color, value);
        }
    }
    
    svg << "</g>" << std::endl;
}

std::string SVGRenderer::getColorFromValue(float value) {
    if (value < 0.3f) return "blue";
    if (value < 0.6f) return "green";
    return "red";
}

std::string SVGRenderer::getColorFromWeight(float weight) {
    if (weight < 0.0f) return "blue";
    if (weight < 0.5f) return "green";
    return "red";
}

std::string SVGRenderer::getColorFromConfidence(float confidence) {
    if (confidence < 0.3f) return "red";
    if (confidence < 0.7f) return "yellow";
    return "green";
}

} // namespace nlm