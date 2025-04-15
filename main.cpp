#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <pistache/http.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>

using namespace Pistache;
using json = nlohmann::json;

// Dummy in-memory data
std::unordered_map<int, json> users;
std::unordered_map<int, json> cameras;

class RestApi {
public:
    explicit RestApi(Address addr) : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

    void init(size_t threads = 2) {
        auto opts = Http::Endpoint::options()
                        .threads(static_cast<int>(threads));
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

private:
    void setupRoutes() {
        using namespace Rest;

        Routes::Post(router, "/users", Routes::bind(&RestApi::createUser, this));
        Routes::Get(router, "/users", Routes::bind(&RestApi::getUsers, this));
        Routes::Put(router, "/users/:id", Routes::bind(&RestApi::updateUser, this));
        Routes::Delete(router, "/users/:id", Routes::bind(&RestApi::deleteUser, this));

        Routes::Post(router, "/cameras", Routes::bind(&RestApi::createCamera, this));
        Routes::Get(router, "/cameras", Routes::bind(&RestApi::getCameras, this));
        Routes::Put(router, "/cameras/:id", Routes::bind(&RestApi::updateCamera, this));
        Routes::Delete(router, "/cameras/:id", Routes::bind(&RestApi::deleteCamera, this));
    }

    void createUser(const Rest::Request& request, Http::ResponseWriter response) {
        auto body = json::parse(request.body());
        int id = body["id"];
        users[id] = body;
        response.send(Http::Code::Created, "User created");
    }

    void getUsers(const Rest::Request&, Http::ResponseWriter response) {
        json result = json::array();
        for (const auto& [id, user] : users) {
            result.push_back(user);
        }
        response.send(Http::Code::Ok, result.dump());
    }

    void updateUser(const Rest::Request& request, Http::ResponseWriter response) {
        int id = request.param(":id").as<int>();
        if (users.count(id)) {
            users[id] = json::parse(request.body());
            response.send(Http::Code::Ok, "User updated");
        } else {
            response.send(Http::Code::Not_Found, "User not found");
        }
    }

    void deleteUser(const Rest::Request& request, Http::ResponseWriter response) {
        int id = request.param(":id").as<int>();
        if (users.erase(id)) {
            response.send(Http::Code::Ok, "User deleted");
        } else {
            response.send(Http::Code::Not_Found, "User not found");
        }
    }

    void createCamera(const Rest::Request& request, Http::ResponseWriter response) {
        auto body = json::parse(request.body());
        int id = body["id"];
        cameras[id] = body;
        response.send(Http::Code::Created, "Camera created");
    }

    void getCameras(const Rest::Request&, Http::ResponseWriter response) {
        json result = json::array();
        for (const auto& [id, cam] : cameras) {
            result.push_back(cam);
        }
        response.send(Http::Code::Ok, result.dump());
    }

    void updateCamera(const Rest::Request& request, Http::ResponseWriter response) {
        int id = request.param(":id").as<int>();
        if (cameras.count(id)) {
            cameras[id] = json::parse(request.body());
            response.send(Http::Code::Ok, "Camera updated");
        } else {
            response.send(Http::Code::Not_Found, "Camera not found");
        }
    }

    void deleteCamera(const Rest::Request& request, Http::ResponseWriter response) {
        int id = request.param(":id").as<int>();
        if (cameras.erase(id)) {
            response.send(Http::Code::Ok, "Camera deleted");
        } else {
            response.send(Http::Code::Not_Found, "Camera not found");
        }
    }

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

int main() {
    Port port(9080);
    Address addr(Ipv4::any(), port);

    RestApi server(addr);
    server.init();
    std::cout << "Server is running on port 9080..." << std::endl;
    server.start();
}
