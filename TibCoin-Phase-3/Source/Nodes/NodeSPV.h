#pragma once
#pragma once
#include "Node.h"
#include "../Blockchain/Blocks/json.hpp"

class NodeSPV : public Node
{
public:
	NodeSPV(boost::asio::io_context&, const std::string&, const unsigned int port, const unsigned int id, const GuiInfo(gui));
	~NodeSPV();

	virtual void transaction(const unsigned int, const std::string& wallet, const unsigned int amount);

	virtual void postBlock(const unsigned int, const unsigned int) {};
	virtual void postMerkleBlock(const unsigned int, const std::string& blockID, const std::string& transID) {};

	virtual void postFilter(const unsigned int, const std::string& key);

	virtual void getBlocks(const unsigned int, const std::string& blockID, const unsigned int count) {};
	virtual void getBlockHeaders(const unsigned int, const std::string& blockID, const unsigned int count);

	virtual const std::string getKey(void);

	virtual std::vector<Actions> getActions(void);

	virtual void perform();

private:
	virtual const std::string getResponse(const std::string&, const boost::asio::ip::tcp::endpoint&);
	virtual const std::string postResponse(const std::string&, const boost::asio::ip::tcp::endpoint&);

	//SPV Node has only headers
	std::map<std::string, json> headers;

	json merkles;
};