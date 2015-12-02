//
//  FieldDataSource.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/21.
//
//

#include "FieldDataSource.h"

using namespace std;
using namespace cocos2d;

const string FieldDataSource::filename = "fields.plist";

vector<FieldData> FieldDataSource::findAll()
{
    cocos2d::ValueVector data = FileUtils::getInstance()->getValueVectorFromFile(filename);
    vector<FieldData> ret;
    ret.reserve(data.size());

    for (auto dat : data) {
        const ValueMap& d = dat.asValueMap();
        ret.push_back({
            d.at("id").asInt(),
            d.at("name").asString(),
            d.at("filename_terrain").asString(),
            d.at("filename_texture_normal").asString(),
            d.at("filename_sky").asString(),
            d.at("filename_sphere_line").asString(),
            Vec3(
                d.at("airplane_position_x").asFloat(),
                d.at("airplane_position_y").asFloat(),
                d.at("airplane_position_z").asFloat()
            ),
            Vec3(
                d.at("airplane_rotation_x").asFloat(),
                d.at("airplane_rotation_y").asFloat(),
                d.at("airplane_rotation_z").asFloat()
            ),
            Vec3(
                d.at("other_airplane_position_x").asFloat(),
                d.at("other_airplane_position_y").asFloat(),
                d.at("other_airplane_position_z").asFloat()
            ),
            Vec3(
                d.at("other_airplane_rotation_x").asFloat(),
                d.at("other_airplane_rotation_y").asFloat(),
                d.at("other_airplane_rotation_z").asFloat()
            ),
            d.at("leaderboard_highscore_key").asString(),
            d.at("bgm_index").asInt(),
            Vec3(
                 d.at("light_direction_x").asFloat(),
                 d.at("light_direction_y").asFloat(),
                 d.at("light_direction_z").asFloat()
            )
        });
    }

    return ret;
}

FieldData FieldDataSource::findById(int id)
{
    vector<FieldData> data = findAll();
    FieldData ret = { FieldData::NOT_FOUND };

    for (auto d : data) {
        if (d.id == id) {
            ret = d;
        }
    }

    return ret;
}