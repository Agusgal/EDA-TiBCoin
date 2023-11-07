#include "App.h"
#include "../Nodes/NodeFull.h"
#include "../Nodes/NodeSPV.h"
#include "../Nodes/NodeMiner.h"

//App Constructor
App::App(void) : running(true)
{
	gui = new Gui;
}

//Dispatcher that decides based on Gui events.
void App::dispatcher(const Events& event) 
{
	switch (event)
	{
	case Events::END_EV:
		running = false;
		break;
	//Events related to phase 1 mode
	case Events::NEW_FILE_EV:
		blockChain.loadBlockChain(gui->getFilename());
		gui->setChainLength(blockChain.getBlockQuantity());
		break;
	case Events::FIRST_UPDATE_EV:
		updateGuiBlockData();
		break;
	case Events::VALIDATE_MROOT_EV:
		updateGuiBlockData();
		gui->setResultMsg(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::VALIDATE_MROOT));
		break;
	case Events::PRINT_TREE_EV:
		updateGuiBlockData();
		gui->setResultMsg(blockChain.drawTree(gui->getBlockIndex()));
		break;
	case Events::CALC_MROOT_EV:
		updateGuiBlockData();
		gui->setResultMsg(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::CALCULATE_MROOT));
		break;
	
	//Events related to Phase 2 mode
	case Events::NODES_CREATED_EV:
		//Node Stuff----> create nodes in app (parses from node list in gui)
		parseNodeData();
		break;
	case Events::SENDERNODE_SELECTED_EV:
		//Some parsing to determine which nodes to show in receiver box----> write on receiver nodes.
		parseReceiverNodes();
		break;
	case Events::RECIEVERNODE_SELECTED_EV:
		//Determine which actions to show on gui based on which nodes were selected in prior steps.
		validateActions();
		break;
	
	//Now communication events begin, might be good idea to check which events is default in gui to prevent errors
	case Events::NO_EV:
		performCom();
		break;
	case Events::FILTER_EV:
		//Third argument is key, for this example it is "0"
		nodes[getIndex()]->postFilter(gui->getReceiverID(), "0");
		gui->ReceivedInfo();
		break;
	case Events::GET_BLOCKS_EV:
		nodes[getIndex()]->getBlocks(gui->getReceiverID(), "84CB2573", 1);
		gui->ReceivedInfo();
		break;
	case Events::GET_HEADERS_EV:
		nodes[getIndex()]->getBlockHeaders(gui->getReceiverID(), "84CB2573", 1);
		gui->ReceivedInfo();
		break;
	case Events::MERKLEBLOCK_EV:
		nodes[getIndex()]->postMerkleBlock(gui->getReceiverID(), "84CB2573", "7B857A14");
		gui->ReceivedInfo();
		break;
	case Events::POST_BLOCK_EV:
		//nodes[getIndex()]->postBlock(gui->getReceiverID(),"84CB2573");
		gui->ReceivedInfo();
		break;
	case Events::TRANSACTION_EV:
		nodes[getIndex()]->transaction(gui->getReceiverID(), gui->getWallet(), gui->getAmount());
		gui->ReceivedInfo();
		break;
	case Events::FAKE_BLOCK_EV:
		gui->ReceivedInfo();
		break;
	case Events::FAKE_TRANS_EV:
		gui->ReceivedInfo();
		break;
	default:
		break;
	}
}


/*Generates event from GUI.*/
const Events App::eventGenerator() 
{ 
	io_context.poll();
	
	return gui->checkForEvent();
}

void App::performCom(void)
{
	for (const auto& node : nodes)
	{
		node->perform();
	}
}


bool App::isRunning(void) 
{
	return running; 
}

void App::updateGuiBlockData()
{
	string bId = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCKID);
	string pbId = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::PREVIOUS_BLOCKID);
	string TxN = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NTX);
	string nonce = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NONCE);
	string bNumber = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCK_NUMBER);
	string root = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::SEE_MROOT);

	BlockShowData data = { bId, pbId, TxN, bNumber, nonce, root };
	gui->setBlockShownData(data);
}


void App::parseNodeData(void)
{
	
	for (const auto& node : gui->getNodes()) 
	{
		/*Creates new node.*/
		if (node.type == NodeTypes::NEW_FULL)
		{
			nodes.push_back(new NodeFull(io_context, node.ip, node.port, node.index, GuiInfo(gui)));
		}
		else if(node.type == NodeTypes::NEW_SVP)
		{
			nodes.push_back(new NodeSPV(io_context, node.ip, node.port, node.index, GuiInfo(gui)));
		}
		else
		{
			nodes.push_back(new NodeMiner(io_context, node.ip, node.port, node.index, GuiInfo(gui)));
		}

		/*Sets neighbors.*/
		for (const auto& neighbor: node.neighbors) 
		{
			auto& ngh = gui->getNode(neighbor);
			//if full public key has to be empty string, if spv then random
			

			nodes.back()->newNeighbor(ngh.index, ngh.ip, ngh.port, nodes.back()->getKey());
		}
	}
}

void App::parseReceiverNodes(void)
{
	gui->clearReceiverNodes();
	unsigned int id = gui->getSenderID();

	for (const auto& neighborId : gui->getNode(id).neighbors)
	{
		//Sorry for this cryptic line hehe
		gui->addReceiverNode(NewNode(gui->getNodes()[neighborId].type, gui->getNode(neighborId).index, gui->getNodes()[neighborId].ip, gui->getNodes()[neighborId].port));
	}
}


void App::validateActions(void)
{
	gui->clearAvailableActions();
	unsigned int senderId = gui->getSenderID();
	unsigned int receiverId = gui->getReceiverID();
	
	//Obtain vectors with actions available (S, R, SR and each string identifier)
	std::vector<Actions> senderActions = nodes[senderId]->getActions();
	std::vector<Actions> receiverActions = nodes[receiverId]->getActions();

	for (const auto& senderAction : senderActions)
	{
		for (const auto& receiverAction : receiverActions)
		{
			if (senderAction.description == receiverAction.description)
			{
				if (((senderAction.type == ActionType::SR) || (senderAction.type == ActionType::S)) & ((receiverAction.type == ActionType::R) || (receiverAction.type == ActionType::SR)))
				{
					//If sender actions and receiver actions coincide then its a valid action
					gui->addAction(senderAction);
				}
			}
		}
	}

}

/*Get message's sender's index*/
const unsigned int App::getIndex() 
{
	const unsigned int& senderID = gui->getSenderID();
	int currentIndex = -1;

	/*Gets sender's index.*/
	for (unsigned int i = 0; i < nodes.size() && currentIndex == -1; i++) 
	{
		if (nodes[i]->getId() == senderID)
		{
			currentIndex = i;
		}
	}
	return currentIndex;
}

/*App destructor.*/
App::~App() 
{
	if (gui)
	{
		delete gui;
	}

	/*Deletes nodes.*/
	for (auto& node : nodes) 
	{
		if (node)
			delete node;
	}
}
