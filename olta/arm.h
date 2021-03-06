#ifndef __ARM_H
#define __ARM_H 1

#define ish "ish"
#define ishld "ishld"
#define ishst "ishst"

#define dsb(opt)    asm volatile("dsb " #opt : : : "memory")

typedef enum _reg_t {
    X0 = 0,
    X1 = 1,
    X2 = 2,
    X3 = 3,
    X4 = 4,
    X5 = 5,
    X6 = 6,
    X7 = 7,
    X8 = 8,
    X9 = 9,
    X10 = 10,
    X11 = 11,
    X12 = 12,
    X13 = 13,
    X14 = 14,
    X15 = 15,
    X16 = 16,
    X17 = 17,
    X18 = 18,
    X19 = 19,
    X20 = 20,
    X21 = 21,
    X22 = 22,
    X23 = 23,
    X24 = 24,
    X25 = 25,
    X26 = 26,
    X27 = 27,
    X28 = 28,
    X29 = 29,
    X30 = 30,
    XZR = 31,
    XINVALID = 0xffffffff
} reg_t;

typedef enum _shift_t {
    SHIFT_LSL = 0x0,
    SHIFT_LSR = 0x1,
    SHIFT_ASR = 0x2,
    SHIFT_ROR = 0x3
} shift_t;

typedef enum _cond_t {
    CC_EQ = 0x0,
    CC_NE = 0x1,
    CC_CS = 0x2,
    CC_CC = 0x3,
    CC_MI = 0x4,
    CC_PL = 0x5,
    CC_VS = 0x6,
    CC_VC = 0x7,
    CC_HI = 0x8,
    CC_LS = 0x9,
    CC_GE = 0xA,
    CC_LT = 0xB,
    CC_GT = 0xC,
    CC_LE = 0xD,
    CC_AL = 0xE,
    CC_NV = 0xF
} cond_t;

typedef enum _prfop_t {
    PLDL1KEEP = 0x00,
    PLDL1STRM = 0x01,
    PLDL2KEEP = 0x02,
    PLDL2STRM = 0x03,
    PLDL3KEEP = 0x04,
    PLDL3STRM = 0x05,
    PLIL1KEEP = 0x08,
    PLIL1STRM = 0x09,
    PLIL2KEEP = 0x0a,
    PLIL2STRM = 0x0b,
    PLIL3KEEP = 0x0c,
    PLIL3STRM = 0x0d,
    PSTL1KEEP = 0x10,
    PSTL1STRM = 0x11,
    PSTL2KEEP = 0x12,
    PSTL2STRM = 0x13,
    PSTL3KEEP = 0x14,
    PSTL3STRM = 0x15
} prfop_t;

typedef enum _bar_type_t {
    BAR_DSB = 0x0,
    BAR_DMB = 0x1,
    BAR_ISB = 0x2
} bar_type_t;

typedef enum _bar_domain_t {
    BAR_OUTER_SHAREABLE  = 0x0,
    BAR_NON_SHAREABLE    = 0x1,
    BAR_INNER_SHAREABLE  = 0x2,
    BAR_FULL_SYSTEM      = 0x3
} bar_domain_t;

typedef enum _bar_req_t {
    BAR_READS   = 0x1,
    BAR_WRITES  = 0x2,
    BAR_ALL     = 0x3
} bar_req_t;

typedef enum _addr_dep_type {
    ADDR_DEP_ADDSUB
} addr_dep_type;

typedef struct _asm_label_t {
    const char *name;
    int pos;
} asm_label_t;

typedef struct _asm_branch_t {
    ins_desc_t *ins;
    int pos;
} asm_branch_t;

#define MAX_ASM_LABEL 2
#define MAX_ASM_BRANCH 2
typedef struct _asm_ctx_t {
    uint32_t *buf;
    int idx;
    /* config */
    addr_dep_type addr_dep;
    litmus_t *test;
    tthread_t *thread;
    thread_ctx_t *thread_ctx;
    /* registers */
    reg_t r_bootrec;
    reg_t r_tmp0, r_tmp1, r_tmp2;
    reg_t r_stall;
    reg_t r_sync_a, r_sync_b;
    reg_t r_threads, r_iterations;
    reg_t r_idx;
    reg_t r_buf_unq_idx;
    reg_t r_buf_shr_idx;
    reg_t r_ts_start, r_ts_end;
    reg_t r_reg[MAX_THREAD_REG];
    /* labels */
    int l_head;
    int n_label;
    asm_label_t label[MAX_ASM_LABEL];
    int n_branch;
    asm_branch_t branch[MAX_ASM_BRANCH];
} asm_ctx_t;


__attribute__ ((unused))
static inline void flush_dcache(void *addr) {
#if defined(ARM_HOST)
    asm volatile (
            "\tdc  civac, %[addr]\n"
            : : [addr] "r" (addr) : "memory"
    );
#endif
}

__attribute__ ((unused))
static inline void full_barrier(void) {
#if defined(ARM_HOST)
    asm __volatile__ ("dsb sy" ::: "memory");
#endif
}

__attribute__ ((unused))
static inline void isb(void) {
#if defined(ARM_HOST)
    asm volatile ("isb" ::: "memory");
#endif
}

__attribute__ ((unused))
static inline void barrier(void) {
#if defined(ARM_HOST)
    asm __volatile__ ("dsb sy" ::: "memory");
#endif
}

static inline void atomic_inc(volatile int64_t *addr) 
{                                   
#if defined(ARM_HOST)
    int64_t result;                            
    int tmp;
    asm volatile("\n\t"
            "1: ldxr    %0, [%2]\n\t"
            "   add     %0, %0, %3\n\t"
            "   stxr    %w1, %0, [%2]\n\t"                   
            "   cbnz    %w1, 1b\n\t"
            : "=r" (result), "=r" (tmp)
            : "r" (addr), "Ir" (1)
            : "memory");
#endif
} 

static inline void atomic_dec(volatile int64_t *addr) 
{                                   
#if defined(ARM_HOST)
    long result;
    int tmp;
    asm volatile("\n\t"
            "1: ldxr    %0, [%2]\n\t"
            "   sub     %0, %0, %3\n\t"
            "   stxr    %w1, %0, [%2]\n\t"                   
            "   cbnz    %w1, 1b\n\t"
            : "=r" (result), "=r" (tmp)
            : "r" (addr), "Ir" (1)
            : "memory");
#endif
} 

static inline int64_t atomic_read(volatile int64_t *addr) 
{                                   
#if defined(ARM_HOST)
    int64_t result;                            
    asm volatile("\n\t"
            "ldr    %0, [%1]\n\t"
            : "=r" (result)
            : "r" (addr)
            : "memory");
    return result;
#else
    return 0;
#endif
} 

static inline void atomic_set(volatile int64_t *addr, int64_t v) 
{                                   
#if defined(ARM_HOST)
    asm volatile("\n\t"
            "str %0, [%1]\n\t"
            :
            : "r" (v), "r" (addr)
            : "memory");
#endif
} 

void *build_thread_code(litmus_t *test, tthread_t *th, thread_ctx_t *thread_ctx);
void free_thread_code(void *code);
void *boot_thread(void *t);
int has_pmu_access(void);

#endif /* __ARM_H */
