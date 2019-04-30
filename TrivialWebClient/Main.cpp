
#include "client.h"

/*
Arma el mensaje en el formato requerido:
GET /path/filename HTTP/1.1 CRLF 
Host:127.0.0.1 CRLF 
CRLF 
 */
string buildRequest(const string& host, const string& pathFile)
{
	string message = "GET " + pathFile + " HTTP/1.1" + CRLF + "Host:" + host + CRLF + CRLF;

	return message;
}

//Recibe el host y el path para enviar el mensaje al server.
void asioTcpClient(string host, string pathFile)
{
	client conquering;
	if (conquering.startConnection(host))					//Intenta conectarse.
	{
		string message = buildRequest(host, pathFile);		//Arma el mensaje segun el formato requerido.
		std::cout << "Press Enter to Send Request " << std::endl;	
		getchar();											//Espera un 'ENTER' para enviar el mensaje.
		conquering.sendMessage(message);					//Envia el mensaje al server
		Sleep(50);											// Le damos 50ms para que llegue el mensaje.
		conquering.receiveMessage();						//Recive el mensaje del server.
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
		std::streampos end = input.find_first_of('/');
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