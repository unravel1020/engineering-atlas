#pragma once

#include "IBackend.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace oml {
namespace backend {

// Factory function type for backend creation.
using BackendFactory = std::function<BackendPtr()>;

// BackendRegistry holds the set of available backends. In the current phase
// backends are registered statically at compile time; dynamic .so loading is
// reserved for future industrialization.
class BackendRegistry {
public:
  static BackendRegistry &instance();

  // Register a backend factory under a unique name.
  void registerBackend(const std::string &name, BackendFactory factory);

  // Create a backend by name. Returns nullptr if not found.
  BackendPtr createBackend(const std::string &name) const;

  // List registered backend names.
  std::vector<std::string> listBackends() const;

  // Check if a backend is registered.
  bool hasBackend(const std::string &name) const;

private:
  BackendRegistry() = default;

  std::unordered_map<std::string, BackendFactory> factories_;
};

// Helper macro to register a backend at program startup.
#define REGISTER_BACKEND(name, BackendClass)                                   \
  static bool g_backend_registrar_##name = []() {                              \
    ::oml::backend::BackendRegistry::instance().registerBackend(               \
        #name, []() { return std::make_shared<BackendClass>(); });             \
    return true;                                                               \
  }()

} // namespace backend
} // namespace oml
