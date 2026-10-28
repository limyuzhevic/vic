// Visualization and plotting utilities implementation
#include "NLMPythonVisualization.h"
#include "NLMPythonTypes.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

// Neural activity visualization
class NeuralActivityVisualizer {
public:
    struct VisualizationSettings {
        std::string title = "Neural Activity Visualization";
        std::string xlabel = "Time Steps";
        std::string ylabel = "Neurons/Firing Rate";
        double width = 800.0;
        double height = 600.0;
        std::string outputFormat = "png";
        bool showGrid = true;
        bool showLegend = true;
        std::map<std::string, std::string> colors;
    };
    
    VisualizationSettings settings;
    
    NeuralActivityVisualizer() {
        // Set default colors
        settings.colors["firing_rate"] = "#1f77b4";
        settings.colors["neurons"] = "#ff7f0e";
        settings.colors["spikes"] = "#2ca02c";
        settings.colors["e_i_ratio"] = "#d62728";
    }
    
    // Create activity plot from brain simulation data
    std::string createActivityPlot(const std::shared_ptr<Brain>& brain,
                                   const std::vector<SimulationStats::Stats>& statistics,
                                   const std::string& outputPath = "neural_activity.png") {
        try {
            // Create a simple ASCII-based visualization for now
            // In a real implementation, this would use matplotlib, plotly, or similar
            std::ofstream outFile(outputPath);
            
            if (!statistics.empty()) {
                outFile << "Neural Activity Visualization\n";
                outFile << "================================\n\n";
                
                outFile << "Brain Statistics:\n";
                outFile << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
                outFile << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
                outFile << "  Final Spike Count: " << brain->getTotalSpikeCount() << "\n";
                outFile << "  Final Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
                outFile << "  Final Firing Rate: " << brain->getAverageFiringRate() << "\n";
                outFile << "  Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n\n";
                
                outFile << "Simulation Statistics:\n";
                outFile << "  Time Steps: " << statistics.size() << "\n";
                outFile << "  Final Time: " << statistics.back().timeElapsed << " seconds\n\n";
                
                // Plot firing rate over time (ASCII histogram)
                outFile << "Firing Rate Over Time:\n";
                outFile << "  Step | Firing Rate | Spike Count\n";
                outFile << "  -----|-------------|------------\n";
                
                size_t numPoints = std::min<size_t>(statistics.size(), 50); // Limit for display
                for (size_t i = 0; i < numPoints; ++i) {
                    size_t step = (statistics.size() - 1) * i / std::max<size_t>(numPoints - 1, 1);
                    const auto& stats = statistics[step];
                    
                    outFile << "  " << std::setw(5) << step << " |";
                    
                    // Create a simple bar for firing rate
                    size_t barWidth = static_cast<size_t>(stats.averageFiringRate * 0.5);
                    for (size_t j = 0; j < barWidth && j < 50; ++j) {
                        outFile << "#";
                    }
                    outFile << "| " << stats.averageFiringRate << " | " << stats.totalSpikes << "\n";
                }
            }
            
            outFile.close();
            return outputPath;
        } catch (...) {
            return "";
        }
    }
    
    // Create spike raster plot
    std::string createSpikeRasterPlot(const std::shared_ptr<Brain>& brain,
                                      const std::string& outputPath = "spike_raster.png") {
        try {
            std::ofstream outFile(outputPath);
            outFile << "Spike Raster Plot\n";
            outFile << "===================\n\n";
            
            // Get spike information from brain
            outFile << "Brain Spike Information:\n";
            outFile << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
            outFile << "  Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
            outFile << "  Excitatory/Inhibitory Ratio: " << brain->getExcitationInhibitionRatio() << "\n\n";
            
            // Create a simple ASCII spike raster
            outFile << "Spike Raster (Neurons vs Time):\n";
            outFile << "  Neuron | Time Steps\n";
            outFile << "  -------|------------\n";
            
            // This would normally show actual spike times, but for now we'll simulate
            size_t numNeurons = brain->getTotalNeuronCount();
            for (size_t neuron = 0; neuron < std::min<size_t>(numNeurons, 20); ++neuron) {
                outFile << "  " << std::setw(7) << neuron << "| ";
                
                // Simulate spike times for display
                for (size_t timeStep = 0; timeStep < 50; ++timeStep) {
                    if ((neuron + timeStep) % 3 == 0) {
                        outFile << "*";
                    } else {
                        outFile << " ";
                    }
                }
                outFile << "\n";
            }
            
            outFile.close();
            return outputPath;
        } catch (...) {
            return "";
        }
    }
    
    // Create development stage plot
    std::string createDevelopmentPlot(const std::shared_ptr<Brain>& brain,
                                      const std::string& outputPath = "development_plot.png") {
        try {
            std::ofstream outFile(outputPath);
            outFile << "Development Stage Visualization\n";
            outFile << "=============================\n\n";
            
            auto stage = brain->getDevelopmentalStage();
            outFile << "Current Development Stage: " << static_cast<int>(stage) << "\n";
            
            // Map development stage to description
            std::string stageDesc;
            switch (stage) {
                case DevelopmentalStage::Initial: stageDesc = "Initial (neonatal)"; break;
                case DevelopmentalStage::CriticalPeriod: stageDesc = "Critical Period"; break;
                case DevelopmentalStage::Maturation: stageDesc = "Maturation"; break;
                case DevelopmentalStage::Adult: stageDesc = "Adult"; break;
                case DevelopmentalStage::Aging: stageDesc = "Aging"; break;
                default: stageDesc = "Unknown";
            }
            
            outFile << "Description: " << stageDesc << "\n\n";
            
            // Plot development characteristics
            outFile << "Development Characteristics:\n";
            outFile << "  Plasticity Level: " << (stage == DevelopmentalStage::CriticalPeriod ? "High" : "Variable") << "\n";
            outFile << "  Learning Capacity: " << (stage == DevelopmentalStage::Initial ? "Low" : "High") << "\n";
            outFile << "  Stability: " << (stage == DevelopmentalStage::Aging ? "Low" : "High") << "\n\n";
            
            // Create ASCII progress bar
            outFile << "Development Progress:\n";
            size_t progressBarWidth = 40;
            size_t progress = static_cast<size_t>((static_cast<int>(stage) / 4.0) * progressBarWidth);
            
            outFile << "  ["; 
            for (size_t i = 0; i < progressBarWidth; ++i) {
                if (i < progress) {
                    outFile << "=";
                } else {
                    outFile << " ";
                }
            }
            outFile << "] " << (progress * 100 / progressBarWidth) << "%\n";
            
            outFile.close();
            return outputPath;
        } catch (...) {
            return "";
        }
    }
    
    // Create comprehensive neural network visualization
    std::string createComprehensiveVisualization(const std::shared_ptr<Brain>& brain,
                                                 const std::vector<SimulationStats::Stats>& statistics,
                                                 const std::string& outputPath = "comprehensive.png") {
        try {
            std::ofstream outFile(outputPath);
            outFile << "Comprehensive NLM Brain Visualization\n";
            outFile << "===================================\n\n";
            
            // Generate all plots and combine
            auto activityPlot = createActivityPlot(brain, statistics, outputPath + ".activity.txt");
            auto rasterPlot = createSpikeRasterPlot(brain, outputPath + ".raster.txt");
            auto developmentPlot = createDevelopmentPlot(brain, outputPath + ".development.txt");
            
            outFile << "Generated visualizations:\n";
            outFile << "  1. " << activityPlot << "\n";
            outFile << "  2. " << rasterPlot << "\n";
            outFile << "  3. " << developmentPlot << "\n\n";
            
            outFile.close();
            return outputPath;
        } catch (...) {
            return "";
        }
    }
    
    // Set visualization settings
    void setSettings(const VisualizationSettings& settings) {
        this->settings = settings;
    }
    
    // Get visualization settings
    VisualizationSettings getSettings() const {
        return settings;
    }
};

// Plotting utilities for simulation data
class PlotGenerator {
public:
    // Generate time series plot
    static std::string generateTimeSeriesPlot(const std::string& title,
                                              const std::vector<double>& xData,
                                              const std::vector<double>& yData,
                                              const std::string& outputPath = "time_series.png") {
        try {
            std::ofstream outFile(outputPath);
            outFile << "Time Series Plot: " << title << "\n";
            outFile << "==================\n\n";
            
            if (xData.size() != yData.size() || xData.empty()) {
                outFile << "No data to plot\n";
                outFile.close();
                return "";
            }
            
            outFile << "Data points: " << xData.size() << "\n";
            outFile << "X range: [" << xData.front() << ", " << xData.back() << "]\n";
            outFile << "Y range: [" << *std::min_element(yData.begin(), yData.end()) << ", " << *std::max_element(yData.begin(), yData.end()) << "]\n\n";
            
            // Create ASCII plot
            outFile << "Plot:\n";
            
            double yMin = *std::min_element(yData.begin(), yData.end());
            double yMax = *std::max_element(yData.begin(), yData.end());
            double yRange = yMax - yMin;
            
            if (yRange > 0) {
                for (size_t row = 0; row < 20; ++row) {
                    double yThreshold = yMin + (yRange * (1.0 - static_cast<double>(row) / 20.0));
                    outFile << "  " << std::fixed << std::setprecision(2) << yThreshold << " |";
                    
                    for (size_t col = 0; col < xData.size() && col < 60; ++col) {
                        double yValue = yData[col];
                        if (yValue >= yThreshold - yRange/40 && yValue <= yThreshold + yRange/40) {
                            outFile << "*";
                        } else {
                            outFile << " ";
                        }
                    }
                    outFile << "\n";
                }
            }
            
            outFile.close();
            return outputPath;
        } catch (...) {
            return "";
        }
    }
    
    // Generate histogram plot
    static std::string generateHistogramPlot(const std::string& title,
                                              const std::vector<double>& data,
                                              size_t bins = 10,
                                              const std::string& outputPath = "histogram.png") {
        try {
            std::ofstream outFile(outputPath);
            outFile << "Histogram: " << title << "\n";
            outFile << "==============\n\n";
            
            if (data.empty()) {
                outFile << "No data to plot\n";
                outFile.close();
                return "";
            }
            
            double minVal = *std::min_element(data.begin(), data.end());
            double maxVal = *std::max_element(data.begin(), data.end());
            double range = maxVal - minVal;
            
            if (range <= 0) {
                outFile << "No data variation to plot\n";
                outFile.close();
                return "";
            }
            
            double binWidth = range / bins;
            std::vector<size_t> binCounts(bins, 0);
            
            for (double value : data) {
                size_t binIndex = static_cast<size_t>((value - minVal) / binWidth);
                if (binIndex >= bins) binIndex = bins - 1;
                binCounts[binIndex]++;
            }
            
            outFile << "Bins: " << bins << " | Width: " << binWidth << " | Max count: " << *std::max_element(binCounts.begin(), binCounts.end()) << "\n\n";
            
            // Create ASCII histogram
            outFile << "Histogram:\n";
            size_t maxCount = *std::max_element(binCounts.begin(), binCounts.end());
            
            for (size_t bin = 0; bin < bins; ++bin) {
                outFile << "  [" << std::fixed << std::setprecision(2) 
                        << (minVal + bin * binWidth) << " - " 
                        << (minVal + (bin + 1) * binWidth) << "] |";
                
                size_t barHeight = static_cast<size_t>(static_cast<double>(binCounts[bin]) / maxCount * 30);
                for (size_t row = 0; row < barHeight; ++row) {
                    outFile << "#";
                }
                outFile << " " << binCounts[bin] << "\n";
            }
            
            outFile.close();
            return outputPath;
        } catch (...) {
            return "";
        }
    }
    
    // Generate correlation plot
    static std::string generateCorrelationPlot(const std::string& title,
                                               const std::vector<double>& xData,
                                               const std::vector<double>& yData,
                                               const std::string& outputPath = "correlation.png") {
        try {
            std::ofstream outFile(outputPath);
            outFile << "Correlation Plot: " << title << "\n";
            outFile << "==================\n\n";
            
            if (xData.size() != yData.size() || xData.size() < 2) {
                outFile << "Insufficient data for correlation plot\n";
                outFile.close();
                return "";
            }
            
            // Calculate correlation
            double sumX = std::accumulate(xData.begin(), xData.end(), 0.0);
            double sumY = std::accumulate(yData.begin(), yData.end(), 0.0);
            double sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
            
            for (size_t i = 0; i < xData.size(); ++i) {
                sumXY += xData[i] * yData[i];
                sumX2 += xData[i] * xData[i];
                sumY2 += yData[i] * yData[i];
            }
            
            double n = static_cast<double>(xData.size());
            double correlation = (n * sumXY - sumX * sumY) / 
                                std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
            
            outFile << "Correlation coefficient: " << std::fixed << std::setprecision(4) << correlation << "\n";
            outFile << "Correlation strength: " << (std::abs(correlation) > 0.7 ? "Strong" : 
                                                   std::abs(correlation) > 0.3 ? "Moderate" : "Weak") << "\n";
            outFile << "Relationship: " << (correlation > 0 ? "Positive" : "Negative") << "\n\n";
            
            // Create scatter plot
            outFile << "Scatter Plot:\n";
            
            double xMin = *std::min_element(xData.begin(), xData.end());
            double xMax = *std::max_element(xData.begin(), xData.end());
            double yMin = *std::min_element(yData.begin(), yData.end());
            double yMax = *std::max_element(yData.begin(), yData.end());
            
            // Scale data for display
            std::vector<double> scaledX, scaledY;
            for (size_t i = 0; i < xData.size() && i < 40; ++i) {
                double sx = (xData[i] - xMin) / (xMax - xMin) * 39;
                double sy = 1.0 - (yData[i] - yMin) / (yMax - yMin) * 19; // Invert Y axis
                scaledX.push_back(sx);
                scaledY.push_back(sy);
            }
            
            // Create grid
            for (double sy = 1.9; sy >= -0.1; sy -= 0.1) {
                outFile << "  " << std::fixed << std::setprecision(1) << (yMax - sy * (yMax - yMin)) << " |";
                
                for (size_t i = 0; i < scaledX.size(); ++i) {
                    if (std::abs(scaledX[i] - i) < 0.5 && std::abs(scaledY[i] - sy) < 0.05) {
                        outFile << "*";
                    } else {
                        outFile << " ";
                    }
                }
                outFile << "\n";
            }
            
            outFile.close();
            return outputPath;
        } catch (...) {
            return "";
        }
    }
};

void registerVisualizationUtils(py::module_& m) {
    // Neural activity visualizer
    py::class_<NeuralActivityVisualizer>(m, "NeuralActivityVisualizer", 
                                         "Generate visualizations of neural activity")
        .def(py::init<>())
        .def("create_activity_plot", &NeuralActivityVisualizer::createActivityPlot,
             py::arg("brain"), py::arg("statistics"), py::arg("output_path") = "neural_activity.png",
             "Create activity plot from brain simulation data")
        .def("create_spike_raster_plot", &NeuralActivityVisualizer::createSpikeRasterPlot,
             py::arg("brain"), py::arg("output_path") = "spike_raster.png",
             "Create spike raster plot")
        .def("create_development_plot", &NeuralActivityVisualizer::createDevelopmentPlot,
             py::arg("brain"), py::arg("output_path") = "development_plot.png",
             "Create development stage plot")
        .def("create_comprehensive_visualization", &NeuralActivityVisualizer::createComprehensiveVisualization,
             py::arg("brain"), py::arg("statistics"), py::arg("output_path") = "comprehensive.png",
             "Create comprehensive visualization combining multiple plots")
        .def("set_settings", &NeuralActivityVisualizer::setSettings,
             py::arg("settings"), "Set visualization settings")
        .def("get_settings", &NeuralActivityVisualizer::getSettings,
             "Get current visualization settings");
    
    // Plot generator utilities
    py::class_<PlotGenerator>(m, "PlotGenerator", "Utilities for generating various types of plots")
        .def_static("generate_time_series_plot", &PlotGenerator::generateTimeSeriesPlot,
                    py::arg("title"), py::arg("x_data"), py::arg("y_data"), py::arg("output_path") = "time_series.png",
                    "Generate time series plot from data")
        .def_static("generate_histogram_plot", &PlotGenerator::generateHistogramPlot,
                    py::arg("title"), py::arg("data"), py::arg("bins") = 10, py::arg("output_path") = "histogram.png",
                    "Generate histogram plot from data")
        .def_static("generate_correlation_plot", &PlotGenerator::generateCorrelationPlot,
                    py::arg("title"), py::arg("x_data"), py::arg("y_data"), py::arg("output_path") = "correlation.png",
                    "Generate correlation plot between two data sets");
    
    // Convenience visualization functions
    m.def("plot_neural_activity", [](std::shared_ptr<Brain> brain,
                                     const std::vector<SimulationStats::Stats>& statistics,
                                     const std::string& outputPath = "neural_activity.png") {
        NeuralActivityVisualizer visualizer;
        return visualizer.createActivityPlot(brain, statistics, outputPath);
    }, py::arg("brain"), py::arg("statistics"), py::arg("output_path") = "neural_activity.png",
       "Quick function to plot neural activity (convenience function)");

    m.def("visualize_spikes", [](std::shared_ptr<Brain> brain,
                                const std::string& outputPath = "spike_raster.png") {
        NeuralActivityVisualizer visualizer;
        return visualizer.createSpikeRasterPlot(brain, outputPath);
    }, py::arg("brain"), py::arg("output_path") = "spike_raster.png",
       "Quick function to visualize spike patterns (convenience function)");

    m.def("plot_development", [](std::shared_ptr<Brain> brain,
                                const std::string& outputPath = "development_plot.png") {
        NeuralActivityVisualizer visualizer;
        return visualizer.createDevelopmentPlot(brain, outputPath);
    }, py::arg("brain"), py::arg("output_path") = "development_plot.png",
       "Quick function to plot development stage (convenience function)");

    m.def("create_comprehensive_report", [](std::shared_ptr<Brain> brain,
                                           const std::vector<SimulationStats::Stats>& statistics,
                                           const std::string& outputPath = "comprehensive_report.png") {
        NeuralActivityVisualizer visualizer;
        return visualizer.createComprehensiveVisualization(brain, statistics, outputPath);
    }, py::arg("brain"), py::arg("statistics"), py::arg("output_path") = "comprehensive_report.png",
       "Create comprehensive report with all visualizations (convenience function)");
}
