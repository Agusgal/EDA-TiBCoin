#include "Node.h"

using namespace std;


Node::Node(boost::asio::io_context& io_context, const std::string& ip, const unsigned int port, const unsigned int id, const GuiInfo& guiMsg)
	: ip(ip), clientState(ConnectionState::FREE), serverState(ConnectionState::FREE),
	port(port), id(id), connectedClientId(-1), server(io_context,
		std::bind(&Node::getResponse, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Node::postResponse, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Node::errorResponse, this), port), guiMsg(guiMsg)
{
	//set publicKey
	//this->publicKey = to_string(rand() % 99999999);
};


Node::~Node() 
{
	for (auto& client: clients) 
	{
		if (client) 
		{
			delete client;
			client = nullptr;
		}
	}
}


void Node::newNeighbor(const unsigned int id, const std::string& ip, const unsigned int port, const std::string& publicKey)
{
	bool addNeighbor = true;
	for (auto& neighbor : neighbors) 
	{
		if (neighbor.second.ip == ip && neighbor.second.port == port)
		{
			addNeighbor = false;
		}
	}
	
	if (addNeighbor)
	{
		neighbors[id] = { ip,publicKey, port };
	}
}


//Generates http response
const std::string Node::errorResponse() 
{
	json result;

	result["status"] = false;
	result["result"] = 1;

	return headerFormat(result.dump());
}

//Returns daytime formated string
std::string Node::makeDaytimeString(bool plusThirty) 
{
	using namespace std::chrono;
	system_clock::time_point currentTime = system_clock::now();

	if (plusThirty)
	{
		currentTime += seconds(30);
	}

	time_t nowTime = system_clock::to_time_t(currentTime);

	return ctime(&nowTime);
}

//Formats header (bizarre error here---> Fixed!!!!) (forgot to add a +4 to result length so client neves connected oops)
const std::string Node::headerFormat(const std::string& result) 
{
	return "HTTP/1.1 200 OK\r\nDate:" + makeDaytimeString(false) + "Location: " + "eda_coins" + "\r\nCache-Control: max-age=30\r\nExpires:" +
		makeDaytimeString(true) + "Content-Length:" + std::to_string(result.length() + 4) +
		"\r\nContent-Type: " + "text/html" + "; charset=iso-8859-1\r\n\r\n" + result;
}



const unsigned int Node::getId() 
{ 
	return id; 
}

//Check if necessary
ConnectionState Node::getClientState(void) 
{
	if (clientState == ConnectionState::FINISHED) 
	{
		clientState = ConnectionState::FREE;
		return ConnectionState::FINISHED;
	}
	else
	{
		return clientState;
	}

}

//Check if necessary
ConnectionState Node::getServerState(void) 
{
	switch (serverState) 
	{
	case ConnectionState::OK:
		serverState = ConnectionState::FINISHED;
		return ConnectionState::OK;
	case ConnectionState::FAILED:
		serverState = ConnectionState::FINISHED;
		return ConnectionState::FAILED;
	case ConnectionState::FINISHED:
		serverState = ConnectionState::FREE;
		return ConnectionState::FINISHED;
	default:
		break;
	}
}

//Check if necessary
int Node::getClientPort(void) 
{
	int temp = connectedClientId;
	connectedClientId = -1;
	return temp;
}

void Node:: setConnectedClientID(const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	for (const auto& neighbor : neighbors) 
	{
		if (neighbor.second.port + 1 == nodeInfo.port() && neighbor.second.ip == nodeInfo.address().to_string())
		{
			connectedClientId = neighbor.first;
		}
	}
}


map <unsigned int, Neighbour> Node::getNeighbours(void)
{
	return neighbors;
}