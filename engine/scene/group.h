#ifndef GROUP_H
#define GROUP_H

#include <vector> 
#include <string>
#include "transform.h"

using namespace std;
    struct Group {
        vector<Transform> transforms;
        vector<string> modelFiles;
        vector<Group> children;
    };

#endif