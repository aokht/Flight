//
//  Sphere.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/11.
//
//

#ifndef __Flight__Sphere__
#define __Flight__Sphere__

#include "cocos2d.h"
#include "Sprite3DBatchNode.h"

class Sphere : public Sprite3DBatchNode
{
public:
    enum Type {
        NONE = 0, // DO NOT USE
        BLUE = 1,
        YELLOW = 2,
        RED = 3,
        LAST      // DO NOT USE
    };

    static Sphere* createWithType(Type type);
    static Sphere* createOne(Type type);
    static Sphere* create(const Sphere& src);
    Sphere(Type type);
    ~Sphere();

    Type getType() const
    {
        return type;
    }

    struct SphereGroupInfo {
        Type type;
        cocos2d::Vec3 position;
    };
    static std::vector<std::vector<SphereGroupInfo>> getSphereGroupInfoList(const std::string& path);

protected:
    const Type type;

    bool init();
    bool initOne();
    void setupShaders();

};

#endif /* defined(__Flight__Sphere__) */
