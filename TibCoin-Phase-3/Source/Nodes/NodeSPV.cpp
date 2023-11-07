#include "NodeSPV.h"
#include "Client/AllClients.h"
#include <typeinfo>

namespace 
{
	const char* MERKLEPOST = "send_merkle_block";
}

//Constructor inherits from Node
NodeSPV::NodeSPV(boost::asio::io_context& io_context, const std::string& ip,
	const unsigned int port, const unsigned int id, const GuiInfo guiMsg): 
	Node(io_context, ip, port, id, guiMsg)
{
	//SPV node has a key
	this->publicKey = to_string(rand() % 99999999);
}

//Callback server get
const std::string NodeSPV::getResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	setConnectedClientID(nodeInfo);

	guiMsg.setMsg("Node " + std::to_string(id) + " is answering a request from "
		+ (connectedClientId == -1 ? "an unknown node." : "node " + std::to_string(connectedClientId)));


	json result;
	result["status"] = false;

	result["result"] = 2;
	return headerFormat(result.dump());
}

//Post server callback
const std::string NodeSPV::postResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo)
{
	setConnectedClientID(nodeInfo);

	guiMsg.setMsg("Node " + std::to_string(id) + " is answering a request from "
		+ (connectedClientId == -1 ? "an unknown node." : "node " + std::to_string(connectedClientId)));

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
			//merklepost bullshit
			
			/*json merkle = json::parse(request.substr(data + 5, content - data - 5));

			if (!validateMerkleBlock(merkle)) {
				merkles.push_back(merkle);

				perform(ConnectionType::GETHEADER, (*std::begin(neighbors)).first, "0", NULL);
			}*/
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
	
	if (neighbors.find(id) != neighbors.end()) 
	{
		json var;

		var["key"] = key;

		clients.push_back(new FilterClient(neighbors[id].ip, port + 1, neighbors[id].port, var));
		clientState = ConnectionState::PERFORMING;
	}
	
};

void NodeSPV::transaction(const unsigned int id, const std::string& wallet, const unsigned int amount) 
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

		clients.push_back(new TransactionClient(neighbors[id].ip, port + 1, neighbors[id].port, tempData));
		clientState = ConnectionState::PERFORMING;
	}
	
}

void NodeSPV::getBlockHeaders(const unsigned int id, const std::string& blockID, const unsigned int count) 
{
	
	if (neighbors.find(id) != neighbors.end() && count) 
	{
		clients.push_back(new GetHeaderClient(neighbors[id].ip, port + 1, neighbors[id].port, blockID, count));
		clientState = ConnectionState::PERFORMING;
	}
	
};


void NodeSPV::perform()
{
	if (clients.size() && clients.front() && !clients.front()->performRequest())
	{
		if (typeid(*clients.front()) == typeid(GetHeaderClient))
		{
			const json& temp = clients.front()->getAnswer();
			if (temp.find("status") != temp.end() && temp["status"])
			{
				if (temp.find("result") != temp.end())
				{
					std::string res = temp["result"].dump();
					for (const auto& header : temp["result"])
					{
						headers[header["blockid"]] = header;
					}
				}
			}
		}
		delete clients.front();
		clients.pop_front();
	}
	
}


std::vector<Actions> NodeSPV::getActions(void)
{
	std::vector<Actions> actionvector;

	actionvector.push_back(Actions(ActionType::S, "Post Transaction"));
	//actionvector.push_back(Actions(ActionType::R, "Post merkleblock"));
	//actionvector.push_back(Actions(ActionType::S, "Post Filter"));
	//actionvector.push_back(Actions(ActionType::S, "Get Block headers"));

	return actionvector;
}


const std::string NodeSPV::getKey(void)
{
	return publicKey;
}
