//
//  AirplaneDataSource.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/21.
//
//

#ifndef __Flight__AirplaneDataSource__
#define __Flight__AirplaneDataSource__

#include "cocos2d.h"

struct AirplaneData
{
    int id;
    std::string name;
    std::string filename;
    float speed;
    cocos2d::Vec3 rotationMax;
    cocos2d::Vec3 rotationSpeed;
    int thrusterSoundIndex;

    static const int NOT_FOUND = -1;
};

class AirplaneDataSource
{
public:
    static const std::string filename;

    static std::vector<AirplaneData> findAll();
    static AirplaneData findById(int id);

private:
    AirplaneDataSource();
};

#endif /* defined(__Flight__AirplaneDataSource__) */
