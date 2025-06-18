// base code obtained from https://pmem.io/pmdk/manpages/linux/master/libpmem/libpmem.7/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libpmem.h>

// PM size of 4k
#define PMEM_LEN 4096

#define PATH "./pmemtest"

// TODO: find way to safely read data from a pm pointer
int main(int argc, char *argv[])
{
	char *pmemaddr;
	size_t mapped_len;
	int is_pmem;

	// create a pm file and memory map it
	char *str = "hello, world!!!!!\n\0";
	int len = strlen(str);

	if ((pmemaddr = pmem_map_file(PATH, PMEM_LEN, PMEM_FILE_CREATE,
								  0666, &mapped_len, &is_pmem)) == NULL)
	{
		perror("pmem_map_file");
		exit(1);
	}

	pmem_memcpy(pmemaddr, str, len, 0); // 0 flag indicates persist data

	// following if/else statement is redundant becaise pmem_memcpy will persist changes already
	if (is_pmem)
		pmem_persist(pmemaddr, len);
	// pmem_memcpy_persist(pmemaddr, str, len);
	else
		pmem_msync(pmemaddr, len);

	/*
	 * Delete the mappings. The region is also
	 * automatically unmapped when the process is
	 * terminated.
	 */
	pmem_unmap(pmemaddr, mapped_len);
}
