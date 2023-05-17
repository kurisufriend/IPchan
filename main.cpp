#include "lib/mongoose/mongoose.h"
#include "lib/sqlite/sqlite3.h"
#include "lib/dumbstr/dumbstr.h"
#include "lib/json.hpp"
#include "lib/captcha/botwall.h"
#include "IPchan.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <any>
#include <string>
#include <vector>
#include <fstream>

#define XCLACKSOVERHEAD "X-Clacks-Overhead: GNU Terry Pratchett, GNU Aaron Swartz, GNU Hal Finney, GNU Norm Macdonald, GNU Gilbert Gottfried, GNU Aniki, GNU Terry Davis, GNU jstark, GNU John McAfee, GNU asshurtmacfags\n"

typedef mg_connection connection;
typedef mg_http_message message;

sqlite3* db;
nlohmann::json cfg;


void callback(connection* c, int ev, void* ev_data, void* fn_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        std::string headers = XCLACKSOVERHEAD;
        message* msg = (message*)ev_data;
        ipv4 ip = *((ipv4*)(&(c->rem.ip)));
        unsigned int ipid = ip2ipid(&ip);
        std::string ipv4 = dumbfmt({
            std::to_string(ip.adr[0]),
            ".", std::to_string(ip.adr[1]),
            ".", std::to_string(ip.adr[2]),
            ".", std::to_string(ip.adr[3])
        });
        if(mg_http_match_uri(msg, "/"))
        {
            headers.append("Content-Type: text/html;charset=shift_jis\n");
            board b = {.ipid = (int)ipid, .threadcount = 1};

            std::pair<std::string, std::string>  // token, b64'd chllenge
            captcha= botwall::generate_captcha(ipid, cfg["captcha_secret"]);

            std::string body = dumbfmt_file("./static/index.html",
                {
                    {"ipid", std::to_string(ipid)},
                    {"ipv4", ipv4},
                    {"threads", b.make_board_fe(db, captcha.second, captcha.first)},
                    {"threadtitles", b.make_threadlist_fe(db)},
                    {"challenge", captcha.second},
                    {"captcha_token", captcha.first}
                }
            );
            mg_http_reply(c, 200, headers.c_str(),
                body.c_str());
        }
        else if (mg_http_match_uri(msg, "/post"))
        {
            std::string res = thread::add_post(db, msg->body, cfg["captcha_secret"]);
            headers.append("Location: /\n");
            mg_http_reply(c, 303, headers.c_str(), 
                res.c_str());
        }        
        else if (mg_http_match_uri(msg, "/post-thread"))
        {
            std::string res = board::add_thread(db, msg->body, cfg["captcha_secret"]);
            headers.append("Location: /\n");
            mg_http_reply(c, 303, headers.c_str(), 
                res.c_str());
        }
        else if (mg_http_match_uri(msg, "/CAPTCHA.bmp"))
        {
            mg_http_serve_opts o = {.mime_types="image/bmp"};
            mg_http_serve_file(c, msg, "./static/imgs/aids.bmp", &o);
        }
        else 
        {
            mg_http_reply(c, 404, headers.c_str(), 
                "the name's huwer, as in who are the fuck is you?");
        }
    }

}

int main(int argc, char* argv[])
{
    std::srand(time(0));
    mg_mgr mongoose;
    mg_mgr_init(&mongoose);

    bool inited = true;
    if (access("./ipchan.db", F_OK) == -1)
        inited = false;

    if (sqlite3_open("./ipchan.db", &db))
    {
        std::cout << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }
    if (!inited)
        db_schema_init(db);
    
    std::fstream f;
    f.open("./config.json");
    cfg = nlohmann::json::parse(f);
    f.close();

    mg_http_listen(&mongoose, cfg["host"].get<std::string>().c_str(), callback, &mongoose);
    while (true) {mg_mgr_poll(&mongoose, 1000);}
    sqlite3_close(db);

    return 0;
}