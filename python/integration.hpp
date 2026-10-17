#pragma once

// Integration and interoperability for NLM
// Provides integration with Python ML libraries (NumPy, SciPy),
// data import/export capabilities, serialization formats (JSON, pickle, custom),
// and cross-language interoperability

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <pybind11/pybind11.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {
namespace python {

// Python ML library integration
struct NumPyArrayInfo {
    py::array_t<float> array;
    std::vector<size_t> shape;
    std::string dtype;
    bool isContiguous;
    double memoryUsage;
    std::chrono::system_clock::time_point timestamp;
};

struct SciPyStats {
    std::string operation;
    double result;
    std::string confidence;
    std::map<std::string, double> parameters;
    std::chrono::system_clock::time_point computedAt;
};

// Data import/export configuration
struct DataExportConfig {
    std::string filepath;
    std::string format;
    bool compress;
    double compressionRatio;
    std::map<std::string, std::string> options;
    std::chrono::system_clock::time_point exportTime;
    bool success;
    std::string errorMessage;
};

struct DataImportConfig {
    std::string filepath;
    std::string format;
    bool validate;
    bool transpose;
    std::vector<size_t> expectedShape;
    std::chrono::system_clock::time_point importTime;
    bool success;
    std::string errorMessage;
    std::map<std::string, std::string> metadata;
};

// Serialization formats
struct SerializationConfig {
    std::string format; // "json", "pickle", "custom"
    bool compress;
    bool encrypt;
    std::string encryptionKey;
    std::map<std::string, std::string> customOptions;
    std::chrono::system_clock::time_point serializationTime;
};

// Cross-language interoperability
struct ForeignFunction {
    std::string name;
    std::string language;
    std::string signature;
    std::function<py::object(const py::args&)> implementation;
    std::map<std::string, std::string> metadata;
    bool isRegistered;
    std::chrono::system_clock::time_point registeredAt;
};

struct LanguageInterface {
    std::string language;
    std::string version;
    std::map<std::string, std::string> capabilities;
    std::vector<std::string> supportedFormats;
    std::map<std::string, std::function<py::object(const py::args&)>> functions;
    bool isActive;
};

// Integration and interoperability system
class IntegrationSystem {
public:
    // Integration with Python ML libraries (NumPy, SciPy)
    static NumPyArrayInfo convertToNumPyFormat(const std::vector<float>& data);
    static NumPyArrayInfo convertToNumPyFormat(const std::vector<std::vector<float>>& data2D);
    static std::vector<float> convertFromNumPyFormat(const py::array_t<float>& array);
    static std::vector<std::vector<float>> convertFromNumPyFormat2D(const py::array_t<float>& array);
    static py::array_t<float> createNumPyArray(const std::vector<size_t>& shape);
    static py::array_t<float> createNumPyArray(const std::vector<std::vector<float>>& data2D);
    static SciPyStats computeStatisticalAnalysis(const py::array_t<float>& data);
    static SciPyStats computeNeuralNetworkMetrics(const py::array_t<float>& predictions,
                                                  const py::array_t<float>& targets);
    static SciPyStats computeSimilarityMatrix(const std::vector<std::vector<float>>& data);
    static SciPyStats computeClusteringAnalysis(const py::array_t<float>& data,
                                               const std::string& method = "kmeans");
    static SciPyStats computeDimensionalityReduction(const py::array_t<float>& data,
                                                    const std::string& method = "pca",
                                                    size_t targetDimensions = 2);
    static bool isNumPyAvailable();
    static bool isSciPyAvailable();
    static std::string getNumPyVersion();
    static std::string getSciPyVersion();
    
    // Data import/export capabilities
    static DataExportConfig exportBrainToJSON(std::shared_ptr<Brain> brain, const std::string& filepath);
    static DataExportConfig exportBrainToCSV(std::shared_ptr<Brain> brain, const std::string& filepath);
    static DataExportConfig exportBrainToHDF5(std::shared_ptr<Brain> brain, const std::string& filepath);
    static DataExportConfig exportBrainToBinary(std::shared_ptr<Brain> brain, const std::string& filepath);
    static DataExportConfig exportDataToCSV(const std::string& filepath, const std::vector<std::vector<double>>& data);
    static DataExportConfig exportDataToJSON(const std::string& filepath, const json& data);
    static DataExportConfig exportDataToHDF5(const std::string& filepath, const std::vector<std::vector<double>>& data);
    
    static DataImportConfig importBrainFromJSON(const std::string& filepath);
    static DataImportConfig importBrainFromCSV(const std::string& filepath);
    static DataImportConfig importBrainFromHDF5(const std::string& filepath);
    static DataImportConfig importBrainFromBinary(const std::string& filepath);
    static DataImportConfig importDataFromCSV(const std::string& filepath);
    static DataImportConfig importDataFromJSON(const std::string& filepath);
    static DataImportConfig importDataFromHDF5(const std::string& filepath);
    
    static std::shared_ptr<Brain> loadBrainFromFile(const std::string& filepath, const std::string& format = "auto");
    static bool saveBrainToFile(std::shared_ptr<Brain> brain, const std::string& filepath, const std::string& format = "auto");
    static json exportBrainToJSONStructure(std::shared_ptr<Brain> brain);
    static std::shared_ptr<Brain> importBrainFromJSONStructure(const json& data);
    
    // Serialization formats (JSON, pickle, custom)
    static std::string serializeToJSON(std::shared_ptr<Brain> brain);
    static std::string serializeToJSON(const std::map<std::string, std::variant<int, float, bool, std::string>>& data);
    static std::string serializeToJSON(const std::vector<std::vector<float>>& data2D);
    static std::shared_ptr<Brain> deserializeFromJSON(const std::string& jsonString);
    static std::map<std::string, std::variant<int, float, bool, std::string>> deserializeFromJSONMap(const std::string& jsonString);
    static std::vector<std::vector<float>> deserializeFromJSON2D(const std::string& jsonString);
    static std::string serializeToPickle(std::shared_ptr<Brain> brain);
    static std::shared_ptr<Brain> deserializeFromPickle(const std::string& pickleData);
    static std::string serializeToCustomFormat(std::shared_ptr<Brain> brain, const SerializationConfig& config);
    static std::shared_ptr<Brain> deserializeFromCustomFormat(const std::string& data, const SerializationConfig& config);
    static bool registerCustomSerializer(const std::string& format, 
                                        const std::function<std::string(std::shared_ptr<Brain>)>& serializeFunc,
                                        const std::function<std::shared_ptr<Brain>(const std::string&)>& deserializeFunc);
    static std::vector<std::string> getAvailableSerialFormats();
    static std::string getSerializationInfo(const std::string& format);
    
    // Cross-language interoperability
    static bool registerForeignFunction(const std::string& name,
                                        const std::string& language,
                                        const std::string& signature,
                                        const std::function<py::object(const py::args&)>& func);
    static bool registerForeignFunction(const ForeignFunction& foreignFunc);
    static bool unregisterForeignFunction(const std::string& name);
    static std::vector<ForeignFunction> getRegisteredForeignFunctions();
    static std::vector<std::string> getRegisteredForeignLanguages();
    static py::object callForeignFunction(const std::string& name, const py::args& args);
    static bool enableLanguageInterface(const std::string& language, const LanguageInterface& interface);
    static bool disableLanguageInterface(const std::string& language);
    static bool isLanguageInterfaceEnabled(const std::string& language);
    static LanguageInterface getLanguageInterface(const std::string& language);
    static std::vector<LanguageInterface> getAllLanguageInterfaces();
    static py::dict getPythonInterface(std::shared_ptr<Brain> brain);
    static std::shared_ptr<Brain> createBrainFromPythonInterface(const py::dict& interface);
    static bool exportPythonInterface(const std::string& filepath, const py::dict& interface);
    static py::dict importPythonInterface(const std::string& filepath);
    
    // Advanced data processing
    static py::array_t<float> computeConvolution(const py::array_t<float>& input,
                                                 const py::array_t<float>& kernel,
                                                 const std::string& mode = "valid");
    static py::array_t<float> computeCorrelation(const py::array_t<float>& data1,
                                                 const py::array_t<float>& data2);
    static py::array_t<float> computeFFT(const py::array_t<float>& data);
    static py::array_t<float> computeInverseFFT(const py::array_t<float>& data);
    static std::vector<std::pair<size_t, size_t>> findPeaks(const py::array_t<float>& data,
                                                         float threshold = 0.5,
                                                         size_t minDistance = 1);
    static std::vector<std::pair<size_t, size_t>> findValleys(const py::array_t<float>& data,
                                                            float threshold = 0.5,
                                                            size_t minDistance = 1);
    static std::vector<size_t> smoothData(const py::array_t<float>& data,
                                         size_t windowSize = 5,
                                         const std::string& method = "moving_average");
    static py::array_t<float> applyFilter(const py::array_t<float>& data,
                                         const std::vector<float>& filterCoefficients);
    static std::vector<std::vector<float>> computeGradients(const py::array_t<float>& data2D);
    static std::vector<std::vector<float>> computeLaplacian(const py::array_t<float>& data2D);
    static std::vector<std::vector<float>> computeSobelEdgeDetection(const py::array_t<float>& data2D);
    
    // Data validation and quality checks
    static bool validateDataShape(const py::array_t<float>& data, const std::vector<size_t>& expectedShape);
    static bool validateDataRange(const py::array_t<float>& data, float minValue, float maxValue);
    static bool validateDataType(const py::array_t<float>& data, const std::string& expectedDtype);
    static bool checkForNaNValues(const py::array_t<float>& data);
    static bool checkForInfValues(const py::array_t<float>& data);
    static size_t countMissingValues(const py::array_t<float>& data);
    static py::array_t<float> cleanData(const py::array_t<float>& data,
                                       const std::string& method = "interpolate");
    static double calculateDataQualityScore(const py::array_t<float>& data);
    
    // Integration utilities
    static std::string getPythonVersion();
    static std::string getNumPyVersion();
    static std::string getSciPyVersion();
    static std::string getPackageVersion(const std::string& packageName);
    static bool checkPackageAvailability(const std::string& packageName);
    static std::vector<std::string> getAllAvailablePackages();
    static std::map<std::string, std::string> getEnvironmentInfo();
    static std::string generateEnvironmentReport();
    static bool createVirtualEnvironment(const std::string& envName, const std::string& pythonVersion = "default");
    static bool activateVirtualEnvironment(const std::string& envName);
    static bool installPythonPackage(const std::string& packageName, const std::string& version = "latest");
    static bool uninstallPythonPackage(const std::string& packageName);
    static bool runPythonScript(const std::string& script, const std::vector<std::string>& args = {});
    static py::object executePythonCode(const std::string& code, const py::dict& globals = py::dict());
    
    // Singleton instance management
    static IntegrationSystem& getInstance();
    
private:
    IntegrationSystem() = default;
    ~IntegrationSystem() = default;
    IntegrationSystem(const IntegrationSystem&) = delete;
    IntegrationSystem& operator=(const IntegrationSystem&) = delete;
    
    static std::map<std::string, ForeignFunction> registeredForeignFunctions_;
    static std::map<std::string, LanguageInterface> registeredLanguageInterfaces_;
    static std::map<std::string, std::function<std::string(std::shared_ptr<Brain>)>> customSerializers_;
    static std::map<std::string, std::function<std::shared_ptr<Brain>(const std::string&)>> customDeserializers_;
    static std::mutex mutex_;
};