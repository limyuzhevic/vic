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
#include <unordered_map>
#include <unordered_set>
#include "ExperimentProtocol.hpp"
#include "ExperimentTemplates.hpp"
#include "ExperimentPresets.hpp"
#include "ExperimentLibrary.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Marketplace entry types
enum class MarketplaceEntryType {
    Template,
    Preset,
    Plugin,
    Optimizer,
    Visualizer,
    Analyzer,
    Exporter,
    Experiment,
    Documentation,
    Tutorial,
    Example,
    Custom
};

// Marketplace entry metadata
struct MarketplaceEntryMetadata {
    std::string id;
    std::string name;
    MarketplaceEntryType type;
    std::string description;
    std::string author;
    std::string version;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    double rating;
    size_t downloadCount;
    size_t purchaseCount;
    double price;
    std::string currency;
    bool isFree;
    bool isFeatured;
    bool isApproved;
    
    MarketplaceEntryMetadata() : created(std::chrono::system_clock::now()), 
                                lastModified(std::chrono::system_clock::now()),
                                rating(0.0), downloadCount(0), purchaseCount(0), 
                                price(0.0), isFree(true), isFeatured(false), isApproved(false) {}
    MarketplaceEntryMetadata(const std::string& id, const std::string& name, MarketplaceEntryType type, 
                           const std::string& desc) : id(id), name(name), type(type), description(desc),
                                                      created(std::chrono::system_clock::now()), 
                                                      lastModified(std::chrono::system_clock::now()),
                                                      rating(0.0), downloadCount(0), purchaseCount(0), 
                                                      price(0.0), isFree(true), isFeatured(false), isApproved(false) {}
};

// Marketplace transaction
struct MarketplaceTransaction {
    std::string transactionId;
    std::string buyerId;
    std::string sellerId;
    std::string entryId;
    std::string entryName;
    MarketplaceEntryType entryType;
    double price;
    std::string currency;
    std::chrono::system_clock::time_point transactionTime;
    std::string paymentStatus;
    std::string downloadStatus;
    std::map<std::string, std::string> transactionData;
    
    MarketplaceTransaction() : transactionTime(std::chrono::system_clock::now()), 
                              price(0.0), isFree(true), currency("USD") {}
    MarketplaceTransaction(const std::string& id, const std::string& buyer, const std::string& seller, 
                           const std::string& entryId, MarketplaceEntryType type, double price)
        : transactionId(id), buyerId(buyer), sellerId(seller), entryId(entryId), entryType(type),
          price(price), transactionTime(std::chrono::system_clock::now()), currency("USD"), isFree(price == 0.0) {}
};

// Marketplace category
struct MarketplaceCategory {
    std::string categoryId;
    std::string name;
    std::string description;
    std::vector<std::string> parentCategories;
    std::vector<MarketplaceCategory> subCategories;
    std::vector<std::string> entryIds;
    bool isFeatured;
    int displayOrder;
    
    MarketplaceCategory() : isFeatured(false), displayOrder(0) {}
    MarketplaceCategory(const std::string& id, const std::string& name, const std::string& desc)
        : categoryId(id), name(name), description(desc), isFeatured(false), displayOrder(0) {}
};

// User review
struct UserReview {
    std::string reviewId;
    std::string userId;
    std::string entryId;
    std::string entryName;
    MarketplaceEntryType entryType;
    int rating;
    std::string comment;
    std::chrono::system_clock::time_point reviewTime;
    std::map<std::string, std::string> reviewData;
    
    UserReview() : rating(0), reviewTime(std::chrono::system_clock::now()) {}
    UserReview(const std::string& id, const std::string& userId, const std::string& entryId, 
              MarketplaceEntryType type, int rating, const std::string& comment)
        : reviewId(id), userId(userId), entryId(entryId), entryType(type), 
          rating(rating), comment(comment), reviewTime(std::chrono::system_clock::now()) {}
};

// Marketplace statistics
struct MarketplaceStatistics {
    size_t totalEntries;
    size_t totalDownloads;
    size_t totalPurchases;
    size_t totalUsers;
    double totalRevenue;
    std::chrono::system_clock::time_point lastUpdate;
    std::map<std::string, double> categoryStats;
    std::map<std::string, size_t> popularEntries;
    std::vector<std::string> featuredEntries;
    
    MarketplaceStatistics() : totalEntries(0), totalDownloads(0), totalPurchases(0), totalUsers(0),
                            totalRevenue(0.0), lastUpdate(std::chrono::system_clock::now()) {}
};

class ExperimentMarketplace {
public:
    ExperimentMarketplace();
    ~ExperimentMarketplace();
    
    // Disable copying, enable moving
    ExperimentMarketplace(const ExperimentMarketplace&) = delete;
    ExperimentMarketplace& operator=(const ExperimentMarketplace&) = delete;
    ExperimentMarketplace(ExperimentMarketplace&&) noexcept;
    ExperimentMarketplace& operator=(ExperimentMarketplace&&) noexcept;
    
    // Marketplace management
    bool initialize(const std::string& marketplacePath = "");
    bool shutdown();
    bool isInitialized() const;
    
    // Entry management
    bool addEntry(const MarketplaceEntryMetadata& entry);
    bool removeEntry(const std::string& entryId);
    bool updateEntry(const std::string& entryId, const MarketplaceEntryMetadata& entry);
    MarketplaceEntryMetadata getEntry(const std::string& entryId) const;
    std::vector<MarketplaceEntryMetadata> getAllEntries() const;
    std::vector<MarketplaceEntryMetadata> getEntriesByType(MarketplaceEntryType type) const;
    std::vector<MarketplaceEntryMetadata> getEntriesByTag(const std::string& tag) const;
    std::vector<MarketplaceEntryMetadata> searchEntries(const std::string& query) const;
    
    // Category management
    bool addCategory(const MarketplaceCategory& category);
    bool removeCategory(const std::string& categoryId);
    bool updateCategory(const std::string& categoryId, const MarketplaceCategory& category);
    MarketplaceCategory getCategory(const std::string& categoryId) const;
    std::vector<MarketplaceCategory> getAllCategories() const;
    std::vector<MarketplaceCategory> getFeaturedCategories() const;
    std::vector<MarketplaceCategory> getCategoryTree() const;
    
    // Download and installation
    bool downloadEntry(const std::string& entryId);
    bool installEntry(const std::string& entryId);
    bool uninstallEntry(const std::string& entryId);
    bool updateEntry(const std::string& entryId);
    
    // Purchase and transaction management
    bool purchaseEntry(const std::string& entryId, const std::string& buyerId, double price = 0.0);
    bool completePurchase(const std::string& transactionId);
    bool refundPurchase(const std::string& transactionId);
    MarketplaceTransaction getTransaction(const std::string& transactionId) const;
    std::vector<MarketplaceTransaction> getUserTransactions(const std::string& userId) const;
    std::vector<MarketplaceTransaction> getSellerTransactions(const std::string& sellerId) const;
    
    // Review management
    bool addReview(const UserReview& review);
    bool removeReview(const std::string& reviewId);
    bool updateReview(const std::string& reviewId, const UserReview& review);
    UserReview getReview(const std::string& reviewId) const;
    std::vector<UserReview> getEntryReviews(const std::string& entryId) const;
    std::vector<UserReview> getUserReviews(const std::string& userId) const;
    
    // Featured and trending
    bool featureEntry(const std::string& entryId, bool featured = true);
    bool promoteEntry(const std::string& entryId);
    std::vector<MarketplaceEntryMetadata> getFeaturedEntries() const;
    std::vector<MarketplaceEntryMetadata> getTrendingEntries() const;
    std::vector<MarketplaceEntryMetadata> getPopularEntries() const;
    
    // Rating and feedback
    bool rateEntry(const std::string& entryId, int rating);
    double getEntryAverageRating(const std::string& entryId) const;
    int getEntryRatingCount(const std::string& entryId) const;
    std::vector<MarketplaceEntryMetadata> getTopRatedEntries() const;
    std::vector<MarketplaceEntryMetadata> getRecentlyReviewedEntries() const;
    
    // Export and import
    bool exportEntry(const std::string& entryId, const std::string& filepath) const;
    bool exportAllEntries(const std::string& directoryPath) const;
    bool importEntry(const std::string& filepath);
    bool importEntriesFromDirectory(const std::string& directoryPath);
    
    // Statistics and analytics
    MarketplaceStatistics getMarketplaceStatistics() const;
    std::map<std::string, double> getCategoryStats(const std::string& categoryId) const;
    std::vector<MarketplaceEntryMetadata> getEntriesByPriceRange(double minPrice, double maxPrice) const;
    std::vector<MarketplaceEntryMetadata> getFreeEntries() const;
    
    // Search and discovery
    std::vector<MarketplaceEntryMetadata> searchByName(const std::string& name) const;
    std::vector<MarketplaceEntryMetadata> searchByDescription(const std::string& description) const;
    std::vector<MarketplaceEntryMetadata> searchByTag(const std::string& tag) const;
    std::vector<MarketplaceEntryMetadata> searchByAuthor(const std::string& author) const;
    std::vector<MarketplaceEntryMetadata> getEntriesByCategory(const std::string& categoryId) const;
    
    // Collaboration and community
    bool shareEntry(const std::string& entryId, const std::string& username);
    bool reportAbuse(const std::string& entryId, const std::string& reason);
    bool flagContent(const std::string& entryId, const std::string& userId, const std::string& reason);
    std::vector<std::string> getReportedEntries() const;
    
    // Moderation
    bool approveEntry(const std::string& entryId);
    bool rejectEntry(const std::string& entryId, const std::string& reason);
    bool suspendEntry(const std::string& entryId, const std::string& reason);
    bool restoreEntry(const std::string& entryId);
    
    // Backup and recovery
    bool backupMarketplace(const std::string& backupPath) const;
    bool restoreMarketplace(const std::string& backupPath);
    bool exportMarketplaceToJSON(const std::string& filepath) const;
    bool importMarketplaceFromJSON(const std::string& filepath);
    
    // Cleanup and maintenance
    bool cleanupUnusedEntries();
    bool cleanupExpiredEntries(const std::chrono::system_clock::time_point& cutoffTime);
    bool validateAllEntries();
    bool fixCorruptedEntries();
    
    // System management
    bool clearAllEntries();
    bool resetMarketplace();
    bool refreshMarketplace();
    std::vector<std::string> getAllEntryIds() const;
    size_t getEntryCount() const;
    
    // Utility functions
    static std::string generateEntryId() const;
    static bool isValidEntryId(const std::string& entryId) const;
    static bool isValidEntryPath(const std::string& path) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm