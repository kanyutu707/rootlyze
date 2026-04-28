#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>

class Server {
public:
    explicit Server(unsigned short port);
    void run();

private:
    void do_accept();

    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif