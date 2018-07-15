#include "stdafx.h"
#include "Math.h"

extern Math* DeepCopy(Math* src);

//Dict.
Dict::Dict(const Dict& _src)
{
	size_t L = _src.vct_dict.size();
	for (size_t i = 0;i < L;i++)
	{
		Pair p;
		strcpy(p.key, _src.vct_dict[i].key);
		p.value = DeepCopy(_src.vct_dict[i].value);
		vct_dict.push_back(p);
	}
}

Dict& Dict::operator=(const Dict& _src)
{
	if (this != &_src)
	{
		//Delete the previous dict.
		size_t L = vct_dict.size();
		for (size_t i = 0;i < L;i++)
		{
			delete vct_dict[i].value;//Release the ptrs.
		}

		vct_dict.clear();

		L = _src.vct_dict.size();
		for (size_t i = 0;i < L;i++)
		{
			Pair p;
			strcpy(p.key, _src.vct_dict[i].key);
			p.value = DeepCopy(_src.vct_dict[i].value);
			vct_dict.push_back(p);
		}

	}
	return *this;
}



