
#define LOCK__(x, y)  x ## y
#define LOCK_(x, y) LOCK__(x, y)
#define LOCK2 LockGuard LOCK_(lock, __LINE__) = []()
#define SCOPED_LOCK(x) LockGuard LOCK_(lock, __LINE__); LOCK_(lock, __LINE__) << &x << [&]()

#if PLATFORM_WINDOWS
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif

#define STR_CONCAT(a, b) STR_CONCAT_I(a, b)
#define STR_CONCAT_I(a, b) STR_CONCAT_II(~, a ## b)
#define STR_CONCAT_II(p, res) res

// This is the meat, a static instance of a class whose constructor runs your code
#define STATIC_EXECUTE_I(func_name)               \
static struct func_name {                         \
  func_name();                                    \
} STR_CONCAT(func_name, __var);                 \
func_name::func_name()  // followed by { ... }

#define STATIC_EXECUTE STATIC_EXECUTE_I(STR_CONCAT(__StaticExecute__, __LINE__))
#define BEFORE_MAIN() STATIC_EXECUTE_I(__BeforeMain__)



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
