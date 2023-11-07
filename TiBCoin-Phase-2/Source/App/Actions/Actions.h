#pragma once

#include <string>

const enum class ActionType
{
	S,
	R,
	SR
};


struct Actions
{
	Actions(ActionType type, std::string description) : type(type), description(description)
	{
	}

	ActionType type;
	std::string description;
};