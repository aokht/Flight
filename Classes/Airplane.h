//
//  Airplane.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#ifndef __Flight__Airplane__
#define __Flight__Airplane__

#include "cocos2d.h"

class Airplane : public cocos2d::Node
{
public:
    CREATE_FUNC(Airplane);
    void setCameraToAirplane(cocos2d::Camera* camera);

    // 姿勢制御
    void step(float dt);
    void goForward(float dt);
    void rotate(float dt);
    void setRotationToDefault(float dt);
    cocos2d::Vec3 getRotation3D() const override;

    // 入力応答
    void onInputBegan();
    void onInputMoved(const cocos2d::Vec2& diff);
    void onInputEnded(const cocos2d::Vec2& diff);

    // デバッグ用
    cocos2d::Vec3 getSpriteRotation() const;
    cocos2d::Vec3 getRotationTarget() const;

protected:
    bool init();
    void onEnter() override;

    cocos2d::Sprite3D* spriteAirplane;

    // 機体の回転量
    cocos2d::Vec3 rotationStep;

    // 姿勢制御時の始点
    // 機体の回転量
    cocos2d::Vec3 rotationStart;
    // 飛行機スプライトの回転量
    cocos2d::Vec3 spriteRotationStart;
};

#endif /* defined(__Flight__Airplane__) */
