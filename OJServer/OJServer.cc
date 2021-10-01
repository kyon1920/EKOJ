#include "OJServer.h"

EKOJ::OJServer::OJServer() {}

void EKOJ::OJServer::run() {
#ifdef _WIN32
	// 初始化Socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else
	// 忽略管道信号发送数据给已关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		return;
	}
#endif

	// 创建libevent的上下文
	m_base = event_base_new();
	if (m_base) {
		std::cout << "event_base_new success!" << std::endl;
	}

    // HTTP 服务器
    // 1. 创建evhttp上下文
    m_evh = evhttp_new(m_base);

    // 2.绑定端口和IP
    if(evhttp_bind_socket(m_evh, "0.0.0.0", 8080)) {
        std::cerr << "evhttp_bind_socket failed!" << std::endl;
    }

    // 3.设定回调函数
    evhttp_set_gencb(m_evh, http_cb, 0);

	// 事件分发处理
	if (m_base) {
		event_base_dispatch(m_base);
	}
}

EKOJ::OJServer::~OJServer() {
	if (m_base) {
		event_base_free(m_base);
	}
    if(m_evh) {
        evhttp_free(m_evh);
    }

#ifdef _WIN32
	WSACleanup();
#else
#endif
}