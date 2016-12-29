#pragma once
#include "Singleton.h"


namespace cd
{
	class CDAPIModule final : public Singleton<CDAPIModule>
	{
		DECL_SINGLETON(CDAPIModule);
	public:
		bool IsReady() { return true; }

	private:
		CDAPIModule();
		~CDAPIModule() = default;
	};
}
