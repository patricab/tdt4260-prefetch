/*
 * A prefetcher which does strided timing rwo-block lookahead.
 * This means that the prefetcher fetches the next two blocks with a certain width
 * _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"

struct TSP_t {
	Addr pc;
	Addr pre_field;
	Addr $addr;
	int  stride;
	Addr pre_addr;
	Addr states;
} table [512];

void prefetch_init(void)
{
	/* Called before any calls to prefetch_access. */
	/* This is the place to initialize data structures. */


	table = new TSP_t;

	DPRINTF(HWPrefetch, "Initialized Timing Stride prefetcher\n");
}

void prefetch_access(AccessStat stat)
{

	// pf_addr is now an address within the _next_ cache block
	Addr pf_addr0 = stat.mem_addr + 1 * table.stride; // * BLOCK_SIZE;
	Addr pf_addr1 = stat.mem_addr + 2 * table.stride; // * BLOCK_SIZE;

	// set prefetch bit on cache hit


	// Issue a prefetch request if the block is not already in cache.
	if (stat.miss) { //&&
		table[k] = {stat.pc, stat.mem_addr, stat.mem_addr - stat.pc}
		k++

		if (!in_cache(pf_addr0)) {
			issue_prefetch(pf_addr0);
		}
		if (!in_cache(pf_addr1)) {
		   issue_prefetch(pf_addr1);
      }
	}
}

void prefetch_complete(Addr addr) {
	/*
	 * Called when a block requested by the prefetcher has been loaded.
	 */
}
