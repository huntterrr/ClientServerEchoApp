#include <bits/stdc++.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include<boost/function.hpp>


class Server {
public:

	Server(boost::asio::io_context& io, short port) : 
		io_(io), acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), sock_(io){}
	
	void start_accept() {
		acceptor_.async_accept(sock_, 
			boost::bind(&Server::connection_handler, this, boost::asio::placeholders::error));
	}
private:
	void connection_handler(const boost::system::error_code &ec) {
		if (!ec) {
			std::cout << "connected\n";
			Read();
		}
		start_accept();
	}

	void Read() {
		sock_.async_read_some(boost::asio::buffer(data_), 
			boost::bind(&Server::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));		
	}

	void Write(size_t bytes) {
		sock_.async_write_some(boost::asio::buffer(data_, bytes),
			boost::bind(&Server::write_handler, this, boost::asio::placeholders::error));
	}

	void read_handler(const boost::system::error_code &ec, size_t bytes) {
		if (!ec) {
			std::string received(data_, bytes);
			std::cout << "received: " << received<<'\n';
			Write(bytes);
		}
	}

	void write_handler(const boost::system::error_code &ec) {
		if (!ec) Read();
	}
private:
	boost::asio::io_context& io_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::socket sock_;
	enum {max_len = 1024};
	char data_[max_len];
};

int main() {
	try {
		boost::asio::io_context io;
		Server server(io, 9001);
		server.start_accept();
		io.run();
	}
	catch (std::exception e) {
		std::cerr << e.what() << '\n';
	}
	return 0;
}