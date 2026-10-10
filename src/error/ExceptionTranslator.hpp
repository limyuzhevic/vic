// Exception Translator Header
#pragma once

#include <string>
#include <map>
#include <functional>
#include "NLMErrors.hpp"

namespace nlm {

/**
 * Exception translator
 */
class ExceptionTranslator {
public:
    ExceptionTranslator();
    ~ExceptionTranslator();
    
    /**
     * Translate C++ exception to NLM error
     */
    NLMError translate(const std::exception& ex);
    
    /**
     * Register exception translation
     */
    template<typename TException>
    void registerTranslation(ErrorCode code) {
        _translationMap[&typeid(TException)] = code;
    }
    
    /**
     * Register custom exception translation
     */
    template<typename TException>
    void registerTranslation(ErrorCode code, std::function<NLMError(const std::exception&)> translator) {
        _translationMap[&typeid(TException)] = code;
        _customTranslators[&typeid(TException)] = translator;
    }
    
    /**
     * Translate exception with specific type
     */
    template<typename TException>
    NLMError translate(const std::exception& ex) {
        auto it = _customTranslators.find(&typeid(TException));
        if (it != _customTranslators.end()) {
            return it->second(ex);
        }
        
        return translate(ex);
    }
    
private:
    std::map<const std::type_info*, ErrorCode> _translationMap;
    std::map<const std::type_info*, std::function<NLMError(const std::exception&)>> _customTranslators;
};

} // namespace nlm