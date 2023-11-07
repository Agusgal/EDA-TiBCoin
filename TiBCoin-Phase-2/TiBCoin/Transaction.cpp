#include "../Source/App/BlockChain/Blocks/Transactions/Transaction.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;

//Constructor and Destructor
Transaction::Transaction()
{

}

Transaction::~Transaction()
{

}

//GETTERS
string Transaction::getTxid() {
	return txid;
}

unsigned int Transaction::getNtxin() {
	return nTxin;
}

unsigned int Transaction::getNtxout() {
	return nTxout;
}

vector<Vin>& Transaction::getVin() {
	return vin;
}

vector<Vout>& Transaction::getVout() {
	return vout;
}

//SETTERS
void Transaction::setTxid(string txid_) {
	txid = txid_;
}

void Transaction::setNtxin(unsigned int nTxin_) {
	nTxin = nTxin_;
}

void Transaction::setNtxout(unsigned int nTxout_) {
	nTxout = nTxout_;
}

void Transaction::setVin(Vin vin_) {
	vin.push_back(vin_);
}

void Transaction::setVout(Vout vout_) {
	vout.push_back(vout_);
}