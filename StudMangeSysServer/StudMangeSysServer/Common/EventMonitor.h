#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdarg.h>
using namespace std;

#define DEF_TYPE_ON_EVENT(className, functionName, type1, type2, type3)		void(__thiscall className::*functionName)(type1, type2, type3)

template <class _TyEvent, class _Type1, typename _Type2, typename _Type3>
class CEventProxy
{
	typedef DEF_TYPE_ON_EVENT(CEventProxy, PFN_ONEVENT, _Type1, _Type2, _Type3);
	struct EventMonitor{
		_TyEvent				Event;
		union {
			void *				pObject;
			CEventProxy *		pMonitor;  
		};
		union {
			void *				pfn;
			PFN_ONEVENT			pfnOnEvent; 
		};
	};
	typedef vector<EventMonitor>		VEventMonitor;

public:
	CEventProxy() {}
	virtual ~CEventProxy() {}

	// 通告事件
	bool DispatchEvent(_TyEvent Event, _Type1 var1, _Type2 var2, _Type3 var3)
	{
		bool bResult = !m_vEventMonitor.empty();
		for (VEventMonitor::iterator it = m_vEventMonitor.begin(); m_vEventMonitor.end() != it; it++)
		{
			if (it->Event == Event )
			{
				(it->pMonitor->*it->pfnOnEvent)(var1, var2, var3);
				break;
			}
		}

		return bResult;
	}

	// 监听事件
	// 模板 不会带来额外开销 仅为编译期间做类型检查 确保安全性
	template <class _TyObj>
	bool MonitorEvent(_TyEvent Event, _TyObj * pMonitor, DEF_TYPE_ON_EVENT(_TyObj, pfn, _Type1, _Type2, _Type3)) { return _MonitorEvent(Event, pMonitor, pfn); }


	// 移除监听
	void RemoveMonitor(void * pMonitor)
	{
		VEventMonitor::iterator it = m_vEventMonitor.begin();
		while (m_vEventMonitor.end() != it)
		{
			if (pMonitor == it->pMonitor)
			{
				it = m_vEventMonitor.erase(it);
			}
		}
	}

	// 清除所有监听
	void ClearMonitor() { m_vEventMonitor.clear(); }

private:
	// 监听事件
	// 变长参数 没有类型检查 不安全 不对外开放
	bool _MonitorEvent(_TyEvent Event, void * pObject, ...)
	{
		bool bResult = false;

		EventMonitor em;
		em.pObject = pObject;
		em.Event = Event;
		va_list ap;
		va_start(ap, pObject);
		em.pfn = va_arg(ap, void *);
		va_end(ap);
		if (em.pObject && em.pfn)
		{
			for (VEventMonitor::const_iterator it = m_vEventMonitor.begin(); m_vEventMonitor.end() != it; it++)
			{
				if (em.Event == it->Event && it->pObject == em.pObject && it->pfn == em.pfn)
				{
					bResult = true;
					break;
				}
			}
			if (!bResult)
			{
				m_vEventMonitor.push_back(em);
				bResult = true;
			}
		}

		return bResult;
	}

private:
	VEventMonitor		m_vEventMonitor;
};

