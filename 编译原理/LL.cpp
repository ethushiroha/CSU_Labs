#include <iostream>
#include <cstdlib>
#include <stack>
#include <iomanip>

#define TitleWeight 18

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::setiosflags;
using std::ios;
using std::string;
using std::stack;

stack<char> st;
char col[6] = {'i', '+', '*', '(', ')', '$'};
char row[5] = {'E', 'A', 'T', 'B', 'F'};
string value[5][6] = {{"E->TA", "", "", "E->TA", "", ""},
                    {"", "A->+TA", "", "", "A->e", "A->e"},
                    {"T->FB", "", "", "T->FB", "", ""},
                    {"", "B->e", "B->*FB", "", "B->e", "B->e"},
                    {"F->i", "", "", "F->(E)", "", ""}};
string gram[] = {"E->TA", "A->+TE", "A->e", "T->FB", "B->*FB", "B->e", "F->(E)", "F->i"};
int position = 0;
string buf;

void coutFlag(int flag, string msg) {
    cout << setiosflags(flag) << setw(TitleWeight) << msg;
}

void init() {
    buf = "(i*i)+ic$";
    st.push('$');
    st.push('E');
    setvbuf(stdin, 0 ,2 ,0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
}

void indexError() {
    cerr << "Error at: " << position << endl;
}

char getTop() {
    if (st.empty()) {
        indexError();
        exit(-1);
    } else {
        char c = st.top();
        return c;
    }
}

void getInput() {
    cout << "Input: ";
    cin >> buf;
    buf += "$";
}

char nextToken() {
    char c = buf[0];
    buf = buf.substr(1);
    return c;
}

int getCol(char c) {
    int index = 0;
    for (int i = 0; i < 6; i++) {
        if (col[i] == c) {
            return i;
        }
    } 
    indexError();
    exit(-1);
}

int getRow(char c) {
    int index = 0;
    for (int i = 0; i < 5; i++) {
        if (row[i] == c) {
            return i;
        }
    }
    indexError();
    exit(-1);
}

void showStack() {
    string s;
    stack<char> sta = st;
    while (!sta.empty()) {
        s += sta.top();
        sta.pop();
    }
    coutFlag(ios::right, s); 
}

void showSub() {
    coutFlag(ios::right, buf);
}

void match(string s) {
    st.pop();
    showStack();
    showSub();
    coutFlag(ios::left, s);
    cout << endl;
}

void outPut(string s) {
    showStack();
    showSub();
    coutFlag(ios::left, "输出 " + s);
    cout << endl;
}

bool handle() {
    char token = nextToken();
    char top = getTop();
    if (token == top) {
        position++;
        if (st.top() == '$') {
            match("接受");
            return true;
        }
        match("匹配");
    } else {
        buf = token + buf;
        int col = getCol(token);
        int row = getRow(top);
        string grama = value[row][col];
        if(grama == "") {
            indexError();
            exit(-1);
        }
        st.pop();
        for (int i = grama.length() - 1; i >= 3; i--) {
            if (grama[i] != 'e') {
                st.push(grama[i]);
            }
        }
        outPut(grama);
        
        
    }
    return false;
}

void showTitle() {
    coutFlag(ios::right, "stack");
    coutFlag(ios::left, "sub");
    coutFlag(ios::left, "do");
    cout << endl;
}

int main() {
    init();
    showTitle();
    int i = 0;
    while(!handle());


    return 0;
}
