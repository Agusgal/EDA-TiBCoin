#pragma once

#include "GetClient.h"
class GetBlockClient : public GetClient 
{
public:

	GetBlockClient(const std::string& ip, const unsigned int ownPort, unsigned int outPort, const std::string id, const unsigned int count)
		: GetClient(ip, ownPort, outPort, id, count) {
		url += '/' + indURL + "?block_id=" + id + "&count=" + std::to_string(count);
	}
	~GetBlockClient() {};
private:
	const std::string indURL = "get_blocks";
};