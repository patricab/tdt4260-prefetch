/* C interface for prefetchers. */
/* DO NOT MODIFY THIS FILE */

#include <stdint.h>
#include <stdlib.h>

/*
 * This makes the DPRINT macro and all trace flags available.
 * DPRINTF is a print macro that takes a trace flag, a format string and
 * a variable number of print parameters (like regular printf), and prints
 * them to stdout if the trace flag in question is enabled on the command
 * line with --trace-flags=.
 *
 * For prefetcher use, the relevant flag i HWPrefetch.
 * Example (which prints out the address of a cache access):
 *
 * DPRINTF(HWPrefetch, "Address %#x was accessed\n", stat.mem_addr)
 *
 */
#include "base/trace.hh"

/* Size of cache blocks (cache lines) in bytes. */
#define BLOCK_SIZE 64

/* Maximum number of pending prefetch requests. */
#define MAX_QUEUE_SIZE 100

/* The largest possible physical memory address. */
#define MAX_PHYS_MEM_ADDR ((uint64_t)(256*1024*1024) - 1)

/* M5 note: must match typedefs in in base/types.hh */
typedef uint64_t Addr;
typedef int64_t Tick;
typedef uint64_t Delta;

/*
 * This is the information provided to the prefetcher on each call to
 * prefetch_access by the simulator.
 */
struct AccessStat {
    Addr pc;        /* The address of the instruction that caused the access */
    Addr mem_addr;  /* The memory address that was requested */
    Tick time;      /* The simulator time cycle when the request was sent */
    int miss;       /* Was this demand access a cache hit (0) or miss (1)? */
};

// typedef struct
// {
//     Addr pc;
//     Addr last_addr;
//     Delta delta;  
// } rpt_line_t;

// typedef struct {
//     rpt_line_t *lines;
//     int size;
// } rpt_t;

typedef struct
{
    Delta *list;
    int size;
} delta_buff_t;

typedef struct
{
    Addr pc;
    Addr last_addr;
    Addr last_prefetch_addr;
    delta_buff_t buffer;
} dcpt_line_t;

typedef struct
{
    dcpt_line_t *lines;
    int size;
} dcpt_t;



/*
 * Functions that are called by the simulator, with implementation
 * provided by the user. The implementation may be an empty function.
 */

/*
 * The simulator calls this before any memory access to let the prefetcher
 * initialize itself.
 */
extern "C" void prefetch_init(void);

/*
 * The simulator calls this function to notify the prefetcher about
 * a cache access (both hits and misses).
 */
extern "C" void prefetch_access(AccessStat stat);

/*
 * The simulator calls this function to notify the prefetcher that
 * a prefetch load to address addr has just completed.
 */
extern "C" void prefetch_complete(Addr addr);



/* Functions callable from the user-defined prefetcher. */

/*
 * The prefetcher calls this function to notify the simulator that
 * a prefetch for address addr should be added to the prefetch queue.
 */
extern "C" void issue_prefetch(Addr addr);


/* Is the prefetch bit set for the cache block corresponding to addr? */
extern "C" int get_prefetch_bit(Addr addr);

/* Set the prefetch bit for the cache block corresponding to addr. */
extern "C" void set_prefetch_bit(Addr addr);

/* Clear the prefetch bit for the cache block corresponding to addr. */
extern "C" void clear_prefetch_bit(Addr addr);



/* Is this address already in the cache? */
extern "C" int in_cache(Addr addr);

/* Is this address already in the MSHR queue? */
extern "C" int in_mshr_queue(Addr addr);

/* Number of occupied slots in the prefetch request queue */
extern "C" int current_queue_size(void);
