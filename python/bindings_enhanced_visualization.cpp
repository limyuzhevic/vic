// Visualization and monitoring utilities
namespace visualization {

    // Simple text-based visualization of brain activity
    std::string visualizeBrainActivity(const Brain& brain, size_t maxNeurons = 20) {
        std::stringstream ss;
        ss << "=== NLM Brain Activity Visualization ===\n\n";
        
        ss << "Statistics:" << std::endl;
        ss << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        ss << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
        ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
        ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << std::endl;
        ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        ss << "  Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        ss << "  Development Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        ss << std::endl;
        
        // Show top firing neurons
        auto regions = brain.getRegions();
        if (!regions.empty()) {
            auto neurons = regions[0]->getAllNeurons();
            if (neurons.size() > maxNeurons) {
                neurons.resize(maxNeurons);
            }
            
            ss << "Top " << neurons.size() << " firing neurons:" << std::endl;
            for (size_t i = 0; i < neurons.size(); ++i) {
                auto* neuron = neurons[i];
                ss << "  Neuron " << i << ": " 
                   << "V=" << neuron->getMembranePotential() 
                   << " thresh=" << neuron->getThreshold()
                   << " type=" << static_cast<int>(neuron->getNeuronType())
                   << " firing=" << neuron->isFiring() << std::endl;
            }
        }
        
        ss << std::endl << "=== End Visualization ===";
        return ss.str();
    }
    
    // Generate comprehensive metrics for training monitoring
    std::map<std::string, double> generateMetrics(
        const std::map<std::string, double>& episodeMetrics,
        size_t episodeNum
    ) {
        std::map<std::string, double> allMetrics;
        allMetrics["episode"] = static_cast<double>(episodeNum);
        allMetrics["total_reward"] = episodeMetrics.at("total_reward");
        allMetrics["steps_completed"] = episodeMetrics.at("steps_completed");
        allMetrics["avg_reward_per_step"] = episodeMetrics.at("avg_reward_per_step");
        allMetrics["max_curiosity"] = episodeMetrics.at("max_curiosity");
        allMetrics["max_novelty"] = episodeMetrics.at("max_novelty");
        allMetrics["max_prediction_error"] = episodeMetrics.at("max_prediction_error");
        allMetrics["avg_firing_rate"] = episodeMetrics.at("avg_firing_rate");
        allMetrics["total_spikes"] = episodeMetrics.at("total_spikes");
        allMetrics["final_development_stage"] = episodeMetrics.at("final_development_stage");
        
        // Calculate rolling averages if available
        if (episodeNum > 0) {
            static double totalReward = 0.0;
            static double totalSteps = 0.0;
            static size_t episodeCount = 0;
            
            totalReward += episodeMetrics.at("total_reward");
            totalSteps += episodeMetrics.at("steps_completed");
            episodeCount++;
            
            allMetrics["avg_total_reward"] = totalReward / episodeCount;
            allMetrics["avg_steps_per_episode"] = totalSteps / episodeCount;
        }
        
        return allMetrics;
    }
    
    // Format metrics for display or logging
    std::string formatMetrics(const std::map<std::string, double>& metrics) {
        std::stringstream ss;
        ss << "=== Training Metrics ===\n";
        
        // Helper function to format values
        auto formatValue = [](const std::string& key, double value) {
            if (key == "episode") return "Episode: " + std::to_string(static_cast<size_t>(value));
            else if (key == "total_reward") return "Total Reward: " + std::to_string(value);
            else if (key == "steps_completed") return "Steps Completed: " + std::to_string(static_cast<size_t>(value));
            else if (key == "avg_reward_per_step") return "Avg Reward/Step: " + std::to_string(value);
            else if (key == "max_curiosity") return "Max Curiosity: " + std::to_string(value);
            else if (key == "max_novelty") return "Max Novelty: " + std::to_string(value);
            else if (key == "max_prediction_error") return "Max Prediction Error: " + std::to_string(value);
            else if (key == "avg_firing_rate") return "Avg Firing Rate: " + std::to_string(value);
            else if (key == "total_spikes") return "Total Spikes: " + std::to_string(static_cast<size_t>(value));
            else if (key == "final_development_stage") return "Dev Stage: " + std::to_string(static_cast<size_t>(value));
            else if (key == "avg_total_reward") return "Avg Total Reward: " + std::to_string(value);
            else if (key == "avg_steps_per_episode") return "Avg Steps/Episode: " + std::to_string(value);
            return key + ": " + std::to_string(value);
        };
        
        for (const auto& [key, value] : metrics) {
            ss << "  " << formatValue(key, value) << std::endl;
        }
        
        ss << "\n=== End Metrics ===";
        return ss.str();
    }
}

// Monitoring and debugging utilities
namespace monitoring {

    // Track training progress and detect anomalies
    struct TrainingMonitor {
        std::vector<double> rewardHistory;
        std::vector<double> noveltyHistory;
        std::vector<double> curiosityHistory;
        std::vector<size_t> stepHistory;
        
        TrainingMonitor() : rewardHistory(), noveltyHistory(), curiosityHistory(), stepHistory() {}
        
        void recordEpisode(const std::map<std::string, double>& metrics) {
            rewardHistory.push_back(metrics.at("total_reward"));
            noveltyHistory.push_back(metrics.at("max_novelty"));
            curiosityHistory.push_back(metrics.at("max_curiosity"));
            stepHistory.push_back(static_cast<size_t>(metrics.at("steps_completed")));
        }
        
        bool detectRewardAnomaly(double threshold = 0.5) const {
            if (rewardHistory.size() < 3) return false;
            
            // Check if last episode reward is significantly different from average
            double avgReward = 0.0;
            for (double r : rewardHistory) avgReward += r;
            avgReward /= rewardHistory.size();
            
            double lastReward = rewardHistory.back();
            return std::abs(lastReward - avgReward) > threshold;
        }
        
        bool detectExplorationDecay(double decayThreshold = 0.8) const {
            if (curiosityHistory.size() < 5) return false;
            
            // Check if curiosity is consistently below threshold
            size_t lowCuriosityCount = 0;
            for (double curiosity : curiosityHistory) {
                if (curiosity < decayThreshold) lowCuriosityCount++;
            }
            
            return (lowCuriosityCount / curiosityHistory.size()) > 0.8;
        }
        
        std::string getStatusReport() const {
            std::stringstream ss;
            ss << "=== Training Monitor Status ===\n";
            ss << "Episodes Tracked: " << rewardHistory.size() << std::endl;
            
            if (!rewardHistory.empty()) {
                double avgReward = 0.0;
                double avgNovelty = 0.0;
                double avgCuriosity = 0.0;
                
                for (double r : rewardHistory) avgReward += r;
                for (double n : noveltyHistory) avgNovelty += n;
                for (double c : curiosityHistory) avgCuriosity += c;
                
                ss << "Average Reward: " << avgReward / rewardHistory.size() << std::endl;
                ss << "Average Novelty: " << avgNovelty / noveltyHistory.size() << std::endl;
                ss << "Average Curiosity: " << avgCuriosity / curiosityHistory.size() << std::endl;
                
                if (detectRewardAnomaly()) {
                    ss << "⚠️  Reward anomaly detected - check for plateaus or overfitted behavior" << std::endl;
                }
                
                if (detectExplorationDecay()) {
                    ss << "⚠️  Exploration decay detected - agent may be exploiting too much" << std::endl;
                }
            }
            
            ss << "=== End Status Report ===";
            return ss.str();
        }
    };

    // Performance metrics for simulation benchmarking
    struct PerformanceMetrics {
        double setupTime;      // Time to initialize systems
        double stepTime;      // Average time per simulation step
        double memoryUsage;    // Memory usage in MB
        double fps;           // Frames per second
        
        PerformanceMetrics() : setupTime(0.0), stepTime(0.0), memoryUsage(0.0), fps(0.0) {}
        
        std::string format() const {
            std::stringstream ss;
            ss << "=== Performance Metrics ===\n";
            ss << "Setup Time: " << setupTime << " seconds\n";
            ss << "Average Step Time: " << stepTime << " seconds\n";
            ss << "Memory Usage: " << memoryUsage << " MB\n";
            ss << "FPS: " << fps << "\n";
            ss << "=== End Performance Metrics ===";
            return ss.str();
        }
    };

    // Simple profiler for timing operations
    class Profiler {
    private:
        std::map<std::string, double> timings;
        std::map<std::string, size_t> counts;
    public:
        void start(const std::string& operation) {
            // In a real implementation, this would record the start time
            counts[operation] = 0;
        }
        
        void record(const std::string& operation, double time) {
            timings[operation] += time;
            counts[operation]++;
        }
        
        std::map<std::string, double> getAverages() const {
            std::map<std::string, double> averages;
            for (const auto& [op, total] : timings) {
                averages[op] = total / counts.at(op);
            }
            return averages;
        }
        
        std::string report() const {
            std::stringstream ss;
            ss << "=== Performance Profile ===\n";
            auto averages = getAverages();
            for (const auto& [op, avg] : averages) {
                ss << "  " << op << ": " << avg << "s avg\n";
            }
            ss << "=== End Profile ===";
            return ss.str();
        }
    };
}

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM Enhanced Python Bindings
        -----------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Includes advanced utilities for visualization, monitoring, and debugging.
        
        Key features:
        - Brain activity visualization and monitoring
        - Training progress tracking and anomaly detection
        - Performance profiling and metrics collection
        - Enhanced error handling and validation
        - High-level convenience functions
    )pbdoc";

    // Register visualization functions
    m.def("visualizeBrainActivity", &visualization::visualizeBrainActivity,
          py::arg("brain") = Brain(nullptr),
          py::arg("maxNeurons") = 20,
          "Generate a text-based visualization of brain activity statistics.");
    
    m.def("generateMetrics", &visualization::generateMetrics,
          py::arg("episodeMetrics"), py::arg("episodeNum"),
          "Generate comprehensive metrics from episode data.");
    
    m.def("formatMetrics", &visualization::formatMetrics,
          py::arg("metrics"),
          "Format metrics dictionary for display or logging.");

    // Register monitoring functions
    py::class_<monitoring::TrainingMonitor>(m, "TrainingMonitor")
        .def(py::init<>())
        .def("recordEpisode", &monitoring::TrainingMonitor::recordEpisode,
             py::arg("metrics"),
             "Record metrics from a training episode.")
        .def("detectRewardAnomaly", &monitoring::TrainingMonitor::detectRewardAnomaly,
             py::arg("threshold") = 0.5,
             "Detect if reward has deviated significantly from average.")
        .def("detectExplorationDecay", &monitoring::TrainingMonitor::detectExplorationDecay,
             py::arg("decayThreshold") = 0.8,
             "Detect if exploration (curiosity) has decayed below threshold.")
        .def("getStatusReport", &monitoring::TrainingMonitor::getStatusReport,
             "Get a status report with anomaly warnings.");

    py::class_<monitoring::PerformanceMetrics>(m, "PerformanceMetrics")
        .def(py::init<>())
        .def_readwrite("setupTime", &monitoring::PerformanceMetrics::setupTime)
        .def_readwrite("stepTime", &monitoring::PerformanceMetrics::stepTime)
        .def_readwrite("memoryUsage", &monitoring::PerformanceMetrics::memoryUsage)
        .def_readwrite("fps", &monitoring::PerformanceMetrics::fps)
        .def("format", &monitoring::PerformanceMetrics::format,
             "Format metrics as a readable string.");

    py::class_<monitoring::Profiler>(m, "Profiler")
        .def(py::init<>())
        .def("start", &monitoring::Profiler::start,
             py::arg("operation"),
             "Start timing an operation.")
        .def("record", &monitoring::Profiler::record,
             py::arg("operation"), py::arg("time"),
             "Record time for an operation.")
        .def("getAverages", &monitoring::Profiler::getAverages,
             "Get average times for all operations.")
        .def("report", &monitoring::Profiler::report,
             "Get a formatted performance profile report.");

    // Include all the high-level utility functions from the main enhanced module
    // These would be linked or included from the main pynlm_enhanced module
    // For now, we'll declare them as forward references
    
    // Re-export important types and constants
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("has", &Config::has, py::arg("key"))
        .def("getKeys", &Config::getKeys)
        .def("getOr", static_cast<double (Config::*)(const std::string&, double) const>(&Config::getOr<double>),
             py::arg("key"), py::arg("defaultValue"))
        .def("getOr", static_cast<int64_t (Config::*)(const std::string&, int64_t) const>(&Config::getOr<int64_t>),
             py::arg("key"), py::arg("defaultValue"));

    py::class_<Brain>(m, "Brain")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage);

    py::class_<AgentBrain>(m, "AgentBrain")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage);

    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getSimulationTime", &SimpleWorld::getSimulationTime);

    // Define key constants
    m.attr("MINIMAL_PRESET") = "minimal";
    m.attr("STANDARD_PRESET") = "standard";
    m.attr("DEVELOPMENT_PRESET") = "development";
    m.attr("HIGH_PERFORMANCE_PRESET") = "high_performance";

} // namespace nlm
