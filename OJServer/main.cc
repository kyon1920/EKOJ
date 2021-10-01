#include "OJServer.h"

int main() {
    char isinit;
    std::cout << "是否初始化数据库(Y/N | y/n)？";
    std::cin >> isinit;
    if(isinit == 'y' || isinit == 'Y') {
        EKOJ::OJSql psql;
        psql.init();
    }
    EKOJ::OJServer server;
    server.run();
    return 0;
}