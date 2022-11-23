#include "ItemData.h"
#include "Wheel.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include <iostream>
#include "spine/Json.h"
#include "CCDirector.h"
#include "json/rapidjson.h"
#include "external/json/document.h"

using namespace cocos2d::network;
using namespace cocos2d;
typedef std::vector<std::tuple<ItemType, long, int>> data_list;


ItemData::ItemData()
{
}

ItemData::~ItemData()
{
}

// Takes response from API and populates wheel items with it
void ItemData::getItemDataFromServer(HttpClient *sender, HttpResponse *response)
{
	std::vector<ItemData*> dataList;
	if (!response)
	{
		return;
	}

	std::vector<char>* buffer = response->getResponseData();
	std::string responseBody(buffer->begin(), buffer->end());
	rapidjson::Document jsonDoc;
	jsonDoc.Parse(responseBody.c_str());

	
	for (rapidjson::Value::ConstValueIterator itr = jsonDoc.Begin(); itr != jsonDoc.End(); ++itr)
	{
		ItemData* itemData = new ItemData;
		auto wheelItemData = itr->GetObject();
		itemData->id = wheelItemData["id"].GetInt();
		itemData->type = (ItemType) wheelItemData["item_type"].GetInt();
		itemData->value = wheelItemData["value"].GetInt();
		itemData->weight = wheelItemData["weight"].GetInt();
		
		dataList.push_back(itemData);
	}


	auto director = Director::getInstance();
	Scene* scene = director->getRunningScene();
	Wheel* wheel = scene->getChildByName<Wheel*>("wheel");
	wheel->setItemData(dataList);
	wheel->setSumOfWeight();
}

// Left in for testing purposes in case server goes down
std::vector<ItemData> ItemData::createFakeData()
{
	data_list fakeData;

	fakeData.emplace_back(std::make_tuple(ItemType::HEART, 30L, 20)); // 5
	fakeData.emplace_back(std::make_tuple(ItemType::BRUSH, 3L, 10)); // 1
	fakeData.emplace_back(std::make_tuple(ItemType::GEM, 35L, 10)); // 3
	fakeData.emplace_back(std::make_tuple(ItemType::HAMMER, 3L, 10)); // 4
	fakeData.emplace_back(std::make_tuple(ItemType::COIN, 750L, 5)); // 2
	fakeData.emplace_back(std::make_tuple(ItemType::BRUSH, 1L, 20)); // 1
	fakeData.emplace_back(std::make_tuple(ItemType::GEM, 75L, 5)); // 3
	fakeData.emplace_back(std::make_tuple(ItemType::HAMMER, 1L, 20)); // 4

	ItemData itemData;
	std::vector<ItemData> dataList;
	for (std::tuple<ItemType, long, int> data : fakeData)
	{
		itemData.type = std::get<0>(data);
		itemData.value = std::get<1>(data);
		itemData.weight = std::get<2>(data);
		dataList.push_back(itemData);
	}

	return dataList;
}