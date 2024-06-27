#include <bits/stdc++.h>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

boost::asio::io_context io;
boost::asio::ip::tcp::socket sock(io);
char read_buf[1024], write_buf[1024];

void Write();
void Read();

void connection_handler(const boost::system::error_code& ec) {
    if (!ec) {
        Write();
    }
    else {
        std::cerr << "Connection error: " << ec.message() << '\n';
    }
}

void read_handler(const boost::system::error_code& ec, std::size_t bytes) {
    if (!ec) {
        std::cout << "Server: " << std::string(read_buf, bytes) << '\n';
        Write();
    }
    else {
        std::cerr << "Client read error: " << ec.message() << '\n';
    }
}

void write_handler(const boost::system::error_code& ec, std::size_t /*bytes*/) {
    if (!ec) {
        Read();
    }
    else {
        std::cerr << "Client write error: " << ec.message() << '\n';
    }
}

void Read() {
    sock.async_read_some(boost::asio::buffer(read_buf),
        [](const boost::system::error_code& ec, std::size_t bytes) {
            read_handler(ec, bytes);
        });
}

void Write() {
    std::string msg;
    std::getline(std::cin, msg);
    std::fill(std::begin(write_buf), std::end(write_buf), 0);
    std::copy(msg.begin(), msg.end(), write_buf);
    boost::asio::async_write(sock, boost::asio::buffer(write_buf, msg.size()),
        [](const boost::system::error_code& ec, std::size_t bytes) {
            write_handler(ec, bytes);
        });
}

int main() {
    try {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::make_address("127.0.0.1"), 9001);
        sock.async_connect(ep, connection_handler);
        io.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    return 0;
}
