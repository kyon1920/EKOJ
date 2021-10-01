#pragma once

#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <string>
#include <algorithm>
#include <json/json.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>

#include <errno.h>
#include <string.h>

#include "OJ.h"
#include "OJSql.h"
#include "../OJThread/OJThreadPool.h"

#ifdef _WIN32
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#endif

/*
 * @file OJServer.h
 * @author Saint-EK
 * @brief OJServer Online Judge Html服务器
 * @date 2021-3-22
 */

// ini
const std::string WEBROOT = "../OJClient/";
const std::string DEFAULTINDEX = "index.html";

// operation
const std::string OPT_LOGIN = "LOGIN";
const std::string OPT_LOGUP = "LOGUP";
const std::string OPT_EXERCISELIST = "EXERCISELIST";
const std::string OPT_LOADEXERPRO = "LOADEXERPRO";
const std::string OPT_SUBMITCODE = "SUBMITCODE";
const std::string OPT_RUNCODE = "RUNCODE";
const std::string OPT_SUBMITEXAM = "SUBMITEXAM";
const std::string OPT_LOADTESTPAPER = "LOADTESTPAPER";
const std::string OPT_LOADTEACHERTESTPAPERPRO = "LOADTEACHERTESTPAPERPRO";
const std::string OPT_LOADTEACHERTESTPAPERCODEPRO = "LOADTEACHERTESTPAPERCODEPRO";
const std::string OPT_SUBMITTESTPAPER = "SUBMITTESTPAPER";
const std::string OPT_DELETETESTPAPER = "DELETETESTPAPER";
const std::string OPT_LOADSTUCLASS = "LOADSTUCLASS";
const std::string OPT_STUADDCLASS = "STUADDCLASS";
const std::string OPT_LOADSTUTESTPAPER = "LOADSTUTESTPAPER";
const std::string OPT_SUBMITSTUTESTPAPERANSWER = "SUBMITSTUTESTPAPERANSWER";
const std::string OPT_SUBMITSTUEXAMCODE = "SUBMITSTUEXAMCODE";
const std::string OPT_RUNSTUEXAMCODE = "RUNSTUEXAMCODE";
const std::string OPT_NEWCLASS = "NEWCLASS";
const std::string OPT_LOADTEACHERCLASS = "LOADTEACHERCLASS";
const std::string OPT_DELETECLASS = "DELETECLASS";
const std::string OPT_LOADCLASSINFO = "LOADCLASSINFO";
const std::string OPT_DELETECLASSMEMBER = "DELETECLASSMEMBER";
const std::string OPT_ADDCLASSMEMBER = "ADDCLASSMEMBER";
const std::string OPT_LOADACHIEVEMENTCLASS = "LOADACHIEVEMENTCLASS";
const std::string OPT_LOADACHIEVEMENT = "LOADACHIEVEMENT";
const std::string OPT_RLOADTEACHER = "RLOADTEACHER";
const std::string OPT_ADDRTEACHER = "ADDRTEACHER";
const std::string OPT_RDELETETEACHER = "RDELETETEACHER";
const std::string OPT_EXECUTEROOTCODE = "EXECUTEROOTCODE";

// Object
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

static void http_cb(evhttp_request* request, void* arg) {
    EKOJ::OJSql ojsql;
    std::cout << "http_cb" << std::endl;
    // 1.获取浏览器的请求信息
    // uri
    const char* uri = evhttp_request_get_uri(request);
    std::cout << "uri: " << uri << std::endl;

    // 请求类型: GET、POST
    std::string cmdtype;
    switch(evhttp_request_get_command(request)) {
    case EVHTTP_REQ_GET:
        cmdtype = "GET";
        break;
    case EVHTTP_REQ_POST:
        cmdtype = "POST";
        break;
    }
    std::cout << "cmdtype: " << cmdtype << std::endl;

    // 消息报头
    evkeyvalq* headers = evhttp_request_get_input_headers(request);

    std::cout << "==========headers==========\n";
    for(evkeyval* p = headers->tqh_first; p != nullptr; p = p->next.tqe_next) {
        std::cout << p->key << ": " << p->value << std::endl;
    }
    std::cout << "===========================" << std::endl;

    // 请求正文(GET为空，POST有表单信息)
    evbuffer* inbuf = evhttp_request_get_input_buffer(request);
    char buf[1024] ={ 0 };
    std::cout << "==========Input data==========\n";
    while(evbuffer_get_length(inbuf)) {
        evkeyvalq* headerss = evhttp_request_get_output_headers(request);
        evhttp_add_header(headerss, "Content-Type", "text/html; charset=UTF8");
        int n = evbuffer_remove(inbuf, buf, sizeof(buf) - 1);
        if(n > 0) {
            std::string reqstr = buf;
            std::string resstr;

            Json::Reader reader;
            Json::Value value;
            reader.parse(reqstr, value);
            Json::Value::Members member = value.getMemberNames();
            std::cout << "JSON 序列化成功~" << std::endl;

            /* for(Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter) {
                std::cout << (*iter) << ": " << value[*iter].asString() << std::endl;
            } */
            evbuffer* outbuff = evhttp_request_get_output_buffer(request);

            if(value["infotype"].asString() == OPT_LOGIN) {
                resstr = "{\"infotype\":\"LOGIN\",\"result\":\"FA\"}";
                std::string query = "select passwd, usertype, name, material, classid from UsersData where username = '" + value["username"].asString() + "';";
                std::cout << query << std::endl;
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                if(res.num_rows() == 1) {
                    if(res[0]["passwd"] == value["password"].asString()) {
                        std::string usertype = res[0]["usertype"].c_str();
                        std::string name = res[0]["name"].c_str();
                        std::string material = res[0]["material"].c_str();
                        std::string classid = res[0]["classid"].c_str();
                        resstr = "{\"infotype\":\"LOGIN\", \"result\":\"SU\", \"usertype\":\"" + usertype
                                + "\", \"name\":\"" + name + "\", \"material\":\"" + material + "\", \"classid\":\"" + classid + "\"}";
                    }
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOGUP) {
                resstr = "{\"infotype\":\"LOGUP\",\"result\":\"FA\"}";
                std::string query1 = "insert into UsersData values('" + value["username"].asString() + "', '"
                                                                     + value["password"].asString() + "', "
                                                                     + value["usertype"].asString() + ", '"
                                                                     + value["name"].asString() + "', '"
                                                                     + value["material"].asString() + "', "
                                                                     + "0, 0);";
                std::cout << query1 << std::endl;
                bool res1 = ojsql.exec(query1);
                std::string query2 = "CREATE TABLE `ExecriseHistroy4" + value["username"].asString() + "` ( \
                                        `id` varchar(255), \
                                        `answer` text, \
                                        `memory` varchar(5), \
                                        `times` varchar(5), \
                                        `status` varchar(2) \
                                    );";
                std::cout << query2 << std::endl;
                bool res2 = ojsql.exec(query2);
                if(res1&&res2)
                    resstr = "{\"infotype\":\"LOGUP\", \"result\":\"SU\"}";
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_EXERCISELIST) {
                resstr = "{\"infotype\":\"EXERCISELIST\", ";
                int page;
                std::stringstream page1;
                page1 << value["pages"].asString();
                page1 >> page;

                mysqlpp::StoreQueryResult res = ojsql.store("select count(*)  from ExerciseList;");
                resstr += ("\"number4exercise\": \"" + std::string(res[0].at(0)) + "\"");
                resstr += ", \"exerciselist\": [";

                std::string query1 = "select id, name, level, pass, submission, attribute from ExerciseList where id >= " + std::to_string(page * 20 + 1) + 
                                      " and id <= " + std::to_string((page + 1) * 20) + ";";
                std::cout << query1 << std::endl;
                mysqlpp::StoreQueryResult res1 = ojsql.store(query1);
                for (size_t i = 0; i < res1.num_rows(); ++i) {
                    resstr += ("{\"id\":\"" + std::string(res1[i]["id"].c_str())
                           + "\", \"name\":\"" + std::string(res1[i]["name"].c_str())
                           + "\", \"level\": \"" + std::string(res1[i]["level"].c_str())
                           + "\", \"pass\": \"" + std::string(res1[i]["pass"].c_str())
                           + "\", \"submission\": \"" + std::string(res1[i]["submission"].c_str())
                           + "\", \"attribute\": \"" + std::string(res1[i]["attribute"].c_str())
                           + "\"}");
                    if(i != res1.num_rows() - 1) {
                        resstr += ',';
                    }
			    }
                resstr += "]";

                std::string query2 = "select id from " + value["username"].asString() + "ExecriseHistroy;";
                std::cout << query2 << std::endl;
                mysqlpp::StoreQueryResult res2 = ojsql.store(query2);
                resstr += ", \"exercisehistroy\": [";
                for (size_t i = 0; i < res2.num_rows(); i++) {
                    resstr += ("{\"id\":\"" + std::string(res1[i]["id"].c_str()) + "\"}");
                    if(i != res1.num_rows() - 1) {
                        resstr += ',';
                    }
			    }
                resstr += "]}";

                std::cout << resstr.c_str() << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADEXERPRO) {
                resstr = "{\"infotype\":\"LOADEXERPRO\", ";
                int id;
                std::stringstream id1;
                id1 << value["id"].asString();
                id1 >> id;

                std::string query = "select `name`, `level`, `description`, `explain`, `examples1`, `examples2` from ExerciseList where `id` = " + std::to_string(id) + ";";
                std::cout << query << std::endl;
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                if(res.num_rows() == 1) {
                    resstr += ("\"name\":\"" + std::string(res[0]["name"].c_str())
                           + "\", \"level\": \"" + std::string(res[0]["level"].c_str())
                           + "\", \"description\": \"" + std::string(res[0]["description"].c_str())
                           + "\", \"explain\": \"" + std::string(res[0]["explain"].c_str())
                           + "\", \"examples1\": \"" + std::string(res[0]["examples1"].c_str())
                           + "\", \"examples2\": \"" + std::string(res[0]["examples2"].c_str()));
                }
                resstr += "\"}";

                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_RUNCODE) {
                resstr = "{\"infotype\":\"RUNCODE\", ";
                int timelimit = 0;
                int memorylimit = 0;
                bool type;
                std::string fname = value["fname"].asString();
                std::string qname = value["qname"].asString();
                std::string tmptype =  value["type"].asString();
                std::string code = value["code"].asString();
                if(tmptype == "C") {
                    type = true;
                    std::string filename = "./question/" + qname + ".c";
                    std::cout << filename << std::endl;
                    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
                    file << code;
                    file.close();
                }
                else if(tmptype == "C++") {
                    type = false;
                    std::string filename = "./question/" + qname + ".cc";
                    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
                    file << code;
                    file.close();
                }

                int id;
                std::stringstream id1;
                id1 << value["id"].asString();
                id1 >> id;
                std::string query = "select `timelimit`, `memorylimit` from ExerciseList where `id` = " + std::to_string(id) + ";";
                std::cout << query << std::endl;
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                if(res.num_rows() == 1) {
                    timelimit = res[0]["timelimit"];
                    memorylimit = res[0]["memorylimit"];
                }

                EKOJ::OJ tmp(fname, qname, timelimit, memorylimit, type, 3);
                std::pair<EKOJ::OJRE, std::string> result = tmp.run();
                std::cout << result.first.status << ":" << result.first.time << ":" << result.first.memory << ":" << result.first.ratio << '\n' << result.second << std::endl;
                
                resstr += ("\"status\":\"" + std::to_string(result.first.status)
                        + "\", \"time\":\"" + std::to_string(result.first.time) 
                        + "\", \"memory\":\"" + std::to_string(result.first.memory)
                        + "\", \"ratio\":\"" + std::to_string(result.first.ratio)
                        + "\", \"resultstr\":\"" + result.second + "\"}");

                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_SUBMITEXAM) {
                resstr = "{\"infotype\":\"SUBMITEXAM\", ";
                int selectpronumber = value["select"].size();
                int judgepronumber = value["judge"].size();
                int codepronumber = value["code"].size();
                struct timeval tv;
                gettimeofday(&tv, NULL);
                std::stringstream tmp;
                tmp << tv.tv_sec;
                std::string tpid = tmp.str() + value["username"].asString();
                std::string query1 = "CREATE TABLE `" + tpid + "`(\n \
                                     `id` tinyint PRIMARY KEY AUTO_INCREMENT, \
                                     `discription` text NOT NULL, \
                                     `type` tinyint NOT NULL, \
                                     `option` text NOT NULL, \
                                     `answer` text NOT NULL, \
                                     `score` tinyint DEFAULT 0, \
                                     `inumber` tinyint DEFAULT 0);";
                std::string query2 = "CREATE TABLE `" + tpid + "_answer`(\n \
                                     `username` varchar(64) NOT NULL, \
                                     `tusername` varchar(64) NOT NULL, \
                                     `classid` varchar(64) NOT NULL\n";
                for(int i = 0; i < selectpronumber; ++i) {
                    int number = i + 1;
                    query2 += ", `" + std::to_string(number) + "_select` varchar(8)";
                }
                for(int i = 0; i < judgepronumber; ++i) {
                    int number = i + 1;
                    query2 += ", `" + std::to_string(number) + "_judge` varchar(8)";
                }
                for(int i = 0; i < codepronumber; ++i) {
                    int number = i + 1;
                    query2 += ", `" + std::to_string(number) + "_code` text";
                }
                query2 += ");";
                bool sres1 = ojsql.exec(query1);
                bool sres2 = ojsql.exec(query2);
                if(sres1 && sres2) {
                    bool execquery = true;
                    for(int i = 0; i < selectpronumber; ++i) {
                        std::cout << value["select"][i]["discription"].asString() << std::endl;
                        std::string queryselect = "INSERT INTO " + tpid + 
                                                  "(`discription`, `type`, `option`, `answer`, `score`, `inumber`) VALUES('"
                                                + value["select"][i]["discription"].asString() + "', "
                                                + value["select"][i]["type"].asString() + ", '"
                                                + value["select"][i]["option"].asString() + "', '"
                                                + value["select"][i]["answer"].asString() + "', "
                                                + value["select"][i]["score"].asString() + ", "
                                                + value["select"][i]["inumber"].asString() + ");";
                        std::cout << queryselect << std::endl;
                        bool res = ojsql.exec(queryselect);
                        if(!res) {
                            execquery = false;
                        }
                    }
                    for(int i = 0; i < judgepronumber; ++i) {
                        std::string queryjudge = "INSERT INTO " + tpid + 
                                                  "(`discription`, `type`, `option`, `answer`, `score`, `inumber`) VALUES('"
                                                + value["judge"][i]["discription"].asString() + "', "
                                                + value["judge"][i]["type"].asString() + ", '"
                                                + value["judge"][i]["option"].asString() + "', '"
                                                + value["judge"][i]["answer"].asString() + "', "
                                                + value["judge"][i]["score"].asString() + ", "
                                                + value["judge"][i]["inumber"].asString() + ");";
                        std::cout << queryjudge << std::endl;
                        bool res = ojsql.exec(queryjudge);
                        if(!res) {
                            execquery = false;
                        }
                    }
                    for(int i = 0; i < codepronumber; ++i) {
                        std::string querycode = "INSERT INTO ExamCodeList VALUES('" + tpid + "', "
                                                + std::to_string(i) + ", "
                                                + value["code"][i]["score"].asString() + ", '"
                                                + value["code"][i]["name"].asString() + "', '"
                                                + value["code"][i]["description"].asString() + "', '"
                                                + value["code"][i]["explain"].asString() + "', '"
                                                + value["code"][i]["examples1"].asString() + "', '"
                                                + value["code"][i]["examples2"].asString() + "', "
                                                + value["code"][i]["timelimit"].asString() + ", "
                                                + value["code"][i]["memorylimit"].asString() + ", 3);";
                        std::cout << querycode << std::endl;
                        bool res = ojsql.exec(querycode);

                        std::string filename_in_1 = "./in/" + tpid + "_" + std::to_string(i) + "_0.in";
                        std::cout << filename_in_1 << std::endl;
                        std::ofstream file1(filename_in_1.c_str(), std::ios::out | std::ios::trunc);
                        file1 << value["code"][i]["file_in_1"].asString();
                        file1.close();
                        std::string filename_in_2 = "./in/" + tpid + "_" + std::to_string(i) + "_1.in";
                        std::cout << filename_in_2 << std::endl;
                        std::ofstream file2(filename_in_2.c_str(), std::ios::out | std::ios::trunc);
                        file2 << value["code"][i]["file_in_2"].asString();
                        file2.close();
                        std::string filename_in_3 = "./in/" + tpid + "_" + std::to_string(i) + "_2.in";
                        std::cout << filename_in_3 << std::endl;
                        std::ofstream file3(filename_in_3.c_str(), std::ios::out | std::ios::trunc);
                        file3 << value["code"][i]["file_in_3"].asString();
                        file3.close();

                        std::string filename_out_1 = "./answer/" + tpid + "_" + std::to_string(i) + "_0.out";
                        std::cout << filename_out_1 << std::endl;
                        std::ofstream file4(filename_out_1.c_str(), std::ios::out | std::ios::trunc);
                        file4 << value["code"][i]["file_out_1"].asString();
                        file4.close();
                        std::string filename_out_2 = "./answer/" + tpid + "_" + std::to_string(i) + "_1.out";
                        std::cout << filename_out_2 << std::endl;
                        std::ofstream file5(filename_out_2.c_str(), std::ios::out | std::ios::trunc);
                        file5 << value["code"][i]["file_out_2"].asString();
                        file5.close();
                        std::string filename_out_3 = "./answer/" + tpid + "_" + std::to_string(i) + "_2.out";
                        std::cout << filename_out_3 << std::endl;
                        std::ofstream file6(filename_out_3.c_str(), std::ios::out | std::ios::trunc);
                        file6 << value["code"][i]["file_out_3"].asString();
                        file6.close();

                        if(!res) {
                            execquery = false;
                        }
                    }
                    std::string queryinsertexam = "INSERT INTO TestPaper VALUES('"
                                                  + value["username"].asString() + "', '"
                                                  + tpid + "', '"
                                                  + value["name"].asString() + "');";
                    std::cout << queryinsertexam << std::endl;
                    
                    bool res = ojsql.exec(queryinsertexam);
                    if(!res) {
                        execquery = false;
                    }
                    if(execquery) {
                        resstr += "\"result\": \"SU\"}";
                    }
                }
                else {
                    resstr += "\"result\": \"FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADTESTPAPER) {
                resstr = "{\"infotype\":\"LOADTESTPAPER\", \"testpaperlist\": [";
                std::string username = value["username"].asString();
                std::string query = "SELECT `tpid`, `tpname` FROM `TestPaper` WHERE `username` = '" + username + "';";
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                for(int i = 0; i < res.num_rows(); ++i) {
                    if(i == res.num_rows() - 1) {
                        resstr += "{\"tpid\": \"" + std::string(res[i]["tpid"].c_str()) + "\", \"name\": \"" + std::string(res[i]["tpname"].c_str()) + "\"}";
                    }
                    else {
                        resstr += "{\"tpid\": \"" + std::string(res[i]["tpid"].c_str()) + "\", \"name\": \"" + std::string(res[i]["tpname"].c_str()) + "\"}, ";
                    }
                }
                resstr += "]}";
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADTEACHERTESTPAPERPRO) {
                resstr = "{\"infotype\":\"LOADTEACHERTESTPAPERPRO\", \"class\": [";

                std::string tusername = value["username"].asString();
                std::string classquery = "SELECT `id` FROM ClassTable WHERE `tusername` = '" + tusername + "';";
                mysqlpp::StoreQueryResult res4class = ojsql.store(classquery);
                int rows4class = res4class.num_rows();
                for(int i = 0; i < rows4class; ++i) {
                    if(i == rows4class - 1) {
                        resstr += "{\"id\":\"" + std::string(res4class[i]["id"].c_str()) + "\"}";
                    }
                    else {
                        resstr += "{\"id\":\"" + std::string(res4class[i]["id"].c_str()) + "\"}, ";
                    }
                }

                resstr += "], \"select\": [";
                std::string selectquery = "SELECT `discription`, `option`, `answer`, `score` FROM "
                                          + value["testpapername"].asString() + " WHERE `type` = 0;";
                mysqlpp::StoreQueryResult res4select = ojsql.store(selectquery);
                int rows4select = res4select.num_rows();
                for(int i = 0; i < rows4select; ++i) {
                    std::string option = res4select[i]["option"].c_str();
                    std::vector<std::string> options = split(option, "[&]");
                    std::string option4A, option4B, option4C, option4D;
                    if(options.size() == 4) {
                        option4A = options[0];
                        option4B = options[1];
                        option4C = options[2];
                        option4D = options[3];
                    }
                    if(i == rows4select - 1) {
                        resstr += "{\"discription\":\"" + std::string(res4select[i]["discription"].c_str())
                           + "\", \"A\": \"" + option4A
                           + "\", \"B\": \"" + option4B
                           + "\", \"C\": \"" + option4C
                           + "\", \"D\": \"" + option4D
                           + "\", \"answer\": \"" + std::string(res4select[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4select[i]["score"].c_str())
                           + "\"}";
                    }
                    else {
                        resstr += "{\"discription\":\"" + std::string(res4select[i]["discription"].c_str())
                           + "\", \"A\": \"" + option4A
                           + "\", \"B\": \"" + option4B
                           + "\", \"C\": \"" + option4C
                           + "\", \"D\": \"" + option4D
                           + "\", \"answer\": \"" + std::string(res4select[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4select[i]["score"].c_str())
                           + "\"}, ";
                    }
                }

                resstr += "], \"judge\": [";
                std::string judgequery = "SELECT `discription`, `option`, `answer`, `score` FROM "
                                          + value["testpapername"].asString() + " WHERE `type` = 1;";
                mysqlpp::StoreQueryResult res4judge = ojsql.store(judgequery);
                int rows4judge = res4judge.num_rows();
                for(int i = 0; i < rows4judge; ++i) {
                    if(i == rows4judge - 1) {
                        resstr += "{\"discription\":\"" + std::string(res4judge[i]["discription"].c_str())
                           + "\", \"A\": \"正确\", \"B\": \"错误\", \"answer\": \"" + std::string(res4judge[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4judge[i]["score"].c_str())
                           + "\"}";
                    }
                    else {
                        resstr += "{\"discription\":\"" + std::string(res4judge[i]["discription"].c_str())
                           + "\", \"A\": \"正确\", \"B\": \"错误\", \"answer\": \"" + std::string(res4judge[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4judge[i]["score"].c_str())
                           + "\"}, ";
                    }
                }

                resstr += "], \"codelist\": [";
                std::string codequery = "SELECT `number`, `score`, `name` FROM ExamCodeList WHERE `id`='" + value["testpapername"].asString() + "';";
                                         std::cout << codequery << std::endl;
                mysqlpp::StoreQueryResult res4code = ojsql.store(codequery);
                int rows4code = res4code.num_rows();
                for(int i = 0; i < rows4code; ++i) {
                    if(i == rows4code - 1) {
                        resstr += "{\"number\":\"" + std::string(res4code[i]["number"].c_str())
                           + "\", \"score\": \"" +  std::string(res4code[i]["score"].c_str())
                           + "\", \"name\": \"" +  std::string(res4code[i]["name"].c_str())
                           + "\"}";
                    }
                    else {
                        resstr += "{\"number\":\"" + std::string(res4code[i]["number"].c_str())
                           + "\", \"score\": \"" +  std::string(res4code[i]["score"].c_str())
                           + "\", \"name\": \"" +  std::string(res4code[i]["name"].c_str())
                           + "\"}, ";
                    }
                }
                resstr += "]}";
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADTEACHERTESTPAPERCODEPRO) {
                resstr = "{\"infotype\":\"LOADTEACHERTESTPAPERCODEPRO\"";
                std::string codequery = "SELECT `number`, `score`, `name`, `description`, `explain`, `examples1`, `examples2` FROM ExamCodeList WHERE `id`='" + value["tpid"].asString() + "' and `number`=" + value["tcodeid"].asString() + ";";
                std::cout << codequery << std::endl;
                mysqlpp::StoreQueryResult res4code = ojsql.store(codequery);
                int rows4code = res4code.num_rows();
                if(rows4code == 1) {
                    resstr += ", \"number\":\"" + std::string(res4code[0]["number"].c_str())
                           + "\", \"score\": \"" +  std::string(res4code[0]["score"].c_str())
                           + "\", \"name\": \"" +  std::string(res4code[0]["name"].c_str())
                           + "\", \"description\": \"" +  std::string(res4code[0]["description"].c_str())
                           + "\", \"explain\": \"" +  std::string(res4code[0]["explain"].c_str())
                           + "\", \"examples1\": \"" + std::string(res4code[0]["examples1"].c_str())
                           + "\", \"examples2\": \"" + std::string(res4code[0]["examples2"].c_str())
                           + "\"}";
                }
                else {
                    resstr += "}";
                }
                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_SUBMITTESTPAPER) {
                struct timeval tv;
                gettimeofday(&tv, NULL);
                std::stringstream tmp;
                tmp << tv.tv_sec;

                resstr = "{\"infotype\":\"SUBMITTESTPAPER\"";
                std::string submitquery = "INSERT INTO NowExam VALUES('" 
                                        + value["username"].asString() + "', '"
                                        + value["classid"].asString() + "', '"
                                        + value["tpid"].asString() + "', '"
                                        + tmp.str() + "', "
                                        + value["timelimit"].asString() + ");";
                bool res = ojsql.exec(submitquery);
                std::cout << submitquery << std::endl;
                if(res) {
                    resstr += ",\"result\": \"SU\"}";
                }
                else {
                    resstr += ",\"result\": \"FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_DELETETESTPAPER) {
                resstr = "{\"infotype\":\"DELETETESTPAPER\"";
                std::string query4delete = "DELETE FROM TestPaper WHERE `tpid`='" + value["tpid"].asString() + "';";
                bool res = ojsql.exec(query4delete);
                if(res) {
                    std::string deletequery = "DELETE FROM ExamCodeList WHERE `id`='" + value["tpid"].asString() + "';";
                    std::string deletetestpaper = "DROP TABLE " + value["tpid"].asString() + ";";
                    std::string deletetestpaperanswer = "DROP TABLE " + value["tpid"].asString() + "_answer;";
                    bool res1 = ojsql.exec(deletequery);
                    bool res2 = ojsql.exec(deletetestpaper);
                    bool res3 = ojsql.exec(deletetestpaperanswer);
                    if(res1 && res2 && res3) {
                        std::string deletefile1 = "rm -rf ./answer" + value["tpid"].asString() + "*";
                        system(deletefile1.c_str());
                        std::string deletefile2 = "rm -rf ./in" + value["tpid"].asString() + "*";
                        system(deletefile2.c_str());
                        resstr += ",\"result\": \"SU\"}";
                    }
                    else {
                        resstr += ",\"result\": \"FA\"}";
                    }
                }
                else {
                    resstr += ",\"result\": \"FA\"}";
                }
                
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADSTUCLASS) {
                resstr = "{\"infotype\":\"LOADSTUCLASS\", \"result\": \"";
                std::string username = value["username"].asString();
                std::string classquery = "SELECT `classid` FROM `UsersData` WHERE `username` = '" + username + "';";
                mysqlpp::StoreQueryResult res4class = ojsql.store(classquery);
                if(res4class.num_rows() == 1 && std::string(res4class[0]["classid"].c_str()).size() > 2) {
                    resstr += "SU\", \"classid\": \"" + std::string(res4class[0]["classid"].c_str()) + "\", \"testpaperlist\": [";
                    std::string query4nowexam = "SELECT `tpid`, `starttime`, `timelimit` FROM `NowExam` WHERE `classid` = '" + std::string(res4class[0]["classid"].c_str()) + "';";
                    mysqlpp::StoreQueryResult res4nowexam = ojsql.store(query4nowexam);
                    int row4nowexam = res4nowexam.num_rows();
                    for(int i = 0; i < row4nowexam; ++i) {
                        std::string query4testpaper = "SELECT `username`, `tpid`, `tpname` FROM `TestPaper` WHERE `tpid` = '" + std::string(res4nowexam[i]["tpid"].c_str()) + "';";
                        mysqlpp::StoreQueryResult res4testpaper = ojsql.store(query4testpaper);
                        int row4testpaper = res4testpaper.num_rows();
                        for(int j = 0; j < row4testpaper; ++j) {
                            if(i == row4nowexam - 1) {
                                resstr += "{\"tusername\": \"" + std::string(res4testpaper[j]["username"].c_str())
                                        + "\", \"tpid\": \"" + std::string(res4testpaper[j]["tpid"].c_str())
                                        + "\", \"tpname\": \"" + std::string(res4testpaper[j]["tpname"].c_str())
                                        + "\", \"starttime\": \"" + std::string(res4nowexam[i]["starttime"].c_str())
                                        + "\", \"timelimit\": \"" + std::string(res4nowexam[i]["timelimit"].c_str())
                                        + "\"}";
                            }
                            else {
                                resstr += "{\"tusername\": \"" + std::string(res4testpaper[j]["username"].c_str())
                                        + "\", \"tpid\": \"" + std::string(res4testpaper[j]["tpid"].c_str())
                                        + "\", \"tpname\": \"" + std::string(res4testpaper[j]["tpname"].c_str())
                                        + "\", \"starttime\": \"" + std::string(res4nowexam[i]["starttime"].c_str())
                                        + "\", \"timelimit\": \"" + std::string(res4nowexam[i]["timelimit"].c_str())
                                        + "\"}, ";
                            }
                        }
                    }
                    resstr += "]}";
                }
                else {
                    resstr += "FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_STUADDCLASS) {
                resstr = "{\"infotype\":\"STUADDCLASS\", \"result\": \"";
                std::string classid = value["classid"].asString();
                std::string query4is = "SELECT `id` FROM `ClassTable` WHERE `id` = '" + classid +"';";
                mysqlpp::StoreQueryResult res4is = ojsql.store(query4is);
                if(res4is.num_rows() >= 1) {
                    std::string query = "UPDATE `UsersData` SET `classid` = '" + classid + "' WHERE `username` = '" + value["username"].asString() + "'";
                    std::cout << query << std::endl;
                    bool res = ojsql.exec(query);
                    if(res) {
                        resstr += "SU\"}";
                    }
                    else {
                        resstr += "FA\"}";
                    }
                }
                else {
                    resstr += "FA\"}";
                }
                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADSTUTESTPAPER) {
                resstr = "{\"infotype\":\"LOADSTUTESTPAPER\", \"select\": [";
                std::string selectquery = "SELECT `discription`, `option`, `answer`, `score` FROM "
                                          + value["testpapername"].asString() + " WHERE `type` = 0;";
                mysqlpp::StoreQueryResult res4select = ojsql.store(selectquery);
                int rows4select = res4select.num_rows();
                for(int i = 0; i < rows4select; ++i) {
                    std::string option = res4select[i]["option"].c_str();
                    std::vector<std::string> options = split(option, "[&]");
                    std::string option4A, option4B, option4C, option4D;
                    if(options.size() == 4) {
                        option4A = options[0];
                        option4B = options[1];
                        option4C = options[2];
                        option4D = options[3];
                    }
                    if(i == rows4select - 1) {
                        resstr += "{\"discription\":\"" + std::string(res4select[i]["discription"].c_str())
                           + "\", \"A\": \"" + option4A
                           + "\", \"B\": \"" + option4B
                           + "\", \"C\": \"" + option4C
                           + "\", \"D\": \"" + option4D
                           + "\", \"answer\": \"" + std::string(res4select[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4select[i]["score"].c_str())
                           + "\"}";
                    }
                    else {
                        resstr += "{\"discription\":\"" + std::string(res4select[i]["discription"].c_str())
                           + "\", \"A\": \"" + option4A
                           + "\", \"B\": \"" + option4B
                           + "\", \"C\": \"" + option4C
                           + "\", \"D\": \"" + option4D
                           + "\", \"answer\": \"" + std::string(res4select[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4select[i]["score"].c_str())
                           + "\"}, ";
                    }
                }

                resstr += "], \"judge\": [";
                std::string judgequery = "SELECT `discription`, `option`, `answer`, `score` FROM "
                                          + value["testpapername"].asString() + " WHERE `type` = 1;";
                mysqlpp::StoreQueryResult res4judge = ojsql.store(judgequery);
                int rows4judge = res4judge.num_rows();
                for(int i = 0; i < rows4judge; ++i) {
                    if(i == rows4judge - 1) {
                        resstr += "{\"discription\":\"" + std::string(res4judge[i]["discription"].c_str())
                           + "\", \"A\": \"正确\", \"B\": \"错误\", \"answer\": \"" + std::string(res4judge[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4judge[i]["score"].c_str())
                           + "\"}";
                    }
                    else {
                        resstr += "{\"discription\":\"" + std::string(res4judge[i]["discription"].c_str())
                           + "\", \"A\": \"正确\", \"B\": \"错误\", \"answer\": \"" + std::string(res4judge[i]["answer"].c_str())
                           + "\", \"score\": \"" + std::string(res4judge[i]["score"].c_str())
                           + "\"}, ";
                    }
                }

                resstr += "], \"codelist\": [";
                std::string codequery = "SELECT `number`, `score`, `name` FROM ExamCodeList WHERE `id`='" + value["testpapername"].asString() + "';";
                                         std::cout << codequery << std::endl;
                mysqlpp::StoreQueryResult res4code = ojsql.store(codequery);
                int rows4code = res4code.num_rows();
                for(int i = 0; i < rows4code; ++i) {
                    if(i == rows4code - 1) {
                        resstr += "{\"number\":\"" + std::string(res4code[i]["number"].c_str())
                           + "\", \"score\": \"" +  std::string(res4code[i]["score"].c_str())
                           + "\", \"name\": \"" +  std::string(res4code[i]["name"].c_str())
                           + "\"}";
                    }
                    else {
                        resstr += "{\"number\":\"" + std::string(res4code[i]["number"].c_str())
                           + "\", \"score\": \"" +  std::string(res4code[i]["score"].c_str())
                           + "\", \"name\": \"" +  std::string(res4code[i]["name"].c_str())
                           + "\"}, ";
                    }
                }
                resstr += "]}";
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_SUBMITSTUTESTPAPERANSWER) {
                resstr = "{\"infotype\":\"SUBMITSTUTESTPAPERANSWER\", \"result\": \"";

                std::map<std::string, std::string> answers;
                std::string query4select = "SELECT `id`, `score`, `answer` FROM " + value["tpid"].asString() + " WHERE `type` = 0";
                mysqlpp::StoreQueryResult res4select = ojsql.store(query4select);
                int rows4select = res4select.num_rows();
                if(rows4select > 0) {
                    answers.insert(std::make_pair("select_score", std::string(res4select[0]["score"].c_str())));
                }
                for(int i = 0; i < rows4select; ++i) {
                    std::string p1 = std::string(res4select[i]["id"].c_str()) + "_select";
                    std::string p2 = std::string(res4select[i]["answer"].c_str());
                    answers.insert(std::make_pair(p1, p2));
                }

                std::string query4judge = "SELECT `id`, `score`, `answer` FROM " + value["tpid"].asString() + " WHERE `type` = 1";
                mysqlpp::StoreQueryResult res4judge = ojsql.store(query4judge);
                int rows4judge = res4judge.num_rows();
                if(rows4judge > 0) {
                    answers.insert(std::make_pair("judge_score", std::string(res4judge[0]["score"].c_str())));
                }
                for(int i = 0; i < rows4judge; ++i) {
                    std::stringstream tmp;
                    tmp << std::string(res4judge[i]["id"].c_str());
                    int number;
                    tmp >> number;
                    number -= rows4select;
                    std::string p1 = std::to_string(number) + "_judge";
                    std::string p2 = std::string(res4judge[i]["answer"].c_str());
                    answers.insert(std::make_pair(p1, p2));
                }
                
                std::string query4existence = "SELECT `username`, `tusername`, `classid` FROM " + value["tpid"].asString() + "_answer WHERE `username` = '" 
                                            + value["username"].asString()
                                            + "' and `classid` = '" + value["classid"].asString()
                                            + "' and `tusername` = '" + value["tusername"].asString() + "';";
                std::cout << query4existence << std::endl;
                mysqlpp::StoreQueryResult res4existence = ojsql.store(query4existence);
                if(res4existence.num_rows() == 1) {
                    std::string query4iexam = "UPDATE `" + value["tpid"].asString() + "_answer` SET ";
                    std::stringstream tmp4select;
                    tmp4select << value["selectpronumber"].asString();
                    int selectnumber;
                    tmp4select >> selectnumber;
                    std::string score4select;
                    std::string score4judge;
                    if(selectnumber > 0) {
                        score4select = answers["select_score"];
                    }
                    for(int i = 0; i < selectnumber; ++i) {
                        std::string name = std::to_string(i + 1) + "_select_answer";
                        std::string name4compare = std::to_string(i + 1) + "_select";
                        if(i == selectnumber - 1) {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]" + score4select + "'";
                            }
                            else {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]0'";
                            }
                        }
                        else {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]" + score4select + "', ";
                            }
                            else {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]0', ";
                            }
                        }
                    }
                    std::stringstream tmp4judge;
                    tmp4judge << value["judgepronumber"].asString();
                    int judgenumber;
                    tmp4judge >> judgenumber;
                    if(judgenumber > 0) {
                        query4iexam += ", ";
                        score4judge = answers["judge_score"];
                    }
                    for(int i = 0; i < judgenumber; ++i) {
                        std::string name = std::to_string(i + 1) + "_judge_answer";
                        std::string name4compare = std::to_string(i + 1) + "_judge";
                        if(i == judgenumber - 1) {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]" + score4judge + "'";
                            }
                            else {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]0'";
                            }
                        }
                        else {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]" + score4judge + "', ";
                            }
                            else {
                                query4iexam += "`" + name4compare + "` = '" + value[name].asString() + "[&]0', ";
                            }
                        }
                    }
                    query4iexam += " WHERE `username` = '" + value["username"].asString() + "';";
                    std::cout << query4iexam << std::endl;
                    bool res = ojsql.exec(query4iexam);
                    if(res) {
                        resstr += "SU\"}";
                    }
                    else {
                        resstr += "FA\"}";
                    }
                }
                else {
                    std::string query4iexam = "INSERT INTO " + value["tpid"].asString() + "_answer VALUES('"
                                            + value["username"].asString() + "', '"
                                            + value["tusername"].asString() + "', '"
                                            + value["classid"].asString() + "'";
                    std::stringstream tmp4select;
                    tmp4select << value["selectpronumber"].asString();
                    int selectnumber;
                    tmp4select >> selectnumber;
                    std::string score4select;
                    std::string score4judge;
                    if(selectnumber > 0) {
                        query4iexam += ", ";
                        score4select = answers["select_score"];
                    }
                    for(int i = 0; i < selectnumber; ++i) {
                        std::string name = std::to_string(i + 1) + "_select_answer";
                        std::string name4compare = std::to_string(i + 1) + "_select";
                        if(i == selectnumber - 1) {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "'" + value[name].asString() + "[&]" + score4select + "'";
                            }
                            else {
                                query4iexam += "'" + value[name].asString() + "[&]0'";
                            }
                        }
                        else {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "'" + value[name].asString() + "[&]" + score4select + "', ";
                            }
                            else {
                                query4iexam += "'" + value[name].asString() + "[&]0', ";
                            }
                        }
                    }
                    std::stringstream tmp4judge;
                    tmp4judge << value["judgepronumber"].asString();
                    int judgenumber;
                    tmp4judge >> judgenumber;
                    if(judgenumber > 0) {
                        query4iexam += ", ";
                        score4judge = answers["judge_score"];
                    }
                    for(int i = 0; i < judgenumber; ++i) {
                        std::string name = std::to_string(i + 1) + "_judge_answer";
                        std::string name4compare = std::to_string(i + 1) + "_judge";
                        if(i == judgenumber - 1) {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "'" + value[name].asString() + "[&]" + score4judge + "'";
                            }
                            else {
                                query4iexam += "'" + value[name].asString() + "[&]0'";
                            }
                        }
                        else {
                            if(value[name].asString() == answers[name4compare]) {
                                query4iexam += "'" + value[name].asString() + "[&]" + score4judge + "', ";
                            }
                            else {
                                query4iexam += "'" + value[name].asString() + "[&]0', ";
                            }
                        }
                    }
                    std::stringstream tmp4code;
                    tmp4code << value["codepronumber"].asString();
                    int codenumber;
                    tmp4code >> codenumber;
                    if(codenumber > 0) {
                        query4iexam += ", ";
                    }
                    for(int i = 0; i < codenumber; ++i) {
                        if(i == codenumber - 1) {
                            query4iexam += "''";
                        }
                        else {
                            query4iexam += "'', ";
                        }
                    }
                    query4iexam += ");";
                    std::cout << query4iexam << std::endl;
                    bool res = ojsql.exec(query4iexam);
                    if(res) {
                        resstr += "SU\"}";
                    }
                    else {
                        resstr += "FA\"}";
                    }
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_RUNSTUEXAMCODE) {
                resstr = "{\"infotype\":\"RUNSTUEXAMCODE\", ";
                int timelimit = 0;
                int memorylimit = 0;
                bool type;
                std::string fname = value["fname"].asString();
                std::string qname = value["qname"].asString();
                std::string tmptype =  value["type"].asString();
                std::string code = value["code"].asString();
                if(tmptype == "C") {
                    type = true;
                    std::string filename = "./question/" + qname + ".c";
                    std::cout << filename << std::endl;
                    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
                    file << code;
                    file.close();
                }
                else if(tmptype == "C++") {
                    type = false;
                    std::string filename = "./question/" + qname + ".cc";
                    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
                    file << code;
                    file.close();
                }

                int number;
                std::stringstream id1;
                id1 << value["number"].asString();
                id1 >> number;
                std::string query = "select `timelimit`, `memorylimit`, `score` from ExamCodeList where `number` = " + std::to_string(number) + " and `id` = '" + value["id"].asString() + "';";
                std::cout << query << std::endl;
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                if(res.num_rows() == 1) {
                    std::stringstream tmp1;
                    tmp1 << std::string(res[0]["timelimit"].c_str());
                    tmp1 >> timelimit;
                    std::stringstream tmp2;
                    tmp2 << std::string(res[0]["memorylimit"].c_str());
                    tmp2 >> memorylimit;
                }

                std::cout << timelimit << " " << memorylimit << std::endl;
                EKOJ::OJ tmp(fname, qname, timelimit, memorylimit, type, 3);
                std::pair<EKOJ::OJRE, std::string> result = tmp.run();
                std::cout << result.first.status << ":" << result.first.time << ":" << result.first.memory << ":" << result.first.ratio << '\n' << result.second << std::endl;
                
                resstr += ("\"status\":\"" + std::to_string(result.first.status)
                        + "\", \"time\":\"" + std::to_string(result.first.time) 
                        + "\", \"memory\":\"" + std::to_string(result.first.memory)
                        + "\", \"ratio\":\"" + std::to_string(result.first.ratio)
                        + "\", \"resultstr\":\"" + result.second + "\"}");

                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_SUBMITSTUEXAMCODE) {
                resstr = "{\"infotype\":\"SUBMITSTUEXAMCODE\", ";
                int timelimit = 0;
                int memorylimit = 0;
                bool type;
                std::string fname = value["fname"].asString();
                std::string qname = value["qname"].asString();
                std::string tmptype =  value["type"].asString();
                std::string code = value["code"].asString();
                if(tmptype == "C") {
                    type = true;
                    std::string filename = "./question/" + qname + ".c";
                    std::cout << filename << std::endl;
                    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
                    file << code;
                    file.close();
                }
                else if(tmptype == "C++") {
                    type = false;
                    std::string filename = "./question/" + qname + ".cc";
                    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
                    file << code;
                    file.close();
                }
                
                int number;
                float score;
                std::stringstream id1;
                id1 << value["number"].asString();
                id1 >> number;
                std::string query = "select `timelimit`, `memorylimit`, `score` from ExamCodeList where `number` = " + std::to_string(number) + " and `id` = '" + value["id"].asString() + "';";
                std::cout << query << std::endl;
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                if(res.num_rows() == 1) {
                    std::stringstream tmp1;
                    tmp1 << std::string(res[0]["timelimit"].c_str());
                    tmp1 >> timelimit;
                    std::stringstream tmp2;
                    tmp2 << std::string(res[0]["memorylimit"].c_str());
                    tmp2 >> memorylimit;
                    std::stringstream tmp3;
                    tmp3 << std::string(res[0]["score"].c_str());
                    tmp3 >> score;
                }

                std::cout << timelimit << " " << memorylimit << std::endl;
                EKOJ::OJ tmp(fname, qname, timelimit, memorylimit, type, 3);
                std::pair<EKOJ::OJRE, std::string> result = tmp.run();
                std::cout << result.first.status << ":" << result.first.time << ":" << result.first.memory << ":" << result.first.ratio << '\n' << result.second << std::endl;
                
                resstr += ("\"status\":\"" + std::to_string(result.first.status)
                        + "\", \"time\":\"" + std::to_string(result.first.time) 
                        + "\", \"memory\":\"" + std::to_string(result.first.memory)
                        + "\", \"ratio\":\"" + std::to_string(result.first.ratio)
                        + "\", \"resultstr\":\"" + result.second);

                if(result.first.status == 8) {
                    score *= result.first.ratio;
                }
                else if(result.first.status == 0) {
                    score *= 1.0;
                }
                else {
                    score = 0;
                }

                std::string query4existence = "SELECT `username`, `tusername`, `classid` FROM " + value["id"].asString() + "_answer WHERE `username` = '" 
                                            + value["username"].asString()
                                            + "' and `classid` = '" + value["classid"].asString()
                                            + "' and `tusername` = '" + value["tusername"].asString() + "';";
                std::cout << query4existence << std::endl;
                mysqlpp::StoreQueryResult res4existence = ojsql.store(query4existence);
                if(res4existence.num_rows() == 1) {
                    std::string query4iexam = "UPDATE `" + value["id"].asString() + "_answer` SET `" + std::to_string(number + 1) + "_code` = '"
                                            + code 
                                            + "[&]" + std::to_string(score)
                                            + "[&]" + std::to_string(result.first.status)
                                            + "[&]" + std::to_string(result.first.time)
                                            + "[&]" + std::to_string(result.first.memory)
                                            + "' WHERE `username` = '" + value["username"].asString() + "';";
                    std::cout << query4iexam << std::endl;
                    bool res = ojsql.exec(query4iexam);
                    if(res) {
                        resstr += "\", \"result\":\"SU\"}";
                    }
                    else {
                        resstr += "\", \"result\":\"FA\"}";
                    }
                    
                }
                else {
                    std::string query4iexam = "INSERT INTO " + value["id"].asString() + "_answer(`username`, `tusername`, `classid`, `"
                                            + std::to_string(number + 1) + "_code`) VALUES('"
                                            + value["username"].asString() + "', '"
                                            + value["tusername"].asString() + "', '"
                                            + value["classid"].asString() + "', '"
                                            + code 
                                            + "[&]" + std::to_string(score)
                                            + "[&]" + std::to_string(result.first.status)
                                            + "[&]" + std::to_string(result.first.time)
                                            + "[&]" + std::to_string(result.first.memory)
                                            + "');";
                    std::cout << query4iexam << std::endl;
                    bool res = ojsql.exec(query4iexam);
                    if(res) {
                        resstr += "\", \"result\":\"SU\"}";
                    }
                    else {
                        resstr += "\", \"result\":\"FA\"}";
                    }
                }

                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_NEWCLASS) {
                resstr = "{\"infotype\":\"NEWCLASS\"";
                std::string query4compare = "SELECT `id` FROM `ClassTable` WHERE `tusername` = '" + value["tusername"].asString() + "' and `id` = '" +  value["classid"].asString() + "';";
                mysqlpp::StoreQueryResult res4existence = ojsql.store(query4compare);
                if(res4existence.num_rows() >= 1) {
                    resstr += ", \"result\":\"FA\"}";
                }
                else {
                    std::string query4newclass = "INSERT INTO `ClassTable` VALUES('" + value["tusername"].asString() + "', '" + value["classid"].asString() + "');";
                    bool res = ojsql.exec(query4newclass);
                    if(res) {
                        resstr += ", \"result\":\"SU\"}";
                    }
                    else {
                        resstr += ", \"result\":\"FA\"}";
                    }
                }
                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADTEACHERCLASS) {
                resstr = "{\"infotype\":\"LOADTEACHERCLASS\", \"classeslist\": [";
                std::string username = value["tusername"].asString();
                std::string query = "SELECT `tusername`, `id` FROM `ClassTable` WHERE `tusername` = '" + username + "';";
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                for(int i = 0; i < res.num_rows(); ++i) {
                    if(i == res.num_rows() - 1) {
                        resstr += "{\"tusername\": \"" + std::string(res[i]["tusername"].c_str()) + "\", \"id\": \"" + std::string(res[i]["id"].c_str()) + "\"}";
                    }
                    else {
                        resstr += "{\"tusername\": \"" + std::string(res[i]["tusername"].c_str()) + "\", \"id\": \"" + std::string(res[i]["id"].c_str()) + "\"}, ";
                    }
                }
                resstr += "]}";
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_DELETECLASS) {
                resstr = "{\"infotype\":\"DELETECLASS\"";
                std::string query4newclass = "DELETE FROM `ClassTable` WHERE `tusername` = '" + value["tusername"].asString() + "' and `id` = '" +  value["classid"].asString() + "';";
                bool res = ojsql.exec(query4newclass);
                if(res) {
                    resstr += ", \"result\":\"SU\"}";
                }
                else {
                    resstr += ", \"result\":\"FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADCLASSINFO) {
                resstr = "{\"infotype\":\"LOADCLASSINFO\", \"classpeoplelist\": [";
                std::string tusername = value["tusername"].asString();
                std::string classid = value["classid"].asString();
                std::string query = "SELECT `username`, `name`, `material` FROM `UsersData` WHERE `classid` = '" + classid + "';";
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                for(int i = 0; i < res.num_rows(); ++i) {
                    if(i == res.num_rows() - 1) {
                        resstr += "{\"username\": \"" + std::string(res[i]["username"].c_str()) + "\", \"name\": \"" + std::string(res[i]["name"].c_str()) + "\", \"material\": \"" + std::string(res[i]["material"].c_str()) + "\"}";
                    }
                    else {
                        resstr += "{\"username\": \"" + std::string(res[i]["username"].c_str()) + "\", \"name\": \"" + std::string(res[i]["name"].c_str()) + "\", \"material\": \"" + std::string(res[i]["material"].c_str()) + "\"}, ";
                    }
                }
                resstr += "]}";
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_ADDCLASSMEMBER) {
                resstr = "{\"infotype\":\"ADDCLASSMEMBER\"";
                std::string query4addmember = "INSERT INTO `UsersData`(`username`, `passwd`, `usertype`, `name`, `material`, `classid`) VALUES('"
                                           + value["username"].asString() + "', '" 
                                           + value["username"].asString() + "12345678', "
                                           + "0, '" 
                                           + value["name"].asString() + "', '"
                                           + value["material"].asString() + "', '"
                                           + value["classid"].asString() + "');";
                std::cout << query4addmember << std::endl;
                bool res = ojsql.exec(query4addmember);
                if(res) {
                    resstr += ", \"result\":\"SU\"}";
                }
                else {
                    resstr += ", \"result\":\"FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_DELETECLASSMEMBER) {
                resstr = "{\"infotype\":\"DELETECLASSMEMBER\"";
                std::string query4deletemember = "UPDATE `UsersData` SET `classid`=\"\" WHERE `username` = '" + value["username"].asString() + "';";
                std::cout << query4deletemember << std::endl;
                bool res = ojsql.exec(query4deletemember);
                if(res) {
                    resstr += ", \"result\":\"SU\"}";
                }
                else {
                    resstr += ", \"result\":\"FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADACHIEVEMENTCLASS) {
                resstr = "{\"infotype\":\"LOADACHIEVEMENTCLASS\", \"classeslist\": [";
                std::string tpid = value["tpid"].asString();
                std::string query = "SELECT `classid` FROM `NowExam` WHERE `tpid` = '" + tpid + "';";
                std::cout << query << std::endl;
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                for(int i = 0; i < res.num_rows(); ++i) {
                    if(i == res.num_rows() - 1) {
                        resstr += "{\"classid\": \"" + std::string(res[i]["classid"].c_str()) + "\"}";
                    }
                    else {
                        resstr += "{\"classid\": \"" + std::string(res[i]["classid"].c_str()) + "\"}, ";
                    }
                }
                resstr += "]}";
                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_LOADACHIEVEMENT) {
                resstr = "{\"infotype\":\"LOADACHIEVEMENT\", \"achievement\": [";
                std::string tpid = value["tpid"].asString();
                std::string classid = value["classid"].asString();
                std::stringstream tmp1;
                tmp1 << value["snumber"].asString();
                int snumber;
                tmp1 >> snumber;
                std::stringstream tmp2;
                tmp2 << value["jnumber"].asString();
                int jnumber;
                tmp2 >> jnumber;
                std::stringstream tmp3;
                tmp3 << value["cnumber"].asString();
                int cnumber;
                tmp3 >> cnumber;
                std::string query = "SELECT UsersData.username, UsersData.name, UsersData.material, ";
                for(int i = 1; i <= snumber; ++i) {
                    if(i != snumber) {
                        query += "`" + std::to_string(i) + "_select`, ";
                    }
                    else {
                        query += "`" + std::to_string(i) + "_select` ";
                    }
                }
                if(snumber > 0) {
                    query += ", ";
                }
                for(int i = 1; i <= jnumber; ++i) {
                    if(i != jnumber) {
                        query += "`" + std::to_string(i) + "_judge`, ";
                    }
                    else {
                        query += "`" + std::to_string(i) + "_judge` ";
                    }
                }
                if(jnumber > 0) {
                    query += ", ";
                }
                for(int i = 1; i <= cnumber; ++i) {
                    if(i != cnumber) {
                        query += "`" + std::to_string(i) + "_code`, ";
                    }
                    else {
                        query += "`" + std::to_string(i) + "_code` ";
                    }
                }
                query += "FROM `" + tpid + "_answer`, `UsersData` WHERE UsersData.username = " 
                      + tpid + "_answer.username and " + tpid + "_answer.classid = '" + classid + "';";
                std::cout << query << std::endl;
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                for(int i = 0; i < res.num_rows(); ++i) {
                    if(i == res.num_rows() - 1) {
                        resstr += "{\"username\": \"" + std::string(res[i]["username"].c_str())
                               + "\", \"name\": \"" + std::string(res[i]["name"].c_str())
                               + "\", \"material\": \"" + std::string(res[i]["material"].c_str()) + "\"";
                        for(int j = 1; j <= snumber; ++j) {
                            std::string tmpname = std::to_string(j) + "_select";
                            resstr += ", \"" + tmpname + "\": \"" + std::string(res[i][tmpname.c_str()].c_str()) + "\"";
                        }
                        for(int j = 1; j <= jnumber; ++j) {
                            std::string tmpname = std::to_string(j) + "_judge";
                            resstr += ", \"" + tmpname + "\": \"" + std::string(res[i][tmpname.c_str()].c_str()) + "\"";
                        }
                        for(int j = 1; j <= cnumber; ++j) {
                            std::string tmpname = std::to_string(j) + "_code";
                            std::string code = std::string(res[i][tmpname.c_str()].c_str());
                            std::replace(code.begin(), code.end(), '"', '`');
                            for(auto it = code.begin(); it != code.end(); ++it) {
                                if(*it == '\n') {
                                    *it = '~';
                                }
                            }
                            resstr += ", \"" + tmpname + "\": \"" + code + "\"";
                        }
                        resstr += "}";
                    }
                    else {
                        resstr += "{\"username\": \"" + std::string(res[i]["username"].c_str())
                               + "\", \"name\": \"" + std::string(res[i]["name"].c_str())
                               + "\", \"material\": \"" + std::string(res[i]["material"].c_str()) + "\"";
                        for(int j = 1; j <= snumber; ++j) {
                            std::string tmpname = std::to_string(j) + "_select";
                            resstr += ", \"" + tmpname + "\": \"" + std::string(res[i][tmpname.c_str()].c_str()) + "\"";
                        }
                        for(int j = 1; j <= jnumber; ++j) {
                            std::string tmpname = std::to_string(j) + "_judge";
                            resstr += ", \"" + tmpname + "\": \"" + std::string(res[i][tmpname.c_str()].c_str()) + "\"";
                        }
                        for(int j = 1; j <= cnumber; ++j) {
                            std::string tmpname = std::to_string(j) + "_code";
                            std::string code = std::string(res[i][tmpname.c_str()].c_str());
                            std::replace(code.begin(), code.end(), '"', '`');
                            for(auto it = code.begin(); it != code.end(); ++it) {
                                if(*it == '\n') {
                                    *it = '~';
                                }
                            }
                            resstr += ", \"" + tmpname + "\": \"" + code + "\"";
                        }
                        resstr += "}, ";
                    }
                }
                resstr += "]}";
                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_RLOADTEACHER) {
                resstr = "{\"infotype\":\"RLOADTEACHER\", \"teacherlist\": [";
                std::string tusername = value["tusername"].asString();
                std::string classid = value["classid"].asString();
                std::string query = "SELECT `username`, `name`, `material` FROM `UsersData` WHERE `usertype` = 1;";
                mysqlpp::StoreQueryResult res = ojsql.store(query);
                for(int i = 0; i < res.num_rows(); ++i) {
                    if(i == res.num_rows() - 1) {
                        resstr += "{\"username\": \"" + std::string(res[i]["username"].c_str()) + "\", \"name\": \"" + std::string(res[i]["name"].c_str()) + "\", \"material\": \"" + std::string(res[i]["material"].c_str()) + "\"}";
                    }
                    else {
                        resstr += "{\"username\": \"" + std::string(res[i]["username"].c_str()) + "\", \"name\": \"" + std::string(res[i]["name"].c_str()) + "\", \"material\": \"" + std::string(res[i]["material"].c_str()) + "\"}, ";
                    }
                }
                resstr += "]}";
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_ADDRTEACHER) {
                resstr = "{\"infotype\":\"ADDRTEACHER\"";
                std::string query4addmember = "INSERT INTO `UsersData`(`username`, `passwd`, `usertype`, `name`, `material`) VALUES('"
                                           + value["username"].asString() + "', '" 
                                           + value["username"].asString() + "12345678', "
                                           + "1, '" 
                                           + value["name"].asString() + "', '"
                                           + value["material"].asString() + "');";
                std::cout << query4addmember << std::endl;
                bool res = ojsql.exec(query4addmember);
                if(res) {
                    resstr += ", \"result\":\"SU\"}";
                }
                else {
                    resstr += ", \"result\":\"FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_RDELETETEACHER) {
                resstr = "{\"infotype\":\"RDELETETEACHER\"";
                std::string query4deletemember = "DELETE FROM `UsersData` WHERE `username` = '" + value["username"].asString() + "';";
                std::cout << query4deletemember << std::endl;
                bool res = ojsql.exec(query4deletemember);
                if(res) {
                    resstr += ", \"result\":\"SU\"}";
                }
                else {
                    resstr += ", \"result\":\"FA\"}";
                }
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
            else if(value["infotype"].asString() == OPT_EXECUTEROOTCODE) {
                if(value["option"].asString() == "0") {
                    resstr = "{\"infotype\":\"EXECUTEROOTCODE\", " + ojsql.rootstore(value["code"].asString()) + "}";
                }
                else {
                    resstr = "{\"infotype\":\"EXECUTEROOTCODE\", " + ojsql.rootexec(value["code"].asString()) + "}";
                }
                std::cout << resstr << std::endl;
                evbuffer_add(outbuff, resstr.c_str(), resstr.size());
                evhttp_send_reply(request, HTTP_OK, "", outbuff);
            }
        }
    }
    std::cout << "==============================" << std::endl;

    // 2.回复浏览器
    // 状态行 消息报头 响应正文 HTTP_NOTFOUND HTTP_INTERNAL
    // 分析出请求的文件 uri
    // 设置根目录 WEBROOT
    std::string filepath = WEBROOT;
    if(strcmp(uri, "/") == 0)
        filepath += DEFAULTINDEX;
    else
        filepath += uri;

    // 消息报头
    evkeyvalq* outhead = evhttp_request_get_output_headers(request);
    // 要支持图片、css、js、下载zip文件
    // 获取文件名后缀
    int pos = filepath.rfind('.');
    std::string postfix = filepath.substr(pos + 1, filepath.size() - (pos + 1));
    std::string tmp;
    if(postfix == "jpg" || postfix == "gif" || postfix == "png")
        tmp = "image/" + postfix;
    else if(postfix == "html")
        evhttp_add_header(outhead, "Content-Type", "text/html; charset=UTF8");
    else if(postfix == "css" || postfix == "js" || postfix == "plain" || postfix == "xml")
        tmp = "text/" + postfix;
    else if(postfix == "zip")
        tmp = "application/" + postfix;
    evhttp_add_header(outhead, "Content-Type", tmp.c_str());
    
    FILE* fp = fopen(filepath.c_str(), "rb");
    if(!fp) {
        evhttp_send_reply(request, HTTP_NOTFOUND, "", 0);
        return;
    }
    evbuffer* outbuf = evhttp_request_get_output_buffer(request);
    for(;;) {
        int len = fread(buf, 1, sizeof(buf), fp);
        if(len <= 0) break;
        evbuffer_add(outbuf, buf, len);
    }
    fclose(fp);
    evhttp_send_reply(request, HTTP_OK, "", outbuf);
}

namespace EKOJ {
class OJServer {
public:
    OJServer();
    ~OJServer();
    void run();

private:
    event_base* m_base;
    evhttp* m_evh;
};
}