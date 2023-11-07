#include "Server.h"
#include <iostream>
#include <boost\bind.hpp>
#include <fstream>

using json = nlohmann::json;
using boost::asio::ip::tcp;

namespace data
{
	const std::string fixed = "eda_coins";
	const char* autoIP = "127.0.0.1";
}


/*Server constructor. Initializes io_context, acceptor and socket.
Calls asyncConnection to accept connections.*/
ServerA::ServerA(boost::asio::io_context& io_context_, const Response& GET,
	const Response& POST, const errorResp& ERROR_RESP, unsigned int port) : host(data::autoIP), port(port), io_context(io_context_),
	acceptor(io_context_, tcp::endpoint(tcp::v4(), port)), getResponse(GET), postResponse(POST), errorResponse(ERROR_RESP)
{
	newConnection();
}


/*If there's a new neighbor, it sets a new socket.*/
void ServerA::newConnection()
{
	//Pushes new socket ---> implementation should be expandable in the future
	sockets.push_back(Connection(io_context));
	

	if (sockets.back().socket.is_open()) 
	{
		sockets.back().socket.shutdown(tcp::socket::shutdown_both);
		sockets.back().socket.close();
	}
	for (unsigned int i = 0; i < MAXSIZE; i++)
	{
		sockets.back().reader[i] = NULL;
	}

	asyncConnection(&sockets.back());
}



ServerA::~ServerA()
{
	for (auto& connector: sockets) 
	{
		if (connector.socket.is_open()) 
		{
			connector.socket.shutdown(tcp::socket::shutdown_both);
			connector.socket.close();
		}
	}

	if (acceptor.is_open())
	{
		acceptor.close();
	}
}


void ServerA::asyncConnection(Connection* connector)
{
	if (acceptor.is_open()) 
	{
		if (!connector->socket.is_open()) 
		{
			acceptor.async_accept(
				connector->socket, boost::bind(&ServerA::connectCallback,
					this, connector, boost::asio::placeholders::error)
			);
		}
	}
}


void ServerA::closeConnection(Connection* connector)
{
	connector->socket.shutdown(tcp::socket::shutdown_both);
	connector->socket.close();

	//Deletes socket if more than 1
	if (sockets.size() > 1)
	{
		auto ptr = sockets.begin();
		for (auto i = sockets.begin(); i != sockets.end(); i++) 
		{
			if (&(*i) == connector)
			{
				ptr = i;
			}
		}

		/*It clears the reader and reconnects.*/
		for (unsigned int i = 0; i < MAXSIZE; i++)
		{
			connector->reader[i] = NULL;
		}

		sockets.erase(ptr);
		newConnection();
	}
	else 
	{
		//Clear and reconnect
		for (unsigned int i = 0; i < MAXSIZE; i++)
		{
			connector->reader[i] = NULL;
		}
		asyncConnection(connector);
	}
}


//Get request input validation
void ServerA::validateInput(Connection* connector, const boost::system::error_code& error, size_t bytes)
{
	if (!error) 
	{
		std::string message = (*connector).reader;
		std::cout << "Server received: " + message << std::endl;

		//HTTP protocol validation
		std::string validator_GET = "GET /" + data::fixed + '/';
		std::string validator_POST = "POST /" + data::fixed + '/';
		std::string host_validator = " HTTP/1.1\r\nHost: " + host /*+ ':' + std::to_string(port)*/;

		//check if get or post request (or error)
		if (!message.find(validator_GET))
		{
			type = ConnectionTypes::GET;
		}
		else if (!message.find(validator_POST) && message.find("Data=") != std::string::npos)
		{
			type = ConnectionTypes::POST;
		}
		else 
		{
			type = ConnectionTypes::NONE;
		}

		if (message.find(host_validator) == std::string::npos)
		{
			type = ConnectionTypes::NONE;
		}
		answer(connector, message);
	}
	else
	{
		std::cout << "Failed to respond. Error: " << error.message() << std::endl;
	}
}


//When conencted execute this callback
void ServerA::connectCallback(Connection* connector, const boost::system::error_code& error)
{
	newConnection();

	connector->state = ServerState::PERFORMING;

	if (!error) 
	{
		//Sets socket to read request.
		connector->socket.async_read_some(
			boost::asio::buffer(connector->reader, MAXSIZE),
			boost::bind
			(
				&ServerA::validateInput,
				this, connector, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}
	else
	{
		std::cout << "Failed to respond. Error: " << error.message() << std::endl;
	}
}


//When message is sent to client use this callback
void ServerA::msgCallback(Connection* connector, const boost::system::error_code& error, size_t bytes_sent)
{
	if (error)
	{
		std::cout << "Failed to respond. Error: " << error.message() << std::endl;
	}
	
	connector->state = ServerState::FINISHED;
	closeConnection(connector);

	//connector->state = ServerState::FINISHED;
}

//Responds to input.
void ServerA::answer(Connection* connector, const std::string& message)
{
	//Depending on requestType a response is given with binded functions
	switch (type) 
	{
	case ConnectionTypes::POST:
		(connector)->response = postResponse(message, connector->socket.remote_endpoint());
		break;
	case ConnectionTypes::GET:
		(connector)->response = getResponse(message, connector->socket.remote_endpoint());
		break;
	case ConnectionTypes::NONE:
		(connector)->response = errorResponse();
		break;
	default:
		break;
	}

	connector->response += "\r\n\r\n";

	//Send to client
	(connector)->socket.async_write_some
	(
		boost::asio::buffer(connector->response),
		boost::bind
		(
			&ServerA::msgCallback,
			this, connector,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}




