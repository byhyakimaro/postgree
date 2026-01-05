#include "includes/httplib.h"
#include "modules/route.h"
#include "modules/database.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

using json = nlohmann::json;

int main() {
  httplib::Server svr;
  auto& db = Database::getInstance();
  db.createTablesIfNotExist();

  std::vector<Route> routes = {

    {Route::Method::GET, "/hello", [](const httplib::Request&, httplib::Response& res){
      res.set_content("Hello GET!", "text/plain");
    }},

    {Route::Method::POST, "/usuarios", [](const httplib::Request& req , httplib::Response& res) {
      try {
        auto body_json = json::parse(req.body);

        if (!body_json.contains("nome")) {
          res.status = 400;
          res.set_content(R"({"erro": "Campo 'nome' é obrigatório"})", "application/json");
          return;
        }

        std::string nome = body_json["nome"];
        double valor = body_json.value("valor", 0.0);

        auto& db = Database::getInstance();
        db.insertUser(nome, valor);

        json resposta = {{"mensagem", "Usuário criado com sucesso"}, {"nome", nome}};
        res.set_content(resposta.dump(), "application/json");
      } catch (const std::exception& e) {
        res.status = 400;
        res.set_content(R"({"erro": "JSON inválido"})", "application/json");
      }
    }},

    {Route::Method::POST, "/hello", [](const httplib::Request&, httplib::Response& res){
      res.set_content("Hello POST!", "text/plain");
    }}
  };

  for (auto& r : routes) r.register_route(svr);

  std::cout << "Server running at http://127.0.0.1:8080\n";
  svr.listen("127.0.0.1", 8080);
}
