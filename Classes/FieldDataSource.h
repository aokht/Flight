//
//  FieldDataSource.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/21.
//
//

#ifndef __Flight__FieldDataSource__
#define __Flight__FieldDataSource__

struct FieldData
{
    int id;
    std::string name;
    std::string filenameTerrain;
    std::string filenameTextureNormal;
    std::string filenameSky;
    std::string filenameSphereLine;

    static const int NOT_FOUND = -1;
};

class FieldDataSource
{
public:
    static const std::string filename;

    static std::vector<FieldData> findAll();
    static FieldData findById(int id);

private:
    FieldDataSource();
};

#endif /* defined(__Flight__FieldDataSource__) */
