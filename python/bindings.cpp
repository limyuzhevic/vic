    }, py::arg("simulations"), py::arg("steps"), "Benchmark simulation performance and return timing metrics");

    // Add visualization interface
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface for debugging and analysis)pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize, "Initialize visualization")
        .def("update", &VisualizationInterface::update, "Update visualization data")
        .def("render", &VisualizationInterface::render, "Render current state")
        .def("close", &VisualizationInterface::close, "Close visualization")
        .def("isActive", &VisualizationInterface::isActive, "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"), "Set update rate")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate, "Get update rate")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"), "Visualize neural network")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"), "Visualize brain activity")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"), "Visualize spike raster plot")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"), "Visualize weight matrix");

    // Add interactive visualization utilities
    m.def("createVisualizer", []() {
        return std::make_shared<VisualizationInterface>();
    }, "Create a new visualization interface")
    
    m.def("plotActivity", [](const std::shared_ptr<Brain>& brain, uint64_t steps) {
        // Generate activity plot data
        std::vector<double> timePoints;
        std::vector<double> firingRates;
        std::vector<size_t> spikeCounts;
        
        for (uint64_t i = 0; i < steps; i += 10) { // Sample every 10 steps
            timePoints.push_back(static_cast<double>(i));
            brain->step(i);
            firingRates.push_back(brain->getAverageFiringRate());
            spikeCounts.push_back(brain->getFiringNeuronCount());
        }
        
        return std::make_tuple(timePoints, firingRates, spikeCounts);
    }, py::arg("brain"), py::arg("steps"), 
       "Generate activity plot data for simulation monitoring")
    
    // Add network topology visualization
    m.def("analyzeNetworkTopology", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> topology;
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        size_t regions = brain->getRegionCount();
        
        topology["total_neurons"] = static_cast<double>(totalNeurons);
        topology["total_synapses"] = static_cast<double>(totalSynapses);
        topology["synapse_density"] = totalSynapses / (totalNeurons * (totalNeurons - 1));
        topology["region_count"] = static_cast<double>(regions);
        
        if (regions > 0) {
            double avgSynapsesPerRegion = static_cast<double>(totalSynapses) / regions;
            topology["avg_synapses_per_region"] = avgSynapsesPerRegion;
        }
        
        return topology;
    }, py::arg("brain"), "Analyze and return network topology metrics")
    
    // Add spike visualization utilities
    m.def("generateSpikeRaster", [](const std::shared_ptr<Brain>& brain, uint64_t steps) {
        std::vector<std::pair<uint64_t, size_t>> spikeEvents;
        
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i);
            size_t firingCount = brain->getFiringNeuronCount();
            if (firingCount > 0) {
                spikeEvents.push_back(std::make_pair(i, firingCount));
            }
        }
        
        return spikeEvents;
    }, py::arg("brain"), py::arg("steps"), "Generate spike raster plot data")
    
    // Add connectivity visualization
    m.def("computeConnectivityMatrix", [](const std::shared_ptr<Brain>& brain, uint64_t steps) {
        std::vector<std::vector<double>> connectivity;
        size_t neuronCount = brain->getTotalNeuronCount();
        
        if (neuronCount == 0) {
            return connectivity;
        }
        
        connectivity.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
        
        // Run simulation to establish stable connectivity
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        // For now, return a simple diagonal matrix as placeholder
        // In a real implementation, this would compute actual connectivity
        for (size_t i = 0; i < neuronCount; ++i) {
            connectivity[i][i] = 1.0; // Self-connection
        }
        
        return connectivity;
    }, py::arg("brain"), py::arg("steps"), "Compute connectivity matrix for visualization")
    
    // Add real-time monitoring utilities
    m.def("monitorRealTime", [](const std::shared_ptr<Brain>& brain, uint64_t steps) {
        std::map<std::string, std::vector<double>> realTimeMetrics;
        
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i);
            
            double firingRate = brain->getAverageFiringRate();
            double avgFiring = brain->getAverageFiringRate();
            size_t totalSpikes = brain->getTotalSpikeCount();
            size_t firingCount = brain->getFiringNeuronCount();
            
            realTimeMetrics["firing_rate"].push_back(firingRate);
            realTimeMetrics["total_spikes"].push_back(static_cast<double>(totalSpikes));
            realTimeMetrics["firing_neuron_count"].push_back(static_cast<double>(firingCount));
            realTimeMetrics["time"].push_back(static_cast<double>(i));
        }
        
        return realTimeMetrics;
    }, py::arg("brain"), py::arg("steps"), "Monitor brain activity in real-time and return metrics")
    
    // Add visualization export utilities
    m.def("exportVisualizationData", [](const std::shared_ptr<Brain>& brain, 
                                       const std::string& filepath, uint64_t steps) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw py::value_error("Cannot open file for writing: '" + filepath + "'");
        }
        
        file << "NLM Visualization Export\n";
        file << "Generated at: " << std::chrono::system_clock::now() << "\n";
        file << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        file << "Total Spikes: " << brain->getTotalSpikeCount() << "\n\n";
        
        // Export spike data
        file << "Spike Events:\n";
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i);
            size_t firingCount = brain->getFiringNeuronCount();
            if (firingCount > 0) {
                file << "  Step " << i << ": " << firingCount << " spikes\n";
            }
        }
        
        file << "\nNeuron Statistics:\n";
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        file << "  Firing Neuron Count: " << brain->getFiringNeuronCount() << "\n";
        file << "  Total Neuron Count: " << brain->getTotalNeuronCount() << "\n";
        file << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), py::arg("steps"), "Export visualization data to file")
    
    // Add interactive plotting utilities
    m.def("createPlotData", [](const std::map<std::string, std::vector<double>>& metrics) {
        py::dict plotData;
        
        for (const auto& pair : metrics) {
            // Convert vector to Python list
            py::list pyList;
            for (const auto& value : pair.second) {
                pyList.append(value);
            }
            plotData[pair.first] = pyList;
        }
        
        return plotData;
    }, py::arg("metrics"), "Convert metrics to Python data structure for plotting")
    
    // Add visualization configuration
    m.def("configureVisualization", [](bool enableNetwork, bool enableActivity, 
                                       bool enableSpikes, bool enableWeights) {
        auto config = std::make_shared<Config>();
        config->set("visualization.enable_network", enableNetwork);
        config->set("visualization.enable_activity", enableActivity);
        config->set("visualization.enable_spikes", enableSpikes);
        config->set("visualization.enable_weights", enableWeights);
        return config;
    }, py::arg("enableNetwork")=true, py::arg("enableActivity")=true,
       py::arg("enableSpikes")=true, py::arg("enableWeights")=false,
       "Configure visualization options")
    
    // Add quick visualization helpers
    m.def("quickViz", [](const std::shared_ptr<Brain>& brain, uint64_t steps) {
        std::map<std::string, double> quickStats;
        
        quickStats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        quickStats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        quickStats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        
        // Run simulation
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        quickStats["final_firing_rate"] = brain->getAverageFiringRate();
        quickStats["current_firing_count"] = static_cast<double>(brain->getFiringNeuronCount());
        quickStats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        return quickStats;
    }, py::arg("brain"), py::arg("steps"), "Get quick visualization statistics for simulation")
    
    // Add visualization comparison utilities
    m.def("compareVisualizations", [](const std::vector<std::pair<std::string, std::map<std::string, double>>>& visualizations) {
        std::map<std::string, double> comparison;
        
        if (visualizations.empty()) {
            return comparison;
        }
        
        // Find all metrics keys
        std::set<std::string> allKeys;
        for (const auto& viz : visualizations) {
            for (const auto& metric : viz.second) {
                allKeys.insert(metric.first);
            }
        }
        
        // Calculate average for each metric across visualizations
        for (const std::string& key : allKeys) {
            double sum = 0.0;
            size_t count = 0;
            
            for (const auto& viz : visualizations) {
                if (viz.second.find(key) != viz.second.end()) {
                    sum += viz.second.at(key);
                    count++;
                }
            }
            
            if (count > 0) {
                comparison[key] = sum / static_cast<double>(count);
            }
        }
        
        return comparison;
    }, py::arg("visualizations"), "Compare multiple visualization results and return averages");
}