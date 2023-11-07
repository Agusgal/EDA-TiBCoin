#pragma once
#include <string>
#include <allegro5/allegro.h>
#include <vector>
#include "../Actions/Actions.h"

/*Gui event codes.*/
const enum class Events:int 
{
	NO_EV = 0,
	END_EV,
	VALIDATE_MROOT_EV,
	ALL_MERKLE_EV,
	NEW_FILE_EV,
	PRINT_TREE_EV,
	CALC_MROOT_EV,
	FIRST_UPDATE_EV,
	NODES_CREATED_EV,
	SENDERNODE_SELECTED_EV,
	RECIEVERNODE_SELECTED_EV,
	FILTER_EV,
	MERKLEBLOCK_EV,
	GET_BLOCKS_EV,
	GET_HEADERS_EV,
	TRANSACTION_EV,
	POST_BLOCK_EV
};


const enum class States
{
	INIT = 0,
	WAITING,
	FILE_OK,
	BLOCK_OK,
	INIT_DONE
};


struct BlockShowData
{
	std::string blockId;
	std::string previousBlockId;
	std::string TxN;
	std::string blockNumber;
	std::string nonce;
	std::string mkRoot;
};

const enum class Mode
{
	NONE = 0,
	ONE,
	TWO
};

/*Some Node types*/
const enum class NodeTypes {
	NEW_SVP,
	NEW_FULL,
};


/*Node struct that holds new node info that then gets parsed to App when we get to the action screen.*/
struct NewNode 
{
	/*Constructor.*/
	NewNode(const NodeTypes type, const unsigned int index, std::string ip, int port) : type(type), index(index), ip(ip), port(port)
	{ 
	}

	/*Data*/
	NodeTypes type;
	unsigned int index;
	std::string ip;
	int port;
	std::vector<unsigned int> neighbors; //has indexes of neighbor nodes
};

class Gui 
{
public:

	Gui();
	~Gui();

	const Events checkForEvent(void);

	const std::vector<NewNode>& getNodes();
	const NewNode& getNode(unsigned int index);

	const unsigned int getBlockIndex() const;

	const std::string& getFilename(void);

	void setResultMsg(const std::string&);
	void setBlockShownData(BlockShowData);

	void setChainLength(unsigned int);

	void updateComMsg(const std::string&);
	const unsigned int& getSenderID();
	const unsigned int& getReceiverID();
	void ReceivedInfo();

	const int getAmount();
	const std::string& getWallet();

	void addReceiverNode(NewNode node);
	void clearReceiverNodes(void);
	void addAction(Actions action);
	void clearAvailableActions(void);

private:
	
	/*Initial setup.*/
	
	void initAllegro();
	void initialImGuiSetup(void) const;
	

	/*Gui Mode*/

	Mode mode;
	void modeSelector(void);
	void phaseOneMode(Events &out);
	void phaseTwoMode(Events &out);


	/*Window displayers.*/
	void prepareNewWindow() const;
	void fileDialog();
	bool showFile();

	inline void showBlockchainMenu();
	void showBlocks();
	void showBlockInfo();


	/*This methods are wrappers to display different imgui widgets*/
	template <class Widget, class F1, class F2 = void(*)(void)>
	inline auto displayWidget(const Widget&, const F1& f1, const F2 & = []() {}) -> decltype(f1());


	template <class F1, class F2 = void(*)(void)>
	inline auto displayWidget(const char*, const F1& f1, const F2 & = []() {})->decltype(f1());

	inline void renderScreen() const;
	inline void setAllFalse(const States&);


	void clearNetworkingInfo(void);

	/*Exit and resize events.*/
	bool windowEvents(void);


	/*Allegro data members.*/
	ALLEGRO_DISPLAY* guiDisplay;
	ALLEGRO_EVENT_QUEUE* guiQueue;
	ALLEGRO_EVENT guiEvent;

	/*Flag data members.*/
	unsigned int chainLength;
	std::string actionMsg, resultMsg;
	Events event;
	States state;

	/*Data members modifiable by user.*/
	std::string path, selected;
	std::string filePath;
	std::string filename;
	std::string fileNamePath;
	unsigned int index;
	
	/*Phase Two*/
	std::vector<std::string> nodeIds;

	std::string networkInfoMsg;

	void createNewNode(void);
	void validateNeighbors(bool &openPopup);
	void nodeInitialization(Events& out);
	void nodeActions(Events& out);


	std::string ip;
	int port;
	int nodeType;

	std::vector<bool> nodeSelection;
	std::vector <NewNode> nodes;
	std::vector <NewNode> receiverNodes;
	std::vector <Actions> availableActions;//cambiar por action u otra cosa

	unsigned int selectedSenderId;
	unsigned int selectedActionId;

	unsigned int selectedReceiverId;

	bool showTranferMenu;

	int coinN;
	std::string publicKey;
	std::string wallet;


	void resetNodeSelection(void);

	/*Misc.*/
	BlockShowData blockData;
	bool firstUpdate;
	void popup(const char* msg);
	std::string popupmsg;
};