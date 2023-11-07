#pragma once
#include "Transactions/Transaction.h"
#include "json.hpp"
#include <list>

class BlockChain;

const enum class BlockInfo
{
	SEE_MROOT,
	CALCULATE_MROOT,
	BLOCKID,
	PREVIOUS_BLOCKID,
	NTX,
	BLOCK_NUMBER,
	NONCE,
	VALIDATE_MROOT
};

using namespace std;
using json = nlohmann::json;

class BlockMicho {
public:
	
	BlockMicho(const json& block);
	~BlockMicho();

	friend BlockChain;

private:
	string fliptree(string str);
	
	const std::list < std::string> getIDs();
	
	const string getData(const BlockInfo& data);
	std::string printTree(void);

	unsigned int ntx;
	unsigned int height;
	unsigned int nonce;
	string blockid;
	string previousBlockId;
	string merkleRoot;

	std::list<std::string> nodes, ids;
	void parseIds(void);


	/*"""Hashing"""*/
	static unsigned int generateID(unsigned char* str);
	inline static const std::string hex2ASCII(unsigned int);
	inline static const std::string hash(const std::string&);

	/*Merkle Tree*/
	void buildTree();
	std::vector<std::string> tree;

	
	json tx;
	json header;
	json jsonData;
	string calculatedMerkleRoot;
	string isValidMR;
	bool validated;

	
};


