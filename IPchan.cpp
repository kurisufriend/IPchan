#include "IPchan.h"
#include "lib/dumbstr/dumbstr.h"
#include "lib/captcha/botwall.h"
#include "lib/sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "lib/mongoose/mongoose.h"
#include "lib/sqleasy/sqleasy.h"

char* sql_err;
size_t stoi_nchars;

void db_schema_init(sqlite3* db)
{
    rows res;

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

    sqlite3_exec(db, make_threads, sqleasy_q::cb, &res, &sql_err);
    sqlite3_exec(db, make_posts, sqleasy_q::cb, &res, &sql_err);
}

std::string thread::make_thread_fe(sqlite3* db, row& r)
{
    std::string acc;

    rows posts;

    sqleasy_q{db, 
        dumbfmt({"select * from posts where bid=",r["bid"]," and tid=",r["tid"],";"})
    }.rexec(&posts);

    for (auto iter = posts.begin(); iter != posts.end(); iter++)
    {
        if (iter == posts.begin() + 1 && posts.size() > 2)
            acc.append("<details><summary>Unroll Thread...</summary>");
        else if (iter == posts.end() - 1 && posts.size() > 2)
            acc.append("</details>");
        time_t ti = (time_t)stoi((*iter)["tim"], &stoi_nchars);
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

bool thread::comparator(const row &l, const row &r)
{
    int left = std::stoi(l.at("lastpost"), &stoi_nchars);
    int right = std::stoi(r.at("lastpost"), &stoi_nchars);
    return left > right; 
}

std::string board::make_threadlist_fe(sqlite3* db)
{
    std::string acc;
    rows threads;

    sqleasy_q{db,
        dumbfmt({"select * from threads where bid=",std::to_string(this->ipid), ";"})
    }.rexec(&threads);
    std::sort(threads.begin(), threads.end(), thread::comparator);
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

std::string board::make_board_fe(sqlite3* db, std::string captcha, std::string capt_token)
{
    std::string acc;
    rows threads;

    sqleasy_q{db,
        dumbfmt({"select * from threads where bid=", std::to_string(this->ipid), ";"})
    }.rexec(&threads);
    
    std::sort(threads.begin(), threads.end(), thread::comparator);

    for (auto iter = threads.begin(); iter != threads.end(); iter++)
    {
        acc.append(
            dumbfmt_file("./static/template/thread.html", {
                {"subject", (*iter)["subject"]},
                {"replycount", (*iter)["replies"]},
                {"replies", thread::make_thread_fe(db, (*iter))},
                {"bid", (*iter)["bid"]},
                {"tid", (*iter)["tid"]},
                {"captcha_token", capt_token},
                {"challenge", captcha}
            })
        );
    }
    return acc;
}

#define buffetchhttpvar(key) char buffer_##key[10000]; \
mg_http_get_var(&querystring, #key, buffer_##key, sizeof(buffer_##key)); \
const char* key = buffer_##key;

std::string thread::add_post(sqlite3* db, mg_str& querystring, std::string secret)
{
    buffetchhttpvar(token);
    buffetchhttpvar(guess);
    buffetchhttpvar(bid);

    if (!botwall::check_captcha(bid, guess, token, secret))
        return "ur shits fucked mayne, try that captcha again";

    rows res;

    buffetchhttpvar(tid);

    time_t tim = time(0);

    rows threads;
    sqleasy_q{db,
        dumbfmt({"select * from threads where bid=",bid," and tid=",tid,";"})
    }.rexec(&threads);
    
    
    const char* replies = threads.at(0)["replies"].c_str();
    int replycount = std::stoi(replies, &stoi_nchars) + 1;

    sqleasy_q{db,
        dumbfmt({"update threads set replies=",std::to_string(replycount)," where bid=",bid," and tid=",tid,";"})
    }.rexec(&threads);

    sqleasy_q{db,
        dumbfmt({"update threads set lastpost=",std::to_string(tim)," where bid=",bid," and tid=",tid,";"})
    }.rexec(&threads);

    buffetchhttpvar(postname);
    buffetchhttpvar(postbody);
    sqleasy_q{db,
        dumbfmt(
            {
            "insert into posts values(",
            tid, ",",
            bid, ",",
            std::to_string(replycount), ",",
            std::to_string(tim), ",\"",
            postname, "\",\"",
            postbody, "\");"
            }
        )
    }.rexec(&res);
    return "done (hopefully.......)";
}

std::string board::add_thread(sqlite3* db, mg_str& querystring, std::string secret)
{
    buffetchhttpvar(token);
    buffetchhttpvar(guess);
    buffetchhttpvar(bid);

    if (!botwall::check_captcha(bid, guess, token, secret))
        return "ur shits fucked mayne, try that captcha again";
    
    rows res;


    time_t tim = time(0);

    rows threads;
    sqleasy_q{db,
        dumbfmt({"select * from threads where bid=", bid, ";"})
    }.rexec(&threads);
    int tid = threads.size();

    buffetchhttpvar(postname);
    buffetchhttpvar(postsub);
    buffetchhttpvar(postbody);
    sqleasy_q{db, 
        dumbfmt(
            {
            "insert into threads values(",
            bid, ",",
            std::to_string(tid), ",\"",
            postsub, "\",",
            "1,",
            std::to_string(tim), ");"
            }
        )
    }.rexec(&res);

    sqleasy_q{db, 
        dumbfmt(
            {
            "insert into posts values(",
            std::to_string(tid), ",",
            bid, ",",
            "1", ",",
            std::to_string(tim), ",\"",
            postname, "\",\"",
            postbody, "\");"
            }
        )
    }.rexec(&res);
    return "done (hopefully.......)";
}