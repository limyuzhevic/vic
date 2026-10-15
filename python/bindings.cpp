// ==============================================================================
// Context Manager Wrapper Classes with Enhanced Error Handling
// ==============================================================================

class PyConfigWrapper {
public:
    PyConfigWrapper(std::shared_ptr<Config> config) : config_(config) {
        if (!config_) {
            throw NLMConfigError("Config cannot be null", "PyConfigWrapper constructor");
        }
    }
    
    PyConfigWrapper(const PyConfigWrapper&) = delete;
    PyConfigWrapper& operator=(const PyConfigWrapper&) = delete;
    
    // Enter method - returns reference to config
    Config& enter() {
        try {
            return *config_;
        } catch (...) {
            throw NLMConfigError("Failed to enter config context", "PyConfigWrapper::enter");
        }
    }
    
    // Exit method - ensures config is cleaned up
    void exit() {
        try {
            config_.reset();
        } catch (...) {
            throw NLMConfigError("Failed to exit config context", "PyConfigWrapper::exit");
        }
    }
    
    // Python context manager protocol
    Config& __enter__() {
        return enter();
    }
    
    void __exit__(py::object, py::object, py::object) {
        exit();
    }
    
    // Access to underlying config for Python methods
    std::shared_ptr<Config>& getConfig() {
        return config_;
    }
    
    // Check if config is in a valid state
    bool isValid() const {
        return static_cast<bool>(config_);
    }
    
    // Recovery method
    void recover() {
        if (!config_) {
            config_ = std::make_shared<Config>();
        }
    }
    
    // Get configuration summary
    std::string getStatus() const {
        if (!config_) {
            return "Config: NULL (Invalid state)";
        }
        try {
            return "Config: Valid (" + config_->summary() + ")";
        } catch (...) {
            return "Config: CORRUPTED";
        }
    }
    
private:
    std::shared_ptr<Config> config_;
};

class PyBrainWrapper {
public:
    PyBrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain_) {
            throw NLMInitializationError("Brain cannot be null", "PyBrainWrapper constructor");
        }
    }
    
    PyBrainWrapper(const PyBrainWrapper&) = delete;
    PyBrainWrapper& operator=(const PyBrainWrapper&) = delete;
    
    Brain& enter() {
        try {
            return *brain_;
        } catch (...) {
            throw NLMInitializationError("Failed to enter brain context", "PyBrainWrapper::enter");
        }
    }
    
    void exit() {
        try {
            brain_.reset();
        } catch (...) {
            throw NLMInitializationError("Failed to exit brain context", "PyBrainWrapper::exit");
        }
    }
    
    Brain& __enter__() {
        return enter();
    }
    
    void __exit__(py::object, py::object, py::object) {
        exit();
    }
    
    std::shared_ptr<Brain>& getBrain() {
        return brain_;
    }
    
    // Check if brain is initialized
    bool isInitialized() const {
        if (!brain_) {
            return false;
        }
        try {
            return brain_->getDevelopmentalStage() != DevelopmentalStage::Initial;
        } catch (...) {
            return false;
        }
    }
    
    // Recovery method
    void recover() {
        if (!brain_) {
            throw NLMResourceError("Cannot recover uninitialized brain", "PyBrainWrapper::recover");
        }
        brain_->reset();
        brain_->initialize();
    }
    
    // Get brain status
    std::string getStatus() const {
        if (!brain_) {
            return "Brain: NULL (Uninitialized)";
        }
        try {
            std::string state = isInitialized() ? "Initialized" : "Not Initialized";
            return "Brain: " + state + " (Stage: " + std::to_string(static_cast<int>(brain_->getDevelopmentalStage())) + ")";
        } catch (...) {
            return "Brain: CORRUPTED";
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

class PyAgentBrainWrapper {
public:
    PyAgentBrainWrapper(std::shared_ptr<AgentBrain> agent_brain) : agent_brain_(agent_brain) {
        if (!agent_brain_) {
            throw NLMInitializationError("AgentBrain cannot be null", "PyAgentBrainWrapper constructor");
        }
    }
    
    PyAgentBrainWrapper(const PyAgentBrainWrapper&) = delete;
    PyAgentBrainWrapper& operator=(const PyAgentBrainWrapper&) = delete;
    
    AgentBrain& enter() {
        try {
            return *agent_brain_;
        } catch (...) {
            throw NLMInitializationError("Failed to enter agent brain context", "PyAgentBrainWrapper::enter");
        }
    }
    
    void exit() {
        try {
            agent_brain_.reset();
        } catch (...) {
            throw NLMInitializationError("Failed to exit agent brain context", "PyAgentBrainWrapper::exit");
        }
    }
    
    AgentBrain& __enter__() {
        return enter();
    }
    
    void __exit__(py::object, py::object, py::object) {
        exit();
    }
    
    std::shared_ptr<AgentBrain>& getAgentBrain() {
        return agent_brain_;
    }
    
    // Check if agent brain is valid
    bool isValid() const {
        return static_cast<bool>(agent_brain_);
    }
    
    // Recovery method
    void recover() {
        if (!agent_brain_) {
            agent_brain_ = std::make_shared<AgentBrain>(std::make_shared<Brain>(std::make_shared<Config>()));
        }
    }
    
    // Get status
    std::string getStatus() const {
        if (!agent_brain_) {
            return "AgentBrain: NULL (Uninitialized)";
        }
        try {
            return "AgentBrain: Valid (Neuromodulation: " + std::to_string(agent_brain_->getNeuromodulationLevel()) + ")";
        } catch (...) {
            return "AgentBrain: CORRUPTED";
        }
    }
    
private:
    std::shared_ptr<AgentBrain> agent_brain_;
};

class PySimpleWorldWrapper {
public:
    PySimpleWorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(world) {
        if (!world_) {
            throw NLMResourceError("SimpleWorld cannot be null", "PySimpleWorldWrapper constructor");
        }
    }
    
    PySimpleWorldWrapper(const PySimpleWorldWrapper&) = delete;
    PySimpleWorldWrapper& operator=(const PySimpleWorldWrapper&) = delete;
    
    SimpleWorld& enter() {
        try {
            return *world_;
        } catch (...) {
            throw NLMResourceError("Failed to enter world context", "PySimpleWorldWrapper::enter");
        }
    }
    
    void exit() {
        try {
            world_.reset();
        } catch (...) {
            throw NLMResourceError("Failed to exit world context", "PySimpleWorldWrapper::exit");
        }
    }
    
    SimpleWorld& __enter__() {
        return enter();
    }
    
    void __exit__(py::object, py::object, py::object) {
        exit();
    }
    
    std::shared_ptr<SimpleWorld>& getWorld() {
        return world_;
    }
    
    // Check if world is valid
    bool isValid() const {
        return static_cast<bool>(world_);
    }
    
    // Recovery method
    void recover() {
        if (!world_) {
            world_ = std::make_shared<SimpleWorld>();
        }
    }
    
    // Get status
    std::string getStatus() const {
        if (!world_) {
            return "SimpleWorld: NULL (Uninitialized)";
        }
        try {
            std::string state = isValid() ? "Valid" : "Invalid";
            return "SimpleWorld: " + state + " (Size: " + std::to_string(world_->getWidth()) + "x" + std::to_string(world_->getHeight()) + ")";
        } catch (...) {
            return "SimpleWorld: CORRUPTED";
        }
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

class PySensoryInputWrapper {
public:
    PySensoryInputWrapper(std::unique_ptr<SensoryInput> sensory_input) : sensory_input_(std::move(sensory_input)) {
        if (!sensory_input_) {
            throw NLMValidationError("SensoryInput cannot be null", "sensory_input", "PySensoryInputWrapper constructor");
        }
    }
    
    PySensoryInputWrapper(const PySensoryInputWrapper&) = delete;
    PySensoryInputWrapper& operator=(const PySensoryInputWrapper&) = delete;
    
    SensoryInput& enter() {
        try {
            return *sensory_input_;
        } catch (...) {
            throw NLMValidationError("Failed to enter sensory input context", "sensory_input", "PySensoryInputWrapper::enter");
        }
    }
    
    void exit() {
        try {
            sensory_input_.reset();
        } catch (...) {
            throw NLMValidationError("Failed to exit sensory input context", "sensory_input", "PySensoryInputWrapper::exit");
        }
    }
    
    SensoryInput& __enter__() {
        return enter();
    }
    
    void __exit__(py::object, py::object, py::object) {
        exit();
    }
    
    std::unique_ptr<SensoryInput>& getSensoryInput() {
        return sensory_input_;
    }
    
    // Check if sensory input is valid
    bool isValid() const {
        return static_cast<bool>(sensory_input_);
    }
    
    // Recovery method
    void recover() {
        if (!sensory_input_) {
            sensory_input_ = std::make_unique<SensoryInput>();
        }
    }
    
    // Get status
    std::string getStatus() const {
        if (!sensory_input_) {
            return "SensoryInput: NULL (Uninitialized)";
        }
        try {
            return "SensoryInput: Valid (Type: " + sensory_input_->getType() + ", Dimensions: " + std::to_string(sensory_input_->getDimensions()) + ")";
        } catch (...) {
            return "SensoryInput: CORRUPTED";
        }
    }
    
private:
    std::unique_ptr<SensoryInput> sensory_input_;
};

class PyActionWrapper {
public:
    PyActionWrapper(std::unique_ptr<Action> action) : action_(std::move(action)) {
        if (!action_) {
            throw NLMResourceError("Action cannot be null", "action", "PyActionWrapper constructor");
        }
    }
    
    PyActionWrapper(const PyActionWrapper&) = delete;
    PyActionWrapper& operator=(const PyActionWrapper&) = delete;
    
    Action& enter() {
        try {
            return *action_;
        } catch (...) {
            throw NLMResourceError("Failed to enter action context", "action", "PyActionWrapper::enter");
        }
    }
    
    void exit() {
        try {
            action_.reset();
        } catch (...) {
            throw NLMResourceError("Failed to exit action context", "action", "PyActionWrapper::exit");
        }
    }
    
    Action& __enter__() {
        return enter();
    }
    
    void __exit__(py::object, py::object, py::object) {
        exit();
    }
    
    std::unique_ptr<Action>& getAction() {
        return action_;
    }
    
    // Check if action is valid
    bool isValid() const {
        return static_cast<bool>(action_);
    }
    
    // Recovery method
    void recover() {
        if (!action_) {
            action_ = std::make_unique<Action>();
        }
    }
    
    // Get status
    std::string getStatus() const {
        if (!action_) {
            return "Action: NULL (Uninitialized)";
        }
        try {
            return "Action: Valid (Type: " + std::to_string(static_cast<int>(action_->getType())) + ")";
        } catch (...) {
            return "Action: CORRUPTED";
        }
    }
    
private:
    std::unique_ptr<Action> action_;
};

// ==============================================================================
// Serialization Helper Classes and Functions
// ==============================================================================

// Progress tracking for serialization
class SerializationProgress {
public:
    SerializationProgress(size_t total_units) : total_units_(total_units), completed_units_(0) {}
    
    void update(size_t completed) {
        std::lock_guard<std::mutex> lock(mutex_);
        completed_units_ = completed;
        if (completed_units_ % update_interval_ == 0) {
            callback_(completed_units_, total_units_);
        }
    }
    
    double getProgress() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return total_units_ > 0 ? static_cast<double>(completed_units_) / total_units_ : 0.0;
    }
    
    void setCallback(std::function<void(size_t, size_t)> callback) {
        callback_ = callback;
    }
    
    void setUpdateInterval(size_t interval) {
        update_interval_ = interval;
    }
    
private:
    size_t total_units_;
    size_t completed_units_;
    std::function<void(size_t, size_t)> callback_ = [](size_t, size_t) {};
    size_t update_interval_ = 100;
    mutable std::mutex mutex_;
};

// Compression utilities
class CompressionUtils {
public:
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data, int level = Z_BEST_COMPRESSION) {
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        
        if (deflateInit(&stream, level) != Z_OK) {
            throw std::runtime_error("Failed to initialize compression");
        }
        
        std::vector<uint8_t> compressed_data;
        compressed_data.resize(data.size() * 2); // Reserve extra space
        
        stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data.data()));
        stream.avail_in = data.size();
        stream.next_out = reinterpret_cast<Bytef*>(compressed_data.data());
        stream.avail_out = compressed_data.size();
        
        int result = deflate(&stream, Z_FINISH);
        if (result == Z_STREAM_ERROR) {
            deflateEnd(&stream);
            throw std::runtime_error("Compression error");
        }
        
        compressed_data.resize(stream.next_out - reinterpret_cast<Bytef*>(compressed_data.data()));
        deflateEnd(&stream);
        
        return compressed_data;
    }
    
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& data) {
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        
        if (inflateInit(&stream) != Z_OK) {
            throw std::runtime_error("Failed to initialize decompression");
        }
        
        std::vector<uint8_t> decompressed_data;
        decompressed_data.resize(data.size() * 4); // Reserve extra space
        
        stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data.data()));
        stream.avail_in = data.size();
        stream.next_out = reinterpret_cast<Bytef*>(decompressed_data.data());
        stream.avail_out = decompressed_data.size();
        
        int result = inflate(&stream, Z_FINISH);
        if (result == Z_STREAM_ERROR) {
            inflateEnd(&stream);
            throw std::runtime_error("Decompression error");
        }
        
        decompressed_data.resize(stream.next_out - reinterpret_cast<Bytef*>(decompressed_data.data()));
        inflateEnd(&stream);
        
        return decompressed_data;
    }
};

// Checksum utilities
class ChecksumUtils {
public:
    static uint64_t calculateCRC64(const std::vector<uint8_t>& data) {
        uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;
        const uint64_t polynomial = 0x42F0E1EBA9EA3693ULL;
        
        for (uint8_t byte : data) {
            crc ^= static_cast<uint64_t>(byte) << 56;
            for (int i = 0; i < 8; ++i) {
                if (crc & (1ULL << 63)) {
                    crc = (crc << 1) ^ polynomial;
                } else {
                    crc <<= 1;
                }
            }
        }
        
        return ~crc;
    }
    
    static std::string toHexString(uint64_t value) {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value;
        return ss.str();
    }
};

// Serialization metadata
struct SerializationMetadata {
    std::string format;
    std::string version;
    uint64_t checksum;
    size_t neuronCount;
    size_t synapseCount;
    size_t regionCount;
    size_t step;
    double simulationTime;
    std::unordered_map<std::string, std::string> customMetadata;
    
    SerializationMetadata() : format("binary"), version("1.0"), checksum(0), 
                              neuronCount(0), synapseCount(0), regionCount(0), 
                              step(0), simulationTime(0.0) {}
};

// JSON serialization utilities
class JsonSerializer {
public:
    static json serializeConfig(const Config& config) {
        json j;
        j["type"] = "Config";
        j["version"] = "1.0";
        
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto valueOpt = config.get<ConfigValue>(key);
            if (valueOpt) {
                const ConfigValue& value = *valueOpt;
                
                if (std::holds_alternative<int>(value)) {
                    j["data"][key] = std::get<int>(value);
                } else if (std::holds_alternative<int64_t>(value)) {
                    j["data"][key] = std::get<int64_t>(value);
                } else if (std::holds_alternative<double>(value)) {
                    j["data"][key] = std::get<double>(value);
                } else if (std::holds_alternative<bool>(value)) {
                    j["data"][key] = std::get<bool>(value);
                } else if (std::holds_alternative<std::string>(value)) {
                    j["data"][key] = std::get<std::string>(value);
                } else if (std::holds_alternative<std::vector<int>>(value)) {
                    j["data"][key] = std::get<std::vector<int>>(value);
                } else if (std::holds_alternative<std::vector<double>>(value)) {
                    j["data"][key] = std::get<std::vector<double>>(value);
                } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                    j["data"][key] = std::get<std::vector<std::string>>(value);
                }
            }
        }
        
        return j;
    }
    
    static bool deserializeConfig(Config& config, const json& j) {
        if (!j.contains("type") || j["type"] != "Config") {
            return false;
        }
        
        if (j.contains("data")) {
            for (auto it = j["data"].begin(); it != j["data"].end(); ++it) {
                std::string key = it.key();
                const json& value = it.value();
                
                if (value.is_number_integer()) {
                    config.set(key, value.get<int>());
                } else if (value.is_number_unsigned()) {
                    config.set(key, value.get<uint64_t>());
                } else if (value.is_number()) {
                    config.set(key, value.get<double>());
                } else if (value.is_boolean()) {
                    config.set(key, value.get<bool>());
                } else if (value.is_string()) {
                    config.set(key, value.get<std::string>());
                } else if (value.is_array()) {
                    // Try to determine array type
                    if (!value.empty()) {
                        if (value[0].is_number_integer()) {
                            config.set(key, value.get<std::vector<int>>());
                        } else if (value[0].is_number()) {
                            config.set(key, value.get<std::vector<double>>());
                        } else if (value[0].is_string()) {
                            config.set(key, value.get<std::vector<std::string>>());
                        }
                    }
                }
            }
        }
        
        return true;
    }
    
    static std::string toJsonString(const json& j) {
        return j.dump(2); // Pretty print with 2-space indentation
    }
    
    static json fromJsonString(const std::string& json_str) {
        return json::parse(json_str);
    }
};

// Binary serialization utilities
class BinarySerializer {
public:
    static std::vector<uint8_t> serializeBrain(const Brain& brain, SerializationProgress& progress) {
        std::vector<uint8_t> buffer;
        
        // Serialize brain metadata
        json metadata;
        metadata["type"] = "Brain";
        metadata["version"] = "1.0";
        metadata["neuron_count"] = brain.getTotalNeuronCount();
        metadata["synapse_count"] = brain.getTotalSynapseCount();
        metadata["region_count"] = brain.getRegionCount();
        metadata["step"] = brain.getTotalSpikeCount(); // Using this as placeholder for step
        metadata["simulation_time"] = 0.0; // Placeholder - could be extended
        
        // Serialize regions
        json regions_json;
        auto region_ids = brain.getRegionIds();
        size_t region_progress = 0;
        
        for (const auto& region_id : region_ids) {
            const NeuralRegion* region = brain.getRegion(region_id);
            if (region) {
                json region_json;
                region_json["id"] = region_id.index();
                region_json["name"] = region->getName();
                
                // Serialize neurons
                json neurons_json;
                size_t total_neurons = region->getTotalNeuronCount();
                neurons_json["count"] = total_neurons;
                
                // For now, just store basic stats (full neuron state would require additional data structures)
                neurons_json["stats"] = {
                    {"average_firing_rate", region->getAverageFiringRate()},
                    {"synapse_count", region->getSynapseCount()}
                };
                
                region_json["neurons"] = neurons_json;
                regions_json.push_back(region_json);
            }
            
            region_progress++;
            progress.update(region_progress);
        }
        
        metadata["regions"] = regions_json;
        
        // Add to buffer
        std::string metadata_str = metadata.dump();
        buffer.insert(buffer.end(), metadata_str.begin(), metadata_str.end());
        
        return buffer;
    }
    
    static bool deserializeBrain(Brain& brain, const std::vector<uint8_t>& data, SerializationProgress& progress) {
        try {
            std::string data_str(data.begin(), data.end());
            json j = json::parse(data_str);
            
            if (!j.contains("type") || j["type"] != "Brain") {
                return false;
            }
            
            // Note: This is a simplified deserialization
            // In a full implementation, we would restore the complete brain state
            // For now, just log the metadata
            size_t neuron_count = j.value("neuron_count", 0);
            size_t synapse_count = j.value("synapse_count", 0);
            size_t region_count = j.value("region_count", 0);
            
            return true;
            
        } catch (const json::exception& e) {
            return false;
        }
    }
};

// Incremental serialization utilities
class IncrementalSerializer {
public:
    class ChunkStream {
    public:
        ChunkStream() : position_(0) {}
        
        void addChunk(const std::vector<uint8_t>& chunk, const std::string& chunk_type) {
            chunks_.push_back({chunk_type, chunk, position_});
            position_ += chunk.size();
        }
        
        bool getNextChunk(std::vector<uint8_t>& chunk, std::string& chunk_type) {
            if (current_chunk_index_ >= chunks_.size()) {
                return false;
            }
            
            chunk = chunks_[current_chunk_index_].data;
            chunk_type = chunks_[current_chunk_index_].type;
            current_chunk_index_++;
            return true;
        }
        
        void reset() {
            current_chunk_index_ = 0;
        }
        
        size_t getTotalSize() const {
            return position_;
        }
        
        size_t getChunkCount() const {
            return chunks_.size();
        }
        
    private:
        struct Chunk {
            std::string type;
            std::vector<uint8_t> data;
            size_t offset;
        };
        
        std::vector<Chunk> chunks_;
        size_t position_;
        size_t current_chunk_index_;
    };
    
    static ChunkStream createChunks(const std::vector<uint8_t>& data, size_t chunk_size) {
        ChunkStream stream;
        
        for (size_t i = 0; i < data.size(); i += chunk_size) {
            size_t current_chunk_size = std::min(chunk_size, data.size() - i);
            std::vector<uint8_t> chunk(data.begin() + i, data.begin() + i + current_chunk_size);
            stream.addChunk(chunk, "data");
        }
        
        return stream;
    }
    
    static std::vector<uint8_t> combineChunks(const ChunkStream& stream) {
        std::vector<uint8_t> combined_data;
        combined_data.reserve(stream.getTotalSize());
        
        std::vector<uint8_t> chunk;
        std::string chunk_type;
        while (stream.getNextChunk(chunk, chunk_type)) {
            combined_data.insert(combined_data.end(), chunk.begin(), chunk.end());
        }
        
        return combined_data;
    }
};

// Serialization error types
class SerializationError {
public:
    enum class Type {
        ValidationError,
        CompressionError,
        IoError,
        JsonError,
        UnsupportedFormat,
        ChecksumError,
        UnknownError
    };
    
    SerializationError(Type type, const std::string& message) 
        : type_(type), message_(message) {}
    
    Type getType() const { return type_; }
    const std::string& getMessage() const { return message_; }
    
    std::string getFullMessage() const {
        std::string type_str;
        switch (type_) {
            case Type::ValidationError: type_str = "Validation Error"; break;
            case Type::CompressionError: type_str = "Compression Error"; break;
            case Type::IoError: type_str = "IO Error"; break;
            case Type::JsonError: type_str = "JSON Error"; break;
            case Type::UnsupportedFormat: type_str = "Unsupported Format"; break;
            case Type::ChecksumError: type_str = "Checksum Error"; break;
            case Type::UnknownError: type_str = "Unknown Error"; break;
        }
        return type_str + ": " + message_;
    }
    
private:
    Type type_;
    std::string message_;
};

// ==============================================================================
// Main Module Initialization
// ==============================================================================

PYBIND11_MODULE(pynlm, m) {