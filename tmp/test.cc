#include <iostream>
#include <vector>
#include <json/json.h>

using namespace std;

inline std::vector<std::string> split(std::string src, std::string delim) {
    std::vector<std::string> res;
    int loc;
    while ((loc = src.find(delim)) != std::string::npos) {
        if (loc != 0) {
            res.push_back(src.substr(0, loc));
        }
        src = src.substr(loc + delim.size());
    }
    if (src.size() > 0) {
        res.push_back(src);
    }
    return res;
}

int main() {
    std::string recvinfo = "LOGIN*:*:";
    std::vector<std::string> res = split(recvinfo, "*:");
    std::cout << res.size() << std::endl;
    for(int i = 0; i < res.size(); ++i) {
        std::cout << res[i] << std::endl;
    }

    string jsonStr = "{\"userName\":\"123456\",\"userPassword\":\"111111\"}";
    Json::Reader reader;
    Json::Value value;
    reader.parse(jsonStr,value);
    Json::Value::Members member = value.getMemberNames();
    for(Json::Value::Members::iterator iter = member.begin();iter != member.end(); ++iter) {
        cout<<(*iter)<<endl;    //获得key
        cout<<value[*iter].asString()<<endl;    //获得value
    }

    return 0;
}