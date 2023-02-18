#define DMALLOC_DISABLE 1
#include "dmalloc.hh"
#include <cassert>
#include <cstring>

/**
 * dmalloc(sz,file,line)
 *      malloc() wrapper. Dynamically allocate the requested amount `sz` of memory and 
 *      return a pointer to it 
 * 
 * @arg size_t sz : the amount of memory requested 
 * @arg const char *file : a string containing the filename from which dmalloc was called 
 * @arg long line : the line number from which dmalloc was called 
 * 
 * @return a pointer to the heap where the memory was reserved
 */
void* dmalloc(size_t sz, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.
    return base_malloc(sz);
}

/**
 * dfree(ptr, file, line)
 *      free() wrapper. Release the block of heap memory pointed to by `ptr`. This should 
 *      be a pointer that was previously allocated on the heap. If `ptr` is a nullptr do nothing. 
 * 
 * @arg void *ptr : a pointer to the heap 
 * @arg const char *file : a string containing the filename from which dfree was called 
 * @arg long line : the line number from which dfree was called 
 */
void dfree(void* ptr, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.
    base_free(ptr);
}

/**
 * dcalloc(nmemb, sz, file, line)
 *      calloc() wrapper. Dynamically allocate enough memory to store an array of `nmemb` 
 *      number of elements with wach element being `sz` bytes. The memory should be initialized 
 *      to zero  
 * 
 * @arg size_t nmemb : the number of items that space is requested for
 * @arg size_t sz : the size in bytes of the items that space is requested for
 * @arg const char *file : a string containing the filename from which dcalloc was called 
 * @arg long line : the line number from which dcalloc was called 
 * 
 * @return a pointer to the heap where the memory was reserved
 */
void* dcalloc(size_t nmemb, size_t sz, const char* file, long line) {
    // Your code here (to fix test014).
    void* ptr = dmalloc(nmemb * sz, file, line);
    if (ptr) {
        memset(ptr, 0, nmemb * sz);
    }
    return ptr;
}

/**
 * get_statistics(stats)
 *      fill a dmalloc_stats pointer with the current memory statistics  
 * 
 * @arg dmalloc_stats *stats : a pointer to the the dmalloc_stats struct we want to fill
 */
void get_statistics(dmalloc_stats* stats) {
    // Stub: set all statistics to enormous numbers
    memset(stats, 255, sizeof(dmalloc_stats));
    // Your code here.
}

/**
 * print_statistics()
 *      print the current memory statistics to stdout       
 */
void print_statistics() {
    dmalloc_stats stats;
    get_statistics(&stats);

    printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}

/**  
 * print_leak_report()
 *      Print a report of all currently-active allocated blocks of dynamic
 *      memory.
 */
void print_leak_report() {
    // Your code here.
}
