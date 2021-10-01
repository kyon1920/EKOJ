#include "OJ.h"

EKOJ::OJ::OJ(const std::string& fname, const std::string& qname, const int& times, const int& memory,
    const bool& corcpp, const unsigned int& number)
        : m_ojfname(fname), m_ojqname(qname), m_timelimit(times), m_memlimit(memory),
            m_corcpp(corcpp), m_number(number) {}

EKOJ::OJ::~OJ() {}

void EKOJ::OJ::OJCompile(std::string CompilationOptions) {
    if(m_corcpp) {
        std::string cmd_c = "gcc -o ./out/" + m_ojqname + ".out "
            + "./question/" + m_ojqname + ".c " + " -O2 "
            + CompilationOptions + " &> ./tmp/" + m_ojqname + ".tmp";
        system(cmd_c.c_str());
    }
    else {
        std::string cmd_cc = "g++ -o ./out/" + m_ojqname + ".out "
            + "./question/" + m_ojqname + ".cc " + " -O2 "
            + CompilationOptions + " &> ./tmp/" + m_ojqname + ".tmp";
        system(cmd_cc.c_str());
    }
}

bool EKOJ::OJ::OJExec() {
    std::string rfilename = "./out/" + m_ojqname + ".out";
    std::ifstream rfile(rfilename.c_str());
    if(!rfile.good())  {
        std::cout << "编译结果文件不存在" << std::endl;
        return false;
    }
    else {
        for(int i = 0; i < m_number; ++i) {
            /* 打包镜像 */
            std::string dockerquery =  "FROM gcc:latest\n"
                "RUN mkdir /usr/src/myapp\n"
                "COPY ./out/" + m_ojqname + ".out /usr/src/myapp\n"
                "COPY ./in/" + m_ojfname + '_' + std::to_string(i) + ".in /usr/src/myapp\n"
                "COPY ./dockerfile/OJCore /usr/src/myapp\n"
                "WORKDIR /usr/src/myapp\n"
                "CMD [\"./OJCore\", \"./" + m_ojqname + ".out\", \"" 
                + std::to_string(m_timelimit) + "\", \"" + std::to_string(m_memlimit) 
                + "\", \"./" + m_ojfname + '_' + std::to_string(i) + ".in" + "\"]";

            std::string dockerfilename = "./dockerfile/" + m_ojqname + ".dockerfile";
            std::ofstream dockerfile(dockerfilename.c_str(), std::ios::out);
            dockerfile << dockerquery;
            dockerfile.close();

            std::string buildimage = "docker build -f " + dockerfilename + " -t image-" 
                + m_ojqname + "_" + std::to_string(i) + " .";
            system(buildimage.c_str());

            /* 运行镜像并保存结果 */
            std::string execimage = "sh OJImage.sh " + m_ojqname + "_" + std::to_string(i);
            system(execimage.c_str());
        }
        return true;
    }
}

std::pair<EKOJ::OJRE, std::string> EKOJ::OJ::OJudge() {
    bool resoj = OJExec();
    if(!resoj) {
        std::ifstream ftmp;
        std::string fjudgename = "./tmp/" + m_ojqname + ".tmp";
        ftmp.open(fjudgename.c_str(), std::ios::in);
        std::stringstream buffer;
        buffer << ftmp.rdbuf();
        std::string fcontent(buffer.str());
        OJRE ojretmp;
        ojretmp.status = CE;
        ojretmp.memory = 0;
        ojretmp.time = 0;
        ojretmp.ratio = 0;
        m_ojresult = std::make_pair(ojretmp, fcontent);
        ftmp.close();
        return m_ojresult;
    }
    else {
        int totlerows = 0;
        int correctrows = 0;
        int resultrows = 0;
        OJRE ojretmp;

        for(int i = 0; i < m_number; ++i) {
            std::string answer = "./answer/" + m_ojfname + '_' + std::to_string(i) + ".out";
            std::string result = "./result/" + m_ojqname + '_' + std::to_string(i) + ".result";
            
            std::ifstream tmp(result);
            std::string lastdata;
            while(getline(tmp, lastdata)) {
                resultrows++;
            }
            tmp.close();

            std::vector<std::string> res = split(lastdata, ":");
            if(res[0] == "0") {
                std::ifstream fanswer(answer);
                std::ifstream fresult(result);
                std::string readline4answer;
                std::string readline4result;
                while(getline(fanswer, readline4answer)) {
                    totlerows++;
                    if(getline(fresult, readline4result)) {
                        if(readline4result == readline4answer) {
                            correctrows++;
                        }
                    }
                }
                
                fanswer.close();
                fresult.close();

                ojretmp.status = 0;
                std::stringstream times, memorys;
                times << res[1];
                memorys << res[2];
                if(i == 0) {
                    times >> ojretmp.time;
                    memorys >> ojretmp.memory;
                }
                else {
                    int tmp1;
                    times >> tmp1;
                    ojretmp.time = std::min(tmp1, ojretmp.time);
                    
                    int tmp2;
                    memorys >> tmp2;
                    ojretmp.memory = std::min(tmp2, ojretmp.memory);
                }
            }
            else if(res[0] == "2") {
                ojretmp.status = 2;
                std::stringstream times, memorys;
                times << res[1];
                memorys << res[2];
                times >> ojretmp.time;
                memorys >> ojretmp.memory;
                ojretmp.ratio = 0;
                m_ojresult = std::make_pair(ojretmp, "运行超时！");
                return m_ojresult;
            }
            else if (res[0] == "1" || res[0] == "3" || res[0] == "4" || res[0] == "5" || res[0] == "6") {
                ojretmp.status = 1;
                std::stringstream times, memorys;
                times << res[1];
                memorys << res[2];
                times >> ojretmp.time;
                memorys >> ojretmp.memory;
                ojretmp.ratio = 0;
                m_ojresult = std::make_pair(ojretmp, "结果错误！");
                return m_ojresult;
            }
        }
        if(resultrows > m_number + totlerows + 10) {
            totlerows += m_number * 10;
        }
        ojretmp.ratio = (correctrows * 1.0) / (totlerows * 1.0);
        std::string fcontent;
        if(ojretmp.ratio < 0.9) {
            ojretmp.status = 8;
            fcontent = "部分正确~";
        }
        else {
            ojretmp.ratio = 1;
            fcontent = "正确~";
        }
        m_ojresult = std::make_pair(ojretmp, fcontent);
        return m_ojresult;
    }
}

std::pair<EKOJ::OJRE, std::string> EKOJ::OJ::run() {
    OJCompile();
    return OJudge();
}

void EKOJ::OJ::clear() {
    std::string dockerfile = "./dockerfile/" + m_ojqname + ".dockerfile";
    unlink(dockerfile.c_str());
    std::string outfile = "./out/" + m_ojqname + ".out";
    unlink(outfile.c_str());
    std::string tmpfile = "./tmp/" + m_ojqname + ".tmp";
    unlink(tmpfile.c_str());
    for (int i = 0; i < m_number; ++i) {
        /* std::string answer = "./answer/" + m_ojfname + '_' + std::to_string(i) + ".out";
        unlink(answer.c_str()); */
        std::string result = "./result/" + m_ojqname + '_' + std::to_string(i) + ".result";
        unlink(result.c_str());
    }
}