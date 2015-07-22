//
//  Field.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#include "Field.h"
#include "ExSprite3D.h"
#include "FieldDataSource.h"

using namespace std;
using namespace cocos2d;

Field* Field::createById(int id)
{
    const FieldData FieldData = FieldDataSource::findById(id);

    if (FieldData.id == FieldData::NOT_FOUND) {
        return nullptr;
    }

    return Field::createWithFilename(FieldData.filenameTerrain);
}

Field* Field::createWithFilename(const std::string &filename)
{
    Field* field = new Field();

    if (field && field->initWithFile(filename)) {
        field->autorelease();
        return field;
    } else {
        delete field;
        field = nullptr;
        return nullptr;
    }
}