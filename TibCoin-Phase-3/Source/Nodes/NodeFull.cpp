#include "NodeFull.h"
#include <typeinfo>
#include "Client/AllClients.h"

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
	const unsigned int port, const unsigned int identifier, const GuiInfo& guiMsg)
	: Node(io_context, ip, port, identifier, guiMsg)
{
	//full node has no key
	this->publicKey = "";
}


//get block request
void NodeFull::getBlocks(const unsigned int id, const std::string& blockID, const unsigned int count) 
{
	/*if (clientState == ConnectionState::FREE && !client) 
	{*/
	if (neighbors.find(id) != neighbors.end() && count) 
	{
		guiMsg.setMsg("\nNode " + std::to_string(id) + " is performing a client request. (NodeFull.cpp)");
		clients.push_back(new GetBlockClient(neighbors[id].ip, port + 1, neighbors[id].port, blockID, count));
		//clientState = ConnectionState::PERFORMING;
	}
	//}
}


//Post block request
void NodeFull::postBlock(const unsigned int id, const unsigned int index) 
{
	/*if (clientState == ConnectionState::FREE && !client) 
	{*/
	if (neighbors.find(id) != neighbors.end()) 
	{
		//guiMsg.setMsg("\nNode " + std::to_string(id) + " is performing a client request. (NodeFull.cpp)");
		clients.push_back(new BlockClient(neighbors[id].ip, port + 1, neighbors[id].port, blockChain.getBlock(index)));
		//clientState = ConnectionState::PERFORMING;
	}
	/*}*/
}

//Post merkleblock request
void NodeFull::postMerkleBlock(const unsigned int id, const std::string& blockID, const std::string& transID) 
{
	/*if (clientState == ConnectionState::FREE && !client) 
	{*/
	if (neighbors.find(id) != neighbors.end()) 
	{
		auto temp = getMerkleBlock(blockID, transID);
		guiMsg.setMsg("\nNode " + std::to_string(id) + " is performing a client request. (NodeFull.cpp)");
		clients.push_back(new MerkleClient(neighbors[id].ip, port + 1, neighbors[id].port, temp));
		//clientState = ConnectionState::PERFORMING;
	}
	//}
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
	/*if (clientState == ConnectionState::FREE && !client) 
	{*/
	if (neighbors.find(id) != neighbors.end()) 
	{
		json var;
		//updtae with data from node TODo

		var["txid"] = "ABCDE123";
		var["nTxin"] = 0;
		var["nTxout"] = 1;
		var["vin"] = json();

		json vout;
		vout["amount"] = amount;
		vout["publicid"] = wallet;

		var["vout"] = vout;

		guiMsg.setMsg("\nNode " + std::to_string(id) + " is performing a client request. (NodeFull.cpp)");
		clients.push_back(new TransactionClient(neighbors[id].ip, port + 1, neighbors[id].port, var));
		//clientState = ConnectionState::PERFORMING;
	}
	//}
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
	setConnectedClientID(nodeInfo);

	guiMsg.setMsg("\nNode " + std::to_string(id) + " is answering a request from "
		+ (connectedClientId == -1 ? "an unknown node." : "node " + std::to_string(connectedClientId)));

	json result;
	result["status"] = true;
	int block;
	//serverState = ConnectionState::FAILED;


	if ((block = request.find(BLOCKSGET)) || request.find(HEADERGET)) 
	{
		int posId = request.find("block_id=");
		int posCount = request.find("count=");

		if (posId != std::string::npos && posCount != std::string::npos)
		{
			json response;

			std::string id = request.substr(posId + 9, request.find_last_of("&") - posId - 9);

			int cont = std::stoi(request.substr(posCount + 6, request.find("HTTP") - posCount - 6));

			int absoluteIndex;
			if (id == "0")
			{
				absoluteIndex = 0;
				//TODO if hashed failed????
			}
			else
			{
				absoluteIndex = blockChain.getBlockIndex(id);
			}



			if (!(++absoluteIndex))
			{
				result["status"] = false;
				result["result"] = 2;
			}
			else 
			{
				if (!cont)
				{
					cont = blockChain.getBlockQuantity();
				}
				
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
					absoluteIndex++;
					//serverState = ConnectionState::OK;
				}

				result["result"] = response;

				//cheching only
				std::string res = response.dump();
				//std::string realRes = blockChain.get
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


	guiMsg.setMsg("\nNode " + std::to_string(id) + " is answering a request from " + std::to_string(connectedClientId));

	//serverState = ConnectionState::FAILED;

	json result;
	result["status"] = false;
	result["result"] = NULL;

	if (request.find(BLOCKPOST) != std::string::npos) 
	{
		int content = request.find("Content-Type");
		int data = request.find("Data=");

		if (content != std::string::npos && data != std::string::npos)
		{
			json blockData = json::parse(request.substr(data + 5, content - data - 5));
			if (validateBlock(blockData))
			{
				guiMsg.setMsg("\nNode " + std::to_string(id) + " validated the block sent by:  " + std::to_string(connectedClientId) + " and it was correct!");
				blockChain.addBlock(blockData);
			}
			/*TODO*/
			//Something to do with transactions


			//set status true
			result["status"] = true;
			
			
			//Something to do with utxo



			//Clean transaction queue because block is already mined
			transactions = json();
		}
	}
	else if (request.find(TRANSPOST) != std::string::npos) 
	{
		//Got a transaction
		int content = request.find("Content-Type");
		int data = request.find("Data=");
		json trans;

		if (content != std::string::npos && data != std::string::npos)
		{
			trans = json::parse(request.substr(data + 5, content - data - 5));
			if (UTXOs.find(trans["txid"]) == UTXOs.end())
			{
				if (validateTransaction(trans, false))
				{
					//Send to its neighbors Todo


					updateUTXOs(trans);
				}
				result["status"] = true;
			}
		}
	}
	else if (request.find(FILTERPOST) != std::string::npos) 
	{
		int content = request.find("Content-Type");
		int data = request.find("Data=");
		
		if (content != std::string::npos && data != std::string::npos) 
		{
			for (auto& neighbor : neighbors) 
			{
				if (neighbor.second.ip == nodeInfo.address().to_string() && neighbor.second.port == (nodeInfo.port() - 1)) 
				{
					neighbor.second.filter = request.substr(data + 5, content - data - 5);
					result["status"] = true;
				}
			}
		}
		//serverState = ConnectionState::OK;
	}

	return headerFormat(result.dump());
}


void NodeFull::perform() 
{
	if (clients.size() && clients.front() && !clients.front()->performRequest()) 
	{
		if (typeid(*clients.front()) == typeid(GetBlockClient)) 
		{
			const json& temp = clients.front()->getAnswer();
			if (temp["status"]) 
			{
				for (const auto& block : temp["result"]) 
				{
					blockChain.addBlock(block);
				}
			}
		}
		delete clients.front();
		clients.pop_front();
		clientState = ConnectionState::FINISHED;
	}
}

std::vector <Actions> NodeFull::getActions(void)
{
	std::vector<Actions> actionvector;

	actionvector.push_back(Actions(ActionType::SR, "Post Transaction"));
	actionvector.push_back(Actions(ActionType::SR, "Post fake transaction"));
	actionvector.push_back(Actions(ActionType::SR, "Post fake block"));

	return actionvector;
}

bool NodeFull::validateTransaction(const json& transaction, bool checked)
{
	bool result = false;

	std::map<std::string, int> usedIds;

	/*Checks if it's a new transaction or an old one*/
	if (transaction.find("vin") != transaction.end()) 
	{
		result = true;

		int totout = 0;

		//print("Validating vins.");
		for (const auto& input: transaction["vin"]) 
		{
			if (usedIds.find(input["txid"]) != usedIds.end() && usedIds[input["txid"]] == input["outputIndex"]) 
			{
				result = false;
			}
			else 
			{
				usedIds[input["txid"]] = input["outputIndex"];
			}
		}

		if (!checked) 
		{
			//print("Adding outputs");
			for (const auto& output: transaction["vout"]) 
			{
				totout += output["amount"];
			}

			int totin = 0;

			//print("Adding inputs");
			for (const auto& utxo: usedIds) 
			{
				if (UTXOs.find(utxo.first) == UTXOs.end()) 
				{
					result = false;
				}
				else if ((UTXOs[utxo.first]["vout"].size() > utxo.second) && !UTXOs[utxo.first]["vout"][utxo.second].is_null()) 
				{
					totin += UTXOs[utxo.first]["vout"][utxo.second]["amount"];
					//UTXOs[utxo.first]["vout"].erase(utxo.second);
				}
			}

			if (totin != totout) 
			{
				std::string ttttfuckingtttetosofuckingtt = transaction.dump();
				result = false;
			}
		}
	}

	if (!result)
	{
		guiMsg.setMsg("\nNode " + std::to_string(id) + " is rejecting a transaction.");
	}

	return result;
}


bool NodeFull::validateBlock(const json& block)
{
	int blockCount = blockChain.getBlockQuantity();

	bool doneMiner = false;
	bool result = false;

	if ((blockCount && blockChain.getBlock(blockCount - 1)["blockid"] != block["blockid"]) || !blockCount) 
	{
		if (BlockChain::calculateMRoot(block) == block["merkleroot"] && BlockChain::calculateBlockID(block) == block["blockid"]) 
		{
			result = true;
			std::string bl = block.dump();
			for (auto& trans : block["tx"]) 
			{
				if (trans["vin"].is_null() && !doneMiner) 
				{
					doneMiner = true;
				}
				else
				{
					if (!validateTransaction(trans, true))
					{
						result = false;
					}
				}
			}
		}
		else
		{
			guiMsg.setMsg("\nNode " + std::to_string(id) + " is rejecting a block.");
		}
	}

	return result;
}

const std::string NodeFull::getKey(void)
{
	return publicKey;
}



void NodeFull::updateUTXOs(const json& trans) 
{
	for (const auto& input : trans["vin"]) 
	{
		if (UTXOs.find(input["txid"]) != UTXOs.end()) 
		{
			std::string inp = input.dump();
			if (UTXOs[input["txid"]]["vout"].size() > input["outputIndex"].get<int>()) 
			{
				UTXOs[input["txid"]]["vout"][input["outputIndex"].get<int>()] = json();
			}
		}
	}

	transactions.push_back(trans["txid"]);
	UTXOs[trans["txid"]] = trans;
}