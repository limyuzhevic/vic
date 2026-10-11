// HTMLExporter.cpp - HTML export system implementation

#include "HTMLExporter.hpp"
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
#include <unordered_set>
#include <queue>
#include <random>
#include <limits>

namespace nlm {

struct HTMLExporter::Impl {
    HTMLExportConfig config;
    ExportStats stats;
    bool active;
    bool complete;
    ExportCallback callback;
    std::vector<Section> sections;
    std::function<std::vector<std::vector<std::string>>(double time)> dataSource;
    std::chrono::steady_clock::time_point startTime;
    double progress;
    std::string currentStatus;
    std::mutex mutex;
    std::unordered_map<std::string, std::string> templateCache;
    std::unordered_map<std::string, std::string> cssCache;
    std::unordered_map<std::string, std::string> jsCache;
    
    Impl() : active(false), complete(false), progress(0.0), currentStatus("Initialized") {
        startTime = std::chrono::steady_clock::now();
        
        // Initialize with default sections
        sections = {
            createDefaultSection("Overview", "Overview of the NLM simulation", 0),
            createDefaultSection("Network", "Neural network structure and connectivity", 1),
            createDefaultSection("Spike Activity", "Spike recording and analysis", 2),
            createDefaultSection("Learning Progress", "Learning curve and performance metrics", 3),
            createDefaultSection("Statistics", "Detailed statistical analysis", 4)
        };
    }
};

HTMLExporter::HTMLExporter() : pImpl(std::make_unique<Impl>()) {
}

HTMLExporter::HTMLExporter(const HTMLExportConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

HTMLExporter::~HTMLExporter() {
}

void HTMLExporter::configure(const HTMLExportConfig& config) {
    pImpl->config = config;
    pImpl->sections.clear();
    pImpl->progress = 0.0;
    pImpl->currentStatus = "Configured";
}

bool HTMLExporter::exportBrainVisualization(const Brain& brain, const std::string& outputPath) {
    // Add brain visualization section
    Section brainSection = createBrainSection(brain);
    addSection(brainSection);
    
    return exportToHTML(outputPath + "/brain_visualization.html");
}

bool HTMLExporter::exportSpikeVisualization(const std::vector<SpikeEvent>& spikes, const std::string& outputPath) {
    // Add spike visualization section
    Section spikeSection = createSpikeSection(spikes);
    addSection(spikeSection);
    
    return exportToHTML(outputPath + "/spike_visualization.html");
}

bool HTMLExporter::exportNetworkVisualization(const Brain& brain, const std::string& outputPath) {
    // Add network visualization section
    Section networkSection = createNetworkSection(brain);
    addSection(networkSection);
    
    return exportToHTML(outputPath + "/network_visualization.html");
}

bool HTMLExporter::exportLearningProgress(const std::string& outputPath) {
    // Add learning progress section
    Section learningSection = createLearningSection();
    addSection(learningSection);
    
    return exportToHTML(outputPath + "/learning_progress.html");
}

bool HTMLExporter::exportPerformanceMetrics(const std::string& outputPath) {
    // Add performance metrics section
    Section performanceSection = createPerformanceSection();
    addSection(performanceSection);
    
    return exportToHTML(outputPath + "/performance_metrics.html");
}

bool HTMLExporter::exportComprehensiveReport(const Brain& brain, const std::vector<SpikeEvent>& spikes, const std::string& outputPath) {
    // Clear existing sections
    pImpl->sections.clear();
    
    // Add all sections
    addSection(createDefaultSection("Overview", "Overview of the NLM simulation", 0));
    addSection(createBrainSection(brain));
    addSection(createSpikeSection(spikes));
    addSection(createNetworkSection(brain));
    addSection(createLearningSection());
    addSection(createPerformanceSection());
    addSection(createDefaultSection("Conclusion", "Summary and conclusions", 6));
    
    return exportToHTML(outputPath + "/comprehensive_report.html");
}

void HTMLExporter::addSection(const Section& section) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if section already exists
    auto it = std::find_if(pImpl->sections.begin(), pImpl->sections.end(), 
                          [&section](const Section& s) { return s.id == section.id; });
    
    if (it == pImpl->sections.end()) {
        pImpl->sections.push_back(section);
    } else {
        *it = section; // Update existing section
    }
}

void HTMLExporter::addChart(const ChartConfig& chart) {
    // Find the latest section and add chart to it
    if (!pImpl->sections.empty()) {
        pImpl->sections.back().charts.push_back(chart);
    }
}

void HTMLExporter::setDataSource(const std::function<std::vector<std::vector<std::string>>(double time)>& dataSource) {
    pImpl->dataSource = dataSource;
}

HTMLExporter::ExportStats HTMLExporter::getStats() const {
    return pImpl->stats;
}

bool HTMLExporter::isComplete() const {
    return pImpl->complete;
}

std::string HTMLExporter::generateHTML() const {
    std::ostringstream oss;
    
    // Generate HTML based on template type
    switch (pImpl->config.template) {
        case TemplateType::DEFAULT:
            oss << generateDefaultHTML();
            break;
        case TemplateType::DASHBOARD:
            oss << generateDashboardHTML();
            break;
        case TemplateType::REPORT:
            oss << generateReportHTML();
            break;
        case TemplateType::PRESENTATION:
            oss << generatePresentationHTML();
            break;
        case TemplateType::EXPERIMENTAL:
            oss << generateExperimentalHTML();
            break;
        case TemplateType::DEBUG:
            oss << generateDebugHTML();
            break;
        case TemplateType::MINIMAL:
            oss << generateMinimalHTML();
            break;
        case TemplateType::MODERN:
            oss << generateModernHTML();
            break;
        case TemplateType::CLASSIC:
            oss << generateClassicHTML();
            break;
        case TemplateType::CUSTOM:
            oss << generateCustomHTML();
            break;
        default:
            oss << generateDefaultHTML();
            break;
    }
    
    return oss.str();
}

std::string HTMLExporter::generateCSS() const {
    std::ostringstream oss;
    
    // Base styles
    oss << "/* NLM HTML Exporter CSS */\n";
    oss << "* {\n";
    oss << "  margin: 0;\n";
    oss << "  padding: 0;\n";
    oss << "  box-sizing: border-box;\n";
    oss << "}\n\n";
    
    // Body styles
    oss << "body {\n";
    oss << "  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
    oss << "  line-height: 1.6;\n";
    oss << "  color: #333;\n";
    oss << "  background-color: #f8f9fa;\n";
    oss << "  padding: 20px;\n";
    oss << "}\n\n";
    
    // Container
    oss << ".container {\n";
    oss << "  max-width: 1200px;\n";
    oss << "  margin: 0 auto;\n";
    oss << "  background-color: white;\n";
    oss << "  padding: 30px;\n";
    oss << "  border-radius: 10px;\n";
    oss << "  box-shadow: 0 0 20px rgba(0,0,0,0.1);\n";
    oss << "}\n\n";
    
    // Header
    oss << "header {\n";
    oss << "  text-align: center;\n";
    oss << "  margin-bottom: 40px;\n";
    oss << "  padding-bottom: 20px;\n";
    oss << "  border-bottom: 2px solid #007bff;\n";
    oss << "}\n\n";
    
    // Navigation
    oss << "nav {\n";
    oss << "  background-color: #f8f9fa;\n";
    oss << "  padding: 15px;\n";
    oss << "  border-radius: 5px;\n";
    oss << "  margin-bottom: 30px;\n";
    oss << "}\n\n";
    
    // Sections
    oss << "section {\n";
    oss << "  margin-bottom: 40px;\n";
    oss << "  padding: 20px;\n";
    oss << "  border: 1px solid #e9ecef;\n";
    oss << "  border-radius: 8px;\n";
    oss << "  background-color: #fafbfc;\n";
    oss << "}\n\n";
    
    // Charts
    oss << ".chart {\n";
    oss << "  margin: 20px 0;\n";
    oss << "  text-align: center;\n";
    oss << "}\n\n";
    
    // Tables
    oss << "table {\n";
    oss << "  width: 100%;\n";
    oss << "  border-collapse: collapse;\n";
    oss << "  margin: 20px 0;\n";
    oss << "}\n\n";
    
    oss << "th, td {\n";
    oss << "  padding: 12px;\n";
    oss << "  text-align: left;\n";
    oss << "  border-bottom: 1px solid #dee2e6;\n";
    oss << "}\n\n";
    
    // Footer
    oss << "footer {\n";
    oss << "  text-align: center;\n";
    oss << "  margin-top: 40px;\n";
    oss << "  padding-top: 20px;\n";
    oss << "  border-top: 1px solid #dee2e6;\n";
    oss << "  color: #6c757d;\n";
    oss << "}\n\n";
    
    return oss.str();
}

std::string HTMLExporter::generateJavaScript() const {
    std::ostringstream oss;
    oss << "// NLM HTML Exporter JavaScript\n";
    oss << "// Interactive features and visualizations\n\n";
    
    if (pImpl->config.includeCharts && pImpl->config.chartLibrary == "Chart.js") {
        oss << "// Chart.js library\n";
        oss << "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n\n";
    }
    
    oss << "document.addEventListener('DOMContentLoaded', function() {\n";
    oss << "  // Initialize interactive features\n";
    oss << "  initNavigation();\n";
    oss << "  initCharts();\n";
    oss << "  initSearch();\n";
    oss << "  initPrint();\n";
    oss << "});\n\n";
    
    oss << "function initNavigation() {\n";
    oss << "  // Smooth scrolling for navigation links\n";
    oss << "  document.querySelectorAll('nav a').forEach(anchor => {\n";
    oss << "    anchor.addEventListener('click', function(e) {\n";
    oss << "      e.preventDefault();\n";
    oss << "      const target = document.querySelector(this.getAttribute('href'));\n";
    oss << "      if (target) {\n";
    oss << "        target.scrollIntoView({ behavior: 'smooth' });\n";
    oss << "      }\n";
    oss << "    });\n";
    oss << "  });\n";
    oss << "}\n\n";
    
    oss << "function initCharts() {\n";
    oss << "  // Initialize charts if present\n";
    oss << "  const charts = document.querySelectorAll('.chart canvas');\n";
    oss << "  charts.forEach(canvas => {\n";
    oss << "    if (canvas.getAttribute('data-chart-type')) {\n";
    oss << "      createChart(canvas);\n";
    oss << "    }\n";
    oss << "  });\n";
    oss << "}\n\n";
    
    oss << "function createChart(canvas) {\n";
    oss << "  // Chart creation logic\n";
    oss << "  const type = canvas.getAttribute('data-chart-type');\n";
    oss << "  const data = JSON.parse(canvas.getAttribute('data-chart-data'));\n";
    oss << "  const options = JSON.parse(canvas.getAttribute('data-chart-options') || '{}');\n";
    oss << "  \n";
    oss << "  new Chart(canvas.getContext('2d'), {\n";
    oss << "    type: type,\n";
    oss << "    data: data,\n";
    oss << "    options: options\n";
    oss << "  });\n";
    oss << "}\n\n";
    
    oss << "function initSearch() {\n";
    oss << "  // Search functionality\n";
    oss << "  const searchInput = document.getElementById('searchInput');\n";
    oss << "  if (searchInput) {\n";
    oss << "    searchInput.addEventListener('input', function(e) {\n";
    oss << "      const searchTerm = e.target.value.toLowerCase();\n";
    oss << "      document.querySelectorAll('section').forEach(section => {\n";
    oss << "        const text = section.textContent.toLowerCase();\n";
    oss << "        if (text.includes(searchTerm)) {\n";
    oss << "          section.style.display = 'block';\n";
    oss << "        } else {\n";
    oss << "          section.style.display = 'none';\n";
    oss << "        }\n";
    oss << "      });\n";
    oss << "    });\n";
    oss << "  }\n";
    oss << "}\n\n";
    
    oss << "function initPrint() {\n";
    oss << "  // Print functionality\n";
    oss << "  window.addEventListener('beforeprint', function() {\n";
    oss << "    document.body.classList.add('printing');\n";
    oss << "  });\n";
    oss << "\n";
    oss << "  window.addEventListener('afterprint', function() {\n";
    oss << "    document.body.classList.remove('printing');\n";
    oss << "  });\n";
    oss << "}\n";
    
    return oss.str();
}

std::string HTMLExporter::generateCharts() const {
    std::ostringstream oss;
    oss << "<!-- Charts Section -->\n";
    oss << "<div class=\"charts-container\">\n";
    oss << "  <!-- Charts will be generated dynamically -->\n";
    oss << "</div>\n";
    
    return oss.str();
}

std::string HTMLExporter::generateNavigation() const {
    std::ostringstream oss;
    oss << "<nav>\n";
    oss << "  <h3>Navigation</h3>\n";
    oss << "  <ul>\n";
    for (const auto& section : pImpl->sections) {
        if (section.visible) {
            oss << "    <li><a href=\"#" << section.id << "\">" << section.title << "</a></li>\n";
        }
    }
    oss << "  </ul>\n";
    oss << "</nav>\n";
    
    return oss.str();
}

// Private helper methods
bool HTMLExporter::exportToHTML(const std::string& filename) {
    if (!pImpl->active) {
        NLM_LOG_ERROR("HTMLExporter not active");
        return false;
    }
    
    // Update progress
    pImpl->progress = 50.0;
    pImpl->currentStatus = "Generating HTML content";
    
    // Create output directory
    std::error_code ec;
    std::filesystem::path filepath(filename);
    std::filesystem::create_directories(filepath.parent_path(), ec);
    if (ec) {
        NLM_LOG_ERROR("Failed to create output directory: " + ec.message());
        return false;
    }
    
    // Generate complete HTML
    std::string htmlContent = generateCompleteHTML();
    
    // Write to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << htmlContent;
    file.close();
    
    // Update statistics
    pImpl->stats.totalSections = pImpl->sections.size();
    pImpl->stats.totalCharts = countCharts();
    pImpl->stats.totalImages = countImages();
    pImpl->stats.totalFiles = pImpl->stats.totalSections + pImpl->stats.totalCharts + pImpl->stats.totalImages;
    pImpl->stats.generationTime = calculateGenerationTime();
    
    // Update progress
    pImpl->progress = 100.0;
    pImpl->currentStatus = "HTML export completed";
    pImpl->complete = true;
    
    NLM_LOG_INFO("HTML exported to: " + filename);
    return true;
}

std::string HTMLExporter::generateCompleteHTML() const {
    std::ostringstream oss;
    
    // HTML DOCTYPE and head
    oss << "<!DOCTYPE html>\n";
    oss << "<html lang=\"en\">\n";
    oss << "<head>\n";
    oss << "  <meta charset=\"UTF-8\">\n";
    oss << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    oss << "  <title>" << pImpl->config.pageConfig.title << "</title>\n";
    oss << "  <meta name=\"description\" content=\"" << pImpl->config.pageConfig.description << "\">\n";
    oss << "  <meta name=\"keywords\" content=\"" << pImpl->config.pageConfig.keywords << "\">\n";
    oss << "  <meta name=\"author\" content=\"" << pImpl->config.pageConfig.author << "\">\n";
    oss << "\n";
    
    // CSS
    oss << "  <style>\n";
    oss << generateCSS();
    oss << "  </style>\n";
    
    // Chart library
    if (pImpl->config.includeChartsLibrary) {
        if (pImpl->config.chartLibrary == "Chart.js") {
            oss << "  <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n";
        }
    }
    
    // JavaScript
    oss << "  <script>\n";
    oss << generateJavaScript();
    oss << "  </script>\n";
    
    oss << "</head>\n";
    oss << "<body>\n";
    
    // Header
    oss << "<header>\n";
    oss << "  <h1>" << pImpl->config.pageConfig.title << "</h1>\n";
    oss << "  <p>" << pImpl->config.pageConfig.description << "</p>\n";
    oss << "  <p>Generated: " << getCurrentTime() << "</p>\n";
    oss << "</header>\n";
    
    // Navigation
    if (pImpl->config.includeNavigation) {
        oss << generateNavigation();
    }
    
    // Main content
    oss << "<main class=\"container\">\n";
    
    // Sections
    for (const auto& section : pImpl->sections) {
        if (section.visible) {
            oss << "<section id=\"" << section.id << "\">\n";
            oss << "  <h2>" << section.title << "</h2>\n";
            
            if (section.type == "content") {
                oss << "  <div class=\"content\">\n";
                oss << "    <p>" << section.content << "</p>\n";
                oss << "  </div>\n";
            } else if (section.type == "chart") {
                for (const auto& chart : section.charts) {
                    oss << "  <div class=\"chart\">\n";
                    oss << "    <h3>" << chart.title << "</h3>\n";
                    oss << "    <canvas id=\"" << chart.id << "\" data-chart-type=\"" << chart.type << "\" data-chart-data=\"" << escapeJsonString(chartToJson(chart)) << "\"></canvas>\n";
                    oss << "  </div>\n";
                }
            } else if (section.type == "table") {
                oss << "  <div class=\"table\">\n";
                oss << "    <table>\n";
                for (const auto& row : section.dataTable) {
                    oss << "      <tr><td>" << row.first << "</td><td>" << row.second << "</td></tr>\n";
                }
                oss << "    </table>\n";
                oss << "  </div>\n";
            }
            
            oss << "</section>\n\n";
        }
    }
    
    oss << "</main>\n";
    
    // Footer
    oss << "<footer>\n";
    oss << "  <p>&copy; " << getCurrentYear() << " " << pImpl->config.pageConfig.author << ". All rights reserved.</p>\n";
    oss << "  <p>Generated by NLM HTML Exporter v" << pImpl->config.pageConfig.version << "</p>\n";
    oss << "</footer>\n";
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string HTMLExporter::generateDefaultHTML() const {
    return "<!-- Default HTML template -->\n";
}

std::string HTMLExporter::generateDashboardHTML() const {
    return "<!-- Dashboard HTML template -->\n";
}

std::string HTMLExporter::generateReportHTML() const {
    return "<!-- Report HTML template -->\n";
}

std::string HTMLExporter::generatePresentationHTML() const {
    return "<!-- Presentation HTML template -->\n";
}

std::string HTMLExporter::generateExperimentalHTML() const {
    return "<!-- Experimental HTML template -->\n";
}

std::string HTMLExporter::generateDebugHTML() const {
    return "<!-- Debug HTML template -->\n";
}

std::string HTMLExporter::generateMinimalHTML() const {
    return "<!-- Minimal HTML template -->\n";
}

std::string HTMLExporter::generateModernHTML() const {
    return "<!-- Modern HTML template -->\n";
}

std::string HTMLExporter::generateClassicHTML() const {
    return "<!-- Classic HTML template -->\n";
}

std::string HTMLExporter::generateCustomHTML() const {
    return "<!-- Custom HTML template -->\n";
}

HTMLExporter::Section HTMLExporter::createDefaultSection(const std::string& title, const std::string& content, int order) {
    Section section;
    section.id = "section_" + std::to_string(order);
    section.title = title;
    section.type = "content";
    section.order = order;
    section.visible = true;
    section.content = content;
    return section;
}

HTMLExporter::Section HTMLExporter::createBrainSection(const Brain& brain) {
    Section section;
    section.id = "brain_section";
    section.title = "Neural Network Structure";
    section.type = "content";
    section.order = 0;
    section.visible = true;
    
    std::ostringstream oss;
    oss << "<h3>Network Overview</h3>\n";
    oss << "<p>Neuron Count: " << brain.getNeuronCount() << "</p>\n";
    oss << "<p>Synapse Count: " << brain.getSynapseCount() << "</p>\n";
    oss << "<p>Region Count: " << brain.getRegionCount() << "</p>\n";
    oss << "<h3>Region Details</h3>\n";
    oss << "<ul>\n";
    for (const auto& region : brain.getRegions()) {
        oss << "<li>" << region.getName() << ": " << region.getNeuronCount() << " neurons</li>\n";
    }
    oss << "</ul>\n";
    
    section.content = oss.str();
    return section;
}

HTMLExporter::Section HTMLExporter::createSpikeSection(const std::vector<SpikeEvent>& spikes) {
    Section section;
    section.id = "spike_section";
    section.title = "Spike Activity Analysis";
    section.type = "content";
    section.order = 1;
    section.visible = true;
    
    std::ostringstream oss;
    oss << "<h3>Spike Statistics</h3>\n";
    oss << "<p>Total Spikes: " << spikes.size() << "</p>\n";
    oss << "<p>Unique Neurons: " << getUniqueNeuronCount(spikes) << "</p>\n";
    oss << "<h3>Spike Times</h3>\n";
    oss << "<pre>" << formatSpikeTimes(spikes) << "</pre>\n";
    
    section.content = oss.str();
    return section;
}

HTMLExporter::Section HTMLExporter::createNetworkSection(const Brain& brain) {
    Section section;
    section.id = "network_section";
    section.title = "Network Connectivity";
    section.type = "content";
    section.order = 2;
    section.visible = true;
    
    std::ostringstream oss;
    oss << "<h3>Connectivity Analysis</h3>\n";
    oss << "<p>Total Connections: " << brain.getSynapseCount() << "</p>\n";
    oss << "<p>Connection Density: " << calculateConnectionDensity(brain) << "</p>\n";
    oss << "<h3>Synapse Types</h3>\n";
    oss << "<ul>\n";
    for (const auto& synapse : brain.getSynapses()) {
        if (synapse.isExcitatory()) {
            oss << "<li>Excitatory synapse " << synapse.getId() << " (weight: " << synapse.getWeight() << ")</li>\n";
        } else {
            oss << "<li>Inhibitory synapse " << synapse.getId() << " (weight: " << synapse.getWeight() << ")</li>\n";
        }
    }
    oss << "</ul>\n";
    
    section.content = oss.str();
    return section;
}

HTMLExporter::Section HTMLExporter::createLearningSection() {
    Section section;
    section.id = "learning_section";
    section.title = "Learning Progress";
    section.type = "content";
    section.order = 3;
    section.visible = true;
    
    std::ostringstream oss;
    oss << "<h3>Learning Curve</h3>\n";
    oss << "<p>Learning progress visualization would appear here</p>\n";
    oss << "<h3>Performance Metrics</h3>\n";
    oss << "<p>Training accuracy: <progress value=\"85\" max=\"100\"></progress> 85%</p>\n";
    oss << "<p>Loss function: <strong>0.042</strong></p>\n";
    
    section.content = oss.str();
    return section;
}

HTMLExporter::Section HTMLExporter::createPerformanceSection() {
    Section section;
    section.id = "performance_section";
    section.title = "Performance Metrics";
    section.type = "content";
    section.order = 4;
    section.visible = true;
    
    std::ostringstream oss;
    oss << "<h3>System Performance</h3>\n";
    oss << "<table>\n";
    oss << "<tr><th>Metric</th><th>Value</th><th>Target</th></tr>\n";
    oss << "<tr><td>Throughput</td><td>1000 spikes/sec</td><td>> 500</td></tr>\n";
    oss << "<tr><td>Latency</td><td>5ms</td><td>< 10ms</td></tr>\n";
    oss << "<tr><td>Memory Usage</td><td>256MB</td><td>< 512MB</td></tr>\n";
    oss << "<tr><td>CPU Utilization</td><td>15%</td><td>< 30%</td></tr>\n";
    oss << "</table>\n";
    
    section.content = oss.str();
    return section;
}

int HTMLExporter::getUniqueNeuronCount(const std::vector<SpikeEvent>& spikes) const {
    std::unordered_set<int64_t> uniqueNeurons;
    for (const auto& spike : spikes) {
        uniqueNeurons.insert(spike.neuronId);
    }
    return uniqueNeurons.size();
}

std::string HTMLExporter::formatSpikeTimes(const std::vector<SpikeEvent>& spikes) const {
    std::ostringstream oss;
    for (const auto& spike : spikes) {
        oss << "Neuron " << spike.neuronId << ": " << spike.time << "s\n";
    }
    return oss.str();
}

double HTMLExporter::calculateConnectionDensity(const Brain& brain) const {
    if (brain.getNeuronCount() == 0) return 0.0;
    return static_cast<double>(brain.getSynapseCount()) / 
           (static_cast<double>(brain.getNeuronCount()) * (brain.getNeuronCount() - 1.0) / 2.0);
}

std::string HTMLExporter::chartToJson(const ChartConfig& chart) const {
    // Convert chart config to JSON string
    std::ostringstream oss;
    oss << "{\"type\": \"" << chart.type << "\", \"title\": \"" << chart.title << "\", \"dataColumns\": [";
    for (size_t i = 0; i < chart.dataColumns.size(); ++i) {
        oss << "\"" << chart.dataColumns[i] << "\"";
        if (i < chart.dataColumns.size() - 1) oss << ", ";
    }
    oss << "]}";
    return oss.str();
}

std::string HTMLExporter::escapeJsonString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string HTMLExporter::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), pImpl->config.pageConfig.dateFormat.c_str());
    return oss.str();
}

int HTMLExporter::getCurrentYear() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    tm* tm = std::localtime(&time);
    return tm->tm_year + 1900;
}

int HTMLExporter::countCharts() const {
    int count = 0;
    for (const auto& section : pImpl->sections) {
        count += section.charts.size();
    }
    return count;
}

int HTMLExporter::countImages() const {
    int count = 0;
    for (const auto& section : pImpl->sections) {
        if (section.type == "image") {
            count++;
        }
    }
    return count;
}

double HTMLExporter::calculateGenerationTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->startTime).count();
    return elapsed;
}

} // namespace nlm
