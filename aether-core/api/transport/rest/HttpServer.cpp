#include "HttpServer.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

namespace Aether::Api
{
    bool HttpServer::start(int port)
    {
        int serverFd = socket(AF_INET, SOCK_STREAM, 0);

        if (serverFd < 0)
        {
            return false;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        {
            close(serverFd);
            return false;
        }

        listen(serverFd, 10);

        std::cout << "HTTP Server iniciado na porta " << port << std::endl;

        while(true)
        {
            int client = accept(serverFd, nullptr, nullptr);

            if(client < 0)
                continue;

            char buffer[4096];

            memset(buffer,0,sizeof(buffer));

            read(client, buffer, sizeof(buffer));

            std::string request(buffer);

            std::string method;
            std::string path;

            auto p1 = request.find(' ');
            auto p2 = request.find(' ', p1 + 1);

            method = request.substr(0, p1);
            path   = request.substr(p1 + 1, p2 - p1 - 1);

            auto response = m_router.route(method, path);

            std::string http;

            http += "HTTP/1.1 ";
            http += std::to_string(response.status);

            if(response.status == 200)
                http += " OK\r\n";
            else
                http += " ERROR\r\n";

            http += "Content-Type: application/json\r\n";
            http += "Content-Length: ";
            http += std::to_string(response.body.size());
            http += "\r\n";
            http += "Connection: close\r\n";
            http += "\r\n";
            http += response.body;

            write(client,
                  http.c_str(),
                  http.size());

            close(client);
        }
    }
}