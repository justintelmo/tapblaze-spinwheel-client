#pragma once

#include "RewardItem.h"
#include "ItemData.h"
#include "cocos/ui/CocosGUI.h"
#include "WheelItem.h"
#include <string>
USING_NS_CC;

using namespace cocos2d::ui;

RewardItem::RewardItem()
{
}

RewardItem::~RewardItem()
{
}

RewardItem* RewardItem::createWith(ItemType type, long value)
{
	RewardItem* instance = new (std::nothrow) RewardItem();
	if (instance && instance->initWith(type, value))
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

bool RewardItem::initWith(ItemType type, long value)
{
	if (!Layout::init())
	{
		return false;
	}

	this->setContentSize(Size(200, 500));

	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	Size winSize = Director::getInstance()->getWinSize();
	this->setPosition(Vec2(winSize.width * 0.5f, winSize.height * 0.5f));

	std::string spriteName = WheelItem::getSpriteFrameNameByItemType(type);
	Sprite* sprite = Sprite::createWithSpriteFrameName(spriteName);
	this->addChild(sprite);

	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	sprite->setPosition(Vec2(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.5f));
	sprite->setScale(1.4f);

	std::ostringstream stm;
	stm << value;


	Label* lblValue = Label::createWithTTF("Test", "fonts/arial.ttf", 35);
	this->addChild(lblValue);

	lblValue->setTextColor(Color4B::WHITE);
	lblValue->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	lblValue->setAlignment(TextHAlignment::CENTER);
	lblValue->setString("x" + stm.str());
	lblValue->setScale(0.7f);
	lblValue->setPosition(Vec2(this->getContentSize().width * 0.5f, sprite->getPositionY() - sprite->getContentSize().height * 0.5f));
	lblValue->enableOutline(Color4B(123, 66, 25, 255), 3);
	lblValue->enableShadow(Color4B(123, 66, 25, 255));

	if (type == ItemType::HEART)
	{
		lblValue->setString(stm.str() + " min");
	}

	Sprite* goldStar = Sprite::createWithSpriteFrameName("hp_gold_star.svg");
	this->addChild(goldStar, -1);
	goldStar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	goldStar->setPosition(sprite->getPosition());
	goldStar->setScale(1.5f);
	

	RotateBy* rotateBy = RotateBy::create(0.2f, 30);
	
	goldStar->runAction(RepeatForever::create(rotateBy));
	goldStar->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);

	return true;
}