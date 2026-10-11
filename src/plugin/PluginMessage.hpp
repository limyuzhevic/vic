#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>

namespace nlm {

class PluginMessage {
public:
    PluginMessage();
    PluginMessage(const std::string& sender, const std::string& receiver,
                  const std::string& type, const std::string& category);
    
    // Message content
    std::string sender;
    std::string receiver;
    std::string type;
    std::string category;
    double timestamp;
    
    // Data storage
    std::map<std::string, std::string> data;
    std::map<std::string, double> numericData;
    std::map<std::string, std::vector<std::string>> stringListData;
    std::string payload;  // For binary data
    
    // Helper methods
    void setTimestamp();
    double getTimestamp() const;
    
    // Data access methods
    template<typename T>
    T getData(const std::string& key) const;
    
    template<typename T>
    void setData(const std::string& key, const T& value);
    
    // Numeric data access
    double getNumericData(const std::string& key) const;
    void setNumericData(const std::string& key, double value);
    
    // String list data access
    std::vector<std::string> getStringListData(const std::string& key) const;
    void setStringListData(const std::string& key, const std::vector<std::string>& value);
    
    // Payload access
    const std::string& getPayload() const;
    void setPayload(const std::string& payload);
};

} // namespace nlm
