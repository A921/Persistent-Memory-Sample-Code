# Persistent-Memory-Sample-Code
Simple introduction to the libpmem library for perisistent memory (PM) programming. Summarized manual pages intended to strengthen my understanding as well for anyone interested.
Although the library provides low-level support for persistent memory, it is still usable without PM devices. 
For use with PM devices, applications need to use direct access storage (DAX), which enables direct access to files stores, directly mapping virtual addresses to physical addresses and thus bypassing page caches.
 
## Open PM file
```c
void *pmem_map_file(const char *path, size_t len, int flags, mode_t mode, size_t *mapped_lenp, int *is_pmemp);
```
Opens persistent memory file 

- __path__: Path to PM file
- __len__: Size of mapped region, it will be extended or truncated to PM_LEN if file exists.
- __flags__: one key one being PMEM_FILE_CREATE, Flag to create a file if PATH doesn't point to one, if specified, PM_LEN must be a non-zero value and mode must be specified
- __mapped_lenp__, Upon successful mapping, will store the size of the mapped file 
- __is_pmemp__ upon successful mapping will store whether the mapped region is persistent memory or not
- Returns pointer to start of memory mapped region upon success

## Persisting Changes
### pmem_flush() & pmem_drain
```c
void pmem_flush(const void *addr, size_t len);
void pmem_drain(void);
```
The are lower level functions that provide access to the individual stages of flushing for more control.

### pmem_persist()
```c
void pmem_persist(const void *addr, size_t len);
```
Tradionally, __msync__ system call was used to syncronize changes to a memory mapped file to guarentee the changes are made to the file before __munmap__ was called.
__pmem_persist__ will force any changes within the passed range of addresses to be stored to PM. Same as calling __msync__ but avoids going into kernel space when possible. It has 2 key steps, __pmem_flush__ to flush data from CPU caches to PM buffers, and __pmem_drain__ to ensure data reaches PM while ensuring store ordering (either is empty or contains the __SFENCE__ instruction if eADR is not supported).

### pmem_msync()
```c
int pmem_msync(const void *addr, size_t len);
```
__pmem_msync__ is the same as __pmem_persist__ but will also work on any traditional storage. Will make call to __msync__ system call and take steps to ensure values passed in will meet conditions for __msync__.


## Copying Data to PM

```c
void *pmem_memmove(void *pmemdest, const void *src, size_t len, unsigned flags);
void *pmem_memcpy(void *pmemdest, const void *src, size_t len, unsigned flags);
void *pmem_memset(void *pmemdest, int c, size_t len, unsigned flags);
```
The above functions optimized for PM copying and are functionally equivalent to __memmove__, __memcpy__, and __memset__ with a __pmem_persist__ after. Ensures changes are flushed unless __PMEM_F_MEM_NOFLUSH__ flag is specified. The __PMEM_F_MEM_NODRAIN__ will not do the final __pmem_drain__ step to allow for cases where multiple ranges are copied and flushed, then to perform only a single call to __pmem_drain__.