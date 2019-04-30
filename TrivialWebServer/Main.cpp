/*
			Server
	
		Este server fue diseñado para atender pedidos dentro de una red interna. Al conectarse y
	recibir un pedido, provee la posibilidad de analizarlo antes de contestar utilizando la tecla
	de 'ENTER'.
		En caso de recibir un pedido invalido en formato, no contestara por red pero mostrara un
	mensaje en la terminar notificando la situacion y mostrando que se sigue esperando un pedido.
		Si el cliente se desconectara antes de una transferencia exitosa de datos, el server
	abrira nuevamente el puerto y esperara una nueva conexion.
		Si la transferencia de datos fue exitosa, se desconectara y finalizara el programa.


	Grupo 1:
	- Lin, Xi
	- Dutriez Diaz, Philippe
	- Smolkin, Pablo
*/
#include "server.h"

#define CRLF "\r\n"

// Esta funcion valida el formato del pedido enviado por el cliente
// Devuelve true en caso de ser valido, false de lo contrario
bool validateMessage(string& message)
{
	bool success = true;
	const char * buf = message.c_str();

	if((message.find("GET")==string::npos) || (message.find(" HTTP/1.1")==string::npos) || 
		(message.find("Host:")== string::npos) || (message.find("GET") != 0))	// busca las palabras clave de un pedido HTTP
	{
		cout << "Invalid request" << endl;
		success = false;
	}
	else
	{
		string aux = "Host:";
		int pos = message.find("Host:") + aux.size();
		int dotcounter = 0;
		int numcounter = 0;
		bool hostvalid= false;
		string sub = message.substr(pos);

		while (!hostvalid) // valida el formato del host IP
		{
			if (buf[pos] >= '0' && buf[pos] <= '9')
			{
				numcounter++;
				if (numcounter > 3)
				{
					cout << "Error number" << endl;
					success = false;
				}
			}
			else if (buf[pos] == '.')
			{
				dotcounter++;
				numcounter = 0;
				if (dotcounter > 3)
				{
					cout << "Error dot" << endl;
					success = false;
				}
			}
			else if (buf[pos] == ' ' || buf[pos] == '\r')
			{
				if (dotcounter != 0 && numcounter != 0) 
				{
					if (dotcounter == 3 && numcounter > 0)
					{
						hostvalid = true;
					}
					else
					{
						cout << "Error format" << endl;
						success = false;
					}
				}
			}
			pos++;
		}
	}

	return success;
}

// Esta funcion construye una respuesta segun el standard de HTTP y la devuelve en su nombre. Se llama si se encontro el archivo pedido
string buildResponse(const string& pathFile, const streampos& size, const string& content)
{
	boost::posix_time::ptime timeRn = boost::posix_time::second_clock::local_time();
	stringstream ss;
	ss << timeRn;
	boost::local_time::local_date_time ldt(boost::local_time::not_a_date_time);
	ss >> ldt;
	boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
	ss.imbue(locale(locale::classic(), output_facet));
	output_facet->format("%a, %d %b %Y %H:%M:%S GMT");	// se formatea la fecha acorde al standard de HTTP
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

// Esta funcion construye una respuesta segun el standard de HTTP y la devuelve en su nombre. Se llama si no se encontro el archivo pedido
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

// Esta funcion abre un puerto y busca una conexion con un cliente. Una vez lograda, recibe un pedido y,
// luego de validarlo, responde apropiadamente
// En caso de realizar una transferencia exitosa de datos, devuelve true. Caso contrario, devuelve false
bool TCPserver()
{
	bool goodDataTransfer = false;
	server conquering;
	string request;
	string pathFile;
	bool validInput = false;

	cout << endl << "Start Listening on port " << HELLO_PORT << endl;
	conquering.startConnection();
	cout << "Somebody connected to port " << HELLO_PORT << endl;
	do {									// este ciclo do-while corre mientras no se reciba un pedido valido (se considera valido un error de conexion)
		cout << "Waiting to hear from client" << endl;
		request = conquering.receiveMessage();
		if (request == "Error")
		{
			validInput = true;
		}
		else if (validateMessage(request))			
		{
			goodDataTransfer = true;
			validInput = true;
			string content;
			streampos size;
			char* buffer;
			streampos beg = request.find_first_of('/');		
			beg += 1;
			streampos end = request.find(" HTTP/1.1");
			pathFile = request.substr(beg, end - beg);	// se obtiene el path y filename especificados en el pedido
			const char* filename = pathFile.c_str();

			ifstream file(filename, ios::in | ios::binary);	// se intenta de abrir el archivo seleccionado
			if (file.is_open())
			{
				file.seekg(0, file.end);
				size = file.tellg();

				buffer = new char[size];
				file.seekg(0, ios::beg);
				file.read(buffer, size);
				file.close();

				for (int i = 0; i < size; i++)
					content.push_back(buffer[i]);	// el contenido del archivo se guarda en el string 'content'

				delete[] buffer;

				string response = buildResponse(pathFile, size, content);
				cout << "Press Enter to Reply  " << endl;
				getchar();
				conquering.sendMessage(response);
				Sleep(50); // Le damos 50ms para que llegue el mensaje antes de cerrar el socket.
			}
			else // en el caso de no haberse encontrado el archivo, la respuesta sera del tipo 404 NOT FOUND
			{
				string response = buildResponse();			
				cout << "Press Enter to Reply  " << endl;
				getchar();
				conquering.sendMessage(response);
				Sleep(50); // Le damos 50ms para que llegue el mensaje antes de cerrar el socket. 
			}
		}
	} while (!validInput);

	return goodDataTransfer;
}
int main(int argc, char* argv[])
{
	bool jobDone = false;
	do {
		jobDone = TCPserver();
	} while (!jobDone);
	cout << "Press Enter to exit..." << endl;
	getchar();
}