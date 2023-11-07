#pragma once
#include "PostClient.h"

class MerkleClient : public PostClient 
{
public:
	MerkleClient(const std::string& ip, const unsigned int ownPort, const unsigned int outPort,
		const json& data) : PostClient(ip, ownPort, outPort, data) {
		url += '/' + indURL;
	}
private:
	const std::string indURL = "send_merkle_block";
};