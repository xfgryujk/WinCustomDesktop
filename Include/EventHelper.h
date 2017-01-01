#pragma once
#include <functional>
#include <map>


namespace cd
{
	class EventBase
	{
	public:
		virtual ~EventBase() = default;

		virtual void DeleteListener(int listenerID) = 0;
		virtual void DeleteListenersOfModule(HMODULE module) = 0;
	};

	template<class KeyCmp, class... ArgTypes>
	class Event final : public EventBase
	{
	public:
		typedef std::function<bool(ArgTypes...)> FunctionType;

	private:
		struct Listener
		{
			FunctionType m_function;
			HMODULE m_module;
		};

		std::map<int, Listener, KeyCmp> m_listeners;
		int m_nextListenerID = 0;


	public:
		// 返回listener ID，如果指定module则在MOD卸载时会自动删除，否则需要手动删除
		int AddListener(FunctionType callback, HMODULE module = NULL)
		{
			int listenerID = m_nextListenerID++;
			Listener& listener = m_listeners[listenerID];
			listener.m_function = std::move(callback);
			listener.m_module = module;
			return listenerID;
		}

		void DeleteListener(int listenerID)
		{
			m_listeners.erase(listenerID);
		}

		void DeleteListenersOfModule(HMODULE module)
		{
			for (auto it = m_listeners.begin(); it != m_listeners.end();)
			{
				if (it->second.m_module == module)
					it = m_listeners.erase(it);
				else
					++it;
			}
		}

		// 如果事件被取消返回false
		bool operator () (ArgTypes... args)
		{
			bool res = true;
			for (const auto& i : m_listeners)
			{
				// 按原来的写法32位版会被迷之优化掉，返回false后面的函数不会被调用...
				if (!i.second.m_function(std::forward<ArgTypes>(args)...))
					res = false;
			}
			return res;
		}
	};

	// 先监听先被调用
	template<class... ArgTypes>
	using PreEvent = Event<std::less<int>, ArgTypes...>;

	// 先监听后被调用
	template<class... ArgTypes>
	using PostEvent = Event<std::greater<int>, ArgTypes...>;
}
