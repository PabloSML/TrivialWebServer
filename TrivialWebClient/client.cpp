
#include "client.h"
#include <fstream>


client::client() {
	IO_handler = new boost::asio::io_service();
	socket_forClient = new boost::asio::ip::tcp::socket(*IO_handler);
	client_resolver = new boost::asio::ip::tcp::resolver(*IO_handler);
}

client::~client() {
	socket_forClient->close();
	delete client_resolver;
	delete socket_forClient;
	delete IO_handler;
}

bool client::startConnection(const string& host) {
	boost::asio::ip::tcp::resolver::iterator end;
	boost::asio::ip::tcp::resolver::iterator endpoint_copy;
	endpoint = client_resolver->resolve(boost::asio::ip::tcp::resolver::query(host, HELLO_PORT_STR));
	endpoint_copy = endpoint;
	cout << "Trying to connect to " << host << " on port " << HELLO_PORT_STR << std::endl;

	boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && endpoint_copy != end)
	{
		socket_forClient->close();
		socket_forClient->connect(*endpoint_copy++, error);
	}
	
	if (endpoint == end || error)
	{
		socket_forClient->close();
		cout << "Host not Found" << endl;
		return false;
	}
	else
	{
		socket_forClient->non_blocking(true);
		return true;
	}
}

void client::sendMessage(const string& message) {

	size_t len;
	boost::system::error_code error;

	do
	{
		len = socket_forClient->write_some(boost::asio::buffer(message, message.size()), error);
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;
}

void client::receiveMessage() {
	boost::system::error_code error;
	char buf[512];
	size_t len = 0;
	cout << "Receiving Message" << std::endl;
	do
	{
		len = socket_forClient->read_some(boost::asio::buffer(buf), error);

		if (!error)
			buf[len] = '\0';

	} while (error.value() == WSAEWOULDBLOCK);

	if (!error)
	{
		std::cout << std::endl << "Server says: " << buf << std::endl;
		CopyMessage(buf,len);
	}
	else
	{
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;
	}
}

void
client::CopyMessage(char* server_info,size_t length)
{
	if (server_info != NULL)
	{
		ofstream dest;
		dest.open("server_copy.txt", ios::binary); // Abre el archivo de salida
		dest.write(server_info, length); // Escribe el bloque de memoria
		dest.close();
		std::cout << "Server copied" << std::endl;
	}
}