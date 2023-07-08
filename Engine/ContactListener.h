#pragma once

#include <sstream>
#include <unordered_map>
#include <box2d/box2d.h>
#include "Box.h"

class ContactListener : public b2ContactListener
{
public:
	enum Pattern
	{
		SameColor,
		Count
	};

public:
	void BeginContact(b2Contact* contact) override;

public:
	std::unordered_map<b2Contact*, Pattern> contactPatternMap;
};