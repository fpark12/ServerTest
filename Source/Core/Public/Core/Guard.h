#include <functional>

#define LOCK__(x, y)  x ## y
#define LOCK_(x, y) LOCK__(x, y)
#define LOCK2 LockGuard LOCK_(lock, __LINE__) = []()
#define SCOPED_LOCK(x) LockGuard LOCK_(lock, __LINE__); LOCK_(lock, __LINE__) << &x << [&]()

#if PLATFORM_WINDOWS
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif

typedef void(*LambdaFuncType)();

struct LockGuard
{
	/*
	template<class Callable>
	LockGuard(Callable && LambdaFunc) : Lambda(std::forward<Callable>(LambdaFunc))
	{
		if (Lambda) Lambda(); // must not throw
	}

	LockGuard(LockGuard && Other):  Lambda(std::move(Other.Lambda))
	{
		Other.Lambda = nullptr;
	}

	LockGuard() = default;
	~LockGuard() = default;
	void dismiss() throw()
	{
		Lambda = nullptr;
	}
	*/

	FORCEINLINE LockGuard& operator<< (std::mutex* Mutex)
	{
		this->Mutex = Mutex;
		return *this;
	}

	template<class Callable>
	FORCEINLINE void operator<< (Callable Lambda)
	{
		Mutex->lock();
		Lambda();
		Mutex->unlock();
		//return *this;
	}

/*
	LockGuard& operator<< (std::function<void()> Lambda)
	{
		this->Lambda = Lambda;
		if (Mutex && Lambda)
		{
			Lambda();
		}
		return *this;
	}
*/

	//LockGuard(const LockGuard&) = delete;
	//void operator = (const LockGuard&) = delete;

	std::mutex* Mutex;
	//std::function<void()> Lambda;

};
