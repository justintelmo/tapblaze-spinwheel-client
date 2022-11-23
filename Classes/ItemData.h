#pragma once

#include <tuple>
#include <string>
#include <vector>
#include "network/HttpClient.h"
#include "network/HttpResponse.h"

enum class ItemType : int
{
	NONE = 0,
	BRUSH,
	COIN,
	GEM,
	HAMMER,
	HEART,
	END
};

class ItemData
{
	public:
		ItemData();
		~ItemData();
		int id;
		ItemType type;
		int value;
		int weight;

		static void getItemDataFromServer(cocos2d::network::HttpClient* sender, cocos2d::network::HttpResponse* response);
		std::vector<ItemData> createFakeData();
};