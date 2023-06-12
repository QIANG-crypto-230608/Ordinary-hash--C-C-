/*
	Implementation of linear detection
*/
#pragma once
#include<vector>
#include<iostream>
#include<string>
using namespace std;

//Use 3 status bits to represent empty, occupiedand and deleted respectively, and use enumeration to represent status bits
namespace CloseHash
{
	enum State
	{
		EMPTY,
		EXIST,
		DELETE
	};

	/*
		Define hash data
		Hash data should contain two members: the data and the status
	*/
	template<class K, class V>
	struct HashData
	{
		pair<K, V> _kv; //Date
		State _state = EMPTY; //Status
	};


	/*
		Functor:
			The purpose of the functor is order to make different data type can be modular, easy to compute the data position
	*/

	//Class template, default support for int 
	template<class K>
	struct Hash
	{
		size_t operator()(const K& key)
		{
			return key;
		}
	};

	//Class template, support for string
	template<>
	struct Hash<string>
	{
		//Using BKDR hashing(multiplied by a prime number, such as 131) will reduce collisons
		size_t operator()(const string& s)
		{
			size_t value = 0;
			//Take the sum after each character * 131
			for (auto e : s)
			{
				value += e;
				value *= 131;
			}
			return value;
		}
	};


	/*
		Hash table should contain two members: hash data and the number of vaild data stored
		The template has three parameters: K, V, HashFunc
			1.Since we don't know whether key is K or pair, we define two template parameters K and V to cover the two cases where key is K or pair
			2.Since ew don't know whether the data type of key is int or string, pair or struct, modulo needs to be taken when calculating the
			mapping position of key, but only modulo can be taken for int, and string, struct, pair cannot be modulo.
			HashFunc is used as a copying function. Its instance can handle these types of modulating separately.
	*/
	template<class K, class V, class HashFunc = Hash<K>>
	class HashTable
	{
	public:
		/*
		Insert:
			1.Check whether key exists, if the key is present, insertion fails
			2.When first inserted, the value of hash table is 0, so let the table expanded when the first inserted
			3.Check if the load factor is more than 0.7, if the table is full, a new table is created, and insert the data from the
			  ord table into the new table.
			4.When the calculated position has data, it is probed backwords until the empty data can be detected, and the data is stored.
	*/
		bool Insert(const pair<K, V>& kv)
		{
			HashData<K, V>* ret = Find(kv.first);
			if (ret)
				return false;
			if (_table.size() == 0)
				_table.resize(10);
			else if ((double)_n / (double)_table.size() > 0.7) //Load factor > 0.7, need to increase capacity
			{
				HashTable<K, V, HashFunc> newHashTable;
				newHashTable._table.resize(2 * _table.size());

				for (auto& e : _table)
				{
					if (e._state == EXIST)
						newHashTable.Insert(e._kv);
				}
				_table.swap(newHashTable._table);
			}

			HashFunc hf;
			size_t start = hf(kv.first) % _table.size();
			size_t index = start;

			//Probe the rear position --- linear probing
			size_t i = 1;
			while (_table[index]._state == EXIST)
			{
				//When the state is EXIST, collision occurs, and finding the empty position backwords
				index = start + i;
				index %= _table.size();
				++i;
			}

			//Store the data when the empty data is found
			_table[index]._kv = kv;
			_table[index]._state = EXIST;
			++_n;

			return true;
		}


		/*
			Search:
			1.Whether the data passed to the hash table is K or pair, using K as the Key to search
			2.Calculate the position of the elements
			3.If the current location is key, the return that element, otherwise a conflict may have occurredand, comtinue probing backwords
		*/
		//Use K to research
		HashData<K, V>* Find(const K& key)
		{
			if (_table.size() == 0)
				return nullptr;

			HashFunc hf; //functor
			size_t start = hf(key) % _table.size(); //Divide the residual method to find the element position
			size_t index = start;
			size_t i = 1;
			while (_table[index]._state != EMPTY)
			{
				if (_table[index]._state == EXIST && _table[index]._kv.first == key) //Find it
					return &_table[index]; //The location exists and the value is key, return address facilitates the modification of the data

				//In case of collision, look backwords
				index = start + i; //Linear detection ==> index = start + i * i //quadratic probing
				index %= _table.size();
				++i;
			}
			return nullptr;
		}


		/*
		Delete:
			With a fake delete, the state is simply marked as deleted.
	*/
		bool Erase(const K& key)
		{
			HashData<K, V>* ret = Find(key);
			if (ret == nullptr) //Didn't find
				return false;
			else //Find it
			{
				ret->_state = DELETE;
				--_n;

				return false;
			}
		}

	private:
		vector<HashData<K, V>> _table;//Hash table
		size_t _n = 0; //The number of valid data stored
	};


	void test_CloseHashInt()
	{
		int a[] = { 6, 201, 35, 76, 89, 2 };
		HashTable<int, int> ht;
		for (auto e : a)
			ht.Insert(make_pair(e, e));
	}


	void test_CloseHashString()
	{
		string a[] = { "footable", "basketball", "soccer", "football", "tennis", "tennis", "ping-pang ball", "tennis" };
		HashTable<string, int> ht;
		for (auto e : a)
		{
			auto ret = ht.Find(e);
			if (ret)
				ret->_kv.second++;
			else
				ht.Insert(make_pair(e, 1));
		}
	}
}




	


	

	





