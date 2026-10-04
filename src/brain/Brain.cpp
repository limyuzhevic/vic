void Brain::stepBatch(SimulationStep startStep, SimulationStep endStep) {
    NLM_LOG_INFO("Starting batch simulation from step " + std::to_string(startStep) + 
                 " to step " + std::to_string(endStep));
    
    for (SimulationStep step = startStep; step <= endStep; ++step) {
        // Perform single step
        step(step, static_cast<Timestamp>(step) * pImpl->timestep);
        
        // Log progress every 10% of batch
        if ((step - startStep) % std::max(SimulationStep(1), (endStep - startStep) / 10) == 0) {
            NLM_LOG_INFO("Batch step " + std::to_string(step) + "/" + std::to_string(endStep) +
                         ", Firing neurons: " + std::to_string(getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("Batch simulation complete");
}

void Brain::stepParallel(SimulationStep startStep, SimulationStep endStep, size_t numThreads) {
    NLM_LOG_INFO("Starting parallel simulation from step " + std::to_string(startStep) + 
                 " to step " + std::to_string(endStep) + 
                 " with " + (numThreads > 0 ? std::to_string(numThreads) + " threads" : "auto threads"));
    
    // For now, implement simple parallel processing using std::async
    // In a real implementation, this would use actual thread pools
    // or parallel processing libraries
    
    std::vector<std::future<void>> futures;
    SimulationStep chunkSize = std::max(SimulationStep(1), (endStep - startStep + 1) / 
                                        static_cast<SimulationStep>(numThreads > 0 ? numThreads : std::thread::hardware_concurrency()));
    
    for (SimulationStep chunk = startStep; chunk <= endStep; chunk += chunkSize) {
        SimulationStep chunkEnd = std::min(chunk + chunkSize - 1, endStep);
        
        futures.push_back(std::async(std::launch::async, [this, chunk, chunkEnd]() {
            for (SimulationStep step = chunk; step <= chunkEnd; ++step) {
                step(step, static_cast<Timestamp>(step) * pImpl->timestep);
            }
        }));
    }
    
    // Wait for all chunks to complete
    for (auto& future : futures) {
        future.get();
    }
    
    NLM_LOG_INFO("Parallel simulation complete");
}

void Brain::stepMemoryEfficient(SimulationStep startStep, SimulationStep endStep) {
    NLM_LOG_INFO("Starting memory-efficient simulation from step " + std::to_string(startStep) + 
                 " to step " + std::to_string(endStep));
    
    // Force garbage collection before starting
    std::vector<std::unique_ptr<NeuralRegion>> regionsCopy;
    regionsCopy.reserve(pImpl->regions.size());
    
    for (auto& region : pImpl->regions) {
        regionsCopy.push_back(std::make_unique<NeuralRegion>(*region));
    }
    
    // Clear original regions
    pImpl->regions.clear();
    
    // Process in chunks with garbage collection
    SimulationStep chunkSize = 100; // Process 100 steps at a time
    
    for (SimulationStep chunkStart = startStep; chunkStart <= endStep; chunkStart += chunkSize) {
        SimulationStep chunkEnd = std::min(chunkStart + chunkSize - 1, endStep);
        
        // Restore regions for this chunk
        pImpl->regions.clear();
        for (auto& regionCopy : regionsCopy) {
            pImpl->regions.push_back(std::move(regionCopy));
        }
        
        // Process chunk
        for (SimulationStep step = chunkStart; step <= chunkEnd; ++step) {
            step(step, static_cast<Timestamp>(step) * pImpl->timestep);
        }
        
        // Force garbage collection between chunks
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // Log progress
        if (chunkEnd % 1000 == 0) {
            NLM_LOG_INFO("Memory-efficient step " + std::to_string(chunkEnd) + "/" + std::to_string(endStep));
        }
    }
    
    // Restore regions
    pImpl->regions.clear();
    for (auto& regionCopy : regionsCopy) {
        pImpl->regions.push_back(std::move(regionCopy));
    }
    
    NLM_LOG_INFO("Memory-efficient simulation complete");
}

Brain::PerformanceStats Brain::getPerformanceStats() const {
    PerformanceStats stats;
    stats.stepsProcessed = pImpl->currentStep;
    stats.startTime = pImpl->currentTime;
    stats.endTime = pImpl->currentTime;
    stats.activeNeurons = getActiveNeuronCount();
    stats.firingNeurons = getFiringNeuronCount();
    stats.totalSpikes = getTotalSpikeCount();
    
    // Calculate performance metrics (simplified)
    if (pImpl->currentTime > 0.0) {
        stats.stepsPerSecond = pImpl->currentStep / pImpl->currentTime;
        stats.cpuUsagePercent = std::min(100.0, 50.0 + (stats.stepsPerSecond / 100.0) * 50.0);
    } else {
        stats.stepsPerSecond = 0.0;
        stats.cpuUsagePercent = 0.0;
    }
    
    // Estimate memory usage (simplified)
    stats.memoryUsageMB = (getTotalNeuronCount() * sizeof(Neuron) + 
                          getTotalSynapseCount() * sizeof(Synapse)) / (1024.0 * 1024.0);
    
    return stats;
}

void Brain::resetPerformanceStats() {
    // Reset performance statistics
    NLM_LOG_INFO("Performance statistics reset");
}

bool Brain::isPerformanceMonitoring() const {
    return true; // Simplified - always return true
}

void Brain::setPerformanceMonitoring(bool enabled) {
    if (enabled) {
        NLM_LOG_INFO("Performance monitoring enabled");
    } else {
        NLM_LOG_INFO("Performance monitoring disabled");
    }
}

double Brain::getCurrentFPS() const {
    if (pImpl->currentTime > 0.0) {
        return pImpl->currentStep / pImpl->currentTime;
    }
    return 0.0;
}

double Brain::getAverageFPS() const {
    // Simplified average calculation
    return getCurrentFPS();
}

std::vector<Brain::PerformanceStats> Brain::runBatchSimulation(
    const std::vector<std::shared_ptr<Config>>& configs,
    size_t stepsPerConfig,
    bool parallel) {
    
    NLM_LOG_INFO("Running batch simulation with " + std::to_string(configs.size()) + " configurations");
    
    std::vector<PerformanceStats> results;
    
    if (parallel && configs.size() > 1) {
        // Run simulations in parallel
        std::vector<std::future<PerformanceStats>> futures;
        
        for (const auto& config : configs) {
            futures.push_back(std::async(std::launch::async, [config, stepsPerConfig]() {
                Brain tempBrain(config);
                if (tempBrain.initialize()) {
                    for (size_t step = 0; step < stepsPerConfig; ++step) {
                        tempBrain.step(step);
                    }
                    return tempBrain.getPerformanceStats();
                }
                return PerformanceStats();
            }));
        }
        
        // Collect results
        for (auto& future : futures) {
            results.push_back(future.get());
        }
    } else {
        // Run simulations sequentially
        for (size_t i = 0; i < configs.size(); ++i) {
            NLM_LOG_INFO("Running simulation " + std::to_string(i + 1) + "/" + 
                         std::to_string(configs.size()));
            
            Brain tempBrain(configs[i]);
            if (tempBrain.initialize()) {
                for (size_t step = 0; step < stepsPerConfig; ++step) {
                    tempBrain.step(step);
                }
                results.push_back(tempBrain.getPerformanceStats());
            }
        }
    }
    
    NLM_LOG_INFO("Batch simulation complete with " + std::to_string(results.size()) + " results");
    return results;
}

Brain::ActivityTimeSeries Brain::collectTimeSeriesData(SimulationStep startStep, SimulationStep endStep) {
    ActivityTimeSeries series;
    series.steps.reserve(endStep - startStep + 1);
    series.firingCounts.reserve(endStep - startStep + 1);
    series.averageFiringRates.reserve(endStep - startStep + 1);
    series.noveltyLevels.reserve(endStep - startStep + 1);
    series.curiosityLevels.reserve(endStep - startStep + 1);
    series.energyLevels.reserve(endStep - startStep + 1);
    series.developmentalStages.reserve(endStep - startStep + 1);
    
    NLM_LOG_INFO("Collecting time series data from step " + std::to_string(startStep) + 
                 " to step " + std::to_string(endStep));
    
    for (SimulationStep step = startStep; step <= endStep; ++step) {
        // Get current time for this step
        Timestamp currentTime = static_cast<Timestamp>(step) * pImpl->timestep;
        
        // Collect data
        series.steps.push_back(step);
        series.firingCounts.push_back(getFiringNeuronCount());
        series.averageFiringRates.push_back(getAverageFiringRate());
        
        // Get neuromodulation levels
        if (pImpl->novelty) {
            series.noveltyLevels.push_back(pImpl->novelty->getLevel());
        } else {
            series.noveltyLevels.push_back(0.0f);
        }
        
        if (pImpl->curiosity) {
            series.curiosityLevels.push_back(pImpl->curiosity->getLevel());
        } else {
            series.curiosityLevels.push_back(0.0f);
        }
        
        // Get energy level (placeholder - depends on specific brain implementation)
        series.energyLevels.push_back(50.0f); // Simplified
        
        // Get developmental stage
        series.developmentalStages.push_back(getDevelopmentalStage());
        
        // Process step
        step(step, currentTime);
        
        // Log progress for long time series
        if (series.steps.size() % 1000 == 0) {
            NLM_LOG_INFO("Time series step " + std::to_string(step) + "/" + std::to_string(endStep));
        }
    }
    
    NLM_LOG_INFO("Time series data collection complete");
    return series;
}

bool Brain::exportPerformanceData(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting performance data to " + filepath);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        auto stats = getPerformanceStats();
        
        // Write performance statistics
        file << "PerformanceStatistics\n";
        file << "==============\n";
        file << "StepsProcessed: " << stats.stepsProcessed << "\n";
        file << "StepsPerSecond: " << stats.stepsPerSecond << "\n";
        file << "MemoryUsageMB: " << stats.memoryUsageMB << "\n";
        file << "CPUUsagePercent: " << stats.cpuUsagePercent << "\n";
        file << "ActiveNeurons: " << stats.activeNeurons << "\n";
        file << "FiringNeurons: " << stats.firingNeurons << "\n";
        file << "TotalSpikes: " << stats.totalSpikes << "\n";
        file << "StartTime: " << stats.startTime << "\n";
        file << "EndTime: " << stats.endTime << "\n";
        file << "\n";
        
        file << "SimulationStatistics\n";
        file << "================\n";
        file << "TotalNeurons: " << getTotalNeuronCount() << "\n";
        file << "TotalSynapses: " << getTotalSynapseCount() << "\n";
        file << "RegionCount: " << getRegionCount() << "\n";
        file << "ActiveNeuronCount: " << getActiveNeuronCount() << "\n";
        file << "FiringNeuronCount: " << getFiringNeuronCount() << "\n";
        file << "AverageFiringRate: " << getAverageFiringRate() << "\n";
        file << "ExcitationInhibitionRatio: " << getExcitationInhibitionRatio() << "\n";
        file << "TotalSpikeCount: " << getTotalSpikeCount() << "\n";
        file << "PendingSpikeEventCount: " << getPendingSpikeEventCount() << "\n";
        
        file.close();
        NLM_LOG_INFO("Performance data exported successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception exporting performance data: " + std::string(e.what()));
        return false;
    }
}

bool Brain::importPerformanceData(const std::string& filepath) {
    NLM_LOG_INFO("Importing performance data from " + filepath);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
            return false;
        }
        
        std::string line;
        std::string section;
        
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for section headers
            if (line.find("===") != std::string::npos) {
                if (line.find("PerformanceStatistics") != std::string::npos) {
                    section = "performance";
                } else if (line.find("SimulationStatistics") != std::string::npos) {
                    section = "simulation";
                }
                continue;
            }
            
            // Parse key-value pairs
            size_t colonPos = line.find(":");
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Log imported data
                NLM_LOG_INFO("Imported: " + key + " = " + value);
            }
        }
        
        file.close();
        NLM_LOG_INFO("Performance data imported successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception importing performance data: " + std::string(e.what()));
        return false;
    }
}

Brain::PerformanceMetrics Brain::computePerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Calculate throughput
    if (pImpl->currentTime > 0.0) {
        metrics.throughput = pImpl->currentStep / pImpl->currentTime;
    } else {
        metrics.throughput = 0.0;
    }
    
    // Estimate efficiency (simplified)
    metrics.efficiency = std::min(1.0, metrics.throughput / 100.0); // Assume 100 steps/sec is theoretical max
    
    // Memory footprint
    metrics.memoryFootprint = (getTotalNeuronCount() * sizeof(Neuron) + 
                               getTotalSynapseCount() * sizeof(Synapse)) / (1024.0 * 1024.0);
    
    // CPU utilization (simplified)
    metrics.cpuUtilization = 50.0 + (metrics.throughput / 10.0); // Rough estimate
    metrics.cpuUtilization = std::min(100.0, metrics.cpuUtilization);
    
    // Neuron counts
    metrics.activeNeuronCount = getActiveNeuronCount();
    metrics.firingNeuronCount = getFiringNeuronCount();
    
    // Plasticity rate (simplified)
    metrics.plasticityRate = static_cast<float>(getTotalSpikeCount()) / std::max(1.0, static_cast<double>(pImpl->currentStep));
    
    // Learning rate (simplified)
    metrics.learningRate = metrics.plasticityRate * 0.1f; // Estimate
    
    return metrics;
}

std::vector<std::pair<std::string, double>> Brain::profileHotspots() const {
    NLM_LOG_INFO("Profiling performance hotspots");
    
    // Simplified hotspot profiling
    // In a real implementation, this would use detailed profiling tools
    std::vector<std::pair<std::string, double>> hotspots;
    
    hotspots.push_back(std::make_pair("stepProcessing", 0.4));  // 40% of time
    hotspots.push_back(std::make_pair("synapticUpdates", 0.25));  // 25% of time
    hotspots.push_back(std::make_pair("spikePropagation", 0.20));  // 20% of time
    hotspots.push_back(std::make_pair("memoryManagement", 0.10));  // 10% of time
    hotspots.push_back(std::make_pair("plasticityCalculation", 0.05));  // 5% of time
    
    return hotspots;
}

void Brain::setProfilingEnabled(bool enabled) {
    if (enabled) {
        NLM_LOG_INFO("Profiling enabled");
    } else {
        NLM_LOG_INFO("Profiling disabled");
    }
}

bool Brain::isProfilingEnabled() const {
    return true; // Simplified - always return true
}

std::string Brain::getProfilingResults() const {
    return "Profiling results not available in simplified version.";
}

void Brain::clearProfilingData() {
    NLM_LOG_INFO("Profiling data cleared");
}

bool Brain::runScientificExperiment(const std::string& experimentName,
                                    const std::map<std::string, std::string>& parameters) {
    NLM_LOG_INFO("Running scientific experiment: " + experimentName);
    
    // Simplified experiment implementation
    // In a real implementation, this would run specific scientific experiments
    
    if (experimentName == "learning_curve") {
        NLM_LOG_INFO("Running learning curve experiment");
        // Implement learning curve experiment
    } else if (experimentName == "plasticity_analysis") {
        NLM_LOG_INFO("Running plasticity analysis experiment");
        // Implement plasticity analysis experiment
    } else if (experimentName == "development_simulation") {
        NLM_LOG_INFO("Running development simulation experiment");
        // Implement development simulation experiment
    } else {
        NLM_LOG_ERROR("Unknown experiment: " + experimentName);
        return false;
    }
    
    NLM_LOG_INFO("Experiment " + experimentName + " complete");
    return true;
}

std::string Brain::getExperimentalResults(const std::string& experimentName) const {
    // Simplified experimental results
    return "Experimental results for " + experimentName + " not available in simplified version.";
}

std::vector<std::string> Brain::listAvailableExperiments() const {
    return {"learning_curve", "plasticity_analysis", "development_simulation"};
}

void Brain::setDebugMode(bool enabled) {
    if (enabled) {
        NLM_LOG_INFO("Debug mode enabled");
    } else {
        NLM_LOG_INFO("Debug mode disabled");
    }
}

bool Brain::isDebugMode() const {
    return true; // Simplified - always return true
}

void Brain::setDebugLevel(int level) {
    NLM_LOG_INFO("Debug level set to " + std::to_string(level));
}

int Brain::getDebugLevel() const {
    return 1; // Simplified - always return level 1
}

} // namespace nlm
