#pragma once

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

enum class ItemType : int;
class ItemData;

class Wheel : public cocos2d::ui::Layout
{
public:
	Wheel();
	~Wheel();
	Wheel(const Wheel& wheel);
	CREATE_FUNC(Wheel);

	void setItemData(const std::vector<ItemData*>& data);

	// onEndSpinAnimation is a callback
	void playSpinAnimation(const std::function<void()>& onEndSpinAnimation);

	void resetPosition();
	void setSumOfWeight();

	std::vector<ItemData*> getItemData();
	std::function<void()> getCallbackEndSpinAnimation();
	void fireCallbackEndSpinAnimation();

	bool getIsSpinning();
	void setIsSpinning(bool isSpinning);
protected:
	virtual bool init();

private:
	cocos2d::Sprite* _wheelSection;
	std::vector<ItemData*> _listItem;
	std::function<void()> _callbackEndSpinAnimation;

	bool _isSpinning;
	int _sumOfWeight;
	float _animationDuration;
	float _speed;
};