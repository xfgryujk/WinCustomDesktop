#pragma once


namespace cd
{
	template<class T>
	class Singleton
	{
	protected:
		Singleton() = default;
		virtual ~Singleton() = default;

	public:
		static T& GetInstance()
		{
			static T s_instance;
			return s_instance;
		}
	};

#define DECL_SINGLETON(T) friend class Singleton<T>
#define DECL_SINGLETON_DEFAULT(T) \
	DECL_SINGLETON(T); \
	private: \
	T() = default; \
	~T() = default
}
