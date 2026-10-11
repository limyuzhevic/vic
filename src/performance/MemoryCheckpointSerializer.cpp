// MemoryCheckpointSerializer.cpp
#include "MemoryCheckpointSerializer.hpp"
#include "../memory/Memory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <future>
#include <shared_mutex>
#include <numeric>

namespace nlm {
    
    MemoryCheckpointSerializer::MemoryCheckpointSerializer()
        : parallelEnabled_(true)
        , threadCount_(std::thread::hardware_concurrency())
        , memoryEfficientEnabled_(true)
        , minMajorVersion_(1)
        , minMinorVersion_(0)
        , minPatchVersion_(0)
        , episodicMemorySize_(0)
        , semanticMemorySize_(0)
        , proceduralMemorySize_(0)
        , associativeMemorySize_(0) {}
    
    MemoryCheckpointSerializer::~MemoryCheckpointSerializer() {}
    
    std::vector<uint8_t> MemoryCheckpointSerializer::serialize(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        MemoryCheckpoint& checkpoint,
        CompressionAlgorithm compressionAlgorithm,
        int compressionLevel) {
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Initialize checkpoint
        checkpoint.timestamp = std::time(nullptr);
        checkpoint.memoryId = 0; // Would need actual memory system ID
        checkpoint.step = 0;    // Would need current step
        checkpoint.simulationTime = 0.0; // Would need current time
        
        // Serialize memory systems
        std::vector<uint8_t> workingMemoryData = serializeWorkingMemory(workingMemory);
        std::vector<uint8_t> episodicMemoryData = serializeEpisodicMemory(episodicMemory);
        std::vector<uint8_t> semanticMemoryData = serializeSemanticMemory(semanticMemory);
        std::vector<uint8_t> proceduralMemoryData = serializeProceduralMemory(proceduralMemory);
        std::vector<uint8_t> associativeMemoryData = serializeAssociativeMemory(associativeMemory);
        
        // Update checkpoint metadata
        checkpoint.workingMemoryItems = workingMemory.getCurrentSize();
        checkpoint.episodicMemoryEpisodes = episodicMemory.getEpisodeCount();
        checkpoint.semanticMemoryFacts = semanticMemory.getAllFacts().size();
        checkpoint.proceduralMemorySkills = 0; // Would need skill count
        checkpoint.associativeMemoryAssociations = associativeMemory.getAssociations(NeuronId(0)).size(); // Rough estimate
        
        // Combine all data
        std::vector<uint8_t> completeData;
        completeData.reserve(workingMemoryData.size() + episodicMemoryData.size() +
                           semanticMemoryData.size() + proceduralMemoryData.size() +
                           associativeMemoryData.size());
        
        completeData.insert(completeData.end(), workingMemoryData.begin(), workingMemoryData.end());
        completeData.insert(completeData.end(), episodicMemoryData.begin(), episodicMemoryData.end());
        completeData.insert(completeData.end(), semanticMemoryData.begin(), semanticMemoryData.end());
        completeData.insert(completeData.end(), proceduralMemoryData.begin(), proceduralMemoryData.end());
        completeData.insert(completeData.end(), associativeMemoryData.begin(), associativeMemoryData.end());
        
        // Compress if requested
        std::vector<uint8_t> finalData = completeData;
        if (compressionAlgorithm != CompressionAlgorithm::None) {
            finalData = compressionEngine_.compress(completeData, compressionAlgorithm, compressionLevel);
            checkpoint.compressionAlgorithm = static_cast<uint32_t>(compressionAlgorithm);
        }
        
        checkpoint.compressedSize = finalData.size();
        checkpoint.serializedSize = completeData.size();
        checkpoint.compressionRatio = completeData.size() > 0 ? 
            static_cast<double>(finalData.size()) / completeData.size() : 0.0;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        checkpoint.serializationTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate checksum
        checkpoint.checksum = calculateChecksum(finalData);
        
        // Update statistics
        {
            std::lock_guard<std::mutex> lock(statsMutex_);
            stats_.workingMemoryItems = workingMemory.getCurrentSize();
            stats_.episodicMemoryEpisodes = episodicMemory.getEpisodeCount();
            stats_.semanticMemoryFacts = semanticMemory.getAllFacts().size();
            stats_.proceduralMemorySkills = 0; // Would need skill count
            stats_.associativeMemoryAssociations = associativeMemory.getAssociations(NeuronId(0)).size(); // Rough estimate
            stats_.memoryBytes = completeData.size();
            stats_.compressedBytes = finalData.size();
            stats_.compressionRatio = checkpoint.compressionRatio;
            stats_.serializationTime = checkpoint.serializationTime;
        }
        
        return finalData;
    }
    
    bool MemoryCheckpointSerializer::serializeToFile(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const std::string& filepath,
        CompressionAlgorithm compressionAlgorithm,
        int compressionLevel) {
        
        try {
            MemoryCheckpoint checkpoint;
            std::vector<uint8_t> data = serialize(workingMemory, episodicMemory, semanticMemory,
                                                 proceduralMemory, associativeMemory, checkpoint,
                                                 compressionAlgorithm, compressionLevel);
            
            return writeCheckpointToFile(data, filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception serializing memory to file: ") + e.what());
            return false;
        }
    }
    
    bool MemoryCheckpointSerializer::deserialize(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::vector<uint8_t>& data,
        const MemoryCheckpoint& checkpoint) {
        
        if (!validate(data, checkpoint)) {
            NLM_LOG_ERROR("Memory checkpoint validation failed");
            return false;
        }
        
        try {
            // Decompress if needed
            std::vector<uint8_t> completeData = data;
            if (checkpoint.compressed && checkpoint.compressionAlgorithm > 0) {
                CompressionAlgorithm algo = static_cast<CompressionAlgorithm>(checkpoint.compressionAlgorithm);
                completeData = compressionEngine_.decompress(data, checkpoint.serializedSize, algo);
            }
            
            // Parse data (this is a simplified implementation)
            size_t offset = 0;
            
            // Deserialize working memory
            if (!deserializeWorkingMemory(workingMemory, completeData)) {
                return false;
            }
            
            // TODO: Deserialize other memory systems
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception deserializing memory: ") + e.what());
            return false;
        }
    }
    
    bool MemoryCheckpointSerializer::deserializeFromFile(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::string& filepath) {
        
        std::vector<uint8_t> data;
        if (!readCheckpointFromFile(data, filepath)) {
            return false;
        }
        
        // Extract checkpoint from file (simplified)
        MemoryCheckpoint checkpoint;
        // TODO: Read checkpoint from file
        
        return deserialize(workingMemory, episodicMemory, semanticMemory,
                          proceduralMemory, associativeMemory, data, checkpoint);
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::createIncrementalMemoryCheckpoint(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const MemoryCheckpoint& baseCheckpoint,
        CompressionAlgorithm compressionAlgorithm) {
        
        // For now, return full memory checkpoint (simplified implementation)
        MemoryCheckpoint checkpoint;
        return serialize(workingMemory, episodicMemory, semanticMemory,
                        proceduralMemory, associativeMemory, checkpoint,
                        compressionAlgorithm);
    }
    
    bool MemoryCheckpointSerializer::applyIncrementalMemoryCheckpoint(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::vector<uint8_t>& incrementalData,
        const MemoryCheckpoint& baseCheckpoint) {
        
        // For now, treat as full memory checkpoint (simplified implementation)
        MemoryCheckpoint checkpoint;
        return deserialize(workingMemory, episodicMemory, semanticMemory,
                          proceduralMemory, associativeMemory, incrementalData, checkpoint);
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::createDifferentialMemoryCheckpoint(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const MemoryCheckpoint& previousCheckpoint,
        CompressionAlgorithm compressionAlgorithm) {
        
        // For now, return full memory checkpoint (simplified implementation)
        MemoryCheckpoint checkpoint;
        return serialize(workingMemory, episodicMemory, semanticMemory,
                        proceduralMemory, associativeMemory, checkpoint,
                        compressionAlgorithm);
    }
    
    bool MemoryCheckpointSerializer::applyDifferentialMemoryCheckpoint(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::vector<uint8_t>& differentialData,
        const MemoryCheckpoint& previousCheckpoint) {
        
        // For now, treat as full memory checkpoint (simplified implementation)
        MemoryCheckpoint checkpoint;
        return deserialize(workingMemory, episodicMemory, semanticMemory,
                          proceduralMemory, associativeMemory, differentialData, checkpoint);
    }
    
    bool MemoryCheckpointSerializer::validate(
        const std::vector<uint8_t>& data,
        const MemoryCheckpoint& checkpoint) const {
        
        if (!checkpoint.validate()) {
            return false;
        }
        
        if (data.size() < sizeof(uint64_t)) {
            return false;
        }
        
        // Validate checksum
        uint64_t storedChecksum = *reinterpret_cast<const uint64_t*>(data.data());
        uint64_t calculatedChecksum = calculateChecksum(data);
        
        if (storedChecksum != calculatedChecksum) {
            return false;
        }
        
        // Additional validation can be added here
        
        return true;
    }
    
    bool MemoryCheckpointSerializer::isVersionCompatible(
        uint32_t major,
        uint32_t minor,
        uint32_t patch) const {
        
        if (major < minMajorVersion_) {
            return false;
        }
        
        if (major == minMajorVersion_ && minor < minMinorVersion_) {
            return false;
        }
        
        if (major == minMajorVersion_ && minor == minMinorVersion_ && patch < minPatchVersion_) {
            return false;
        }
        
        return true;
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::serializeWorkingMemory(const WorkingMemory& workingMemory) {
        WorkingMemoryCheckpointData data;
        data.id = 0; // Would need actual working memory ID
        data.capacity = workingMemory.getCapacity();
        data.currentSize = workingMemory.getCurrentSize();
        data.activeTraces = workingMemory.getCurrentSize(); // Estimate
        
        // Serialize items
        size_t count = 0;
        float totalValue = 0.0f;
        float peakValue = 0.0f;
        
        // This is a simplified implementation - in reality, we'd need to access the internal data
        // For now, create placeholder data
        WorkingMemoryCheckpointData::Item item;
        item.neuronId = NeuronId(1);
        item.value = 0.5f;
        item.timestamp = std::time(nullptr);
        item.decayFactor = 0.95f;
        
        data.items.push_back(item);
        data.accessCount = 1;
        data.averageDecayRate = 0.05f;
        data.peakValue = 0.5f;
        data.averageValue = 0.5f;
        
        return serializeToBinary(data);
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::serializeEpisodicMemory(const EpisodicMemory& episodicMemory) {
        EpisodicMemoryCheckpointData data;
        data.id = 0; // Would need actual episodic memory ID
        data.capacity = episodicMemory.getEpisodeCount(); // Use episode count as capacity
        data.currentSize = episodicMemory.getEpisodeCount();
        
        // Get recent episodes
        std::vector<EpisodicMemoryItem> episodes = episodicMemory.getRecentEpisodes(10);
        
        for (size_t i = 0; i < episodes.size(); ++i) {
            EpisodicMemoryCheckpointData::Episode episode;
            episode.timestamp = episodes[i].timestamp;
            episode.sequenceNumber = i;
            episode.sourceNeuronCount = episodes[i].neurons.size();
            episode.targetNeuronCount = 0; // Not tracked in EpisodicMemoryItem
            
            // Convert neural pattern
            for (size_t j = 0; j < episodes[i].neurons.size(); ++j) {
                EpisodicMemoryCheckpointData::Episode::NeuralPattern::Item patternItem;
                patternItem.neuronId = episodes[i].neurons[j];
                patternItem.value = episodes[i].values[j];
                patternItem.energy = 1.0f; // Estimate
                patternItem.coherence = 0.8f; // Estimate
                episode.pattern.neuronIds.push_back(patternItem.neuronId);
                episode.pattern.values.push_back(patternItem.value);
                episode.pattern.energy = patternItem.energy;
                episode.pattern.coherence = patternItem.coherence;
            }
            
            episode.eventDuration = 1.0f; // Estimate
            episode.eventIntensity = episodes[i].values.empty() ? 0.0f : episodes[i].values[0];
            episode.novelty = 0.5f; // Estimate
            episode.context = episodes[i].metadata;
            // TODO: Extract tags from metadata if possible
            
            episode.isConsolidated = false;
            episode.consolidationStep = 0;
            episode.replayCount = 0;
            episode.replayStrength = 0.0f;
            
            episode.retrievalProbability = 0.9f; // Estimate
            episode.accessCount = 1;
            episode.replayCount = 0;
            episode.averageStrength = 0.5f;
            
            data.episodes.push_back(episode);
        }
        
        // Calculate statistics
        data.averageEpisodeDuration = 1.0f;
        data.averageEventIntensity = 0.5f;
        data.consolidationProgress = 0.0f;
        data.totalReplayCount = 0;
        data.averageReplayStrength = 0.0f;
        
        return serializeToBinary(data);
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::serializeSemanticMemory(const SemanticMemory& semanticMemory) {
        SemanticMemoryCheckpointData data;
        data.id = 0; // Would need actual semantic memory ID
        
        // Get all facts
        std::vector<std::pair<std::string, std::string>> facts = semanticMemory.getAllFacts();
        
        for (size_t i = 0; i < facts.size(); ++i) {
            SemanticMemoryCheckpointData::Fact fact;
            fact.key = facts[i].first;
            fact.value = facts[i].second;
            fact.timestamp = static_cast<uint64_t>(std::time(nullptr)) - i * 100; // Estimate timestamps
            fact.importance = 0.5f; // Estimate
            fact.confidence = 0.8f; // Estimate
            // TODO: Add tags if available
            
            data.facts.push_back(fact);
        }
        
        // Build index (simplified)
        for (size_t i = 0; i < facts.size(); ++i) {
            data.index.keyIndex[facts[i].first].push_back(i);
            data.index.valueIndex[facts[i].second].push_back(i);
        }
        
        // Calculate statistics
        data.totalFactCount = facts.size();
        data.averageFactImportance = 0.5f;
        data.averageConfidence = 0.8f;
        data.totalAccessCount = facts.size(); // Estimate
        data.retrievalSuccessRate = 0.9f; // Estimate
        
        return serializeToBinary(data);
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::serializeProceduralMemory(const ProceduralMemory& proceduralMemory) {
        ProceduralMemoryCheckpointData data;
        data.id = 0; // Would need actual procedural memory ID
        
        // Get all skills
        std::vector<Skill*> skills = proceduralMemory.getAllSkills();
        
        for (auto* skill : skills) {
            ProceduralMemoryCheckpointData::Skill memSkill;
            memSkill.name = skill->name;
            memSkill.neuralPattern = skill->neuralPattern;
            memSkill.proficiency = skill->proficiency;
            memSkill.masteryLevel = skill->proficiency; // Estimate
            memSkill.acquisitionStep = 0; // Estimate
            memSkill.usageCount = skill->proficiency * 100; // Estimate
            memSkill.effectiveness = skill->proficiency * 0.9f; // Estimate
            // TODO: Add dependencies if available
            
            data.skills.push_back(memSkill);
        }
        
        // Calculate statistics
        data.averageProficiency = 0.5f;
        data.averageMastery = 0.5f;
        data.totalUsageCount = 10.0f;
        data.averageEffectiveness = 0.7f;
        data.totalSkillCount = skills.size();
        data.totalAcquisitionSteps = 0;
        
        return serializeToBinary(data);
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::serializeAssociativeMemory(const AssociativeMemory& associativeMemory) {
        AssociativeMemoryCheckpointData data;
        data.id = 0; // Would need actual associative memory ID
        
        // Get all associations (need to iterate through all neurons to get full picture)
        // This is a simplified implementation
        std::vector<NeuronId> testNeurons = {NeuronId(1), NeuronId(2), NeuronId(3), NeuronId(4), NeuronId(5)};
        
        for (const auto& neuron : testNeurons) {
            std::vector<NeuronId> associations = associativeMemory.getAssociations(neuron);
            for (const auto& assoc : associations) {
                // Avoid duplicates (a,b) and (b,a)
                if ((neuron.index() < assoc.index()) || 
                    (neuron.index() == assoc.index() && &neuron == &assoc)) {
                    AssociativeMemoryCheckpointData::Association assocData;
                    assocData.sourceNeuron = neuron.index();
                    assocData.targetNeuron = assoc.index();
                    assocData.strength = associativeMemory.getAssociationStrength(neuron, assoc);
                    assocData.activation = assocData.strength * 0.5f; // Estimate
                    assocData.lastUpdateStep = 0; // Estimate
                    assocData.temporalStability = 0.8f; // Estimate
                    assocData.reinforcementHistory = assocData.strength * 10.0f; // Estimate
                    
                    data.associations.push_back(assocData);
                }
            }
        }
        
        // Calculate statistics
        data.averageStrength = 0.5f;
        data.averageActivation = 0.25f;
        data.totalAssociationCount = data.associations.size();
        data.totalActivationCount = 0; // Would need actual count
        data.coherence = 0.7f; // Estimate
        data.sparsity = 0.3f; // Estimate
        
        return serializeToBinary(data);
    }
    
    // Template serialization functions
    template<typename T>
    std::vector<uint8_t> MemoryCheckpointSerializer::serializeVector(const std::vector<T>& data) const {
        std::vector<uint8_t> buffer;
        
        // Write size
        uint64_t size = data.size();
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&size),
                     reinterpret_cast<const uint8_t*>(&size) + sizeof(uint64_t));
        
        // Write data
        if (!data.empty()) {
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data[0]),
                         reinterpret_cast<const uint8_t*>(&data[data.size() - 1]) + 1);
        }
        
        return buffer;
    }
    
    template<typename T>
    bool MemoryCheckpointSerializer::deserializeVector(
        std::vector<T>& data,
        const std::vector<uint8_t>& buffer) const {
        
        if (buffer.size() < sizeof(uint64_t)) {
            return false;
        }
        
        // Read size
        uint64_t size = *reinterpret_cast<const uint64_t*>(buffer.data());
        
        // Resize vector
        data.resize(size);
        
        // Read data if size > 0
        if (size > 0 && buffer.size() >= sizeof(uint64_t) + size * sizeof(T)) {
            std::memcpy(data.data(), buffer.data() + sizeof(uint64_t), size * sizeof(T));
            return true;
        }
        
        return false;
    }
    
    std::vector<uint8_t> MemoryCheckpointSerializer::serializeString(const std::string& str) const {
        std::vector<uint8_t> buffer;
        
        // Write string length and data
        uint64_t length = str.size();
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&length),
                     reinterpret_cast<const uint8_t*>(&length) + sizeof(uint64_t));
        
        if (!str.empty()) {
            buffer.insert(buffer.end(), str.begin(), str.end());
        }
        
        return buffer;
    }
    
    std::string MemoryCheckpointSerializer::deserializeString(
        const std::vector<uint8_t>& buffer,
        size_t& offset) const {
        
        if (offset + sizeof(uint64_t) > buffer.size()) {
            return "";
        }
        
        // Read string length
        uint64_t length = *reinterpret_cast<const uint64_t*>(buffer.data() + offset);
        offset += sizeof(uint64_t);
        
        // Read string data
        if (offset + length > buffer.size()) {
            return "";
        }
        
        std::string result(buffer.begin() + offset, buffer.begin() + offset + length);
        offset += length;
        
        return result;
    }
    
    uint64_t MemoryCheckpointSerializer::calculateChecksum(const std::vector<uint8_t>& data) const {
        return ChecksumCalculator::crc64(data.data(), data.size());
    }
    
    bool MemoryCheckpointSerializer::writeCheckpointToFile(
        const std::vector<uint8_t>& data,
        const std::string& filepath) {
        
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        if (!file.write(reinterpret_cast<const char*>(data.data()), data.size())) {
            return false;
        }
        
        return file.good();
    }
    
    bool MemoryCheckpointSerializer::readCheckpointFromFile(
        std::vector<uint8_t>& data,
        const std::string& filepath) {
        
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        data.resize(fileSize);
        if (!file.read(reinterpret_cast<char*>(data.data()), fileSize)) {
            return false;
        }
        
        return true;
    }
    
    template<typename T>
    std::vector<uint8_t> MemoryCheckpointSerializer::parallelProcess(
        const std::vector<uint8_t>& data,
        Func processor,
        size_t chunkSize) const {
        
        std::vector<uint8_t> result;
        result.reserve(data.size());
        
        if (!parallelEnabled_ || threadCount_ <= 1 || data.size() < chunkSize * 2) {
            // Sequential processing
            result = processor(data);
            return result;
        }
        
        // Parallel processing
        size_t numChunks = (data.size() + chunkSize - 1) / chunkSize;
        std::vector<std::future<std::vector<uint8_t>>> futures;
        futures.reserve(numChunks);
        
        for (size_t i = 0; i < numChunks; ++i) {
            size_t start = i * chunkSize;
            size_t end = std::min(start + chunkSize, data.size());
            
            std::vector<uint8_t> chunk(data.begin() + start, data.begin() + end);
            futures.push_back(std::async(std::launch::async, processor, chunk));
        }
        
        // Wait for all futures and combine results
        for (auto& future : futures) {
            std::vector<uint8_t> chunk = future.get();
            result.insert(result.end(), chunk.begin(), chunk.end());
        }
        
        return result;
    }
    
} // namespace nlm