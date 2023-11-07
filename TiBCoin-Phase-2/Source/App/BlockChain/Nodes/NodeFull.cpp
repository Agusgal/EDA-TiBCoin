#include "NodeFull.h"
#include <typeinfo>
#include "../../Client/AllClients.h"


//Constants to satisfy request
namespace 
{
	const char* BLOCKPOST = "send_block";
	const char* TRANSPOST = "send_tx";
	const char* FILTERPOST = "send_filter";
	const char* BLOCKSGET = "get_blocks";
	const char* HEADERGET = "get_block_header";
}


const json error = {"error"};

//Constructor inherits from Node ----> if you want to run this project you need to configure path below (it doesnt work for us for some reason without absolute paths)
NodeFull::NodeFull(boost::asio::io_context& io_context, const std::string& ip,
	const unsigned int port, const unsigned int identifier)
	: Node(io_context, ip, port, identifier), blockChain("C:\\Users\\Agus\\source\\repos\\TiBCoin Phase 2\\Resources\\sample0.json")
{
}


//get block request
void NodeFull::getBlocks(const unsigned int id, const std::string& blockID, const unsigned int count) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		if (neighbors.find(id) != neighbors.end() && count) 
		{
			client = new GetBlockClient(neighbors[id].ip, port + 1, neighbors[id].port, blockID, count);
			clientState = ConnectionState::PERFORMING;
		}
	}
}


//Post block request
void NodeFull::postBlock(const unsigned int id, const std::string& blockID) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		if (neighbors.find(id) != neighbors.end()) 
		{
			client = new BlockClient(neighbors[id].ip, port + 1, neighbors[id].port, blockChain.getBlock(0)/*getBlock(blockID)*/);
			clientState = ConnectionState::PERFORMING;
		}
	}
}

//Post merkleblock request
void NodeFull::postMerkleBlock(const unsigned int id, const std::string& blockID, const std::string& transID) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		if (neighbors.find(id) != neighbors.end()) 
		{
			auto temp = getMerkleBlock(blockID, transID);
			client = new MerkleClient(neighbors[id].ip, port + 1, neighbors[id].port, temp);
			clientState = ConnectionState::PERFORMING;
		}
	}
}

//Get block request
const json& NodeFull::getBlock(const std::string& blockID) 
{
	for (unsigned int i = 0; i < blockChain.getBlockQuantity(); i++)
	{
		if (blockChain.getBlockInfo(i, BlockInfo::BLOCKID) == blockID)
		{
			return blockChain.getBlock(i);
		}
	}

	return error;
}

void NodeFull::transaction(const unsigned int id, const std::string& wallet, const unsigned int amount) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		if (neighbors.find(id) != neighbors.end()) 
		{
			json var;

			var["txid"] = "ABCDE123";
			var["nTxin"] = 0;
			var["nTxout"] = 1;
			var["vin"] = json();

			json vout;
			vout["amount"] = amount;
			vout["publicid"] = wallet;

			var["vout"] = vout;

			client = new TransactionClient(neighbors[id].ip, port + 1, neighbors[id].port, var);
			clientState = ConnectionState::PERFORMING;
		}
	}
}


const json NodeFull::getMerkleBlock(const std::string& blockID, const std::string& transID) 
{
	unsigned int k = 0;
	auto tree = blockChain.getTree(k);

	json result;
	int treeSize = static_cast<int>(log2(tree.size() + 1));
	
	std::vector<std::string> merklePath;

	while (k < (tree.size() - 1)) 
	{
		if (k % 2)
		{
			merklePath.push_back(tree[--k]);
		}
		else
		{
			merklePath.push_back(tree[k + 1]);
		}

		k = static_cast<unsigned int>(k / 2 + pow(2, treeSize - 1));
	}

	result["blockid"] = blockID;
	result["tx"] = blockChain.getBlock(0)["tx"];
	result["txPos"] = 0;
	result["merklePath"] = merklePath;

	return result;
}


NodeFull::~NodeFull() {}


//Get callback (Server)
const std::string NodeFull::getResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	json result;
	setConnectedClientID(nodeInfo);
	result["status"] = true;
	int block;
	serverState = ConnectionState::FAILED;


	if ((block = request.find(BLOCKSGET)) || request.find(HEADERGET)) 
	{
		int posId = request.find("block_id=");
		int posCount = request.find("count=");

		if (posId != std::string::npos && posCount != std::string::npos)
		{
			json response;

			std::string id = request.substr(posId + 9, request.find_last_of("&") - posId - 9);

			int cont = std::stoi(request.substr(posCount + 6, request.find("HTTP") - posCount - 6));

			int absoluteIndex = blockChain.getBlockIndex(id);

			if (!(++absoluteIndex))
			{
				result["status"] = false;
				result["result"] = 2;
			}
			else 
			{
				while (absoluteIndex < blockChain.getBlockQuantity() && cont)
				{
					if (block != std::string::npos) 
					{
						response.push_back(blockChain.getBlock(absoluteIndex));
					}
					else 
					{
						response.push_back(blockChain.getHeader(absoluteIndex));
					}
					cont--;
					serverState = ConnectionState::OK;
				}

				result["result"] = response;
			}
		}
		else 
		{
			result["status"] = false;
			result["result"] = 1;
		}
	}
	else
	{
		result["status"] = false;
		result["result"] = 2;
	}

	return headerFormat(result.dump());
}

//Post server (Server)
const std::string NodeFull::postResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	setConnectedClientID(nodeInfo);

	serverState = ConnectionState::FAILED;

	json result;
	result["status"] = true;
	result["result"] = NULL;

	if (request.find(BLOCKPOST) != std::string::npos) 
	{
		int content = request.find("Content-Type");
		int data = request.find("Data=");

		if (content == std::string::npos || data == std::string::npos)
		{
			result["status"] = false;
		}
		else 
		{
			blockChain.addBlock(json::parse(request.substr(data + 5, content - data - 5)));
			serverState = ConnectionState::OK;
		}
	}
	else if (request.find(TRANSPOST) != std::string::npos) 
	{
		serverState = ConnectionState::OK;
	}
	else if (request.find(FILTERPOST) != std::string::npos) 
	{
		serverState = ConnectionState::OK;
	}

	return headerFormat(result.dump());
}


void NodeFull::perform() 
{
	if (client) 
	{
		if (!client->performRequest())
		{
			if (typeid(*client) == typeid(GetBlockClient)) 
			{
				const json& temp = client->getAnswer();
				if (temp["status"]) 
				{
					for (const auto& block : temp["result"]) 
					{
						blockChain.addBlock(block);
					}
				}
			}
			delete client;
			client = nullptr;
			clientState = ConnectionState::FINISHED;
		}
	}
}

std::vector <Actions> NodeFull::getActions(void)
{
	std::vector<Actions> actionvector;

	actionvector.push_back(Actions(ActionType::SR, "Post Block"));
	actionvector.push_back(Actions(ActionType::SR, "Post Transaction"));
	actionvector.push_back(Actions(ActionType::S, "Post merkleblock"));
	actionvector.push_back(Actions(ActionType::R, "Post Filter"));
	actionvector.push_back(Actions(ActionType::R, "Get Block headers"));
	actionvector.push_back(Actions(ActionType::SR, "Get Blocks"));

	return actionvector;
}