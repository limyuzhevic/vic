#include "APIHandler.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <unordered_map>

namespace nlm {
namespace integration {

// RestApiHandler implementation
bool RestApiHandler::addRoute(const ApiEndpoint& endpoint) {
    std::lock_guard<std::mutex> lock(mutex_);
    routes_[endpoint.path + "_" + endpoint.method] = endpoint;
    return true;
}

bool RestApiHandler::removeRoute(const std::string& path, const std::string& method) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key = path + "_" + method;
    return routes_.erase(key) > 0;
}

ApiEndpoint RestApiHandler::getEndpoint(const std::string& path, const std::string& method) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key = path + "_" + method;
    auto it = routes_.find(key);
    if (it != routes_.end()) {
        return it->second;
    }
    
    // Return empty endpoint
    ApiEndpoint empty_endpoint;
    empty_endpoint.path = path;
    empty_endpoint.method = method;
    return empty_endpoint;
}

HttpResponse RestApiHandler::handleRequest(const HttpRequest& request) {
    // Default response
    HttpResponse response;
    response.status_code = 404;
    response.status_message = "Not Found";
    response.body = "Endpoint not found";
    response.headers["Content-Type"] = "application/json";
    
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key = request.path + "_" + request.method;
    auto it = routes_.find(key);
    
    if (it != routes_.end()) {
        const ApiEndpoint& endpoint = it->second;
        
        // Authentication check
        if (endpoint.requires_authentication) {
            if (!authenticateRequest(request)) {
                response.status_code = 401;
                response.status_message = "Unauthorized";
                response.body = "Authentication required";
                return response;
            }
        }
        
        // Authorization check
        if (endpoint.required_auth.empty()) {
            response.status_code = 403;
            response.status_message = "Forbidden";
            response.body = "Insufficient permissions";
            return response;
        }
        
        // Route to appropriate handler based on method
        if (request.method == "GET") {
            return handleGet(request);
        } else if (request.method == "POST") {
            return handlePost(request);
        } else if (request.method == "PUT") {
            return handlePut(request);
        } else if (request.method == "DELETE") {
            return handleDelete(request);
        } else if (request.method == "PATCH") {
            return handlePatch(request);
        }
    }
    
    return response;
}

bool RestApiHandler::supportsMethod(const std::string& method) const {
    return method == "GET" || method == "POST" || method == "PUT" || 
           method == "DELETE" || method == "PATCH";
}

bool RestApiHandler::initialize(const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (config.count("base_path")) {
        base_path_ = config.at("base_path");
    }
    if (config.count("host")) {
        host_ = config.at("host");
    }
    if (config.count("port")) {
        port_ = std::stoi(config.at("port"));
    }
    if (config.count("ssl_enabled")) {
        use_ssl_ = config.at("ssl_enabled") == "true";
    }
    
    // Add default routes
    ApiEndpoint default_endpoint;
    default_endpoint.path = "/health";
    default_endpoint.method = "GET";
    default_endpoint.requires_authentication = false;
    default_endpoint.handler_name = "health_check";
    default_endpoint.description = "Health check endpoint";
    routes_[default_endpoint.path + "_" + default_endpoint.method] = default_endpoint;
    
    initialized_ = true;
    return true;
}

bool RestApiHandler::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    routes_.clear();
    initialized_ = false;
    return true;
}

std::map<std::string, std::string> RestApiHandler::getConfiguration() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, std::string> config;
    config["base_path"] = base_path_;
    config["host"] = host_;
    config["port"] = std::to_string(port_);
    config["ssl_enabled"] = use_ssl_ ? "true" : "false";
    config["initialized"] = initialized_ ? "true" : "false";
    
    return config;
}

bool RestApiHandler::allocateResources(size_t resources) {
    // In real implementation, would track allocated resources
    return true;
}

bool RestApiHandler::releaseResources(size_t resources) {
    // In real implementation, would release resources
    return true;
}

size_t RestApiHandler::getAvailableResources() const {
    return 1000; // In real implementation, would track available resources
}

std::vector<std::string> RestApiHandler::getRoutes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> route_strings;
    for (const auto& [key, endpoint] : routes_) {
        route_strings.push_back(endpoint.method + " " + endpoint.path);
    }
    
    return route_strings;
}

// GraphQLApiHandler implementation
bool GraphQLApiHandler::setSchema(const std::string& schema) {
    std::lock_guard<std::mutex> lock(mutex_);
    schema_ = schema;
    return true;
}

bool GraphQLApiHandler::addResolver(const std::string& type, const std::string& field,
                                    std::function<std::string(const std::map<std::string, std::string>&)> resolver) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key = type + "_" + field;
    resolvers_[key] = resolver;
    return true;
}

bool GraphQLApiHandler::removeResolver(const std::string& type, const std::string& field) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key = type + "_" + field;
    return resolvers_.erase(key) > 0;
}

HttpResponse GraphQLApiHandler::handleRequest(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "POST") {
        response.status_code = 405;
        response.status_message = "Method Not Allowed";
        response.body = "Only POST method is supported for GraphQL";
        return response;
    }
    
    // Parse GraphQL query from request body
    // In real implementation, would parse JSON and validate GraphQL query
    std::string query = request.body;
    
    // For now, return a simple response
    response.status_code = 200;
    response.status_message = "OK";
    response.body = "GraphQL endpoint - schema length: " + std::to_string(schema_.size());
    response.headers["Content-Type"] = "application/json";
    
    return response;
}

bool GraphQLApiHandler::initialize(const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (config.count("enable_introspection")) {
        enable_introspection_ = config.at("enable_introspection") == "true";
    }
    
    // Set default schema if not set
    if (schema_.empty()) {
        schema_ = "type Query { hello: String }";
    }
    
    initialized_ = true;
    return true;
}

bool GraphQLApiHandler::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    schema_.clear();
    resolvers_.clear();
    initialized_ = false;
    return true;
}

std::map<std::string, std::string> GraphQLApiHandler::getConfiguration() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, std::string> config;
    config["schema_length"] = std::to_string(schema_.size());
    config["enable_introspection"] = enable_introspection_ ? "true" : "false";
    config["initialized"] = initialized_ ? "true" : "false";
    config["resolver_count"] = std::to_string(resolvers_.size());
    
    return config;
}

bool GraphQLApiHandler::allocateResources(size_t resources) {
    return true;
}

bool GraphQLApiHandler::releaseResources(size_t resources) {
    return true;
}

size_t GraphQLApiHandler::getAvailableResources() const {
    return 1000;
}

// ApiGateway implementation
bool ApiGateway::registerHandler(const std::string& name, std::unique_ptr<ApiHandler> handler) {
    if (!handler) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_[name] = std::move(handler);
    return true;
}

bool ApiGateway::unregisterHandler(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    return handlers_.erase(name) > 0;
}

ApiHandler* ApiGateway::getHandler(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(name);
    if (it != handlers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

HttpResponse ApiGateway::routeRequest(const HttpRequest& request) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find handler that can handle this route
    for (const auto& [name, handler] : handlers_) {
        if (handler->supportsMethod(request.method) && !handler->getRoutes().empty()) {
            return handler->handleRequest(request);
        }
    }
    
    // No handler found
    HttpResponse response;
    response.status_code = 404;
    response.status_message = "Not Found";
    response.body = "No handler found for this request";
    response.headers["Content-Type"] = "application/json";
    
    return response;
}

bool ApiGateway::supportsRoute(const HttpRequest& request) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& [name, handler] : handlers_) {
        if (handler->supportsMethod(request.method)) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> ApiGateway::getRegisteredHandlers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& [name, handler] : handlers_) {
        names.push_back(name);
    }
    
    return names;
}

std::map<std::string, std::string> ApiGateway::getHandlerInfo(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, std::string> info;
    
    auto it = handlers_.find(name);
    if (it != handlers_.end()) {
        ApiHandler* handler = it->second.get();
        info["name"] = handler->getName();
        info["version"] = handler->getVersion();
        
        std::vector<std::string> routes = handler->getRoutes();
        info["routes"] = "";
        for (size_t i = 0; i < routes.size(); ++i) {
            if (i > 0) info["routes"] += ", ";
            info["routes"] += routes[i];
        }
    }
    
    return info;
}

std::map<std::string, size_t> ApiGateway::getRequestStatistics() const {
    std::map<std::string, size_t> stats;
    
    for (const auto& [name, handler] : handlers_) {
        // In real implementation, would track actual request statistics
        stats[name] = 0;
    }
    
    return stats;
}

double ApiGateway::getSystemHealth() const {
    return 1.0; // In real implementation, would calculate health based on handler status
}

std::string ApiGateway::getSystemStatus() const {
    std::stringstream status;
    status << "API Gateway Status:" << std::endl;
    status << "Enabled: " << (enabled_ ? "Yes" : "No") << std::endl;
    status << "HTTP Port: " << http_port_ << std::endl;
    status << "HTTPS Port: " << https_port_ << std::endl;
    status << "Host: " << host_ << std::endl;
    status << "Registered Handlers: " << handlers_.size() << std::endl;
    
    for (const auto& [name, handler] : handlers_) {
        status << "  - " << name << ": " << handler->getName() << " v" << handler->getVersion() << std::endl;
    }
    
    return status.str();
}

std::map<std::string, std::string> ApiGateway::getConfiguration() const {
    std::map<std::string, std::string> config;
    config["enabled"] = enabled_ ? "true" : "false";
    config["http_port"] = std::to_string(http_port_);
    config["https_port"] = std::to_string(https_port_);
    config["host"] = host_;
    config["ssl_cert_path"] = ssl_cert_path_;
    config["ssl_key_path"] = ssl_key_path_;
    config["handler_count"] = std::to_string(handlers_.size());
    
    return config;
}

bool ApiGateway::configure(const std::map<std::string, std::string>& config) {
    enabled_ = config.count("enabled") && config.at("enabled") == "true";
    
    if (config.count("http_port")) {
        http_port_ = std::stoi(config.at("http_port"));
    }
    
    if (config.count("https_port")) {
        https_port_ = std::stoi(config.at("https_port"));
    }
    
    if (config.count("host")) {
        host_ = config.at("host");
    }
    
    if (config.count("ssl_cert_path")) {
        ssl_cert_path_ = config.at("ssl_cert_path");
    }
    
    if (config.count("ssl_key_path")) {
        ssl_key_path_ = config.at("ssl_key_path");
    }
    
    return true;
}

bool ApiGateway::saveConfiguration(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        return false;
    }

    try {
        file << "enabled=" << (enabled_ ? "true" : "false") << std::endl;
        file << "http_port=" << http_port_ << std::endl;
        file << "https_port=" << https_port_ << std::endl;
        file << "host=" << host_ << std::endl;
        file << "ssl_cert_path=" << ssl_cert_path_ << std::endl;
        file << "ssl_key_path=" << ssl_key_path_ << std::endl;
        file << "handler_count=" << handlers_.size() << std::endl;
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving API gateway configuration: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool ApiGateway::loadConfiguration(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t equals_pos = line.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = line.substr(0, equals_pos);
            std::string value = line.substr(equals_pos + 1);
            
            if (key == "enabled") {
                enabled_ = (value == "true");
            } else if (key == "http_port") {
                http_port_ = std::stoi(value);
            } else if (key == "https_port") {
                https_port_ = std::stoi(value);
            } else if (key == "host") {
                host_ = value;
            } else if (key == "ssl_cert_path") {
                ssl_cert_path_ = value;
            } else if (key == "ssl_key_path") {
                ssl_key_path_ = value;
            }
        }
    }
    
    file.close();
    return true;
}

bool ApiGateway::start() const {
    // In real implementation, would start HTTP/HTTPS servers
    return true;
}

bool ApiGateway::stop() const {
    // In real implementation, would stop HTTP/HTTPS servers
    return true;
}

bool ApiGateway::restart() const {
    // In real implementation, would restart HTTP/HTTPS servers
    return stop() && start();
}

} // namespace integration
} // namespace nlm