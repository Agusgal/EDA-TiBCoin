#include "NodeSPV.h"
#include "../../Client/AllClients.h"
#include <typeinfo>

namespace 
{
	const char* MERKLEPOST = "send_merkle_block";
}

//Constructor inherits from Node
NodeSPV::NodeSPV(boost::asio::io_context& io_context, const std::string& ip,
	const unsigned int port, const unsigned int identifier) : Node(io_context, ip, port, identifier)
{
}

//Callback server get
const std::string NodeSPV::getResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	setConnectedClientID(nodeInfo);
	serverState = ConnectionState::FAILED;

	json result;
	result["status"] = false;

	result["result"] = 2;
	return headerFormat(result.dump());
}

//Post server callback
const std::string NodeSPV::postResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo)
{
	setConnectedClientID(nodeInfo);
	serverState = ConnectionState::FAILED;
	json result;
	result["status"] = true;
	result["result"] = NULL;

	if (request.find(MERKLEPOST) != std::string::npos) 
	{
		int content = request.find("Content-Type");
		int data = request.find("Data=");
		if (content == std::string::npos || data == std::string::npos)
		{
			result["status"] = false;
		}
		else 
		{
			serverState = ConnectionState::OK;
		}
	}
	else 
	{
		result["status"] = false;
		result["result"] = 2;
	}

	return headerFormat(result.dump());
}


NodeSPV::~NodeSPV() {}

void NodeSPV::postFilter(const unsigned int id, const std::string& key) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		if (neighbors.find(id) != neighbors.end()) 
		{
			json var;

			var["key"] = key;

			client = new FilterClient(neighbors[id].ip, port + 1, neighbors[id].port, var);
			clientState = ConnectionState::PERFORMING;
		}
	}
};

void NodeSPV::transaction(const unsigned int id, const std::string& wallet, const unsigned int amount) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		if (neighbors.find(id) != neighbors.end()) 
		{
			json tempData;

			tempData["txid"] = "ABCDE123";
			tempData["nTxin"] = 0;
			tempData["nTxout"] = 1;
			tempData["vin"] = json();

			json vout;
			vout["amount"] = amount;
			vout["publicid"] = wallet;

			tempData["vout"] = vout;

			client = new TransactionClient(neighbors[id].ip, port + 1, neighbors[id].port, tempData);
			clientState = ConnectionState::PERFORMING;
		}
	}
}

void NodeSPV::getBlockHeaders(const unsigned int id, const std::string& blockID, const unsigned int count) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		if (neighbors.find(id) != neighbors.end() && count) 
		{
			client = new GetHeaderClient(neighbors[id].ip, port + 1, neighbors[id].port, blockID, count);
			clientState = ConnectionState::PERFORMING;
		}
	}
};


void NodeSPV::perform()
{
	if (client) 
	{
		if (!client->performRequest()) 
		{
			if (typeid(*client) == typeid(GetHeaderClient)) 
			{
				const json& temp = client->getAnswer();
				if (temp["status"]) 
				{
					for (const auto& header : temp["result"])
						headers.push_back(header);
				}
			}
			delete client;
			client = nullptr;
			clientState = ConnectionState::FINISHED;
		}
	}
}


std::vector<Actions> NodeSPV::getActions(void)
{
	std::vector<Actions> actionvector;

	actionvector.push_back(Actions(ActionType::S, "Post Transaction"));
	actionvector.push_back(Actions(ActionType::R, "Post merkleblock"));
	actionvector.push_back(Actions(ActionType::S, "Post Filter"));
	actionvector.push_back(Actions(ActionType::S, "Get Block headers"));

	return actionvector;
}