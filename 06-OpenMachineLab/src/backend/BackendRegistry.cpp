#include "backend/BackendRegistry.h"

namespace oml {
namespace backend {

BackendRegistry &BackendRegistry::instance() {
  static BackendRegistry registry;
  return registry;
}

void BackendRegistry::registerBackend(const std::string &name,
                                      BackendFactory factory) {
  factories_[name] = std::move(factory);
}

BackendPtr BackendRegistry::createBackend(const std::string &name) const {
  auto it = factories_.find(name);
  if (it == factories_.end()) {
    return nullptr;
  }
  return it->second();
}

std::vector<std::string> BackendRegistry::listBackends() const {
  std::vector<std::string> names;
  names.reserve(factories_.size());
  for (const auto &pair : factories_) {
    names.push_back(pair.first);
  }
  return names;
}

bool BackendRegistry::hasBackend(const std::string &name) const {
  return factories_.find(name) != factories_.end();
}

} // namespace backend
} // namespace oml
