#include "OJSql.h"

namespace EKOJ {

const static std::string usersdata {
    "CREATE TABLE `UsersData` (\n"
    "`username` varchar(128) PRIMARY KEY,\n"
    "`passwd` varchar(128) NOT NULL,\n"
    "`usertype` tinyint NOT NULL,\n"
    "`name` varchar(128) NOT NULL,\n"
    "`material` varchar(128) NOT NULL,\n"
    "`integral` tinyint DEFAULT 0,\n"
    "`classid` varchar(64)\n"
    ");"
};

const static std::string list4exercise {
    "CREATE TABLE `ExerciseList` (\n"
    "`id` int PRIMARY KEY AUTO_INCREMENT,\n"
    "`name` varchar(1024) NOT NULL,\n"
    "`description` text NOT NULL,\n"
    "`level` tinyint NOT NULL,\n"
    "`attribute` varchar(1024) NOT NULL,\n"
    "`explain` text NOT NULL,\n"
    "`examples1` text NOT NULL,\n"
    "`examples2` text NOT NULL,\n"
    "`pass` int,\n"
    "`submission` int,\n"
    "`timelimit` int NOT NULL,\n"
    "`memorylimit` int NOT NULL,\n"
    "`inumber` tinyint DEFAULT 0,\n"
    "`filename` varchar(1024) NOT NULL\n"
    ");"
};

const static std::string classtable {
    "CREATE TABLE `ClassTable` (\n"
    "`tusername` varchar(128),\n"
    "`id` varchar(64) NOT NULL,\n"
    "`member` text,\n"
    "`examlist` text\n"
    ");"
};

const static std::string testpaper {
"CREATE TABLE `TestPaper` (\n"
"  `username` varchar(128),\n"
"  `tpid` varchar(64) PRIMARY KEY,\n"
"  `tpname` varchar(256) NOT NULL,\n"
"  `classes` text\n"
");"
};

const static std::string examcodelist {
"CREATE TABLE `ExamCodeList` (\n"
    "`id` varchar(128) PRIMARY KEY,\n"
    "`name` varchar(1024) NOT NULL,\n"
    "`description` text NOT NULL,\n"
    "`explain` text NOT NULL,\n"
    "`examples1` text NOT NULL,\n"
    "`examples2` text NOT NULL,\n"
    "`timelimit` int NOT NULL,\n"
    "`memorylimit` int NOT NULL,\n"
    "`inumber` tinyint DEFAULT 0,\n"
    "`filename` varchar(1024)\n"
");"
};

const static std::string reference1 { "ALTER TABLE `usernameExecriseHistroy` ADD FOREIGN KEY (`id`) REFERENCES `ExerciseList` (`id`);" };
const static std::string reference2 { "ALTER TABLE `ClassTable` ADD FOREIGN KEY (`tusername`) REFERENCES `UsersData` (`username`);" };
const static std::string reference3 { "ALTER TABLE `TestPaper` ADD FOREIGN KEY (`username`) REFERENCES `UsersData` (`username`);" };

mysqlpp::Connection OJSql::m_connection(false);

bool OJSql::exec(std::string psql) {
    mysqlpp::Query query = m_connection.query(psql.c_str());
    if(query.exec()) {
        std::cout << "DB exec success!" << std::endl;
        return true;
    }
    else {
        std::cerr << "DB exec failed: " << query.error() << std::endl;
        return false;
    }
}

void OJSql::init() {
    exec(usersdata);
    exec(list4exercise);
    exec(classtable);
    exec(testpaper);
    exec(examcodelist);

    std::string user1 = "INSERT INTO UsersData VALUES('zhangshao', 'zhangshao', 0, '张绍', '2017211795', 0, '04011701');";
    exec(user1);
    std::string user2 = "INSERT INTO UsersData VALUES('MrZhang', 'MrZhang1234', 1, 'MrZhang', '1654974', 0, '');";
    exec(user2);
    std::string user3 = "INSERT INTO UsersData VALUES('root', 'root1234', 2, 'root', '5201314', 0, 'root');";
    exec(user3);

    std::string exerhistroy = "CREATE TABLE `ExecriseHistroy4zhangshao` ( \
                                        `id` varchar(255), \
                                        `answer` text, \
                                        `memory` varchar(5), \
                                        `times` varchar(5), \
                                        `status` varchar(2) \
                                    );";
    exec(exerhistroy);

    for(int i = 0; i < 22; ++i) {
        std::string code = "INSERT INTO ExerciseList VALUES(" + std::to_string(i + 1) + ", '两数之和', \
        '求两数之和', 0, '[C语言][&][C++][&][测试题]', '不超出int类型范围', \
        '输入：10 20 \
        输出：30 \
        解释：因为 10 + 20 = 30', \
        '输入：1000 2000 \
        输出：3000 \
        解释：因为 1000 + 2000 = 3000' , 0, 0, 100, 1024, 3, 'exerpro" + std::to_string(i + 1) + "');";
        exec(code);
    }
}

OJSql::OJSql() {
    m_connection.set_option(new mysqlpp::SetCharsetNameOption("utf8"));
    if(m_connection.connect(database.c_str(), ip.c_str(), username.c_str(), passwd.c_str())) {
        std::cout << "Connection success!" << std::endl;
    }
    else {
        std::cerr << "DB connection failed: " << m_connection.error() << std::endl;
        exit(1);
    }
}

OJSql::~OJSql() {}

mysqlpp::StoreQueryResult OJSql::store(std::string psql) {
    mysqlpp::Query query = m_connection.query(psql.c_str());
	return query.store();
}

}

/*int main() {
    EKOJ::OJSql ojsql;
    ojsql.exec("drop table `List4Exercise`;");
    ojsql.exec("drop table `ClassTable`;");
    ojsql.exec("drop table `TestPaper`;");
    ojsql.exec("drop table `Forum`;");
    ojsql.exec("drop table `UsersDate`;"); 
}*/