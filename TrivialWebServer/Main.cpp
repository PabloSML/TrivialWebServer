
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
using namespace std;

#define HELLO_PORT 80

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

bool validateMessage(string& message)
{
	return true;
}

void TCPserver()
{
	server conquering;
	string request;
	string pathFile;

	cout << endl << "Start Listening on port " << HELLO_PORT << endl;
	conquering.startConnection();
	cout << "Somebody connected to port " << HELLO_PORT << endl;
	cout << "Waiting to hear from client" << endl;
	request = conquering.receiveMessage();
	if (validateMessage(request))
	{
		string content;
		streampos size;
		char* buffer;
		streampos beg = request.find_first_of('/');
		beg += 1;
		streampos end = request.find(" HTTP/1.1");
		pathFile = request.substr(beg, end - beg);
		const char* filename = pathFile.c_str();

		ifstream file(filename, ios::in | ios::binary);
		if (file.is_open())
		{
			file.seekg(0, file.end);
			size = file.tellg();

			buffer = new char[size];
			file.seekg(0, ios::beg);
			file.read(buffer, size);
			file.close();

			for (int i = 0; i < size; i++)
			{
				content.push_back(buffer[i]);
			}

			delete[] buffer;

			cout << "Press Enter to Reply  " << endl;
			getchar();
			conquering.sendMessage(content);
			Sleep(50); // Le damos 50ms para que llegue el mensaje antes de cerrar el socket.
		}

	}
}
int main(int argc, char* argv[])
{
	TCPserver();
	cout << "Press Enter to exit..." << endl;
	getchar();
}