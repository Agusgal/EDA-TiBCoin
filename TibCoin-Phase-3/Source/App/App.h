#pragma once
#include "../Gui/Gui.h"
#include "../Blockchain/BlockChain.h"
#include "../Nodes/Node.h"

class App 
{

public:
	App(void);
	~App(void);

	bool isRunning(void);

	void dispatcher(const Events&);
	const Events eventGenerator();

private:
	/*Prevents from using copy constructor.*/
	App(const App&);
	
	void performCom(void);
	const unsigned int getIndex(void);

	void updateGuiBlockData();

	void parseNodeData(void);
	void parseReceiverNodes(void);
	void validateActions(void);

	boost::asio::io_context io_context;

	BlockChain blockChain;

	Gui* gui;

	std::vector <Node*> nodes;

	bool running;
};