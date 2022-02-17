/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"

dcpt_t table;

Addr *candidates
int candidates_size;
// delta_buff_t buffer;

/* Function forward declarations */
void buffer_push(delta_buff_t *buffer, Delta delta);
void delta_correlate(dcpt_line_t *entry) {

/* Called before any calls to prefetch_access. */
/* This is the place to initialize data structures. */
void prefetch_init(void)
{
    /* Initialize DCPT table */
    table.lines = (dcpt_line_t *)calloc(1, sizeof(dcpt_line_t));
    table.size = 1;
    
    /* Initialize delta buffer */
    table.lines[0].buffer.list = (Delta *)calloc(1, sizeof(Delta));
    table.lines[0].buffer.size = 0;

    /* Initialize prefetching candidate buffer */
    candidates = (Addr *)calloc(1, sizeof(Addr));
    candidates_size = 1;
}

void prefetch_access(AccessStat stat)
{
    Delta new_delta;
    Addr prefetch_addr;

    /* Calculate new delta if PC match */
    bool pc_match_found = false;
    for (int i = 0; i < table.size; i++) {
        if (stat.pc == table.lines[i].pc) {
            pc_match_found = true; 
            new_delta = stat.mem_addr - table.lines[i].last_addr;

            /* Add new element to table if delta > 0 */
            if (new_delta > 0) {
                buffer_push(&table.lines[i].buffer, new_delta);
                table.lines[i].last_addr = stat.mem_addr;
            }
        }
    }
    /* If not, create new table entry */
    if (!pc_match_found) {
        table.lines = (dcpt_line_t *)realloc(table.lines, table.size + 1);
        table.size++;
        table.lines[table.size + 1].pc = stat.pc;
        table.lines[table.size + 1].last_addr = stat.mem_addr;
        table.lines[table.size + 1].last_prefetch_addr = 0;
    }
}

void prefetch_complete(Addr addr) {
    // free(table.lines);
    // free(buffer.list);
}

void buffer_push(delta_buff_t *buffer, Delta delta) {
    /* Re-allocate buffer */
    if (buffer->size == 0) { // Check if buffer is empty
       buffer->list[0] = delta;
       buffer->size = 1; 
    } else {
        buffer->list = (Delta *)realloc(buffer->list, buffer->size + 1);
        buffer->size++;
        buffer->list[buffer->size] = delta;
    }
}

void delta_correlate(dcpt_line_t *entry) {
    /* Variables */
    int buffer_size = entry->buffer.size;
    Delta d1 = entry->buffer.list[size];
    Delta d2 = entry->buffer.list[size - 1];
    Addr address = entry->last_addr;

    /* Search through pairs in delta buffer */
    for (int j = 0; j < buffer_size; j++) {
        for (int k = 1; k < (buffer_size - 1); k++) {
            /* Matching delta pairs */
            if ((entry->buffer.list[j] == d1) && (entry->buffer.list[k] == d2)) {
                /* Go through remaining entries, add as candidates */
                for (int i = k + 1; i < buffer_size; i++) {
                    address = address + entry->buffer.list[i]; 
                    
                    /*  */
                    if (candidates_size > 1)
                        candidates = (Addr *)realloc(candidates, candidates_size + 1);

                    candidates[candidates_size - 1] = address;
                    candidates_size++;
                }
            }
        }
    }
}