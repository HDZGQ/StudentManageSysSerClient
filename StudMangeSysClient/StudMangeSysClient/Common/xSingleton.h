#ifndef _XSINGLETONCLASS_H_
#define _XSINGLETONCLASS_H_


#include <iostream>
using namespace std;

//用于多线程，考虑是否需要加锁
template <class T>
class xSingleton
{
public:
	static T * GetInstance()
	{
		if( m_pInstance == NULL )
			m_pInstance = new T;
		return m_pInstance;
	}

protected:
	xSingleton()
	{
		m_pInstance = (T*)this; //注释也可以
	}
	~xSingleton()
	{
		delete m_pInstance;
	}

	static T* m_pInstance;
};
template <class T>
T * xSingleton<T>::m_pInstance = (T*)NULL;

#endif