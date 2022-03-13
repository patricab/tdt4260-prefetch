/*
 * A prefetcher which does delta-correlated prediction with a table.
 * This means that the prefetcher fetches based on a
 * local History Buffer for each program counter.
 * It also ignores requests to blocks already in the cache.
 */
#include <algorithm>
#include <deque>
#include <iostream>
#include <list>
#include <tr1/unordered_map>
#include "interface.hh"

#define TABLE_SIZE 64
#define DELTA_SIZE 6

using namespace std;
struct DCPT {
	Addr tag;
	Addr prev_addr;
	Addr prev_fetch;
	deque<int> deltas;
};

tr1::unordered_map<Addr, DCPT> table;

list<Addr> candidates;
list<Addr> prefetches;

void issue_prefetch(Addr addr){
	cout << "prefetched: " << addr << endl;
}

/* Is this address already in the cache? */
int in_cache(Addr addr) {
	return false;
}

/* Is this address already in the MSHR queue? */
int in_mshr_queue(Addr addr) {
	return false;
}

void delta_correlation(DCPT entry) {
	candidates.clear();

	deque<int>::iterator itr = entry.deltas.end();
	itr--;
	int d1 = *itr;
	itr--;
	int d2 = *itr;

	Addr address = entry.prev_addr;

	itr = entry.deltas.begin();
	while (itr != entry.deltas.end())
	{
		int u = *itr;
		itr++;
		int v = *itr;
		if (u == d2 && v == d1)
		{
			for (itr++; itr != entry.deltas.end(); ++itr)
			{
				address += *itr * BLOCK_SIZE;
				candidates.push_back(address);
			}
		}
	}
}

void prefetch_filter(DCPT entry) {
	prefetches.clear();
	list<Addr>::iterator itr;
	for (itr = candidates.begin(); itr != candidates.end(); ++itr)
	{
		if (!in_cache(*itr))
			prefetches.push_back(*itr);

		entry.prev_fetch = *itr;
	}
}


void prefetch_init(void)
{
	/* Called before any calls to prefetch_access. */
	/* This is the place to initialize data structures. */

	// * DPRINTF(HWPrefetch, "Initialized delta-correlated prediction table prefetcher\n");

}

void prefetch_access(AccessStat stat) {
	Addr idx = stat.pc % TABLE_SIZE;

	tr1::unordered_map<Addr, DCPT>::iterator itr = table.find(idx);
	DCPT entry;

	if (itr == table.end() || itr->second.tag != stat.pc) {

		// if (table.size() >= TABLE_SIZE) {
		// 	table.erase(table.begin()->first);
		// }
		// replace entry in table
		entry.deltas.clear();
		entry.tag        = stat.pc;
		entry.prev_addr  = stat.mem_addr;
		entry.prev_fetch = 0;
		entry.deltas.push_front(1);
		table[idx] = entry;
	} else {
		entry = itr->second;
		int delta = stat.mem_addr - entry.prev_addr;
		if (delta != 0) {
			// update entry in table
			if (entry.deltas.size() >= DELTA_SIZE)
				entry.deltas.pop_front();

			entry.deltas.push_back(delta);
			entry.prev_addr = stat.mem_addr;

			table.erase(itr->first);
			table[idx] = entry;
		}

		delta_correlation(entry);
		prefetch_filter(entry);

		list<Addr> buf = prefetches;

		for (list<Addr>::iterator it = buf.begin(); it != buf.end(); ++it) {
			if (*it != 0)
				issue_prefetch(*it);
		}
	}
}

void prefetch_complete(Addr addr) {
	// Called when a block requested by the prefetcher has been loaded.

}


int main() {
   prefetch_init();

	AccessStat stat;

	for (int i = 0; i < 6; i++)
	{
		for (int k = 10; k < 13; k++)
		{
			stat.pc = k;
			stat.mem_addr = k * i;
			stat.time = k + i;
			stat.miss = true;
			prefetch_access(stat);
		}
	}
}
