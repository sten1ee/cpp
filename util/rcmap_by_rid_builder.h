#ifndef _RCMAP_BY_RID_BUILDER_H_
#define _RCMAP_BY_RID_BUILDER_H_

#include "rcmap_by_rid.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <memory.h>
#include <memory>
#include <map>
#include <iostream>

using std::map;
using std::string;
using std::unique_ptr;

class RcMapByRidBuilder {

private:
	typedef RcMapByRid::rid_t  rid_t;
	typedef RcMapByRid::off_t  off_t;
	typedef RcMapByRid::off_t  idx_t;
	typedef struct RcMapByRid::File         File;
	typedef struct RcMapByRid::Cluster      Cluster;
	typedef struct RcMapByRid::Resource     Resource;

	struct Data 
	{
		void* data;
		idx_t data_size;

		Data() {
			data = nullptr;
			data_size = 0;
		}

		void SetData(const char* dt, idx_t dt_sz)
		{
			assert(data == 0 && data_size == 0); //if this fails, we will work correctly, but strongly suggests you are duplicating resoruces
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
		Data & operator=(Data& d) {}
		Data(const Data& d) {}
	};

	map<rid_t, Data>   _map;

	static idx_t  alligned(size_t sz) 
	{
		const idx_t ALGN = sizeof(off_t);
		return (idx_t)((sz + ALGN - 1) / ALGN) * ALGN;
	}

	/** Calculates descriptor size (incl. allignment) */
	static off_t  filehdr_size(rid_t total_n_clusters) 
	{
		return (off_t)alligned(sizeof(File) + total_n_clusters * sizeof(off_t));
	}

	/** Calculates cluster size (incl. allignment) */
	static size_t  cluster_size(rid_t beg, rid_t end) 
	{
		assert(beg < end);
		return alligned(sizeof(Cluster) + (end - beg) * sizeof(off_t));
	}

	/** Calculates resource size (incl. allignment) */
	static size_t  resource_size(size_t data_size) 
	{
		return alligned(sizeof(Resource) + data_size);
	}

	rid_t  beg_rid() const {
		assert(!_map.empty());
		map<rid_t, Data>::const_iterator itBeg = _map.begin();
		return (*itBeg).first;
	}

	rid_t  end_rid() const {
		assert(!_map.empty());
		return 1 + _map.rbegin()->first;
	}

public:
	/** Call this after last resource has been added
	 *  to calculate the total RcMapByRid size for this given cluster_sz
	 */
	off_t  CalculateTotalSize(rid_t cluster_sz) const
	{
		assert(cluster_sz > 1);

		rid_t total_n_clusters = (end_rid() - beg_rid() + cluster_sz - 1) / cluster_sz;
		off_t total_sz = filehdr_size(total_n_clusters);

		map<rid_t, Data>::const_iterator cpair = _map.begin();
		const rid_t  base_rid = beg_rid();
		rid_t  crid = base_rid;
		for (rid_t cci = 0; cci < total_n_clusters; cci++) 
		{ // cci is for 'current cluster idx' (0-based)
			if ((crid - base_rid) / cluster_sz > cci) { // cci is an empty cluster:
				continue;
			}

			assert((crid - base_rid) / cluster_sz == cci);
			const rid_t ccbeg_rid = crid;
			rid_t cclast_rid;
			do
			{
				total_sz += resource_size(cpair->second.data_size);
				cclast_rid = crid;
				assert((crid - base_rid) / cluster_sz == cci);
				++cpair; 
				if (cpair == _map.end())
					break;
				crid = cpair->first;
			} while ((crid - base_rid) / cluster_sz == cci);
			total_sz += cluster_size(ccbeg_rid, 1 + cclast_rid);
		}
		return total_sz;
	}

	/** Should be called after last resource has been add-ed
	 *  To construct the RcMapByRid table in memory and return pointer to it
	 */
	unique_ptr<char>  BuildRcMapStructInMemory(rid_t cluster_sz, off_t* ptotal_sz) 
	{
		size_t total_size;
		if (cluster_sz == 0)
		{
			total_size = (off_t)-1;
			for (int cs = 2; cs < 65536; cs *= 2)
			{
				off_t curSize = CalculateTotalSize(cs);
				if (curSize < total_size)
				{
					total_size = curSize;
					cluster_sz = cs;
				}
			}
		}
		else
			total_size = CalculateTotalSize(cluster_sz);

		unique_ptr<char> membuf(new char[total_size]);
		memset(membuf.get(), 0, total_size);

		File* f = (File*)membuf.get();
		f->beg_rid = beg_rid();
		f->end_rid = end_rid();
		f->cluster_size = cluster_sz;
		const rid_t total_n_clusters = f->NumClusters();
		off_t cur_pos_in_blob = filehdr_size(total_n_clusters); //memory consumed so far

		map<rid_t, Data>::const_iterator cpair = _map.begin();
		const rid_t  base_rid = beg_rid();
		for (rid_t cci = 0; cci < total_n_clusters; cci++) 
		{ // cci is for 'current cluster idx' (0-based)
			rid_t  crid = cpair->first;
			if ((crid - base_rid) / cluster_sz > cci)
			{ // cci is an empty cluster:
				f->SetOffCluster(crid, 0);
				continue;
			}

			assert((crid - base_rid) / cluster_sz == cci);
			off_t o_cluster = cur_pos_in_blob;
			f->SetOffCluster(crid, o_cluster);

			Cluster* c = f->clusterRef(crid);
			c->beg_rid = crid;
			{
				rid_t cclast_rid = crid;
				map<rid_t, Data>::const_iterator cpair_tmp = cpair;
				assert(cpair != _map.end());
				do
				{
					++cpair_tmp;
					if (cpair_tmp == _map.end())
						break;
					crid = cpair_tmp->first;
					if ((crid - base_rid) / cluster_sz != cci)
						break;
					cclast_rid = crid;
				} while (true);
				c->end_rid = 1 + cclast_rid;
			}
			cur_pos_in_blob += cluster_size(c->beg_rid, c->end_rid);

			crid = c->beg_rid;
			while (crid < c->end_rid)
			{
				assert(cpair != _map.end() && crid == cpair->first);
				c->set_off_resource(crid, cur_pos_in_blob - o_cluster);
				Resource* r = c->resourceRef(crid);
				r->size = cpair->second.data_size;
				memcpy(r->dataRef(), cpair->second.data, r->size);
				cur_pos_in_blob += resource_size(r->size);

				++cpair;
				++crid;
				if (cpair == _map.end())
				{
					assert(crid == c->end_rid);
					break;
				}
				while (crid != cpair->first && crid < c->end_rid)
				{
					assert(crid < cpair->first);
					c->set_off_resource(crid, 0); //fill the non-used resources with 0
					++crid;
				}
			} //current cluster populated
		} //for each cluster
		assert(total_size == cur_pos_in_blob);
		*ptotal_sz = total_size;
		return membuf;
	}

	RcMapByRidBuilder() {
	}

	void  AddRawResource(rid_t rid, const char* data) {
		AddRawResource(rid, data, (idx_t)strlen(data) + 1);
	}

	void  AddRawResource(rid_t rid, const char* data, idx_t data_size) {
		assert(_map.find(rid) == _map.end()); //proves we do not have duplicates
		_map[rid].SetData(data, data_size);
	}



	static void  test() {
		struct Pair {
			rid_t rid;
			const char* data;
		};

		const Pair kv[] = {
			{ 3, "3 value is this" },
			{ 5, "5 value is this" },
			{ 1, "1 value is this" },
			{ 4, "4 value is this" },
			{ 2, "2 value is this" },

			{ 103, "103 value is this" },
			{ 105, "105 value is this" },
			{ 101, "101 value is this" },
			{ 104, "104 value is this" },
			{ 102, "102 value is this" },

			{ 203, "203 value is this" },
			{ 205, "205 value is this" },
			{ 201, "201 value is this" },
			{ 204, "204 value is this" },
			{ 202, "202 value is this" },

			{ 1203, "1203 value is this" },
			{ 1205, "1205 value is this" },
			{ 1201, "1201 value is this" },
			{ 1204, "1204 value is this" },
			{ 1202, "1202 value is this" },
		};
		size_t nAdded = (sizeof(kv) / sizeof(*kv));
		RcMapByRidBuilder  builder;
		for (size_t i = 0; i < nAdded; i++) {
			builder.AddRawResource(kv[i].rid, kv[i].data);
		}

		RcMapByRid::off_t size = -1;
		//builder.CalculateTotalSize(64);
		unique_ptr<char>  file = builder.BuildRcMapStructInMemory(64, &size);
		RcMapByRid idmap(file.get());
		std::cout << "Total size is " << size << std::endl;
		for (rid_t i = 0; i < nAdded; i++) {
			std::cout << (const char*)idmap.GetRawResource(kv[i].rid) << std::endl;
		}

	}
};
#endif // _RCMAP_BY_RID_BUILDER_H_
