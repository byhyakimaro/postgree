#pragma once
#include <pqxx/pqxx>
#include <string>
#include <memory>

class Database {
public:
    // Singleton para facilitar (uma única conexão global)
    static Database& getInstance();
    ~Database() = default;

    // Executa operações no banco
    void createTablesIfNotExist();
    void insertUser(const std::string& nome, double valor = 0.0);
    std::string getAllUsers();  // Retorna JSON simples como string

    // Não permitir cópias
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

private:
    Database();

    std::unique_ptr<pqxx::connection> conn_;
};
