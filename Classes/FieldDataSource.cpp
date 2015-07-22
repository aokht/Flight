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

    for (auto dat = data.begin(), last = data.end(); dat != last; ++dat) {
        const ValueMap& d = dat->asValueMap();
        ret.push_back({
            d.at("id").asInt(),
            d.at("name").asString(),
            d.at("filename_terrain").asString()
        });
    }

    return ret;
}

FieldData FieldDataSource::findById(int id)
{
    vector<FieldData> data = findAll();
    FieldData ret = { FieldData::NOT_FOUND };

    for (auto d = data.begin(), last = data.end(); d != last; ++d) {
        if (d->id == id) {
            ret = *d;
        }
    }

    return ret;
}