#ifndef ENV_LOADER_H
#define ENV_LOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

class EnvLoader {
public:
    static void load(const std::string& filename = ".env") {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Aviso: Arquivo " << filename << " não encontrado." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = trim(line.substr(0, delimiterPos));
                std::string value = trim(line.substr(delimiterPos + 1));

                #ifdef _WIN32
                  _putenv_s(key.c_str(), value.c_str());
                #else
                  setenv(key.c_str(), value.c_str(), 1);
                #endif
            }
        }
    }

    static std::string get(const std::string& key, const std::string& defaultValue = "") {
        const char* val = std::getenv(key.c_str());
        return val ? std::string(val) : defaultValue;
    }

private:
    static std::string trim(const std::string& s) {
        size_t first = s.find_first_not_of(" \t\r\n");
        if (std::string::npos == first) return s;
        size_t last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, (last - first + 1));
    }
};

#endif
