/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"

rpt_t table;

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    //DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");

    table.lines = (rpt_line_t *)calloc(1, sizeof(rpt_line_t));
    table.size = 1;
}

void prefetch_access(AccessStat stat)
{
    Delta new_delta;
    Addr prefetch_addr;
    
    /* prefetch_addr is now an address within the _next_ cache block */
    // Addr prefetch_addr = stat.mem_addr + BLOCK_SIZE;

    /*
     * Issue a prefetch request if a demand miss occured,
     * and the block is not already in cache.
     */
    // if (stat.miss && !in_cache(prefetch_addr)) {
    //     issue_prefetch(prefetch_addr);
    // }

    /* Check table on cache miss */
    if (stat.miss) {
        for(int i = 0; i < table.size; i++) {
            /* Calculate new delta if PC match */
            if (stat.pc == table.lines[i].pc) {
                    /* Calculate new delta based on last address */
                    new_delta = stat.mem_addr - table.lines[i].last_addr; 
                    
                    /* Issue prefetch if new delta matches last delta stored */
                    if (new_delta == table.lines[i].delta) {
                        prefetch_addr = stat.mem_addr + new_delta;
                        issue_prefetch(prefetch_addr);
                    }
                    break;
            } else {
                /* If not, create new table entry */
                table.lines = (rpt_line_t *)realloc(table.lines, table.size + 1);
            }
        }
    }

}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}