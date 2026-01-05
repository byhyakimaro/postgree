#include "../includes/httplib.h"
#include <functional>
#include <string>

class Route {
public:
  using Handler = std::function<void(const httplib::Request&, httplib::Response&)>;

  enum class Method { GET, POST };

  Route() = default;

  Route(Method m,const std::string& path, Handler handler)
    : method_(m), path_(path), handler_(handler) {}

  void handle(const httplib::Request& req, httplib::Response& res) const {
    handler_(req, res);
  }

  void register_route(httplib::Server& svr) const {
    if (method_ == Method::GET) {
      svr.Get(path_.c_str(), [this](const httplib::Request& req, httplib::Response& res){
        this->handle(req, res);
      });
    } else if (method_ == Method::POST) {
      svr.Post(path_.c_str(), [this](const httplib::Request& req, httplib::Response& res){
        this->handle(req, res);
      });
    }
  }

  const std::string& path() const { return path_; }
  Method method() const { return method_; }

private:
  Method method_;
  std::string path_;
  Handler handler_;
};
