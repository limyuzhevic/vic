// Copyright (c) 2026 NLM System
// Data Export Tool Implementation - Main implementation file

#include "DataExportTool.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <zlib.h>
#include <nlohmann/json.hpp>

namespace nlm {

// Implementation structure
struct DataExportTool::Impl {
    // Configuration
    ExportConfig config;

    // NLM system pointers
    const Brain* brain{nullptr};
    const Metrics* metrics{nullptr};
    const Memory* memory{nullptr};
    std::vector<const Neuromodulator*> neuromodulators;
    const Observation* observation{nullptr};
    const Action* action{nullptr};
    const Config* systemConfig{nullptr};

    // Progress tracking
    ExportProgress progress;
    std::function<void(const ExportProgress&)> progressCallback;

    // Export writers
    std::unique_ptr<CSVExporter> csvExporter;
    std::unique_ptr<JSONExporter> jsonExporter;
    std::unique_ptr<BinaryExporter> binaryExporter;
    std::unique_ptr<ParquetExporter> parquetExporter;
    std::unique_ptr<HDF5Exporter> hdf5Exporter;

    // Validation
    std::vector<std::string> validationErrors;

    Impl() {
        progress.totalSteps = 0;
        progress.completedSteps = 0;
        progress.progressPercentage = 0.0f;
    }
};

// DataExportTool implementation
DataExportTool::DataExportTool() : pImpl(std::make_unique<Impl>()) {
    // Initialize export writers
    pImpl->csvExporter = std::make_unique<CSVExporter>();
    pImpl->jsonExporter = std::make_unique<JSONExporter>();
    pImpl->binaryExporter = std::make_unique<BinaryExporter>();
    // Other exporters can be initialized similarly
}

DataExportTool::~DataExportTool() = default;

bool DataExportTool::initialize(const ExportConfig& config) {
    pImpl->config = config;
    createDirectories(config.exportPath);

    // Initialize progress tracking
    pImpl->progress.totalSteps = 6; // Neural state, metrics, behavioral, memory, neuromodulators, metadata
    pImpl->progress.completedSteps = 0;
    pImpl->progress.progressPercentage = 0.0f;
    pImpl->progress.startTime = std::chrono::system_clock::now();

    return true;
}

void DataExportTool::shutdown() {
    pImpl->brain = nullptr;
    pImpl->metrics = nullptr;
    pImpl->memory = nullptr;
    pImpl->neuromodulators.clear();
    pImpl->observation = nullptr;
    pImpl->action = nullptr;
    pImpl->systemConfig = nullptr;

    pImpl->progressCallback = nullptr;
}

void DataExportTool::setBrain(const Brain* brain) {
    pImpl->brain = brain;
}

void DataExportTool::setMetrics(const Metrics* metrics) {
    pImpl->metrics = metrics;
}

void DataExportTool::setMemory(const Memory* memory) {
    pImpl->memory = memory;
}

void DataExportTool::setNeuromodulators(
    const std::vector<const Neuromodulator*>& modulators) {
    pImpl->neuromodulators = modulators;
}

void DataExportTool::setObservation(const Observation* observation) {
    pImpl->observation = observation;
}

void DataExportTool::setAction(const Action* action) {
    pImpl->action = action;
}

void DataExportTool::setConfig(const Config* config) {
    pImpl->systemConfig = config;
}

ExportProgress DataExportTool::getProgress() const {
    return pImpl->progress;
}

void DataExportTool::setProgressCallback(
    std::function<void(const ExportProgress&)> callback) {
    pImpl->progressCallback = callback;
}

void DataExportTool::updateProgress(const std::string& operation,
    size_t current, size_t total) {
    pImpl->progress.currentOperation = operation;
    pImpl->progress.totalSteps = total;
    pImpl->progress.completedSteps = current;
    pImpl->progress.progressPercentage =
        total > 0 ? (static_cast<float>(current) / total) * 100.0f : 0.0f;
    pImpl->progress.elapsedTime =
        std::chrono::system_clock::now() - pImpl->progress.startTime;

    if (pImpl->progressCallback) {
        pImpl->progressCallback(pImpl->progress);
    }
}

std::string DataExportTool::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

std::string DataExportTool::generateFilename(const std::string& prefix,
    ExportFormat format) {
    std::string ext;
    switch (format) {
    case ExportFormat::CSV:
        ext = ".csv";
        break;
    case ExportFormat::JSON:
        ext = ".json";
        break;
    case ExportFormat::Binary:
        ext = ".bin";
        break;
    case ExportFormat::Parquet:
        ext = ".parquet";
        break;
    case ExportFormat::HDF5:
        ext = ".h5";
        break;
    case ExportFormat::CompressedCSV:
        ext = ".csv.gz";
        break;
    case ExportFormat::CompressedJSON:
        ext = ".json.gz";
        break;
    case ExportFormat::CompressedBinary:
        ext = ".bin.gz";
        break;
    default:
        ext = ".dat";
    }

    return prefix.empty() ? ("export_" + getCurrentTimestamp() + ext) :
        prefix + ext;
}

void DataExportTool::createDirectories(const std::string& path) {
    std::filesystem::create_directories(path);
}

std::string DataExportTool::getFileExtension(ExportFormat format) {
    std::string ext;
    switch (format) {
    case ExportFormat::CSV:
        ext = ".csv";
        break;
    case ExportFormat::JSON:
        ext = ".json";
        break;
    case ExportFormat::Binary:
        ext = ".bin";
        break;
    case ExportFormat::Parquet:
        ext = ".parquet";
        break;
    case ExportFormat::HDF5:
        ext = ".h5";
        break;
    case ExportFormat::CompressedCSV:
        ext = ".csv.gz";
        break;
    case ExportFormat::CompressedJSON:
        ext = ".json.gz";
        break;
    case ExportFormat::CompressedBinary:
        ext = ".bin.gz";
        break;
    default:
        ext = ".dat";
    }
    return ext;
}

bool DataExportTool::exportCompleteData(const std::string& outputPath,
    ExportFormat format) {
    CompleteExportData data;
    data.config = pImpl->config;

    // Collect all data
    updateProgress("Collecting neural state data", 0, 6);
    collectNeuralStateData(data.neuralState);
    updateProgress("Collecting metrics data", 1, 6);
    collectMetricsData(data.metrics);
    updateProgress("Collecting behavioral data", 2, 6);
    collectBehavioralData(data.behavioralData);
    updateProgress("Collecting memory data", 3, 6);
    collectMemoryData(data.memoryData);
    updateProgress("Collecting neuromodulator data", 4, 6);
    collectNeuromodulatorData(data.neuromodulators);
    updateProgress("Collecting system info", 5, 6);
    collectSystemInfo(data.systemInfo);
    updateProgress("Writing export file", 6, 6);

    // Export to file
    bool success =
        exportToFile(data, outputPath, format);

    // Reset progress
    updateProgress("", 0, 0);

    return success;
}

bool DataExportTool::exportNeuralState(const std::string& outputPath,
    ExportFormat format) {
    NeuralConnectivityExportData data;

    updateProgress("Collecting neural state data", 0, 1);
    collectNeuralStateData(data);
    updateProgress("Writing neural state export", 1, 1);

    std::string fullPath = outputPath.empty() ?
        pImpl->config.exportPath + "/neural_state_" + getCurrentTimestamp() +
        getFileExtension(format) : outputPath;

    return exportToFile(data, fullPath, format);
}

bool DataExportTool::exportMetrics(const std::string& outputPath,
    ExportFormat format) {
    MetricsExportData data;

    updateProgress("Collecting metrics data", 0, 1);
    collectMetricsData(data);
    updateProgress("Writing metrics export", 1, 1);

    std::string fullPath = outputPath.empty() ?
        pImpl->config.exportPath + "/metrics_" + getCurrentTimestamp() +
        getFileExtension(format) : outputPath;

    return exportToFile(data, fullPath, format);
}

bool DataExportTool::exportBehavioralData(const std::string& outputPath,
    ExportFormat format) {
    BehavioralDataExportData data;

    updateProgress("Collecting behavioral data", 0, 1);
    collectBehavioralData(data);
    updateProgress("Writing behavioral data export", 1, 1);

    std::string fullPath = outputPath.empty() ?
        pImpl->config.exportPath + "/behavioral_" + getCurrentTimestamp() +
        getFileExtension(format) : outputPath;

    return exportToFile(data, fullPath, format);
}

bool DataExportTool::exportMemoryData(const std::string& outputPath,
    ExportFormat format) {
    MemoryExportData data;

    updateProgress("Collecting memory data", 0, 1);
    collectMemoryData(data);
    updateProgress("Writing memory export", 1, 1);

    std::string fullPath = outputPath.empty() ?
        pImpl->config.exportPath + "/memory_" + getCurrentTimestamp() +
        getFileExtension(format) : outputPath;

    return exportToFile(data, fullPath, format);
}

void DataExportTool::collectNeuralStateData(
    NeuralConnectivityExportData& data) const {
    if (!pImpl->brain) return;

    // Collect neurons
    const auto& regions = pImpl->brain->getRegions();
    for (const auto& region : regions) {
        NeuralRegionExportData regionData;
        regionData.id = region->getId();
        regionData.name = "Region_" + std::to_string(regionData.id.index());

        // Get all neurons in region
        auto neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            NeuronExportData neuronData;
            neuronData.id = neuron->getId();
            neuronData.type = neuron->getType();
            neuronData.state = neuron->getState();
            neuronData.regionId = regionData.id;
            neuronData.populationId = neuron->getPopulationId();
            neuronData.spikeHistory = neuron->getSpikeHistory();
            neuronData.plasticity = neuron->getPlasticityFlags();
            neuronData.adaptationVariable = neuron->getState().adaptationVariable;
            neuronData.totalCurrent = neuron->getTotalCurrent();

            regionData.neuronIds.push_back(neuronData.id);
            data.neurons.push_back(neuronData);
        }

        // Collect synapses
        auto synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            SynapseExportData synapseData;
            synapseData.id = synapse->getId();
            synapseData.sourceNeuron = synapse->getSourceNeuron();
            synapseData.destinationNeuron =
                synapse->getDestinationNeuron();
            synapseData.weight = synapse->getWeight();
            synapseData.delay = synapse->getDelay();
            synapseData.type = synapse->getType();
            synapseData.preSpikeTime = getCurrentTime(); // TODO: Get from synapse
            synapseData.postSpikeTime = getCurrentTime(); // TODO: Get from synapse
            synapseData.plasticity = synapse->getPlasticityFlags();
            synapseData.eligibilityTrace = synapse->getEligibilityTrace();
            synapseData.efficacy = synapse->getEfficacy();

            data.synapses.push_back(synapseData);
        }

        regionData.totalNeuronCount = neurons.size();
        regionData.activeNeuronCount = countActiveNeurons(neurons);
        regionData.firingNeuronCount = countFiringNeurons(neurons);
        regionData.averageFiringRate = calculateAverageFiringRate(neurons);

        data.regions.push_back(regionData);
    }

    // Collect inter-region connections
    data.excitationInhibitionRatio =
        pImpl->brain->getExcitationInhibitionRatio();
    data.totalSpikeCount = pImpl->brain->getTotalSpikeCount();
    data.pendingSpikeEvents =
        pImpl->brain->getPendingSpikeEventCount();
}

void DataExportTool::collectMetricsData(MetricsExportData& data) const {
    if (!pImpl->metrics) return;

    data.metricNames = pImpl->metrics->getMetricNames();

    for (const auto& name : data.metricNames) {
        data.metricValues[name] = pImpl->metrics->getValues(name);
        data.meanValues[name] = pImpl->metrics->getMean(name);
        data.stdDevValues[name] = pImpl->metrics->getStdDev(name);
        data.minValues[name] = pImpl->metrics->getMin(name);
        data.maxValues[name] = pImpl->metrics->getMax(name);
    }

    data.exportTime = std::chrono::system_clock::now();
    data.simulationTime = getSimulationTime(); // TODO: Get from simulation clock
}

void DataExportTool::collectBehavioralData(
    BehavioralDataExportData& data) const {
    if (!pImpl->observation) return;

    BehavioralObservationExportData observationData;
    observationData.timestamp = std::chrono::system_clock::now();
    observationData.step = getCurrentStep(); // TODO: Get from simulation clock
    observationData.simulationTime = getSimulationTime(); // TODO: Get from simulation clock

    // Get sensory data
    const auto* sensoryInput = pImpl->observation->getSensoryInput();
    if (sensoryInput) {
        observationData.sensoryData = sensoryInput->getData();
    }

    // Get action data
    if (pImpl->action) {
        observationData.actionType = pImpl->action->getType();
        observationData.actionParameters = pImpl->action->getParameters();
    }

    // Get neural information
    if (pImpl->brain) {
        auto neurons = pImpl->brain->getAllNeurons();
        for (const auto& neuron : neurons) {
            if (neuron->isFiring()) {
                observationData.activeNeurons.push_back(neuron->getId());
            }
        }
    }

    data.observations.push_back(observationData);

    // Calculate statistics
    data.totalActions = data.observations.size();
    if (!data.observations.empty()) {
        float totalReward = 0.0f;
        float totalPredictionError = 0.0f;
        std::map<ActionType, size_t> actionCounts;

        for (const auto& obs : data.observations) {
            totalReward += obs.reward;
            totalPredictionError += obs.predictionError;
            actionCounts[obs.actionType]++;
        }

        data.averageReward = totalReward / data.totalActions;
        data.averagePredictionError =
            totalPredictionError / data.totalActions;

        for (const auto& pair : actionCounts) {
            data.actionFrequencies[pair.first] =
                static_cast<float>(pair.second) / data.totalActions;
            data.actionNames.push_back(actionTypeToString(pair.first));
        }
    }
}

void DataExportTool::collectMemoryData(MemoryExportData& data) const {
    if (!pImpl->memory) return;

    // Working memory
    // TODO: Extract working memory data from memory system

    // Episodic memory
    data.episodicMemory = getEpisodicMemoryData(); // TODO: Implement

    // Semantic memory
    data.semanticMemory = getSemanticMemoryData(); // TODO: Implement

    // Procedural memory
    data.proceduralMemory = getProceduralMemoryData(); // TODO: Implement

    // Associative memory
    data.associations = getAssociativeMemoryData(); // TODO: Implement
    data.associationStrengths = getAssociationStrengthData(); // TODO: Implement
}

void DataExportTool::collectNeuromodulatorData(
    std::vector<NeuromodulatorExportData>& data) const {
    for (const auto& modulator : pImpl->neuromodulators) {
        NeuromodulatorExportData modData;
        modData.name = modulator->getName();
        modData.level = modulator->getLevel();
        modData.plasticityFactor = modulator->getPlasticityFactor();
        modData.lastUpdate = std::chrono::system_clock::now();

        data.push_back(modData);
    }
}

void DataExportTool::collectSystemInfo(
    std::map<std::string, std::string>& info) const {
    if (pImpl->systemConfig) {
        info["config_keys"] =
            std::to_string(pImpl->systemConfig->getKeys().size());
        info["config_summary"] = pImpl->systemConfig->summary();
    }

    info["export_tool_version"] = "1.0.0";
    info["nlm_version"] = "Phase 6";
    info["export_timestamp"] = getCurrentTimestamp();
}

bool DataExportTool::exportToFile(const CompleteExportData& data,
    const std::string& filepath, ExportFormat format) {
    createDirectories(std::filesystem::path(filepath).parent_path().string());

    switch (format) {
    case ExportFormat::CSV:
        return pImpl->csvExporter->export(data, filepath);
    case ExportFormat::JSON:
        return pImpl->jsonExporter->export(data, filepath);
    case ExportFormat::Binary:
        return pImpl->binaryExporter->export(data, filepath);
    case ExportFormat::Parquet:
        if (pImpl->parquetExporter) {
            return pImpl->parquetExporter->export(data, filepath);
        }
        break;
    case ExportFormat::HDF5:
        if (pImpl->hdf5Exporter) {
            return pImpl->hdf5Exporter->export(data, filepath);
        }
        break;
    case ExportFormat::CompressedCSV:
        return compressAndExport(pImpl->csvExporter->exportToString(data),
            filepath + ".gz");
    case ExportFormat::CompressedJSON:
        return compressAndExport(pImpl->jsonExporter->exportToString(data),
            filepath + ".gz");
    case ExportFormat::CompressedBinary:
        return compressAndExport(pImpl->binaryExporter->exportToString(data),
            filepath + ".gz");
    }

    return false;
}

bool DataExportTool::validateExportData() const {
    pImpl->validationErrors.clear();

    if (!pImpl->brain) {
        pImpl->validationErrors.push_back("Brain system not set");
    }

    if (pImpl->config.exportPath.empty()) {
        pImpl->validationErrors.push_back("Export path not specified");
    }

    return pImpl->validationErrors.empty();
}

std::string DataExportTool::getValidationErrors() const {
    std::stringstream ss;
    for (const auto& error : pImpl->validationErrors) {
        ss << error << "\n";
    }
    return ss.str();
}

// Helper functions
float DataExportTool::calculateAverageFiringRate(
    const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;

    float totalRate = 0.0f;
    for (const auto& neuron : neurons) {
        totalRate += neuron->getFiringRate();
    }

    return totalRate / neurons.size();
}

size_t DataExportTool::countActiveNeurons(
    const std::vector<Neuron*>& neurons) const {
    size_t count = 0;
    for (const auto& neuron : neurons) {
        if (neuron->isFiring()) count++;
    }
    return count;
}

size_t DataExportTool::countFiringNeurons(
    const std::vector<Neuron*>& neurons) const {
    size_t count = 0;
    for (const auto& neuron : neurons) {
        if (neuron->isFiring()) count++;
    }
    return count;
}

Timestamp DataExportTool::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::duration<double>>(now.time_since_epoch());
    return duration.count();
}

SimulationStep DataExportTool::getCurrentStep() const {
    // TODO: Get from simulation clock
    return 0;
}

double DataExportTool::getSimulationTime() const {
    // TODO: Get from simulation clock
    return 0.0;
}

bool DataExportTool::compressAndExport(const std::string& data,
    const std::string& filepath) {
    std::ofstream outFile(filepath, std::ios::binary);
    if (!outFile) return false;

    uLongf compressedSize = compressBound(data.size());
    std::vector<byte> compressedData(compressedSize);

    int result = compress2(compressedData.data(), &compressedSize,
        reinterpret_cast<const byte*>(data.data()), data.size(), Z_BEST_COMPRESSION);

    if (result != Z_OK) return false;

    outFile.write(reinterpret_cast<const char*>(compressedData.data()),
        compressedSize);
    outFile.close();

    return true;
}

// Enum conversion helpers
std::string DataExportTool::neuronTypeToString(NeuronType type) {
    switch (type) {
    case NeuronType::Excitatory: return "Excitatory";
    case NeuronType::Inhibitory: return "Inhibitory";
    case NeuronType::Modulatory: return "Modulatory";
    case NeuronType::Sensory: return "Sensory";
    case NeuronType::Motor: return "Motor";
    case NeuronType::Internal: return "Internal";
    default: return "Unknown";
    }
}

std::string DataExportTool::synapseTypeToString(SynapseType type) {
    switch (type) {
    case SynapseType::Excitatory: return "Excitatory";
    case SynapseType::Inhibitory: return "Inhibitory";
    case SynapseType::Modulatory: return "Modulatory";
    case SynapseType::Electrical: return "Electrical";
    case SynapseType::GapJunction: return "GapJunction";
    default: return "Unknown";
    }
}

std::string DataExportTool::actionTypeToString(ActionType type) {
    switch (type) {
    case ActionType::MoveForward: return "MoveForward";
    case ActionType::MoveBackward: return "MoveBackward";
    case ActionType::MoveLeft: return "MoveLeft";
    case ActionType::MoveRight: return "MoveRight";
    case ActionType::TurnLeft: return "TurnLeft";
    case ActionType::TurnRight: return "TurnRight";
    case ActionType::Look: return "Look";
    case ActionType::LookUp: return "LookUp";
    case ActionType::LookDown: return "LookDown";
    case ActionType::Interact: return "Interact";
    case ActionType::Eat: return "Eat";
    case ActionType::Drink: return "Drink";
    case ActionType::Rest: return "Rest";
    case ActionType::Wait: return "Wait";
    case ActionType::Custom: return "Custom";
    default: return "Unknown";
    }
}

} // namespace nlm
