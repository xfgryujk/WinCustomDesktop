#pragma once
#include "Singleton.h"
#include <thread>
#include <memory>


namespace cd
{
	// 检测桌面是否被遮挡了
	class CheckCovered final : public Singleton<CheckCovered>
	{
		DECL_SINGLETON(CheckCovered);
	public:
		bool IsReady() { return m_runThreadFlag; }

		bool Init();
		bool Uninit();

	private:
		CheckCovered();
		~CheckCovered();


		std::unique_ptr<std::thread> m_thread;
		bool m_runThreadFlag = true;
		bool m_isCovered = false;
		HWND m_coveredByHwnd = NULL;


		void CheckCoveredThread();
		bool IsDesktopCovered();
	};
}
