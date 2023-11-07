#include "BlockChain.h"
#include <fstream>

using namespace std;

BlockChain::BlockChain()
{

}

BlockChain::~BlockChain()
{

}

BlockChain::BlockChain(const string& filename)
{
	loadBlockChain(filename);
}

void BlockChain::loadBlockChain(const string& filename)
{
	Blocks.clear();

	fstream jsonFile(filename, ios::in);

	if (!jsonFile.is_open()) 
	{
		jsonFile.close();
		throw std::exception("Failed to open file!");
	}

	jsonObj = json::parse(jsonFile);

	jsonFile.close();

	/*We cerate a block for every json object.*/
	for (auto& j : jsonObj)
	{
		Blocks.push_back(BlockMicho(j));
	}
}

const string BlockChain::getBlockInfo(int id, const BlockInfo& member) 
{
	return Blocks[id].getData(member);
}

const unsigned int BlockChain::getBlockQuantity() 
{ 
	return Blocks.size(); 
}

const json& BlockChain::getBlock(unsigned int index) 
{ 
	return Blocks[index].jsonData;
}

void BlockChain::addBlock(const json& block) 
{
	Blocks.push_back(BlockMicho(block));

	jsonObj.push_back(block);
}

const std::vector<std::string>& BlockChain::getTree(unsigned int index) 
{
	if (!Blocks[index].tree.size())
	{
		Blocks[index].buildTree();
	}

	return Blocks[index].tree;
};

unsigned int BlockChain::getBlockIndex(const std::string& blockID) 
{
	for (unsigned int i = 0; i < Blocks.size(); i++) 
	{
		if (Blocks[i].header["blockid"] == blockID)
		{
			return i;
		}
	}
	return -1;
}

const json& BlockChain::getHeader(unsigned int index) 
{ 
	return Blocks[index].header; 
}

const string BlockChain::drawTree(unsigned int id) 
{
	return Blocks[id].printTree();
};

const std::string BlockChain::calculateMRoot(const json& newBlock) 
{
	BlockMicho temp(newBlock);
	temp.getData(BlockInfo::VALIDATE_MROOT);
	return temp.calculatedMerkleRoot;
}

const std::string BlockChain::calculateBlockID(const json& block) 
{
	BlockMicho temp(block);
	auto& header = temp.header;

	std::string result;

	if (header.find("blockid") != header.end())
	{
		header.erase("blockid");
	}

	for (auto& member: header) 
	{
		result.append(member.get<std::string>());
	}

	//hashes result of temporary block header and returns it
	return BlockMicho::hash(result);
};

const std::string BlockChain::calculateTXID(const json& trans) 
{
	std::string result;
	for (auto& input: trans["vin"]) 
	{
		result.append(input["txid"].get<std::string>());
	}

	//hashes reslt of transactions and returns it
	return BlockMicho::hash(result);
};

const std::string BlockChain::generateID(const std::string& block)
{ 
	return BlockMicho::hash(block); 
}
