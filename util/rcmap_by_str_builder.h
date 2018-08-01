#ifndef _RCMAP_BY_STR_BUILDER_H_
#define _RCMAP_BY_STR_BUILDER_H_

#include "rcmap_by_str.h"
#include <stdint.h>
#include <assert.h>
#include <string>
#include <memory>
#include <map>

using std::map;
using std::string;
using std::unique_ptr;


class RcMapByStrBuilder {

private:
	//typedef RcMapByStr::rid_t  rid_t;
	typedef RcMapByStr::off_t  off_t;
	typedef RcMapByStr::off_t  idx_t;
	typedef struct RcMapByStr::File         File;
	typedef struct RcMapByStr::Descriptor   Descriptor;
	typedef struct RcMapByStr::Resource     Resource;

	struct Data {
		void* data;
		idx_t data_size;

		Data() {
			data = nullptr;
			data_size = 0;
		}

		void SetData(const void* dt, idx_t dt_sz) 
		{
			assert(data == nullptr);
			if (data)
				free(data);
			data = malloc(dt_sz);
			memcpy(data, dt, dt_sz);
			data_size = dt_sz;
		}

		~Data() {
			if (data)
				free(data);
		}
	private:
		Data(const Data& d);
		Data& operator=(const Data& d);
	};

	map<string, Data>  _map;
	//off_t              _total_rc_count;
	//off_t              _total_dsc_size;
	//off_t              _total_res_size;

	static constexpr idx_t  alligned(size_t sz) {
		const idx_t ALGN = sizeof(off_t);
		return (idx_t)((sz + ALGN - 1) / ALGN) * ALGN;
	}

	/** Calculates descriptor size (incl. allignment) */
	static constexpr off_t  filehdr_size(size_t numResources)
	{
		return (off_t)alligned(sizeof(File) + numResources * sizeof(off_t));
	}

	/** Calculates descriptor size (incl. allignment) */
	static constexpr size_t  descriptor_size(size_t key_size) 
	{
		return alligned(sizeof(Descriptor) + key_size);
	}

	/** Calculates resource size (incl. allignment) */
	static constexpr size_t  resource_size(size_t data_size) 
	{
		return alligned(sizeof(Resource) + data_size);
	}

public:
	RcMapByStrBuilder() {
		//_total_rc_count = _total_dsc_size = _total_res_size = 0;
	}

	void  AddRawResource(const std::string& key, const void* data, idx_t data_size)
	{
		assert(_map.find(key) == _map.end());
		_map[key].SetData(data, data_size);

		//_total_rc_count++;
		//_total_dsc_size += descriptor_size(key_size);
		//_total_res_size += resource_size(data_size);
	}

	unique_ptr<char>  BuildRcMapStructInMemory(off_t* ptotal_size) const
	{
		off_t _hdr_size = filehdr_size(_map.size());
		off_t _total_dsc_size = 0;
		off_t _total_res_size = 0;
		for (auto itRes = _map.begin(); itRes != _map.end(); ++itRes)
		{
			_total_dsc_size += descriptor_size(itRes->first.size() + 1);
			_total_res_size += resource_size(itRes->second.data_size);
		}
		off_t total_size = _hdr_size + _total_dsc_size + _total_res_size;

		unique_ptr<char> membuf(new char[total_size]);
		memset(membuf.get(), 0, total_size); //make sure the padding is not random
		File* f = (File*)membuf.get();
		f->rc_count = _map.size();
		off_t dsc_offset = _hdr_size;
		off_t res_offset = dsc_offset + _total_dsc_size;
		idx_t idxRes = 0;
		//for (const auto& pair : _map) {
		for (map<string, Data>::const_iterator pair = _map.begin(); pair != _map.end(); ++pair, ++idxRes)
		{
			const string& key = pair->first;
			const Data&   data = pair->second;
			f->set_off_dsc(idxRes, dsc_offset);
			Descriptor* dsc = f->dscRef(idxRes);
			dsc->key_size = key.size(); 
			assert(dsc->key_size == strlen(key.c_str()));
			dsc->o_resource = res_offset;
			memcpy(dsc->keyRef(), key.c_str(), key.size()+1); //copy the null-terminator as well
			dsc_offset += alligned(sizeof(Descriptor) + key.size() + 1);
			Resource* res = f->resRef(*dsc);
			res->data_size = data.data_size;
			memcpy(res->dataRef(), data.data, data.data_size);
			res_offset += alligned(sizeof(Resource) + data.data_size);
		}
		assert(idxRes == _map.size());
		assert(res_offset == total_size);
		*ptotal_size = total_size;
		return membuf;
	}



	static void  test() {
		const char* kv[][2] = {
			{ "xYz", "xYz value is this" },
			{ "ABC", "ABC value is this" },
			{ "123", "123 value is this" },
			{ "__", "__ value is this" },
			{ "_ABC", "_ABC value is this" },

			{ "3", "3 value is this" },
			{ "5", "5 value is this" },
			{ "1", "1 value is this" },
			{ "4", "4 value is this" },
			{ "2", "2 value is this" },

			{ "103", "103 value is this" },
			{ "105", "105 value is this" },
			{ "101", "101 value is this" },
			{ "104", "104 value is this" },
			{ "102", "102 value is this" },
		};
		size_t nAdded = (sizeof(kv) / sizeof(*kv));
		RcMapByStrBuilder  builder;
		for (size_t i = 0; i < nAdded; i++) {
			builder.AddRawResource(kv[i][0], kv[i][1], strlen(kv[i][1]));
		}

		RcMapByStr::off_t size = -1;
		unique_ptr<char>  file = builder.BuildRcMapStructInMemory(&size);
		RcMapByStr strmap(file.get());

		const RcMapByStr::idx_t nRes = strmap.nResources();
		assert(nRes == nAdded);
		for (RcMapByStr::idx_t i = 0; i < nRes; ++i) {
			const char* key = strmap.GetResourceKey(i);
			const char* data = strmap.GetResourceData(i);
			std::cout << i << "\t{ " << key << ", " << data << " },\n";
			assert(strmap.GetRawResource(key) == data);
		}
	}
};
#endif // _RCMAP_BY_STR_BUILDER_H_
