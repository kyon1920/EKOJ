#include "OJThreadPool.h"
#include <iostream>
#include <chrono>

int main() {
	EKOJ::ThreadPool threadpool(4);
	std::vector<std::future<int>> results;

	for (int i = 0; i < 8; ++i) {
		// 设定线程池工作任务
		results.emplace_back(
			threadpool.enqueue([i]() {
					std::cout << "Hello " << i << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds(1));
					std::cout << " World! " << i << std::endl;
					return i * i;
				})
		);
	}

	for (auto&& result : results)
		std::cout << result.get() << ' ';
	std::cout << '\n';
	return 0;
}

/* 
#include <iostream>
#include <unistd.h>
#include "ThreadPool.h"
using namespace std;
void task1(){
    while(1){
        cout<<"task1 thread ID:"<<this_thread::get_id()<<endl;
        sleep(1);
    }
}
 
void task2(){
    while(1){
        cout<<"task2 thread ID:"<<this_thread::get_id()<<endl;
        sleep(2);
    }
}
 
int main()
{
    ThreadPool pool(4);
    pool.enqueue(task1);
    pool.enqueue(task2);
}
*/