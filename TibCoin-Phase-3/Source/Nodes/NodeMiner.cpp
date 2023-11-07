#include "NodeMiner.h"
#include "Client/GetBlockClient.h"

#include <chrono>

const double timeElapsed = 15.0;
const double minerFee = 10;
const unsigned int zeroCount = 4;

NodeMiner::NodeMiner(boost::asio::io_context& io_context, const std::string& ip, const unsigned int port,
	const unsigned int identifier, const GuiInfo guiMsg)
	: NodeFull(io_context, ip, port, identifier, guiMsg)
{
}


/*Performs client mode. */
void NodeMiner::perform() 
{
	
	mine(true);
	
	
	
	if (clients.size() && clients.front() && !clients.front()->performRequest())
	{
		if (typeid(*clients.front()) == typeid(GetBlockClient))
		{
			const json& temp = clients.front()->getAnswer();
			if (temp.find("status") != temp.end() && temp["status"]) 
			{
				if (temp.find("result") != temp.end()) 
				{
					for (const auto& block : temp["result"]) 
					{
						blockChain.addBlock(block);
					}
				}
			}
		}
		delete clients.front();
		clients.pop_front();
	}

	
}

void NodeMiner::mine(bool real) 
{
	json block;

	

	for (const auto& trans: transactions) 
	{
		block["tx"].push_back(UTXOs[trans]);
	}

	block["tx"].push_back(getFeeTrans(real));

	block["height"] = blockChain.getBlockQuantity();
	block["nTx"] = transactions.size() + 1;

	//generates random nonce to mine
	block["nonce"] = rand() % 65536;


	int blockCount;
	if ((blockCount = blockChain.getBlockQuantity())) 
	{
		block["previousblockid"] = blockChain.getBlockInfo(blockCount - 1, BlockInfo::BLOCKID);
	}
	else {
		block["previousblockid"] = "00000000";
	}

	block["merkleroot"] = BlockChain::calculateMRoot(block);
	
	//we calculate blockid ----> has of the bloc header
	block["blockid"] = BlockChain::calculateBlockID(block);

	//check if challenged was completed
	bool completed = checkChallenge(block["blockid"]);

	if (completed) 
	{
		guiMsg.setMsg("\nNode " + std::to_string(id) + " has mined a block.");
		
		//add block to blockchain
		blockChain.addBlock(block);

		//clean transaction list
		transactions = json();

		//has to comunicate to other nodes when mining completed challenge
		for (auto& neighbor: neighbors)
		{
			//for every neighbor if it is a full node the we have to send the 
			if (!neighbor.second.filter.length()) 
			{
				guiMsg.setMsg("\nNode " + std::to_string(id) + " is performing a client request. (NodeMiner.cpp)");
				NodeFull::postBlock(neighbor.first, blockChain.getBlockQuantity() - 1);
			}
		}
	}
}

void NodeMiner::perform(ConnectionType type, const unsigned int id, const std::string& blockID, const unsigned int count) {
	/*if (type == ConnectionType::POSTBLOCK && actions[ConnectionType::POSTBLOCK]->isDataNull())
		actions[ConnectionType::POSTBLOCK]->setData(getBlock(blockID));

	else if (type == ConnectionType::POSTTRANS && actions[ConnectionType::POSTTRANS]->isDataNull()) {
		actions[ConnectionType::POSTTRANS]->setData(generateTransJSON(blockID, count, true));
	}

	else if (type == ConnectionType::FALSEBLOCK) {
		mineBlock(false);
		return;
	}

	if (actions.find(type) != actions.end()) {
		if (!actions[type]->isDataNull()) messenger.setMessage("Node " + std::to_string(identifier) + " is performing a client request.");

		actions[type]->Perform(id, blockID, count);
		actions[type]->clearData();
	}*/
}

/*This transaction is the miner reward and gets assigned every mined block (with or withour transactions)*/
const json NodeMiner::getFeeTrans(bool real)
{
	json result;

	result["vin"];

	json vout;

	vout["publicid"] = publicKey;
	vout["amount"] = minerFee;
	result["vout"].push_back(vout);
	result["nTxin"] = 0;
	result["nTxout"] = 1;
	result["txid"] = BlockChain::generateID(std::to_string(rand()));

	if (real)
		UTXOs[result["txid"]] = result;

	return result;
}


bool NodeMiner::checkChallenge(std::string hashedHeader)
{

	int count = 0;
	for (int i = 0; i < hashedHeader.size() && hashedHeader[i] == '0'; i++)
	{
		count++;
	}


	if (count >= zeroCount)
	{
		return true;
	}
	else
	{
		return false;
	}

}



NodeMiner::~NodeMiner() {}