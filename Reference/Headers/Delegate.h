#pragma once

template <class... Args>
class FDelegate
{
public:
	typedef typename std::list<std::function<void(Args...)>>::iterator iterator;

	void operator() (const Args... args)
	{
		if (m_Functions.empty())
		{
			return;
		}

		for (auto iter = m_Functions.begin(); iter != m_Functions.end();)
		{

			if (!(*iter))
			{
				iter = m_Functions.erase(iter);
			}
			else
			{
				(*iter)(args...);
				++iter;
			}
		}
	}

	FDelegate& operator= (std::function<void(Args...)> const& func)
	{
		m_Functions.clear();
		m_Functions.push_back(func);
		return *this;
	}

	FDelegate& operator+= (std::function<void(Args...)> const& func)
	{
		m_Functions.push_back(func);

#ifdef _DEBUG
		if (m_Functions.size() % 100 == 0)
		{
			MSG_BOX("Warning: %d functions have been bound to Delegate.", &m_Functions.size());
			//cout << "Warning: " << m_Functions.size() << " functions have been bound to Delegate." << endl;
		}
#endif // _DEBUG

		return *this;
	}

	FDelegate& operator-= (std::function<void(Args...)> const& func)
	{
		void(* const* fp)(Args...) = func.template target<void(*)(Args...)>();
		const std::size_t funcHash = func.target_type().hash_code();

		if (nullptr == fp)
		{
			for (auto iter = m_Functions.begin(); iter != m_Functions.end(); iter++)
			{
				if (funcHash == (*iter).target_type().hash_code())
				{
					m_Functions.erase(iter);
					return *this;
				}
			}
		}

		else
		{
			for (auto iter = m_Functions.begin(); iter != m_Functions.end(); iter++)
			{
				void(* const* delegate_ptr)(Args...) = (*iter).template target<void(*)(Args...)>();
				if (nullptr != delegate_ptr && *fp == *delegate_ptr)
				{
					m_Functions.erase(iter);
					return *this;
				}
			}
		}

		return *this;
	}

	bool		empty()	{ return m_Functions.empty(); }
	size_t		size()	{ return m_Functions.size(); }
	iterator	begin()	{ return m_Functions.begin(); }
	iterator	end()	{ return m_Functions.end(); }
	void		clear()	{ m_Functions.clear(); }

private:
	std::list<std::function<void(Args...)>> m_Functions;
};