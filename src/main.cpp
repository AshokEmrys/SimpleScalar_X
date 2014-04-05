/* main.c - main line routines */

/* SimpleScalar(TM) Tool Suite
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 * All Rights Reserved. 
 * 
 * THIS IS A LEGAL DOCUMENT, BY USING SIMPLESCALAR,
 * YOU ARE AGREEING TO THESE TERMS AND CONDITIONS.
 * 
 * No portion of this work may be used by any commercial entity, or for any
 * commercial purpose, without the prior, written permission of SimpleScalar,
 * LLC (info@simplescalar.com). Nonprofit and noncommercial use is permitted
 * as described below.
 * 
 * 1. SimpleScalar is provided AS IS, with no warranty of any kind, express
 * or implied. The user of the program accepts full responsibility for the
 * application of the program and the use of any results.
 * 
 * 2. Nonprofit and noncommercial use is encouraged. SimpleScalar may be
 * downloaded, compiled, executed, copied, and modified solely for nonprofit,
 * educational, noncommercial research, and noncommercial scholarship
 * purposes provided that this notice in its entirety accompanies all copies.
 * Copies of the modified software can be delivered to persons who use it
 * solely for nonprofit, educational, noncommercial research, and
 * noncommercial scholarship purposes provided that this notice in its
 * entirety accompanies all copies.
 * 
 * 3. ALL COMMERCIAL USE, AND ALL USE BY FOR PROFIT ENTITIES, IS EXPRESSLY
 * PROHIBITED WITHOUT A LICENSE FROM SIMPLESCALAR, LLC (info@simplescalar.com).
 * 
 * 4. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 5. Noncommercial and nonprofit users may distribute copies of SimpleScalar
 * in compiled or executable form as set forth in Section 2, provided that
 * either: (A) it is accompanied by the corresponding machine-readable source
 * code, or (B) it is accompanied by a written offer, with no time limit, to
 * give anyone a machine-readable copy of the corresponding source code in
 * return for reimbursement of the cost of distribution. This written offer
 * must permit verbatim duplication by anyone, or (C) it is distributed by
 * someone who received only the executable form, and is accompanied by a
 * copy of the written offer of source code.
 * 
 * 6. SimpleScalar was developed by Todd M. Austin, Ph.D. The tool suite is
 * currently maintained by SimpleScalar LLC (info@simplescalar.com). US Mail:
 * 2395 Timbercrest Court, Ann Arbor, MI 48105.
 * 
 * Copyright (C) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/time.h>
#include <thread>
#endif
#ifdef BFD_LOADER
#include <bfd.h>
#endif /* BFD_LOADER */

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "endian.h"
#include "version.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include "loader.h"
#include "sim.h"
#include "sim-outorder.hpp"

/* stats signal handler */
static void
signal_sim_stats(int sigtype)
{
  sim_dump_stats = TRUE;
}

/* exit signal handler */
static void
signal_exit_now(int sigtype)
{
  sim_exit_now = TRUE;
}

/* execution instruction counter */
counter_t sim_num_insn = 0;

#if 0 /* not portable... :-( */
/* total simulator (data) memory usage */
unsigned int sim_mem_usage = 0;
#endif

/* execution start/end times */
time_t sim_start_time;
time_t sim_end_time;
int sim_elapsed_time;

/* byte/word swapping required to execute target executable on this host */
int sim_swap_bytes;
int sim_swap_words;

/* exit when this becomes non-zero */
int sim_exit_now = FALSE;

/* longjmp here when simulation is completed */
jmp_buf sim_exit_buf;

/* set to non-zero when simulator should dump statistics */
int sim_dump_stats = FALSE;

/* options database */
struct opt_odb_t *sim_odb;
struct opt_odb_t *sim_odb2;
/* stats database */
struct stat_sdb_t *sim_sdb;
struct stat_sdb_t *sim_sdb2;
/* EIO interfaces */
char *sim_eio_fname = NULL;
char *sim_chkpt_fname = NULL;
FILE *sim_eio_fd = NULL;

/* redirected program/simulator output file names */
static char *sim_simout = NULL;
static char *sim_progout = NULL;
FILE *sim_progfd = NULL;

/* track first argument orphan, this is the program to execute */
static int exec_index = -1;

/* dump help information */
static int help_me;

/* random number generator seed */
static int rand_seed;

/* initialize and quit immediately */
static int init_quit;

#ifndef _MSC_VER
/* simulator scheduling priority */
static int nice_priority;
#endif

/* default simulator scheduling priority */
#define NICE_DEFAULT_VALUE		0

static int
orphan_fn(int i, int argc, char **argv)
{
  exec_index = i;
  return /* done */FALSE;
}

static void
banner(FILE *fd, int argc, char **argv)
{
  char *s;

  fprintf(fd,
	  "%s: SimpleScalar/%s Tool Set version %d.%d of %s.\n"
	  "Copyright (c) 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.\n"
	  "All Rights Reserved. This version of SimpleScalar is licensed for academic\n"
	  "non-commercial use.  No portion of this work may be used by any commercial\n"
	  "entity, or for any commercial purpose, without the prior written permission\n"
	  "of SimpleScalar, LLC (info@simplescalar.com).\n"
	  "\n",
	  ((s = strrchr(argv[0], '/')) ? s+1 : argv[0]),
	  VER_TARGET, VER_MAJOR, VER_MINOR, VER_UPDATE);
}

static void
usage(FILE *fd, int argc, char **argv)
{
  fprintf(fd, "Usage: %s {-options} executable {arguments}\n", argv[0]);
  opt_print_help(sim_odb, fd);
}

static int running = FALSE;

/* print all simulator stats */
void
sim_print_stats(FILE *fd)		/* output stream */
{
#if 0 /* not portable... :-( */
  extern char etext, *sbrk(int);
#endif

  if (!running)
    return;

  /* get stats time */
  sim_end_time = time((time_t *)NULL);
  sim_elapsed_time = MAX(sim_end_time - sim_start_time, 1);

#if 0 /* not portable... :-( */
  /* compute simulator memory usage */
  sim_mem_usage = (sbrk(0) - &etext) / 1024;
#endif

  /* print simulation stats */
  fprintf(fd, "\nsim: ** simulation statistics **\n");
  stat_print_stats(sim_sdb, fd);
  //sim_aux_stats(fd);
  fprintf(fd, "\n");
}

/* print stats, uninitialize simulator components, and exit w/ exitcode */
static void
exit_now(int exit_code, simoutorder *simobj)
{
  /* print simulation stats */
  sim_print_stats(stderr);

  /* un-initialize the simulator */
  simobj->sim_uninit();

  /* all done! */
  //exit(exit_code);
}
int init(struct stat_sdb_t* &in_statdb, 
         struct opt_odb_t* &in_optdb,
         simoutorder* in_simobj,
         int argc,
         char** argv,
         char **envp
         )
{
in_optdb = opt_new(orphan_fn);
opt_reg_flag(in_optdb, "-h", "print help message",
	       &help_me, /* default */FALSE, /* !print */FALSE, NULL);
  opt_reg_flag(in_optdb, "-v", "verbose operation",
	       &verbose, /* default */FALSE, /* !print */FALSE, NULL);
#ifdef DEBUG
  opt_reg_flag(in_optdb, "-d", "enable debug message",
	       &debugging, /* default */FALSE, /* !print */FALSE, NULL);
#endif /* DEBUG */
  opt_reg_flag(in_optdb, "-i", "start in Dlite debugger",
	       &dlite_active, /* default */FALSE, /* !print */FALSE, NULL);
  opt_reg_int(in_optdb, "-seed",
	      "random number generator seed (0 for timer seed)",
	      &rand_seed, /* default */1, /* print */TRUE, NULL);
  opt_reg_flag(in_optdb, "-q", "initialize and terminate immediately",
	       &init_quit, /* default */FALSE, /* !print */FALSE, NULL);
  opt_reg_string(in_optdb, "-chkpt", "restore EIO trace execution from <fname>",
		 &sim_chkpt_fname, /* default */NULL, /* !print */FALSE, NULL);

  /* stdio redirection options */
  opt_reg_string(in_optdb, "-redir:sim",
		 "redirect simulator output to file (non-interactive only)",
		 &sim_simout,
		 /* default */NULL, /* !print */FALSE, NULL);
  opt_reg_string(in_optdb, "-redir:prog",
		 "redirect simulated program output to file",
		 &sim_progout, /* default */NULL, /* !print */FALSE, NULL);
  #ifndef _MSC_VER
  /* scheduling priority option */
  opt_reg_int(in_optdb, "-nice",
	      "simulator scheduling priority", &nice_priority,
	      /* default */NICE_DEFAULT_VALUE, /* print */TRUE, NULL);
#endif
in_simobj->sim_reg_options(in_optdb);

exec_index = -1;
  opt_process_options(in_optdb, argc, argv);

  /* redirect I/O? */
  if (sim_simout != NULL)
    {
      /* send simulator non-interactive output (STDERR) to file SIM_SIMOUT */
      fflush(stderr);
      if (!freopen(sim_simout, "w", stderr))
	fatal("unable to redirect simulator output to file `%s'", sim_simout);
    }

  if (sim_progout != NULL)
    {
      /* redirect simulated program output to file SIM_PROGOUT */
      sim_progfd = fopen(sim_progout, "w");
      if (!sim_progfd)
	fatal("unable to redirect program output to file `%s'", sim_progout);
    }

  /* need at least two argv values to run */
  if (argc < 2)
    {
      banner(stderr, argc, argv);
      usage(stderr, argc, argv);
      exit(1);
    }
  
  if (help_me)
    {
      /* print help message and exit */
      usage(stderr, argc, argv);
      exit(1);
    }

  /* seed the random number generator */
  if (rand_seed == 0)
    {
      /* seed with the timer value, true random */
      mysrand(time((time_t *)NULL));
    }
  else
    {
      /* seed with default or user-specified random number generator seed */
      mysrand(rand_seed);
    }

  /* exec_index is set in orphan_fn() */
  if (exec_index == -1)
    {
      /* executable was not found */
      fprintf(stderr, "error: no executable specified\n");
      usage(stderr, argc, argv);
      exit(1);
    }
  /* else, exec_index points to simulated program arguments */

  /* check simulator-specific options */
  in_simobj->sim_check_options(in_optdb, argc, argv);

#ifndef _MSC_VER
  /* set simulator scheduling priority */
  if (nice(0) < nice_priority)
    {
      if (nice(nice_priority - nice(0)) < 0)
        fatal("could not renice simulator process");
    }
#endif

  /* default architected value... */
  sim_num_insn = 0;

#ifdef BFD_LOADER
  /* initialize the bfd library */
  bfd_init();
#endif /* BFD_LOADER */



  /* initialize all simulation modules */
  in_simobj->sim_init();

  /* initialize architected state */
  in_simobj->sim_load_prog(argv[exec_index], argc-exec_index, argv+exec_index, envp);
 
  /* register all simulator stats */
  in_statdb = stat_new();
  in_simobj->sim_reg_stats(in_statdb);
  
  return 1;
  
}
int
main(int argc, char **argv, char **envp)
{
  char *s;
  int i, exit_code;
simoutorder sim1, sim2;
#ifndef _MSC_VER
  /* catch SIGUSR1 and dump intermediate stats */
  signal(SIGUSR1, signal_sim_stats);

  /* catch SIGUSR2 and dump final stats and exit */
  signal(SIGUSR2, signal_exit_now);
#endif /* _MSC_VER */

  /* register an error handler */
  fatal_hook(sim_print_stats);

  /* set up a non-local exit point */
  if ((exit_code = setjmp(sim_exit_buf)) != 0)
    {
      /* special handling as longjmp cannot pass 0 */
      exit_now(exit_code-1, &sim1);
      goto sim2ex;
    }
  
  

  banner(stderr, argc, argv);

 

  sim_num_insn = 0;

#ifdef BFD_LOADER
  /* initialize the bfd library */
  bfd_init();
#endif /* BFD_LOADER */

  /* initialize the instruction decoder */
  md_init_decoder();

  init(sim_sdb, 
         sim_odb,
         &sim1,
         argc,
         argv,
         envp
         );
 init(sim_sdb2, 
         sim_odb2,
         &sim2,
         argc,
         argv,
         envp
         );
  /* record start of execution time, used in rate stats */
  sim_start_time = time((time_t *)NULL);

  /* emit the command line for later reuse */
  fprintf(stderr, "sim: command line: ");
  for (i=0; i < argc; i++)
    fprintf(stderr, "%s ", argv[i]);
  fprintf(stderr, "\n");

  /* output simulation conditions */
  s = ctime(&sim_start_time);
  if (s[strlen(s)-1] == '\n')
    s[strlen(s)-1] = '\0';
  fprintf(stderr, "\nsim: simulation started @ %s, options follow:\n", s);
  opt_print_options(sim_odb, stderr, /* short */TRUE, /* notes */TRUE);
  sim1.sim_aux_config(stderr);
  fprintf(stderr, "\n");

  /* omit option dump time from rate stats */
  sim_start_time = time((time_t *)NULL);

  if (init_quit)
    exit_now(0, &sim1);

  running = TRUE;
 
  sim1.sim_main();
  running = TRUE;     
  
  
  sim2ex:
  sim2.sim_main();

  /* simulation finished early */
  exit_now(0, &sim1);

  return 0;
}
