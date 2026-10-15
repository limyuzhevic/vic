#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentTemplates.hpp"
#include "ExperimentPresets.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Library entry type
enum class LibraryEntryType {
    Template,
    Preset,
    Protocol,
    Optimizer,
    Visualizer,
    Analyzer,
    Exporter,
    Custom
};

// Library entry metadata
struct LibraryEntryMetadata {
    std::string name;
    LibraryEntryType type;
    std::string description;
    std::string author;
    std::string version;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    double rating;
    size_t downloadCount;
    
    LibraryEntryMetadata() : created(std::chrono::system_clock::now()), 
                           lastModified(std::chrono::system_clock::now()),
                           rating(0.0), downloadCount(0) {}
    LibraryEntryMetadata(const std::string& name, LibraryEntryType type, const std::string& desc)
        : name(name), type(type), description(desc),
          created(std::chrono::system_clock::now()), lastModified(std::chrono::system_clock::now()),
          rating(0.0), downloadCount(0) {}
};

// Library version
struct LibraryVersion {
    std::string version;
    std::chrono::system_clock::time_point releaseDate;
    std::string changelog;
    std::map<std::string, std::string> versionMetadata;
    
    LibraryVersion() : releaseDate(std::chrono::system_clock::now()) {}
    LibraryVersion(const std::string& ver, const std::string& changelog)
        : version(ver), changelog(changelog), releaseDate(std::chrono::system_clock::now()) {}
};

// Library statistics
struct LibraryStatistics {
    size_t totalEntries;
    size_t totalTemplates;
    size_t totalPresets;
    size_t totalProtocols;
    size_t totalOptimizers;
    size_t totalVisualizers;
    size_t totalAnalyzers;
    size_t totalExporters;
    size_t totalCustomEntries;
    size_t totalDownloads;
    std::chrono::system_clock::time_point lastUpdate;
    std::map<std::string, double> categoryStats;
    
    LibraryStatistics() : totalEntries(0), totalTemplates(0), totalPresets(0), totalProtocols(0),
                         totalOptimizers(0), totalVisualizers(0), totalAnalyzers(0), totalExporters(0),
                         totalCustomEntries(0), totalDownloads(0), lastUpdate(std::chrono::system_clock::now()) {}
};

// Library compatibility
struct LibraryCompatibility {
    std::string platform;
    std::string os;
    std::string architecture;
    std::string compiler;
    std::string languageVersion;
    std::vector<std::string> requiredLibraries;
    std::map<std::string, std::string> dependencyVersions;
    
    LibraryCompatibility() {}
    LibraryCompatibility(const std::string& platform, const std::string& os, const std::string& arch)
        : platform(platform), os(os), architecture(arch) {}
};

class ExperimentLibrary {
public:
    ExperimentLibrary();
    ~ExperimentLibrary();
    
    // Disable copying, enable moving
    ExperimentLibrary(const ExperimentLibrary&) = delete;
    ExperimentLibrary& operator=(const ExperimentLibrary&) = delete;
    ExperimentLibrary(ExperimentLibrary&&) noexcept;
    ExperimentLibrary& operator=(ExperimentLibrary&&) noexcept;
    
    // Library management
    bool initialize(const std::string& libraryPath = "");
    bool shutdown();
    bool isInitialized() const;
    
    // Entry management
    bool addEntry(const LibraryEntryMetadata& metadata, const std::string& filepath = "");
    bool removeEntry(const std::string& name);
    bool updateEntry(const std::string& name, const LibraryEntryMetadata& metadata);
    bool updateEntry(const std::string& name, const std::string& filepath);
    
    // Entry retrieval
    LibraryEntryMetadata getEntry(const std::string& name) const;
    std::vector<LibraryEntryMetadata> getAllEntries() const;
    std::vector<LibraryEntryMetadata> getEntriesByType(LibraryEntryType type) const;
    std::vector<LibraryEntryMetadata> getEntriesByTag(const std::string& tag) const;
    std::vector<LibraryEntryMetadata> searchEntries(const std::string& query) const;
    
    // Template management
    bool addTemplate(const ExperimentTemplate& templateObj);
    bool removeTemplate(const std::string& name);
    bool updateTemplate(const std::string& name, const ExperimentTemplate& templateObj);
    ExperimentTemplate getTemplate(const std::string& name) const;
    std::vector<ExperimentTemplate> getAllTemplates() const;
    
    // Preset management
    bool addPreset(const ExperimentPreset& preset);
    bool removePreset(const std::string& name);
    bool updatePreset(const std::string& name, const ExperimentPreset& preset);
    ExperimentPreset getPreset(const std::string& name) const;
    std::vector<ExperimentPreset> getAllPresets() const;
    
    // Protocol management
    bool addProtocol(const std::string& name, std::shared_ptr<ExperimentProtocol> protocol);
    bool removeProtocol(const std::string& name);
    std::shared_ptr<ExperimentProtocol> getProtocol(const std::string& name) const;
    std::vector<std::shared_ptr<ExperimentProtocol>> getAllProtocols() const;
    
    // Version management
    bool addVersion(const std::string& entryName, const LibraryVersion& version);
    bool removeVersion(const std::string& entryName, const std::string& version);
    std::vector<LibraryVersion> getVersions(const std::string& entryName) const;
    LibraryVersion getLatestVersion(const std::string& entryName) const;
    
    // Compatibility management
    bool setCompatibility(const std::string& entryName, const LibraryCompatibility& compatibility);
    LibraryCompatibility getCompatibility(const std::string& entryName) const;
    
    // Statistics and analytics
    LibraryStatistics getLibraryStatistics() const;
    std::map<std::string, double> getUsageStatistics(const std::string& entryName) const;
    std::vector<std::string> getPopularEntries() const;
    std::vector<std::string> getRecentlyAddedEntries() const;
    
    // Download and installation
    bool downloadEntry(const std::string& entryName, const std::string& version = "");
    bool installEntry(const std::string& entryName, const std::string& version = "");
    bool uninstallEntry(const std::string& entryName);
    bool updateEntry(const std::string& entryName);
    
    // Export and import
    bool exportEntry(const std::string& entryName, const std::string& filepath) const;
    bool exportAllEntries(const std::string& directoryPath) const;
    bool importEntry(const std::string& filepath);
    bool importEntriesFromDirectory(const std::string& directoryPath);
    
    // Search and discovery
    std::vector<LibraryEntryMetadata> searchByName(const std::string& name) const;
    std::vector<LibraryEntryMetadata> searchByDescription(const std::string& description) const;
    std::vector<LibraryEntryMetadata> searchByTag(const std::string& tag) const;
    std::vector<LibraryEntryMetadata> searchByAuthor(const std::string& author) const;
    std::vector<LibraryEntryMetadata> searchByCategory(LibraryEntryType type, const std::string& category) const;
    
    // Recommendation system
    std::vector<std::string> recommendEntries(const std::string& useCase) const;
    std::vector<LibraryEntryMetadata> recommendSimilarEntries(const std::string& entryName) const;
    
    // Rating and feedback
    bool rateEntry(const std::string& entryName, double rating);
    std::vector<LibraryEntryMetadata> getTopRatedEntries() const;
    std::vector<LibraryEntryMetadata> getRecentReviews() const;
    
    // Collaboration
    bool shareEntry(const std::string& entryName, const std::string& username);
    bool reviewEntry(const std::string& entryName, const std::string& review, int rating);
    std::vector<std::string> getEntryReviews(const std::string& entryName) const;
    
    // Backup and recovery
    bool backupLibrary(const std::string& backupPath) const;
    bool restoreLibrary(const std::string& backupPath);
    bool exportLibraryToJSON(const std::string& filepath) const;
    bool importLibraryFromJSON(const std::string& filepath);
    
    // Cleanup and maintenance
    bool cleanupUnusedEntries();
    bool cleanupExpiredEntries(const std::chrono::system_clock::time_point& cutoffTime);
    bool validateAllEntries();
    bool fixCorruptedEntries();
    
    // System management
    bool clearAllEntries();
    bool resetLibrary();
    bool refreshLibrary();
    std::vector<std::string> getAllEntryNames() const;
    size_t getEntryCount() const;
    
    // Utility functions
    static std::string generateEntryName(const std::string& prefix = "entry") const;
    static bool isValidEntryName(const std::string& name) const;
    static bool isValidEntryPath(const std::string& path) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm