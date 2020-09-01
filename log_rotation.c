/********************* LOG ROTATION ******************************/
/*
Why write into log files?
=========================
One of the main debugging/trouble shooting tools for multi threaded projects is a log file for function 
entry/exit and diagnostic purpose data logging and error logging. Logs have to be thread safe for this purpose 
as multiple threads can access the log file simultaneously.

Setting a log mechanism
=======================
This mechanism creates a child logger process, it reads the logging message by pipe what parent process writes 
on it.

This routine sets up a log: it sets up a pipe and forks. Then the child reads from the pipe and writes what it 
has read to some central logging destination, adding a timestamp. The parent redirects its stderr to point to 
the write end of the pipe, so that any write in the parent to stderr will end up in the log along with a timestamp.

Argument 'name' is the name of a file into which to redirect all log messages.

Note: We can provide host name and port if we want to log into a file on a central server.
*/

/* Beauty of below code:
 * We want to create a logger child process which will be dedicated to log the messages in a file which will
 * written on pipe by parent process. So, the below two problem occurs for this implementation:
 * 1. We can't have waitpid() in parent process to wait for child process because if waitpid is called, parent 
 * process will stuck there and wont execute further. Our parent process is main process for all function execu-
 * tion so, this is not possible.
 * 2. We can't keep executing in parent process without having waiting for child process because sometimes parent
 * process will die due to some execution or child process will become orphan process.
 *
 * So, we want something so that child process still be copy of parent process so could share the same pipe for
 * communication and child shouldnot be dependent on parent so even parent exits, child won't become orphan.
 *
 *To make this happen, we create a grand child of the process and let child die. So once child dies grand child
  is collected by init process table cleanup responsibility is transferred to init. Though, main process will 
  wait on waitpid but it will move on because own child is already died. Since, grandchild is copy of the child
  process so it will have the copy of pipe fd to communicate with grandparent(main process). Now, whatever
  grandparent(main process will write on the pipe, grandchild will read the same and write on the log file.
  */
/* Note:
 * If a program forks a child process and it dies somehow during execution or exit()/_exit() or killed/terminated
 * by other process or killed/terminated by command line (kill -9) command. Child process will be alive and 
 * become a orphan process.
 *
 * In PB logger, if we kill parent process then child process also dies. It happens because child process is in
 * while loop to read from shared pipe fd. Once parent process dies, pipe is broken and child process comeas out
 * of while loop and terminates itself.
 *
 * Same behaviour will be adhered by below mentioned code also.
 *
 * Parent Process Id = x;
 * Child Proces Id = x+1;
 * Grand Child Process Id = x+2;
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include <stdarg.h>
#include <string.h>
#include<sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>


void pbInitLog(const char* name)
{
   int pfd[2], msec, tstamp, vflag;
   pid_t kid_pid;
   char buf[8192], tbuf1[32], tbuf2[8];
   FILE *fin, *fout = NULL;
   struct timeval time_tv;
   struct tm time_tm;
   int usec_div, usec_width;
   int ret = 0;
   FILE *ret_fp = NULL;

   if (pipe(pfd) != 0) {
       fprintf(stderr, "pbInitLog: pipe failed! %s\n", strerror(errno));
       exit(1);
   }

   kid_pid = fork();
   if (kid_pid == -1) {
      fprintf(stderr, "pbInitLog: fork failed! %s\n", strerror(errno));
      exit(1);
   } else if (kid_pid == 0) {
     // double fork, so that (grand-)child will become inherited
     // by init and we can safely waitpid() on child below
     kid_pid = fork();
     if (kid_pid == -1) {
         fprintf(stderr, "pbInitLog: fork failed! %s\n", strerror(errno));
         exit(1);
     } else if (kid_pid > 0) {
         _exit(0);   // don't call atexit() or any cleanup routine
     }
     ret = close(pfd[1]);   // write end is closed in grand-child logger process
     if (-1 == ret) {
         fprintf(stderr, "pbInitLog: pipe close failed at %d. Errno %d\n",
                 __LINE__, errno);
     }

     fin = fdopen(pfd[0], "r");
     if (NULL == fin) {
         fprintf(stderr, "pbInitLog: fdopen failed at %d. Errno %d\n",
                 __LINE__, errno);
     }

     fout = fopen(name, "w");
     if ((fin == NULL) || (fout == NULL)) {
         fprintf(stderr, "pbInitLog: f[d]open failed! %s\n",
                 strerror(errno));
         kill(getppid(), SIGTERM); // zombie killer
         exit(1);
     }

     gettimeofday(&time_tv, NULL);
     localtime_r(&time_tv.tv_sec, &time_tm);
     msec = time_tv.tv_usec/1000;
     strftime(tbuf1, sizeof(tbuf1), "%F %T", &time_tm);
     strftime(tbuf2, sizeof(tbuf2), "%Z", &time_tm);
     fprintf(fout, "%s.%03d %s Logging started\n", tbuf1, msec, tbuf2);
     fflush(fout);

     /* by default we print timestamp in milliseconds */
     usec_width = 3;
     usec_div = 1000;

     ret_fp = freopen("/dev/null", "w", stderr);
     if (NULL == ret_fp) {
         fprintf(fout, "pbInitLog: freopen on /dev/null failed\n");
         fflush(fout);
     }

     gettimeofday(&time_tv, NULL);
     localtime_r(&time_tv.tv_sec, &time_tm);
     strftime(tbuf1, sizeof(tbuf1), "%m-%d %T", &time_tm);
     fprintf(fout, "%s.%0*ld ", tbuf1, usec_width,
             time_tv.tv_usec/usec_div);
     fprintf(fout, "pbInitLog: freopen succeeded\n");
     fflush(fout);

     /* Allow for possibility of a message being written in pieces,
           or something that's too long to fit into buf in one fgets;
           however, it's better for the writer(s) on the other end of
           the pipe to avoid this, because it can mangle messages. */

     tstamp = 1;
     while (fgets(buf, sizeof(buf), fin)) {
           if (tstamp) {
               gettimeofday(&time_tv, NULL);
               localtime_r(&time_tv.tv_sec, &time_tm);
               strftime(tbuf1, sizeof(tbuf1), "%m-%d %T", &time_tm);
               fprintf(fout, "%s.%0*ld %s", tbuf1, usec_width, 
                       time_tv.tv_usec/usec_div, buf);
           } else {    
                fputs(buf, fout);
           }   
           tstamp = (strchr(buf, '\n') != NULL);
           fflush(fout);
     }

     gettimeofday(&time_tv, NULL);
     localtime_r(&time_tv.tv_sec, &time_tm);
     strftime(tbuf1, sizeof(tbuf1), "%m-%d %T", &time_tm);
     fprintf(fout, "%s.%0*ld ", tbuf1, usec_width,
             time_tv.tv_usec/usec_div);
     fprintf(fout, "pbInitLog: Out of listen loop\n");
     fflush(fout);

     ret = fclose(fin);
     if (ret != 0) {
         fprintf(fout, "pbInitLog: fclose failed for fin. Errno: %d\n",
                 errno);
         fflush(fout);
     }

     gettimeofday(&time_tv, NULL);
     localtime_r(&time_tv.tv_sec, &time_tm);
     strftime(tbuf1, sizeof(tbuf1), "%m-%d %T", &time_tm);
     fprintf(fout, "%s.%0*ld ", tbuf1, usec_width,
             time_tv.tv_usec/usec_div);
     fprintf(fout, "pbInitLog: fclose'd for fin\n");
     fflush(fout);

     gettimeofday(&time_tv, NULL);
     localtime_r(&time_tv.tv_sec, &time_tm);
     msec = time_tv.tv_usec/1000;
     strftime(tbuf1, sizeof(tbuf1), "%F %T", &time_tm);
     strftime(tbuf2, sizeof(tbuf2), "%Z", &time_tm);
     fprintf(fout, "%s.%03d %s Logging complete\n", tbuf1, msec, tbuf2);
     fclose(fout);

     close(pfd[0]);

     _exit(0);

  } else {
     if (waitpid(kid_pid, NULL, 0) < 0) {   // own child dies, grand child becomes logger proc so won't stop here
        fprintf(stderr, "waitpid(%d) failed! zombie?\n", kid_pid);
     }

     fprintf(stderr, "pbInitLog: Out of waitpid\n");
     
     /* parent writes to pipe: close read end */
     ret = close(pfd[0]);
     if (ret != 0) {
         fprintf(stderr,"pbInitLog: Unable to close pipe's read end. "
                 "Errno %d\n", errno);
     }

     fprintf(stderr, "pbInitLog: Close read end of pipe\n");

     if (dup2(pfd[1], fileno(stderr)) == -1) {   // here redirecting stderr to write end of pipe
         fprintf(stderr, "pbInitLog: dup2 failed! %s\n", strerror(errno));
         exit(1);
     }

     fprintf(stderr, "pbInitLog: dup2 done\n");

     ret = close(pfd[1]);
     if (ret != 0) {
         fprintf(stderr,"pbInitLog: Unable to close pipe's read end. "
                 "Errno %d\n", errno);
     }

     setbuf(stderr, NULL);   // assure stderr unbuffered from parent
     fprintf(stderr, "pbInitLog: Unbuffered\n");

   }
}

int main()
{
   char log_file[20] = {0};
   snprintf(log_file, sizeof(log_file), "./nik_log.txt"); 
   return 0;
}
