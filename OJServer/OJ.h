#pragma once

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <sstream>
#include <cstring>
/*
 * @file OJ.h
 * @author Saint-EK
 * @brief OJCore Online Judge 核心类
 * @date 2021-3-2
 */

namespace EKOJ {

// 字符串分割
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

const unsigned short AC = 0;    /* Accepted 通过 */
const unsigned short PE = 1;    /* Presentation Error 格式错误 */
const unsigned short TLE = 2;   /* Time Limit Exceed 超时 */
const unsigned short MLE = 3;   /* Memory Limit Exceed 超内存 */
const unsigned short WA = 4;    /* Wrong Answer 答案错误 */
const unsigned short RE = 5;    /* Runtime Error 运行时错误 */
const unsigned short OLE = 6;   /* Output Limit Exceed 超过输出限制 */
const unsigned short CE = 7;    /* Compile Error 无法编译 */
const unsigned short SE = 8;    /* 答案部分正确 */

typedef struct result {
    ushort status;
    int time;
    int memory;
    float ratio;
} OJRE;

class OJ {
public:
    OJ(const std::string& fname, const std::string& qname, const int& times, const int& memory,
        const bool& corcpp, const unsigned int& number);
    ~OJ();

    /* @brief: Online Judge 编译函数 */
    void OJCompile(std::string CompilationOptions = "");
    /* @brief: 设置时间、内存限制 */
    void setProcessLimit(int time_limit, int memory_limit);
    /* @brief: Online Judge 运行函数 */
    bool OJExec();
    /*
     * @brief: OJudge 答案判断
     * @return: 答案类型代码（包括正确、错误、超时、内存过载......）
     */
    std::pair<OJRE, std::string> OJudge();
    std::pair<OJRE, std::string> run();
    void clear();

private:
    std::string m_ojfname;      /* OJ题名 */
    std::string m_ojqname;      /* 用户提交文件名称 */
    bool m_corcpp;              /* OJ程序语言类型 */
    int m_memlimit;             /* OJ程序运行内存限制 ms */
    int m_timelimit;            /* OJ程序运行时间限制 b */
    unsigned int m_number;      /* 测试时输入文件数量 */
    std::pair<OJRE, std::string> m_ojresult;    /* OJ结果返回 */
};

}

