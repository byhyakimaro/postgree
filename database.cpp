#include "modules/database.h"
#include "includes/dotenv.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <pqxx/zview>

using json = nlohmann::json;

Database &Database::getInstance() {
  static Database instance;
  return instance;
}

Database::Database() {
  try {
    EnvLoader::load();

    std::string dbname = EnvLoader::get("DB_NAME", "evia");
    std::string user = EnvLoader::get("DB_USER", "postgres");
    std::string pass = EnvLoader::get("DB_PASS", "");
    std::string host = EnvLoader::get("DB_HOST", "127.0.0.1");
    std::string port = EnvLoader::get("DB_PORT", "5432");

    std::string conn_str = "dbname=" + dbname + " user=" + user +
                           " password=" + pass + " host=" + host +
                           " port=" + port;

    conn_ = std::make_unique<pqxx::connection>(conn_str);

    if (conn_->is_open()) {
      std::cout << "Conectado ao PostgreSQL com sucesso!\n";
    } else {
      throw std::runtime_error("Falha ao conectar ao banco");
    }
  } catch (const std::exception &e) {
    std::cerr << "Erro de conexão com PostgreSQL: " << e.what() << std::endl;
    throw;
  }
}

void Database::createTablesIfNotExist() {
  pqxx::work txn(*conn_);
  txn.exec(R"(
        CREATE TABLE IF NOT EXISTS usuarios (
            id SERIAL PRIMARY KEY,
            nome VARCHAR(100) NOT NULL,
            valor DECIMAL(10,2) DEFAULT 0.00,
            criado_em TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )");
  txn.commit();
  std::cout << "Tabela 'usuarios' pronta.\n";
}

void Database::insertUser(const std::string &nome, double valor) {
  pqxx::work txn(*conn_);

  txn.exec("INSERT INTO usuarios (nome, valor) VALUES ($1, $2)",
           pqxx::params(nome, valor));

  txn.commit();
}

std::string Database::getAllUsers() {
  pqxx::nontransaction txn(*conn_);
  pqxx::result result =
      txn.exec("SELECT id, nome, valor, criado_em FROM usuarios ORDER BY id");

  json usuarios = json::array();
  for (auto row : result) {
    usuarios.push_back({{"id", row["id"].as<int>()},
                        {"nome", row["nome"].as<std::string>()},
                        {"valor", row["valor"].as<double>()},
                        {"criado_em", row["criado_em"].as<std::string>()}});
  }

  return usuarios.dump(2); // JSON formatado com indentação
}
