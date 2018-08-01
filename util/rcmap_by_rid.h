#ifndef _RCMAP_BY_RID_H_
#define _RCMAP_BY_RID_H_

#include <stdint.h>
#include <assert.h>


class RcMapByRid {
public:

	typedef uint32_t  rid_t;
	typedef uint32_t  off_t;
	typedef uint32_t  idx_t;

private:
	friend class RcMapByRidBuilder;

	struct File;
	struct Cluster;
	struct Resource;

	const File* const file;

	struct File 
	{
		rid_t   beg_rid;   // smallest resource id in the bundle
		rid_t   end_rid;   // 1 + greatest resource id in the bundle
		rid_t   cluster_size; // how many consecutive rid(s) form a cluster.
		//---rid_t   n_clusters   = ceil((end_rid - beg_rid)/cluster_size)
		//followed by off_t clusterOffsets[n_clusters]

		rid_t NumClusters() const { return (end_rid - beg_rid + cluster_size - 1) / cluster_size; }
		/** offset of i-th rid-cluster from beginning of File; 0-offset means 'no such cluster'**/
		//off_t   o_cluster[n_clusters];
		/** Get offset of i-th rid-cluster relative to begining of File **/
		off_t  off_cluster(rid_t rid) const
		{
			assert(rid >= beg_rid);
			assert(rid < end_rid);
			rid_t  cluster_idx = (rid - beg_rid) / cluster_size;
			assert(cluster_idx < NumClusters());

			return ((off_t*)(this + 1))[cluster_idx];
		}
		void  SetOffCluster(rid_t rid, off_t off)
		{
			assert(rid >= beg_rid);
			assert(rid < end_rid);
			rid_t  cluster_idx = (rid - beg_rid) / cluster_size;
			assert(cluster_idx < NumClusters());

			((off_t*)(this + 1))[cluster_idx] = off;
		}

		const Cluster* cluster(rid_t rid)  const
		{
			if (rid >= beg_rid && rid < end_rid)
			{
				off_t off = off_cluster(rid);
				if (off != 0)
					return (Cluster*)((char*)this + off);
				else
					return nullptr;
			}
			else
				return nullptr;
		}
		Cluster* clusterRef(rid_t rid)
		{
			return const_cast<Cluster*>(cluster(rid));
		}
	};

	struct Cluster 
	{
		rid_t  beg_rid; // smallest resource id in the cluster
		rid_t  end_rid; // 1 + greatest resource id in the cluster;
		//followed by off_t resourceOffsets[beg_rid-end_rid]

		// real size is (end_rid - beg_rid); 0-offset means 'no such resource'
		//off_t  o_resource[end_rid - beg_rid];
		/** Get offset of i-th resource relative to beginning of this Cluster */
		off_t  off_resource(rid_t rid) const
		{
			assert(beg_rid <= rid && rid < end_rid);
			return ((off_t*)(this + 1))[rid - beg_rid];
		}
		void  set_off_resource(rid_t rid, off_t off)
		{
			assert(beg_rid <= rid && rid < end_rid);
			((off_t*)(this + 1))[rid - beg_rid] =off;
		}

		const Resource* resource(rid_t rid) const 
		{
			if (rid >= beg_rid && rid < end_rid)
			{
				off_t off = off_resource(rid);
				if (off != 0)
					return (Resource*)((char*)this + off);
				else
					return nullptr;
			}
			else
				return nullptr;
		}
		Resource* resourceRef(rid_t rid) const
		{
			return const_cast<Resource*>(resource(rid));
		}
	};

	struct Resource 
	{
		off_t  size;     // resource size in bytes
		//followed by char resourceOffsets[size] bytes of data

		//char   data[size];  // resource data
		const char*  data() const {
			return ((const char*)(this + 1));
		}
		char*  dataRef() {
			return ((char*)(this + 1));
		}
	};

public:
	RcMapByRid(const void* rcMapBase)
		: file((const File*)rcMapBase)
	{}

	const void*  GetRawResource(rid_t rid, off_t* rsize = nullptr) {
		if (const Cluster* c = file->cluster(rid))
		{
			if (const Resource* res = c->resource(rid))
			{
				if (rsize)
					*rsize = res->size;
				return res->data();
			}
		}
		return nullptr;
	}
};

#endif // _RCMAP_BY_RID_H_
