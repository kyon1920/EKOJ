#pragma once

#include <iostream>
#include <errno.h>
#include <string>
#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

/*
 * @file OJSql.h
 * @author Saint-EK
 * @brief OJSql Online Judge 数据库操作类
 * @date 2021-3-20
 */

namespace EKOJ {

const std::string ip = "127.0.0.1";
const std::string username = "root";
const std::string passwd = "Saint97.";
const std::string database = "EKOJ";

class OJSql {
public:
    OJSql();
    ~OJSql();
    void init();
    bool exec(std::string psql);
    std::string rootexec(std::string psql);
    std::string rootstore(std::string psql);
    mysqlpp::StoreQueryResult store(std::string psql);

private:
    static mysqlpp::Connection m_connection;
};

}