#pragma once
#include <string>
#include <unordered_map>

class EnvLoader {
public:
    static std::unordered_map<std::string, std::string> load(const std::string& path);
    static std::string buildConnectionString(const std::unordered_map<std::string, std::string>& env);
};