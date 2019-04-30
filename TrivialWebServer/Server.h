#pragma once

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#define HELLO_PORT 80


using namespace std;



class server {
public:
	server();
	void startConnection();
	void sendMessage(const string& message);
	string receiveMessage();
	void writeCompletitionCallback(const boost::system::error_code& error, size_t transfered_bytes);
	~server();

private:
	boost::asio::io_service*  IO_handler;
	boost::asio::ip::tcp::socket* socket_forServer;
	boost::asio::ip::tcp::acceptor* server_acceptor;
};