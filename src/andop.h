#ifndef ANDOP_H
#define ANDOP_H
#include "connector.h"

class AndOp:public connector
{
public:
    AndOp();
    AndOp(Base* left, Base* right):connector(left,right){}
    bool execute();
    /*
     * execute summary:
     * execute left child
     * check left child passed
     * if yes -> execute right child
     * if no -> nothing
     */
    ~AndOp();

};

#endif // ANDOP_H

