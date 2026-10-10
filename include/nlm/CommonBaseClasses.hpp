#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <functional>
#include <vector>
#include <unordered_map>

namespace nlm {

/**
 * Template base class for all NLM objects
 * Provides common interface for all neural system components
 */
class NLMObject {
public:
    virtual ~NLMObject() = default;
    
    // Get the unique ID of this object
    virtual uint64_t getId() const = 0;
    
    // Get the type name of this object
    virtual const std::string& getTypeName() const = 0;
    
    // Check if this object is valid
    virtual bool isValid() const { return true; }
};

/**
 * Template base class for configurable objects
 * Provides interface for configuration management
 */
class Configurable : public virtual NLMObject {
public:
    virtual ~Configurable() = default;
    
    // Load configuration from string
    virtual void loadConfig(const std::string& config) = 0;
    
    // Export configuration to string
    virtual std::string exportConfig() const = 0;
    
    // Validate current configuration
    virtual bool validateConfig() const = 0;
};

/**
 * Base class for observable objects
 * Implements the Observer pattern
 */
class Observable {
protected:
    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void update(const std::string& event, const std::string& data) = 0;
    };
    
    using ObserverPtr = std::shared_ptr<Observer>;
    std::vector<ObserverPtr> observers;
    
public:
    virtual ~Observable() = default;
    
    // Add an observer
    void addObserver(ObserverPtr observer);
    
    // Remove an observer
    void removeObserver(ObserverPtr observer);
    
    // Notify all observers
    void notifyObservers(const std::string& event, const std::string& data = "");
};

/**
 * Command interface for the Command pattern
 * Represents an action that can be executed, undone, or redone
 */
class Command {
public:
    virtual ~Command() = default;
    
    // Execute the command
    virtual void execute() = 0;
    
    // Undo the command
    virtual void undo() = 0;
    
    // Redo the command
    virtual void redo() = 0;
    
    // Check if the command can be undone
    virtual bool canUndo() const = 0;
    
    // Check if the command can be redone
    virtual bool canRedo() const = 0;
};
}