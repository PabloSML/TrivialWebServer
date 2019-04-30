#include "server.h"

#define CRLF "\r\n"
#define SERVER_IP "127.0.0.1"


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
			string response = buildResponse();			//****esto no deberia ir.
			cout << "Press Enter to Reply  " << endl;
			getchar();
			response = "Error 404. Page not found";		//****deberia tirar un error al no encontrar el archivo.
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