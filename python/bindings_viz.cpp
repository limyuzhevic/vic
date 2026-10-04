#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <atomic>

#include "src/visualization/VisualizationInterface.hpp"
#include "src/visualization/Visualizers.hpp"

namespace py = pybind11;
namespace nlm {

class PyVisualizationManager {
public:
    PyVisualizationManager() = default;
    
    bool initialize(const std::string& output_directory = ".", const std::string& format = "terminal") {
        config.outputDirectory = output_directory;
        config.outputFormat = format;
        return interface.initialize();
    }
    
    void start_monitoring(double interval = 1.0) {
        interface.setUpdateRate(1000.0 / interval);
        monitoring = true;
        monitor_thread = std::thread(&PyVisualizationManager::monitor_loop, this);
    }
    
    void stop_monitoring() {
        monitoring = false;
        if (monitor_thread.joinable()) {
            monitor_thread.join();
        }
    }
    
    void update() {
        interface.update();
    }
    
    void render() {
        interface.render();
    }
    
    void visualize_brain(const Brain& brain) {
        interface.visualizeNetwork(brain);
        interface.visualizeActivity(brain);
        interface.visualizeWeights(brain);
        interface.visualizeRegions(brain);
    }
    
    std::string get_status() const {
        std::stringstream ss;
        ss << "Visualization Interface Status:" << std::endl;
        ss << "  Active: " << (interface.isActive() ? "Yes" : "No") << std::endl;
        ss << "  Update Rate: " << interface.getUpdateRate() << " Hz" << std::endl;
        ss << "  Monitoring: " << (monitoring ? "Yes" : "No") << std::endl;
        ss << "  Config Output Format: " << config.outputFormat << std::endl;
        ss << "  Config Output Directory: " << config.outputDirectory << std::endl;
        return ss.str();
    }
    
    bool save_snapshot(const std::string& filename = "") {
        return interface.saveSnapshot(filename);
    }
    
    std::string create_dashboard() {
        std::stringstream ss;
        ss << "Dashboard created with " << metrics_history.size() << " data points" << std::endl;
        return ss.str();
    }
    
    const VisualizationConfig& get_config() const { return config; }
    void set_config(const VisualizationConfig& c) { config = c; }
    
    std::vector<SystemAlert> get_alerts() const { return interface.getAlerts(); }
    void clear_alerts() { interface.clearAlerts(); }
    
    std::vector<PerformanceMetrics> get_metrics_history() const { return interface.getMetricsHistory(); }
    void reset_metrics() { interface.resetMetrics(); }
    
    ~PyVisualizationManager() {
        stop_monitoring();
    }
    
private:
    void monitor_loop() {
        while (monitoring) {
            auto start = std::chrono::steady_clock::now();
            
            // Get current metrics
            PerformanceMetrics metrics = interface.getCurrentMetrics();
            metrics_history.push_back(metrics);
            
            // Limit history size
            if (metrics_history.size() > 1000) {
                metrics_history.erase(metrics_history.begin());
            }
            
            // Check for alerts
            check_system_health();
            
            auto end = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            
            // Sleep for remaining time to maintain consistent interval
            if (elapsed < 1000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 - elapsed));
            }
        }
    }
    
    void check_system_health() {
        PerformanceMetrics metrics = interface.getCurrentMetrics();
        
        if (metrics.cpuUsage > 90.0) {
            interface.addAlert(SystemAlert(
                SystemAlert::Level::WARNING,
                "High CPU usage detected: " + std::to_string(metrics.cpuUsage) + "%"
            ));
        }
        
        if (metrics.memoryUsage > 95.0) {
            interface.addAlert(SystemAlert(
                SystemAlert::Level::ERROR,
                "High memory usage detected: " + std::to_string(metrics.memoryUsage) + "%"
            ));
        }
        
        if (metrics.simulationTime > 1000.0) {
            interface.addAlert(SystemAlert(
                SystemAlert::Level::WARNING,
                "Slow simulation detected: " + std::to_string(metrics.simulationTime) + " ms"
            ));
        }
    }
    
    VisualizationInterface interface;
    VisualizationConfig config;
    std::atomic<bool> monitoring{false};
    std::thread monitor_thread;
    std::vector<PerformanceMetrics> metrics_history;
};

class PyCommandLineTools {
public:
    PyCommandLineTools() = default;
    
    void print_neural_activity_summary(const Brain& brain) {
        std::cout << std::endl << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM NEURAL ACTIVITY SUMMARY" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        int neurons = brain.getTotalNeuronCount();
        int active = brain.getActiveNeuronCount();
        int firing = brain.getFiringNeuronCount();
        double firing_rate = brain.getAverageFiringRate();
        int synapses = brain.getTotalSynapseCount();
        double ratio = brain.getExcitationInhibitionRatio();
        
        std::cout << "Total Neurons:          " << neurons << std::endl;
        std::cout << "Active Neurons:         " << active << " (" 
                  << (neurons > 0 ? (active * 100.0 / neurons) : 0.0) << "%)" << std::endl;
        std::cout << "Firing Neurons:         " << firing << std::endl;
        std::cout << "Average Firing Rate:    " << firing_rate << " Hz" << std::endl;
        std::cout << "Total Synapses:         " << synapses << std::endl;
        std::cout << "Excitatory/Inhibitory Ratio: " << ratio << std::endl;
        
        // Activity level
        std::string activity_level;
        if (firing_rate > 10.0) {
            activity_level = "HIGH";
        } else if (firing_rate > 5.0) {
            activity_level = "MEDIUM";
        } else {
            activity_level = "LOW";
        }
        
        std::cout << "\nActivity Level:         " << activity_level << std::endl;
        std::cout << "Development Stage:      " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        
        std::cout << "=" << std::string(60, '=') << std::endl << std::endl;
    }
    
    void print_system_status(const PerformanceMetrics& metrics, const std::vector<SystemAlert>& alerts) {
        std::cout << std::endl << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM SYSTEM STATUS" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        // Determine health status
        std::string health_status;
        double health_score = 100.0;
        std::vector<std::string> issues;
        
        if (metrics.cpuUsage > 80.0) {
            health_score -= 10;
            issues.push_back("High CPU usage");
        }
        
        if (metrics.memoryUsage > 90.0) {
            health_score -= 20;
            issues.push_back("Low memory");
        }
        
        if (metrics.simulationTime > 1000.0) {
            health_score -= 15;
            issues.push_back("Slow simulation");
        }
        
        if (health_score > 90) {
            health_status = "HEALTHY";
        } else if (health_score > 70) {
            health_status = "WARNING";
        } else {
            health_status = "CRITICAL";
        }
        
        std::cout << "Overall Health:         " << health_status << " (" << health_score << "%)" << std::endl;
        
        std::cout << "\nResource Usage:" << std::endl;
        std::cout << "  CPU Usage:             " << metrics.cpuUsage << "%" << std::endl;
        std::cout << "  Memory Usage:          " << metrics.memoryUsage << "%" << std::endl;
        std::cout << "  Simulation Time:       " << metrics.simulationTime << " ms" << std::endl;
        
        if (!alerts.empty()) {
            std::cout << "\nAlerts (" << alerts.size() << " total):" << std::endl;
            for (size_t i = std::max<size_t>(0, alerts.size() - 5); i < alerts.size(); ++i) {
                const auto& alert = alerts[i];
                std::string level_str;
                switch (alert.level) {
                    case SystemAlert::Level::INFO: level_str = "INFO"; break;
                    case SystemAlert::Level::WARNING: level_str = "WARN"; break;
                    case SystemAlert::Level::ERROR: level_str = "ERROR"; break;
                    case SystemAlert::Level::CRITICAL: level_str = "CRITICAL"; break;
                }
                std::cout << "  [" << level_str << "] " << alert.message << std::endl;
            }
        } else {
            std::cout << "\nAlerts: No active alerts" << std::endl;
        }
        
        std::cout << "=" << std::string(60, '=') << std::endl << std::endl;
    }
    
    void print_help() {
        std::cout << std::endl << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM VISUALIZATION TOOLS HELP" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        std::cout << "Available Commands:" << std::endl;
        std::cout << "  --monitor          Start real-time monitoring" << std::endl;
        std::cout << "  --plot-dashboard   Create dashboard visualization" << std::endl;
        std::cout << "  --plot-spikes      Create spike raster plot" << std::endl;
        std::cout << "  --plot-regions     Create region heatmap" << std::endl;
        std::cout << "  --plot-development  Create development timeline" << std::endl;
        std::cout << "  --status           Show system status" << std::endl;
        std::cout << "  --help             Show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --monitor --plot-dashboard --interval 1.0" << std::endl;
        std::cout << "  nlm --plot-spikes --output spikes.png" << std::endl;
        std::cout << "  nlm --status --verbose" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl << std::endl;
    }
};

PYBIND11_MODULE(pynlm_viz, m) {
    m.doc() = R"pbdoc(
        NLM Python Visualization Module
        -------------------------------
        Python bindings for NLM visualization and monitoring tools.
        Provides real-time plotting, activity monitoring, and system status visualization.
    )pbdoc";
    
    // Bind VisualizationManager
    py::class_<PyVisualizationManager>(m, "VisualizationManager")
        .def(py::init<>())
        .def("initialize", &PyVisualizationManager::initialize,
             py::arg("output_directory") = ".",
             py::arg("format") = "terminal",
             "Initialize visualization manager")
        .def("start_monitoring", &PyVisualizationManager::start_monitoring,
             py::arg("interval") = 1.0,
             "Start real-time monitoring")
        .def("stop_monitoring", &PyVisualizationManager::stop_monitoring,
             "Stop monitoring")
        .def("update", &PyVisualizationManager::update,
             "Update visualization")
        .def("render", &PyVisualizationManager::render,
             "Render visualization")
        .def("visualize_brain", &PyVisualizationManager::visualize_brain,
             "Visualize brain activity")
        .def("get_status", &PyVisualizationManager::get_status,
             "Get visualization status")
        .def("save_snapshot", &PyVisualizationManager::save_snapshot,
             py::arg("filename") = "",
             "Save current state as snapshot")
        .def("create_dashboard", &PyVisualizationManager::create_dashboard,
             "Create dashboard visualization")
        .def("get_config", &PyVisualizationManager::get_config,
             "Get visualization configuration")
        .def("set_config", &PyVisualizationManager::set_config,
             "Set visualization configuration")
        .def("get_alerts", &PyVisualizationManager::get_alerts,
             "Get system alerts")
        .def("clear_alerts", &PyVisualizationManager::clear_alerts,
             "Clear all alerts")
        .def("get_metrics_history", &PyVisualizationManager::get_metrics_history,
             "Get metrics history")
        .def("reset_metrics", &PyVisualizationManager::reset_metrics,
             "Reset metrics history");
    
    // Bind CommandLineTools
    py::class_<PyCommandLineTools>(m, "CommandLineTools")
        .def(py::init<>())
        .def("print_neural_activity_summary", &PyCommandLineTools::print_neural_activity_summary,
             "Print neural activity summary")
        .def("print_system_status", &PyCommandLineTools::print_system_status,
             "Print system status")
        .def("print_help", &PyCommandLineTools::print_help,
             "Print help information");
    
    // Bind data structures
    py::class_<VisualizationConfig>(m, "VisualizationConfig")
        .def(py::init<>())
        .def_readwrite("enableRealTimeUpdates", &VisualizationConfig::enableRealTimeUpdates)
        .def_readwrite("updateInterval", &VisualizationConfig::updateInterval)
        .def_readwrite("outputFormat", &VisualizationConfig::outputFormat)
        .def_readwrite("outputDirectory", &VisualizationConfig::outputDirectory)
        .def_readwrite("saveSnapshots", &VisualizationConfig::saveSnapshots)
        .def_readwrite("snapshotInterval", &VisualizationConfig::snapshotInterval)
        .def_readwrite("showGrid", &VisualizationConfig::showGrid)
        .def_readwrite("showLabels", &VisualizationConfig::showLabels)
        .def_readwrite("zoomLevel", &VisualizationConfig::zoomLevel)
        .def_readwrite("colorScheme", &VisualizationConfig::colorScheme);
    
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics")
        .def(py::init<>())
        .def_readwrite("cpuUsage", &PerformanceMetrics::cpuUsage)
        .def_readwrite("memoryUsage", &PerformanceMetrics::memoryUsage)
        .def_readwrite("simulationTime", &PerformanceMetrics::simulationTime)
        .def_readwrite("renderingTime", &PerformanceMetrics::renderingTime)
        .def_readwrite("spikeCount", &PerformanceMetrics::spikeCount)
        .def_readwrite("activeNeuronCount", &PerformanceMetrics::activeNeuronCount)
        .def_readwrite("averageFiringRate", &PerformanceMetrics::averageFiringRate)
        .def_readwrite("stepCount", &PerformanceMetrics::stepCount);
    
    py::class_<SystemAlert>(m, "SystemAlert")
        .def(py::init<SystemAlert::Level, std::string, std::string>(),
             py::arg("level"),
             py::arg("message"),
             py::arg("source") = "")
        .def_readwrite("level", &SystemAlert::level)
        .def_readwrite("message", &SystemAlert::message)
        .def_readwrite("source", &SystemAlert::source)
        .def_readwrite("timestamp", &SystemAlert::timestamp);
    
    py::enum_<SystemAlert::Level>(m, "AlertLevel")
        .value("INFO", SystemAlert::Level::INFO)
        .value("WARNING", SystemAlert::Level::WARNING)
        .value("ERROR", SystemAlert::Level::ERROR)
        .value("CRITICAL", SystemAlert::Level::CRITICAL)
        .export_values();
    
    // Bind helper functions
    m.def("create_visualization_manager", &[]() {
        return std::make_shared<PyVisualizationManager>();
    }, "Create a visualization manager");
    
    m.def("create_command_line_tools", &[]() {
        return std::make_shared<PyCommandLineTools>();
    }, "Create command line tools");
    
    m.def("get_version", []() {
        return "NLM Visualization Module v1.0.0";
    }, "Get version information");
}

} // namespace nlm
