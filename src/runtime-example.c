/* Reference I/O and program-lifetime heap allocation for RV32/ILP32. */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef char rx_int32_is_four_bytes[(sizeof(int32_t) == 4) ? 1 : -1];
typedef char rx_uint32_is_four_bytes[(sizeof(uint32_t) == 4) ? 1 : -1];

extern int printf(const char *format, ...);
extern int scanf(const char *format, ...);
extern void *malloc(uint32_t size);

/*
 * Valid course allocations have nonzero size and alignment 1 or 4. REIMU's
 * malloc result satisfies both alignments. Objects are not individually freed;
 * the execution environment reclaims the whole heap when the program ends.
 */
void *__rx_alloc(uint32_t size, uint32_t align) {
  (void)align;
  return malloc(size);
}

int32_t getInt(void) {
  int32_t value;
  (void)scanf("%d", &value);
  return value;
}

void printInt(int32_t value) { (void)printf("%d", value); }

void printlnInt(int32_t value) { (void)printf("%d\n", value); }
