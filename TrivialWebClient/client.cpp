
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

//Crea la conexion, devuelve true si pudo y false en caso de que no
bool client::startConnection(const string& host) {
	boost::asio::ip::tcp::resolver::iterator end; //Valor creado para comparar los iteradores
	boost::asio::ip::tcp::resolver::iterator endpoint_copy; //Copia del interator para manejo de la funcion
	endpoint = client_resolver->resolve(boost::asio::ip::tcp::resolver::query(host, HELLO_PORT_STR)); //Cargamos el puerto
	endpoint_copy = endpoint;
	cout << "Trying to connect to " << host << " on port " << HELLO_PORT_STR << std::endl;

	boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && endpoint_copy != end)//Probamos puerto por puerto, en caso de error salta "error" y si se pasa salta endpoint_copy
	{
		socket_forClient->close();//Cerramos socket anterior para poder abrir otro
		socket_forClient->connect(*endpoint_copy++, error);
	}
	
	if (endpoint == end || error)//No se pudo entablar la conexion con el servidor/host
	{
		socket_forClient->close();
		cout << "Host not Found" << endl;
		return false;
	}
	else
	{
		cout << "Connection established." << endl; //avisamos que se logro
		socket_forClient->non_blocking(true);
		return true;
	}
}
//Escribe la data en el stream del socket
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
//Lee y guarda en un txt lo que mande el servidor
void client::receiveMessage() {
	boost::system::error_code error;
	char buf[512];
	size_t len = 0;
	cout << "Receiving Message" << std::endl;
	do 
	{
		len = socket_forClient->read_some(boost::asio::buffer(buf), error);

		if (!error) //El error implica que termino, se agrega un terminador
			buf[len] = '\0';

	} while (error.value() == WSAEWOULDBLOCK);

	if (!error)
	{
		std::cout << std::endl << "Server says: " << buf << std::endl;
		CopyMessage(buf,len); //Copiamos lo recibido
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