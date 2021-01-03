#include <iostream>
#include <stack>
#include <cstdlib>
#include <iomanip>

#define IndexError 9999

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::stack;
using std::setw;
using std::setiosflags;

stack<string> st;
string keys[9] = {"id", "+", "*", "(", ")", "$", "E", "T", "F"};

string values[9][12] = {{"s5","","","","s5","","s5","s5","","","",""}, 
                        {"","s6","r2","r4","","r6","","","s6","r1","r3","r5"},
                        {"","","s7","r4","","r6","","","","s7","r3","r5"},
                        {"s4","","","","s4","","s4","s4","","","","",},
                        {"","","r2","r4","","r6","","","s11","r1","r3","r5"},
                        {"","acc","r2","r4","","r6","","","","r1","r3","r5"},
                        {"1","","","","8","","","","","","",""},
                        {"2","","","","2","","9","","","","","",},
                        {"3","","","","3","","3","10","","","",""}};

string gram[7] = {"S->E", "E->E+T", "E->T", "T->T*F", "T->F", "F->(E)", "F->id"};
string sub;             // 待处理
int position = 0;           // 分析到的位置

void init() {
    // value[11][9] = {{"s4", "", "", "s5", "", "", "1", "8", "10"},
    //                             {"", "s2", "", "", "", "acc", "", "", ""},
    //                             {"", "", "", "", "", "", "", "8", "3"},
    //                             {"", "r4", "r4", "", "r4", "r4", "", "", ""},
    //                             {"", "r6", "r6", "", "r6", "r6", "", "", ""},
    //                             {"s4", "", "", "s5", "", "", "6", "8", "10"},
    //                             {"", "s2", "", "", "s7", "", "", "", ""},
    //                             {"", "r5", "r5", "", "r5", "r5", "", "", ""},
    //                             {"", "r2", "s9", "", "r2", "r2", "", "", ""},
    //                             {"s4", "", "", "s5", "", "", "", "", "10"},
    //                             {"", "r3", "r3", "", "r3", "r3", "", "", ""}};
    

}

void output() {
    for (int i = 0; i < 9; i++) {
        cout << keys[i] << ": ";
        for (int j = 0; j < 11; j++) {
            cout << values[i][j] << "\t";
        }
        cout << endl;
    }
    for (int i = 0; i < 7; i++) {
        cout << gram[i] << endl;
    }
}

string nextToken() {
    if (sub.substr(0, 2) == "id") {
        sub = sub.substr(2);
        return "id";
    } else {
        string s = sub.substr(0, 1);
        sub = sub.substr(1);
        return s;
    }
}

string showNextToken() {
    if (sub.substr(0, 2) == "id") {
        return "id";
    } else {
        string s = sub.substr(0, 1);
        return s;
    }
}

void getInput() {
    cout << "input the sub: ";
    cin >> sub;
    sub += "$";
    cout << "------------------------" << endl;
}

string getTop() {
    if (!st.empty()) {
        string token = st.top();
        st.pop();
        return token;
    } else {
        return "";
    }   
}

void showStack() {
    string str;
    stack<string> s = st;
    while(!s.empty()) {
        str = s.top() + str;
        s.pop();
    }
    cout << setiosflags(std::ios::left) << setw(15) << str;
}

void showSub() {
    cout << setiosflags(std::ios::right) << setw(15) << sub;
}

void showAcc() {
    showStack();
    showSub();
    cout << setiosflags(std::ios::left) << setw(15) << "accept" << endl;
}

int getRow(string token) {
    for (int i = 0; i < 9; i++) {
        if (keys[i] == token) {
            return i;
        }
    }
    return IndexError;
}

void showS(string idx) {
    showStack();
    showSub();
    cout << setiosflags(std::ios::right) << setw(15) << "移进 " << endl;
}

void showR(string grama) {
    showStack();
    showSub();
    cout << setiosflags(std::ios::left) << setw(15) << "按 " << grama << " 归约" << endl; 
}

int rightLength(string s) {
    int index = s.find("->");
    string ss = s.substr(index + 2);
    if (ss == "id") {
        return 1;
    } else {
        return ss.length();
    }
}

int handle() {
    string token = getTop();
    string idx = getTop();

    st.push(idx);
    st.push(token);

    int index = atoi(idx.c_str());
    int row = getRow(token);
    if (row == IndexError) {
        cerr << "Error at: " << position << endl;
    }
    string action = values[row][index];
    if (action == "acc") {
        showAcc();
        return 1;
    } else if (action[0] == 's') {
        st.push(action.substr(1));
        showS(action.substr(1));
        st.push(nextToken());
        
        position++;
    } else if (action[0] == 'r') {
        sub = token + sub;  // * 放入sub中，再一次判断
        string rule = action.substr(1);
        string grama = gram[atoi(rule.c_str())];

        int length = rightLength(grama);
        
        for (int i = 0; i <= length * 2; i++) {
            st.pop();
        }
        row = atoi(st.top().c_str());
        st.push(grama.substr(0, 1));
        index = getRow(grama.substr(0, 1));
        st.push(values[index][row]);
    
        showR(grama);
        st.push(nextToken());
        
    } else {
        cerr << "Error at: " << position << endl;
        exit(-1);
    }

    return 0;
}

void showTitle() {
    cout << setiosflags(std::ios::left) << setw(15) << "stack";
    cout << setiosflags(std::ios::right) << setw(15) << "sub";
    cout << setiosflags(std::ios::left) << setw(15) << "do" << endl;
}

int main() {
    //getInput();
    sub = "(id*id)+id$";
    st.push("0");
    st.push(nextToken());
    showTitle();
    while (!handle()) {
        //showNextToken();
    }
    return 0;
}