#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

uint64_t get_msr_value (int cpu, uint32_t reg, unsigned int highbit,
			unsigned int lowbit, int* error_indx)
{
    uint64_t data;
    int fd;
    //  char *pat;
    //  int width;
    char msr_file_name[64];
    int bits;
    *error_indx =0;

    sprintf (msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open (msr_file_name, O_RDONLY);
    if (fd < 0)
    {
	if (errno == ENXIO)
	{
	    //fprintf (stderr, "rdmsr: No CPU %d\n", cpu);
	    *error_indx = 1;
	    return 1;
	} else if (errno == EIO) {
	    //fprintf (stderr, "rdmsr: CPU %d doesn't support MSRs\n", cpu);
	    *error_indx = 1;
	    return 1;
	} else {
	    //perror ("rdmsr:open");
	    *error_indx = 1;
	    return 1;
	    //exit (127);
	}
    }

    if (pread (fd, &data, sizeof data, reg) != sizeof data)
    {
	perror ("rdmsr:pread");
	exit (127);
    }

    close (fd);

    bits = highbit - lowbit + 1;
    if (bits < 64)
    {
	/* Show only part of register */
	data >>= lowbit;
	data &= (1ULL << bits) - 1;
    }

    /* Make sure we get sign correct */
    if (data & (1ULL << (bits - 1)))
    {
	data &= ~(1ULL << (bits - 1));
	data = -data;
    }

    *error_indx = 0;
    return (data);
}

int main()
{
	while (1)
	{
		int error_indx;
		uint64_t v = get_msr_value(0, 0x198, 47, 32, &error_indx);
		if (error_indx == 0)
		{
			float vcore = v/(float)8192;
			printf("vcore: %f\n", vcore);
		}
		else
		{
			printf("cannot access msr (did you run this program as su?)\n");
            return 1;
		}
		sleep(1);
	}
	return 0;
}

