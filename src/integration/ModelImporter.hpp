#ifndef MODEL_IMPORTER_HPP
#define MODEL_IMPORTER_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace nlm {
namespace integration {

// Model format definitions
enum class ModelFormat {
    NLM_BINARY,
    NLM_JSON,
    NEURAL_CKPT,
    H5PY,
    ONNX,
    TENSORFLOW,
    PYTORCH,
    CAFFE,
    CUSTOM
};

// Model metadata
struct ModelMetadata {
    std::string name;                    // Model name
    std::string description;              // Description
    std::string version;                  // Version
    std::string author;                   // Author
    std::string date;                     // Creation date
    std::string tags;                     // Tags for categorization
    std::map<std::string, std::string> custom_metadata;  // Custom metadata fields
};

// Model component structures
struct NeuralLayer {
    std::string name;                    // Layer name
    std::string type;                    // Layer type
    size_t neurons;                       // Number of neurons
    std::map<std::string, double> parameters;  // Layer parameters
    std::vector<double> weights;         // Layer weights (flattened)
    std::vector<double> biases;          // Layer biases
    std::map<std::string, std::string> attributes;  // Additional attributes
};

struct Connection {
    std::string source_layer;            // Source layer name
    std::string target_layer;            // Target layer name
    std::vector<std::pair<size_t, size_t>> connections;  // (source_neuron, target_neuron)
    std::vector<double> weights;         // Connection weights
    std::vector<double> delays;          // Connection delays
    std::map<std::string, double> properties;  // Connection properties
};

struct NeuronModel {
    std::string name;                    // Neuron model name
    std::string type;                    // Model type (LIF, Izhikevich, etc.)
    std::map<std::string, double> parameters;  // Model parameters
    std::map<std::string, std::string> dynamics;  // Differential equations
};

struct SynapseModel {
    std::string name;                    // Synapse model name
    std::string type;                    // Synapse type
    std::map<std::string, double> parameters;  // Synapse parameters
    std::string plasticity_rule;         // Plasticity rule
    std::map<std::string, double> plasticity_params;  // Plasticity parameters
};

// Full model structure
struct NeuralNetworkModel {
    ModelMetadata metadata;              // Model metadata
    std::vector<NeuralLayer> layers;     // Network layers
    std::vector<Connection> connections; // Network connections
    NeuronModel neuron_model;            // Default neuron model
    SynapseModel synapse_model;          // Default synapse model
    
    // Derived information
    size_t input_size;                   // Input layer size
    size_t output_size;                   // Output layer size
    std::vector<std::string> layer_names; // Layer names for reference
};

// Model importer interface
class ModelImporter {
public:
    virtual ~ModelImporter() = default;
    
    // Format detection
    virtual bool canImport(const std::string& filename) const = 0;
    virtual ModelFormat getSupportedFormat() const = 0;
    
    // Import operations
    virtual std::unique_ptr<NeuralNetworkModel> importModel(const std::string& filename) = 0;
    virtual bool importModel(const std::string& filename, NeuralNetworkModel& model) = 0;
    
    // Export operations
    virtual bool exportModel(const NeuralNetworkModel& model, const std::string& filename) = 0;
    virtual bool exportModel(const NeuralNetworkModel& model, std::ostream& output_stream) = 0;
    
    // Validation
    virtual bool validateModel(const NeuralNetworkModel& model) const = 0;
    virtual std::vector<std::string> getValidationErrors(const NeuralNetworkModel& model) const = 0;
    
    // Model conversion
    virtual std::unique_ptr<NeuralNetworkModel> convertModel(const NeuralNetworkModel& source_model,
                                                           ModelFormat target_format) = 0;
};

// Binary format importer
class BinaryModelImporter : public ModelImporter {
public:
    bool canImport(const std::string& filename) const override {
        return std::filesystem::path(filename).extension() == ".nlmbin";
    }
    
    ModelFormat getSupportedFormat() const override {
        return ModelFormat::NLM_BINARY;
    }
    
    std::unique_ptr<NeuralNetworkModel> importModel(const std::string& filename) override;
    bool importModel(const std::string& filename, NeuralNetworkModel& model) override;
    
    bool exportModel(const NeuralNetworkModel& model, const std::string& filename) override;
    bool exportModel(const NeuralNetworkModel& model, std::ostream& output_stream) override;
    
    bool validateModel(const NeuralNetworkModel& model) const override;
    std::vector<std::string> getValidationErrors(const NeuralNetworkModel& model) const override;
    
    std::unique_ptr<NeuralNetworkModel> convertModel(const NeuralNetworkModel& source_model,
                                                   ModelFormat target_format) override;
};

// JSON format importer
class JSONModelImporter : public ModelImporter {
public:
    bool canImport(const std::string& filename) const override {
        return std::filesystem::path(filename).extension() == ".json";
    }
    
    ModelFormat getSupportedFormat() const override {
        return ModelFormat::NLM_JSON;
    }
    
    std::unique_ptr<NeuralNetworkModel> importModel(const std::string& filename) override;
    bool importModel(const std::string& filename, NeuralNetworkModel& model) override;
    
    bool exportModel(const NeuralNetworkModel& model, const std::string& filename) override;
    bool exportModel(const NeuralNetworkModel& model, std::ostream& output_stream) override;
    
    bool validateModel(const NeuralNetworkModel& model) const override;
    std::vector<std::string> getValidationErrors(const NeuralNetworkModel& model) const override;
    
    std::unique_ptr<NeuralNetworkModel> convertModel(const NeuralNetworkModel& source_model,
                                                   ModelFormat target_format) override;

private:
    // JSON parsing helpers
    void parseMetadata(const rapidjson::Value& json, ModelMetadata& metadata);
    void parseLayers(const rapidjson::Value& json, std::vector<NeuralLayer>& layers);
    void parseConnections(const rapidjson::Value& json, std::vector<Connection>& connections);
    void parseNeuronModel(const rapidjson::Value& json, NeuronModel& neuron_model);
    void parseSynapseModel(const rapidjson::Value& json, SynapseModel& synapse_model);
    
    // JSON serialization helpers
    void serializeMetadata(rapidjson::Writer<rapidjson::StringBuffer>& writer, const ModelMetadata& metadata);
    void serializeLayers(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<NeuralLayer>& layers);
    void serializeConnections(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<Connection>& connections);
    void serializeNeuronModel(rapidjson::Writer<rapidjson::StringBuffer>& writer, const NeuronModel& neuron_model);
    void serializeSynapseModel(rapidjson::Writer<rapidjson::StringBuffer>& writer, const SynapseModel& synapse_model);
};

// Model importer registry
class ModelImporterRegistry {
private:
    std::map<ModelFormat, std::unique_ptr<ModelImporter>> importers_;
    std::map<std::string, ModelFormat> format_extensions_;
    bool auto_detect_enabled_;

public:
    ModelImporterRegistry() : auto_detect_enabled_(true) {}
    
    // Plugin management
    bool registerImporter(ModelFormat format, std::unique_ptr<ModelImporter> importer);
    bool unregisterImporter(ModelFormat format);
    ModelImporter* getImporter(ModelFormat format) const;
    
    // Format discovery
    bool registerFormatExtension(const std::string& extension, ModelFormat format);
    ModelFormat detectFormat(const std::string& filename) const;
    std::vector<ModelFormat> getSupportedFormats() const;
    
    // Import/export operations
    std::unique_ptr<NeuralNetworkModel> importModel(const std::string& filename);
    bool importModel(const std::string& filename, NeuralNetworkModel& model);
    bool exportModel(const NeuralNetworkModel& model, const std::string& filename);
    
    // Model conversion
    std::unique_ptr<NeuralNetworkModel> convertModel(const NeuralNetworkModel& source_model,
                                                   ModelFormat target_format);
    
    // Validation and error handling
    bool validateModel(const NeuralNetworkModel& model) const;
    std::vector<std::string> getValidationErrors(const NeuralNetworkModel& model) const;
    
    // Configuration
    void enableAutoDetection(bool enable) { auto_detect_enabled_ = enable; }
    bool isAutoDetectionEnabled() const { return auto_detect_enabled_; }
    
    // Statistics
    size_t getImporterCount() const { return importers_.size(); }
    std::string getSupportedFormatsString() const;
};

} // namespace integration
} // namespace nlm

#endif // MODEL_IMPORTER_HPP