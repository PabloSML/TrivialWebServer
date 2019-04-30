#include "server.h"

//Constructor
server::server() {
	IO_handler = new boost::asio::io_service();
	socket_forServer = new boost::asio::ip::tcp::socket(*IO_handler);
	server_acceptor = new boost::asio::ip::tcp::acceptor(*IO_handler,
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), HELLO_PORT));
	cout << endl << "Ready. Port " << HELLO_PORT << " created" << endl;
}

//Destructor
server::~server() {
	server_acceptor->close();
	socket_forServer->close();
	delete server_acceptor;
	delete socket_forServer;
	delete IO_handler;
}


//Server acepta una coneccion y setea el socket en modo no bloqueante.
void server::startConnection() {
	server_acceptor->accept(*socket_forServer);
	socket_forServer->non_blocking(true);
}

//Envia mensaje al socket. En caso de haber un error, lo imprime en pantalla.
void server::sendMessage(const string& message) {

	size_t len;
	boost::system::error_code error;

	do
	{
		len = socket_forServer->write_some(boost::asio::buffer(message, message.size()), error);	//Escribe el mensaje en el socket.
	} while ((error.value() == WSAEWOULDBLOCK));													//Evalua si el tipo de error es de operation_would_block.
	if (error)
		std::cout << "Error while trying to connect to client " << error.message() << std::endl;	//De haber un error, avisa por terminal que no se pudo conectar el client.
}

//Devuelve el mensaje recibido. En caso de surgir un error, avisa por terminal.
string server::receiveMessage() {
	boost::system::error_code error;
	char buf[512];
	string request = "Error";
	size_t len = 0;
	cout << "Receiving Message" << endl;
	do
	{
		len = socket_forServer->read_some(boost::asio::buffer(buf), error);		//Lee el mensaje del socket y lo guarda en el arreglo.

		if (!error)
		{
			buf[len] = '\0';
			request = string(buf);												//arreglo de chars a string para luego devolver el string del mensaje.
		}

	} while (error.value() == WSAEWOULDBLOCK);									//Evalua si el tipo de error es de operation_would_block.

	if (!error)																	//Si no hubo ningun error, imprime el mensae en la terminal.
		cout << endl << "Client says: " << request << endl;
	else
		cout << "Error while trying to connect to client " << error.message() << endl;	//De haber un error, avisa por terminal que no se pudo conectar el client.

	return request;
}

