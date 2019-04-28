#include "Server.h"

Server::Server(string port) {

	this->IO_handler = new boost::asio::io_service();
	this->serverSocket = new boost::asio::ip::tcp::socket(*(this->IO_handler));
	this->serverAcceptor = new boost::asio::ip::tcp::acceptor(*(this->IO_handler),
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), stoi(port)));

}

Server::~Server() {

	cout << "Disconnecting.." << endl;
	this->serverAcceptor->close();
	this->serverSocket->close();
	delete this->serverAcceptor;
	delete this->serverSocket;
	delete this->IO_handler;

}

void Server::connect() {

	cout << "Waiting for somebody to connect.. :( " << endl;
	this->serverAcceptor->accept(*(this->serverSocket));

}

string Server::getInfo() {

	char buffer[1 + 255 + 1];
	size_t lenght = 0;
	boost::system::error_code error;

	do {
		lenght = this->serverSocket->read_some(boost::asio::buffer(buffer), error);
	} while (error);

	buffer[lenght] = 0;
	string retValue = buffer;
	cout << "Recieved a message" << endl;

	return retValue;
}

string Server::getInfoTimed(int ms)
{
	//Timer timer;



	char buffer[1 + 255 + 1];
	size_t lenght = 0;
	boost::system::error_code error;

	//timer.start();

	bool timeout = false;

	do {
		lenght = this->serverSocket->read_some(boost::asio::buffer(buffer), error);
		//timer.stop();
		if (/*timer.getTime() > ms && lenght == 0*/1) { // Pido que lenght == 0 asi no lo paro mientras esta mandando
			timeout = true;
		}

	} while (error && !timeout);
	string retValue;

	if (!timeout) {
		buffer[lenght] = 0;
		retValue = buffer;
		cout << "Recieved a message" << endl;
	}
	else
		retValue = SERVER_TIMEOUT;

	return retValue;
}