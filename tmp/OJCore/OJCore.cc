#include "OJCore.h"

EKOJ::EKOJ(char *args[], int timelimit, int memlimit, std::string in)
    :m_args(args), m_timelimit(timelimit), m_memlimit(memlimit), m_in(in) {}

void EKOJ::setProcessLimit() {
    struct rlimit rl;
    /* 限制运行时间 (second) */
    rl.rlim_cur = m_timelimit / 1000;
    rl.rlim_max = rl.rlim_cur + 1;
    setrlimit(RLIMIT_CPU, &rl);
 
    /* 限制运行内存 (b) */
    rl.rlim_cur = m_memlimit * 1024;
    rl.rlim_max = rl.rlim_cur + 1024;
    setrlimit(RLIMIT_DATA, &rl);
}

void EKOJ::cmd() {
    int newstdin = open(m_in.c_str(), O_RDWR|O_CREAT, 0644);
    setProcessLimit();
    if(newstdin != -1) {
        dup2(newstdin, fileno(stdin));
        if (execvp(m_args[0], m_args) == -1) {
            std::cout << "Failed to start the process!" << std::endl;
        }
        close(newstdin);
    }
    else {
        std::cout << "Failed to open file!" << std::endl;
    }
}

void EKOJ::OJMonitor(pid_t pid) {
    int status;
    struct rusage ru;
    m_res.memory = get_proc_mem(pid);

    if (wait4(pid, &status, 0, &ru) == -1)
        std::cout << "wait4 failure" << std::endl;
    m_res.time = ru.ru_utime.tv_sec * 1000
            + ru.ru_utime.tv_usec / 1000
            + ru.ru_stime.tv_sec * 1000
            + ru.ru_stime.tv_usec / 1000;

    if (WIFSIGNALED(status)) {
        switch (WTERMSIG(status)) {
            case SIGSEGV:
                if (m_res.memory > m_memlimit)
                    m_res.status = MLE;
                else
                    m_res.status = RE;
                break;
            case SIGALRM:
            case SIGXCPU:
                m_res.status = TLE;
                break;
            default:
                m_res.status = RE;
                break;
        }
    }
    else {
        if (m_res.time > m_timelimit) m_res.status = TLE;
        else if (m_res.memory > m_memlimit) m_res.status = MLE;
        else m_res.status = AC;
    }
}

void EKOJ::run() {
    pid_t pid = vfork();
    if(pid < 0)
        std::cout << "error in fork!" << std::endl;
    else if(pid == 0)
        cmd();
    else {
        OJMonitor(pid);
        std::cout << '\n' << m_res.status << ":" << m_res.time << ":" << m_res.memory;
    }
}

int main(int argc, char *argv[]) {
    char *cmd[20];
    split(cmd, argv[1], "*");
    EKOJ oj(cmd, atoi(argv[2]), atoi(argv[3]), argv[4]);
    oj.run();
}