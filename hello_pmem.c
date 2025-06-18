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

/* using 4k of pmem for this example */
#define PMEM_LEN 4096

#define PATH "./pmemtest"

int main(int argc, char *argv[])
{
	char *pmemaddr;
	size_t mapped_len;
	int is_pmem;

	/* create a pmem file and memory map it */
	char *str = "hello, world!!!!!\n";
	int len = strlen(str);

	if ((pmemaddr = pmem_map_file(PATH, PMEM_LEN, PMEM_FILE_CREATE,
								  0666, &mapped_len, &is_pmem)) == NULL)
	{
		perror("pmem_map_file");
		exit(1);
	}
	char *pmem_str = pmemaddr;
	// strcpy(pmemaddr, str);
	// memcpy();
	pmem_memcpy(pmem_str, str, len, 0);

	// following if statement is redundant becaise pmem_memcpy will persist changes
	if (is_pmem)
		pmem_persist(pmemaddr, len);
	// pmem_memcpy_persist(pmemaddr, str, len);

	else
		pmem_msync(pmemaddr, len);

	printf("reading contents stored in pm: %s", pmem_str);
	/*
	 * Delete the mappings. The region is also
	 * automatically unmapped when the process is
	 * terminated.
	 */
	pmem_unmap(pmemaddr, mapped_len);
}
