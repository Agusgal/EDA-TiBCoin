#pragma once
#include "Client/Client.h"
#include "Server/Server.h"
#include "../Actions/Actions.h"
#include "../Gui/GuiDisplayInfo.h"

#include <map>
#include <vector>

using namespace std;


const enum class ConnectionState : unsigned int 
{
	FREE,
	PERFORMING,
	OK,
	FAILED,
	FINISHED
};

const enum class ConnectionType : unsigned int 
{
	POSTMERKLE,
	POSTTRANS,
	POSTFILTER,
	GETBLOCK,
	GETHEADER,
	POSTBLOCK,
	FALSEBLOCK,
	FALSETRANSACTION,
	PING,//might erase
	LAYOUT//might erase
};


struct Neighbour 
{
	string ip;
	string filter;
	unsigned int port;
};


class Node 
{
public:
	Node(boost::asio::io_context& io_context, const std::string& ip, const unsigned int port,
		const unsigned int identifier, const GuiInfo& guiMsg);
	virtual ~Node();

	virtual void newNeighbor(const unsigned int id, const std::string& ip, const unsigned int port, const std::string &publicKey);

	virtual const unsigned int getId(void);
	virtual ConnectionState getClientState(void);
	virtual ConnectionState getServerState(void);
	virtual map <unsigned int, Neighbour> getNeighbours(void);
	virtual int getClientPort(void);
	
	/*virtual methods*/
	virtual void perform(void) = 0;

	virtual void transaction(const unsigned int, const string& wallet, const unsigned int amount) = 0;
	virtual void postBlock(const unsigned int, const unsigned int index) = 0;
	virtual void postMerkleBlock(const unsigned int, const string& blockId, const string& transId) = 0;

	virtual void postFilter(const unsigned int, const string& key) = 0;
	
	virtual void getBlocks(const unsigned int, const string& blockId, const unsigned int count) = 0;
	virtual void getBlockHeaders(const unsigned int, const string& blockId, const unsigned int count) = 0;

	virtual const std::string getKey() = 0;

	virtual std::vector <Actions> getActions(void) = 0;


protected:
	virtual std::string makeDaytimeString(bool);

	virtual const string getResponse(const string&, const boost::asio::ip::tcp::endpoint&) = 0;
	virtual const string postResponse(const string&, const boost::asio::ip::tcp::endpoint&) = 0;
	virtual const string errorResponse(void);
	

	virtual const string headerFormat(const string&);
	virtual void setConnectedClientID(const boost::asio::ip::tcp::endpoint&);

	/*Client/Server stuff*/
	list<Client*> clients;
	ServerA server;
	ConnectionState clientState;
	ConnectionState serverState;
	int sentMessage;
	int receivedMessage;
	/*********************/

	
	/*Node Info*/
	string ip;
	unsigned int port;
	unsigned int id;
	/***********/


	/*transaction stuff*/
	string publicKey;
	std::map<std::string, json> UTXOs;
	/*******************/


	/*Misc*/
	GuiInfo guiMsg;
	map <unsigned int, Neighbour> neighbors;
	/******/
	

	/*Misc*/
	int connectedClientId;
	string errorMsg;
	/******/
};
