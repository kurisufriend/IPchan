#include "sqleasy.h"
#include "../dumbstr/dumbstr.h"
#include <iostream>

int sqleasy_q::cb(void* a, int argc, char** argv, char** colname)
{
    rows* rs = (rows*)a;
    row r;
    for(int i = 0; i < argc; i++)
        r.emplace(colname[i], argv[i] ? argv[i] : "NULL");
    rs->push_back(r);
    return 0;
}


rows sqleasy_q::exec()
{
    rows res;
    sqlite3_exec(this->db, this->query.c_str(), sqleasy_q::cb, &res, &this->err);
    return res;
}

void sqleasy_q::rexec(rows* res)
{
    sqlite3_exec(this->db, this->query.c_str(), sqleasy_q::cb, res, &this->err);
}

std::string sqlesc(std::string s)
{
    std::string res;
    for (auto c = s.begin(); c != s.end(); c++)
    {
        if (*c == '"')
            res.append("\"\"");
        else if (*c == '\'')
            res.append("\'\'");
        else if (*c == '\\')
            res.append("\\\\");
        else
            res.push_back(*c);
    }
    std::cout << res << std::endl;
    return res;
}