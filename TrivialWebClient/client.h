#pragma once

#include <iostream>
#include <cstdio>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/timer/timer.hpp>

#define CRLF "\r\n"
#define HELLO_PORT_STR "80"
#define SERVER_IP "localhost"

using namespace std;

class client {
public:
	client();
	bool startConnection(const string& host); //Inicia la coneccion con el HOST recibido
	void sendMessage(const string& message); //Manda un mensaje al servidor
	void receiveMessage(); //Recibe el mensaje del servidor y lo guarda
	~client();

private:
	void CopyMessage(char* s,size_t length); //Copia un mensaje recibido en un txt
	boost::asio::io_service* IO_handler;
	boost::asio::ip::tcp::socket* socket_forClient;
	boost::asio::ip::tcp::resolver* client_resolver;
	boost::asio::ip::tcp::resolver::iterator endpoint;
};
