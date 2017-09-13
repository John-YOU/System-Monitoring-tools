/*
 * cui.cpp
 *
 * Copyright (c) 2004-2006,2008,2010,2011 Arnout Engelen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *USA.
 *
 */

/* NetHogs console UI */
#include <string>
#include <pwd.h>
#include <sys/types.h>
#include <cstdlib>
#include <cerrno>
#include <cstdlib>
#include <algorithm>
///start
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sys/sysctl.h>
///end
#include <ncurses.h>
#include "nethogs.h"
#include "process.h"

std::string *caption;
extern const char version[];
extern ProcList *processes;
extern timeval curtime;

extern Process *unknowntcp;
extern Process *unknownudp;
extern Process *unknownip;

extern bool sortRecv;

extern int viewMode;
///start
extern std::string outFilePath;
extern int tracingPid;
bool flag=true;///check if it's the first time
///end
extern unsigned refreshlimit;
extern unsigned refreshcount;

#define PID_MAX 4194303

const int COLUMN_WIDTH_PID = 7;
const int COLUMN_WIDTH_USER = 8;
const int COLUMN_WIDTH_DEV = 5;
const int COLUMN_WIDTH_SENT = 11;
const int COLUMN_WIDTH_RECEIVED = 11;
const int COLUMN_WIDTH_UNIT = 6;

const char *COLUMN_FORMAT_PID = "%7d";
const char *COLUMN_FORMAT_SENT = "%11.3f";
const char *COLUMN_FORMAT_RECEIVED = "%11.3f";

class Line {
public:
  Line(const char *name, double n_recv_value, double n_sent_value, pid_t pid,
       uid_t uid, const char *n_devicename) {
    assert(pid >= 0);
    assert(pid <= PID_MAX);
    m_name = name;
    sent_value = n_sent_value;
    recv_value = n_recv_value;
    devicename = n_devicename;
    m_pid = pid;
    m_uid = uid;
    assert(m_pid >= 0);
  }

  void show(int row, unsigned int proglen);
  void log();

  double sent_value;
  double recv_value;

private:
  const char *m_name;
  const char *devicename;
  pid_t m_pid;
  uid_t m_uid;
};

#include <sstream>

std::string itoa(int i) {
  std::stringstream out;
  out << i;
  return out.str();
}

/**
 * @returns the username that corresponds to this uid
 */
std::string uid2username(uid_t uid) {
  struct passwd *pwd = NULL;
  errno = 0;

  /* points to a static memory area, should not be freed */
  pwd = getpwuid(uid);

  if (pwd == NULL)
    if (errno == 0)
      return itoa(uid);
    else
      forceExit(false, "Error calling getpwuid(3) for uid %d: %d %s", uid,
                errno, strerror(errno));
  else
    return std::string(pwd->pw_name);
}

/**
 * Render the provided text at the specified location, truncating if the length
 * of the text exceeds a maximum. If the
 * text must be truncated, the string ".." will be rendered, followed by max_len
 * - 2 characters of the provided text.
 */
static void mvaddstr_truncate_leading(int row, int col, const char *str,
                                      std::size_t str_len,
                                      std::size_t max_len) {
  if (str_len < max_len) {
    mvaddstr(row, col, str);
  } else {
    mvaddstr(row, col, "..");
    addnstr(str + 2, max_len - 2);
  }
}

/**
 * Render the provided text at the specified location, truncating if the length
 * of the text exceeds a maximum. If the
 * text must be truncated, the text will be rendered up to max_len - 2
 * characters and then ".." will be rendered.
 */

static void mvaddstr_truncate_trailing(int row, int col, const char *str,
                                       std::size_t str_len,
                                       std::size_t max_len) {
  if (str_len < max_len) {
    mvaddstr(row, col, str);
  } else {
    mvaddnstr(row, col, str, max_len - 2);
    addstr("..");
  }
}


///start
static int wtf_truncate_trailing(int col, const char *str,
                                       std::size_t str_len,
                                       std::size_t max_len, std::ofstream& ofile) {
  if (str_len < max_len) {
	  for (int i=0;i<col;i++)
	      	ofile<<" ";
	  ofile<<str;
	  return str_len;
  } else {
	  for (int i=0;i<col;i++)
		  ofile<<" ";
	  std::string tmp(str,0,max_len-2);
	  ofile<<tmp;
	  ofile<<"..";
	  return max_len;
  }
}
static int wtf_truncate_leading(int col, const char *str,
										std::size_t str_len,
										std::size_t max_len, std::ofstream& ofile) {
  if (str_len < max_len) {
	  for (int i=0;i<col;i++)
	  	      	ofile<<" ";
	  ofile<<str;
	  return str_len;
  } else {
	  for (int i=0;i<col;i++)
		  ofile<<" ";
	  ofile<<"..";
	  std::string tmp(str,0,max_len-2);
	  ofile<<tmp;
	  return max_len;
  }
}
static void skip_write(int col, const char *str,std::ofstream& ofile){
	for (int i=0;i<col;i++)
		  	      	ofile<<" ";
	ofile<<str;
}
///end

void Line::show(int row, unsigned int proglen) {
  assert(m_pid >= 0);
  assert(m_pid <= PID_MAX);

  const int column_offset_pid = 0;
  const int column_offset_user = column_offset_pid + COLUMN_WIDTH_PID + 1;
  const int column_offset_program = column_offset_user + COLUMN_WIDTH_USER + 1;
  const int column_offset_dev = column_offset_program + proglen + 2;
  const int column_offset_sent = column_offset_dev + COLUMN_WIDTH_DEV + 1;
  const int column_offset_received = column_offset_sent + COLUMN_WIDTH_SENT + 1;
  const int column_offset_unit =
      column_offset_received + COLUMN_WIDTH_RECEIVED + 1;

  ///start
  int currentIndex=0;//record current index
  std::ofstream outfile;
  std::string filename=outFilePath;
  outfile.open(filename.c_str(), std::ofstream::app);
  char* x;
  ///end

  ///TIME
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
  std::string dt(buffer);
  outfile<<dt;

  // PID column
  if (m_pid == 0){
    mvaddch(row, column_offset_pid + COLUMN_WIDTH_PID - 1, '?');
    ///start
    for (int i=0;i<column_offset_pid + COLUMN_WIDTH_PID - 1;i++)
    	outfile<<" ";
    outfile<<"?";
    ///end
  }
  else{
    mvprintw(row, column_offset_pid, COLUMN_FORMAT_PID, m_pid);
    ///start
    for (int i=0;i<column_offset_pid;i++)
    	outfile<<" ";
    x= new char[100];
    sprintf(x,COLUMN_FORMAT_PID, m_pid);
    std::string str=x;
    outfile<<str;
    ///end
  }

  std::string username = uid2username(m_uid);

  mvaddstr_truncate_trailing(row, column_offset_user, username.c_str(),
                             username.size(), COLUMN_WIDTH_USER);
  mvaddstr_truncate_leading(row, column_offset_program, m_name, strlen(m_name),
                            proglen);
  mvaddstr(row, column_offset_dev, devicename);
  mvprintw(row, column_offset_sent, COLUMN_FORMAT_SENT, sent_value);

  mvprintw(row, column_offset_received, COLUMN_FORMAT_RECEIVED, recv_value);

  ///start
  currentIndex=column_offset_user-1;//for clarify
  currentIndex=column_offset_user + wtf_truncate_trailing(1,username.c_str(),
          	  	  	  	  	  username.size(), COLUMN_WIDTH_USER, outfile);
  currentIndex=column_offset_program + wtf_truncate_leading(column_offset_program-currentIndex,m_name,
		  	  	  	  	  	  strlen(m_name), proglen, outfile);
  skip_write(column_offset_dev-currentIndex,devicename,outfile);
  currentIndex=column_offset_dev+strlen(devicename);

  skip_write(column_offset_sent-currentIndex, "", outfile);
  x=new char[64];
  sprintf(x,COLUMN_FORMAT_SENT, sent_value);
  std::string str=x;
  outfile<<str;
  currentIndex=column_offset_sent+11;

  x=new char[64];
  sprintf(x,COLUMN_FORMAT_RECEIVED, recv_value);
  str=x;
  outfile<<str;
  currentIndex=column_offset_received+11;

  if (viewMode == VIEWMODE_KBPS) {
		  skip_write(column_offset_unit-currentIndex,"KB/sec",outfile);
		  currentIndex=column_offset_unit+6;
	} else if (viewMode == VIEWMODE_TOTAL_MB) {
		  skip_write(column_offset_unit-currentIndex,"MB    ",outfile);
		  currentIndex=column_offset_unit+6;
	} else if (viewMode == VIEWMODE_TOTAL_KB) {
		  skip_write(column_offset_unit-currentIndex,"KB    ",outfile);
		  currentIndex=column_offset_unit+6;
	} else if (viewMode == VIEWMODE_TOTAL_B) {
		  skip_write(column_offset_unit-currentIndex,"B     ",outfile);
		  currentIndex=column_offset_unit+6;
	}
  outfile<<std::endl;
  outfile.close();

  ///end


  if (viewMode == VIEWMODE_KBPS) {
    mvaddstr(row, column_offset_unit, "KB/sec");
  } else if (viewMode == VIEWMODE_TOTAL_MB) {
    mvaddstr(row, column_offset_unit, "MB    ");
  } else if (viewMode == VIEWMODE_TOTAL_KB) {
    mvaddstr(row, column_offset_unit, "KB    ");
  } else if (viewMode == VIEWMODE_TOTAL_B) {
    mvaddstr(row, column_offset_unit, "B     ");
  }
}

void Line::log() {
  std::cout << m_name << '/' << m_pid << '/' << m_uid << "\t" << sent_value
            << "\t" << recv_value << std::endl;
}

int GreatestFirst(const void *ma, const void *mb) {
  Line **pa = (Line **)ma;
  Line **pb = (Line **)mb;
  Line *a = *pa;
  Line *b = *pb;
  double aValue;
  if (sortRecv) {
    aValue = a->recv_value;
  } else {
    aValue = a->sent_value;
  }

  double bValue;
  if (sortRecv) {
    bValue = b->recv_value;
  } else {
    bValue = b->sent_value;
  }

  if (aValue > bValue) {
    return -1;
  }
  if (aValue == bValue) {
    return 0;
  }
  return 1;
}

///modify here to add arguments
void init_ui() {

  WINDOW *screen = initscr();

  raw();
  noecho();
  cbreak();

  nodelay(screen, TRUE);
  caption = new std::string("NetHogs");
  caption->append(getVersion());

  // caption->append(", running at ");
}

void exit_ui() {
  clear();
  endwin();
  delete caption;
}

void ui_tick() {
  switch (getch()) {
  case 'q':
    /* quit */
    quit_cb(0);
    break;
  case 's':
    /* sort on 'sent' */
    sortRecv = false;
    break;
  case 'r':
    /* sort on 'received' */
    sortRecv = true;
    break;
  case 'm':
    /* switch mode: total vs kb/s */
    viewMode = (viewMode + 1) % VIEWMODE_COUNT;
    break;
  }
}

void show_trace(Line *lines[], int nproc) {
  std::cout << "\nRefreshing:\n";

  /* print them */
  for (int i = 0; i < nproc; i++) {
    lines[i]->log();
    delete lines[i];
  }

  /* print the 'unknown' connections, for debugging */
  ConnList *curr_unknownconn = unknowntcp->connections;
  while (curr_unknownconn != NULL) {
    std::cout << "Unknown connection: "
              << curr_unknownconn->getVal()->refpacket->gethashstring()
              << std::endl;

    curr_unknownconn = curr_unknownconn->getNext();
  }
}

void show_ncurses(Line *lines[], int nproc) {
  int rows=10;             // number of terminal rows///I modify it to 10
  int cols;             // number of terminal columns
  unsigned int proglen; // max length of the "PROGRAM" column

  double sent_global = 0;
  double recv_global = 0;

  getmaxyx(stdscr, rows, cols); /* find the boundaries of the screeen */
  ///seems no need for terminal display, I want to set the width of output file fixed
  cols=95;

  if (cols < 62) {
    erase();
    mvprintw(0, 0,
             "The terminal is too narrow! Please make it wider.\nI'll wait...");
    return;
  }

  if (cols > PROGNAME_WIDTH)
    cols = PROGNAME_WIDTH;

  proglen = cols - 67;
  /// start

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
  std::string dt(buffer);

   std::ofstream outfile (outFilePath.c_str(),std::ofstream::app);
   char* x = new char[150];

  if (flag)
  {
	  //outfile<< "Local time: "<<dt;
	  	sprintf(x,"TIME                   PID USER     %-*.*s  DEV        SENT      RECEIVED       ",
	               proglen, proglen, "PROGRAM");
		std::string str=x;
		outfile<<str<<std::endl;
		flag=false;
  }

  outfile.close();
  ///end

  erase();
  mvprintw(0, 0, "%s", caption->c_str());
  attron(A_REVERSE);
  mvprintw(2, 0,
           "    PID USER     %-*.*s  DEV        SENT      RECEIVED       ",
           proglen, proglen, "PROGRAM");
  attroff(A_REVERSE);
  /* print them */

  ///if there is no io for specified process, I also need to display 0 for it


  int i;
  for (i = 0; i < nproc; i++) {
    if (i + 3 < rows)///i want to set rows as fixed, say only top 10
      lines[i]->show(i + 3, proglen);///need to modify show function
    recv_global += lines[i]->recv_value;
    sent_global += lines[i]->sent_value;
    delete lines[i];
  }

  /*
  x=new char[150];
  sprintf(x,"  TOTAL        %-*.*s          %11.3f%11.3f ",
           proglen, proglen, " ", sent_global, recv_global);
  ofile<<x;
  if (viewMode == VIEWMODE_KBPS) {
  		  skip_write(0,"KB/sec",ofile);
  	} else if (viewMode == VIEWMODE_TOTAL_MB) {
  		  skip_write(0,"MB    ",ofile);
  	} else if (viewMode == VIEWMODE_TOTAL_KB) {
  		  skip_write(0,"KB    ",ofile);
  	} else if (viewMode == VIEWMODE_TOTAL_B) {
  		  skip_write(0,"B     ",ofile);
  	}*///this section is not needed now

  /*for (int i=0;i<cols;i++)
    	  ofile<<"#";
  ofile<<"\n\n\n";*/
  ///end

  attron(A_REVERSE);
  int totalrow = std::min(rows - 1, 3 + 1 + i);
  mvprintw(totalrow, 0, "  TOTAL        %-*.*s          %11.3f %11.3f ",
           proglen, proglen, " ", sent_global, recv_global);
  if (viewMode == VIEWMODE_KBPS) {
    mvprintw(3 + 1 + i, cols - COLUMN_WIDTH_UNIT, "KB/sec ");
  } else if (viewMode == VIEWMODE_TOTAL_B) {
    mvprintw(3 + 1 + i, cols - COLUMN_WIDTH_UNIT, "B      ");
  } else if (viewMode == VIEWMODE_TOTAL_KB) {
    mvprintw(3 + 1 + i, cols - COLUMN_WIDTH_UNIT, "KB     ");
  } else if (viewMode == VIEWMODE_TOTAL_MB) {
    mvprintw(3 + 1 + i, cols - COLUMN_WIDTH_UNIT, "MB     ");
  }
  attroff(A_REVERSE);
  mvprintw(totalrow + 1, 0, "");
  refresh();
}


// Display all processes and relevant network traffic using show function
void do_refresh() {
  refreshconninode();
  refreshcount++;

  if (viewMode == VIEWMODE_KBPS) {
    remove_timed_out_processes();
  }

  ProcList *curproc = processes;
  int nproc = processes->size();

  /* initialize to null pointers */
  Line *lines[nproc];
  for (int i = 0; i < nproc; i++)
    lines[i] = NULL;

  int n = 0;

  while (curproc != NULL) {
    // walk though its connections, summing up their data, and
    // throwing away connections that haven't received a package
    // in the last CONNTIMEOUT seconds.
	 ///start

	  /*
	  if (tracingPid!=0){
		if (curproc->getVal()->pid!=(pid_t)tracingPid){
			 curproc = curproc->next;
			continue;
		}
	}*/
	///end

    assert(curproc->getVal() != NULL);
    assert(nproc == processes->size());

    float value_sent = 0, value_recv = 0;

    if (viewMode == VIEWMODE_KBPS) {
      curproc->getVal()->getkbps(&value_recv, &value_sent);
    } else if (viewMode == VIEWMODE_TOTAL_KB) {
      curproc->getVal()->gettotalkb(&value_recv, &value_sent);
    } else if (viewMode == VIEWMODE_TOTAL_MB) {
      curproc->getVal()->gettotalmb(&value_recv, &value_sent);
    } else if (viewMode == VIEWMODE_TOTAL_B) {
      curproc->getVal()->gettotalb(&value_recv, &value_sent);
    } else {
      forceExit(false, "Invalid viewMode: %d", viewMode);
    }
    uid_t uid = curproc->getVal()->getUid();
    assert(curproc->getVal()->pid >= 0);
    assert(n < nproc);

    lines[n] =
      new Line(curproc->getVal()->name, value_recv, value_sent,
               curproc->getVal()->pid, uid, curproc->getVal()->devicename);
    n++;

    curproc = curproc->next;
  }
  /* sort the accumulated lines *////modify size
  qsort(lines, n, sizeof(Line *), GreatestFirst);

  if (tracingPid!=0 && n==0){
	  lines[n]=new Line("This process has no IO now",0.0,0.0,(pid_t)tracingPid,-1,"");
	  n++;
  }

  if (tracemode || DEBUG)
    show_trace(lines, nproc);
  else///here modify parameters to n
    show_ncurses(lines, n);

  if (refreshlimit != 0 && refreshcount >= refreshlimit)
    quit_cb(0);
}
