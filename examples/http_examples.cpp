#include <client_http.hpp>
#include <future>
#include <server_http.hpp>

// Added for the default_resource example
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

int main() {
  // HTTP-server at port 8080 using 1 thread
  // Unless you do more heavy non-threaded processing in the resources,
  // 1 thread is usually faster than several threads
  HttpServer server;
  server.config.port = 8080;

  // Add resources using path-regex and method-string, and an anonymous function
  // POST-example for the path /string, responds the posted string
  server.resource["^/string$"]["POST"] =
      [](std::shared_ptr<HttpServer::Response> response,
         std::shared_ptr<HttpServer::Request> request) {
        // Retrieve string:
        auto content = request->content.string();
        // request->content.string() is a convenience function for:
        // stringstream ss;
        // ss << request->content.rdbuf();
        // auto content=ss.str();

        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length()
                  << "\r\n\r\n"
                  << content;

        // Alternatively, use one of the convenience functions, for instance:
        // response->write(content);
      };

  // POST-example for the path /json, responds firstName+" "+lastName from the
  // posted json Responds with an appropriate error message if the posted json
  // is not valid, or if firstName or lastName is missing Example posted json:
  // {
  //   "firstName": "John",
  //   "lastName": "Smith",
  //   "age": 25
  // }
  server.resource["^/json$"]["POST"] =
      [](std::shared_ptr<HttpServer::Response> response,
         std::shared_ptr<HttpServer::Request> request) {
        try {
          std::string name = "firstName";

          *response << "HTTP/1.1 200 OK\r\n"
                    << "Content-Length: " << name.length() << "\r\n\r\n"
                    << name;
        } catch (const std::exception &e) {
          *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: "
                    << std::strlen(e.what()) << "\r\n\r\n"
                    << e.what();
        }

        // Alternatively, using a convenience function:
        // try {
        //     ptree pt;
        //     read_json(request->content, pt);

        //     auto name=pt.get<string>("firstName")+"
        //     "+pt.get<string>("lastName"); response->write(name);
        // }
        // catch(const exception &e) {
        //     response->write(SimpleWeb::StatusCode::client_error_bad_request,
        //     e.what());
        // }
      };

  // GET-example for the path /info
  // Responds with request-information
  server.resource["^/info$"]["GET"] =
      [](std::shared_ptr<HttpServer::Response> response,
         std::shared_ptr<HttpServer::Request> request) {
        std::stringstream stream;
        stream << "<h1>Request from "
               << request->remote_endpoint().address().to_string() << ":"
               << request->remote_endpoint().port() << "</h1>";

        stream << request->method << " " << request->path << " HTTP/"
               << request->http_version;

        stream << "<h2>Query Fields</h2>";
        auto query_fields = request->parse_query_string();
        for (auto &field : query_fields)
          stream << field.first << ": " << field.second << "<br>";

        stream << "<h2>Header Fields</h2>";
        for (auto &field : request->header)
          stream << field.first << ": " << field.second << "<br>";

        response->write(stream);
      };

  // GET-example for the path /match/[number], responds with the matched string
  // in path (number) For instance a request GET /match/123 will receive: 123
  server.resource["^/match/([0-9]+)$"]["GET"] =
      [](std::shared_ptr<HttpServer::Response> response,
         std::shared_ptr<HttpServer::Request> request) {
        response->write(request->path_match[1].str());
      };

  // GET-example simulating heavy work in a separate thread
  server.resource["^/work$"]["GET"] =
      [](std::shared_ptr<HttpServer::Response> response,
         std::shared_ptr<HttpServer::Request> /*request*/) {
        std::thread work_thread([response] {
          std::this_thread::sleep_for(std::chrono::seconds(5));
          response->write("Work done");
        });
        work_thread.detach();
      };

  // Default GET-example. If no other matches, this anonymous function will be
  // called. Will respond with content in the web/-directory, and its
  // subdirectories. Default file: index.html Can for instance be used to
  // retrieve an HTML 5 client that uses REST-resources on this server
  server.default_resource["GET"] =
      [](std::shared_ptr<HttpServer::Response> response,
         std::shared_ptr<HttpServer::Request> request) {
        try {
          auto web_root_path = std::filesystem::canonical("web");
          auto path = std::filesystem::canonical(web_root_path / request->path);
          // Check if path is within web_root_path
          if (std::distance(web_root_path.begin(), web_root_path.end()) >
                  std::distance(path.begin(), path.end()) ||
              !std::equal(web_root_path.begin(), web_root_path.end(),
                          path.begin()))
            throw std::invalid_argument("path must be within root path");
          if (std::filesystem::is_directory(path)) path /= "index.html";

          SimpleWeb::CaseInsensitiveMultimap header;

      // Uncomment the following line to enable Cache-Control
      // header.emplace("Cache-Control", "max-age=86400");

#ifdef HAVE_OPENSSL
//    Uncomment the following lines to enable ETag
//    {
//      ifstream ifs(path.string(), ifstream::in | ios::binary);
//      if(ifs) {
//        auto hash =
//        SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
//        header.emplace("ETag", "\"" + hash + "\"");
//        auto it = request->header.find("If-None-Match");
//        if(it != request->header.end()) {
//          if(!it->second.empty() && it->second.compare(1, hash.size(), hash)
//          == 0) {
//            response->write(SimpleWeb::StatusCode::redirection_not_modified,
//            header); return;
//          }
//        }
//      }
//      else
//        throw invalid_argument("could not read file");
//    }
#endif

          auto ifs = std::make_shared<std::ifstream>();
          ifs->open(path.string(),
                    std::ifstream::in | std::ios::binary | std::ios::ate);

          if (*ifs) {
            auto length = ifs->tellg();
            ifs->seekg(0, std::ios::beg);

            header.emplace("Content-Length", to_string(length));
            response->write(header);

            // Trick to define a recursive function within this scope (for
            // example purposes)
            class FileServer {
             public:
              static void read_and_send(
                  const std::shared_ptr<HttpServer::Response> &response,
                  const std::shared_ptr<std::ifstream> &ifs) {
                // Read and send 128 KB at a time
                static std::vector<char> buffer(
                    131072);  // Safe when server is running on one thread
                std::streamsize read_length;
                if ((read_length =
                         ifs->read(&buffer[0],
                                   static_cast<std::streamsize>(buffer.size()))
                             .gcount()) > 0) {
                  response->write(&buffer[0], read_length);
                  if (read_length ==
                      static_cast<std::streamsize>(buffer.size())) {
                    response->send(
                        [response, ifs](const SimpleWeb::error_code &ec) {
                          if (!ec)
                            read_and_send(response, ifs);
                          else
                            std::cerr << "Connection interrupted" << std::endl;
                        });
                  }
                }
              }
            };
            FileServer::read_and_send(response, ifs);
          } else
            throw std::invalid_argument("could not read file");
        } catch (const std::exception &e) {
          response->write(
              SimpleWeb::StatusCode::client_error_bad_request,
              "Could not open path " + request->path + ": " + e.what());
        }
      };

  server.on_error = [](std::shared_ptr<HttpServer::Request> /*request*/,
                       const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
    // Note that connection timeouts will also call this handle with ec set to
    // SimpleWeb::errc::operation_canceled
  };

  // Start server and receive assigned port when server is listening for
  // requests
  std::promise<unsigned short> server_port;
  std::thread server_thread([&server, &server_port]() {
    // Start server
    server.start(
        [&server_port](unsigned short port) { server_port.set_value(port); });
  });
  std::cout << "Server listening on port " << server_port.get_future().get()
            << std::endl
            << std::endl;

  // Client examples
  std::string json_string =
      "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

  // Synchronous request examples
  {
    HttpClient client("localhost:8080");
    try {
      std::cout << "Example GET request to http://localhost:8080/match/123"
                << std::endl;
      auto r1 = client.request("GET", "/match/123");
      std::cout << "Response content: " << r1->content.rdbuf()
                << std::endl  // Alternatively, use the convenience function
                              // r1->content.string()
                << std::endl;

      std::cout << "Example POST request to http://localhost:8080/string"
                << std::endl;
      auto r2 = client.request("POST", "/string", json_string);
      std::cout << "Response content: " << r2->content.rdbuf() << std::endl
                << std::endl;
    } catch (const SimpleWeb::system_error &e) {
      std::cerr << "Client request error: " << e.what() << std::endl;
    }
  }

  // Asynchronous request example
  {
    HttpClient client("localhost:8080");
    std::cout << "Example POST request to http://localhost:8080/json"
              << std::endl;
    client.request("POST", "/json", json_string,
                   [](std::shared_ptr<HttpClient::Response> response,
                      const SimpleWeb::error_code &ec) {
                     if (!ec)
                       std::cout
                           << "Response content: " << response->content.rdbuf()
                           << std::endl;
                   });
    client.io_service->run();
  }

  server_thread.join();
}
