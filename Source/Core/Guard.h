#include <functional>

#define SCOPED__(x, y)  x ## y
#define SCOPED_(x, y) SCOPED__(x, y)
#define SCOPED2 LockGuard SCOPED_(lock, __LINE__) = []()
#define SCOPED(x) LockGuard SCOPED_(lock, __LINE__); SCOPED_(lock, __LINE__) << &x << [&]()


class LockGuard
{
public:
	template<class Callable>
	LockGuard(Callable && LambdaFunc) : Lambda(std::forward<Callable>(LambdaFunc))
	{
		if (Lambda) Lambda(); // must not throw
	}

	LockGuard(LockGuard && Other):  Lambda(std::move(Other.Lambda))
	{
		Other.Lambda = nullptr;
	}

	LockGuard()
	{
	}

	~LockGuard()
	{
	}

	void dismiss() throw()
	{
		Lambda = nullptr;
	}

	LockGuard& operator<< (std::mutex* Mutex)
	{
		this->Mutex = Mutex;
		return *this;
	}

	LockGuard& operator<< (std::function<void()> Lambda)
	{
		this->Lambda = Lambda;
		if (Mutex && Lambda)
		{
			Lambda();
		}
		return *this;
	}

	LockGuard(const LockGuard&) = delete;
	void operator = (const LockGuard&) = delete;

private:
	std::mutex* Mutex;
	std::function<void()> Lambda;

};
