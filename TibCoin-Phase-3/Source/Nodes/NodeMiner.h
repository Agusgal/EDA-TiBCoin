#pragma once
#include "NodeFull.h"

class NodeMiner : public NodeFull
{
public:
	NodeMiner(boost::asio::io_context&, const std::string&, const unsigned int, const unsigned int, const GuiInfo guiMsg);
	
	virtual ~NodeMiner();

	virtual void perform();

	virtual const json getFeeTrans(bool);

	/*Sets info (if necessary) and queues new action.*/
	virtual void perform(ConnectionType type, const unsigned int id, const std::string& blockID, const unsigned int count);

protected:

	void mine(bool real);

	bool checkChallenge(std::string hashedHeader);
};