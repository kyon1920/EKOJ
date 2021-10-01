#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/resource.h>

const ushort AC = 0;    /* Accepted 通过 */
const ushort PE = 1;    /* Presentation Error 格式错误 */
const ushort TLE = 2;   /* Time Limit Exceed 超时 */
const ushort MLE = 3;   /* Memory Limit Exceed 超内存 */
const ushort WA = 4;    /* Wrong Answer 答案错误 */
const ushort RE = 5;    /* Runtime Error 运行时错误 */
const ushort OLE = 6;   /* Output Limit Exceed 超过输出限制 */
const ushort CE = 7;    /* Compile Error 无法编译 */

typedef struct result {
    ushort status;
    int time;
    int memory;
} OJRE;

// 字符串分割
static void split(char **arr, char *str, const char *del) {
    char* s = NULL;
    s = strtok(str, del);
    while(s != NULL) {
        *arr++ = s;
        s = strtok(NULL, del);
    }
    *arr++ = NULL;
}

// 获取进程占用内存
static unsigned int get_proc_mem(unsigned int pid){
	char file_name[64] = {0};
	FILE *fd;
	char line_buff[512] = {0};
	sprintf(file_name, "/proc/%d/status", pid);
	
	fd = fopen(file_name, "r");
	if(nullptr == fd)
		return 0;
	
	char name[64];
	int vmrss;
	for (int i = 0; i < 16; ++i)
		fgets(line_buff, sizeof(line_buff), fd);
        
	fgets(line_buff, sizeof(line_buff), fd);
	sscanf(line_buff, "%s %d", name, &vmrss);
	fclose(fd);

	return vmrss;
}

/*
 * @file OJ.h
 * @author Saint-EK
 * @brief OJCore Online Judge 核心类
 * @date 2021-3-4
 */

class EKOJ {
public:
    /* @brief 构造函数
     * @param[in] args 命令行参数，timelimit 时间限制，memlimit 内存限制，in 输入文件，out 输出文件
     */
    EKOJ(char *args[], int timelimit, int memlimit, std::string in);
    void setProcessLimit();
    void cmd();
    void OJMonitor(pid_t pid);
    void run();

private:
    int m_timelimit;
    int m_memlimit;
    std::string m_in;
    char** m_args;
    OJRE m_res;
};