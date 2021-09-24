/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2021.
*/
#include <iostream>
using namespace std;
#include "scan.h"

const char *names[] = {"read", "write", "id", "literal", "gets", "add",
                       "sub", "mul", "div", "lparen", "rparen", "eof", "if", "fi", "do", "od", "check", "==", "<>", ">", "<", ">=", "<="};

static token upcoming_token;

void error()
{
    cerr << "Got your self a syntax Error! \n";
    exit(1);
}

static string answer;

// Given a current token. We try and consume it. If it's Not a valid token in names, then we have to throw an error.
void match(token expected)
{
    if (upcoming_token == expected)
    {
        cout << "matched " << names[upcoming_token];
        if (upcoming_token == t_id || upcoming_token == t_literal)
            cout << ": " << token_image;
        cout << "\n";
        upcoming_token = scan();
    }
    else
    {
        error();
    }
}

void program();
string stmt_list();
string stmt();
string expr();
string term();
string term_tail();
string factor();
string factor_tail();
string add_op();
string mul_op();
string relation_op();
string expr_tail();
string relation();

string relation()
{
    switch (upcoming_token)
    {
    case t_lparen:
    case t_id:
    case t_literal:
        cout << "Relation -> E Et\n";
        expr();
        expr_tail();
        break;
    default:
        error();
    }
}

string expr_tail()
{
    switch (upcoming_token)
    {

    case t_equals:
    case t_carrot:
    case t_greater:
    case t_less:
    case t_lessE:
    case t_greaterE:
        cout << "predict  expr tail --> ro E\n";
        relation_op();
        expr();
        break;
    case t_id:
    case t_rparen:
    case t_read:
    case t_write:
    case t_if:
    case t_fi:
    case t_do:
    case t_od:
    case t_check:
    case t_eof:
    default:
        error();
    }
}

void program()
{
    //  cout << upcoming_token;
    switch (upcoming_token)
    {
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
    case t_if:
    case t_do:
    case t_check:
       // cout << "predict program --> stmt_list eof\n";
       answer = "( [ program( "+ stmt_list() + " ] )";
        //stmt_list();
        match(t_eof);
        break;
    default:
        error();
    }
}

string stmt_list()
{
    string stmt_list_string = "";
    switch (upcoming_token)
    {
    case t_id:
    case t_read:
    case t_write:
    case t_check:
    case t_if:
    case t_do:
            stmt_list_string = "( [ ( " + stmt() + ") " + "( " + stmt_list() + ")";
        return stmt_list_string;
        break;
    case t_eof:
    case t_fi:
    case t_od:
        cout << "predict stmt_list --> epsilon\n";
        break; /* epsilon production */
    default:
        error();
    }
}

string stmt()
{
    string stmt_string = "";
    // cout << upcoming_token;
    switch (upcoming_token)
    {
    case t_id:
        //cout << "predict stmt --> id gets Relation\n";
        match(t_id);
        match(t_gets);
        stmt_string = ":= " + t_id + expr();
        //expr();
        return stmt_string;
        break;
    case t_read:
        //cout << "predict stmt --> read id\n";
        match(t_read);
        match(t_id);
        stmt_string = "read" + t_id;
        return stmt_string;
        break;
    case t_write:
        //cout << "predict stmt --> write relation\n";
        match(t_write);
        stmt_string = "write " + expr(); 
        return stmt_string;
        break;
    case t_if:
        //cout << "predict stmt --> if relation SL if\n";
        match(t_if);
        relation();
        stmt_list();
        match(t_fi);
        return stmt_string;
        break;
    case t_do:
        cout << "predict stmt --> do SL od\n";
        match(t_do);
        stmt_list();
        match(t_od);
        return stmt_string;
        break;
    case t_check:
        cout << "predict stmt --> check relation \n";
        match(t_check);
        relation();
        return stmt_string;
        break;

    default:
        error();
    }
}

string expr()
{
    switch (upcoming_token)
    {
    case t_id:
    case t_literal:
    case t_lparen:
        cout << "predict expr --> term term_tail\n";
        term();
        term_tail();
        break;
    default:
        error();
    }
}

string term()
{
    switch (upcoming_token)
    {
    case t_id:
    case t_literal:
    case t_lparen:
        cout << "predict term --> factor factor_tail\n";
        factor();
        factor_tail();
        break;
    default:
        error();
    }
}

string term_tail()
{
    switch (upcoming_token)
    {
    case t_add:
    case t_sub:
        cout << "predict term_tail --> add_op term term_tail\n";
        add_op();
        term();
        term_tail();
        break;
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
    case t_if:
    case t_do:
    case t_check:
    case t_fi:
    case t_od:
    case t_equals:
    case t_greaterE:
    case t_carrot:
    case t_lessE:
    case t_less:
    case t_greater:
        cout << "predict term_tail --> epsilon\n";
        break; /* epsilon production */
    default:
        error();
    }
}

string factor()
{
    switch (upcoming_token)
    {
    case t_literal:
        cout << "predict factor --> literal\n";
        match(t_literal);
        break;
    case t_id:
        cout << "predict factor --> id\n";
        match(t_id);
        break;
    case t_lparen:
        cout << "predict factor --> lparen expr rparen\n";
        match(t_lparen);
        expr();
        match(t_rparen);
        break;
    default:
        error();
    }
}

string factor_tail()
{
    switch (upcoming_token)
    {
    case t_mul:
    case t_div:
        cout << "predict factor_tail --> mul_op factor factor_tail\n";
        mul_op();
        factor();
        factor_tail();
        break;
    case t_add:
    case t_sub:
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_greaterE:
    case t_greater:
    case t_lessE:
    case t_less:
    case t_carrot:
    case t_equals:
    case t_if:
    case t_fi:
    case t_do:
    case t_od:
    case t_check:
    case t_eof:
        cout << "predict factor_tail --> epsilon\n";
        break; /* epsilon production */
    default:
        error();
    }
}

string add_op()
{
    switch (upcoming_token)
    {
    case t_add:
        cout << "predict add_op --> add\n";
        match(t_add);
        break;
    case t_sub:
        cout << "predict add_op --> sub\n";
        match(t_sub);
        break;
    default:
        error();
    }
}

string mul_op()
{
    switch (upcoming_token)
    {
    case t_mul:
        cout << "predict mul_op --> mul\n";
        match(t_mul);
        break;
    case t_div:
        cout << "predict mul_op --> div\n";
        match(t_div);
        break;
    default:
        error();
    }
}
string relation_op()
{
    switch (upcoming_token)
    {
    case t_equals:
        cout << "predict rop -- > eqauls\n";
        match(t_equals);
        break;
    case t_carrot:
        cout << "predict rop --> <>\n";
        match(t_carrot);
        break;
    case t_greater:
        cout << "predict rop --> >\n";
        match(t_greater);
        break;
    case t_less:
        cout << "predict rop --> <\n";
        match(t_less);
        break;
    case t_lessE:
        cout << "predict rop --> <=\n";
        match(t_lessE);
        break;
    case t_greaterE:
        cout << "predict rop --> >=\n";
        match(t_greaterE);
        break;
    default:
        error();
    }
}

int main()
{   answer = "";
    upcoming_token = scan();
    program();
    cout << answer; 
    
       return 0;
}
