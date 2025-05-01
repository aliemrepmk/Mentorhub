#include "EnvLoader.hpp"
#include <fstream>
#include <sstream>
#include <vector>

std::unordered_map<std::string, std::string> EnvLoader::load(const std::string& path) {
    std::unordered_map<std::string, std::string> env;
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto delimiter = line.find('=');
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            env[key] = value;
        }
    }
    return env;
}

std::string EnvLoader::buildConnectionString(const std::unordered_map<std::string, std::string>& env) {
    static const std::vector<std::string> required = { "DB_NAME", "DB_USER", "DB_PASS", "DB_HOST", "DB_PORT" };
    for (const auto& key : required) {
        if (env.find(key) == env.end()) {
            throw std::runtime_error("Missing required env variable: " + key);
        }
    }

    return "dbname=" + env.at("DB_NAME") +
           " user=" + env.at("DB_USER") +
           " password=" + env.at("DB_PASS") +
           " hostaddr=" + env.at("DB_HOST") +
           " port=" + env.at("DB_PORT");
}