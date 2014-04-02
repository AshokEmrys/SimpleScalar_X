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

#include "sim-outorder.hpp"

/*Sim Outorder Class Definition*/

simoutorder::simoutorder()
{
mem = NULL;
ptrace_nelt = 0;
bimod_nelt = 1;
bimod_config[0] = 2048;
twolev_nelt = 4;
comb_nelt = 1;
ras_size = 8;
btb_nelt = 2;
ruu_include_spec = TRUE;
RUU_size = 8;
LSQ_size = 4;
mem_nelt = 2;
pcstat_nelt = 0;
sim_slip = 0;
sim_total_insn = 0;
sim_num_refs = 0;
sim_total_refs = 0;
sim_num_loads = 0;
sim_total_loads = 0;
sim_num_branches = 0;
sim_total_branches = 0;
sim_cycle = 0;
inst_seq = 0;
ptrace_seq = 0;
ruu_fetch_issue_delay = 0;
pred_perfect = FALSE;
fu_pool = NULL;
}


unsigned int simoutorder::mem_access_latency(int blk_sz)		
{
  int chunks = (blk_sz + (mem_bus_width - 1)) / mem_bus_width;
  assert(chunks > 0);
  return (mem_lat[0] +
	  (mem_lat[1] * (chunks - 1)));
}


unsigned int			
simoutorder::dl1_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now)		
{
  unsigned int lat;
  if (cache_dl2)
    {
      lat = cache_access(cache_dl2, cmd, baddr, NULL, bsize,
			 now, NULL, NULL);
      if (cmd == Read)
	return lat;
      else
	{
	  return 0;
	}
    }
  else
    {
      if (cmd == Read)
	return mem_access_latency(bsize);
      else
	{
	  return 0;
	}
    }
}


unsigned int			
simoutorder::dl2_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now)		
{
  if (cmd == Read)
    return mem_access_latency(bsize);
  else
    {
      return 0;
    }
}

unsigned int			
simoutorder::il1_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now)		
{
  unsigned int lat;
if (cache_il2)
    {
      lat = cache_access(cache_il2, cmd, baddr, NULL, bsize,
			 now, NULL, NULL);
      if (cmd == Read)
	return lat;
      else
	panic("writes to instruction memory not supported");
    }
  else
    {
      if (cmd == Read)
	return mem_access_latency(bsize);
      else
	panic("writes to instruction memory not supported");
    }
}

unsigned int			
simoutorder::il2_access_fn(enum mem_cmd cmd,		
	      md_addr_t baddr,		
	      int bsize,		
	      struct cache_blk_t *blk,	
	      tick_t now)		
{
  if (cmd == Read)
    return mem_access_latency(bsize);
  else
    panic("writes to instruction memory not supported");
}

unsigned int			
simoutorder::itlb_access_fn(enum mem_cmd cmd,	
	       md_addr_t baddr,		
	       int bsize,		
	       struct cache_blk_t *blk,	
	       tick_t now)		
{
  md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;
  assert(phy_page_ptr);
  *phy_page_ptr = 0;
  return tlb_miss_lat;
}

unsigned int			
simoutorder::dtlb_access_fn(enum mem_cmd cmd,	
	       md_addr_t baddr,	
	       int bsize,		
	       struct cache_blk_t *blk,	
	       tick_t now)		
{
  md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;
  assert(phy_page_ptr);
  *phy_page_ptr = 0;
  return tlb_miss_lat;
}

void
simoutorder::sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-outorder: This simulator implements a very detailed out-of-order issue\n"
"superscalar processor with a two-level memory system and speculative\n"
"execution support.  This simulator is a performance simulator, tracking the\n"
"latency of all pipeline operations.\n"
		 );
  opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
	       &max_insts, 0,
	       TRUE, NULL);
  opt_reg_int(odb, "-fastfwd", "number of insts skipped before timing starts",
	      &fastfwd_count, 0,
	      TRUE, NULL);
  opt_reg_string_list(odb, "-ptrace",
	      "generate pipetrace, i.e., <fname|stdout|stderr> <range>",
	      ptrace_opts, 2, &ptrace_nelt, NULL,
	      FALSE, NULL, FALSE);
  opt_reg_note(odb,
"  Pipetrace range arguments are formatted as follows:\n"
"\n"
"    {{@|#}<start>}:{{@|#|+}<end>}\n"
"\n"
"  Both ends of the range are optional, if neither are specified, the entire\n"
"  execution is traced.  Ranges that start with a `@' designate an address\n"
"  range to be traced, those that start with an `#' designate a cycle count\n"
"  range.  All other range values represent an instruction count range.  The\n"
"  second argument, if specified with a `+', indicates a value relative\n"
"  to the first argument, e.g., 1000:+100 == 1000:1100.  Program symbols may\n"
"  be used in all contexts.\n"
"\n"
"    Examples:   -ptrace FOO.trc #0:#1000\n"
"                -ptrace BAR.trc @2000:\n"
"                -ptrace BLAH.trc :1500\n"
"                -ptrace UXXE.trc :\n"
"                -ptrace FOOBAR.trc @main:+278\n"
	       );
  opt_reg_int(odb, "-fetch:ifqsize", "instruction fetch queue size (in insts)",
	      &ruu_ifq_size, 4,
	      TRUE, NULL);
  opt_reg_int(odb, "-fetch:mplat", "extra branch mis-prediction latency",
	      &ruu_branch_penalty, 3,
	      TRUE, NULL);
  opt_reg_int(odb, "-fetch:speed",
	      "speed of front-end of machine relative to execution core",
	      &fetch_speed, 1,
	      TRUE, NULL);
  opt_reg_note(odb,
"  Branch predictor configuration examples for 2-level predictor:\n"
"    Configurations:   N, M, W, X\n"
"      N   # entries in first level (# of shift register(s))\n"
"      W   width of shift register(s)\n"
"      M   # entries in 2nd level (# of counters, or other FSM)\n"
"      X   (yes-1/no-0) xor history and address for 2nd level index\n"
"    Sample predictors:\n"
"      GAg     : 1, W, 2^W, 0\n"
"      GAp     : 1, W, M (M > 2^W), 0\n"
"      PAg     : N, W, 2^W, 0\n"
"      PAp     : N, W, M (M == 2^(N+W)), 0\n"
"      gshare  : 1, W, 2^W, 1\n"
"  Predictor `comb' combines a bimodal and a 2-level predictor.\n"
               );
  opt_reg_string(odb, "-bpred",
		 "branch predictor type {nottaken|taken|perfect|bimod|2lev|comb}",
                 &pred_type, "bimod",
                 TRUE, NULL);
  opt_reg_int_list(odb, "-bpred:bimod",
		   "bimodal predictor config (<table size>)",
		   bimod_config, bimod_nelt, &bimod_nelt,
		   bimod_config,
		   TRUE, NULL, FALSE);
  opt_reg_int_list(odb, "-bpred:2lev",
                   "2-level predictor config "
		   "(<l1size> <l2size> <hist_size> <xor>)",
                   twolev_config, twolev_nelt, &twolev_nelt,
		   twolev_config,
                   TRUE, NULL, FALSE);
  opt_reg_int_list(odb, "-bpred:comb",
		   "combining predictor config (<meta_table_size>)",
		   comb_config, comb_nelt, &comb_nelt,
		   comb_config,
		   TRUE, NULL, FALSE);
  opt_reg_int(odb, "-bpred:ras",
              "return address stack size (0 for no return stack)",
              &ras_size, ras_size,
              TRUE, NULL);
  opt_reg_int_list(odb, "-bpred:btb",
		   "BTB config (<num_sets> <associativity>)",
		   btb_config, btb_nelt, &btb_nelt,
		   btb_config,
		   TRUE, NULL, FALSE);
  opt_reg_string(odb, "-bpred:spec_update",
		 "speculative predictors update in {ID|WB} (default non-spec)",
		 &bpred_spec_opt, NULL,
		 TRUE, NULL);
  opt_reg_int(odb, "-decode:width",
	      "instruction decode B/W (insts/cycle)",
	      &ruu_decode_width, 4,
	      TRUE, NULL);
  opt_reg_int(odb, "-issue:width",
	      "instruction issue B/W (insts/cycle)",
	      &ruu_issue_width, 4,
	      TRUE, NULL);
  opt_reg_flag(odb, "-issue:inorder", "run pipeline with in-order issue",
	       &ruu_inorder_issue, FALSE,
	       TRUE, NULL);
  opt_reg_flag(odb, "-issue:wrongpath",
	       "issue instructions down wrong execution paths",
	       &ruu_include_spec, TRUE,
	       TRUE, NULL);
  opt_reg_int(odb, "-commit:width",
	      "instruction commit B/W (insts/cycle)",
	      &ruu_commit_width, 4,
	      TRUE, NULL);
  opt_reg_int(odb, "-ruu:size",
	      "register update unit (RUU) size",
	      &RUU_size, 16,
	      TRUE, NULL);
  opt_reg_int(odb, "-lsq:size",
	      "load/store queue (LSQ) size",
	      &LSQ_size, 8,
	      TRUE, NULL);
  opt_reg_string(odb, "-cache:dl1",
		 "l1 data cache config, i.e., {<config>|none}",
		 &cache_dl1_opt, "dl1:128:32:4:l",
		 TRUE, NULL);
  opt_reg_note(odb,
"  The cache config parameter <config> has the following format:\n"
"\n"
"    <name>:<nsets>:<bsize>:<assoc>:<repl>\n"
"\n"
"    <name>   - name of the cache being defined\n"
"    <nsets>  - number of sets in the cache\n"
"    <bsize>  - block size of the cache\n"
"    <assoc>  - associativity of the cache\n"
"    <repl>   - block replacement strategy, 'l'-LRU, 'f'-FIFO, 'r'-random\n"
"\n"
"    Examples:   -cache:dl1 dl1:4096:32:1:l\n"
"                -dtlb dtlb:128:4096:32:r\n"
	       );
  opt_reg_int(odb, "-cache:dl1lat",
	      "l1 data cache hit latency (in cycles)",
	      &cache_dl1_lat, 1,
	      TRUE, NULL);
  opt_reg_string(odb, "-cache:dl2",
		 "l2 data cache config, i.e., {<config>|none}",
		 &cache_dl2_opt, "ul2:1024:64:4:l",
		 TRUE, NULL);
  opt_reg_int(odb, "-cache:dl2lat",
	      "l2 data cache hit latency (in cycles)",
	      &cache_dl2_lat, 6,
	      TRUE, NULL);
  opt_reg_string(odb, "-cache:il1",
		 "l1 inst cache config, i.e., {<config>|dl1|dl2|none}",
		 &cache_il1_opt, "il1:512:32:1:l",
		 TRUE, NULL);
  opt_reg_note(odb,
"  Cache levels can be unified by pointing a level of the instruction cache\n"
"  hierarchy at the data cache hiearchy using the \"dl1\" and \"dl2\" cache\n"
"  configuration arguments.  Most sensible combinations are supported, e.g.,\n"
"\n"
"    A unified l2 cache (il2 is pointed at dl2):\n"
"      -cache:il1 il1:128:64:1:l -cache:il2 dl2\n"
"      -cache:dl1 dl1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n"
"\n"
"    Or, a fully unified cache hierarchy (il1 pointed at dl1):\n"
"      -cache:il1 dl1\n"
"      -cache:dl1 ul1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n"
	       );
  opt_reg_int(odb, "-cache:il1lat",
	      "l1 instruction cache hit latency (in cycles)",
	      &cache_il1_lat, 1,
	      TRUE, NULL);
  opt_reg_string(odb, "-cache:il2",
		 "l2 instruction cache config, i.e., {<config>|dl2|none}",
		 &cache_il2_opt, "dl2",
		 TRUE, NULL);
  opt_reg_int(odb, "-cache:il2lat",
	      "l2 instruction cache hit latency (in cycles)",
	      &cache_il2_lat, 6,
	      TRUE, NULL);
  opt_reg_flag(odb, "-cache:flush", "flush caches on system calls",
	       &flush_on_syscalls, FALSE, TRUE, NULL);
  opt_reg_flag(odb, "-cache:icompress",
	       "convert 64-bit inst addresses to 32-bit inst equivalents",
	       &compress_icache_addrs, FALSE,
	       TRUE, NULL);
  opt_reg_int_list(odb, "-mem:lat",
		   "memory access latency (<first_chunk> <inter_chunk>)",
		   mem_lat, mem_nelt, &mem_nelt, mem_lat,
		   TRUE, NULL, FALSE);
  opt_reg_int(odb, "-mem:width", "memory access bus width (in bytes)",
	      &mem_bus_width, 8,
	      TRUE, NULL);
  opt_reg_string(odb, "-tlb:itlb",
		 "instruction TLB config, i.e., {<config>|none}",
		 &itlb_opt, "itlb:16:4096:4:l", TRUE, NULL);
  opt_reg_string(odb, "-tlb:dtlb",
		 "data TLB config, i.e., {<config>|none}",
		 &dtlb_opt, "dtlb:32:4096:4:l", TRUE, NULL);
  opt_reg_int(odb, "-tlb:lat",
	      "inst/data TLB miss latency (in cycles)",
	      &tlb_miss_lat, 30,
	      TRUE, NULL);
  opt_reg_int(odb, "-res:ialu",
	      "total number of integer ALU's available",
	      &res_ialu, fu_config[FU_IALU_INDEX].quantity,
	      TRUE, NULL);
  opt_reg_int(odb, "-res:imult",
	      "total number of integer multiplier/dividers available",
	      &res_imult, fu_config[FU_IMULT_INDEX].quantity,
	      TRUE, NULL);
  opt_reg_int(odb, "-res:memport",
	      "total number of memory system ports available (to CPU)",
	      &res_memport, fu_config[FU_MEMPORT_INDEX].quantity,
	      TRUE, NULL);
  opt_reg_int(odb, "-res:fpalu",
	      "total number of floating point ALU's available",
	      &res_fpalu, fu_config[FU_FPALU_INDEX].quantity,
	      TRUE, NULL);
  opt_reg_int(odb, "-res:fpmult",
	      "total number of floating point multiplier/dividers available",
	      &res_fpmult, fu_config[FU_FPMULT_INDEX].quantity,
	      TRUE, NULL);
  opt_reg_string_list(odb, "-pcstat",
		      "profile stat(s) against text addr's (mult uses ok)",
		      pcstat_vars, MAX_PCSTAT_VARS, &pcstat_nelt, NULL,
		      FALSE, NULL, TRUE);
  opt_reg_flag(odb, "-bugcompat",
	       "operate in backward-compatible bugs mode (for testing only)",
	       &bugcompat_mode, FALSE, TRUE, NULL);
}

void
simoutorder::sim_check_options(struct opt_odb_t *odb,        
		  int argc, char **argv)        
{
  char name[128], c;
  int nsets, bsize, assoc;
  if (fastfwd_count < 0 || fastfwd_count >= 2147483647)
    fatal("bad fast forward count: %d", fastfwd_count);
  if (ruu_ifq_size < 1 || (ruu_ifq_size & (ruu_ifq_size - 1)) != 0)
    fatal("inst fetch queue size must be positive > 0 and a power of two");
  if (ruu_branch_penalty < 1)
    fatal("mis-prediction penalty must be at least 1 cycle");
  if (fetch_speed < 1)
    fatal("front-end speed must be positive and non-zero");
  if (!mystricmp(pred_type, "perfect"))
    {
      pred = NULL;
      pred_perfect = TRUE;
    }
  else if (!mystricmp(pred_type, "taken"))
    {
      pred = bpred_create(BPredTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
  else if (!mystricmp(pred_type, "nottaken"))
    {
      pred = bpred_create(BPredNotTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
  else if (!mystricmp(pred_type, "bimod"))
    {
      if (bimod_nelt != 1)
	fatal("bad bimod predictor config (<table_size>)");
      if (btb_nelt != 2)
	fatal("bad btb config (<num_sets> <associativity>)");
      pred = bpred_create(BPred2bit,
			  bimod_config[0],
			  0,
			  0,
			  0,
			  0,
			  0,
			  btb_config[0],
			  btb_config[1],
			  ras_size);
    }
  else if (!mystricmp(pred_type, "2lev"))
    {
      if (twolev_nelt != 4)
	fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
      if (btb_nelt != 2)
	fatal("bad btb config (<num_sets> <associativity>)");
      pred = bpred_create(BPred2Level,
			  0,
			  twolev_config[0],
			  twolev_config[1],
			  0,
			  twolev_config[2],
			  twolev_config[3],
			  btb_config[0],
			  btb_config[1],
			  ras_size);
    }
  else if (!mystricmp(pred_type, "comb"))
    {
      if (twolev_nelt != 4)
	fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
      if (bimod_nelt != 1)
	fatal("bad bimod predictor config (<table_size>)");
      if (comb_nelt != 1)
	fatal("bad combining predictor config (<meta_table_size>)");
      if (btb_nelt != 2)
	fatal("bad btb config (<num_sets> <associativity>)");
      pred = bpred_create(BPredComb,
			  bimod_config[0],
			  twolev_config[0],
			  twolev_config[1],
			  comb_config[0],
			  twolev_config[2],
			  twolev_config[3],
			  btb_config[0],
			  btb_config[1],
			  ras_size);
    }
  else
    fatal("cannot parse predictor type `%s'", pred_type);
  if (!bpred_spec_opt)
    bpred_spec_update = spec_CT;
  else if (!mystricmp(bpred_spec_opt, "ID"))
    bpred_spec_update = spec_ID;
  else if (!mystricmp(bpred_spec_opt, "WB"))
    bpred_spec_update = spec_WB;
  else
    fatal("bad speculative update stage specifier, use {ID|WB}");
  if (ruu_decode_width < 1 || (ruu_decode_width & (ruu_decode_width-1)) != 0)
    fatal("issue width must be positive non-zero and a power of two");
  if (ruu_issue_width < 1 || (ruu_issue_width & (ruu_issue_width-1)) != 0)
    fatal("issue width must be positive non-zero and a power of two");
  if (ruu_commit_width < 1)
    fatal("commit width must be positive non-zero");
  if (RUU_size < 2 || (RUU_size & (RUU_size-1)) != 0)
    fatal("RUU size must be a positive number > 1 and a power of two");
  if (LSQ_size < 2 || (LSQ_size & (LSQ_size-1)) != 0)
    fatal("LSQ size must be a positive number > 1 and a power of two");
  if (!mystricmp(cache_dl1_opt, "none"))
    {
      cache_dl1 = NULL;
      if (strcmp(cache_dl2_opt, "none"))
	fatal("the l1 data cache must defined if the l2 cache is defined");
      cache_dl2 = NULL;
    }
  else 
    {
      if (sscanf(cache_dl1_opt, "%[^:]:%d:%d:%d:%c",
		 name, &nsets, &bsize, &assoc, &c) != 5)
	fatal("bad l1 D-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");
      cache_dl1 = cache_create(name, nsets, bsize, FALSE,
			       0, assoc, cache_char2policy(c),
			       dl1_access_fn, cache_dl1_lat);
      if (!mystricmp(cache_dl2_opt, "none"))
	cache_dl2 = NULL;
      else
	{
	  if (sscanf(cache_dl2_opt, "%[^:]:%d:%d:%d:%c",
		     name, &nsets, &bsize, &assoc, &c) != 5)
	    fatal("bad l2 D-cache parms: "
		  "<name>:<nsets>:<bsize>:<assoc>:<repl>");
	  cache_dl2 = cache_create(name, nsets, bsize, FALSE,
				   0, assoc, cache_char2policy(c),
				   dl2_access_fn, cache_dl2_lat);
	}
    }
  if (!mystricmp(cache_il1_opt, "none"))
    {
      cache_il1 = NULL;
      if (strcmp(cache_il2_opt, "none"))
	fatal("the l1 inst cache must defined if the l2 cache is defined");
      cache_il2 = NULL;
    }
  else if (!mystricmp(cache_il1_opt, "dl1"))
    {
      if (!cache_dl1)
	fatal("I-cache l1 cannot access D-cache l1 as it's undefined");
      cache_il1 = cache_dl1;
      if (strcmp(cache_il2_opt, "none"))
	fatal("the l1 inst cache must defined if the l2 cache is defined");
      cache_il2 = NULL;
    }
  else if (!mystricmp(cache_il1_opt, "dl2"))
    {
      if (!cache_dl2)
	fatal("I-cache l1 cannot access D-cache l2 as it's undefined");
      cache_il1 = cache_dl2;
      if (strcmp(cache_il2_opt, "none"))
	fatal("the l1 inst cache must defined if the l2 cache is defined");
      cache_il2 = NULL;
    }
  else 
    {
      if (sscanf(cache_il1_opt, "%[^:]:%d:%d:%d:%c",
		 name, &nsets, &bsize, &assoc, &c) != 5)
	fatal("bad l1 I-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");
      cache_il1 = cache_create(name, nsets, bsize, FALSE,
			       0, assoc, cache_char2policy(c),
			       il1_access_fn, cache_il1_lat);
      if (!mystricmp(cache_il2_opt, "none"))
	cache_il2 = NULL;
      else if (!mystricmp(cache_il2_opt, "dl2"))
	{
	  if (!cache_dl2)
	    fatal("I-cache l2 cannot access D-cache l2 as it's undefined");
	  cache_il2 = cache_dl2;
	}
      else
	{
	  if (sscanf(cache_il2_opt, "%[^:]:%d:%d:%d:%c",
		     name, &nsets, &bsize, &assoc, &c) != 5)
	    fatal("bad l2 I-cache parms: "
		  "<name>:<nsets>:<bsize>:<assoc>:<repl>");
	  cache_il2 = cache_create(name, nsets, bsize, FALSE,
				   0, assoc, cache_char2policy(c),
				   il2_access_fn, cache_il2_lat);
	}
    }
  if (!mystricmp(itlb_opt, "none"))
    itlb = NULL;
  else
    {
      if (sscanf(itlb_opt, "%[^:]:%d:%d:%d:%c",
		 name, &nsets, &bsize, &assoc, &c) != 5)
	fatal("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
      itlb = cache_create(name, nsets, bsize, FALSE,
			  sizeof(md_addr_t), assoc,
			  cache_char2policy(c), itlb_access_fn,
			  1);
    }
  if (!mystricmp(dtlb_opt, "none"))
    dtlb = NULL;
  else
    {
      if (sscanf(dtlb_opt, "%[^:]:%d:%d:%d:%c",
		 name, &nsets, &bsize, &assoc, &c) != 5)
	fatal("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
      dtlb = cache_create(name, nsets, bsize, FALSE,
			  sizeof(md_addr_t), assoc,
			  cache_char2policy(c), dtlb_access_fn,
			  1);
    }
  if (cache_dl1_lat < 1)
    fatal("l1 data cache latency must be greater than zero");
  if (cache_dl2_lat < 1)
    fatal("l2 data cache latency must be greater than zero");
  if (cache_il1_lat < 1)
    fatal("l1 instruction cache latency must be greater than zero");
  if (cache_il2_lat < 1)
    fatal("l2 instruction cache latency must be greater than zero");
  if (mem_nelt != 2)
    fatal("bad memory access latency (<first_chunk> <inter_chunk>)");
  if (mem_lat[0] < 1 || mem_lat[1] < 1)
    fatal("all memory access latencies must be greater than zero");
  if (mem_bus_width < 1 || (mem_bus_width & (mem_bus_width-1)) != 0)
    fatal("memory bus width must be positive non-zero and a power of two");
  if (tlb_miss_lat < 1)
    fatal("TLB miss latency must be greater than zero");
  if (res_ialu < 1)
    fatal("number of integer ALU's must be greater than zero");
  if (res_ialu > MAX_INSTS_PER_CLASS)
    fatal("number of integer ALU's must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_IALU_INDEX].quantity = res_ialu;
  if (res_imult < 1)
    fatal("number of integer multiplier/dividers must be greater than zero");
  if (res_imult > MAX_INSTS_PER_CLASS)
    fatal("number of integer mult/div's must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_IMULT_INDEX].quantity = res_imult;
  if (res_memport < 1)
    fatal("number of memory system ports must be greater than zero");
  if (res_memport > MAX_INSTS_PER_CLASS)
    fatal("number of memory system ports must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_MEMPORT_INDEX].quantity = res_memport;
  if (res_fpalu < 1)
    fatal("number of floating point ALU's must be greater than zero");
  if (res_fpalu > MAX_INSTS_PER_CLASS)
    fatal("number of floating point ALU's must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_FPALU_INDEX].quantity = res_fpalu;
  if (res_fpmult < 1)
    fatal("number of floating point multiplier/dividers must be > zero");
  if (res_fpmult > MAX_INSTS_PER_CLASS)
    fatal("number of FP mult/div's must be <= MAX_INSTS_PER_CLASS");
  fu_config[FU_FPMULT_INDEX].quantity = res_fpmult;
}

simoutorder::sim_reg_stats(struct stat_sdb_t *sdb)   
{
  int i;
  stat_reg_counter(sdb, "sim_num_insn",
		   "total number of instructions committed",
		   &sim_num_insn, sim_num_insn, NULL);
  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores committed",
		   &sim_num_refs, 0, NULL);
  stat_reg_counter(sdb, "sim_num_loads",
		   "total number of loads committed",
		   &sim_num_loads, 0, NULL);
  stat_reg_formula(sdb, "sim_num_stores",
		   "total number of stores committed",
		   "sim_num_refs - sim_num_loads", NULL);
  stat_reg_counter(sdb, "sim_num_branches",
		   "total number of branches committed",
		   &sim_num_branches, 0, NULL);
  stat_reg_int(sdb, "sim_elapsed_time",
	       "total simulation time in seconds",
	       &sim_elapsed_time, 0, NULL);
  stat_reg_formula(sdb, "sim_inst_rate",
		   "simulation speed (in insts/sec)",
		   "sim_num_insn / sim_elapsed_time", NULL);
  stat_reg_counter(sdb, "sim_total_insn",
		   "total number of instructions executed",
		   &sim_total_insn, 0, NULL);
  stat_reg_counter(sdb, "sim_total_refs",
		   "total number of loads and stores executed",
		   &sim_total_refs, 0, NULL);
  stat_reg_counter(sdb, "sim_total_loads",
		   "total number of loads executed",
		   &sim_total_loads, 0, NULL);
  stat_reg_formula(sdb, "sim_total_stores",
		   "total number of stores executed",
		   "sim_total_refs - sim_total_loads", NULL);
  stat_reg_counter(sdb, "sim_total_branches",
		   "total number of branches executed",
		   &sim_total_branches, 0, NULL);
  stat_reg_counter(sdb, "sim_cycle",
		   "total simulation time in cycles",
		   &sim_cycle, 0, NULL);
  stat_reg_formula(sdb, "sim_IPC",
		   "instructions per cycle",
		   "sim_num_insn / sim_cycle", NULL);
  stat_reg_formula(sdb, "sim_CPI",
		   "cycles per instruction",
		   "sim_cycle / sim_num_insn", NULL);
  stat_reg_formula(sdb, "sim_exec_BW",
		   "total instructions (mis-spec + committed) per cycle",
		   "sim_total_insn / sim_cycle", NULL);
  stat_reg_formula(sdb, "sim_IPB",
		   "instruction per branch",
		   "sim_num_insn / sim_num_branches", NULL);
  stat_reg_counter(sdb, "IFQ_count", "cumulative IFQ occupancy",
                   &IFQ_count, 0, NULL);
  stat_reg_counter(sdb, "IFQ_fcount", "cumulative IFQ full count",
                   &IFQ_fcount, 0, NULL);
  stat_reg_formula(sdb, "ifq_occupancy", "avg IFQ occupancy (insn's)",
                   "IFQ_count / sim_cycle", NULL);
  stat_reg_formula(sdb, "ifq_rate", "avg IFQ dispatch rate (insn/cycle)",
                   "sim_total_insn / sim_cycle", NULL);
  stat_reg_formula(sdb, "ifq_latency", "avg IFQ occupant latency (cycle's)",
                   "ifq_occupancy / ifq_rate", NULL);
  stat_reg_formula(sdb, "ifq_full", "fraction of time (cycle's) IFQ was full",
                   "IFQ_fcount / sim_cycle", NULL);
  stat_reg_counter(sdb, "RUU_count", "cumulative RUU occupancy",
                   &RUU_count, 0, NULL);
  stat_reg_counter(sdb, "RUU_fcount", "cumulative RUU full count",
                   &RUU_fcount, 0, NULL);
  stat_reg_formula(sdb, "ruu_occupancy", "avg RUU occupancy (insn's)",
                   "RUU_count / sim_cycle", NULL);
  stat_reg_formula(sdb, "ruu_rate", "avg RUU dispatch rate (insn/cycle)",
                   "sim_total_insn / sim_cycle", NULL);
  stat_reg_formula(sdb, "ruu_latency", "avg RUU occupant latency (cycle's)",
                   "ruu_occupancy / ruu_rate", NULL);
  stat_reg_formula(sdb, "ruu_full", "fraction of time (cycle's) RUU was full",
                   "RUU_fcount / sim_cycle", NULL);
  stat_reg_counter(sdb, "LSQ_count", "cumulative LSQ occupancy",
                   &LSQ_count, 0, NULL);
  stat_reg_counter(sdb, "LSQ_fcount", "cumulative LSQ full count",
                   &LSQ_fcount, 0, NULL);
  stat_reg_formula(sdb, "lsq_occupancy", "avg LSQ occupancy (insn's)",
                   "LSQ_count / sim_cycle", NULL);
  stat_reg_formula(sdb, "lsq_rate", "avg LSQ dispatch rate (insn/cycle)",
                   "sim_total_insn / sim_cycle", NULL);
  stat_reg_formula(sdb, "lsq_latency", "avg LSQ occupant latency (cycle's)",
                   "lsq_occupancy / lsq_rate", NULL);
  stat_reg_formula(sdb, "lsq_full", "fraction of time (cycle's) LSQ was full",
                   "LSQ_fcount / sim_cycle", NULL);
  stat_reg_counter(sdb, "sim_slip",
                   "total number of slip cycles",
                   &sim_slip, 0, NULL);
  stat_reg_formula(sdb, "avg_sim_slip",
                   "the average slip between issue and retirement",
                   "sim_slip / sim_num_insn", NULL);
  if (pred)
    bpred_reg_stats(pred, sdb);
  if (cache_il1
      && (cache_il1 != cache_dl1 && cache_il1 != cache_dl2))
    cache_reg_stats(cache_il1, sdb);
  if (cache_il2
      && (cache_il2 != cache_dl1 && cache_il2 != cache_dl2))
    cache_reg_stats(cache_il2, sdb);
  if (cache_dl1)
    cache_reg_stats(cache_dl1, sdb);
  if (cache_dl2)
    cache_reg_stats(cache_dl2, sdb);
  if (itlb)
    cache_reg_stats(itlb, sdb);
  if (dtlb)
    cache_reg_stats(dtlb, sdb);
  stat_reg_counter(sdb, "sim_invalid_addrs",
		   "total non-speculative bogus addresses seen (debug var)",
                   &sim_invalid_addrs, 0, NULL);
  for (i=0; i<pcstat_nelt; i++)
    {
      char buf[512], buf1[512];
      struct stat_stat_t *stat;
      stat = stat_find_stat(sdb, pcstat_vars[i]);
      if (!stat)
	fatal("cannot locate any statistic named `%s'", pcstat_vars[i]);
      if (stat->sc != sc_int && stat->sc != sc_uint && stat->sc != sc_counter)
	fatal("`-pcstat' statistical variable `%s' is not an integral type",
	      stat->name);
      pcstat_stats[i] = stat;
      pcstat_lastvals[i] = STATVAL(stat);
      sprintf(buf, "%s_by_pc", stat->name);
      sprintf(buf1, "%s (by text address)", stat->desc);
      pcstat_sdists[i] = stat_reg_sdist(sdb, buf, buf1,
					0,
					(PF_COUNT|PF_PDF),
					"0x%lx %lu %.2f",
					NULL);
    }
  ld_reg_stats(sdb);
  mem_reg_stats(mem, sdb);
}

void
simoutorder::sim_aux_config(FILE *stream)            
{
}

void
simoutorder::sim_init(void)
{
  sim_num_refs = 0;
  regs_init(&regs);
  mem = mem_create("mem");
  mem_init(mem);
}

void
simoutorder::sim_load_prog(char *fname,		
	      int argc, char **argv,	
	      char **envp)		
{
  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);
  if (ptrace_nelt == 2)
    {
      ptrace_open(ptrace_opts[0], ptrace_opts[1]);
    }
  else if (ptrace_nelt == 0)
    {
      ;
    }
  else
    fatal("bad pipetrace args, use: <fname|stdout|stderr> <range>");
  fu_pool = res_create_pool("fu-pool", fu_config, N_ELT(fu_config));
  rslink_init(MAX_RS_LINKS);
  tracer_init();
  fetch_init();
  cv_init();
  eventq_init();
  readyq_init();
  ruu_init();
  lsq_init();
  dlite_init(simoo_reg_obj, simoo_mem_obj, simoo_mstate_obj);
}

void
simoutorder::sim_aux_stats(FILE *stream)             
{
}

void
simoutorder::sim_uninit(void)
{
  if (ptrace_nelt > 0)
    ptrace_close();
}

void
simoutorder::ruu_init(void)
{
  RUU = calloc(RUU_size, sizeof(struct RUU_station));
  if (!RUU)
    fatal("out of virtual memory");
  RUU_num = 0;
  RUU_head = RUU_tail = 0;
  RUU_count = 0;
  RUU_fcount = 0;
}

void
simoutorder::ruu_dumpent(struct RUU_station *rs,		
	    int index,				
	    FILE *stream,			
	    int header)				
{
  if (!stream)
    stream = stderr;
  if (header)
    fprintf(stream, "idx: %2d: opcode: %s, inst: `",
	    index, MD_OP_NAME(rs->op));
  else
    fprintf(stream, "       opcode: %s, inst: `",
	    MD_OP_NAME(rs->op));
  md_print_insn(rs->IR, rs->PC, stream);
  fprintf(stream, "'\n");
  myfprintf(stream, "         PC: 0x%08p, NPC: 0x%08p (pred_PC: 0x%08p)\n",
	    rs->PC, rs->next_PC, rs->pred_PC);
  fprintf(stream, "         in_LSQ: %s, ea_comp: %s, recover_inst: %s\n",
	  rs->in_LSQ ? "t" : "f",
	  rs->ea_comp ? "t" : "f",
	  rs->recover_inst ? "t" : "f");
  myfprintf(stream, "         spec_mode: %s, addr: 0x%08p, tag: 0x%08x\n",
	    rs->spec_mode ? "t" : "f", rs->addr, rs->tag);
  fprintf(stream, "         seq: 0x%08x, ptrace_seq: 0x%08x\n",
	  rs->seq, rs->ptrace_seq);
  fprintf(stream, "         queued: %s, issued: %s, completed: %s\n",
	  rs->queued ? "t" : "f",
	  rs->issued ? "t" : "f",
	  rs->completed ? "t" : "f");
  fprintf(stream, "         operands ready: %s\n",
	  OPERANDS_READY(rs) ? "t" : "f");
}

void
simoutorder::ruu_dump(FILE *stream)				
{
  int num, head;
  struct RUU_station *rs;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** RUU state **\n");
  fprintf(stream, "RUU_head: %d, RUU_tail: %d\n", RUU_head, RUU_tail);
  fprintf(stream, "RUU_num: %d\n", RUU_num);
  num = RUU_num;
  head = RUU_head;
  while (num)
    {
      rs = &RUU[head];
      ruu_dumpent(rs, rs - RUU, stream, TRUE);
      head = (head + 1) % RUU_size;
      num--;
    }
}

void
simoutorder::lsq_init(void)
{
  LSQ = calloc(LSQ_size, sizeof(struct RUU_station));
  if (!LSQ)
    fatal("out of virtual memory");
  LSQ_num = 0;
  LSQ_head = LSQ_tail = 0;
  LSQ_count = 0;
  LSQ_fcount = 0;
}

void
simoutorder::lsq_dump(FILE *stream)				
{
  int num, head;
  struct RUU_station *rs;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** LSQ state **\n");
  fprintf(stream, "LSQ_head: %d, LSQ_tail: %d\n", LSQ_head, LSQ_tail);
  fprintf(stream, "LSQ_num: %d\n", LSQ_num);
  num = LSQ_num;
  head = LSQ_head;
  while (num)
    {
      rs = &LSQ[head];
      ruu_dumpent(rs, rs - LSQ, stream, TRUE);
      head = (head + 1) % LSQ_size;
      num--;
    }
}

void
simoutorder::rslink_init(int nlinks)			
{
  int i;
  struct RS_link *link;
  rslink_free_list = NULL;
  for (i=0; i<nlinks; i++)
    {
      link = calloc(1, sizeof(struct RS_link));
      if (!link)
	fatal("out of virtual memory");
      link->next = rslink_free_list;
      rslink_free_list = link;
    }
}

void
simoutorder::ruu_release_fu(void)
{
  int i;
  for (i=0; i<fu_pool->num_resources; i++)
    {
      if (fu_pool->resources[i].busy > 0)
	fu_pool->resources[i].busy--;
    }
}

void
simoutorder::eventq_init(void)
{
  event_queue = NULL;
}

void
simoutorder::eventq_dump(FILE *stream)			
{
  struct RS_link *ev;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** event queue state **\n");
  for (ev = event_queue; ev != NULL; ev = ev->next)
    {
      if (RSLINK_VALID(ev))
	{
	  struct RUU_station *rs = RSLINK_RS(ev);
	  fprintf(stream, "idx: %2d: @ %.0f\n",
		  (int)(rs - (rs->in_LSQ ? LSQ : RUU)), (double)ev->x.when);
	  ruu_dumpent(rs, rs - (rs->in_LSQ ? LSQ : RUU),
		      stream, FALSE);
	}
    }
}

void
simoutorder::eventq_queue_event(struct RUU_station *rs, tick_t when)
{
  struct RS_link *prev, *ev, *new_ev;
  if (rs->completed)
    panic("event completed");
  if (when <= sim_cycle)
    panic("event occurred in the past");
  RSLINK_NEW(new_ev, rs);
  new_ev->x.when = when;
  for (prev=NULL, ev=event_queue;
       ev && ev->x.when < when;
       prev=ev, ev=ev->next);
  if (prev)
    {
      new_ev->next = prev->next;
      prev->next = new_ev;
    }
  else
    {
      new_ev->next = event_queue;
      event_queue = new_ev;
    }
}

struct RUU_station *
simoutorder::eventq_next_event(void)
{
  struct RS_link *ev;
  if (event_queue && event_queue->x.when <= sim_cycle)
    {
      ev = event_queue;
      event_queue = event_queue->next;
      if (RSLINK_VALID(ev))
	{
	  struct RUU_station *rs = RSLINK_RS(ev);
	  RSLINK_FREE(ev);
	  return rs;
	}
      else
	{
	  RSLINK_FREE(ev);
	  return eventq_next_event();
	}
    }
  else
    {
      return NULL;
    }
}

void
simoutorder::readyq_init(void)
{
  ready_queue = NULL;
}

void
simoutorder::readyq_dump(FILE *stream)			
{
  struct RS_link *link;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** ready queue state **\n");
  for (link = ready_queue; link != NULL; link = link->next)
    {
      if (RSLINK_VALID(link))
	{
	  struct RUU_station *rs = RSLINK_RS(link);
	  ruu_dumpent(rs, rs - (rs->in_LSQ ? LSQ : RUU),
		      stream, TRUE);
	}
    }
}

void
simoutorder::readyq_enqueue(struct RUU_station *rs)		
{
  struct RS_link *prev, *node, *new_node;
  if (rs->queued)
    panic("node is already queued");
  rs->queued = TRUE;
  RSLINK_NEW(new_node, rs);
  new_node->x.seq = rs->seq;
  if (rs->in_LSQ || MD_OP_FLAGS(rs->op) & (F_LONGLAT|F_CTRL))
    {
      prev = NULL;
      node = ready_queue;
    }
  else
    {
      for (prev=NULL, node=ready_queue;
	   node && node->x.seq < rs->seq;
	   prev=node, node=node->next);
    }
  if (prev)
    {
      new_node->next = prev->next;
      prev->next = new_node;
    }
  else
    {
      new_node->next = ready_queue;
      ready_queue = new_node;
    }
}

void
simoutorder::cv_init(void)
{
  int i;
  for (i=0; i < MD_TOTAL_REGS; i++)
    {
      create_vector[i] = CVLINK_NULL;
      create_vector_rt[i] = 0;
      spec_create_vector[i] = CVLINK_NULL;
      spec_create_vector_rt[i] = 0;
    }
  BITMAP_CLEAR_MAP(use_spec_cv, CV_BMAP_SZ);
}

void
simoutorder::cv_dump(FILE *stream)				
{
  int i;
  struct CV_link ent;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** create vector state **\n");
  for (i=0; i < MD_TOTAL_REGS; i++)
    {
      ent = CREATE_VECTOR(i);
      if (!ent.rs)
	fprintf(stream, "[cv%02d]: from architected reg file\n", i);
      else
	fprintf(stream, "[cv%02d]: from %s, idx: %d\n",
		i, (ent.rs->in_LSQ ? "LSQ" : "RUU"),
		(int)(ent.rs - (ent.rs->in_LSQ ? LSQ : RUU)));
    }
}

void
simoutorder::ruu_commit(void)
{
  int i, lat, events, committed = 0;
  static counter_t sim_ret_insn = 0;
  while (RUU_num > 0 && committed < ruu_commit_width)
    {
      struct RUU_station *rs = &(RUU[RUU_head]);
      if (!rs->completed)
	{
	  break;
	}
      events = 0;
      if (RUU[RUU_head].ea_comp)
	{
	  if (LSQ_num <= 0 || !LSQ[LSQ_head].in_LSQ)
	    panic("RUU out of sync with LSQ");
	  if (!LSQ[LSQ_head].completed)
	    {
	      break;
	    }
	  if ((MD_OP_FLAGS(LSQ[LSQ_head].op) & (F_MEM|F_STORE))
	      == (F_MEM|F_STORE))
	    {
	      struct res_template *fu;
	      fu = res_get(fu_pool, MD_OP_FUCLASS(LSQ[LSQ_head].op));
	      if (fu)
		{
		  if (fu->master->busy)
		    panic("functional unit already in use");
		  fu->master->busy = fu->issuelat;
		  if (cache_dl1)
		    {
		      lat =
			cache_access(cache_dl1, Write, (LSQ[LSQ_head].addr&~3),
				     NULL, 4, sim_cycle, NULL, NULL);
		      if (lat > cache_dl1_lat)
			events |= PEV_CACHEMISS;
		    }
		  if (dtlb)
		    {
		      lat =
			cache_access(dtlb, Read, (LSQ[LSQ_head].addr & ~3),
				     NULL, 4, sim_cycle, NULL, NULL);
		      if (lat > 1)
			events |= PEV_TLBMISS;
		    }
		}
	      else
		{
		  break;
		}
	    }
	  LSQ[LSQ_head].tag++;
          sim_slip += (sim_cycle - LSQ[LSQ_head].slip);
	  ptrace_newstage(LSQ[LSQ_head].ptrace_seq, PST_COMMIT, events);
	  ptrace_endinst(LSQ[LSQ_head].ptrace_seq);
	  LSQ_head = (LSQ_head + 1) % LSQ_size;
	  LSQ_num--;
	}
      if (pred
	  && bpred_spec_update == spec_CT
	  && (MD_OP_FLAGS(rs->op) & F_CTRL))
	{
	  bpred_update(pred,
		       rs->PC,
		       rs->next_PC,
                       rs->next_PC != (rs->PC +
                                                   sizeof(md_inst_t)),
                       rs->pred_PC != (rs->PC +
                                                        sizeof(md_inst_t)),
                       rs->pred_PC == rs->next_PC,
                       rs->op,
                       &rs->dir_update);
	}
      RUU[RUU_head].tag++;
      sim_slip += (sim_cycle - RUU[RUU_head].slip);
      if (verbose)
	{
	  sim_ret_insn++;
	  myfprintf(stderr, "%10n @ 0x%08p: ", sim_ret_insn, RUU[RUU_head].PC);
 	  md_print_insn(RUU[RUU_head].IR, RUU[RUU_head].PC, stderr);
	  if (MD_OP_FLAGS(RUU[RUU_head].op) & F_MEM)
	    myfprintf(stderr, "  mem: 0x%08p", RUU[RUU_head].addr);
	  fprintf(stderr, "\n");
	}
      ptrace_newstage(RUU[RUU_head].ptrace_seq, PST_COMMIT, events);
      ptrace_endinst(RUU[RUU_head].ptrace_seq);
      RUU_head = (RUU_head + 1) % RUU_size;
      RUU_num--;
      committed++;
      for (i=0; i<MAX_ODEPS; i++)
	{
	  if (rs->odep_list[i])
	    panic ("retired instruction has odeps\n");
        }
    }
}

void
simoutorder::ruu_recover(int branch_index)			
{
  int i, RUU_index = RUU_tail, LSQ_index = LSQ_tail;
  int RUU_prev_tail = RUU_tail, LSQ_prev_tail = LSQ_tail;
  RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
  LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;
  while (RUU_index != branch_index)
    {
      if (!RUU_num)
	panic("empty RUU");
      if (RUU_index == RUU_head)
	panic("RUU head and tail broken");
      if (RUU[RUU_index].ea_comp)
	{
	  if (!LSQ_num)
	    panic("RUU and LSQ out of sync");
	  for (i=0; i<MAX_ODEPS; i++)
	    {
	      RSLINK_FREE_LIST(LSQ[LSQ_index].odep_list[i]);
	      LSQ[LSQ_index].odep_list[i] = NULL;
	    }
	  LSQ[LSQ_index].tag++;
	  ptrace_endinst(LSQ[LSQ_index].ptrace_seq);
	  LSQ_prev_tail = LSQ_index;
	  LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;
	  LSQ_num--;
	}
      for (i=0; i<MAX_ODEPS; i++)
	{
	  RSLINK_FREE_LIST(RUU[RUU_index].odep_list[i]);
	  RUU[RUU_index].odep_list[i] = NULL;
	}
      RUU[RUU_index].tag++;
      ptrace_endinst(RUU[RUU_index].ptrace_seq);
      RUU_prev_tail = RUU_index;
      RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
      RUU_num--;
    }
  RUU_tail = RUU_prev_tail;
  LSQ_tail = LSQ_prev_tail;
  BITMAP_CLEAR_MAP(use_spec_cv, CV_BMAP_SZ);
}


void
simoutorder::ruu_writeback(void)
{
  int i;
  struct RUU_station *rs;
  while ((rs = eventq_next_event()))
    {
      if (!OPERANDS_READY(rs) || rs->queued || !rs->issued || rs->completed)
	panic("inst completed and !ready, !issued, or completed");
      rs->completed = TRUE;
      if (rs->recover_inst)
	{
	  if (rs->in_LSQ)
	    panic("mis-predicted load or store?!?!?");
	  ruu_recover(rs - RUU);
	  tracer_recover();
	  bpred_recover(pred, rs->PC, rs->stack_recover_idx);
	  ruu_fetch_issue_delay = ruu_branch_penalty;
	}
      if (pred
	  && bpred_spec_update == spec_WB
	  && !rs->in_LSQ
	  && (MD_OP_FLAGS(rs->op) & F_CTRL))
	{
	  bpred_update(pred,
		       rs->PC,
		       rs->next_PC,
		       rs->next_PC != (rs->PC +
						   sizeof(md_inst_t)),
		       rs->pred_PC != (rs->PC +
							sizeof(md_inst_t)),
		       rs->pred_PC == rs->next_PC,
		       rs->op,
		       &rs->dir_update);
	}
      ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK,
		      rs->recover_inst ? PEV_MPDETECT : 0);
      for (i=0; i<MAX_ODEPS; i++)
	{
	  if (rs->onames[i] != NA)
	    {
	      struct CV_link link;
	      struct RS_link *olink, *olink_next;
	      if (rs->spec_mode)
		{
		  link = spec_create_vector[rs->onames[i]];
		  if (link.rs
		      && (link.rs == rs && link.odep_num == i))
		    {
		      spec_create_vector[rs->onames[i]] = CVLINK_NULL;
		      spec_create_vector_rt[rs->onames[i]] = sim_cycle;
		    }
		}
	      else
		{
		  link = create_vector[rs->onames[i]];
		  if (link.rs
		      && (link.rs == rs && link.odep_num == i))
		    {
		      create_vector[rs->onames[i]] = CVLINK_NULL;
		      create_vector_rt[rs->onames[i]] = sim_cycle;
		    }
		}
	      for (olink=rs->odep_list[i]; olink; olink=olink_next)
		{
		  if (RSLINK_VALID(olink))
		    {
		      if (olink->rs->idep_ready[olink->x.opnum])
			panic("output dependence already satisfied");
		      olink->rs->idep_ready[olink->x.opnum] = TRUE;
		      if (OPERANDS_READY(olink->rs))
			{
			  if (!olink->rs->in_LSQ
			      || ((MD_OP_FLAGS(olink->rs->op)&(F_MEM|F_STORE))
				  == (F_MEM|F_STORE)))
			    readyq_enqueue(olink->rs);
			}
		    }
		  olink_next = olink->next;
		  RSLINK_FREE(olink);
		}
	      rs->odep_list[i] = NULL;
	    } 
	} 
   } 
}

void
simoutorder::lsq_refresh(void)
{
  int i, j, index, n_std_unknowns;
  md_addr_t std_unknowns[MAX_STD_UNKNOWNS];
  for (i=0, index=LSQ_head, n_std_unknowns=0;
       i < LSQ_num;
       i++, index=(index + 1) % LSQ_size)
    {
      if (
	  (MD_OP_FLAGS(LSQ[index].op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
	{
	  if (!STORE_ADDR_READY(&LSQ[index]))
	    {
	      break;
	    }
	  else if (!OPERANDS_READY(&LSQ[index]))
	    {
	      if (n_std_unknowns == MAX_STD_UNKNOWNS)
		fatal("STD unknown array overflow, increase MAX_STD_UNKNOWNS");
	      std_unknowns[n_std_unknowns++] = LSQ[index].addr;
	    }
	  else 
	    {
	      for (j=0; j<n_std_unknowns; j++)
		{
		  if (std_unknowns[j] == LSQ[index].addr)
		    std_unknowns[j] = 0;
		}
	    }
	}
      if (
	  ((MD_OP_FLAGS(LSQ[index].op) & (F_MEM|F_LOAD)) == (F_MEM|F_LOAD))
	  && !LSQ[index].queued
	  && !LSQ[index].issued
	  && !LSQ[index].completed
	  && OPERANDS_READY(&LSQ[index]))
	{
	  for (j=0; j<n_std_unknowns; j++)
	    {
	      if (std_unknowns[j] == LSQ[index].addr)
		break;
	    }
	  if (j == n_std_unknowns)
	    {
	      readyq_enqueue(&LSQ[index]);
	    }
	}
    }
}

void
simoutorder::ruu_issue(void)
{
  int i, load_lat, tlb_lat, n_issued;
  struct RS_link *node, *next_node;
  struct res_template *fu;
  node = ready_queue;
  ready_queue = NULL;
  for (n_issued=0;
       node && n_issued < ruu_issue_width;
       node = next_node)
    {
      next_node = node->next;
      if (RSLINK_VALID(node))
	{
	  struct RUU_station *rs = RSLINK_RS(node);
	  if (!OPERANDS_READY(rs) || !rs->queued
	      || rs->issued || rs->completed)
	    panic("issued inst !ready, issued, or completed");
	  rs->queued = FALSE;
	  if (rs->in_LSQ
	      && ((MD_OP_FLAGS(rs->op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE)))
	    {
	      rs->issued = TRUE;
	      rs->completed = TRUE;
	      if (rs->onames[0] || rs->onames[1])
		panic("store creates result");
	      if (rs->recover_inst)
		panic("mis-predicted store");
	      ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK, 0);
	      n_issued++;
	    }
	  else
	    {
	      if (MD_OP_FUCLASS(rs->op) != NA)
		{
		  fu = res_get(fu_pool, MD_OP_FUCLASS(rs->op));
		  if (fu)
		    {
		      rs->issued = TRUE;
		      if (fu->master->busy)
			panic("functional unit already in use");
		      fu->master->busy = fu->issuelat;
		      if (rs->in_LSQ
			  && ((MD_OP_FLAGS(rs->op) & (F_MEM|F_LOAD))
			      == (F_MEM|F_LOAD)))
			{
			  int events = 0;
			  load_lat = 0;
			  i = (rs - LSQ);
			  if (i != LSQ_head)
			    {
			      for (;;)
				{
				  i = (i + (LSQ_size-1)) % LSQ_size;
				  if ((MD_OP_FLAGS(LSQ[i].op) & F_STORE)
				      && (LSQ[i].addr == rs->addr))
				    {
				      load_lat = 1;
				      break;
				    }
				  if (i == LSQ_head)
				    break;
				}
			    }
			  if (!load_lat)
			    {
			      int valid_addr = MD_VALID_ADDR(rs->addr);
			      if (!spec_mode && !valid_addr)
				sim_invalid_addrs++;
			      if (cache_dl1 && valid_addr)
				{
				  load_lat =
				    cache_access(cache_dl1, Read,
						 (rs->addr & ~3), NULL, 4,
						 sim_cycle, NULL, NULL);
				  if (load_lat > cache_dl1_lat)
				    events |= PEV_CACHEMISS;
				}
			      else
				{
				  load_lat = fu->oplat;
				}
			    }
			  if (dtlb && MD_VALID_ADDR(rs->addr))
			    {
			      tlb_lat =
				cache_access(dtlb, Read, (rs->addr & ~3),
					     NULL, 4, sim_cycle, NULL, NULL);
			      if (tlb_lat > 1)
				events |= PEV_TLBMISS;
			      load_lat = MAX(tlb_lat, load_lat);
			    }
			  eventq_queue_event(rs, sim_cycle + load_lat);
			  ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
					  ((rs->ea_comp ? PEV_AGEN : 0)
					   | events));
			}
		      else 
			{
			  eventq_queue_event(rs, sim_cycle + fu->oplat);
			  ptrace_newstage(rs->ptrace_seq, PST_EXECUTE, 
					  rs->ea_comp ? PEV_AGEN : 0);
			}
		      n_issued++;
		    }
		  else 
		    {
		      readyq_enqueue(rs);
		    }
		}
	      else 
		{
		  rs->issued = TRUE;
		  eventq_queue_event(rs, sim_cycle + 1);
		  ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
				  rs->ea_comp ? PEV_AGEN : 0);
		  n_issued++;
		}
	    } 
	}
      RSLINK_FREE(node);
    }
  for (; node; node = next_node)
    {
      next_node = node->next;
      if (RSLINK_VALID(node))
        {
	  struct RUU_station *rs = RSLINK_RS(node);
          rs->queued = FALSE;
          readyq_enqueue(rs);
        }
      RSLINK_FREE(node);
    }
}

void
simoutorder::rspec_dump(FILE *stream)			
{
  int i;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** speculative register contents **\n");
  fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");
  for (i=0; i < MD_NUM_IREGS; i++)
    {
      if (BITMAP_SET_P(use_spec_R, R_BMAP_SZ, i))
	{
	  md_print_ireg(spec_regs_R, i, stream);
	  fprintf(stream, "\n");
	}
    }
  for (i=0; i < MD_NUM_FREGS; i++)
    {
      if (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, i))
	{
	  md_print_fpreg(spec_regs_F, i, stream);
	  fprintf(stream, "\n");
	}
    }
  for (i=0; i < MD_NUM_CREGS; i++)
    {
      if (BITMAP_SET_P(use_spec_C, C_BMAP_SZ, i))
	{
	  md_print_creg(spec_regs_C, i, stream);
	  fprintf(stream, "\n");
	}
    }
}

enum md_fault_type
simoutorder::spec_mem_access(struct mem_t *mem,		
		enum mem_cmd cmd,		
		md_addr_t addr,			
		void *p,			
		int nbytes)			
{
  int i, index;
  struct spec_mem_ent *ent, *prev;
  if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
    {
      for (i=0; i < nbytes; i++)
	((char *)p)[i] = 0;
      return md_fault_none;
    }
  if (!((addr >= ld_text_base && addr < (ld_text_base+ld_text_size)
	 && cmd == Read)
	|| MD_VALID_ADDR(addr)))
    {
      for (i=0; i < nbytes; i++)
	((char *)p)[i] = 0;
      return md_fault_none;
    }
  index = HASH_ADDR(addr);
  for (prev=NULL,ent=store_htable[index]; ent; prev=ent,ent=ent->next)
    {
      if (ent->addr == addr)
	{
	  if (prev != NULL)
	    {
	      prev->next = ent->next;
              ent->next = store_htable[index];
              store_htable[index] = ent;
	    }
	  break;
	}
    }
  if (!ent && cmd == Write)
    {
      if (!bucket_free_list)
	{
	  bucket_free_list = calloc(1, sizeof(struct spec_mem_ent));
	  if (!bucket_free_list)
	    fatal("out of virtual memory");
	}
      ent = bucket_free_list;
      bucket_free_list = bucket_free_list->next;
      if (!bugcompat_mode)
	{
	  ent->next = store_htable[index];
	  store_htable[index] = ent;
	  ent->addr = addr;
	  ent->data[0] = 0; ent->data[1] = 0;
	}
    }
  switch (nbytes)
    {
    case 1:
      if (cmd == Read)
	{
	  if (ent)
	    {
	      *((byte_t *)p) = *((byte_t *)(&ent->data[0]));
	    }
	  else
	    {
	      *((byte_t *)p) = MEM_READ_BYTE(mem, addr);
	    }
	}
      else
	{
	  *((byte_t *)(&ent->data[0])) = *((byte_t *)p);
	}
      break;
    case 2:
      if (cmd == Read)
	{
	  if (ent)
	    {
	      *((half_t *)p) = *((half_t *)(&ent->data[0]));
	    }
	  else
	    {
	      *((half_t *)p) = MEM_READ_HALF(mem, addr);
	    }
	}
      else
	{
	  *((half_t *)&ent->data[0]) = *((half_t *)p);
	}
      break;
    case 4:
      if (cmd == Read)
	{
	  if (ent)
	    {
	      *((word_t *)p) = *((word_t *)&ent->data[0]);
	    }
	  else
	    {
	      *((word_t *)p) = MEM_READ_WORD(mem, addr);
	    }
	}
      else
	{
	  *((word_t *)&ent->data[0]) = *((word_t *)p);
	}
      break;
    case 8:
      if (cmd == Read)
	{
	  if (ent)
	    {
	      *((word_t *)p) = *((word_t *)&ent->data[0]);
	      *(((word_t *)p)+1) = *((word_t *)&ent->data[1]);
	    }
	  else
	    {
	      *((word_t *)p) = MEM_READ_WORD(mem, addr);
	      *(((word_t *)p)+1) =
		MEM_READ_WORD(mem, addr + sizeof(word_t));
	    }
	}
      else
	{
	  *((word_t *)&ent->data[0]) = *((word_t *)p);
	  *((word_t *)&ent->data[1]) = *(((word_t *)p)+1);
	}
      break;
    default:
      panic("access size not supported in mis-speculative mode");
    }
  return md_fault_none;
}

void
simoutorder::tracer_recover(void)
{
  int i;
  struct spec_mem_ent *ent, *ent_next;
  if (!spec_mode)
    panic("cannot recover unless in speculative mode");
  spec_mode = FALSE;
  BITMAP_CLEAR_MAP(use_spec_R, R_BMAP_SZ);
  BITMAP_CLEAR_MAP(use_spec_F, F_BMAP_SZ);
  BITMAP_CLEAR_MAP(use_spec_C, C_BMAP_SZ);
  for (i=0; i<STORE_HASH_SIZE; i++)
    {
      for (ent=store_htable[i]; ent; ent=ent_next)
	{
	  ent_next = ent->next;
	  ent->next = bucket_free_list;
	  bucket_free_list = ent;
	}
      store_htable[i] = NULL;
    }
  if (ptrace_active)
    {
      while (fetch_num != 0)
	{
	  ptrace_endinst(fetch_data[fetch_head].ptrace_seq);
	  fetch_head = (fetch_head+1) & (ruu_ifq_size - 1);
	  fetch_num--;
	}
    }
  fetch_num = 0;
  fetch_tail = fetch_head = 0;
  fetch_pred_PC = fetch_regs_PC = recover_PC;
}

void
simoutorder::tracer_init(void)
{
  int i;
  spec_mode = FALSE;
  BITMAP_CLEAR_MAP(use_spec_R, R_BMAP_SZ);
  BITMAP_CLEAR_MAP(use_spec_F, F_BMAP_SZ);
  BITMAP_CLEAR_MAP(use_spec_C, C_BMAP_SZ);
  for (i=0; i<STORE_HASH_SIZE; i++)
    store_htable[i] = NULL;
}

void
simoutorder::mspec_dump(FILE *stream)			
{
  int i;
  struct spec_mem_ent *ent;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** speculative memory contents **\n");
  fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");
  for (i=0; i<STORE_HASH_SIZE; i++)
    {
      for (ent=store_htable[i]; ent; ent=ent->next)
	{
	  myfprintf(stream, "[0x%08p]: %12.0f/0x%08x:%08x\n",
		    ent->addr, (double)(*((double *)ent->data)),
		    *((unsigned int *)&ent->data[0]),
		    *(((unsigned int *)&ent->data[0]) + 1));
	}
    }
}

char *					
simoutorder::simoo_mem_obj(struct mem_t *mem,		
	      int is_write,			
	      md_addr_t addr,			
	      char *p,				
	      int nbytes)			
{
  enum mem_cmd cmd;
  if (!is_write)
    cmd = Read;
  else
    cmd = Write;
#if 0
  char *errstr;
  errstr = mem_valid(cmd, addr, nbytes, FALSE);
  if (errstr)
    return errstr;
#endif
  if (spec_mode)
    spec_mem_access(mem, cmd, addr, p, nbytes);
  else
    mem_access(mem, cmd, addr, p, nbytes);
  return NULL;
}

INLINE void
simoutorder::ruu_link_idep(struct RUU_station *rs,		
	      int idep_num,			
	      int idep_name)			
{
  struct CV_link head;
  struct RS_link *link;
  if (idep_name == NA)
    {
      rs->idep_ready[idep_num] = TRUE;
      return;
    }
  head = CREATE_VECTOR(idep_name);
  if (!head.rs)
    {
      rs->idep_ready[idep_num] = TRUE;
      return;
    }
  rs->idep_ready[idep_num] = FALSE;
  RSLINK_NEW(link, rs); link->x.opnum = idep_num;
  link->next = head.rs->odep_list[head.odep_num];
  head.rs->odep_list[head.odep_num] = link;
}

INLINE void
simoutorder::ruu_install_odep(struct RUU_station *rs,	
		 int odep_num,			
		 int odep_name)			
{
  struct CV_link cv;
  if (odep_name == NA)
    {
      rs->onames[odep_num] = NA;
      return;
    }
  rs->onames[odep_num] = odep_name;
  rs->odep_list[odep_num] = NULL;
  CVLINK_INIT(cv, rs, odep_num);
  SET_CREATE_VECTOR(odep_name, cv);
}

char *					
simoutorder::simoo_reg_obj(struct regs_t *xregs,		
	      int is_write,			
	      enum md_reg_type rt,		
	      int reg,				
	      struct eval_value_t *val)		
{
  switch (rt)
    {
    case rt_gpr:
      if (reg < 0 || reg >= MD_NUM_IREGS)
	return "register number out of range";
      if (!is_write)
	{
	  val->type = et_uint;
	  val->value.as_uint = GPR(reg);
	}
      else
	SET_GPR(reg, eval_as_uint(*val));
      break;
    case rt_lpr:
      if (reg < 0 || reg >= MD_NUM_FREGS)
	return "register number out of range";
      abort();
#if 0
      if (!is_write)
	{
	  val->type = et_uint;
	  val->value.as_uint = FPR_L(reg);
	}
      else
	SET_FPR_L(reg, eval_as_uint(*val));
#endif
      break;
    case rt_fpr:
      abort();
#if 0
      if (!is_write)
	val->value.as_float = FPR_F(reg);
      else
	SET_FPR_F(reg, val->value.as_float);
#endif
      break;
    case rt_dpr:
      abort();
#if 0
      if (reg < 0 || reg >= MD_NUM_REGS/2)
	return "register number out of range";
      if (at == at_read)
	val->as_double = FPR_D(reg * 2);
      else
	SET_FPR_D(reg * 2, val->as_double);
#endif
      break;
#if 0
      abort();
    case rt_hi:
      if (at == at_read)
	val->as_word = HI;
      else
	SET_HI(val->as_word);
      break;
    case rt_lo:
      if (at == at_read)
	val->as_word = LO;
      else
	SET_LO(val->as_word);
      break;
    case rt_FCC:
      if (at == at_read)
	val->as_condition = FCC;
      else
	SET_FCC(val->as_condition);
      break;
#endif
    case rt_PC:
      if (!is_write)
	{
	  val->type = et_addr;
	  val->value.as_addr = regs.regs_PC;
	}
      else
	regs.regs_PC = eval_as_addr(*val);
      break;
    case rt_NPC:
      if (!is_write)
	{
	  val->type = et_addr;
	  val->value.as_addr = regs.regs_NPC;
	}
      else
	regs.regs_NPC = eval_as_addr(*val);
      break;
    default:
      panic("bogus register bank");
    }
  return NULL;
}

void
simoutorder::ruu_dispatch(void)
{
  int i;
  int n_dispatched;			
  md_inst_t inst;			
  enum md_opcode op;			
  int out1, out2, in1, in2, in3;	
  md_addr_t target_PC;			
  md_addr_t addr;			
  struct RUU_station *rs;		
  struct RUU_station *lsq;		
  struct bpred_update_t *dir_update_ptr;
  int stack_recover_idx;		
  unsigned int pseq;			
  int is_write;				
  int made_check;			
  int br_taken, br_pred_taken;		
  int fetch_redirected = FALSE;
  byte_t temp_byte = 0;			
  half_t temp_half = 0;			
  word_t temp_word = 0;			
#ifdef HOST_HAS_QWORD
  qword_t temp_qword = 0;		
#endif 
  enum md_fault_type fault;
  made_check = FALSE;
  n_dispatched = 0;
  while (
	 n_dispatched < (ruu_decode_width * fetch_speed)
	 && RUU_num < RUU_size && LSQ_num < LSQ_size
	 && fetch_num != 0
	 && (ruu_include_spec || !spec_mode))
    {
      if (ruu_inorder_issue
	  && (last_op.rs && RSLINK_VALID(&last_op)
	      && !OPERANDS_READY(last_op.rs)))
	{
	  break;
	}
      inst = fetch_data[fetch_head].IR;
      regs.regs_PC = fetch_data[fetch_head].regs_PC;
      pred_PC = fetch_data[fetch_head].pred_PC;
      dir_update_ptr = &(fetch_data[fetch_head].dir_update);
      stack_recover_idx = fetch_data[fetch_head].stack_recover_idx;
      pseq = fetch_data[fetch_head].ptrace_seq;
      MD_SET_OPCODE(op, inst);
      regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);
      if (MD_OP_FLAGS(op) & F_TRAP)
	{
	  if (RUU_num != 0)
	    break;
	  if (spec_mode)
	    panic("drained and speculative");
	}
      regs.regs_R[MD_REG_ZERO] = 0; spec_regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
      regs.regs_F.d[MD_REG_ZERO] = 0.0; spec_regs_F.d[MD_REG_ZERO] = 0.0;
#endif 
      if (!spec_mode)
	{
	  sim_num_insn++;
	}
      addr = 0; is_write = FALSE;
      fault = md_fault_none;
      switch (op)
	{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,CLASS,O1,O2,I1,I2,I3)		\
	case OP:							\
	  	\
	  out1 = O1; out2 = O2;						\
	  in1 = I1; in2 = I2; in3 = I3;					\
	  					\
	  SYMCAT(OP,_IMPL);						\
	  break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
	case OP:							\
	  	\
	  op = MD_NOP_OP;						\
	  	\
	  out1 = NA; out2 = NA;						\
	  in1 = NA; in2 = NA; in3 = NA;					\
	  							\
	  break;
#define CONNECT(OP)	
#define DECLARE_FAULT(FAULT)						\
	  {								\
	    if (!spec_mode)						\
	      fault = (FAULT);						\
	    	\
	    break;							\
	  }
#include "machine.def"
	default:
	  op = MD_NOP_OP;
	  	\
	  out1 = NA; out2 = NA;
	  in1 = NA; in2 = NA; in3 = NA;
	}
      if (!spec_mode && verbose)
        {
          myfprintf(stderr, "++ %10n [xor: 0x%08x] {%d} @ 0x%08p: ",
                    sim_num_insn, md_xor_regs(&regs),
                    inst_seq+1, regs.regs_PC);
          md_print_insn(inst, regs.regs_PC, stderr);
          fprintf(stderr, "\n");
        }
      if (fault != md_fault_none)
	fatal("non-speculative fault (%d) detected @ 0x%08p",
	      fault, regs.regs_PC);
      if (MD_OP_FLAGS(op) & F_MEM)
	{
	  sim_total_refs++;
	  if (!spec_mode)
	    sim_num_refs++;
	  if (MD_OP_FLAGS(op) & F_STORE)
	    is_write = TRUE;
	  else
	    {
	      sim_total_loads++;
	      if (!spec_mode)
		sim_num_loads++;
	    }
	}
      br_taken = (regs.regs_NPC != (regs.regs_PC + sizeof(md_inst_t)));
      br_pred_taken = (pred_PC != (regs.regs_PC + sizeof(md_inst_t)));
      if ((pred_PC != regs.regs_NPC && pred_perfect)
	  || ((MD_OP_FLAGS(op) & (F_CTRL|F_DIRJMP)) == (F_CTRL|F_DIRJMP)
	      && target_PC != pred_PC && br_pred_taken))
	{
	  fetch_pred_PC = fetch_regs_PC = regs.regs_NPC;
	  if (pred_perfect)
	    pred_PC = regs.regs_NPC;
	  fetch_head = (ruu_ifq_size-1);
	  fetch_num = 1;
	  fetch_tail = 0;
	  if (!pred_perfect)
	    ruu_fetch_issue_delay = ruu_branch_penalty;
	  fetch_redirected = TRUE;
	}
      if (op != MD_NOP_OP)
	{
	  rs = &RUU[RUU_tail];
          rs->slip = sim_cycle - 1;
	  rs->IR = inst;
	  rs->op = op;
	  rs->PC = regs.regs_PC;
	  rs->next_PC = regs.regs_NPC; rs->pred_PC = pred_PC;
	  rs->in_LSQ = FALSE;
	  rs->ea_comp = FALSE;
	  rs->recover_inst = FALSE;
          rs->dir_update = *dir_update_ptr;
	  rs->stack_recover_idx = stack_recover_idx;
	  rs->spec_mode = spec_mode;
	  rs->addr = 0;
	  rs->seq = ++inst_seq;
	  rs->queued = rs->issued = rs->completed = FALSE;
	  rs->ptrace_seq = pseq;
	  if (MD_OP_FLAGS(op) & F_MEM)
	    {
	      rs->op = MD_AGEN_OP;
	      rs->ea_comp = TRUE;
	      lsq = &LSQ[LSQ_tail];
              lsq->slip = sim_cycle - 1;
	      lsq->IR = inst;
	      lsq->op = op;
	      lsq->PC = regs.regs_PC;
	      lsq->next_PC = regs.regs_NPC; lsq->pred_PC = pred_PC;
	      lsq->in_LSQ = TRUE;
	      lsq->ea_comp = FALSE;
	      lsq->recover_inst = FALSE;
	      lsq->dir_update.pdir1 = lsq->dir_update.pdir2 = NULL;
	      lsq->dir_update.pmeta = NULL;
	      lsq->stack_recover_idx = 0;
	      lsq->spec_mode = spec_mode;
	      lsq->addr = addr;
	      lsq->seq = ++inst_seq;
	      lsq->queued = lsq->issued = lsq->completed = FALSE;
	      lsq->ptrace_seq = ptrace_seq++;
	      ptrace_newuop(lsq->ptrace_seq, "internal ld/st", lsq->PC, 0);
	      ptrace_newstage(lsq->ptrace_seq, PST_DISPATCH, 0);
	      ruu_link_idep(rs, 0, NA);
	      ruu_link_idep(rs, 1, in2);
	      ruu_link_idep(rs, 2, in3);
	      ruu_install_odep(rs, 0, DTMP);
	      ruu_install_odep(rs, 1, NA);
	      ruu_link_idep(lsq,
			    STORE_OP_INDEX,
			    in1);
	      ruu_link_idep(lsq,
			    STORE_ADDR_INDEX,
			    DTMP);
	      ruu_link_idep(lsq, 2, NA);
	      ruu_install_odep(lsq, 0, out1);
	      ruu_install_odep(lsq, 1, out2);
	      n_dispatched++;
	      RUU_tail = (RUU_tail + 1) % RUU_size;
	      RUU_num++;
	      LSQ_tail = (LSQ_tail + 1) % LSQ_size;
	      LSQ_num++;
	      if (OPERANDS_READY(rs))
		{
		  readyq_enqueue(rs);
		}
	      RSLINK_INIT(last_op, lsq);
	      if (((MD_OP_FLAGS(op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
		  && OPERANDS_READY(lsq))
		{
		  readyq_enqueue(lsq);
		}
	    }
	  else 
	    {
	      ruu_link_idep(rs, 0, in1);
	      ruu_link_idep(rs, 1, in2);
	      ruu_link_idep(rs, 2, in3);
	      ruu_install_odep(rs, 0, out1);
	      ruu_install_odep(rs, 1, out2);
	      n_dispatched++;
	      RUU_tail = (RUU_tail + 1) % RUU_size;
	      RUU_num++;
	      if (OPERANDS_READY(rs))
		{
		  readyq_enqueue(rs);
		  last_op = RSLINK_NULL;
		}
	      else
		{
		  RSLINK_INIT(last_op, rs);
		}
	    }
	}
      else
	{
	  rs = NULL;
	}
      sim_total_insn++;
      if (MD_OP_FLAGS(op) & F_CTRL)
	sim_total_branches++;
      if (!spec_mode)
	{
#if 0 
	  sim_num_insn++;
#endif
	  if (MD_OP_FLAGS(op) & F_CTRL)
	    {
	      sim_num_branches++;
	      if (pred && bpred_spec_update == spec_ID)
		{
		  bpred_update(pred,
			       regs.regs_PC,
			       regs.regs_NPC,
			       regs.regs_NPC != (regs.regs_PC +
						       sizeof(md_inst_t)),
			       pred_PC != (regs.regs_PC +
							sizeof(md_inst_t)),
			       pred_PC == regs.regs_NPC,
			       op,
			       &rs->dir_update);
		}
	    }
	  if (pred_PC != regs.regs_NPC && !fetch_redirected)
	    {
	      spec_mode = TRUE;
	      rs->recover_inst = TRUE;
	      recover_PC = regs.regs_NPC;
	    }
	}
      ptrace_newstage(pseq, PST_DISPATCH,
		      (pred_PC != regs.regs_NPC) ? PEV_MPOCCURED : 0);
      if (op == MD_NOP_OP)
	{
	  ptrace_endinst(pseq);
	}
      for (i=0; i<pcstat_nelt; i++)
	{
	  counter_t newval;
	  int delta;
	  newval = STATVAL(pcstat_stats[i]);
	  delta = newval - pcstat_lastvals[i];
	  if (delta != 0)
	    {
	      stat_add_samples(pcstat_sdists[i], regs.regs_PC, delta);
	      pcstat_lastvals[i] = newval;
	    }
	}
      fetch_head = (fetch_head+1) & (ruu_ifq_size - 1);
      fetch_num--;
      made_check = TRUE;
      if (dlite_check_break(pred_PC,
			    is_write ? ACCESS_WRITE : ACCESS_READ,
			    addr, sim_num_insn, sim_cycle))
	dlite_main(regs.regs_PC, pred_PC, sim_cycle, &regs, mem);
    }
  if (!made_check)
    {
      if (dlite_check_break(0,
			    is_write ? ACCESS_WRITE : ACCESS_READ,
			    addr, sim_num_insn, sim_cycle))
	dlite_main(regs.regs_PC, 0, sim_cycle, &regs, mem);
    }
}

void
simoutorder::fetch_init(void)
{
  fetch_data =
    (struct fetch_rec *)calloc(ruu_ifq_size, sizeof(struct fetch_rec));
  if (!fetch_data)
    fatal("out of virtual memory");
  fetch_num = 0;
  fetch_tail = fetch_head = 0;
  IFQ_count = 0;
  IFQ_fcount = 0;
}

void
simoutorder::fetch_dump(FILE *stream)			
{
  int num, head;
  if (!stream)
    stream = stderr;
  fprintf(stream, "** fetch stage state **\n");
  fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");
  myfprintf(stream, "pred_PC: 0x%08p, recover_PC: 0x%08p\n",
	    pred_PC, recover_PC);
  myfprintf(stream, "fetch_regs_PC: 0x%08p, fetch_pred_PC: 0x%08p\n",
	    fetch_regs_PC, fetch_pred_PC);
  fprintf(stream, "\n");
  fprintf(stream, "** fetch queue contents **\n");
  fprintf(stream, "fetch_num: %d\n", fetch_num);
  fprintf(stream, "fetch_head: %d, fetch_tail: %d\n",
	  fetch_head, fetch_tail);
  num = fetch_num;
  head = fetch_head;
  while (num)
    {
      fprintf(stream, "idx: %2d: inst: `", head);
      md_print_insn(fetch_data[head].IR, fetch_data[head].regs_PC, stream);
      fprintf(stream, "'\n");
      myfprintf(stream, "         regs_PC: 0x%08p, pred_PC: 0x%08p\n",
		fetch_data[head].regs_PC, fetch_data[head].pred_PC);
      head = (head + 1) & (ruu_ifq_size - 1);
      num--;
    }
}

void
simoutorder::ruu_fetch(void)
{
  int i, lat, tlb_lat, done = FALSE;
  md_inst_t inst;
  int stack_recover_idx;
  int branch_cnt;
  for (i=0, branch_cnt=0;
       i < (ruu_decode_width * fetch_speed)
       && fetch_num < ruu_ifq_size
       && !done;
       i++)
    {
      fetch_regs_PC = fetch_pred_PC;
      if (ld_text_base <= fetch_regs_PC
	  && fetch_regs_PC < (ld_text_base+ld_text_size)
	  && !(fetch_regs_PC & (sizeof(md_inst_t)-1)))
	{
	  MD_FETCH_INST(inst, mem, fetch_regs_PC);
	  lat = cache_il1_lat;
	  if (cache_il1)
	    {
	      lat =
		cache_access(cache_il1, Read, IACOMPRESS(fetch_regs_PC),
			     NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
			     NULL, NULL);
	      if (lat > cache_il1_lat)
		last_inst_missed = TRUE;
	    }
	  if (itlb)
	    {
	      tlb_lat =
		cache_access(itlb, Read, IACOMPRESS(fetch_regs_PC),
			     NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
			     NULL, NULL);
	      if (tlb_lat > 1)
		last_inst_tmissed = TRUE;
	      lat = MAX(tlb_lat, lat);
	    }
	  if (lat != cache_il1_lat)
	    {
	      ruu_fetch_issue_delay += lat - 1;
	      break;
	    }
	}
      else
	{
	  inst = MD_NOP_INST;
	}
      if (pred)
	{
	  enum md_opcode op;
	  MD_SET_OPCODE(op, inst);
	  if (MD_OP_FLAGS(op) & F_CTRL)
	    fetch_pred_PC =
	      bpred_lookup(pred,
			   fetch_regs_PC,
			   0,
			   op,
			   MD_IS_CALL(op),
			   MD_IS_RETURN(op),
			   &(fetch_data[fetch_tail].dir_update),
			   &stack_recover_idx);
	  else
	    fetch_pred_PC = 0;
	  if (!fetch_pred_PC)
	    {
	      fetch_pred_PC = fetch_regs_PC + sizeof(md_inst_t);
	    }
	  else
	    {
	      branch_cnt++;
	      if (branch_cnt >= fetch_speed)
		done = TRUE;
	    }
	}
      else
	{
	  fetch_pred_PC = fetch_regs_PC + sizeof(md_inst_t);
	}
      fetch_data[fetch_tail].IR = inst;
      fetch_data[fetch_tail].regs_PC = fetch_regs_PC;
      fetch_data[fetch_tail].pred_PC = fetch_pred_PC;
      fetch_data[fetch_tail].stack_recover_idx = stack_recover_idx;
      fetch_data[fetch_tail].ptrace_seq = ptrace_seq++;
      ptrace_newinst(fetch_data[fetch_tail].ptrace_seq,
		     inst, fetch_data[fetch_tail].regs_PC,
		     0);
      ptrace_newstage(fetch_data[fetch_tail].ptrace_seq,
		      PST_IFETCH,
		      ((last_inst_missed ? PEV_CACHEMISS : 0)
		       | (last_inst_tmissed ? PEV_TLBMISS : 0)));
      last_inst_missed = FALSE;
      last_inst_tmissed = FALSE;
      fetch_tail = (fetch_tail + 1) & (ruu_ifq_size - 1);
      fetch_num++;
    }
}

char *					
simoutorder::simoo_mstate_obj(FILE *stream,			
		 char *cmd,			
		 struct regs_t *regs,		
		 struct mem_t *mem)		
{
  if (!cmd || !strcmp(cmd, "help"))
    fprintf(stream,
"mstate commands:\n"
"\n"
"    mstate help   - show all machine-specific commands (this list)\n"
"    mstate stats  - dump all statistical variables\n"
"    mstate res    - dump current functional unit resource states\n"
"    mstate ruu    - dump contents of the register update unit\n"
"    mstate lsq    - dump contents of the load/store queue\n"
"    mstate eventq - dump contents of event queue\n"
"    mstate readyq - dump contents of ready instruction queue\n"
"    mstate cv     - dump contents of the register create vector\n"
"    mstate rspec  - dump contents of speculative regs\n"
"    mstate mspec  - dump contents of speculative memory\n"
"    mstate fetch  - dump contents of fetch stage registers and fetch queue\n"
"\n"
	    );
  else if (!strcmp(cmd, "stats"))
    {
      sim_print_stats(stream);
    }
  else if (!strcmp(cmd, "res"))
    {
      res_dump(fu_pool, stream);
    }
  else if (!strcmp(cmd, "ruu"))
    {
      ruu_dump(stream);
    }
  else if (!strcmp(cmd, "lsq"))
    {
      lsq_dump(stream);
    }
  else if (!strcmp(cmd, "eventq"))
    {
      eventq_dump(stream);
    }
  else if (!strcmp(cmd, "readyq"))
    {
      readyq_dump(stream);
    }
  else if (!strcmp(cmd, "cv"))
    {
      cv_dump(stream);
    }
  else if (!strcmp(cmd, "rspec"))
    {
      rspec_dump(stream);
    }
  else if (!strcmp(cmd, "mspec"))
    {
      mspec_dump(stream);
    }
  else if (!strcmp(cmd, "fetch"))
    {
      fetch_dump(stream);
    }
  else
    return "unknown mstate command";
  return NULL;
}

void
simoutorder::sim_main(void)
{
  signal(SIGFPE, SIG_IGN);
  regs.regs_PC = ld_prog_entry;
  regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);
  if (dlite_check_break(regs.regs_PC, 0, 0, 0, 0))
    dlite_main(regs.regs_PC, regs.regs_PC + sizeof(md_inst_t),
	       sim_cycle, &regs, mem);
  if (fastfwd_count > 0)
    {
      int icount;
      md_inst_t inst;			
      enum md_opcode op;		
      md_addr_t target_PC;		
      md_addr_t addr;			
      int is_write;			
      byte_t temp_byte = 0;		
      half_t temp_half = 0;		
      word_t temp_word = 0;		
#ifdef HOST_HAS_QWORD
      qword_t temp_qword = 0;		
#endif 
      enum md_fault_type fault;
      fprintf(stderr, "sim: ** fast forwarding %d insts **\n", fastfwd_count);
      for (icount=0; icount < fastfwd_count; icount++)
	{
	  regs.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
	  regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif 
	  MD_FETCH_INST(inst, mem, regs.regs_PC);
	  addr = 0; is_write = FALSE;
	  fault = md_fault_none;
	  MD_SET_OPCODE(op, inst);
	  switch (op)
	    {
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
	    case OP:							\
	      SYMCAT(OP,_IMPL);						\
	      break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
	    case OP:							\
	      panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#undef DECLARE_FAULT
#define DECLARE_FAULT(FAULT)						\
	      { fault = (FAULT); break; }
#include "machine.def"
	    default:
	      panic("attempted to execute a bogus opcode");
	    }
	  if (fault != md_fault_none)
	    fatal("fault (%d) detected @ 0x%08p", fault, regs.regs_PC);
	  if (MD_OP_FLAGS(op) & F_MEM)
	    {
	      if (MD_OP_FLAGS(op) & F_STORE)
		is_write = TRUE;
	    }
	  if (dlite_check_break(regs.regs_NPC,
				is_write ? ACCESS_WRITE : ACCESS_READ,
				addr, sim_num_insn, sim_num_insn))
	    dlite_main(regs.regs_PC, regs.regs_NPC, sim_num_insn, &regs, mem);
	  regs.regs_PC = regs.regs_NPC;
	  regs.regs_NPC += sizeof(md_inst_t);
	}
    }
  fprintf(stderr, "sim: ** starting performance simulation **\n");
  fetch_regs_PC = regs.regs_PC - sizeof(md_inst_t);
  fetch_pred_PC = regs.regs_PC;
  regs.regs_PC = regs.regs_PC - sizeof(md_inst_t);
  for (;;)
    {
      if (RUU_num < LSQ_num)
	panic("RUU_num < LSQ_num");
      if (((RUU_head + RUU_num) % RUU_size) != RUU_tail)
	panic("RUU_head/RUU_tail wedged");
      if (((LSQ_head + LSQ_num) % LSQ_size) != LSQ_tail)
	panic("LSQ_head/LSQ_tail wedged");
      ptrace_check_active(regs.regs_PC, sim_num_insn, sim_cycle);
      ptrace_newcycle(sim_cycle);
      ruu_commit();
      ruu_release_fu();
      ruu_writeback();
      if (!bugcompat_mode)
	{
	  lsq_refresh();
	  ruu_issue();
	}
      ruu_dispatch();
      if (bugcompat_mode)
	{
	  lsq_refresh();
	  ruu_issue();
	}
      if (!ruu_fetch_issue_delay)
	ruu_fetch();
      else
	ruu_fetch_issue_delay--;
      IFQ_count += fetch_num;
      IFQ_fcount += ((fetch_num == ruu_ifq_size) ? 1 : 0);
      RUU_count += RUU_num;
      RUU_fcount += ((RUU_num == RUU_size) ? 1 : 0);
      LSQ_count += LSQ_num;
      LSQ_fcount += ((LSQ_num == LSQ_size) ? 1 : 0);
      sim_cycle++;
      if (max_insts && sim_num_insn >= max_insts)
	return;
    }
}
