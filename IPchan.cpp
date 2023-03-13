#include "IPchan.h"
#include "lib/dumbstr/dumbstr.h"
#include "lib/sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "lib/mongoose/mongoose.h"

const char* make_posts = "create table posts("
"tid INTEGER,"
"bid INTEGER,"
"no INTEGER,"
"tim INTEGER,"
"nam TEXT,"
"bod TEXT"
");";

const char* make_threads = "create table threads("
"bid INTEGER,"
"tid INTEGER,"
"subject TEXT,"
"replies INTEGER,"
"lastpost INTEGER"
");";

const char* make_boards = "create table boards("
"ipid INTEGER,"
"threadcount INTEGER"
");";

int sqlcb(void* a, int argc, char** argv, char** colname)
{
    rows* rs = (rows*) a;
    row r;
    int i;
    for(i=0; i<argc; i++)
    {
        r.emplace(colname[i], argv[i] ? argv[i] : "NULL");
        //printf("%s = %s\n", colname[i], argv[i] ? argv[i] : "NULL");
    }
    //printf("\n");
    rs->push_back(r);
    return 0;
}

char* sql_err;

void db_schema_init(sqlite3* db)
{
    rows res;
    std::cout << make_threads << make_posts << std::endl;
    sqlite3_exec(db, make_threads, sqlcb, &res, &sql_err);
    sqlite3_exec(db, make_posts, sqlcb, &res, &sql_err);
}

std::string thread::make_thread_fe(sqlite3* db, row& r)
{
    std::string acc;

    rows posts;

    std::string query = dumbfmt(
        {
        "select * from posts where bid=",r["bid"],
        " and tid=",r["tid"],";"
        }
    );
    sqlite3_exec(db, query.c_str(), sqlcb, &posts, &sql_err);

    for (auto iter = posts.begin(); iter != posts.end(); iter++)
    {
        if (iter == posts.begin() + 1 && posts.size() > 2)
            acc.append("<details><summary>Unroll Thread...</summary>");
        else if (iter == posts.end() - 1 && posts.size() > 2)
            acc.append("</details>");
        size_t nchars;
        time_t ti = (time_t)stoi((*iter)["tim"], &nchars);
        acc.append(
            dumbfmt_file("./static/template/reply.html", {
                {"no", (*iter)["no"]},
                {"name", dumbfmt_html_escape((*iter)["nam"])},
                {"trip", dumbfmt_html_escape("")},
                {"datetime", ctime(&ti)},
                {"body", dumbfmt_html_escape((*iter)["bod"])}
            })
        );
    }
    return acc;
}

std::string board::make_threadlist_fe(sqlite3* db)
{
    std::string acc;
    rows threads;

    std::string query = dumbfmt(
        {
        "select * from threads where bid=", std::to_string(this->ipid), ";"
        }
    );

    sqlite3_exec(db, query.c_str(), sqlcb, &threads, &sql_err);
    for (auto iter = threads.begin(); iter != threads.end(); iter++)
    {
        acc.append(
            dumbfmt({
                "<a href=\"#", (*iter)["tid"], "\">",
                (*iter)["subject"], " (" ,(*iter)["replies"], ")", "</a> "
            })
        );
    }
    return acc;
}

struct thread_comp
{
    bool operator()(const row &l, const row &r)
    {
        size_t nchars;
        int left = std::stoi(l.at("lastpost"), &nchars);
        int right = std::stoi(r.at("lastpost"), &nchars);
        return left > right; 
    }
};

std::string board::make_board_fe(sqlite3* db)
{
    std::string acc;
    rows threads;

    std::string query = dumbfmt(
        {
        "select * from threads where bid=", std::to_string(this->ipid), ";"
        }
    );

    sqlite3_exec(db, query.c_str(), sqlcb, &threads, &sql_err);
    
    std::sort(threads.begin(), threads.end(), thread_comp());

    for (auto iter = threads.begin(); iter != threads.end(); iter++)
    {
        acc.append(
            dumbfmt_file("./static/template/thread.html", {
                {"subject", (*iter)["subject"]},
                {"replycount", (*iter)["replies"]},
                {"replies", thread::make_thread_fe(db, (*iter))},
                {"bid", (*iter)["bid"]},
                {"tid", (*iter)["tid"]}
            })
        );
    }
    return acc;
}

#define buffetchhttpvar(key) char buffer_##key[10000]; \
mg_http_get_var(&querystring, #key, buffer_##key, sizeof(buffer_##key)); \
const char* key = buffer_##key;

void thread::add_post(sqlite3* db, mg_str& querystring)
{
    rows res;

    buffetchhttpvar(tid);
    buffetchhttpvar(bid);

    time_t tim = time(0);

    rows threads;
    std::string quer = dumbfmt(
        {
        "select * from threads where bid=", bid, " and tid=", tid, ";"
        }
    );
    sqlite3_exec(db, quer.c_str(), sqlcb, &threads, &sql_err);
    const char* replies = threads.at(0)["replies"].c_str();
    size_t nchars;
    int replycount = std::stoi(replies, &nchars) + 1;

    quer = dumbfmt(
        {
        "update threads set replies=", std::to_string(replycount),
        " where bid=", bid, " and tid=", tid, ";"
        }
    );
    sqlite3_exec(db, quer.c_str(), sqlcb, &threads, &sql_err);

    quer = dumbfmt(
    {
    "update threads set lastpost=", std::to_string(tim),
    " where bid=", bid, " and tid=", tid, ";"
    }
    );
    sqlite3_exec(db, quer.c_str(), sqlcb, &threads, &sql_err);
    

    buffetchhttpvar(postname);
    buffetchhttpvar(postbody);
    std::string query = dumbfmt(
        {
        "insert into posts values(",
        tid, ",",
        bid, ",",
        std::to_string(replycount), ",",
        std::to_string(tim), ",\"",
        postname, "\",\"",
        postbody, "\");"
        }
    );
    sqlite3_exec(db, query.c_str(), sqlcb, &res, &sql_err);
}

void board::add_thread(sqlite3* db, mg_str& querystring)
{
    rows res;

    buffetchhttpvar(bid);

    time_t tim = time(0);

    rows threads;
    std::string quer = dumbfmt(
        {
        "select * from threads where bid=", bid, ";"
        }
    );
    sqlite3_exec(db, quer.c_str(), sqlcb, &threads, &sql_err);
    int tid = threads.size();

    buffetchhttpvar(postname);
    buffetchhttpvar(postsub);
    buffetchhttpvar(postbody);
    std::string query = dumbfmt(
        {
        "insert into threads values(",
        bid, ",",
        std::to_string(tid), ",\"",
        postsub, "\",",
        "1,",
        std::to_string(tim), ");"
        }
    );
    sqlite3_exec(db, query.c_str(), sqlcb, &res, &sql_err);

    query = dumbfmt(
        {
        "insert into posts values(",
        std::to_string(tid), ",",
        bid, ",",
        "1", ",",
        std::to_string(tim), ",\"",
        postname, "\",\"",
        postbody, "\");"
        }
    );
    sqlite3_exec(db, query.c_str(), sqlcb, &res, &sql_err);
}