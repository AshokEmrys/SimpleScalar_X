/*
 * =====================================================================================
 *
 *       Filename:  sim-outorder.hpp
 *
 *    Description:  interface for sim-outorder
 *
 *        Version:  0.0
 *        Created:  
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Ashok Kumar P (ParokshaX), paroksha.x@gmail.com
 *        Company:  
 *        License:  Private (Provisory)
 *
 * =====================================================================================
 */

#ifndef SIM_OUTRDER_HPP
#define	SIM_OUTRDER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <signal.h>


#include <iostream>


#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"

#include "loader.h"
#include "syscall.h"
#include "bpred.h"
#include "resource.h"
#include "bitmap.h"
#include "options.h"
#include "eval.h"
#include "stats.h"
#include "ptrace.h"
#include "dlite.h"
#include "sim.h"







void
sim_print_stats(FILE *fd);













/*Definitions*/
#define MAX_PCSTAT_VARS 8
#define FU_IALU_INDEX			0
#define FU_IMULT_INDEX			1
#define FU_MEMPORT_INDEX		2
#define FU_FPALU_INDEX			3
#define FU_FPMULT_INDEX			4
#define MAX_IDEPS               3
#define MAX_ODEPS               2
#define MAX_RS_LINKS                    4096
#define STORE_OP_INDEX                  0
#define STORE_ADDR_INDEX                1
#define RSLINK_NULL_DATA		{ NULL, NULL, 0 }
#define MAX_STD_UNKNOWNS		64
#define STORE_HASH_SIZE		32

#define OPERANDS_READY(RS)                                              \
  ((RS)->idep_ready[0] && (RS)->idep_ready[1] && (RS)->idep_ready[2])

#define STORE_OP_READY(RS)              ((RS)->idep_ready[STORE_OP_INDEX])
#define STORE_ADDR_READY(RS)            ((RS)->idep_ready[STORE_ADDR_INDEX])

#ifdef TARGET_PISA
#define IACOMPRESS(A)							\
  (compress_icache_addrs ? ((((A) - ld_text_base) >> 1) + ld_text_base) : (A))
#define ISCOMPRESS(SZ)							\
  (compress_icache_addrs ? ((SZ) >> 1) : (SZ))
#else 
#define IACOMPRESS(A)		(A)
#define ISCOMPRESS(SZ)		(SZ)
#endif 

#define STATVAL(STAT)							\
  ((STAT)->sc == sc_int							\
   ? (counter_t)*((STAT)->variant.for_int.var)			\
   : ((STAT)->sc == sc_uint						\
      ? (counter_t)*((STAT)->variant.for_uint.var)		\
      : ((STAT)->sc == sc_counter					\
	 ? *((STAT)->variant.for_counter.var)				\
	 : (panic("bad stat classM"), 0))))

#define RSLINK_INIT(RSL, RS)						\
  ((RSL).next = NULL, (RSL).rs = (RS), (RSL).tag = (RS)->tag)
#define RSLINK_IS_NULL(LINK)            ((LINK)->rs == NULL)
#define RSLINK_VALID(LINK)              ((LINK)->tag == (LINK)->rs->tag)
#define RSLINK_RS(LINK)                 ((LINK)->rs)
#define RSLINK_NEW(DST, RS)						\
  { struct RS_link *n_link;						\
    if (!rslink_free_list)						\
      panic("out of rs links");						\
    n_link = rslink_free_list;						\
    rslink_free_list = rslink_free_list->next;				\
    n_link->next = NULL;						\
    n_link->rs = (RS); n_link->tag = n_link->rs->tag;			\
    (DST) = n_link;							\
  }
#define RSLINK_FREE(LINK)						\
  {  struct RS_link *f_link = (LINK);					\
     f_link->rs = NULL; f_link->tag = 0;				\
     f_link->next = rslink_free_list;					\
     rslink_free_list = f_link;						\
  }
#define RSLINK_FREE_LIST(LINK)						\
  {  struct RS_link *fl_link, *fl_link_next;				\
     for (fl_link=(LINK); fl_link; fl_link=fl_link_next)		\
       {								\
	 fl_link_next = fl_link->next;					\
	 RSLINK_FREE(fl_link);						\
       }								\
  }
#define CVLINK_INIT(CV, RS,ONUM)	((CV).rs = (RS), (CV).odep_num = (ONUM))
#define CV_BMAP_SZ              (BITMAP_SIZE(MD_TOTAL_REGS))
#define CREATE_VECTOR(N)        (BITMAP_SET_P(use_spec_cv, CV_BMAP_SZ, (N))\
				 ? spec_create_vector[N]                \
				 : create_vector[N])
#define CREATE_VECTOR_RT(N)     (BITMAP_SET_P(use_spec_cv, CV_BMAP_SZ, (N))\
				 ? spec_create_vector_rt[N]             \
				 : create_vector_rt[N])
#define SET_CREATE_VECTOR(N, L) (spec_mode                              \
				 ? (BITMAP_SET(use_spec_cv, CV_BMAP_SZ, (N)),\
				    spec_create_vector[N] = (L))        \
				 : (create_vector[N] = (L)))
#define R_BMAP_SZ       (BITMAP_SIZE(MD_NUM_IREGS))
#define F_BMAP_SZ       (BITMAP_SIZE(MD_NUM_FREGS))
#define C_BMAP_SZ       (BITMAP_SIZE(MD_NUM_CREGS))
#define HASH_ADDR(ADDR)							\
  ((((ADDR) >> 24)^((ADDR) >> 16)^((ADDR) >> 8)^(ADDR)) & (STORE_HASH_SIZE-1))
#define DNA			(0)
#if defined(TARGET_PISA)
#define DGPR(N)			(N)
#define DGPR_D(N)		((N) &~1)
#define DFPR_L(N)		(((N)+32)&~1)
#define DFPR_F(N)		(((N)+32)&~1)
#define DFPR_D(N)		(((N)+32)&~1)
#define DHI			(0+32+32)
#define DLO			(1+32+32)
#define DFCC			(2+32+32)
#define DTMP			(3+32+32)
#elif defined(TARGET_ALPHA)
#define DGPR(N)			(31 - (N)) 
#define DFPR(N)			(((N) == 31) ? DNA : ((N)+32))
#define DFPCR			(0+32+32)
#define DUNIQ			(1+32+32)
#define DTMP			(2+32+32)
#else
#error No ISA target defined...
#endif
#define SET_NPC(EXPR)           (regs.regs_NPC = (EXPR))
#undef  SET_TPC
#define SET_TPC(EXPR)		(target_PC = (EXPR))
#define CPC                     (regs.regs_PC)
#define SET_CPC(EXPR)           (regs.regs_PC = (EXPR))
#define GPR(N)                  (BITMAP_SET_P(use_spec_R, R_BMAP_SZ, (N))\
				 ? spec_regs_R[N]                       \
				 : regs.regs_R[N])
#define SET_GPR(N,EXPR)         (spec_mode				\
				 ? ((spec_regs_R[N] = (EXPR)),		\
				    BITMAP_SET(use_spec_R, R_BMAP_SZ, (N)),\
				    spec_regs_R[N])			\
				 : (regs.regs_R[N] = (EXPR)))
#if defined(TARGET_PISA)
#define FPR_L(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? spec_regs_F.l[(N)]                   \
				 : regs.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)       (spec_mode				\
				 ? ((spec_regs_F.l[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    spec_regs_F.l[(N)])			\
				 : (regs.regs_F.l[(N)] = (EXPR)))
#define FPR_F(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? spec_regs_F.f[(N)]                   \
				 : regs.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)       (spec_mode				\
				 ? ((spec_regs_F.f[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    spec_regs_F.f[(N)])			\
				 : (regs.regs_F.f[(N)] = (EXPR)))
#define FPR_D(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? spec_regs_F.d[(N) >> 1]              \
				 : regs.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)       (spec_mode				\
				 ? ((spec_regs_F.d[(N) >> 1] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    spec_regs_F.d[(N) >> 1])		\
				 : (regs.regs_F.d[(N) >> 1] = (EXPR)))
#define HI			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, 0)\
				 ? spec_regs_C.hi			\
				 : regs.regs_C.hi)
#define SET_HI(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.hi = (EXPR)),		\
				    BITMAP_SET(use_spec_C, C_BMAP_SZ,0),\
				    spec_regs_C.hi)			\
				 : (regs.regs_C.hi = (EXPR)))
#define LO			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, 1)\
				 ? spec_regs_C.lo			\
				 : regs.regs_C.lo)
#define SET_LO(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.lo = (EXPR)),		\
				    BITMAP_SET(use_spec_C, C_BMAP_SZ,1),\
				    spec_regs_C.lo)			\
				 : (regs.regs_C.lo = (EXPR)))
#define FCC			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,2)\
				 ? spec_regs_C.fcc			\
				 : regs.regs_C.fcc)
#define SET_FCC(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.fcc = (EXPR)),		\
				    BITMAP_SET(use_spec_C,C_BMAP_SZ,2),\
				    spec_regs_C.fcc)			\
				 : (regs.regs_C.fcc = (EXPR)))
#elif defined(TARGET_ALPHA)
#define FPR_Q(N)		(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N))\
				 ? spec_regs_F.q[(N)]                   \
				 : regs.regs_F.q[(N)])
#define SET_FPR_Q(N,EXPR)	(spec_mode				\
				 ? ((spec_regs_F.q[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)),\
				    spec_regs_F.q[(N)])			\
				 : (regs.regs_F.q[(N)] = (EXPR)))
#define FPR(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N))\
				 ? spec_regs_F.d[(N)]			\
				 : regs.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(spec_mode				\
				 ? ((spec_regs_F.d[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)),\
				    spec_regs_F.d[(N)])			\
				 : (regs.regs_F.d[(N)] = (EXPR)))
#define FPCR			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,0)\
				 ? spec_regs_C.fpcr			\
				 : regs.regs_C.fpcr)
#define SET_FPCR(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.fpcr = (EXPR)),	\
				   BITMAP_SET(use_spec_C,C_BMAP_SZ,0),\
				    spec_regs_C.fpcr)			\
				 : (regs.regs_C.fpcr = (EXPR)))
#define UNIQ			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,1)\
				 ? spec_regs_C.uniq			\
				 : regs.regs_C.uniq)
#define SET_UNIQ(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.uniq = (EXPR)),	\
				   BITMAP_SET(use_spec_C,C_BMAP_SZ,1),\
				    spec_regs_C.uniq)			\
				 : (regs.regs_C.uniq = (EXPR)))
#define FCC			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,2)\
				 ? spec_regs_C.fcc			\
				 : regs.regs_C.fcc)
#define SET_FCC(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.fcc = (EXPR)),		\
				    BITMAP_SET(use_spec_C,C_BMAP_SZ,1),\
				    spec_regs_C.fcc)			\
				 : (regs.regs_C.fcc = (EXPR)))
#else
#error No ISA target defined...
#endif
#define __READ_SPECMEM(SRC, SRC_V, FAULT)				\
  (addr = (SRC),							\
   (spec_mode								\
    ? ((FAULT) = spec_mem_access(mem, Read, addr, &SRC_V, sizeof(SRC_V)))\
    : ((FAULT) = mem_access(mem, Read, addr, &SRC_V, sizeof(SRC_V)))),	\
   SRC_V)
#define READ_BYTE(SRC, FAULT)						\
  __READ_SPECMEM((SRC), temp_byte, (FAULT))
#define READ_HALF(SRC, FAULT)						\
  MD_SWAPH(__READ_SPECMEM((SRC), temp_half, (FAULT)))
#define READ_WORD(SRC, FAULT)						\
  MD_SWAPW(__READ_SPECMEM((SRC), temp_word, (FAULT)))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)						\
  MD_SWAPQ(__READ_SPECMEM((SRC), temp_qword, (FAULT)))
#endif 
#define __WRITE_SPECMEM(SRC, DST, DST_V, FAULT)				\
  (DST_V = (SRC), addr = (DST),						\
   (spec_mode								\
    ? ((FAULT) = spec_mem_access(mem, Write, addr, &DST_V, sizeof(DST_V)))\
    : ((FAULT) = mem_access(mem, Write, addr, &DST_V, sizeof(DST_V)))))
#define WRITE_BYTE(SRC, DST, FAULT)					\
  __WRITE_SPECMEM((SRC), (DST), temp_byte, (FAULT))
#define WRITE_HALF(SRC, DST, FAULT)					\
  __WRITE_SPECMEM(MD_SWAPH(SRC), (DST), temp_half, (FAULT))
#define WRITE_WORD(SRC, DST, FAULT)					\
  __WRITE_SPECMEM(MD_SWAPW(SRC), (DST), temp_word, (FAULT))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)					\
  __WRITE_SPECMEM(MD_SWAPQ(SRC), (DST), temp_qword, (FAULT))
#endif 
#define SYSCALL(INST)							\
  (		\
   (spec_mode ? panic("speculative syscall") : (void) 0),		\
   sys_syscall(&regs, mem_access, mem, INST, TRUE))


//Typedefs

typedef unsigned int INST_TAG_TYPE;
typedef unsigned int INST_SEQ_TYPE;




struct RUU_station {
  md_inst_t IR;			
  enum md_opcode op;			
  md_addr_t PC, next_PC, pred_PC;	
  int in_LSQ;				
  int ea_comp;				
  int recover_inst;			
  int stack_recover_idx;		
  struct bpred_update_t dir_update;	
  int spec_mode;			
  md_addr_t addr;			
  INST_TAG_TYPE tag;			
  INST_SEQ_TYPE seq;			
  unsigned int ptrace_seq;		
  int slip;
  int queued;				
  int issued;				
  int completed;			
  int onames[MAX_ODEPS];		
  struct RS_link *odep_list[MAX_ODEPS];	
  int idep_ready[MAX_IDEPS];		
};

struct RS_link {
  struct RS_link *next;			
  struct RUU_station *rs;		
  INST_TAG_TYPE tag;			
  union {
    tick_t when;			
    INST_SEQ_TYPE seq;			
    int opnum;				
  } x;
  RS_link()
  {
  next = NULL;
  rs = NULL;
  tag = 0;
  }
};

struct CV_link {
  struct RUU_station *rs;               
  int odep_num;
  CV_link()
  {
  rs = NULL;
  odep_num = 0;
  }
};

struct spec_mem_ent {
  struct spec_mem_ent *next;		
  md_addr_t addr;			
  unsigned int data[2];			
};

struct fetch_rec {
  md_inst_t IR;				
  md_addr_t regs_PC, pred_PC;		
  struct bpred_update_t dir_update;	
  int stack_recover_idx;		
  unsigned int ptrace_seq;		
};

class simoutorder
{
public:
 struct regs_t regs;
 struct mem_t *mem = NULL;
 unsigned int max_insts;
 int fastfwd_count;
 int ptrace_nelt;
 char *ptrace_opts[2];
 int ruu_ifq_size;
 int ruu_branch_penalty;
 int fetch_speed;
 char *pred_type = NULL;
 int bimod_nelt;
 int bimod_config[1] ={2048 };
 int twolev_nelt;
 int twolev_config[4] = { 1, 1024, 8, FALSE};
 int comb_nelt;
 int comb_config[1] = { 1024 };
 int ras_size;
 int btb_nelt;
 int btb_config[2] = { 512, 4 };
 int ruu_decode_width;
 int ruu_issue_width;
 int ruu_inorder_issue;
 int ruu_include_spec;
 int ruu_commit_width;
 int RUU_size;
 int LSQ_size;
 char *cache_dl1_opt = NULL;
 int cache_dl1_lat;
 char *cache_dl2_opt = NULL;
 int cache_dl2_lat;
 char *cache_il1_opt = NULL;
 int cache_il1_lat;
 char *cache_il2_opt = NULL;
 int cache_il2_lat;
 int flush_on_syscalls;
 int compress_icache_addrs;
 int mem_nelt;
 int mem_lat[2] = { 18, 2 };
 int mem_bus_width;
 char *itlb_opt = NULL;
 char *dtlb_opt = NULL;
 int tlb_miss_lat;
 int res_ialu;
 int res_imult;
 int res_memport;
 int res_fpalu;
 int res_fpmult;
 counter_t sim_slip;
 counter_t sim_total_insn;
 counter_t sim_num_refs;
 counter_t sim_total_refs;
 counter_t sim_num_loads;
 counter_t sim_total_loads;
 counter_t sim_num_branches;
 counter_t sim_total_branches;
 tick_t sim_cycle;
 counter_t IFQ_count;		
 counter_t IFQ_fcount;		
 counter_t RUU_count;		
 counter_t RUU_fcount;		
 counter_t LSQ_count;		
 counter_t LSQ_fcount;		
 counter_t sim_invalid_addrs;
 unsigned int inst_seq;
 unsigned int ptrace_seq;
 int spec_mode = FALSE;
 unsigned ruu_fetch_issue_delay;
 int pred_perfect;
 char *bpred_spec_opt = NULL;
 enum { spec_ID, spec_WB, spec_CT } bpred_spec_update;
 struct cache_t *cache_il1 = NULL;
 struct cache_t *cache_il2 = NULL;
 struct cache_t *cache_dl1 = NULL;
 struct cache_t *cache_dl2 = NULL;
 struct cache_t *itlb = NULL;
 struct cache_t *dtlb = NULL;
 struct bpred_t *pred = NULL;
 struct res_pool *fu_pool = NULL;
 struct stat_stat_t *pcstat_stats[MAX_PCSTAT_VARS];
 counter_t pcstat_lastvals[MAX_PCSTAT_VARS];
 struct stat_stat_t *pcstat_sdists[MAX_PCSTAT_VARS];
 
 int pcstat_nelt;
 char *pcstat_vars[MAX_PCSTAT_VARS];
 int bugcompat_mode;
 
 struct RUU_station *RUU = NULL;
 int RUU_head, RUU_tail;
 int RUU_num;
 struct RUU_station *LSQ = NULL; 
 int LSQ_head, LSQ_tail;  
 int LSQ_num;  
 struct RS_link *rslink_free_list = NULL;
 struct RS_link RSLINK_NULL;
 struct RS_link *event_queue = NULL;
 struct RS_link *ready_queue = NULL;
 struct CV_link CVLINK_NULL;
 #define BITMAP_SIZE(BITS)	(((BITS)+31)/32)
#define BITMAP_TYPE(BITS, NAME)	unsigned int (NAME)[BITMAP_SIZE(BITS)]
 BITMAP_TYPE(MD_TOTAL_REGS, use_spec_cv);
 struct CV_link create_vector[MD_TOTAL_REGS];
 struct CV_link spec_create_vector[MD_TOTAL_REGS];
 tick_t create_vector_rt[MD_TOTAL_REGS];
 tick_t spec_create_vector_rt[MD_TOTAL_REGS];
 BITMAP_TYPE(MD_NUM_IREGS, use_spec_R);
 md_gpr_t spec_regs_R;
 BITMAP_TYPE(MD_NUM_FREGS, use_spec_F);
 md_fpr_t spec_regs_F;
 BITMAP_TYPE(MD_NUM_FREGS, use_spec_C);
 md_ctrl_t spec_regs_C;
 struct spec_mem_ent *store_htable[STORE_HASH_SIZE];
 struct spec_mem_ent *bucket_free_list = NULL;
 md_addr_t pred_PC;
 md_addr_t recover_PC;
 md_addr_t fetch_regs_PC;
 md_addr_t fetch_pred_PC;
 struct fetch_rec *fetch_data = NULL;
 int fetch_num;
 int fetch_tail, fetch_head;
 struct RS_link last_op;
 int last_inst_missed = FALSE;
 int last_inst_tmissed = FALSE;
 
 
 
 
 simoutorder();
 unsigned int mem_access_latency(int blk_sz);
 
 unsigned int			
 dl1_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now);
 
unsigned int			
dl2_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now);
 
unsigned int			
il1_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now);

unsigned int			
il2_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now);

unsigned int			
itlb_access_fn(enum mem_cmd cmd,	
	       md_addr_t baddr,		
	       int bsize,		
	       struct cache_blk_t *blk,	
	       tick_t now);

unsigned int			
dtlb_access_fn(enum mem_cmd cmd,	
	       md_addr_t baddr,	
	       int bsize,		
	       struct cache_blk_t *blk,	
	       tick_t now);
void
sim_reg_options(struct opt_odb_t *odb);

void
sim_check_options(struct opt_odb_t *odb,        
		  int argc, char **argv);

void
sim_aux_config(FILE *stream);

void
sim_reg_stats(struct stat_sdb_t *sdb);

void
sim_init(void);

void
sim_load_prog(char *fname,		
	      int argc, char **argv,	
	      char **envp);

void
sim_aux_stats(FILE *stream);

void
sim_uninit(void);


void
ruu_init(void);

void
ruu_dumpent(struct RUU_station *rs,		
	    int index,				
	    FILE *stream,			
	    int header);

void
ruu_dump(FILE *stream);

void
lsq_init(void);

void
lsq_dump(FILE *stream);

void
rslink_init(int nlinks);

void
ruu_release_fu(void);

void
eventq_init(void);

void
eventq_dump(FILE *stream);

void
eventq_queue_event(struct RUU_station *rs, tick_t when);

struct RUU_station *
eventq_next_event(void);

void
readyq_init(void);

void
readyq_dump(FILE *stream);

void
readyq_enqueue(struct RUU_station *rs);

void
cv_init(void);

void
cv_dump(FILE *stream);

void
ruu_commit(void);

void
ruu_recover(int branch_index);

void
ruu_writeback(void);

void
lsq_refresh(void);

void
ruu_issue(void);

void
rspec_dump(FILE *stream);

void
tracer_recover(void);

void
tracer_init(void);

enum md_fault_type
spec_mem_access(struct mem_t *mem,		
		enum mem_cmd cmd,		
		md_addr_t addr,			
		void *p,			
		int nbytes);

void
mspec_dump(FILE *stream);

char *					
simoo_mem_obj(struct mem_t *mem,		
	      int is_write,			
	      md_addr_t addr,			
	      char *p,				
	      int nbytes);

void
ruu_link_idep(struct RUU_station *rs,		
	      int idep_num,			
	      int idep_name);

void
ruu_install_odep(struct RUU_station *rs,	
		 int odep_num,			
		 int odep_name);

char *					
simoo_reg_obj(struct regs_t *xregs,		
	      int is_write,			
	      enum md_reg_type rt,		
	      int reg,				
	      struct eval_value_t *val);

void
ruu_dispatch(void);

void
fetch_init(void);

void
fetch_dump(FILE *stream);

void
ruu_fetch(void);

char *					
simoo_mstate_obj(FILE *stream,			
		 char *cmd,			
		 struct regs_t *regs,		
		 struct mem_t *mem)	;

void
sim_main(void);


 
};

#endif	/* SIM_OUTRDER_HPP */

