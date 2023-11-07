#pragma once
#include "Blocks/Block.h"

using json = nlohmann::json;
using namespace std;

class BlockChain
{
public:
	~BlockChain();
	BlockChain();
	BlockChain(const string&);

	void loadBlockChain(const string& filename);
	const string drawTree(unsigned int id);
	void addBlock(const json&);
	static const std::string calculateMRoot(const json& newBlock);

	static const std::string generateID(const std::string& block);

	static const std::string calculateBlockID(const json&);

	static const std::string calculateTXID(const json&);

	//getters
	const string getBlockInfo(int id, const BlockInfo&);
	const unsigned int getBlockQuantity();
	const json& getBlock(unsigned int);
	unsigned int getBlockIndex(const std::string&);
	const json& getHeader(unsigned int);
	const std::vector<std::string>& getTree(unsigned int);


private:
	vector <BlockMicho> Blocks;
	json jsonObj;
};