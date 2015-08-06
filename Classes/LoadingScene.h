//
//  LoadingScene.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/05.
//
//

#ifndef __Flight__LoadingScene__
#define __Flight__LoadingScene__

#include "cocos2d.h"
#include "cocosGUI.h"

class LoadingScene : public cocos2d::Layer
{
public:
    static LoadingScene* create(const std::function<void()>& callback, const std::string& label);
    static cocos2d::Scene* createScene(const std::function<void()>& callback, const std::string& label);

protected:
    cocos2d::ui::Text* textLabel;
    std::string text;
    std::function<void()> callback;

    bool init(const std::function<void()>& callback, const std::string& label);
    void onEnter() override;

    void grabElements();

};

#endif /* defined(__Flight__LoadingScene__) */
