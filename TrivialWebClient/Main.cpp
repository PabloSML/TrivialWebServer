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
	void startConnection(const string& host);
	void sendMessage(const string& message);
	void receiveMessage();
	~client();

private:
	boost::asio::io_service* IO_handler;
	boost::asio::ip::tcp::socket* socket_forClient;
	boost::asio::ip::tcp::resolver* client_resolver;
	boost::asio::ip::tcp::resolver::iterator endpoint;
};

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

void client::startConnection(const string& host) {
	endpoint = client_resolver->resolve(
		boost::asio::ip::tcp::resolver::query(host, HELLO_PORT_STR));
	cout << "Trying to connect to " << host << " on port " << HELLO_PORT_STR << std::endl;
	boost::asio::connect(*socket_forClient, endpoint);
	socket_forClient->non_blocking(true);
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
		std::cout << std::endl << "Server says: " << buf << std::endl;
	else
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;
}

string buildRequest(const string& host, const string& pathFile)
{
	string message = "GET " + pathFile + " HTTP/1.1" + CRLF + "Host:" + host + CRLF + CRLF;

	return message;
}

void asioTcpClient(string host, string pathFile)
{
	client conquering;
	conquering.startConnection(host);
	string message = buildRequest(host, pathFile);
	std::cout << "Press Enter to Send Request " << std::endl;
	getchar();
	conquering.sendMessage(message);
	Sleep(50);
	conquering.receiveMessage();

}


#include <Windows.h>
int main(int argc, char* argv[])
{
	std::string host;
	std::string pathFile;
	std::string input = argv[1];
	std::streampos beg = 0;
	std::streampos end = input.find_first_of('/');
	host = input.substr(beg, end - beg);
	beg = end;
	pathFile = input.substr(beg, std::string::npos);

	asioTcpClient(host, pathFile);
	std::cout << "Press Enter to exit..." << std::endl;
	getchar();
}