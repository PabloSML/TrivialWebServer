
#include "client.h"

string buildRequest(const string& host, const string& pathFile)
{
	string message = "GET " + pathFile + " HTTP/1.1" + CRLF + "Host:" + host + CRLF + CRLF;

	return message;
}

void asioTcpClient(string host, string pathFile)
{
	client conquering;
	if (conquering.startConnection(host))
	{
		string message = buildRequest(host, pathFile);
		std::cout << "Press Enter to Send Request " << std::endl;
		getchar();
		conquering.sendMessage(message);
		Sleep(50);
		conquering.receiveMessage();
	}
}


#include <Windows.h>
int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		std::string host;
		std::string pathFile;
		std::string input = argv[1];
		std::streampos beg = 0;
		std::streampos end = input.find_first_of('\\');
		if (end != std::string::npos)
		{
			host = input.substr(beg, end - beg);
			beg = end;
			pathFile = input.substr(beg, std::string::npos);

			asioTcpClient(host, pathFile);
			std::cout << "Press Enter to exit..." << std::endl;
			getchar();
		}
		else
		{
			std::cout << "Missing commands..." << endl;
			getchar();
		}
	}
	else
	{
		std::cout << "Missing commands..." << std::endl;
		getchar();
	}
}