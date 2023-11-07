#pragma once
#include "Node.h"
#include "../BlockChain.h"


class NodeFull : public Node {
public:
	NodeFull(boost::asio::io_context&, const std::string&, const unsigned int, const unsigned int);
	~NodeFull();

	virtual void transaction(const unsigned int, const std::string& wallet, const unsigned int amount);

	virtual void postBlock(const unsigned int, const std::string& blockID);
	virtual void postMerkleBlock(const unsigned int, const std::string& blockID, const std::string& transID);

	virtual void postFilter(const unsigned int, const std::string& key) {};

	virtual void getBlocks(const unsigned int, const std::string& blockID, const unsigned int count);
	virtual void getBlockHeaders(const unsigned int, const std::string& blockID, const unsigned int count) {};

	virtual std::vector <Actions> getActions(void);

	virtual void perform();

private:
	const json getMerkleBlock(const std::string&, const std::string&);
	const json& getBlock(const std::string& blockID);

	virtual const std::string getResponse(const std::string&, const boost::asio::ip::tcp::endpoint&);
	virtual const std::string postResponse(const std::string&, const boost::asio::ip::tcp::endpoint&);
	
	//Full node has the whole blockchain
	BlockChain blockChain;
};