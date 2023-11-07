#include <boost/asio.hpp>
#include <string>
#include <functional>

#include <iostream>
#include <boost\bind.hpp>
#include <chrono>
#include <fstream>
#include "../../Blockchain/Blocks/json.hpp"
#include <boost/lexical_cast.hpp>
#include <boost\asio\ip\address.hpp>
#include <queue>

using namespace std;


#define MAXSIZE 999999


const enum class ServerState : unsigned int 
{
	FREE,
	PERFORMING,
	OK,
	FAIL,
	FINISHED
};



//Bind
namespace {
	using Response = function<const string(const string&, const boost::asio::ip::tcp::endpoint&)>;
	using errorResp = function<const string(void)>;
}



class ServerA
{
public:
	ServerA(boost::asio::io_context&, const Response&, const Response&, const errorResp&, unsigned int);
	virtual ~ServerA();

protected:
	void newConnection(void);

	const enum class ConnectionTypes
	{
		NONE = 0,
		POST,
		GET
	};

	struct Connection {
		Connection(boost::asio::io_context& io_context) : socket(io_context), state(ServerState::FREE){}
		boost::asio::ip::tcp::socket socket;
		char reader[MAXSIZE];
		std::string response;
		ServerState state;
	};


	/*Connection methods.*/
	void asyncConnection(Connection*);
	void closeConnection(Connection*);

	void answer(Connection*, const std::string&);
	
	Response getResponse;
	Response postResponse;
	errorResp errorResponse;


	/*Callbacks*/
	void validateInput(Connection*, const boost::system::error_code& error, size_t bytes);
	void connectCallback(Connection*, const boost::system::error_code& error);
	void msgCallback(Connection*, const boost::system::error_code& error, size_t bytes_sent);



	/*Boost::asio data.*/
	boost::asio::io_context& io_context;
	boost::asio::ip::tcp::acceptor acceptor;
	std::list<Connection> sockets;

	/*Connection data.*/
	size_t size;
	std::string host;
	ConnectionTypes type;
	unsigned int port;
};









