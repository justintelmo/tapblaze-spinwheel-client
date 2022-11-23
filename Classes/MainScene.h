#pragma once
#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
#include "ItemData.h"
#include "network/HttpClient.h"
#include "network/HttpResponse.h"
#include "CCDirector.h"
#include "Buttons/PlayButton.h"
#include "Buttons/ClaimButton.h"

class Wheel;
class ItemData;

#define WINDOWS 1
#define TEST_MODE 1

using namespace cocos2d::ui;
using namespace cocos2d::network;

class MainScene : public cocos2d::Scene
{

public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(MainScene);
	void menuCloseCallback(cocos2d::Ref* pSender);
	void onClickBtnClaim();
	static void getSpinResult(HttpClient* request, HttpResponse* response);
	Wheel* getCurrentWheel();
	cocos2d::Size getWinSize();
	MainScene* getCurrentScene();
	PlayButton* getPlayButton();
	ClaimButton* getClaimButton();
	void onEndWheelSpinAnimation();
private:
	Wheel* _wheel;
	cocos2d::ui::Button* _btnPlay;
	cocos2d::ui::Button* _btnClaim;
	cocos2d::Size _winSize;
	std::vector<ItemData> _dataList;
	int _lastRewardItemIndex;

	void setupButtons();
	void setupWheel();
	void sendItemDataRequest();

	void setLastRewardItemIndex(int lastRewardItemIndex);

#if TEST_MODE
	void testResult();
	void onTestRequestCompleted(HttpClient* request, HttpResponse* response);
	static const char* getItemDescription(int index, int value);
#endif
};

#endif
