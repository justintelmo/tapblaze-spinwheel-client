#include "Wheel.h"
#include "WheelItem.h"
#include "ItemData.h"
#include "network/HttpClient.h"

USING_NS_CC;
using namespace cocos2d::ui;


const float WHEEL_SECTION_ROTATION_OFFSET = -22.5f;
const float ROTATION_PER_ITEM_SLOTS = 45.0f;
const int WHEEL_SPIN_ANIMATION_TAG = 3;

Wheel::Wheel()
{
	_wheelSection = nullptr;
	_animationDuration = 0.0f;
}

Wheel::Wheel(const Wheel& wheel)
{
	_wheelSection = wheel._wheelSection;
	_animationDuration = wheel._animationDuration;
}

Wheel::~Wheel()
{
}

bool Wheel::init()
{
	if (!Layout::init())
	{
		return false;
	}

	_wheelSection = Sprite::createWithSpriteFrameName("wheel_sections_8.png");
	this->addChild(_wheelSection);
	_wheelSection->setName("wheel_section");
	_wheelSection->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	Sprite* wheelBorder = Sprite::createWithSpriteFrameName("wheel_border.png");
	this->addChild(wheelBorder);

	Sprite* wheelArrow = Sprite::createWithSpriteFrameName("wheel_arrow.png");
	this->addChild(wheelArrow);
	wheelArrow->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	wheelArrow->setPosition(Vec2(0.0f, wheelBorder->getContentSize().height * 0.5f - 20.0f));

	return true;
}

void Wheel::setItemData(const std::vector<ItemData*>& data)
{
	_listItem = data;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		ItemData* itemData = data.at(i);
		WheelItem* item = WheelItem::createWith(itemData->id, itemData->type, itemData->value);
		if (item) {
			item->setName("Wheel_Item_" + std::to_string(i));
			_wheelSection->addChild(item);
			item->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
			item->setPosition(Vec2(_wheelSection->getContentSize().width * 0.5f, _wheelSection->getContentSize().height * 0.5f));
			item->setRotation(-WHEEL_SECTION_ROTATION_OFFSET + ROTATION_PER_ITEM_SLOTS * i);
		}
	}
}

std::vector<ItemData*> Wheel::getItemData()
{
	return _listItem;
}

std::function<void()> Wheel::getCallbackEndSpinAnimation()
{
	if (this->_callbackEndSpinAnimation) {
		return this->_callbackEndSpinAnimation;
	}

	return nullptr;
}

void Wheel::fireCallbackEndSpinAnimation()
{
	if (this->_callbackEndSpinAnimation) {
		this->_callbackEndSpinAnimation();
	}
}

bool Wheel::getIsSpinning()
{
	return _isSpinning;
}

void Wheel::setIsSpinning(bool isSpinning)
{
	_isSpinning = isSpinning;
}

void Wheel::setSumOfWeight()
{
	if (_listItem.empty()) {
		return;
	}

	_sumOfWeight = 0;
	for (auto & item : _listItem)
	{
		_sumOfWeight += item->weight;
	}
}

void Wheel::playSpinAnimation(const std::function<void()>& onEndSpinAnimation)
{
	_isSpinning = true;
	_callbackEndSpinAnimation = onEndSpinAnimation;
	RotateBy* spinForward = RotateBy::create(RandomHelper::random_real(3.0f, 3.5f), 360.0f * 3.0f);
	RepeatForever* repeatForever = RepeatForever::create(spinForward);
	repeatForever->setTag(WHEEL_SPIN_ANIMATION_TAG);
	_wheelSection->runAction(repeatForever);
}


void Wheel::resetPosition()
{
	_wheelSection->setRotation(0);
}