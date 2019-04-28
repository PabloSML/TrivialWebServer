#pragma once

#include <boost/asio.hpp>
#include <string>
#include <cstdio>
#include <iostream>
using namespace std;

#define SERVER_TIMEOUT "timeout"

class Server {
public:
	Server(string port);
	~Server();
	void connect();
	string getInfo();
	string getInfoTimed(int ms);

private:
	boost::asio::io_service* IO_handler;
	boost::asio::ip::tcp::socket* serverSocket;
	boost::asio::ip::tcp::acceptor* serverAcceptor;
};