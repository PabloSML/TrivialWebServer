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
#define SERVER_IP "127.0.0.1"

using namespace std;



class server {
public:
	server();									//Constructor
	void startConnection();						//Server acepta una coneccion y setea el socket en modo no bloqueante.
	void sendMessage(const string& message);	//Envia mensaje al socket. En caso de haber un error, lo imprime en pantalla.
	string receiveMessage();					//Devuelve el mensaje recibido. En caso de surgir un error, avisa por terminal.
	~server();									//Destructor

private:
	boost::asio::io_service*  IO_handler;
	boost::asio::ip::tcp::socket* socket_forServer;
	boost::asio::ip::tcp::acceptor* server_acceptor;
};