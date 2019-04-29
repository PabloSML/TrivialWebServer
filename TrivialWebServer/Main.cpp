
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#define CRLF "\r\n"
#define HELLO_PORT 80
#define SERVER_IP "127.0.0.1"

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
	
	const char * buf = message.c_str();

	if((message.find("GET")==string::npos) || (message.find(" HTTP/1.1")==string::npos) || (message.find("Host:")== string::npos) || (message.find("GET") != 0))
	{
		cout << "Not found" << endl;
		return false;
	}

	string aux = "Host:";
	int pos = message.find("Host:") + aux.size();
	int dotcounter = 0;
	int numcounter = 0;
	bool hostvalid= false;
	string sub = message.substr(pos);
	

	while (!hostvalid) {
		if (buf[pos] >= '0' && buf[pos] <= '9')
		{
			numcounter++;
			if (numcounter > 3)
			{
				cout << "Error number" << endl;
				return false;
			}
		}
		else if (buf[pos] == '.')
		{
			dotcounter++;
			numcounter = 0;
			if (dotcounter > 3)
			{
				cout << "Error dot" << endl;
				return false;
			}
		}
		else if (buf[pos] == ' ' || buf[pos] == '\r')
		{
			if (dotcounter != 0 && numcounter != 0) {
				if (dotcounter == 3 && numcounter > 0)
				{
					hostvalid = true;
				}
				else
				{
					cout << "Error format" << endl;
					return false;
				}
			}
		}
		pos++;
	}

	return true;
}

string buildResponse(const string& pathFile, const streampos& size, const string& content)
{
	boost::posix_time::ptime timeRn = boost::posix_time::second_clock::local_time();
	stringstream ss;
	ss << timeRn;
	boost::local_time::local_date_time ldt(boost::local_time::not_a_date_time);
	ss >> ldt;
	boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
	ss.imbue(locale(locale::classic(), output_facet));
	output_facet->format("%a, %d %b %Y %H:%M:%S GMT");
	ss.str("");
	ss << ldt;
	string actualTime = ss.str();

	timeRn += boost::posix_time::seconds(30);
	ss.str("");
	ss << timeRn;
	ss >> ldt;
	ss.imbue(locale(locale::classic(), output_facet));
	output_facet->format("%a, %d %b %Y %H:%M:%S GMT");
	ss.str("");
	ss << ldt;
	string timePlus30 = ss.str();

	string response = string("HTTP/1.1 200 OK") + CRLF + "Date: " + actualTime + CRLF + "Location: " +
		SERVER_IP + "/" + pathFile + CRLF + "Cache-Control: max-age=30" + CRLF + "Expires: " + timePlus30 +
		CRLF + "Content-Lenght: " + to_string(size) + CRLF + "Content-Type: text/html; charset=iso-8859-1" + CRLF + content + CRLF;

	return response;
}

string buildResponse()
{
	boost::posix_time::ptime timeRn = boost::posix_time::second_clock::local_time();
	stringstream ss;
	ss << timeRn;
	boost::local_time::local_date_time ldt(boost::local_time::not_a_date_time);
	ss >> ldt;
	boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
	ss.imbue(locale(locale::classic(), output_facet));
	output_facet->format("%a, %d %b %Y %H:%M:%S GMT");
	ss.str("");
	ss << ldt;
	string actualTime = ss.str();

	timeRn += boost::posix_time::seconds(30);
	ss.str("");
	ss << timeRn;
	ss >> ldt;
	ss.imbue(locale(locale::classic(), output_facet));
	output_facet->format("%a, %d %b %Y %H:%M:%S GMT");
	ss.str("");
	ss << ldt;
	string timePlus30 = ss.str();

	string response = string("HTTP/1.1 404 Not Found") + CRLF + "Date: " + actualTime + CRLF +
		"Cache-Control: public, max-age=30" + CRLF + "Expires: " + timePlus30 + CRLF + "Content-Lenght: 0" +
		"Content-Type: text/html; charset=iso-8859-1" + CRLF;

	return response;
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
				content.push_back(buffer[i]);

			delete[] buffer;

			string response = buildResponse(pathFile, size, content);
			cout << "Press Enter to Reply  " << endl;
			getchar();
			conquering.sendMessage(response);
			Sleep(50); // Le damos 50ms para que llegue el mensaje antes de cerrar el socket.
		}
		else
		{
			string response = buildResponse();
			cout << "Press Enter to Reply  " << endl;
			getchar();
			conquering.sendMessage(response);
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