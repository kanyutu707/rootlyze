#include "server.hpp"

#include <boost/beast.hpp>
#include <iostream>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket)
        : socket_(std::move(socket)) {}

    void start() {
        read_request();
    }

private:
    void read_request() {
        auto self = shared_from_this();

        http::async_read(socket_, buffer_, request_,
            [self](beast::error_code ec, std::size_t) {
                if (!ec) {
                    self->handle_request();
                }
            });
    }

    void handle_request() {
        if (request_.method() == http::verb::post) {
            std::cout << request_.body() << std::endl;
        }

        response_.version(request_.version());
        response_.result(http::status::ok);
        response_.set(http::field::server, "rootlyze");
        response_.set(http::field::content_type, "text/plain");
        response_.body() = "OK";
        response_.prepare_payload();

        write_response();
    }

    void write_response() {
        auto self = shared_from_this();

        http::async_write(socket_, response_,
            [self](beast::error_code ec, std::size_t) {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            });
    }

    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
};

Server::Server(unsigned short port)
    : ioc_(1),
      acceptor_(ioc_, tcp::endpoint(tcp::v4(), port)) {}

void Server::run() {
    do_accept();
    ioc_.run();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            do_accept();
        });
}