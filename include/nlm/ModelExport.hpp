// ModelExport.hpp - Export functionality for NLM models
#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Brain.hpp"

namespace nlm {

class ModelExporter {
public:
    virtual ~ModelExporter() = default;
    virtual std::string getFormatName() const = 0;
    virtual std::string getFileExtension() const = 0;
    virtual bool exportModel(const std::shared_ptr<Brain>& brain, 
                            const std::string& filepath,
                            const std::unordered_map<std::string, std::string>& options = {}) = 0;
    virtual bool canExport(const std::shared_ptr<Brain>& brain) const = 0;
};

// ONNX model exporter
class ONNXExporter : public ModelExporter {
public:
    std::string getFormatName() const override { return "ONNX"; }
    std::string getFileExtension() const override { return ".onnx"; }
    bool exportModel(const std::shared_ptr<Brain>& brain, 
                    const std::string& filepath,
                    const std::unordered_map<std::string, std::string>& options) override;
    bool canExport(const std::shared_ptr<Brain>& brain) const override;
};

// TensorFlow SavedModel exporter
class TensorFlowExporter : public ModelExporter {
public:
    std::string getFormatName() const override { return "TensorFlow SavedModel"; }
    std::string getFileExtension() const override { return "_saved_model"; }
    bool exportModel(const std::shared_ptr<Brain>& brain,
                    const std::string& filepath,
                    const std::unordered_map<std::string, std::string>& options) override;
    bool canExport(const std::shared_ptr<Brain>& brain) const override;
};

// NumPy pickle exporter
class NumPyExporter : public ModelExporter {
public:
    std::string getFormatName() const override { return "NumPy"; }
    std::string getFileExtension() const override { return ".npz"; }
    bool exportModel(const std::shared_ptr<Brain>& brain,
                    const std::string& filepath,
                    const std::unordered_map<std::string, std::string>& options) override;
    bool canExport(const std::shared_ptr<Brain>& brain) const override;
};

// JSON model specification exporter
class JSONExporter : public ModelExporter {
public:
    std::string getFormatName() const override { return "JSON"; }
    std::string getFileExtension() const override { return ".json"; }
    bool exportModel(const std::shared_ptr<Brain>& brain,
                    const std::string& filepath,
                    const std::unordered_map<std::string, std::string>& options) override;
    bool canExport(const std::shared_ptr<Brain>& brain) const override;
};

// Model import functionality
class ModelImporter {
public:
    virtual ~ModelImporter() = default;
    virtual std::string getFormatName() const = 0;
    virtual bool importModel(std::shared_ptr<Brain>& brain,
                            const std::string& filepath,
                            const std::unordered_map<std::string, std::string>& options = {}) = 0;
    virtual bool canImport(const std::string& filepath) const = 0;
};

// Model format registry
class ModelFormatRegistry {
public:
    static ModelFormatRegistry& getInstance();
    
    // Exporter registration
    void registerExporter(std::unique_ptr<ModelExporter> exporter);
    
    // Importer registration  
    void registerImporter(std::unique_ptr<ModelImporter> importer);
    
    // Format detection
    std::vector<std::string> getSupportedExportFormats() const;
    std::vector<std::string> getSupportedImportFormats() const;
    std::string detectFormat(const std::string& filepath) const;
    
    // Export/Import operations
    bool exportModel(const std::shared_ptr<Brain>& brain,
                    const std::string& filepath,
                    const std::unordered_map<std::string, std::string>& options = {});
    
    bool importModel(std::shared_ptr<Brain>& brain,
                    const std::string& filepath,
                    const std::unordered_map<std::string, std::string>& options = {});
    
    // Format information
    std::string getExporterInfo(const std::string& format) const;
    std::string getImporterInfo(const std::string& format) const;
    
private:
    ModelFormatRegistry() = default;
    ~ModelFormatRegistry() = default;
    ModelFormatRegistry(const ModelFormatRegistry&) = delete;
    ModelFormatRegistry& operator=(const ModelFormatRegistry&) = delete;
    
    std::vector<std::unique_ptr<ModelExporter>> exporters_;
    std::vector<std::unique_ptr<ModelImporter>> importers_;
};
}