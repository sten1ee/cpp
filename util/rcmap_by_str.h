#ifndef _RCMAP_BY_STR_H_
#define _RCMAP_BY_STR_H_

#include <stdint.h>
#include <assert.h>
#include <string.h>


class RcMapByStr {
public:

	//typedef const char*  rid_t;
	typedef uint32_t     off_t;
	typedef uint32_t     idx_t;


private:
	friend class RcMapByStrBuilder;

	struct File;
	struct Descriptor;
	struct Resource;

	const File* const file;

	struct File
	{
		idx_t  rc_count;

		//offset_t  o_dsc[rc_count];  // offset to the (sorted by key asc) descriptors of the resources

		/** Get offset if i-th descriptor */
		off_t  off_dsc(idx_t i) const {
			return ((const off_t*)(this + 1))[i];
		}
		void set_off_dsc(idx_t i, off_t off) {
			((off_t*)(this + 1))[i] = off;
		}

		/** Get i-th descriptor */
		const Descriptor* dsc(idx_t i) const {
			assert(i < rc_count);
			return (const Descriptor*)((char*)this + off_dsc(i));
		}
		Descriptor* dscRef(idx_t i) {
			assert(i < rc_count);
			return (Descriptor*)((char*)this + off_dsc(i));
		}

		/** Get resource corresponding to spec. descriptor */
		const Resource* res(const Descriptor& dsc) const {
			return (const Resource*)((char*)this + dsc.o_resource);
		}
		Resource* resRef(const Descriptor& dsc) {
			return (Resource*)((char*)this + dsc.o_resource);
		}
	};

	struct Descriptor
	{
		idx_t key_size; // key size
		off_t o_resource;

		//char     key[key_size]; // resource key
		const char* key() const {
			return (const char*)(this + 1);
		}
		char* keyRef() {
			return (char*)(this + 1);
		}
	};

	struct Resource
	{
		idx_t  data_size;  // resource size in bytes

		//char     data[data_size]; // resource (raw) data
		const char* data() const {
			return (char*)(this + 1);
		}
		char* dataRef() {
			return (char*)(this + 1);
		}
	};

	static const void*  GetRawResource(const File* f, const char* key, idx_t key_size, idx_t* rsize = nullptr)
	{
		// if rc_count < 7 go for linear search, otherwise -- binary search
		//
		if (f->rc_count < 7)
			//const idx_t  key_size = (idx_t) strlen(key);
			for (idx_t i = 0; i < f->rc_count; i++)
			{
				const Descriptor* dsc = f->dsc(i);
				if (dsc->key_size == key_size && 0 == memcmp(key, dsc->key(), key_size))
				{
					const Resource* res = f->res(*dsc);
					if (rsize)
						*rsize = res->data_size;
					return res->data();
				}
			}
		else
		{
			idx_t  low = 0, high = f->rc_count, mid;
			while (low < high) {
				mid = (low + high) / 2;
				const Descriptor* dsc = f->dsc(mid);
				const char* pCurKey = dsc->key();
				int diff = memcmp(key, pCurKey, /*min*/(key_size <= dsc->key_size ? key_size : dsc->key_size));
				if (diff < 0 || (diff == 0 && key_size < dsc->key_size)) {
					high = mid;
				}
				else if (diff > 0 || (diff == 0 && key_size > dsc->key_size)) {
					low = mid + 1;
				}
				else {
					assert(diff == 0 && key_size == dsc->key_size);
					const Resource* res = f->res(*dsc);
					if (rsize)
						*rsize = res->data_size;
					return res->data(); // found
				}
			}
		}

		return nullptr;
	}

public:
	RcMapByStr(const void* rcMapBase)
		: file((const File*)rcMapBase)
	{}

	const void*  GetRawResource(const char* key, idx_t* rsize = nullptr) {
		return GetRawResource(file, key, (idx_t)strlen(key), rsize);
	}

	// For testing purposes only:
	idx_t  nResources() const { return file->rc_count; }
	const char*  GetResourceKey(idx_t i) const { return file->dsc(i)->key(); }
	const char*  GetResourceData(idx_t i) const { return file->res(*file->dsc(i))->data(); }
};
#endif