#pragma once
#include<iostream>
#include<vector>
#include <string>

using namespace std;

struct Vin {
	string blockid;
	string intxid;
	string signature;
	unsigned int outputIndex;
};

struct Vout {
	string publicid;
	unsigned int amount;
};

class Transaction {
public:
	Transaction();
	~Transaction();
	string getTxid();
	unsigned int getNtxin();
	unsigned int getNtxout();
	vector<Vin>& getVin();
	vector<Vout>& getVout();

	void setTxid(string txid_);
	void setNtxin(unsigned int nTxin_);
	void setNtxout(unsigned int nTxout_);
	void setVin(Vin vin_);
	void setVout(Vout vout_);
	
private:

	string txid;
	unsigned int nTxin;
	unsigned int nTxout;
	vector<Vin> vin;
	vector<Vout> vout;
	friend class Merkle;
};

