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

#include<thread>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "cache.h"
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
	 : (panic("bad stat class"), 0))))

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




//Typedefs

typedef unsigned int INST_TAG_TYPE;
typedef unsigned int INST_SEQ_TYPE;


struct res_desc fu_config[] = {
  {
    "integer-ALU",
    4,
    0,
    {
      { IntALU, 1, 1 }
    }
  },
  {
    "integer-MULT/DIV",
    1,
    0,
    {
      { IntMULT, 3, 1 },
      { IntDIV, 20, 19 }
    }
  },
  {
    "memory-port",
    2,
    0,
    {
      { RdPort, 1, 1 },
      { WrPort, 1, 1 }
    }
  },
  {
    "FP-adder",
    4,
    0,
    {
      { FloatADD, 2, 1 },
      { FloatCMP, 2, 1 },
      { FloatCVT, 2, 1 }
    }
  },
  {
    "FP-MULT/DIV",
    1,
    0,
    {
      { FloatMULT, 4, 1 },
      { FloatDIV, 12, 12 },
      { FloatSQRT, 24, 24 }
    }
  },
};


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
  x = 0;
  }
};

struct CV_link {
  struct RUU_station *rs;               
  int odep_num;
  CV_link()
  {
  RUU_station = NULL;
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
 struct mem_t *mem;
 unsigned int max_insts;
 int fastfwd_count;
 int ptrace_nelt;
 char *ptrace_opts[2];
 int ruu_ifq_size;
 int ruu_branch_penalty;
 int fetch_speed;
 char *pred_type;
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
 char *cache_dl1_opt;
 int cache_dl1_lat;
 char *cache_dl2_opt;
 int cache_dl2_lat;
 char *cache_il1_opt;
 int cache_il1_lat;
 char *cache_il2_opt;
 int cache_il2_lat;
 int flush_on_syscalls;
 int compress_icache_addrs;
 int mem_nelt;
 int mem_lat[2] = { 18, 2 };
 int mem_bus_width;
 char *itlb_opt;
 char *dtlb_opt;
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
 char *bpred_spec_opt;
 enum { spec_ID, spec_WB, spec_CT } bpred_spec_update;
 struct cache_t *cache_il1;
 struct cache_t *cache_il2;
 struct cache_t *cache_dl1;
 struct cache_t *cache_dl2;
 struct cache_t *itlb;
 struct cache_t *dtlb;
 struct bpred_t *pred;
 struct res_pool *fu_pool;
 struct stat_stat_t *pcstat_stats[MAX_PCSTAT_VARS];
 counter_t pcstat_lastvals[MAX_PCSTAT_VARS];
 struct stat_stat_t *pcstat_sdists[MAX_PCSTAT_VARS];
 
 int pcstat_nelt;
 char *pcstat_vars[MAX_PCSTAT_VARS];
 int bugcompat_mode;
 
 struct RUU_station *RUU;
 int RUU_head, RUU_tail;
 int RUU_num;
 struct RUU_station *LSQ; 
 int LSQ_head, LSQ_tail;  
 int LSQ_num;  
 struct RS_link *rslink_free_list;
 struct RS_link RSLINK_NULL;
 struct RS_link *event_queue;
 struct RS_link *ready_queue;
 struct CV_link CVLINK_NULL;
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
 struct spec_mem_ent *bucket_free_list;
 md_addr_t pred_PC;
 md_addr_t recover_PC;
 md_addr_t fetch_regs_PC;
 md_addr_t fetch_pred_PC;
 struct fetch_rec *fetch_data;
 int fetch_num;
 int fetch_tail, fetch_head;
 
 
 
 
 
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

static struct RUU_station *
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




 
};


#endif	/* SIM_OUTRDER_HPP */

