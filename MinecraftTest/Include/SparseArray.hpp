#ifndef _SPARSE_ARRAY_HPP
#define _SPARSE_ARRAY_HPP

#include <set>
#include <vector>

#include <EngineTypeDefs.h>

template<typename T>
class SparseArray
{
private:
	std::set<st> keys;
	std::vector<st> indices;
	std::vector<T> data;
	T nil;

public:
	SparseArray()
	{
		nil = {0};
	}

	SparseArray(const T& _nil) : nil(_nil) {};

	void insert(st key, const T& entry)
	{
		auto keyIter = keys.find(key);
		if (keyIter != keys.end())
		{
			st indexOffset = std::distance(keys.begin(), keyIter);
			data[indices[indexOffset]] = entry;
		}
		else
		{
			auto newIter = keys.insert(key);
			indices.insert(indices.begin() + (std::distance(keys.begin(), newIter.first)), data.size());
			data.push_back(entry);
		}
	}

	const T& operator[](st index)
	{
		auto keyIter = keys.find(index);
		if (keyIter != keys.end())
		{
			st indexOffset = std::distance(keys.begin(), keyIter);
			return data[indices[indexOffset]];
		}
		else
		{
			return nil;
		}
	}

	st size()
	{
		return data.size();
	}

	T* getData()
	{
		return data.data();
	}

	const std::set<st>& getKeys()
	{
		return keys;
	}
};

#endif