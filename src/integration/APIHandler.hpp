#ifndef API_HANDLER_HPP
#define API_HANDLER_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <functional>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace nlm {
namespace integration {

// HTTP request and response structures
struct HttpRequest {
    std::string method;
    std::string path;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> query_params;
};

struct HttpResponse {
    int status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> additional_data;
};

// API endpoint definition
struct ApiEndpoint {
    std::string path;                                    // Endpoint path (e.g., "/api/models")
    std::string method;                                  // HTTP method (GET, POST, PUT, DELETE)
    std::string handler_name;                            // Name of handler function
    std::vector<std::string> required_auth;              // Required authentication scopes
    bool requires_authentication;                        // Whether endpoint requires auth
    std::string description;                             // Endpoint description
};

// API handler interface
class ApiHandler {
public:
    virtual ~ApiHandler() = default;
    
    // Handler registration and management
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::vector<std::string> getRoutes() const = 0;
    
    // Request handling
    virtual HttpResponse handleRequest(const HttpRequest& request) = 0;
    virtual bool supportsMethod(const std::string& method) const = 0;
    
    // Configuration and setup
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual bool shutdown() = 0;
    
    // State management
    virtual bool isInitialized() const = 0;
    virtual std::map<std::string, std::string> getConfiguration() const = 0;
    
    // Resource management
    virtual bool allocateResources(size_t resources) = 0;
    virtual bool releaseResources(size_t resources) = 0;
    virtual size_t getAvailableResources() const = 0;
};

// REST API handler
class RestApiHandler : public ApiHandler {
private:
    std::map<std::string, ApiEndpoint> routes_;
    std::mutex mutex_;
    bool initialized_;
    std::string base_path_;
    std::string host_;
    int port_;
    bool use_ssl_;

public:
    RestApiHandler() : initialized_(false), host_("0.0.0.0"), port_(8080), use_ssl_(false) {}

    // Route management
    bool addRoute(const ApiEndpoint& endpoint);
    bool removeRoute(const std::string& path, const std::string& method);
    ApiEndpoint getEndpoint(const std::string& path, const std::string& method) const;
    
    // Request handling
    HttpResponse handleRequest(const HttpRequest& request) override;
    bool supportsMethod(const std::string& method) const override;
    
    // Configuration
    bool initialize(const std::map<std::string, std::string>& config) override;
    bool shutdown() override;
    
    // State management
    bool isInitialized() const override { return initialized_; }
    std::map<std::string, std::string> getConfiguration() const override;
    
    // Resource management
    bool allocateResources(size_t resources) override;
    bool releaseResources(size_t resources) override;
    size_t getAvailableResources() const override;
    
    // API info
    std::string getName() const override { return "REST_API_Handler"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::vector<std::string> getRoutes() const override;

protected:
    // Route handlers (to be implemented by users)
    virtual HttpResponse handleGet(const HttpRequest& request);
    virtual HttpResponse handlePost(const HttpRequest& request);
    virtual HttpResponse handlePut(const HttpRequest& request);
    virtual HttpResponse handleDelete(const HttpRequest& request);
    virtual HttpResponse handlePatch(const HttpRequest& request);
    
    // Authentication and authorization
    virtual bool authenticateRequest(const HttpRequest& request) const;
    virtual bool authorizeRequest(const HttpRequest& request, const std::string& required_scope) const;
    
    // Request/response utilities
    HttpRequest parseHttpRequest(const std::string& raw_request) const;
    std::string serializeHttpResponse(const HttpResponse& response) const;
    std::map<std::string, std::string> parseQueryParams(const std::string& query_string) const;
    std::map<std::string, std::string> parseHeaders(const std::string& headers_string) const;
};

// GraphQL API handler
class GraphQLApiHandler : public ApiHandler {
private:
    std::string schema_;                                  // GraphQL schema
    std::mutex mutex_;
    bool initialized_;
    bool enable_introspection_;
    std::map<std::string, std::function<std::string(const std::map<std::string, std::string>&)>> resolvers_;

public:
    GraphQLApiHandler() : initialized_(false), enable_introspection_(true) {}

    // Schema management
    bool setSchema(const std::string& schema);
    std::string getSchema() const { return schema_; }
    
    // Resolver management
    bool addResolver(const std::string& type, const std::string& field,
                    std::function<std::string(const std::map<std::string, std::string>&)> resolver);
    bool removeResolver(const std::string& type, const std::string& field);
    
    // Request handling
    HttpResponse handleRequest(const HttpRequest& request) override;
    bool supportsMethod(const std::string& method) const override { return method == "POST"; }
    
    // Configuration
    bool initialize(const std::map<std::string, std::string>& config) override;
    bool shutdown() override;
    
    // State management
    bool isInitialized() const override { return initialized_; }
    std::map<std::string, std::string> getConfiguration() const override;
    
    // Resource management
    bool allocateResources(size_t resources) override;
    bool releaseResources(size_t resources) override;
    size_t getAvailableResources() const override;
    
    // API info
    std::string getName() const override { return "GraphQL_API_Handler"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::vector<std::string> getRoutes() const override { return {"graphql"}; }

private:
    // GraphQL query processing
    std::string processGraphQLQuery(const std::string& query, const std::map<std::string, std::string>& variables) const;
    std::string validateGraphQLQuery(const std::string& query) const;
    std::map<std::string, std::string> parseVariables(const std::string& variables_json) const;
};

// API gateway
class ApiGateway {
private:
    std::map<std::string, std::unique_ptr<ApiHandler>> handlers_;
    std::mutex mutex_;
    bool enabled_;
    int http_port_;
    int https_port_;
    std::string host_;
    std::string ssl_cert_path_;
    std::string ssl_key_path_;

public:
    ApiGateway() : enabled_(false), http_port_(8080), https_port_(8443), host_("0.0.0.0") {}

    // Handler management
    bool registerHandler(const std::string& name, std::unique_ptr<ApiHandler> handler);
    bool unregisterHandler(const std::string& name);
    ApiHandler* getHandler(const std::string& name) const;
    
    // Gateway configuration
    bool enable() { enabled_ = true; return true; }
    bool disable() { enabled_ = false; return true; }
    bool isEnabled() const { return enabled_; }
    
    bool configureHttp(int port, const std::string& host = "0.0.0.0");
    bool configureHttps(int port, const std::string& cert_path, const std::string& key_path);
    
    // Request routing
    HttpResponse routeRequest(const HttpRequest& request) const;
    bool supportsRoute(const HttpRequest& request) const;
    
    // API management
    std::vector<std::string> getRegisteredHandlers() const;
    std::map<std::string, std::string> getHandlerInfo(const std::string& name) const;
    
    // Statistics and monitoring
    std::map<std::string, size_t> getRequestStatistics() const;
    double getSystemHealth() const;
    std::string getSystemStatus() const;
    
    // Configuration
    std::map<std::string, std::string> getConfiguration() const;
    bool configure(const std::map<std::string, std::string>& config);
    
    // Persistence and backup
    bool saveConfiguration(const std::string& filename) const;
    bool loadConfiguration(const std::string& filename);
    
    // Maintenance
    bool start() const;
    bool stop() const;
    bool restart() const;
};

} // namespace integration
} // namespace nlm

#endif // API_HANDLER_HPP