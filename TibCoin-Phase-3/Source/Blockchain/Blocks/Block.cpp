#include "Block.h"
#include <string>

#include <cryptopp/sha.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/channels.h>

#include <iostream>

using namespace std;

//Constructor and Destructor

BlockMicho::~BlockMicho()
{

};

BlockMicho::BlockMicho(const json& block)
{
	this->jsonData = block;
	this->header = this->jsonData;
	this->tx = this->header["tx"];
	this->header.erase("tx");

	header["height"] = to_string(header["height"].get<unsigned int>());
	header["nonce"] = to_string(header["nonce"].get<unsigned int>());
	header["nTx"] = to_string(header["nTx"].get<unsigned int>());

	this->validated = false;

	auto transactions = block["tx"];
}


/*Transforms int into hex Coded ASCII.*/
inline const string BlockMicho::hex2ASCII(unsigned int n)
{
	char res[9];
	sprintf_s(res, "%08X", n);

	return res;
}

/*Hashes*/
inline const string BlockMicho::hash(const string& code)
{
	return generateID(code);
	//return hex2ASCII(generateID(code));
}

/*PArses from json to ids and nodes*/
void BlockMicho::parseIds(void)
{
	this->ids.clear();

	unsigned int tempID;

	if (jsonData.is_null())
	{
		return;
	}

	string sstring = jsonData.dump();

	string tx_id;
	bool Add;
	
	/*Loops over every transaction*/
	for (const auto& Tx : jsonData["tx"]) 
	{
		/*Loops through every element in jsonData['vin'].*/
		for (const auto& elementVin : Tx["vin"])
		{
			/*Gets string from JSON.*/
			tx_id.append(elementVin["txid"].get<string>());
		}

		/*Hashes id and appends it to IDs.*/
		ids.push_back(hash(tx_id));

		tx_id.clear();
	}
}

void BlockMicho::buildTree()
{
	/*Parses from json to ids in Block*/
	parseIds();
	
	/*Copies IDs vector to nodes.*/
	nodes.assign(ids.begin(), ids.end());

	std::list<std::string>::iterator itr;

	/*iterates until its on mroot*/
	while (nodes.size() > 1) 
	{
		/*Prevents unevent amount*/
		if (nodes.size() % 2)
		{
			nodes.push_back(nodes.back());
		}

		tree.insert(tree.end(), nodes.begin(), nodes.end());

		for (auto i = nodes.begin(); i != nodes.end(); i++) 
		{
			*i = hash(*i + *std::next(i));

			nodes.erase(std::next(i));
		}
	}
	if (nodes.size()) 
	{
		tree.push_back(nodes.back());
		this->calculatedMerkleRoot = nodes.back();
		if (nodes.back() == jsonData["merkleroot"])
		{
			this->isValidMR = "True";
		}
		else
		{
			this->isValidMR = "False";
		}
	}
	this->validated = true;
}


/*Prints tree as a string.*/
std::string BlockMicho::printTree(void) 
{
	if (!tree.size())
	{
		buildTree();
	}

	int levels = log2(tree.size() + 1);

	std::string out;

	/*Character between words.*/
	const char* spacing = " ";
	int words_in_row, init, middle, abs_pos = 0;
	const int length = tree[0].length();

	const int row = (pow(2, levels) - 1) * length;

	/*Loops from higher to lower level.*/
	for (int i = levels; i > 0; i--) 
	{
		words_in_row = pow(2, i - 1);

		init = (pow(2, levels - i) - 1) * length;

		/*Sets number of characters between words.*/
		if (words_in_row - 1)
		{
			middle = (row - 2 * init - words_in_row * length) / (words_in_row - 1);
		}
		else
		{
			middle = 0;
		}

		for (int j = 0; j < init; j++)
		{
			out.append(spacing);
		}

		for (int j = abs_pos; j < abs_pos + words_in_row; j++) 
		{
			out.append(tree[j]);
			for (int k = 0; k < middle; k++)
			{
				out.append(spacing);
			}
		}

		abs_pos += words_in_row;

		/*Goes down one level.*/
		out.append("\n\n");
	}

	return fliptree(out);
}


/*Generates ID with sha256*/
std::string BlockMicho::generateID(const std::string &code)
{
	std::string s1;

	CryptoPP::SHA256 sha256;

	CryptoPP::HashFilter f1(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(s1)));

	CryptoPP::ChannelSwitch cs;
	cs.AddDefaultRoute(f1);

	CryptoPP::StringSource ss(code, true /*pumpAll*/, new CryptoPP::Redirector(cs));

	//std::cout << "Message: " << code << std::endl;
	//std::cout << "SHA-256: " << s1 << std::endl;
	
	return s1;
}


const string BlockMicho::getData(const BlockInfo& data) 
{
	switch (data) 
	{
	case BlockInfo::BLOCKID:
		return  header["blockid"];
	case BlockInfo::BLOCK_NUMBER:
		return header["height"];
	case BlockInfo::CALCULATE_MROOT:
		if (!this->validated)
		{
			buildTree();
		}
		return this->calculatedMerkleRoot;
	case BlockInfo::SEE_MROOT:
		return header["merkleroot"];
	case BlockInfo::VALIDATE_MROOT:
		if (!this->validated)
		{
			buildTree();
		}
		return this->isValidMR;
	case BlockInfo::NTX:
		return header["nTx"];
	case BlockInfo::NONCE:
		return header["nonce"];
	case BlockInfo::PREVIOUS_BLOCKID:
		return header["previousblockid"];
	}
}

string BlockMicho::fliptree(string str) 
{
	string resul = "";
	string salto = "\n\n";
	int lim = str.size();
	int nivel = 2;
	int j = 0;

	for (int i = lim; nivel > 0; i--)
	{
		if (str[i] == '\n') {
			j = 1 + i;
			while ((str[j] != '\n') && (j < lim)) {
				resul += str[j];
				j++;
			}
			resul += salto;
			//nivel--;
			i--;
		}
		if (i == 0) {
			j = i;
			while ((str[j] != '\n') && (j < lim)) {
				resul += str[j];
				j++;
			}
			nivel = 0;
		}
	}
	return resul;
}