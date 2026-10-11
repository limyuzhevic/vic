// Visualization.cpp - Performance visualization implementation

#include "profiler/Visualization.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

Visualization::Visualization()
    : visConfig() {
}

Visualization::Visualization(const VisualizationConfig& config)
    : visConfig(config) {
}

Visualization::~Visualization() {
    // Cleanup if needed
}

std::string Visualization::generateHtmlDashboard(const std::vector<ChartConfig>& charts) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "  <title>Performance Dashboard</title>\n";
    oss << "  <style>\n";
    oss << "    " << generateCssStyles() << "\n";
    oss << "  </style>\n";
    oss << "  <script>\n";
    oss << "    " << generateJavaScript() << "\n";
    oss << "  </script>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "  <h1>Performance Dashboard</h1>\n";
    oss << "  <div class=\"dashboard\">\n";
    
    for (const auto& chart : charts) {
        oss << "    <div class=\"chart\">\n";
        oss << "      <h2>" << chart.title << "</h2>\n";
        oss << "      <div id=\"chart-" << chart.title << " class=\"chart-container\"></div>\n";
        oss << "    </div>\n";
    }
    
    oss << "  </div>\n";
    oss << "</body>\n";
    oss << "</html>";
    
    return oss.str();
}

std::string Visualization::generateJsonData(const std::vector<ChartConfig>& charts) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"version\": \"1.0\",\n";
    oss << "  \"generated_at\": \"" << getCurrentTime() << "\",\n";
    oss << "  \"charts\": [\n";
    
    for (size_t i = 0; i < charts.size(); ++i) {
        const auto& chart = charts[i];
        oss << "    {\n";
        oss << "      \"title\": \"" << chart.title << "\",\n";
        oss << "      \"type\": \"" << chartTypeToString(chart.type) << "\",\n";
        oss << "      \"data\": [\n";
        
        for (size_t j = 0; j < chart.data.size(); ++j) {
            const auto& dataPoint = chart.data[j];
            oss << "        {\"timestamp\": " << dataPoint.timestamp << ", \"value\": " << dataPoint.value << ", \"label\": \"" << dataPoint.label << "\", \"series\": \"" << dataPoint.series << "\"}";
            if (j < chart.data.size() - 1) {
                oss << ",";
            }
            oss << "\n";
        }
        
        oss << "      ],\n";
        oss << "      \"xAxisLabel\": \"" << chart.xAxisLabel << "\",\n";
        oss << "      \"yAxisLabel\": \"" << chart.yAxisLabel << "\",\n";
        oss << "      \"seriesName\": \"" << chart.seriesName << "\",\n";
        oss << "      \"colorScheme\": \"" << chart.colorScheme << "\",\n";
        oss << "      \"smooth\": " << (chart.smooth ? "true" : "false") << ",\n";
        oss << "      \"lineWidth\": " << chart.lineWidth << ",\n";
        oss << "      \"aggregation\": \"" << chart.aggregation << "\"\n";
        
        oss << "    }";
        if (i < charts.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string Visualization::generateCsvData(const std::vector<ChartConfig>& charts) const {
    std::ostringstream oss;
    
    for (const auto& chart : charts) {
        oss << chart.title << "," << chartTypeToString(chart.type) << "\n";
        oss << "timestamp,value,label,series\n";
        
        for (const auto& dataPoint : chart.data) {
            oss << dataPoint.timestamp << ","
                << dataPoint.value << ","
                << dataPoint.label << ","
                << dataPoint.series << "\n";
        }
        
        oss << "\n\n";
    }
    
    return oss.str();
}

bool Visualization::generatePdfReport(const std::vector<ChartConfig>& charts, const std::string& filename) const {
    // For now, we'll just generate an HTML report and a text summary
    // In a real implementation, you would use a PDF generation library
    std::string htmlContent = generateHtmlDashboard(charts);
    
    // Save HTML content to a temporary file
    std::ofstream htmlFile(filename + ".html");
    if (!htmlFile.is_open()) {
        NLM_LOG_ERROR("Failed to open HTML file for writing: " + filename + ".html");
        return false;
    }
    
    htmlFile << htmlContent;
    htmlFile.close();
    
    // Generate a text summary
    std::string textContent = generateTextReport(charts);
    
    std::ofstream textFile(filename + ".txt");
    if (!textFile.is_open()) {
        NLM_LOG_ERROR("Failed to open text file for writing: " + filename + ".txt");
        return false;
    }
    
    textFile << textContent;
    textFile.close();
    
    NLM_LOG_INFO("PDF report generated (HTML and text versions): " + filename);
    return true;
}

bool Visualization::generatePngChart(const ChartConfig& chart, const std::string& filename) const {
    // For now, we'll just generate an HTML chart
    // In a real implementation, you would use a chart library like Chart.js with a headless browser
    std::string htmlContent = generateSingleChartHtml(chart);
    
    std::ofstream htmlFile(filename + ".html");
    if (!htmlFile.is_open()) {
        NLM_LOG_ERROR("Failed to open HTML file for writing: " + filename + ".html");
        return false;
    }
    
    htmlFile << htmlContent;
    htmlFile.close();
    
    NLM_LOG_INFO("Chart HTML generated: " + filename + ".html");
    return true;
}

std::string Visualization::generatePerformanceDashboard(
    const std::vector<std::vector<ChartDataPoint>>& datasets,
    const std::vector<std::string>& labels) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "  <title>Performance Dashboard</title>\n";
    oss << "  <style>\n";
    oss << "    " << generateCssStyles() << "\n";
    oss << "  </style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "  <h1>Performance Dashboard</h1>\n";
    
    for (size_t i = 0; i < datasets.size(); ++i) {
        oss << "  <div class=\"chart\">\n";
        oss << "    <h2>" << labels[i] << "</h2>\n";
        oss << "    <div id=\"chart-" << i << " class=\"chart-container\"></div>\n";
        oss << "    <script>\n";
        oss << "      const data" << i << " = " << generateJsonForDataPoints(datasets[i]) << ";\n";
        oss << "      createChart('chart-" << i << "', data" << i << ");\n";
        oss << "    </script>\n";
        oss << "  </div>\n";
    }
    
    oss << "</body>\n";
    oss << "</html>";
    
    return oss.str();
}

ChartConfig Visualization::createLineChart(const std::string& title,
                                          const std::vector<ChartDataPoint>& data,
                                          const std::string& xAxisLabel,
                                          const std::string& yAxisLabel) const {
    ChartConfig chart;
    chart.title = title;
    chart.type = ChartType::LINE_CHART;
    chart.data = data;
    chart.xAxisLabel = xAxisLabel.empty() ? "Time" : xAxisLabel;
    chart.yAxisLabel = yAxisLabel.empty() ? "Value" : yAxisLabel;
    chart.seriesName = "Series 1";
    chart.colorScheme = "default";
    chart.smooth = false;
    chart.lineWidth = 2.0;
    chart.aggregation = "none";
    
    return chart;
}

ChartConfig Visualization::createBarChart(const std::string& title,
                                         const std::vector<ChartDataPoint>& data,
                                         const std::string& xAxisLabel,
                                         const std::string& yAxisLabel) const {
    ChartConfig chart;
    chart.title = title;
    chart.type = ChartType::BAR_CHART;
    chart.data = data;
    chart.xAxisLabel = xAxisLabel.empty() ? "Category" : xAxisLabel;
    chart.yAxisLabel = yAxisLabel.empty() ? "Value" : yAxisLabel;
    chart.seriesName = "Series 1";
    chart.colorScheme = "default";
    chart.smooth = false;
    chart.lineWidth = 1.0;
    chart.aggregation = "none";
    
    return chart;
}

ChartConfig Visualization::createHeatMap(const std::string& title,
                                         const std::vector<std::vector<double>>& data,
                                         const std::vector<std::string>& xLabels,
                                         const std::vector<std::string>& yLabels) const {
    ChartConfig chart;
    chart.title = title;
    chart.type = ChartType::HEAT_MAP;
    chart.colorScheme = "heatmap";
    
    // Convert heat map data to ChartDataPoint format
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            ChartDataPoint point;
            point.timestamp = static_cast<int64_t>(i * 1000);
            point.value = data[i][j];
            point.label = (i < xLabels.size() ? xLabels[i] : std::to_string(i)) + ", " + 
                         (j < yLabels.size() ? yLabels[j] : std::to_string(j));
            point.series = "Heatmap";
            chart.data.push_back(point);
        }
    }
    
    chart.xAxisLabel = "X Axis";
    chart.yAxisLabel = "Y Axis";
    chart.seriesName = "Heatmap Data";
    
    return chart;
}

std::vector<ChartConfig> Visualization::createPerformanceTrends(
    const std::vector<std::vector<ChartDataPoint>>& datasets) const {
    std::vector<ChartConfig> charts;
    
    for (size_t i = 0; i < datasets.size(); ++i) {
        ChartConfig chart = createLineChart("Performance Trend " + std::to_string(i + 1), datasets[i]);
        chart.title = "Performance Trend: " + std::to_string(i + 1);
        chart.seriesName = datasets.size() > 1 ? ("Series " + std::to_string(i + 1)) : "Performance";
        charts.push_back(chart);
    }
    
    return charts;
}

std::vector<ChartConfig> Visualization::createBottleneckVisualization(
    const std::vector<std::string>& bottleneckData) const {
    std::vector<ChartConfig> charts;
    
    // Create a bar chart for bottleneck visualization
    std::vector<ChartDataPoint> dataPoints;
    
    for (size_t i = 0; i < bottleneckData.size(); ++i) {
        ChartDataPoint point;
        point.timestamp = static_cast<int64_t>(i);
        point.value = 1.0; // All bottlenecks are equally important
        point.label = bottleneckData[i];
        point.series = "Bottlenecks";
        dataPoints.push_back(point);
    }
    
    ChartConfig chart = createBarChart("Performance Bottlenecks", dataPoints, "Bottleneck", "Count");
    chart.colorScheme = "warning";
    charts.push_back(chart);
    
    return charts;
}

bool Visualization::startWebServer() const {
    // For now, this is a placeholder
    // In a real implementation, you would start a web server with the dashboard
    NLM_LOG_INFO("Web server not yet implemented. Enable web dashboard manually.");
    return false;
}

void Visualization::stopWebServer() const {
    NLM_LOG_INFO("Web server stop not yet implemented.");
}

std::string Visualization::generateReportTemplate() const {
    std::ostringstream oss;
    oss << "Performance Report Template\n";
    oss << "=============================\n\n";
    oss << "Report Generated: " << getCurrentTime() << "\n\n";
    oss << "This is a template for performance reports.\n\n";
    oss << "Key Sections:\n";
    oss << "1. Executive Summary\n";
    oss << "2. Performance Metrics\n";
    oss << "3. Trend Analysis\n";
    oss << "4. Bottleneck Analysis\n";
    oss << "5. Recommendations\n";
    oss << "6. Detailed Data\n\n";
    oss << "For detailed reports, enable web dashboard or specify output format.";
    
    return oss.str();
}

std::string Visualization::generateHtmlChart(const ChartConfig& chart) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "  <title>" << chart.title << "</title>\n";
    oss << "  <style>\n";
    oss << "    " << generateCssStyles() << "\n";
    oss << "  </style>\n";
    oss << "  <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "  <div style=\"width: " << visConfig.chartWidth << "px; height: " << visConfig.chartHeight << "px;\">\n";
    oss << "    <canvas id=\"chart\"></canvas>\n";
    oss << "  </div>\n";
    oss << "  <script>\n";
    oss << "    const ctx = document.getElementById('chart').getContext('2d');\n";
    oss << "    const data = {\n";
    oss << "      labels: [";
    
    for (size_t i = 0; i < chart.data.size(); ++i) {
        oss << "\"" << chart.data[i].timestamp << "\"";
        if (i < chart.data.size() - 1) {
            oss << ", ";
        }
    }
    
    oss << "],\n";
    oss << "      datasets: [{\n";
    oss << "        label: \"" << chart.seriesName << "\",\n";
    oss << "        data: [";
    
    for (size_t i = 0; i < chart.data.size(); ++i) {
        oss << chart.data[i].value;
        if (i < chart.data.size() - 1) {
            oss << ", ";
        }
    }
    
    oss << "],\n";
    oss << "        backgroundColor: 'rgba(75, 192, 192, 0.2)',\n";
    oss << "        borderColor: 'rgba(75, 192, 192, 1)',\n";
    oss << "        borderWidth: " << chart.lineWidth << "\n";
    oss << "      }]\n";
    oss << "    };\n";
    oss << "\n";
    oss << "    const config = {\n";
    oss << "      type: '\" << chartTypeToString(chart.type) << "\",\n";
    oss << "      data: data,\n";
    oss << "      options: {\n";
    oss << "        responsive: true,\n";
    oss << "        plugins: {\n";
    oss << "          title: {\n";
    oss << "            display: true,\n";
    oss << "            text: '\" << chart.title << "\"\n";
    oss << "          },\n";
    oss << "          legend: {\n";
    oss << "            display: " << (visConfig.showLegend ? "true" : "false") << "\n";
    oss << "          }\n";
    oss << "        },\n";
    oss << "        scales: {\n";
    oss << "          x: {\n";
    oss << "            title: {\n";
    oss << "              display: true,\n";
    oss << "              text: '\" << chart.xAxisLabel << "\"
";
    oss << "            }\n";
    oss << "          },\n";
    oss << "          y: {\n";
    oss << "            title: {\n";
    oss << "              display: true,\n";
    oss << "              text: '\" << chart.yAxisLabel << "\"
";
    oss << "            }\n";
    oss << "          }\n";
    oss << "        },\n";
    oss << "        interaction: {\n";
    oss << "          intersect: false,\n";
    oss << "          mode: 'index'\n";
    oss << "        }\n";
    oss << "      }\n";
    oss << "    };\n";
    oss << "\n";
    oss << "    new Chart(ctx, config);\n";
    oss << "  </script>\n";
    oss << "</body>\n";
    oss << "</html>";
    
    return oss.str();
}

std::string Visualization::generateCssStyles() const {
    std::string theme = visConfig.theme == "dark" ? "dark" : "light";
    
    return "\n"
           "    body { font-family: Arial, sans-serif; margin: 20px; background-color: " + (theme == "dark" ? "#2d2d2d" : "#ffffff") + "; color: " + (theme == "dark" ? "#ffffff" : "#000000") + "; }\n"
           "    .dashboard { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 20px; }\n"
           "    .chart { border: 1px solid #ddd; border-radius: 5px; padding: 15px; background-color: " + (theme == "dark" ? "#3d3d3d" : "#f9f9f9") + "; }\n"
           "    .chart-container { position: relative; height: 300px; }\n"
           "    h1 { color: " + (theme == "dark" ? "#4a9" : "#333") + "; }\n"
           "    h2 { color: " + (theme == "dark" ? "#6bf" : "#666") + "; margin-bottom: 10px; }\n";
}

std::string Visualization::generateJavaScript() const {
    return "function createChart(canvasId, data) {\n"
           "  const ctx = document.getElementById(canvasId).getContext('2d');\n"
           "  const chart = new Chart(ctx, {\n"
           "    type: 'line',\n"
           "    data: data,\n"
           "    options: {\n"
           "      responsive: true,\n"
           "      plugins: {\n"
           "        legend: { display: true },\n"
           "        title: { display: true, text: 'Performance Chart' }\n"
           "      },\n"
           "      scales: {\n"
           "        x: { title: { display: true, text: 'Time' } },\n"
           "        y: { title: { display: true, text: 'Value' } }\n"
           "      }\n"
           "    }\n"
           "  });\n"
           "}\n";
}

std::string Visualization::escapeHtml(const std::string& text) const {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '&', "&amp;");
    std::replace(escaped.begin(), escaped.end(), '<', "&lt;");
    std::replace(escaped.begin(), escaped.end(), '>', "&gt;");
    std::replace(escaped.begin(), escaped.end(), '"', "&quot;");
    std::replace(escaped.begin(), escaped.end(), '\'', "&apos;");
    return escaped;
}

std::string Visualization::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Visualization::chartTypeToString(ChartType type) const {
    switch (type) {
        case ChartType::LINE_CHART: return "line";
        case ChartType::BAR_CHART: return "bar";
        case ChartType::SCATTER_PLOT: return "scatter";
        case ChartType::HEAT_MAP: return "heatmap";
        case ChartType::GANTT_CHART: return "gantt";
        case ChartType::TIMELINE: return "timeline";
        case ChartType::COMPARISON: return "comparison";
        case ChartType::DISTRIBUTION: return "distribution";
        case ChartType::STACKED_BAR: return "stackedBar";
        case ChartType::AREA_CHART: return "area";
        default: return "line";
    }
}

std::string Visualization::generateJsonForDataPoints(const std::vector<ChartDataPoint>& dataPoints) const {
    std::ostringstream oss;
    oss << "[\n";
    
    for (size_t i = 0; i < dataPoints.size(); ++i) {
        const auto& point = dataPoints[i];
        oss << "    {\n";
        oss << "      \"timestamp\": " << point.timestamp << ",\n";
        oss << "      \"value\": " << point.value << ",\n";
        oss << "      \"label\": \"" << escapeHtml(point.label) << "\",\n";
        oss << "      \"series\": \"" << point.series << "\"\n";
        oss << "    }";
        if (i < dataPoints.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    return oss.str();
}

std::string Visualization::generateTextReport(const std::vector<ChartConfig>& charts) const {
    std::ostringstream oss;
    oss << "Performance Visualization Report\n";
    oss << "==============================\n\n";
    oss << "Generated: " << getCurrentTime() << "\n";
    oss << "Theme: " << visConfig.theme << "\n\n";
    
    for (size_t i = 0; i < charts.size(); ++i) {
        const auto& chart = charts[i];
        oss << "Chart " << i + 1 << ": " << chart.title << "\n";
        oss << "  Type: " << chartTypeToString(chart.type) << "\n";
        oss << "  Series: " << chart.seriesName << "\n";
        oss << "  Data Points: " << chart.data.size() << "\n";
        oss << "  X-Axis: " << chart.xAxisLabel << "\n";
        oss << "  Y-Axis: " << chart.yAxisLabel << "\n\n";
    }
    
    return oss.str();
}

} // namespace nlm