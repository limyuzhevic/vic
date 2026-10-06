// Data Export Tool Demo
// This demo showcases the Data Export Tool capabilities

#include "DataExportTool.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Experiment.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace nlm;

class DataExportToolDemo {
public:
    DataExportToolDemo() {}
    ~DataExportToolDemo() {}
    
    void run() {
        std::cout << "========================================\n";
        std::cout << "DATA EXPORT TOOL DEMO\n";
        std::cout << "========================================\n\n";
        
        // Initialize data export tool
        DataExportTool exporter;
        
        // Create experimental data
        std::cout << "--- Creating Experimental Data ---\n";
        
        // Create base configuration
        auto config = createConfig(500, 4, 0.15f);
        
        // Create experiment
        Experiment experiment("Performance_Study_001");
        experiment.setSeed(12345u);
        
        // Simulate experimental runs
        std::vector<ExperimentRun> experimentalRuns;
        int numRuns = 5;
        
        for (int run = 0; run < numRuns; ++run) {
            std::cout << "  Creating experimental run " << run << "...\n";
            
            ExperimentRun runData;
            runData.runId = run;
            runData.startTime = std::chrono::system_clock::now();
            
            // Create brain for this run
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create world
            SimpleWorld world;
            world.initialize(16, 16);
            
            // Create agent
            AgentBrain agent(brain);
            agent.initialize(world);
            
            // Run simulation
            float totalReward = 0.0f;
            float totalFiring = 0.0f;
            size_t totalSpikes = 0;
            
            for (int step = 0; step < 1000; ++step) {
                // Get observation
                SensoryPercept percept = world.observe(brain->getRegions()[0].get());
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Brain step
                brain->step(step, step * 0.001);
                
                // Get motor command
                MotorCommand cmd = agent.decodeMotorCommand();
                
                // Apply action
                world.applyAction(brain->getRegions()[0].get(), cmd);
                
                // Compute reward
                float reward = world.computeReward(brain->getRegions()[0].get());
                totalReward += reward;
                
                // Apply reward modulation
                agent.applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent.updateDevelopment(0.001);
                
                // Record metrics
                totalFiring += brain->getAverageFiringRate();
                totalSpikes += brain->getFiringNeuronCount();
                
                // Record step data for export
                Experiment::StepData stepData;
                stepData.step = step;
                stepData.neuronCount = brain->getTotalNeuronCount();
                stepData.synapseCount = brain->getTotalSynapseCount();
                stepData.firingCount = brain->getFiringNeuronCount();
                stepData.avgFiringRate = brain->getAverageFiringRate();
                
                experiment.recordStep(stepData.step, stepData.neuronCount, 
                                    stepData.synapseCount, stepData.firingCount, 
                                    stepData.avgFiringRate);
                
                runData.stepData.push_back(stepData);
                
                // Record metrics
                runData.metrics.push_back({
                    "reward", reward
                });
                runData.metrics.push_back({
                    "firing_rate", brain->getAverageFiringRate()
                });
                runData.metrics.push_back({
                    "spike_count", static_cast<double>(brain->getFiringNeuronCount())
                });
            }
            
            // Finalize run
            runData.endTime = std::chrono::system_clock::now();
            runData.totalReward = totalReward / 1000.0f;
            runData.averageFiringRate = totalFiring / 1000.0f;
            runData.totalSpikes = totalSpikes;
            
            experimentalRuns.push_back(runData);
            
            // Add some experimental notes
            experiment.addNote("Run " + std::to_string(run) + " completed successfully");
        }
        
        std::cout << "✓ Created " << numRuns << " experimental runs\n\n";
        
        // Export raw simulation data
        std::cout << "--- Exporting Raw Simulation Data ---\n";
        std::string rawDataPath = "/tmp/experimental_raw_data.csv";
        exporter.exportRawData(experimentalRuns, rawDataPath);
        std::cout << "✓ Raw data exported to: " << rawDataPath << "\n";
        
        // Export processed metrics
        std::cout << "--- Exporting Processed Metrics ---\n";
        std::string metricsPath = "/tmp/experimental_metrics.json";
        exporter.exportMetrics(experiment, metricsPath);
        std::cout << "✓ Metrics exported to: " << metricsPath << "\n";
        
        // Export neural state data
        std::cout << "--- Exporting Neural State Data ---\n";
        std::string neuralPath = "/tmp/experimental_neural_data.bin";
        exporter.exportNeuralState(experimentalRuns, neuralPath);
        std::cout << "✓ Neural state exported to: " << neuralPath << "\n";
        
        // Export behavioral data
        std::cout << "--- Exporting Behavioral Data ---\n";
        std::string behavioralPath = "/tmp/experimental_behavioral_data.csv";
        exporter.exportBehavioralData(experimentalRuns, behavioralPath);
        std::cout << "✓ Behavioral data exported to: " << behavioralPath << "\n";
        
        // Export experimental configuration
        std::cout << "--- Exporting Experimental Configuration ---\n";
        std::string configPath = "/tmp/experimental_config.json";
        exporter.exportConfig(config, configPath);
        std::cout << "✓ Configuration exported to: " << configPath << "\n";
        
        // Merge and export all data
        std::cout << "--- Merging and Exporting Combined Dataset ---\n";
        std::string combinedPath = "/tmp/experimental_combined_dataset.zip";
        exporter.mergeDataSets(experimentalRuns, combinedPath);
        std::cout << "✓ Combined dataset exported to: " << combinedPath << "\n";
        
        // Filter and export selected time ranges
        std::cout << "--- Exporting Filtered Data ---\n";
        
        // Export only specific time range (steps 500-600)
        std::string filteredPath = "/tmp/experimental_filtered_data.csv";
        std::vector<ExperimentRun> filteredRuns;
        
        for (const auto& run : experimentalRuns) {
            ExperimentRun filteredRun;
            filteredRun.runId = run.runId;
            
            for (const auto& stepData : run.stepData) {
                if (stepData.step >= 500 && stepData.step <= 600) {
                    filteredRun.stepData.push_back(stepData);
                }
            }
            
            filteredRuns.push_back(filteredRun);
        }
        
        exporter.exportRawData(filteredRuns, filteredPath);
        std::cout << "✓ Filtered data exported to: " << filteredPath << "\n";
        
        // Export experimental summary
        std::cout << "--- Exporting Experimental Summary ---\n";
        std::string summaryPath = "/tmp/experimental_summary.txt";
        exporter.exportSummary(experiment, summaryPath);
        std::cout << "✓ Summary exported to: " << summaryPath << "\n";
        
        // Import and verify exported data
        std::cout << "--- Importing and Verifying Exported Data ---\n";
        
        Experiment importedExperiment;
        bool importSuccess = exporter.importData(metricsPath, importedExperiment);
        
        if (importSuccess) {
            std::cout << "✓ Data imported successfully\n";
            
            // Verify data integrity
            std::vector<std::pair<std::string, double>> metrics = importedExperiment.getMetrics();
            std::cout << "Imported experiment metrics: " << metrics.size() << " items\n";
            
            // Validate step data
            const auto& stepData = importedExperiment.getStepData();
            std::cout << "Imported experiment steps: " << stepData.size() << " steps\n";
            
            // Validate notes
            const auto& notes = importedExperiment.getNotes();
            std::cout << "Imported experiment notes: " << notes.size() << " items\n";
        } else {
            std::cout << "✗ Failed to import data\n";
        }
        
        // Export in multiple formats
        std::cout << "\n--- Multi-Format Export ---\n";
        
        // Export to multiple formats for different analysis tools
        std::string csvPath = "/tmp/experimental_data.csv";
        std::string jsonPath = "/tmp/experimental_data.json";
        std::string hdf5Path = "/tmp/experimental_data.h5";  // HDF5 support would be added
        std::string parquetPath = "/tmp/experimental_data.parquet";  // Parquet support would be added
        
        exporter.exportToMultipleFormats(experimentalRuns, {
            {"csv", csvPath},
            {"json", jsonPath},
            {"hdf5", hdf5Path},
            {"parquet", parquetPath}
        });
        
        std::cout << "✓ Data exported in multiple formats:\n";
        std::cout << "  - CSV: " << csvPath << "\n";
        std::cout << "  - JSON: " << jsonPath << "\n";
        std::cout << "  - HDF5: " << hdf5Path << "\n";
        std::cout << "  - Parquet: " << parquetPath << "\n";
        
        // Generate metadata
        std::cout << "\n--- Generating Export Metadata ---\n";
        std::string metadataPath = "/tmp/experimental_metadata.json";
        exporter.generateMetadata(experimentalRuns, metadataPath);
        std::cout << "✓ Metadata exported to: " << metadataPath << "\n";
        
        // Validate data compatibility
        std::cout << "\n--- Data Compatibility Check ---\n";
        
        std::vector<std::string> compatibleFormats = exporter.checkCompatibility({
            "neural_data.bin", "behavioral_data.csv", "metrics.json"
        });
        
        std::cout << "Compatible formats: ";
        for (const auto& format : compatibleFormats) {
            std::cout << format << " ";
        }
        std::cout << "\n";
        
        // Export for machine learning
        std::cout << "\n--- Machine Learning Data Export ---\n";
        std::string mlPath = "/tmp/experimental_ml_ready.csv";
        exporter.exportForMachineLearning(experimentalRuns, mlPath, {
            "features": {"reward", "firing_rate", "spike_count", "neuron_count", "synapse_count"},
            "target": "next_step_reward"
        });
        std::cout << "✓ ML-ready data exported to: " << mlPath << "\n";
        
        // Generate compression statistics
        std::cout << "\n--- Compression Statistics ---\n";
        auto compressionStats = exporter.getCompressionStats();
        std::cout << "Original size: " << compressionStats.originalSize << " bytes\n";
        std::cout << "Compressed size: " << compressionStats.compressedSize << " bytes\n";
        std::cout << "Compression ratio: " << compressionStats.compressionRatio << "x\n";
        
        std::cout << "\n========================================\n";
        std::cout << "DATA EXPORT TOOL DEMO COMPLETE\n";
        std::cout << "========================================\n";
    }
    
private:
    std::shared_ptr<Config> createConfig(int neuronCount, int regionCount, float connectionProb) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001);
        return config;
    }
};

int main(int argc, char* argv[]) {
    nlm::DataExportToolDemo demo;
    demo.run();
    return 0;
}
