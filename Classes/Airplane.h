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
#include "AirplaneDataSource.h"
#include "ExSprite3D.h"

class Airplane : public cocos2d::Node
{
public:
    // Life Cycle
    static Airplane* createById(int i);
    static Airplane* createByData(const AirplaneData& airplaneData);
    static void createByIdAsync(int i, const std::function<void(Airplane*, void*)>& callback, void* callbackparam);
    static void createByDataAsync(const AirplaneData& airplaneData, const std::function<void(Airplane*, void*)>& callback, void* callbackparam);
    void setCameraToAirplane(cocos2d::Camera* camera);
    void setLightDirection(const cocos2d::Vec3& lightDirection);

    // 姿勢制御
    void step(float dt);
    void rotate(float dt);
    void setRotationToDefault(float dt);
    cocos2d::Vec3 getRotation3D() const override;
    cocos2d::Vec3 getDirection() const;

    // 入力応答
    void onInputBegan();
    void onInputMoved(const cocos2d::Vec2& diff);
    void onInputEnded(const cocos2d::Vec2& diff);

    // パラメータ(TODO: そろそろなんとかしたほうがいい
    int getAirplaneId() const;
    const std::string& getAirplaneName() const;
    float getSpeed() const;
    const cocos2d::Vec3& getRotationSpeed() const;
    const cocos2d::Vec3& getRotationMax() const;
    int getThrusterSoundIndex() const;

    // デバッグ用
    cocos2d::Vec3 getSpriteRotation() const;
    cocos2d::Vec3 getRotationTarget() const;

protected:
    static Airplane* createWithFilename(const std::string& filename);
    bool initWithFilename(const std::string& filename);
    static void createWithFilenameAsync(const std::string& filename, const std::function<void(Airplane*, void*)>& callback, void* callbackparam);
    void initWithFilenameAsync(const std::string& filename, const std::function<void(Airplane*, void*)>& callback, void* callbackparam);

    class AirplaneSprite : public ExSprite3D
    {
    public:
        static AirplaneSprite* create(const std::string& filename);
        static void createAsync(const std::string& filename, const std::function<void(AirplaneSprite*, void*)>& callback, void* callbackparam);

        void setLightDirection(const cocos2d::Vec3& lightDirection);

    protected:
        void setupShader();
    };
    AirplaneSprite* spriteAirplane;

    // 機体の目標回転量
    cocos2d::Vec3 rotationTarget;
    // 入力開始時の目標回転量
    cocos2d::Vec3 rotationStart;

    // パラメータ
    int airplaneId;
    std::string airplaneName;
    float speed;
    cocos2d::Vec3 rotationMax;
    cocos2d::Vec3 rotationSpeed;
    int thrusterSoundIndex;

};

#endif /* defined(__Flight__Airplane__) */
