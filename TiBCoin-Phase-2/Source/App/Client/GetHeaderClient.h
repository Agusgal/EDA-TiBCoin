#pragma once

#include "GetClient.h"
class GetHeaderClient : public GetClient {
public:

	GetHeaderClient(const std::string& ip, const unsigned int ownPort, const unsigned int outPort, const std::string id, const unsigned int count)
		: GetClient(ip, ownPort, outPort, id, count) {
		url += '/' + indURL + "?block_id=" + id + "&count=" + std::to_string(count);
	}
	~GetHeaderClient() {};
private:
	const std::string indURL = "get_block_header";
};