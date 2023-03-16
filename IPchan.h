#pragma once
#include <vector>
#include <map>
#include <string> 
#include "lib/sqlite/sqlite3.h"
#include "lib/mongoose/mongoose.h"

#define object_reference struct
//structs are here for db schema reference purposes, NOT for use in development
//there's no need to keep the data in memory, really. maybe as a cache if
//read perf becomes a real issue? problem for later, anyway.
//marshalling shit into objects is messy and i'm not doing it

#define str std::string

typedef std::map<std::string, std::string> row;
typedef std::vector<row> rows;

#define ip2ipid(ip) *((unsigned int*)ip)
#define ipid2ip(ipid) *((ipv4*)(&(ipid)));

struct ipv4
{
    unsigned char adr[4];
};

object_reference post
{
    int bid;     // i'm board
    int tid;    // thread the post is in
    int no;    // post number in thread
    int tim;  // stamp
    str nam; // fag
    str bod;// sjis art ^_^
};         // alternately, ``oh anon-dono, you have such a rocking bod~~~"
object_reference thread
{
    int bid;         // board the thread is in
    int tid;        // thread id on-board
    str subject;   // irrelevant time-wasting question
    int replies;  // shitpost-y buzzword-laden crossboarding replies
    int lastpost;// we gotta have posts NOW like JUST NOW like A SECOND AGO go GO GO

    static std::string make_thread_fe(sqlite3* db, row& r);
    static void add_post(sqlite3* db, mg_str& querystring);
    static bool comparator(const row &l, const row &r);
};
object_reference board
{
    int ipid; // == bid
    int threadcount;

    std::string make_threadlist_fe(sqlite3* db);
    std::string make_board_fe(sqlite3* db);
    static void add_thread(sqlite3* db, mg_str& querystring);
};

void db_schema_init(sqlite3* db);