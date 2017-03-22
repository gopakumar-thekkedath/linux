#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
/*
 * Vector 55
 */
#define LAPIC_START	0xfee00000
#define ICR_LO		0x300
#define ICR_HI		0x310

enum delivery_mode {
    FIXED       = 0x0,
    LOWEST_PRIO = 0x1,
    SMI         = 0x2,
    NMI         = 0x4,
    INIT        = 0x5,
    STARTUP     = 0x6
};

enum destination_mode {
    PHYSICAL = 0,
    LOGICAL  = 1
};

enum trigger_mode {
    EDGE = 0,
    LEVEL    = 1
};

enum level {
    DE_ASSERT = 0,
    ASSERT = 1
};

enum dest_shand {
    SHAND_NONE          = 0x0,
    IPI_SELF            = 0x1,
    IPI_ALL             = 0x2,
    IPI_ALL_BUT_SELF    = 0x3
};

void
generate_ipi(uint8_t dest, uint8_t vector, uint8_t del_mode, uint8_t dest_mode,
            uint8_t trig_mode, uint8_t shand, void *addr)
{
    uint32_t icr_low = 0x0, icr_high = 0x0;

    icr_low |= vector;
    icr_low |= (del_mode << 8);
    icr_low |= (ASSERT << 14);

    if (del_mode == FIXED || del_mode == LOWEST_PRIO) {
        icr_low |= (dest_mode << 11);
        icr_low |= (trig_mode << 15);
    }

	printf("\n Triggering interrupt %u", vector);
    
	if (shand == SHAND_NONE) {
        icr_high |= ((uint32_t)dest << 24);
        *((volatile uint32_t *)((uint64_t)addr + ICR_HI)) =  icr_high;
    } else {
        icr_low |= (shand << 18);
    }

	*((volatile uint32_t *)((uint64_t)addr + ICR_LO)) =  icr_low;
}

static void
generate_interrupt(uint8_t vec, void *addr)
{
	if (vec < 32) {
		printf("\n Please provide vector between 32-255");
		return;
	}

	generate_ipi(0, vec, FIXED, PHYSICAL, EDGE, SHAND_NONE, addr);
}

int
main(int argc, char *argv[])
{

	void *addr;
	uint8_t vector;
	int fd, i;

	if (argc != 2) {
		printf("\nInvalid argument count");
		printf("\n Usage: lapci vector_number");
		return -1;
	}

	vector = (uint8_t)strtol(argv[1], NULL, 10);


	fd = open("/dev/mem", O_RDWR);

	 if (!fd) {
        printf("\nError: Unable to open /dev/mem");
        return -1;
    }

	printf("\n mmap physical address :%lx", LAPIC_START);
	addr =  mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, LAPIC_START);	

	 if (addr == MAP_FAILED) {
        printf("\nFailed to mmap physical address:%lx", LAPIC_START);
        return -1;
    }

	printf("\n%x", *((volatile uint32_t*)addr));
	generate_interrupt(vector, addr);
	printf("\n");
	close(fd);
	return 0;
}
