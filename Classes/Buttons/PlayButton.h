//
// Created by psyba on 11/23/2022.
//

#ifndef PROJ_ANDROID_PLAYBUTTON_H
#define PROJ_ANDROID_PLAYBUTTON_H

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
#include "ItemData.h"
#include "network/HttpClient.h"
#include "network/HttpResponse.h"
#include "CCDirector.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class PlayButton : public Button {
public:
    ~PlayButton();
    static PlayButton* create(const std::string& imageName, const std::string& buttonText);
private:
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    void onTouchCancelled(Touch* touch, Event* event);
    std::string _imageName;
    std::string _buttonText;
    Text* _textField;
    void setConfiguration();
    void setupEventListener();
    void onClickBtn();
};


#endif //PROJ_ANDROID_PLAYBUTTON_H
