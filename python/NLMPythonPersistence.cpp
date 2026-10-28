// Model persistence and checkpointing utilities implementation
#include "NLMPythonPersistence.h"
#include "NLMPythonTypes.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

// Brain checkpoint manager
class BrainCheckpoint {
public:
    struct CheckpointInfo {
        std::string filepath;
        std::string timestamp;
        size_t totalSpikes;
        size_t firingNeurons;
        size_t totalSteps;
        double simulationTime;
        std::map<std::string, std::string> metadata;
    };
    
    std::shared_ptr<Brain> brain;
    std::string baseDir;
    
    BrainCheckpoint(std::shared_ptr<Brain> b, const std::string& directory = "checkpoints")
        : brain(b), baseDir(directory) {
        if (!fs::exists(baseDir)) {
            fs::create_directories(baseDir);
        }
    }
    
    // Save current brain state as checkpoint
    std::string saveCheckpoint(const std::string& description = "") {
        if (!brain) return "";
        
        auto timestamp = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        std::string timestampStr = ss.str();
        
        std::string filename = "checkpoint_" + timestampStr + ".bin";
        std::string filepath = baseDir + "/" + filename;
        
        // Save brain state
        brain->save(filepath);
        
        // Create metadata
        CheckpointInfo info;
        info.filepath = filepath;
        info.timestamp = timestampStr;
        info.totalSpikes = brain->getTotalSpikeCount();
        info.firingNeurons = brain->getFiringNeuronCount();
        info.totalSteps = 1000; // TODO: get from simulation state
        info.simulationTime = info.totalSteps * 0.001; // TODO: get from simulation clock
        info.metadata["description"] = description;
        info.metadata["version"] = "0.1.0";
        
        // Save metadata
        std::string metadataFile = filepath + ".meta";
        std::ofstream metaOut(metadataFile);
        metaOut << "filepath: " << info.filepath << "\n";
        metaOut << "timestamp: " << info.timestamp << "\n";
        metaOut << "total_spikes: " << info.totalSpikes << "\n";
        metaOut << "firing_neurons: " << info.firingNeurons << "\n";
        metaOut << "total_steps: " << info.totalSteps << "\n";
        metaOut << "simulation_time: " << info.simulationTime << "\n";
        metaOut << "description: " << description << "\n";
        metaOut.close();
        
        return filename;
    }
    
    // Load brain state from checkpoint
    bool loadCheckpoint(const std::string& filename) {
        if (!brain) return false;
        
        std::string filepath = baseDir + "/" + filename;
        
        // Check if checkpoint exists
        if (!fs::exists(filepath)) {
            return false;
        }
        
        // Load brain state
        bool success = brain->load(filepath);
        
        // Load metadata if available
        std::string metadataFile = filepath + ".meta";
        if (fs::exists(metadataFile)) {
            std::ifstream metaIn(metadataFile);
            std::string line;
            while (std::getline(metaIn, line)) {
                if (line.find("description:") != std::string::npos) {
                    // Parse description from metadata
                }
            }
            metaIn.close();
        }
        
        return success;
    }
    
    // Get list of available checkpoints
    std::vector<std::string> listCheckpoints() {
        std::vector<std::string> checkpoints;
        
        if (!fs::exists(baseDir)) {
            return checkpoints;
        }
        
        for (const auto& entry : fs::directory_iterator(baseDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                checkpoints.push_back(entry.path().filename().string());
            }
        }
        
        return checkpoints;
    }
    
    // Delete checkpoint
    bool deleteCheckpoint(const std::string& filename) {
        std::string filepath = baseDir + "/" + filename;
        
        if (!fs::exists(filepath)) {
            return false;
        }
        
        // Delete checkpoint and metadata
        fs::remove(filepath);
        fs::remove(filepath + ".meta");
        
        return true;
    }
    
    // Get checkpoint info
    CheckpointInfo getCheckpointInfo(const std::string& filename) {
        CheckpointInfo info;
        std::string filepath = baseDir + "/" + filename;
        std::string metadataFile = filepath + ".meta";
        
        if (fs::exists(metadataFile)) {
            std::ifstream metaIn(metadataFile);
            std::string line;
            while (std::getline(metaIn, line)) {
                if (line.find("filepath:") != std::string::npos) {
                    info.filepath = line.substr(10);
                } else if (line.find("timestamp:") != std::string::npos) {
                    info.timestamp = line.substr(11);
                } else if (line.find("total_spikes:") != std::string::npos) {
                    info.totalSpikes = std::stoul(line.substr(13));
                } else if (line.find("firing_neurons:") != std::string::npos) {
                    info.firingNeurons = std::stoul(line.substr(16));
                } else if (line.find("total_steps:") != std::string::npos) {
                    info.totalSteps = std::stoul(line.substr(12));
                } else if (line.find("simulation_time:") != std::string::npos) {
                    info.simulationTime = std::stod(line.substr(16));
                } else if (line.find("description:") != std::string::npos) {
                    info.metadata["description"] = line.substr(13);
                }
            }
            metaIn.close();
        }
        
        return info;
    }
    
    // Get the underlying brain
    std::shared_ptr<Brain> getBrain() const { return brain; }
};

// Model persistence utilities
class ModelPersistence {
public:
    // Save complete simulation state (brain, world, agent)
    static bool saveCompleteState(const std::string& basePath,
                                  std::shared_ptr<Brain> brain,
                                  std::shared_ptr<SimpleWorld> world = nullptr,
                                  std::shared_ptr<AgentBrain> agent = nullptr,
                                  const std::string& description = "") {
        try {
            if (!fs::exists(basePath)) {
                fs::create_directories(basePath);
            }
            
            // Save brain state
            if (brain) {
                brain->save(basePath + "/brain.bin");
            }
            
            // Save world state
            if (world) {
                // TODO: Add world save method
                // world->save(basePath + "/world.bin");
            }
            
            // Save agent state
            if (agent) {
                // TODO: Add agent save method
                // agent->save(basePath + "/agent.bin");
            }
            
            // Save configuration
            if (brain) {
                auto config = brain->getConfig();
                config->saveToFile(basePath + "/config.json");
            }
            
            // Save metadata
            std::ofstream metaFile(basePath + "/metadata.txt");
            metaFile << "Description: " << description << "\n";
            metaFile << "Saved: " << std::chrono::system_clock::now() << "\n";
            metaFile << "Brain neurons: " << (brain ? brain->getTotalNeuronCount() : 0) << "\n";
            metaFile << "Brain synapses: " << (brain ? brain->getTotalSynapseCount() : 0) << "\n";
            if (world) {
                metaFile << "World width: " << world->getWidth() << "\n";
                metaFile << "World height: " << world->getHeight() << "\n";
            }
            metaFile.close();
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Load complete simulation state
    static bool loadCompleteState(const std::string& basePath,
                                  std::shared_ptr<Brain>& brain,
                                  std::shared_ptr<SimpleWorld>& world = nullptr,
                                  std::shared_ptr<AgentBrain>& agent = nullptr) {
        try {
            // Check if files exist
            if (!fs::exists(basePath + "/brain.bin") || !fs::exists(basePath + "/config.json")) {
                return false;
            }
            
            // Load configuration
            auto config = std::make_shared<Config>();
            config->loadFromFile(basePath + "/config.json");
            
            // Create brain with loaded config
            brain = std::make_shared<Brain>(config);
            brain->load(basePath + "/brain.bin");
            
            // Load world if available
            if (world && fs::exists(basePath + "/world.bin")) {
                // TODO: Add world load method
                // world->load(basePath + "/world.bin");
            }
            
            // Load agent if available
            if (agent && fs::exists(basePath + "/agent.bin")) {
                // TODO: Add agent load method
                // agent->load(basePath + "/agent.bin");
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Create experiment directory structure
    static std::string createExperimentDirectory(const std::string& experimentName,
                                                  const std::string& baseDir = "experiments") {
        auto timestamp = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        std::string timestampStr = ss.str();
        
        std::string experimentPath = baseDir + "/" + experimentName + "_" + timestampStr;
        
        if (!fs::exists(baseDir)) {
            fs::create_directories(baseDir);
        }
        
        fs::create_directories(experimentPath);
        
        // Create subdirectories
        fs::create_directories(experimentPath + "/checkpoints");
        fs::create_directories(experimentPath + "/logs");
        fs::create_directories(experimentPath + "/metrics");
        
        // Create experiment metadata file
        std::ofstream metaFile(experimentPath + "/experiment.json");
        metaFile << "{\n";
        metaFile << "  \"name\": \"" << experimentName << "\",\n";
        metaFile << "  \"created_at\": \"" << timestampStr << "\",\n";
        metaFile << "  \"brain_neuron_count\": 0,\n";
        metaFile << "  \"brain_synapse_count\": 0,\n";
        metaFile << "  \"description\": \"Created experiment \" + experimentName + "\",\n";
        metaFile << "  \"tags\": [],\n";
        metaFile << "  \"status\": \"active\"\n";
        metaFile << "}\n";
        metaFile.close();
        
        return experimentPath;
    }
    
    // Export simulation results as JSON
    static bool exportResultsAsJSON(const std::string& outputPath,
                                    const std::string& experimentName,
                                    const std::vector<SimulationStats::Stats>& statistics,
                                    const std::vector<std::string>& checkpoints) {
        try {
            std::ofstream jsonFile(outputPath);
            jsonFile << "{\n";
            jsonFile << "  \"experiment_name\": \"" << experimentName << "\",\n";
            jsonFile << "  \"export_time\": \"" << std::chrono::system_clock::now() << "\",\n";
            jsonFile << "  \"statistics\": [\n";
            
            for (size_t i = 0; i < statistics.size(); ++i) {
                const auto& stats = statistics[i];
                jsonFile << "    {\n";
                jsonFile << "      \"step\": " << stats.steps << ",\n";
                jsonFile << "      \"total_spikes\": " << stats.totalSpikes << ",\n";
                jsonFile << "      \"firing_neurons\": " << stats.firingNeurons << ",\n";
                jsonFile << "      \"average_firing_rate\": " << stats.averageFiringRate << ",\n";
                jsonFile << "      \"time_elapsed\": " << stats.timeElapsed << ",\n";
                jsonFile << "      \"spike_history\": {\n";
                
                size_t count = 0;
                for (const auto& pair : stats.spikeHistory) {
                    jsonFile << "        \"" << pair.first << "\": " << pair.second;
                    if (++count < stats.spikeHistory.size()) {
                        jsonFile << ",\n";
                    } else {
                        jsonFile << "\n";
                    }
                }
                
                jsonFile << "      }\n";
                jsonFile << "    }";
                if (i < statistics.size() - 1) {
                    jsonFile << ",\n";
                } else {
                    jsonFile << "\n";
                }
            }
            
            jsonFile << "  ],\n";
            jsonFile << "  \"checkpoints\": [\n";
            
            for (size_t i = 0; i < checkpoints.size(); ++i) {
                jsonFile << "    \"" << checkpoints[i] << "\"";
                if (i < checkpoints.size() - 1) {
                    jsonFile << ",\n";
                } else {
                    jsonFile << "\n";
                }
            }
            
            jsonFile << "  ]\n";
            jsonFile << "}\n";
            jsonFile.close();
            
            return true;
        } catch (...) {
            return false;
        }
    }
};

void registerPersistenceUtils(py::module_& m) {
    // Brain checkpoint manager
    py::class_<BrainCheckpoint>(m, "BrainCheckpoint", 
                                "Manage brain checkpoints and saved states")
        .def(py::init<std::shared_ptr<Brain>, std::string>(),
             py::arg("brain"), py::arg("directory") = "checkpoints",
             "Initialize checkpoint manager with brain and directory")
        .def("save_checkpoint", &BrainCheckpoint::saveCheckpoint,
             py::arg("description") = "",
             "Save current brain state as a checkpoint")
        .def("load_checkpoint", &BrainCheckpoint::loadCheckpoint,
             py::arg("filename"),
             "Load brain state from checkpoint")
        .def("list_checkpoints", &BrainCheckpoint::listCheckpoints,
             "Get list of available checkpoints")
        .def("delete_checkpoint", &BrainCheckpoint::deleteCheckpoint,
             py::arg("filename"),
             "Delete a checkpoint")
        .def("get_checkpoint_info", &BrainCheckpoint::getCheckpointInfo,
             py::arg("filename"),
             "Get information about a checkpoint")
        .def("get_brain", &BrainCheckpoint::getBrain,
             "Get the underlying brain instance");
    
    // Model persistence utilities
    py::class_<ModelPersistence>(m, "ModelPersistence", 
                                 "Utilities for saving and loading complete simulation states")
        .def_static("save_complete_state", &ModelPersistence::saveCompleteState,
                    py::arg("basePath"), py::arg("brain"), py::arg("world") = nullptr,
                    py::arg("agent") = nullptr, py::arg("description") = "",
                    "Save complete simulation state including brain, world, and agent")
        .def_static("load_complete_state", &ModelPersistence::loadCompleteState,
                    py::arg("basePath"), py::arg("brain"), py::arg("world") = nullptr,
                    py::arg("agent") = nullptr,
                    "Load complete simulation state from files")
        .def_static("create_experiment_directory", &ModelPersistence::createExperimentDirectory,
                    py::arg("experimentName"), py::arg("baseDir") = "experiments",
                    "Create a new experiment directory with standard structure")
        .def_static("export_results_as_json", &ModelPersistence::exportResultsAsJSON,
                    py::arg("outputPath"), py::arg("experimentName"),
                    py::arg("statistics"), py::arg("checkpoints"),
                    "Export simulation results as JSON file");
    
    // Convenience functions for persistence
    m.def("save_simulation", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        return brain->save(filename);
    }, py::arg("brain"), py::arg("filename"),
       "Save brain state to file (convenience function)");

    m.def("load_simulation", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        return brain->load(filename);
    }, py::arg("brain"), py::arg("filename"),
       "Load brain state from file (convenience function)");

    m.def("create_checkpoint", [](std::shared_ptr<Brain> brain, const std::string& directory) {
        return BrainCheckpoint(brain, directory);
    }, py::arg("brain"), py::arg("directory") = "checkpoints",
       "Create a checkpoint manager for the brain (convenience function)");

    m.def("export_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Export brain state to file (convenience function)");
}
