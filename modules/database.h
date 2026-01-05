#pragma once
#include <pqxx/pqxx>
#include <string>
#include <memory>

class Database {
public:
    static Database& getInstance();
    ~Database() = default;

    void createTablesIfNotExist();
    void insertUser(const std::string& nome, double valor = 0.0);
    std::string getAllUsers();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

private:
    Database();

    std::unique_ptr<pqxx::connection> conn_;
};
