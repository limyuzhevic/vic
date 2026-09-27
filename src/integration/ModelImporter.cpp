#include "ModelImporter.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {
namespace integration {

// BinaryModelImporter implementation
std::unique_ptr<NeuralNetworkModel> BinaryModelImporter::importModel(const std::string& filename) {
    NeuralNetworkModel model;
    if (importModel(filename, model)) {
        return std::make_unique<NeuralNetworkModel>(model);
    }
    return nullptr;
}

bool BinaryModelImporter::importModel(const std::string& filename, NeuralNetworkModel& model) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    try {
        // Read metadata
        size_t name_size;
        file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
        std::string name(name_size, '\0');
        file.read(&name[0], name_size);
        model.metadata.name = name;

        size_t desc_size;
        file.read(reinterpret_cast<char*>(&desc_size), sizeof(size_t));
        std::string desc(desc_size, '\0');
        file.read(&desc[0], desc_size);
        model.metadata.description = desc;

        size_t version_size;
        file.read(reinterpret_cast<char*>(&version_size), sizeof(size_t));
        std::string version(version_size, '\0');
        file.read(&version[0], version_size);
        model.metadata.version = version;

        // Read layers
        size_t layer_count;
        file.read(reinterpret_cast<char*>(&layer_count), sizeof(size_t));
        model.layers.resize(layer_count);

        for (size_t i = 0; i < layer_count; ++i) {
            size_t layer_name_size;
            file.read(reinterpret_cast<char*>(&layer_name_size), sizeof(size_t));
            std::string layer_name(layer_name_size, '\0');
            file.read(&layer_name[0], layer_name_size);
            model.layers[i].name = layer_name;

            size_t type_size;
            file.read(reinterpret_cast<char*>(&type_size), sizeof(size_t));
            std::string type(type_size, '\0');
            file.read(&type[0], type_size);
            model.layers[i].type = type;

            file.read(reinterpret_cast<char*>(&model.layers[i].neurons), sizeof(size_t));

            size_t param_count;
            file.read(reinterpret_cast<char*>(&param_count), sizeof(size_t));
            for (size_t j = 0; j < param_count; ++j) {
                size_t key_size;
                file.read(reinterpret_cast<char*>(&key_size), sizeof(size_t));
                std::string key(key_size, '\0');
                file.read(&key[0], key_size);

                double value;
                file.read(reinterpret_cast<char*>(&value), sizeof(double));
                model.layers[i].parameters[key] = value;
            }
        }

        // Read connections
        size_t conn_count;
        file.read(reinterpret_cast<char*>(&conn_count), sizeof(size_t));
        model.connections.resize(conn_count);

        for (size_t i = 0; i < conn_count; ++i) {
            size_t source_size;
            file.read(reinterpret_cast<char*>(&source_size), sizeof(size_t));
            std::string source(source_size, '\0');
            file.read(&source[0], source_size);
            model.connections[i].source_layer = source;

            size_t target_size;
            file.read(reinterpret_cast<char*>(&target_size), sizeof(size_t));
            std::string target(target_size, '\0');
            file.read(&target[0], target_size);
            model.connections[i].target_layer = target;

            size_t conn_list_size;
            file.read(reinterpret_cast<char*>(&conn_list_size), sizeof(size_t));
            model.connections[i].connections.resize(conn_list_size);

            for (size_t j = 0; j < conn_list_size; ++j) {
                size_t src_neuron, tgt_neuron;
                file.read(reinterpret_cast<char*>(&src_neuron), sizeof(size_t));
                file.read(reinterpret_cast<char*>(&tgt_neuron), sizeof(size_t));
                model.connections[i].connections[j] = {src_neuron, tgt_neuron};
            }

            size_t weight_count;
            file.read(reinterpret_cast<char*>(&weight_count), sizeof(size_t));
            model.connections[i].weights.resize(weight_count);

            for (size_t j = 0; j < weight_count; ++j) {
                file.read(reinterpret_cast<char*>(&model.connections[i].weights[j]), sizeof(double));
            }
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error importing binary model: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool BinaryModelImporter::exportModel(const NeuralNetworkModel& model, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    try {
        // Write metadata
        size_t name_size = model.metadata.name.size();
        file.write(reinterpret_cast<const char*>(&name_size), sizeof(size_t));
        file.write(model.metadata.name.c_str(), name_size);

        size_t desc_size = model.metadata.description.size();
        file.write(reinterpret_cast<const char*>(&desc_size), sizeof(size_t));
        file.write(model.metadata.description.c_str(), desc_size);

        size_t version_size = model.metadata.version.size();
        file.write(reinterpret_cast<const char*>(&version_size), sizeof(size_t));
        file.write(model.metadata.version.c_str(), version_size);

        // Write layers
        size_t layer_count = model.layers.size();
        file.write(reinterpret_cast<const char*>(&layer_count), sizeof(size_t));

        for (const auto& layer : model.layers) {
            size_t layer_name_size = layer.name.size();
            file.write(reinterpret_cast<const char*>(&layer_name_size), sizeof(size_t));
            file.write(layer.name.c_str(), layer_name_size);

            size_t type_size = layer.type.size();
            file.write(reinterpret_cast<const char*>(&type_size), sizeof(size_t));
            file.write(layer.type.c_str(), type_size);

            file.write(reinterpret_cast<const char*>(&layer.neurons), sizeof(size_t));

            size_t param_count = layer.parameters.size();
            file.write(reinterpret_cast<const char*>(&param_count), sizeof(size_t));

            for (const auto& [key, value] : layer.parameters) {
                size_t key_size = key.size();
                file.write(reinterpret_cast<const char*>(&key_size), sizeof(size_t));
                file.write(key.c_str(), key_size);
                file.write(reinterpret_cast<const char*>(&value), sizeof(double));
            }
        }

        // Write connections
        size_t conn_count = model.connections.size();
        file.write(reinterpret_cast<const char*>(&conn_count), sizeof(size_t));

        for (const auto& conn : model.connections) {
            size_t source_size = conn.source_layer.size();
            file.write(reinterpret_cast<const char*>(&source_size), sizeof(size_t));
            file.write(conn.source_layer.c_str(), source_size);

            size_t target_size = conn.target_layer.size();
            file.write(reinterpret_cast<const char*>(&target_size), sizeof(size_t));
            file.write(conn.target_layer.c_str(), target_size);

            size_t conn_list_size = conn.connections.size();
            file.write(reinterpret_cast<const char*>(&conn_list_size), sizeof(size_t));

            for (const auto& pair : conn.connections) {
                file.write(reinterpret_cast<const char*>(&pair.first), sizeof(size_t));
                file.write(reinterpret_cast<const char*>(&pair.second), sizeof(size_t));
            }

            size_t weight_count = conn.weights.size();
            file.write(reinterpret_cast<const char*>(&weight_count), sizeof(size_t));

            for (double weight : conn.weights) {
                file.write(reinterpret_cast<const char*>(&weight), sizeof(double));
            }
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting binary model: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool BinaryModelImporter::validateModel(const NeuralNetworkModel& model) const {
    // Basic validation
    if (model.layers.empty()) {
        return false;
    }

    // Check layer sizes
    for (const auto& layer : model.layers) {
        if (layer.neurons == 0) {
            return false;
        }
    }

    // Check connections
    for (const auto& conn : model.connections) {
        // Verify that source and target layers exist
        bool source_found = false, target_found = false;
        for (const auto& layer : model.layers) {
            if (layer.name == conn.source_layer) source_found = true;
            if (layer.name == conn.target_layer) target_found = true;
        }

        if (!source_found || !target_found) {
            return false;
        }
    }

    return true;
}

std::vector<std::string> BinaryModelImporter::getValidationErrors(const NeuralNetworkModel& model) const {
    std::vector<std::string> errors;

    if (model.layers.empty()) {
        errors.push_back("Model must have at least one layer");
    }

    // Check for duplicate layer names
    std::set<std::string> layer_names;
    for (const auto& layer : model.layers) {
        if (layer_names.count(layer.name)) {
            errors.push_back("Duplicate layer name: " + layer.name);
        }
        layer_names.insert(layer.name);
    }

    // Check for duplicate connection specifications
    std::set<std::pair<std::string, std::string>> connection_pairs;
    for (const auto& conn : model.connections) {
        std::pair<std::string, std::string> pair = {conn.source_layer, conn.target_layer};
        if (connection_pairs.count(pair)) {
            errors.push_back("Duplicate connection from " + conn.source_layer + " to " + conn.target_layer);
        }
        connection_pairs.insert(pair);
    }

    return errors;
}

std::unique_ptr<NeuralNetworkModel> BinaryModelImporter::convertModel(const NeuralNetworkModel& source_model,
                                                                    ModelFormat target_format) {
    // For now, return a copy - in real implementation would convert format
    if (target_format != ModelFormat::NLM_BINARY) {
        return nullptr; // Not implemented
    }

    return std::make_unique<NeuralNetworkModel>(source_model);
}

// JSONModelImporter implementation
std::unique_ptr<NeuralNetworkModel> JSONModelImporter::importModel(const std::string& filename) {
    NeuralNetworkModel model;
    if (importModel(filename, model)) {
        return std::make_unique<NeuralNetworkModel>(model);
    }
    return nullptr;
}

bool JSONModelImporter::importModel(const std::string& filename, NeuralNetworkModel& model) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json_string = buffer.str();

    rapidjson::Document document;
    if (document.Parse(json_string.c_str()).HasParseError()) {
        return false;
    }

    if (!document.IsObject()) {
        return false;
    }

    parseMetadata(document, model.metadata);
    parseLayers(document, model.layers);
    parseConnections(document, model.connections);
    parseNeuronModel(document, model.neuron_model);
    parseSynapseModel(document, model.synapse_model);

    return true;
}

bool JSONModelImporter::exportModel(const NeuralNetworkModel& model, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        return false;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();

    // Serialize all components
    serializeMetadata(writer, model.metadata);
    serializeLayers(writer, model.layers);
    serializeConnections(writer, model.connections);
    serializeNeuronModel(writer, model.neuron_model);
    serializeSynapseModel(writer, model.synapse_model);

    writer.EndObject();

    file << buffer.GetString();
    return true;
}

bool JSONModelImporter::validateModel(const NeuralNetworkModel& model) const {
    // Basic JSON validation
    if (model.layers.empty()) {
        return false;
    }

    // Check for valid layer types
    std::set<std::string> valid_layer_types = {"input", "hidden", "output", "conv", "lstm", "gru"};
    for (const auto& layer : model.layers) {
        if (valid_layer_types.find(layer.type) == valid_layer_types.end()) {
            return false;
        }
    }

    return true;
}

std::vector<std::string> JSONModelImporter::getValidationErrors(const NeuralNetworkModel& model) const {
    std::vector<std::string> errors;

    if (model.layers.empty()) {
        errors.push_back("Model must have at least one layer");
    }

    // Check for missing required fields
    if (model.metadata.name.empty()) {
        errors.push_back("Model metadata missing name");
    }

    return errors;
}

std::unique_ptr<NeuralNetworkModel> JSONModelImporter::convertModel(const NeuralNetworkModel& source_model,
                                                                 ModelFormat target_format) {
    if (target_format != ModelFormat::NLM_JSON) {
        return nullptr; // Not implemented
    }

    // Create a copy with JSON-specific serialization
    auto copy = std::make_unique<NeuralNetworkModel>(source_model);
    return copy;
}

void JSONModelImporter::parseMetadata(const rapidjson::Value& json, ModelMetadata& metadata) {
    if (json.HasMember("name") && json["name"].IsString()) {
        metadata.name = json["name"].GetString();
    }
    if (json.HasMember("description") && json["description"].IsString()) {
        metadata.description = json["description"].GetString();
    }
    if (json.HasMember("version") && json["version"].IsString()) {
        metadata.version = json["version"].GetString();
    }
    if (json.HasMember("author") && json["author"].IsString()) {
        metadata.author = json["author"].GetString();
    }
    if (json.HasMember("date") && json["date"].IsString()) {
        metadata.date = json["date"].GetString();
    }
}

void JSONModelImporter::parseLayers(const rapidjson::Value& json, std::vector<NeuralLayer>& layers) {
    if (!json.HasMember("layers") || !json["layers"].IsArray()) {
        return;
    }

    const rapidjson::Value& layers_json = json["layers"];
    layers.resize(layers_json.Size());

    for (size_t i = 0; i < layers_json.Size(); ++i) {
        const rapidjson::Value& layer_json = layers_json[i];
        NeuralLayer layer;

        if (layer_json.HasMember("name") && layer_json["name"].IsString()) {
            layer.name = layer_json["name"].GetString();
        }
        if (layer_json.HasMember("type") && layer_json["type"].IsString()) {
            layer.type = layer_json["type"].GetString();
        }
        if (layer_json.HasMember("neurons") && layer_json["neurons"].IsUint()) {
            layer.neurons = layer_json["neurons"].GetUint();
        }

        if (layer_json.HasMember("parameters") && layer_json["parameters"].IsObject()) {
            const rapidjson::Value& params_json = layer_json["parameters"];
            for (auto it = params_json.MemberBegin(); it != params_json.MemberEnd(); ++it) {
                if (it->value.IsDouble()) {
                    layer.parameters[it->name.GetString()] = it->value.GetDouble();
                }
            }
        }

        layers[i] = layer;
    }
}

void JSONModelImporter::parseConnections(const rapidjson::Value& json, std::vector<Connection>& connections) {
    if (!json.HasMember("connections") || !json["connections"].IsArray()) {
        return;
    }

    const rapidjson::Value& connections_json = json["connections"];
    connections.resize(connections_json.Size());

    for (size_t i = 0; i < connections_json.Size(); ++i) {
        const rapidjson::Value& conn_json = connections_json[i];
        Connection conn;

        if (conn_json.HasMember("source_layer") && conn_json["source_layer"].IsString()) {
            conn.source_layer = conn_json["source_layer"].GetString();
        }
        if (conn_json.HasMember("target_layer") && conn_json["target_layer"].IsString()) {
            conn.target_layer = conn_json["target_layer"].GetString();
        }

        if (conn_json.HasMember("connections") && conn_json["connections"].IsArray()) {
            const rapidjson::Value& conn_list_json = conn_json["connections"];
            conn.connections.resize(conn_list_json.Size());

            for (size_t j = 0; j < conn_list_json.Size(); ++j) {
                if (conn_list_json[j].IsArray() && conn_list_json[j].Size() == 2) {
                    conn.connections[j].first = conn_list_json[j][0].GetUint();
                    conn.connections[j].second = conn_list_json[j][1].GetUint();
                }
            }
        }

        if (conn_json.HasMember("weights") && conn_json["weights"].IsArray()) {
            const rapidjson::Value& weights_json = conn_json["weights"];
            conn.weights.resize(weights_json.Size());

            for (size_t j = 0; j < weights_json.Size(); ++j) {
                if (weights_json[j].IsDouble()) {
                    conn.weights[j] = weights_json[j].GetDouble();
                }
            }
        }

        connections[i] = conn;
    }
}

void JSONModelImporter::parseNeuronModel(const rapidjson::Value& json, NeuronModel& neuron_model) {
    if (!json.HasMember("neuron_model") || !json["neuron_model"].IsObject()) {
        return;
    }

    const rapidjson::Value& nm_json = json["neuron_model"];

    if (nm_json.HasMember("name") && nm_json["name"].IsString()) {
        neuron_model.name = nm_json["name"].GetString();
    }
    if (nm_json.HasMember("type") && nm_json["type"].IsString()) {
        neuron_model.type = nm_json["type"].GetString();
    }

    if (nm_json.HasMember("parameters") && nm_json["parameters"].IsObject()) {
        const rapidjson::Value& params_json = nm_json["parameters"];
        for (auto it = params_json.MemberBegin(); it != params_json.MemberEnd(); ++it) {
            if (it->value.IsDouble()) {
                neuron_model.parameters[it->name.GetString()] = it->value.GetDouble();
            }
        }
    }
}

void JSONModelImporter::parseSynapseModel(const rapidjson::Value& json, SynapseModel& synapse_model) {
    if (!json.HasMember("synapse_model") || !json["synapse_model"].IsObject()) {
        return;
    }

    const rapidjson::Value& sm_json = json["synapse_model"];

    if (sm_json.HasMember("name") && sm_json["name"].IsString()) {
        synapse_model.name = sm_json["name"].GetString();
    }
    if (sm_json.HasMember("type") && sm_json["type"].IsString()) {
        synapse_model.type = sm_json["type"].GetString();
    }

    if (sm_json.HasMember("parameters") && sm_json["parameters"].IsObject()) {
        const rapidjson::Value& params_json = sm_json["parameters"];
        for (auto it = params_json.MemberBegin(); it != params_json.MemberEnd(); ++it) {
            if (it->value.IsDouble()) {
                synapse_model.parameters[it->name.GetString()] = it->value.GetDouble();
            }
        }
    }
}

void JSONModelImporter::serializeMetadata(rapidjson::Writer<rapidjson::StringBuffer>& writer, const ModelMetadata& metadata) {
    writer.Key("metadata");
    writer.StartObject();

    writer.Key("name");
    writer.String(metadata.name.c_str());

    if (!metadata.description.empty()) {
        writer.Key("description");
        writer.String(metadata.description.c_str());
    }

    if (!metadata.version.empty()) {
        writer.Key("version");
        writer.String(metadata.version.c_str());
    }

    if (!metadata.author.empty()) {
        writer.Key("author");
        writer.String(metadata.author.c_str());
    }

    if (!metadata.date.empty()) {
        writer.Key("date");
        writer.String(metadata.date.c_str());
    }

    if (!metadata.tags.empty()) {
        writer.Key("tags");
        writer.String(metadata.tags.c_str());
    }

    writer.EndObject();
}

void JSONModelImporter::serializeLayers(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<NeuralLayer>& layers) {
    writer.Key("layers");
    writer.StartArray();

    for (const auto& layer : layers) {
        writer.StartObject();

        writer.Key("name");
        writer.String(layer.name.c_str());

        writer.Key("type");
        writer.String(layer.type.c_str());

        writer.Key("neurons");
        writer.Uint(layer.neurons);

        if (!layer.parameters.empty()) {
            writer.Key("parameters");
            writer.StartObject();

            for (const auto& [key, value] : layer.parameters) {
                writer.Key(key.c_str());
                writer.Double(value);
            }

            writer.EndObject();
        }

        writer.EndObject();
    }

    writer.EndArray();
}

void JSONModelImporter::serializeConnections(rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::vector<Connection>& connections) {
    writer.Key("connections");
    writer.StartArray();

    for (const auto& conn : connections) {
        writer.StartObject();

        writer.Key("source_layer");
        writer.String(conn.source_layer.c_str());

        writer.Key("target_layer");
        writer.String(conn.target_layer.c_str());

        writer.Key("connections");
        writer.StartArray();

        for (const auto& pair : conn.connections) {
            writer.StartArray();
            writer.Uint(pair.first);
            writer.Uint(pair.second);
            writer.EndArray();
        }

        writer.EndArray();

        writer.Key("weights");
        writer.StartArray();

        for (double weight : conn.weights) {
            writer.Double(weight);
        }

        writer.EndArray();

        writer.EndObject();
    }

    writer.EndArray();
}

void JSONModelImporter::serializeNeuronModel(rapidjson::Writer<rapidjson::StringBuffer>& writer, const NeuronModel& neuron_model) {
    writer.Key("neuron_model");
    writer.StartObject();

    writer.Key("name");
    writer.String(neuron_model.name.c_str());

    writer.Key("type");
    writer.String(neuron_model.type.c_str());

    if (!neuron_model.parameters.empty()) {
        writer.Key("parameters");
        writer.StartObject();

        for (const auto& [key, value] : neuron_model.parameters) {
            writer.Key(key.c_str());
            writer.Double(value);
        }

        writer.EndObject();
    }

    writer.EndObject();
}

void JSONModelImporter::serializeSynapseModel(rapidjson::Writer<rapidjson::StringBuffer>& writer, const SynapseModel& synapse_model) {
    writer.Key("synapse_model");
    writer.StartObject();

    writer.Key("name");
    writer.String(synapse_model.name.c_str());

    writer.Key("type");
    writer.String(synapse_model.type.c_str());

    if (!synapse_model.parameters.empty()) {
        writer.Key("parameters");
        writer.StartObject();

        for (const auto& [key, value] : synapse_model.parameters) {
            writer.Key(key.c_str());
            writer.Double(value);
        }

        writer.EndObject();
    }

    writer.EndObject();
}

// ModelImporterRegistry implementation
bool ModelImporterRegistry::registerImporter(ModelFormat format, std::unique_ptr<ModelImporter> importer) {
    if (!importer) {
        return false;
    }

    importers_[format] = std::move(importer);
    return true;
}

bool ModelImporterRegistry::unregisterImporter(ModelFormat format) {
    return importers_.erase(format) > 0;
}

ModelImporter* ModelImporterRegistry::getImporter(ModelFormat format) const {
    auto it = importers_.find(format);
    if (it != importers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool ModelImporterRegistry::registerFormatExtension(const std::string& extension, ModelFormat format) {
    format_extensions_[extension] = format;
    return true;
}

ModelFormat ModelImporterRegistry::detectFormat(const std::string& filename) const {
    std::string extension = std::filesystem::path(filename).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (format_extensions_.count(extension)) {
        return format_extensions_[extension];
    }

    if (auto_detect_enabled_) {
        // Try to detect based on file content
        std::ifstream file(filename);
        if (!file) {
            return ModelFormat::NLM_JSON; // Default fallback
        }

        std::string first_line;
        std::getline(file, first_line);
        file.close();

        // Check for magic bytes or content patterns
        if (first_line.find('{') != std::string::npos && first_line.find('}') != std::string::npos) {
            return ModelFormat::NLM_JSON;
        }
    }

    return ModelFormat::NLM_JSON; // Default fallback
}

std::vector<ModelFormat> ModelImporterRegistry::getSupportedFormats() const {
    std::vector<ModelFormat> formats;
    for (const auto& [format, importer] : importers_) {
        formats.push_back(format);
    }
    return formats;
}

std::unique_ptr<NeuralNetworkModel> ModelImporterRegistry::importModel(const std::string& filename) {
    ModelFormat format = detectFormat(filename);

    ModelImporter* importer = getImporter(format);
    if (importer) {
        return importer->importModel(filename);
    }

    return nullptr;
}

bool ModelImporterRegistry::importModel(const std::string& filename, NeuralNetworkModel& model) {
    ModelFormat format = detectFormat(filename);

    ModelImporter* importer = getImporter(format);
    if (importer) {
        return importer->importModel(filename, model);
    }

    return false;
}

bool ModelImporterRegistry::exportModel(const NeuralNetworkModel& model, const std::string& filename) {
    // Try to determine format from extension
    std::string extension = std::filesystem::path(filename).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    ModelFormat format = ModelFormat::NLM_JSON; // Default
    if (extension == ".nlmbin") {
        format = ModelFormat::NLM_BINARY;
    } else if (extension == ".json") {
        format = ModelFormat::NLM_JSON;
    }

    ModelImporter* importer = getImporter(format);
    if (importer) {
        return importer->exportModel(model, filename);
    }

    return false;
}

std::unique_ptr<NeuralNetworkModel> ModelImporterRegistry::convertModel(const NeuralNetworkModel& source_model,
                                                                       ModelFormat target_format) {
    ModelImporter* source_importer = getImporter(static_cast<ModelFormat>(0)); // Use first available importer
    if (!source_importer) {
        return nullptr;
    }

    ModelImporter* target_importer = getImporter(target_format);
    if (!target_importer) {
        return nullptr;
    }

    return target_importer->convertModel(source_model, target_format);
}

bool ModelImporterRegistry::validateModel(const NeuralNetworkModel& model) const {
    for (const auto& [format, importer] : importers_) {
        if (importer->validateModel(model)) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ModelImporterRegistry::getValidationErrors(const NeuralNetworkModel& model) const {
    std::vector<std::string> all_errors;

    for (const auto& [format, importer] : importers_) {
        std::vector<std::string> errors = importer->getValidationErrors(model);
        all_errors.insert(all_errors.end(), errors.begin(), errors.end());
    }

    return all_errors;
}

std::string ModelImporterRegistry::getSupportedFormatsString() const {
    std::string result;
    for (const auto& [format, importer] : importers_) {
        if (!result.empty()) {
            result += ", ";
        }

        switch (format) {
            case ModelFormat::NLM_BINARY: result += "Binary (.nlmbin)"; break;
            case ModelFormat::NLM_JSON: result += "JSON (.json)"; break;
            case ModelFormat::NEURAL_CKPT: result += "Checkpoint (.ckpt)"; break;
            case ModelFormat::H5PY: result += "HDF5 (.h5)"; break;
            case ModelFormat::ONNX: result += "ONNX (.onnx)"; break;
            case ModelFormat::TENSORFLOW: result += "TensorFlow (.tf)"; break;
            case ModelFormat::PYTORCH: result += "PyTorch (.pt)"; break;
            case ModelFormat::CAFFE: result += "Caffe (.caffemodel)"; break;
            case ModelFormat::CUSTOM: result += "Custom"; break;
        }
    }

    return result;
}

} // namespace integration
} // namespace nlm