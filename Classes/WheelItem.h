#pragma once

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

enum class ItemType : int;

class WheelItem : public cocos2d::ui::Layout
{
public:
	WheelItem();
	~WheelItem();

	static WheelItem* createWith(int id, ItemType type, long value);
	static const std::string getSpriteFrameNameByItemType(ItemType type);

protected:
	bool initWith(int id, ItemType type, long value);
};