#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "../core/Types/Types.hpp"

namespace nlm {

class SVGRenderer {
public:
    SVGRenderer();
    ~SVGRenderer();

    // Initialize the renderer
    bool initialize();

    // Shutdown the renderer
    void shutdown();

    // Update the renderer
    void update(double deltaTime);

    // Render data to SVG
    bool render(const std::string& filepath, const std::map<std::string, VisualizationData>& data);

    // Set SVG width and height
    void setSize(int width, int height);

    // Get SVG width
    int getWidth() const;

    // Get SVG height
    int getHeight() const;

    // Set SVG view box
    void setViewBox(float minX, float minY, float width, float height);

    // Set SVG background color
    void setBackgroundColor(float r, float g, float b, float a = 1.0f);

    // Get SVG background color
    std::array<float, 4> getBackgroundColor() const;

    // Set SVG document title
    void setDocumentTitle(const std::string& title);

    // Get SVG document title
    const std::string& getDocumentTitle() const;

    // Set SVG description
    void setDescription(const std::string& description);

    // Get SVG description
    const std::string& getDescription() const;

    // Enable/disable compression
    void enableCompression(bool enabled);
    bool isCompressionEnabled() const;

    // Set compression level
    void setCompressionLevel(int level);
    int getCompressionLevel() const;

    // Render network to SVG
    void renderNetworkToSVG(std::ofstream& svg, const VisualizationData::Network& network);

    // Render activity to SVG
    void renderActivityToSVG(std::ofstream& svg, const VisualizationData::Activity& activity);

    // Render spikes to SVG
    void renderSpikesToSVG(std::ofstream& svg, const VisualizationData::Spikes& spikes);

    // Render weights to SVG
    void renderWeightsToSVG(std::ofstream& svg, const VisualizationData::Weights& weights);

    // Render 3D view to SVG
    void render3DViewToSVG(std::ofstream& svg, const VisualizationData& data);

    // Render time series to SVG
    void renderTimeSeriesToSVG(std::ofstream& svg, const VisualizationData::TimeSeries& timeSeries);

    // Render memory to SVG
    void renderMemoryToSVG(std::ofstream& svg, const VisualizationData::Memory& memory);

    // Render prediction to SVG
    void renderPredictionToSVG(std::ofstream& svg, const VisualizationData::Prediction& prediction);

    // Write SVG header
    void writeSVGHeader(std::ofstream& svg);

    // Write SVG footer
    void writeSVGFooter(std::ofstream& svg);

    // Write SVG group
    void writeSVGG(std::ofstream& svg, float x, float y, float width, float height);

    // Write SVG circle
    void writeSVGCircle(std::ofstream& svg, float cx, float cy, float r, const std::string& fill = "black", float opacity = 1.0f);

    // Write SVG rectangle
    void writeSVGRect(std::ofstream& svg, float x, float y, float width, float height, const std::string& fill = "black", float opacity = 1.0f);

    // Write SVG line
    void writeSVGLine(std::ofstream& svg, float x1, float y1, float x2, float y2, const std::string& stroke = "black", float strokeWidth = 1.0f);

    // Write SVG text
    void writeSVGText(std::ofstream& svg, float x, float y, const std::string& text, const std::string& fontFamily = "Arial", float fontSize = 12.0f, const std::string& fill = "black");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm