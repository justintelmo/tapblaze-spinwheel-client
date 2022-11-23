#pragma once
#include "WheelItem.h"
#include "ItemData.h"
#include "cocos/ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

WheelItem::WheelItem()
{
}

WheelItem::~WheelItem()
{
}

WheelItem* WheelItem::createWith(int id, ItemType type, long value)
{
	WheelItem* instance = new (std::nothrow) WheelItem();
	if (instance && instance->initWith(id, type, value))
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}


const std::string WheelItem::getSpriteFrameNameByItemType(ItemType type)
{
	switch (type)
	{
		case ItemType::BRUSH:
			return "brush.png";
			break;
		case ItemType::COIN:
			return "coin.png";
			break;
		case ItemType::GEM:
			return "gem.png";
			break;
		case ItemType::HAMMER:
			return "hammer.png";
			break;
		case ItemType::HEART:
			return "heart.png";
			break;
		default:
			return "heart.png";
			break;
	}
}

bool WheelItem::initWith(int id, ItemType type, long value)
{
	if (!Layout::init())
	{
		return false;
	}

	// TODO: Refactor into more generic functions
	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->setPosition(Vec2(getContentSize().width * 0.5f, getContentSize().height));

	std::string spriteFileName = getSpriteFrameNameByItemType(type);
	Sprite* sprite = Sprite::createWithSpriteFrameName(spriteFileName);
	this->setName("Wheel_Item_Id_" + std::to_string(id));
	this->addChild(sprite);
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	sprite->setRotation(180.0f);
	sprite->setPosition(Vec2(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.5f - 175.0f));

	std::ostringstream stm;
	stm << value;

	Label* lblValue = Label::createWithTTF("Test", "fonts/arial.ttf", 30);
	sprite->addChild(lblValue);
	lblValue->setTextColor(Color4B::WHITE);
	lblValue->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	lblValue->setAlignment(TextHAlignment::CENTER);
	lblValue->setString("x" + stm.str());
	lblValue->setPosition(Vec2(this->getContentSize().width * 0.5f + (sprite->getContentSize().width * 0.5f), this->getPositionY() - sprite->getContentSize().height * 0.15f));
	lblValue->enableOutline(Color4B(123, 66, 25, 255), 1);
	lblValue->enableShadow(Color4B(123, 66, 26, 255));

	if (type == ItemType::HEART)
	{
		lblValue->setString(stm.str());

		Label* lblMin = Label::createWithTTF("Test", "fonts/arial.ttf", 30);
		lblValue->addChild(lblMin);
		lblMin->setTextColor(Color4B::WHITE);
		lblMin->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		lblMin->setAlignment(TextHAlignment::CENTER);
		lblMin->setString("min");

		lblMin->setPosition(Vec2(lblValue->getPositionX() + lblValue->getContentSize().width * 0.5f - 50.0f, lblValue->getPositionY() + 5.0f));
		lblMin->enableOutline(Color4B(123, 66, 25, 255), 1);
		lblMin->enableShadow(Color4B(123, 66, 25, 255));
	}

	return true;
}