#pragma once

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

enum class ItemType : int;

class RewardItem : public cocos2d::ui::Layout
{
public: 
	RewardItem();
	~RewardItem();

	static RewardItem* createWith(ItemType type, long value);

protected:
	bool initWith(ItemType type, long value);
};