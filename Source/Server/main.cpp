#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <future>
#include <regex>
#include "class.h"
#include "Config.h"
#include "Guard.h"

using namespace std;

void function_1()
{
	std::cout << "Beauty is only skin-deep" << std::endl;
	while(true)
	{
		//int i = 0;
	}
}

class LogFile
{
	std::mutex _mu;
	ofstream _f;

public:
	LogFile()
	{
		_f.open("log.txt");
	}

	~LogFile()
	{

	}
	void shared_print(std::string msg, int id)
	{
		std::lock_guard<mutex> locker(_mu);
		_f << "From " << id << ": " << msg << endl;
	}

private:

};

class Factorial
{
public:

	int operator()(int num)
	{
		for (int i = 1; i <= num; i++)
		{
			num = num * i;
		}
		return num;
	}

	int memberfunc(int num)
	{
		return this->operator()(num);
	}
private:

};

int factorial_shared_future(std::shared_future<int> sfu)
{
    auto num = sfu.get();
    for (int i = 1; i <= num; i++)
    {
        num = num * i;
    }
    return num;
}

int factorial_future(std::future<int>& fu)
{
    auto num = fu.get();
    for (int i = 1; i <= num; i++)
    {
        num = num * i;
    }
    return num;
}

int factorial(int num)
{
	for (int i = 1; i <= num; i++)
	{
		num = num * i;
	}
	return num;
}

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE static void TOKENPASTE2(scope_guard, __LINE__)(void) {}
UNIQUE;
UNIQUE;
UNIQUE;


int main()
{
	std::mutex m;
	SCOPED(m)
	{
		static int i = 0;
		i++;
		if (i > 3)
		{
			return;
		}
	};

	SCOPED(m)
	{
		static int i = 0;
		i++;
		if (i > 3)
		{
			return;
		}
	};

	GConfig;

	SCOPED2
	{
		// revert step 1
		printf("123");
	};
	SCOPED2
	{
		// revert step 1
		printf("123");
	};


	Factorial f;
    bool should_cont = true;
    
	/* thread invocation template */
	std::thread t1(f, 6); //copy of f invoked in new thread
	std::thread t2(std::ref(f), 6); // ref of f in main thread
	std::thread t3(std::move(f), 6); // turn f into a rvalue ref and move construct it in new thread
	std::thread t4(Factorial(), 6); // temp factorial in both main and new thread (? unsure)
	std::thread t5([](int x) { return x*x; }, 6); // call lambda
	std::thread t6(factorial, 6); // lower case factorial, global function
	std::thread t7(&Factorial::memberfunc, f, 6); // memberfunc invoked on a copy of f
	std::thread t8(&Factorial::memberfunc, &f, 6); // memberfunc invoked on ref of f


	/* mutex */
	std::mutex mu;
    mu.lock(); // manual lock & unlock is highly discouraged
    mu.unlock(); // due to possible runtime exception.
    
	{
        // RAII lock guard
		std::lock_guard<mutex> locker(mu);
	}
    
    std::unique_lock<mutex> ul(mu); // locks on initialization.
    std::unique_lock<mutex> ul2(mu, std::defer_lock); // defer loc

	/* condition variable */
	std::condition_variable cond;
    cond.wait(ul, [&]{return should_cont;} ); // must be locked before wait

	/* future and promise */
	std::promise<int> p;
    std::future<int> fu = p.get_future(); //fu.get() should be called by the async func
    std::future<int> fu2 = async(std::launch::deferred, factorial_future, std::ref(fu)); // defer launch until get() is called
    p.set_value(6); // send future to deferred async func
    //p.set_exception(std::make_exception_ptr(std::runtime_error("Cannot pass argument."))); // set exception when argument cannot be passed
    fu2.get();
    
    std::promise<int> p1;
    std::future<int> fu_1 = p1.get_future(); //fu.get() should be called by the async func
    std::shared_future<int> sfu = fu_1.share(); // shared_future can be copied.
    std::future<int> fu2_1 = async(std::launch::deferred, factorial_shared_future, sfu); // defer launch until get() is called
    std::future<int> fu2_2 = async(std::launch::deferred, factorial_shared_future, sfu); // defer launch until get() is called
    p1.set_value(6);
    fu2_1.get();
    fu2_2.get();
    
	/* async() */
    std::future<int> fu3 = async(factorial, 6);
    std::future<int> fu4 = async(std::launch::deferred, factorial, 6); // defer launch until get() is called
    fu3.get();
    fu4.get(); // call factorial since it's been deferred

	/* packaged task */
	std::packaged_task<int(int)> t(factorial);
	std::future<int> fu5 = t.get_future();
	t(6);


	cout << std::this_thread::get_id() << endl;

	string str = "idk what to do";
	//std::thread t1((Functor()), std::move(str));
	//cout << t1.get_id() << endl;

	cout << std::thread::hardware_concurrency() << endl; // max threads on current system

	//t1.join(); // main thread waits for t1 to finish
	//t1.detach(); //t1 will be free, -- daemon process

	// RAII
	// Wrapper w(t1) with destructor that calls join
	try
	{
		for(int i = 0; i < 3; i++)
		{
			cout << "from main: " << str.size() << endl;
		}
	}
	catch(...)
	{
		t1.join();
		throw;
	}

	if(t1.joinable())
	{
		t1.join(); // will crash without test
	}
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();

	string str1;

	/* regex */
	regex e1("abc.", regex_constants::icase); // abcc abcd, ...
	regex e2("abc?"); // ab or abc 
	regex e3("abc*"); // ab, abc, abcc, abccc, ...
	regex e4("abc+"); // abc, abcc, abccc, ...
	regex e5("ab[cd]*"); // abddcdcdcddcdcdcdccccdcdd
	regex e6("ab[^cd]*"); // abwotiwutr
	regex e7("ab[cd]{3,5}"); // 3 to 5 c's and d's
	regex e8("abc|de[\]fg]"); // abc or de + and combination of ]fg
	regex e9("(abc)de+\\1"); // first group
	regex e10("(ab)c(de+)\\2\\1"); // 2 groups
	regex e11("[[:w:]]+@[[:w:]]+\\.com"); // email format


	while(1)
	{
		cin >> str1;
		bool match = regex_match(str1, e6);
		cout << (match ? "Matched" : "Not Matched") << endl << endl;
	}
	return 0;
}