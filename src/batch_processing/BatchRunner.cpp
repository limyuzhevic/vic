#include "BatchUtils.hpp"

namespace nlm {

std::vector<double> BatchConfigGenerator::ParameterRange::generateValues(size_t count) const {
    std::vector<double> values;
    values.reserve(count);
    
    if (type == "logarithmic") {
        double logMin = std::log(min);
        double logMax = std::log(max);
        for (size_t i = 0; i < count; ++i) {
            double logVal = logMin + (logMax - logMin) * (i / static_cast<double>(count - 1));
            values.push_back(std::exp(logVal));
        }
    } else if (type == "exponential") {
        double logMin = std::log(min);
        double logMax = std::log(max);
        for (size_t i = 0; i < count; ++i) {
            double logVal = logMax - (logMax - logMin) * (i / static_cast<double>(count - 1));
            values.push_back(std::exp(logVal));
        }
    } else { // uniform
        for (size_t i = 0; i < count; ++i) {
            double val = min + (max - min) * (i / static_cast<double>(count - 1));
            values.push_back(val);
        }
    }
    
    // Override with custom values if provided
    if (!customValues.empty()) {
        values = customValues;
    }
    
    return values;
}

std::vector<std::map<std::string, double>> BatchConfigGenerator::generateParameterGrid(
    const std::vector<SweepConfig>& sweeps) {
    std::vector<std::map<std::string, double>> grid;
    
    if (sweeps.empty()) {
        return grid;
    }
    
    // Start with empty parameter set
    grid.push_back(std::map<std::string, double>());
    
    for (const auto& sweep : sweeps) {
        std::vector<std::map<std::string, double>> newGrid;
        std::vector<double> values = sweep.range.generateValues(sweep.iterations);
        
        for (const auto& params : grid) {
            for (double val : values) {
                std::map<std::string, double> newParams = params;
                newParams[sweep.paramName] = val;
                newGrid.push_back(newParams);
            }
        }
        
        grid = std::move(newGrid);
    }
    
    return grid;
}

std::vector<std::vector<size_t>> BatchConfigGenerator::generateCrossValidationFolds(
    size_t totalSamples, size_t numFolds, bool shuffle) {
    std::vector<size_t> indices(totalSamples);
    std::iota(indices.begin(), indices.end(), 0);
    
    if (shuffle) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(indices.begin(), indices.end(), gen);
    }
    
    std::vector<std::vector<size_t>> folds(numFolds);
    size_t samplesPerFold = totalSamples / numFolds;
    
    for (size_t i = 0; i < totalSamples; ++i) {
        size_t foldIndex = i / samplesPerFold;
        if (foldIndex >= numFolds) foldIndex = numFolds - 1;
        folds[foldIndex].push_back(indices[i]);
    }
    
    return folds;
}

std::vector<std::shared_ptr<Config>> BatchConfigGenerator::createBatchConfigurations(
    const std::vector<std::map<std::string, double>>& parameterGrid) {
    std::vector<std::shared_ptr<Config>> configs;
    configs.reserve(parameterGrid.size());
    
    for (const auto& params : parameterGrid) {
        auto config = std::make_shared<Config>();
        config->configure_for_task("research", std::vector<std::string>());
        
        // Apply parameters
        for (const auto& [key, value] : params) {
            // Note: This assumes config has a method to set values
            // The actual implementation depends on Config class interface
            config->clear();
        }
        
        configs.push_back(config);
    }
    
    return configs;
}

BatchSimulationRunner::BatchSimulationRunner(std::shared_ptr<Brain> brainTemplate,
                                             std::shared_ptr<Environment> environmentTemplate,
                                             BatchProgress& progress)
    : brainTemplate_(std::move(brainTemplate))
    , environmentTemplate_(std::move(environmentTemplate))
    , progress_(progress)
    , threadPool_(0) // Use default thread count
{
}

BatchSimulationResult BatchSimulationRunner::runSingleSimulation(
    size_t batchIndex,
    std::shared_ptr<Config> config,
    int timesteps) {
    return runSingleSimulationImpl(batchIndex, std::move(config), timesteps);
}

std::vector<BatchSimulationResult> BatchSimulationRunner::runParallelBatch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int timesteps,
    size_t maxThreads) {
    std::vector<BatchSimulationResult> results;
    results.reserve(configs.size());
    
    progress_ = BatchProgress(configs.size());
    
    if (maxThreads == 0) {
        maxThreads = threadPool_.getNumThreads();
    }
    
    // Create futures for parallel execution
    std::vector<std::future<BatchSimulationResult>> futures;
    futures.reserve(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        futures.push_back(threadPool_.submit(
            [this, i, config = configs[i], timesteps]() {
                return runSingleSimulationImpl(i, config, timesteps);
            }
        ));
    }
    
    // Collect results
    for (size_t i = 0; i < futures.size(); ++i) {
        try {
            results.push_back(futures[i].get());
        } catch (const std::exception& e) {
            BatchSimulationResult failedResult(i, nullptr, nullptr, "error", 0.0, "failed");
            failedResult.errorMessage = e.what();
            results.push_back(std::move(failedResult));
        }
    }
    
    return results;
}

std::vector<BatchSimulationResult> BatchSimulationRunner::runSequentialBatch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int timesteps) {
    std::vector<BatchSimulationResult> results;
    results.reserve(configs.size());
    
    progress_ = BatchProgress(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        progress_.currentTask = i;
        BatchSimulationResult result = runSingleSimulationImpl(i, configs[i], timesteps);
        results.push_back(std::move(result));
    }
    
    return results;
}

std::map<std::string, double> BatchSimulationRunner::getBatchPerformanceMetrics(
    const std::vector<BatchSimulationResult>& results) const {
    std::map<std::string, double> metrics;
    
    if (results.empty()) {
        return metrics;
    }
    
    double totalTime = 0;
    size_t successfulRuns = 0;
    double totalNeurons = 0;
    double totalSpikes = 0;
    
    for (const auto& result : results) {
        if (result.status == "completed") {
            totalTime += result.executionTime;
            successfulRuns++;
            if (result.brain) {
                totalNeurons += result.brain->getTotalNeuronCount();
                totalSpikes += result.brain->getTotalSpikeCount();
            }
        }
    }
    
    if (successfulRuns > 0) {
        metrics["mean_execution_time"] = totalTime / successfulRuns;
        metrics["success_rate"] = static_cast<double>(successfulRuns) / results.size();
        metrics["total_neurons_processed"] = totalNeurons / successfulRuns;
        metrics["total_spikes_generated"] = totalSpikes / successfulRuns;
        metrics["neurons_per_second"] = totalNeurons / (totalTime > 0 ? totalTime : 1);
        metrics["spikes_per_second"] = totalSpikes / (totalTime > 0 ? totalTime : 1);
    }
    
    return metrics;
}

BatchSimulationResult BatchSimulationRunner::runSingleSimulationImpl(
    size_t batchIndex,
    std::shared_ptr<Config> config,
    int timesteps) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    progress_.currentTask = batchIndex;
    
    try {
        // Create copies of template objects
        auto brain = std::make_shared<Brain>(brainTemplate_->getConfig());
        auto environment = std::make_shared<SimpleWorld>();
        
        // Configure with specific config
        brain->initialize();
        
        // Run simulation
        brain->run_simulation(timesteps, environment.get());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get metrics
        std::map<std::string, double> metrics;
        metrics["execution_time"] = executionTime;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        
        BatchSimulationResult result(batchIndex, brain, environment, 
                                   "simulation_" + std::to_string(batchIndex),
                                   executionTime, "completed");
        result.metrics = metrics;
        
        return result;
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BatchSimulationResult result(batchIndex, nullptr, nullptr,
                                   "simulation_" + std::to_string(batchIndex),
                                   executionTime, "failed");
        result.errorMessage = e.what();
        
        return result;
    }
}

std::vector<double> BatchConfigGenerator::ParameterRange::generateValues(size_t count) const {
    std::vector<double> values;
    values.reserve(count);
    
    if (type == "logarithmic") {
        double logMin = std::log(min);
        double logMax = std::log(max);
        for (size_t i = 0; i < count; ++i) {
            double logVal = logMin + (logMax - logMin) * (i / static_cast<double>(count - 1));
            values.push_back(std::exp(logVal));
        }
    } else if (type == "exponential") {
        double logMin = std::log(min);
        double logMax = std::log(max);
        for (size_t i = 0; i < count; ++i) {
            double logVal = logMax - (logMax - logMin) * (i / static_cast<double>(count - 1));
            values.push_back(std::exp(logVal));
        }
    } else { // uniform
        for (size_t i = 0; i < count; ++i) {
            double val = min + (max - min) * (i / static_cast<double>(count - 1));
            values.push_back(val);
        }
    }
    
    // Override with custom values if provided
    if (!customValues.empty()) {
        values = customValues;
    }
    
    return values;
}

std::vector<std::map<std::string, double>> BatchConfigGenerator::generateParameterGrid(
    const std::vector<SweepConfig>& sweeps) {
    std::vector<std::map<std::string, double>> grid;
    
    if (sweeps.empty()) {
        return grid;
    }
    
    // Start with empty parameter set
    grid.push_back(std::map<std::string, double>());
    
    for (const auto& sweep : sweeps) {
        std::vector<std::map<std::string, double>> newGrid;
        std::vector<double> values = sweep.range.generateValues(sweep.iterations);
        
        for (const auto& params : grid) {
            for (double val : values) {
                std::map<std::string, double> newParams = params;
                newParams[sweep.paramName] = val;
                newGrid.push_back(newParams);
            }
        }
        
        grid = std::move(newGrid);
    }
    
    return grid;
}

std::vector<std::vector<size_t>> BatchConfigGenerator::generateCrossValidationFolds(
    size_t totalSamples, size_t numFolds, bool shuffle) {
    std::vector<size_t> indices(totalSamples);
    std::iota(indices.begin(), indices.end(), 0);
    
    if (shuffle) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(indices.begin(), indices.end(), gen);
    }
    
    std::vector<std::vector<size_t>> folds(numFolds);
    size_t samplesPerFold = totalSamples / numFolds;
    
    for (size_t i = 0; i < totalSamples; ++i) {
        size_t foldIndex = i / samplesPerFold;
        if (foldIndex >= numFolds) foldIndex = numFolds - 1;
        folds[foldIndex].push_back(indices[i]);
    }
    
    return folds;
}

std::vector<std::shared_ptr<Config>> BatchConfigGenerator::createBatchConfigurations(
    const std::vector<std::map<std::string, double>>& parameterGrid) {
    std::vector<std::shared_ptr<Config>> configs;
    configs.reserve(parameterGrid.size());
    
    for (const auto& params : parameterGrid) {
        auto config = std::make_shared<Config>();
        config->configure_for_task("research", std::vector<std::string>());
        
        // Apply parameters
        for (const auto& [key, value] : params) {
            // Note: This assumes config has a method to set values
            // The actual implementation depends on Config class interface
            config->clear();
        }
        
        configs.push_back(config);
    }
    
    return configs;
}

BatchSimulationRunner::BatchSimulationRunner(std::shared_ptr<Brain> brainTemplate,
                                             std::shared_ptr<Environment> environmentTemplate,
                                             BatchProgress& progress)
    : brainTemplate_(std::move(brainTemplate))
    , environmentTemplate_(std::move(environmentTemplate))
    , progress_(progress)
    , threadPool_(0) // Use default thread count
{
}

BatchSimulationResult BatchSimulationRunner::runSingleSimulation(
    size_t batchIndex,
    std::shared_ptr<Config> config,
    int timesteps) {
    return runSingleSimulationImpl(batchIndex, std::move(config), timesteps);
}

std::vector<BatchSimulationResult> BatchSimulationRunner::runParallelBatch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int timesteps,
    size_t maxThreads) {
    std::vector<BatchSimulationResult> results;
    results.reserve(configs.size());
    
    progress_ = BatchProgress(configs.size());
    
    if (maxThreads == 0) {
        maxThreads = threadPool_.getNumThreads();
    }
    
    // Create futures for parallel execution
    std::vector<std::future<BatchSimulationResult>> futures;
    futures.reserve(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        futures.push_back(threadPool_.submit(
            [this, i, config = configs[i], timesteps]() {
                return runSingleSimulationImpl(i, config, timesteps);
            }
        ));
    }
    
    // Collect results
    for (size_t i = 0; i < futures.size(); ++i) {
        try {
            results.push_back(futures[i].get());
        } catch (const std::exception& e) {
            BatchSimulationResult failedResult(i, nullptr, nullptr, "error", 0.0, "failed");
            failedResult.errorMessage = e.what();
            results.push_back(std::move(failedResult));
        }
    }
    
    return results;
}

std::vector<BatchSimulationResult> BatchSimulationRunner::runSequentialBatch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int timesteps) {
    std::vector<BatchSimulationResult> results;
    results.reserve(configs.size());
    
    progress_ = BatchProgress(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        progress_.currentTask = i;
        BatchSimulationResult result = runSingleSimulationImpl(i, configs[i], timesteps);
        results.push_back(std::move(result));
    }
    
    return results;
}

std::map<std::string, double> BatchSimulationRunner::getBatchPerformanceMetrics(
    const std::vector<BatchSimulationResult>& results) const {
    std::map<std::string, double> metrics;
    
    if (results.empty()) {
        return metrics;
    }
    
    double totalTime = 0;
    size_t successfulRuns = 0;
    double totalNeurons = 0;
    double totalSpikes = 0;
    
    for (const auto& result : results) {
        if (result.status == "completed") {
            totalTime += result.executionTime;
            successfulRuns++;
            if (result.brain) {
                totalNeurons += result.brain->getTotalNeuronCount();
                totalSpikes += result.brain->getTotalSpikeCount();
            }
        }
    }
    
    if (successfulRuns > 0) {
        metrics["mean_execution_time"] = totalTime / successfulRuns;
        metrics["success_rate"] = static_cast<double>(successfulRuns) / results.size();
        metrics["total_neurons_processed"] = totalNeurons / successfulRuns;
        metrics["total_spikes_generated"] = totalSpikes / successfulRuns;
        metrics["neurons_per_second"] = totalNeurons / (totalTime > 0 ? totalTime : 1);
        metrics["spikes_per_second"] = totalSpikes / (totalTime > 0 ? totalTime : 1);
    }
    
    return metrics;
}

BatchSimulationResult BatchSimulationRunner::runSingleSimulationImpl(
    size_t batchIndex,
    std::shared_ptr<Config> config,
    int timesteps) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    progress_.currentTask = batchIndex;
    
    try {
        // Create copies of template objects
        auto brain = std::make_shared<Brain>(brainTemplate_->getConfig());
        auto environment = std::make_shared<SimpleWorld>();
        
        // Configure with specific config
        brain->initialize();
        
        // Run simulation
        brain->run_simulation(timesteps, environment.get());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get metrics
        std::map<std::string, double> metrics;
        metrics["execution_time"] = executionTime;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        
        BatchSimulationResult result(batchIndex, brain, environment, 
                                   "simulation_" + std::to_string(batchIndex),
                                   executionTime, "completed");
        result.metrics = metrics;
        
        return result;
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BatchSimulationResult result(batchIndex, nullptr, nullptr,
                                   "simulation_" + std::to_string(batchIndex),
                                   executionTime, "failed");
        result.errorMessage = e.what();
        
        return result;
    }
}

std::vector<double> BatchConfigGenerator::ParameterRange::generateValues(size_t count) const {
    std::vector<double> values;
    values.reserve(count);
    
    if (type == "logarithmic") {
        double logMin = std::log(min);
        double logMax = std::log(max);
        for (size_t i = 0; i < count; ++i) {
            double logVal = logMin + (logMax - logMin) * (i / static_cast<double>(count - 1));
            values.push_back(std::exp(logVal));
        }
    } else if (type == "exponential") {
        double logMin = std::log(min);
        double logMax = std::log(max);
        for (size_t i = 0; i < count; ++i) {
            double logVal = logMax - (logMax - logMin) * (i / static_cast<double>(count - 1));
            values.push_back(std::exp(logVal));
        }
    } else { // uniform
        for (size_t i = 0; i < count; ++i) {
            double val = min + (max - min) * (i / static_cast<double>(count - 1));
            values.push_back(val);
        }
    }
    
    // Override with custom values if provided
    if (!customValues.empty()) {
        values = customValues;
    }
    
    return values;
}

std::vector<std::map<std::string, double>> BatchConfigGenerator::generateParameterGrid(
    const std::vector<SweepConfig>& sweeps) {
    std::vector<std::map<std::string, double>> grid;
    
    if (sweeps.empty()) {
        return grid;
    }
    
    // Start with empty parameter set
    grid.push_back(std::map<std::string, double>());
    
    for (const auto& sweep : sweeps) {
        std::vector<std::map<std::string, double>> newGrid;
        std::vector<double> values = sweep.range.generateValues(sweep.iterations);
        
        for (const auto& params : grid) {
            for (double val : values) {
                std::map<std::string, double> newParams = params;
                newParams[sweep.paramName] = val;
                newGrid.push_back(newParams);
            }
        }
        
        grid = std::move(newGrid);
    }
    
    return grid;
}

std::vector<std::vector<size_t>> BatchConfigGenerator::generateCrossValidationFolds(
    size_t totalSamples, size_t numFolds, bool shuffle) {
    std::vector<size_t> indices(totalSamples);
    std::iota(indices.begin(), indices.end(), 0);
    
    if (shuffle) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(indices.begin(), indices.end(), gen);
    }
    
    std::vector<std::vector<size_t>> folds(numFolds);
    size_t samplesPerFold = totalSamples / numFolds;
    
    for (size_t i = 0; i < totalSamples; ++i) {
        size_t foldIndex = i / samplesPerFold;
        if (foldIndex >= numFolds) foldIndex = numFolds - 1;
        folds[foldIndex].push_back(indices[i]);
    }
    
    return folds;
}

std::vector<std::shared_ptr<Config>> BatchConfigGenerator::createBatchConfigurations(
    const std::vector<std::map<std::string, double>>& parameterGrid) {
    std::vector<std::shared_ptr<Config>> configs;
    configs.reserve(parameterGrid.size());
    
    for (const auto& params : parameterGrid) {
        auto config = std::make_shared<Config>();
        config->configure_for_task("research", std::vector<std::string>());
        
        // Apply parameters
        for (const auto& [key, value] : params) {
            // Note: This assumes config has a method to set values
            // The actual implementation depends on Config class interface
            config->clear();
        }
        
        configs.push_back(config);
    }
    
    return configs;
}

BatchSimulationRunner::BatchSimulationRunner(std::shared_ptr<Brain> brainTemplate,
                                             std::shared_ptr<Environment> environmentTemplate,
                                             BatchProgress& progress)
    : brainTemplate_(std::move(brainTemplate))
    , environmentTemplate_(std::move(environmentTemplate))
    , progress_(progress)
    , threadPool_(0) // Use default thread count
{
}

BatchSimulationResult BatchSimulationRunner::runSingleSimulation(
    size_t batchIndex,
    std::shared_ptr<Config> config,
    int timesteps) {
    return runSingleSimulationImpl(batchIndex, std::move(config), timesteps);
}

std::vector<BatchSimulationResult> BatchSimulationRunner::runParallelBatch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int timesteps,
    size_t maxThreads) {
    std::vector<BatchSimulationResult> results;
    results.reserve(configs.size());
    
    progress_ = BatchProgress(configs.size());
    
    if (maxThreads == 0) {
        maxThreads = threadPool_.getNumThreads();
    }
    
    // Create futures for parallel execution
    std::vector<std::future<BatchSimulationResult>> futures;
    futures.reserve(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        futures.push_back(threadPool_.submit(
            [this, i, config = configs[i], timesteps]() {
                return runSingleSimulationImpl(i, config, timesteps);
            }
        ));
    }
    
    // Collect results
    for (size_t i = 0; i < futures.size(); ++i) {
        try {
            results.push_back(futures[i].get());
        } catch (const std::exception& e) {
            BatchSimulationResult failedResult(i, nullptr, nullptr, "error", 0.0, "failed");
            failedResult.errorMessage = e.what();
            results.push_back(std::move(failedResult));
        }
    }
    
    return results;
}

std::vector<BatchSimulationResult> BatchSimulationRunner::runSequentialBatch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int timesteps) {
    std::vector<BatchSimulationResult> results;
    results.reserve(configs.size());
    
    progress_ = BatchProgress(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        progress_.currentTask = i;
        BatchSimulationResult result = runSingleSimulationImpl(i, configs[i], timesteps);
        results.push_back(std::move(result));
    }
    
    return results;
}

std::map<std::string, double> BatchSimulationRunner::getBatchPerformanceMetrics(
    const std::vector<BatchSimulationResult>& results) const {
    std::map<std::string, double> metrics;
    
    if (results.empty()) {
        return metrics;
    }
    
    double totalTime = 0;
    size_t successfulRuns = 0;
    double totalNeurons = 0;
    double totalSpikes = 0;
    
    for (const auto& result : results) {
        if (result.status == "completed") {
            totalTime += result.executionTime;
            successfulRuns++;
            if (result.brain) {
                totalNeurons += result.brain->getTotalNeuronCount();
                totalSpikes += result.brain->getTotalSpikeCount();
            }
        }
    }
    
    if (successfulRuns > 0) {
        metrics["mean_execution_time"] = totalTime / successfulRuns;
        metrics["success_rate"] = static_cast<double>(successfulRuns) / results.size();
        metrics["total_neurons_processed"] = totalNeurons / successfulRuns;
        metrics["total_spikes_generated"] = totalSpikes / successfulRuns;
        metrics["neurons_per_second"] = totalNeurons / (totalTime > 0 ? totalTime : 1);
        metrics["spikes_per_second"] = totalSpikes / (totalTime > 0 ? totalTime : 1);
    }
    
    return metrics;
}

BatchSimulationResult BatchSimulationRunner::runSingleSimulationImpl(
    size_t batchIndex,
    std::shared_ptr<Config> config,
    int timesteps) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    progress_.currentTask = batchIndex;
    
    try {
        // Create copies of template objects
        auto brain = std::make_shared<Brain>(brainTemplate_->getConfig());
        auto environment = std::make_shared<SimpleWorld>();
        
        // Configure with specific config
        brain->initialize();
        
        // Run simulation
        brain->run_simulation(timesteps, environment.get());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get metrics
        std::map<std::string, double> metrics;
        metrics["execution_time"] = executionTime;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        
        BatchSimulationResult result(batchIndex, brain, environment, 
                                   "simulation_" + std::to_string(batchIndex),
                                   executionTime, "completed");
        result.metrics = metrics;
        
        return result;
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        BatchSimulationResult result(batchIndex, nullptr, nullptr,
                                   "simulation_" + std::to_string(batchIndex),
                                   executionTime, "failed");
        result.errorMessage = e.what();
        
        return result;
    }
}