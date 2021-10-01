#include "OJ.h"

int main() {
    const std::string fname = "test";
    const std::string qname = "testxxx";
    EKOJ::OJ tmp(fname, qname, 1000, 65535, true, 3);
    std::pair<EKOJ::OJRE, std::string> result = tmp.run();
    std::cout << result.first.status << ":" << result.first.time << ":" << result.first.memory << ":" << result.first.ratio << '\n' << result.second << std::endl;
    tmp.clear();
}