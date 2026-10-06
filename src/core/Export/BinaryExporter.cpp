// Copyright (c) 2026 NLM System
// BinaryExporter implementation for NLM Data Export Tool

#include "BinaryExporter.hpp"
#include <fstream>
#include <iostream>
#include <cstring>

namespace nlm {

bool DataExportTool::BinaryExporter::export(
    const CompleteExportData& data, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) return false;

    // Write binary header
    writeBinaryHeader(file, data);

    // Write neural data
    writeNeuralDataBinary(file, data.neuralState);

    // Write metrics data
    writeMetricsDataBinary(file, data.metrics);

    // Write behavioral data
    writeBehavioralDataBinary(file, data.behavioralData);

    // Write memory data
    writeMemoryDataBinary(file, data.memoryData);

    // Write neuromodulator data
    writeNeuromodulatorDataBinary(file, data.neuromodulators);

    // Write system info
    writeSystemInfoBinary(file, data.systemInfo);

    file.close();
    return true;
}

std::string DataExportTool::BinaryExporter::exportToString(
    const CompleteExportData& data) {
    std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
    writeBinaryHeader(ss, data);
    writeNeuralDataBinary(ss, data.neuralState);
    writeMetricsDataBinary(ss, data.metrics);
    writeBehavioralDataBinary(ss, data.behavioralData);
    writeMemoryDataBinary(ss, data.memoryData);
    writeNeuromodulatorDataBinary(ss, data.neuromodulators);
    writeSystemInfoBinary(ss, data.systemInfo);
    return ss.str();
}

void DataExportTool::BinaryExporter::writeBinaryHeader(
    std::ostream& stream, const CompleteExportData& data) {
    const char header[] = "NLM_EXPORT_V1.0";
    stream.write(header, sizeof(header));

    // Write export version
    uint32_t version = 1;
    stream.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // Write timestamp
    auto time = std::chrono::system_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
    auto timestamp = duration.count();
    stream.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
}

void DataExportTool::BinaryExporter::writeNeuralDataBinary(
    std::ostream& stream, const NeuralConnectivityExportData& data) {
    // Write number of neurons
    uint64_t neuronCount = data.neurons.size();
    stream.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));

    // Write neuron data
    for (const auto& neuron : data.neurons) {
        writeNeuronBinary(stream, neuron);
    }

    // Write number of synapses
    uint64_t synapseCount = data.synapses.size();
    stream.write(reinterpret_cast<const char*>(&synapseCount), sizeof(synapseCount));

    // Write synapse data
    for (const auto& synapse : data.synapses) {
        writeSynapseBinary(stream, synapse);
    }

    // Write number of regions
    uint64_t regionCount = data.regions.size();
    stream.write(reinterpret_cast<const char*>(&regionCount), sizeof(regionCount));

    // Write region data
    for (const auto& region : data.regions) {
        writeRegionBinary(stream, region);
    }
}

void DataExportTool::BinaryExporter::writeMetricsDataBinary(
    std::ostream& stream, const MetricsExportData& data) {
    // Write number of metrics
    uint64_t metricCount = data.metricNames.size();
    stream.write(reinterpret_cast<const char*>(&metricCount), sizeof(metricCount));

    // Write metric names and values
    for (const auto& name : data.metricNames) {
        writeStringBinary(stream, name);

        const auto& values = data.metricValues.at(name);
        uint64_t valueCount = values.size();
        stream.write(reinterpret_cast<const char*>(&valueCount), sizeof(valueCount));

        stream.write(reinterpret_cast<const char*>(values.data()),
            values.size() * sizeof(double));
    }
}

void DataExportTool::BinaryExporter::writeBehavioralDataBinary(
    std::ostream& stream, const BehavioralDataExportData& data) {
    // Write number of observations
    uint64_t observationCount = data.observations.size();
    stream.write(reinterpret_cast<const char*>(&observationCount), sizeof(observationCount));

    // Write observation data
    for (const auto& obs : data.observations) {
        writeBehavioralObservationBinary(stream, obs);
    }
}

void DataExportTool::BinaryExporter::writeMemoryDataBinary(
    std::ostream& stream, const MemoryExportData& data) {
    // Write working memory
    writeWorkingMemoryBinary(stream, data.workingMemory);

    // Write episodic memory
    writeEpisodicMemoryBinary(stream, data.episodicMemory);

    // Write semantic memory
    writeSemanticMemoryBinary(stream, data.semanticMemory);

    // Write procedural memory
    writeProceduralMemoryBinary(stream, data.proceduralMemory);

    // Write associative memory
    writeAssociativeMemoryBinary(stream, data.associations, data.associationStrengths);
}

void DataExportTool::BinaryExporter::writeNeuromodulatorDataBinary(
    std::ostream& stream,
    const std::vector<NeuromodulatorExportData>& data) {
    // Write number of neuromodulators
    uint64_t modCount = data.size();
    stream.write(reinterpret_cast<const char*>(&modCount), sizeof(modCount));

    // Write neuromodulator data
    for (const auto& mod : data) {
        writeNeuromodulatorBinary(stream, mod);
    }
}

void DataExportTool::BinaryExporter::writeSystemInfoBinary(
    std::ostream& stream,
    const std::map<std::string, std::string>& systemInfo) {
    // Write number of system info entries
    uint64_t infoCount = systemInfo.size();
    stream.write(reinterpret_cast<const char*>(&infoCount), sizeof(infoCount));

    // Write system info entries
    for (const auto& pair : systemInfo) {
        writeStringBinary(stream, pair.first);
        writeStringBinary(stream, pair.second);
    }
}

// Individual data serialization methods
void DataExportTool::BinaryExporter::writeNeuronBinary(
    std::ostream& stream, const NeuronExportData& neuron) {
    // Write neuron data as binary
    writeNeuronIdBinary(stream, neuron.id);
    writeNeuronTypeBinary(stream, neuron.type);

    // Write state data
    stream.write(reinterpret_cast<const char*>(&neuron.state.membranePotential),
        sizeof(float) * 27); // All state fields

    // Write other fields
    writeNeuronIdBinary(stream, neuron.regionId);
    writePopulationIdBinary(stream, neuron.populationId);

    // Write spike history
    uint64_t spikeCount = neuron.spikeHistory.size();
    stream.write(reinterpret_cast<const char*>(&spikeCount), sizeof(spikeCount));
    for (const auto& spike : neuron.spikeHistory) {
        stream.write(reinterpret_cast<const char*>(&spike), sizeof(Timestamp));
    }

    // Write plasticity flags
    writePlasticityFlagsBinary(stream, neuron.plasticity);
    stream.write(reinterpret_cast<const char*>(&neuron.adaptationVariable),
        sizeof(float));
    stream.write(reinterpret_cast<const char*>(&neuron.totalCurrent),
        sizeof(float));
}

void DataExportTool::BinaryExporter::writeSynapseBinary(
    std::ostream& stream, const SynapseExportData& synapse) {
    writeSynapseIdBinary(stream, synapse.id);
    writeNeuronIdBinary(stream, synapse.sourceNeuron);
    writeNeuronIdBinary(stream, synapse.destinationNeuron);

    stream.write(reinterpret_cast<const char*>(&synapse.weight), sizeof(float));
    stream.write(reinterpret_cast<const char*>(&synapse.delay), sizeof(Delay));
    writeSynapseTypeBinary(stream, synapse.type);

    stream.write(reinterpret_cast<const char*>(&synapse.preSpikeTime),
        sizeof(Timestamp));
    stream.write(reinterpret_cast<const char*>(&synapse.postSpikeTime),
        sizeof(Timestamp));

    writePlasticityFlagsBinary(stream, synapse.plasticity);
    stream.write(reinterpret_cast<const char*>(&synapse.eligibilityTrace),
        sizeof(float));
    stream.write(reinterpret_cast<const char*>(&synapse.efficacy), sizeof(float));
    stream.write(reinterpret_cast<const char*>(&synapse.isActive),
        sizeof(bool));
}

void DataExportTool::BinaryExporter::writeRegionBinary(
    std::ostream& stream, const NeuralRegionExportData& region) {
    writeRegionIdBinary(stream, region.id);
    writeStringBinary(stream, region.name);

    // Write neuron IDs
    uint64_t neuronCount = region.neuronIds.size();
    stream.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
    for (const auto& id : region.neuronIds) {
        writeNeuronIdBinary(stream, id);
    }

    // Write synapse IDs
    uint64_t synapseCount = region.synapseIds.size();
    stream.write(reinterpret_cast<const char*>(&synapseCount), sizeof(synapseCount));
    for (const auto& id : region.synapseIds) {
        writeSynapseIdBinary(stream, id);
    }

    // Write population IDs
    uint64_t popCount = region.populationIds.size();
    stream.write(reinterpret_cast<const char*>(&popCount), sizeof(popCount));
    for (const auto& id : region.populationIds) {
        writePopulationIdBinary(stream, id);
    }

    // Write statistics
    stream.write(reinterpret_cast<const char*>(&region.averageFiringRate),
        sizeof(float));
    stream.write(reinterpret_cast<const char*>(&region.synapticDensity),
        sizeof(float));
    stream.write(reinterpret_cast<const char*>(&region.averageSynapticWeight),
        sizeof(float));
    stream.write(reinterpret_cast<const char*>(&region.totalNeuronCount),
        sizeof(size_t));
    stream.write(reinterpret_cast<const char*>(&region.activeNeuronCount),
        sizeof(size_t));
    stream.write(reinterpret_cast<const char*>(&region.firingNeuronCount),
        sizeof(size_t));
}

void DataExportTool::BinaryExporter::writeStringBinary(
    std::ostream& stream, const std::string& str) {
    uint64_t length = str.length();
    stream.write(reinterpret_cast<const char*>(&length), sizeof(length));
    if (length > 0) {
        stream.write(str.c_str(), length);
    }
}

void DataExportTool::BinaryExporter::writeNeuronIdBinary(
    std::ostream& stream, NeuronId id) {
    stream.write(reinterpret_cast<const char*>(&id.value), sizeof(id.value));
}

void DataExportTool::BinaryExporter::writeSynapseIdBinary(
    std::ostream& stream, SynapseId id) {
    stream.write(reinterpret_cast<const char*>(&id.value), sizeof(id.value));
}

void DataExportTool::BinaryExporter::writeRegionIdBinary(
    std::ostream& stream, RegionId id) {
    stream.write(reinterpret_cast<const char*>(&id.value), sizeof(id.value));
}

void DataExportTool::BinaryExporter::writePopulationIdBinary(
    std::ostream& stream, PopulationId id) {
    stream.write(reinterpret_cast<const char*>(&id.value), sizeof(id.value));
}

void DataExportTool::BinaryExporter::writeNeuronTypeBinary(
    std::ostream& stream, NeuronType type) {
    uint8_t value = static_cast<uint8_t>(type);
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void DataExportTool::BinaryExporter::writeSynapseTypeBinary(
    std::ostream& stream, SynapseType type) {
    uint8_t value = static_cast<uint8_t>(type);
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void DataExportTool::BinaryExporter::writePlasticityFlagsBinary(
    std::ostream& stream, const PlasticityFlags& flags) {
    uint8_t value = 0;
    if (flags.hebbian) value |= 0x01;
    if (flags.stdp) value |= 0x02;
    if (flags.reward_modulated) value |= 0x04;
    if (flags.structural) value |= 0x08;
    if (flags.eligible) value |= 0x10;
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void DataExportTool::BinaryExporter::writeBehavioralObservationBinary(
    std::ostream& stream,
    const BehavioralObservationExportData& obs) {
    // Write observation data as binary
    auto time = obs.timestamp.time_since_epoch();
    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    stream.write(reinterpret_cast<const char*>(&milliseconds),
        sizeof(milliseconds));

    stream.write(reinterpret_cast<const char*>(&obs.step), sizeof(SimulationStep));
    stream.write(reinterpret_cast<const char*>(&obs.simulationTime),
        sizeof(double));

    // Write sensory data
    uint64_t sensoryCount = obs.sensoryData.size();
    stream.write(reinterpret_cast<const char*>(&sensoryCount), sizeof(sensoryCount));
    stream.write(reinterpret_cast<const char*>(obs.sensoryData.data()),
        obs.sensoryData.size() * sizeof(float));

    writeActionTypeBinary(stream, obs.actionType);

    // Write action parameters
    uint64_t paramCount = obs.actionParameters.size();
    stream.write(reinterpret_cast<const char*>(&paramCount), sizeof(paramCount));
    stream.write(reinterpret_cast<const char*>(obs.actionParameters.data()),
        obs.actionParameters.size() * sizeof(float));

    stream.write(reinterpret_cast<const char*>(&obs.reward), sizeof(float));
    stream.write(reinterpret_cast<const char*>(&obs.predictionError),
        sizeof(float));
    writeStringBinary(stream, obs.behavioralContext);

    // Write active neurons
    uint64_t activeCount = obs.activeNeurons.size();
    stream.write(reinterpret_cast<const char*>(&activeCount), sizeof(activeCount));
    for (const auto& id : obs.activeNeurons) {
        writeNeuronIdBinary(stream, id);
    }

    stream.write(reinterpret_cast<const char*>(&obs.arousal), sizeof(float));
    stream.write(reinterpret_cast<const char*>(&obs.attention), sizeof(float));
}

void DataExportTool::BinaryExporter::writeActionTypeBinary(
    std::ostream& stream, ActionType type) {
    uint8_t value = static_cast<uint8_t>(type);
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

// Other serialization methods would follow similar patterns...

} // namespace nlm
