#pragma once

#define DECLARE_SINGLETON(T) \
private: \
template<class U> friend class Singleton; \
T() = default; \
~T() = default; \
T(T const&) = delete; \
T& operator=(T const&) = delete;

template<typename T>
class Singleton {
public:

	static T& Instance()
	{
		static T Instance;
		return Instance;
	}
};

class Singleton2
{
private:
protected:
	Singleton2() = default;
	virtual ~Singleton2() = default;
	Singleton2(Singleton2 const&) = delete;
	Singleton2& operator=(Singleton2 const&) = delete;

public:
	//static Singleton& Instance();

};
//#define GLoginService Singleton::Instance()
//static_assert(std::is_pod<LoginService>::value, "LoginService is not POD!");


template<typename T>
class Singleton3 {
public:
	static T& Instance();
	static void Destroy();

protected:
	inline explicit Singleton3() {
		assert(Singleton3::instance_ == 0);
		Singleton3::instance_ = static_cast<T*>(this);
	}
	inline ~Singleton3() {
		Singleton3::instance_ = 0;
	}

private:
	static T* CreateInstance();
	static void ScheduleForDestruction(void(*)());
	static void DestroyInstance(T*);

private:
	static T* instance_;

private:
	inline explicit Singleton3(Singleton3 const&) {}
	inline Singleton3& operator=(Singleton3 const&) { return *this; }
};    //    end of class Singleton

template<typename T>
typename T& Singleton3<T>::Instance() {
	if (Singleton3::instance_ == 0) {
		Singleton3::instance_ = CreateInstance();
		ScheduleForDestruction(Singleton3::Destroy);
	}
	return *(Singleton3::instance_);
}

template<typename T>
void Singleton3<T>::Destroy() {
	if (Singleton3::instance_ != 0) {
		DestroyInstance(Singleton3::instance_);
		Singleton3::instance_ = 0;
	}
}

template<typename T>
inline typename T* Singleton3<T>::CreateInstance() {
	return new T();
}

template<typename T>
inline void Singleton3<T>::ScheduleForDestruction(void(*pFun)()) {
	std::atexit(pFun);
}

template<typename T>
inline void Singleton3<T>::DestroyInstance(T* p) {
	delete p;
}

template<typename T>
typename T* Singleton3<T>::instance_ = 0;