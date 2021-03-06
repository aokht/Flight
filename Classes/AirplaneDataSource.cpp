//
//  AirplaneDataSource.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/21.
//
//

#include "AirplaneDataSource.h"

using namespace std;
using namespace cocos2d;

const string AirplaneDataSource::filename = "airplanes.plist";

vector<AirplaneData> AirplaneDataSource::findAll()
{
    cocos2d::ValueVector data = FileUtils::getInstance()->getValueVectorFromFile(filename);
    vector<AirplaneData> ret;
    ret.reserve(data.size());

    for (auto dat = data.begin(), last = data.end(); dat != last; ++dat) {
        const ValueMap& d = dat->asValueMap();
        ret.push_back({
            d.at("id").asInt(),
            d.at("name").asString(),
            d.at("filename").asString(),
            d.at("speed").asFloat(),
            Vec3(
                 d.at("rotation_max_x").asFloat(),
                 0.f,
                 d.at("rotation_max_z").asFloat()
            ),
            Vec3(
                 d.at("rotation_speed_x").asFloat(),
                 0.f,
                 d.at("rotation_speed_z").asFloat()
            ),
            d.at("thruster_sound_index").asInt()
        });
    }

    return ret;
}


AirplaneData AirplaneDataSource::findById(int id)
{
    vector<AirplaneData> data = findAll();
    AirplaneData ret = { AirplaneData::NOT_FOUND };

    for (auto d = data.begin(), last = data.end(); d != last; ++d) {
        if (d->id == id) {
            ret = *d;
        }
    }

    return ret;
}