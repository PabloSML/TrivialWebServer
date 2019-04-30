#include "server.h"

void server::writeCompletitionCallback(const boost::system::error_code& error, size_t transfered_bytes) {
	cout << endl << "Write Callback called" << endl;
}

void server::startConnection() {
	server_acceptor->accept(*socket_forServer);
	socket_forServer->non_blocking(true);
}

void server::sendMessage(const string& message) {

	size_t len;
	boost::system::error_code error;

	do
	{
		len = socket_forServer->write_some(boost::asio::buffer(message, message.size()), error);
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;
}

string server::receiveMessage() {
	boost::system::error_code error;
	char buf[512];
	string request = "Error";
	size_t len = 0;
	cout << "Receiving Message" << endl;
	do
	{
		len = socket_forServer->read_some(boost::asio::buffer(buf), error);

		if (!error)
		{
			buf[len] = '\0';
			request = string(buf);
		}

	} while (error.value() == WSAEWOULDBLOCK);

	if (!error)
		cout << endl << "Client says: " << request << endl;
	else
		cout << "Error while trying to connect to server " << error.message() << endl;

	return request;
}

server::server() {
	IO_handler = new boost::asio::io_service();
	socket_forServer = new boost::asio::ip::tcp::socket(*IO_handler);
	server_acceptor = new boost::asio::ip::tcp::acceptor(*IO_handler,
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), HELLO_PORT));
	cout << endl << "Ready. Port " << HELLO_PORT << " created" << endl;
}

server::~server() {
	server_acceptor->close();
	socket_forServer->close();
	delete server_acceptor;
	delete socket_forServer;
	delete IO_handler;
}