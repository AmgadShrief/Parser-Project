#include <bits/stdc++.h>
using namespace std;

void speed() {
    cin.tie(nullptr);
    cout.tie(nullptr);
    ios::sync_with_stdio(false);
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
#endif
}


struct Token {
    string value;
    string type;
    Token(const string& v, const string& t) : value(v), type(t) {}
};

struct Node {
    string name;
    int id;
    bool isSquare;
    Node(const string& v, const int& t, const bool& b) : name(v), id(t), isSquare(b) {}
};

//Node -> {child_node, 1}, {sibling_node, 0}
map<Node, vector<pair<Node, bool>>> graph;

class Parser {
private:
    vector<Token> tokens;
    size_t currentIndex;

    // Get the current token
    Token currentToken() {
        if (currentIndex < tokens.size()) {
            return tokens[currentIndex];
        }
        return Token("", "EOF");
    }

    // Match the current token and move to the next
    void eat(const string& expectedType) {
        if (currentToken().type == expectedType) {
            currentIndex++;
        } else {
            throw runtime_error("Syntax Error: Expected " + expectedType + ", found " + currentToken().type);
        }
    }

    // Grammar rules
    void parseStmtSeq() {
        parseStmt();
        while (currentIndex < tokens.size()) {
            parseStmt();
        }
    }

    void parseStmt() {
        if (currentToken().type == "IF") {
            parseIfStmt();
        } else if (currentToken().type == "REPEAT") {
            parseRepeatStmt();
        } else if (currentToken().type == "IDENTIFIER") {
            parseAssignStmt();
        } else if (currentToken().type == "READ") {
            parseReadStmt();
        } else if (currentToken().type == "WRITE") {
            parseWriteStmt();
        } else {
            //the error is from here
            throw runtime_error("Syntax Error: Unexpected token " + currentToken().type);
        }
    }

    Node parseIfStmt() {
        Node if_node;
        if_node.name = "if";
        if_node.isSquare = true;
        if_node.id = currentIndex;
        eat("IF");
        Node left_child = parseExp();
        eat("THEN");
        parseStmt();
        while (currentToken().type != "END" && currentToken().type != "ELSE") parseStmt();

        // Check for optional "ELSE" block
        if (currentToken().type == "ELSE") {
            eat("ELSE");
            parseStmt();
            while (currentToken().type != "END") parseStmt();
        }

        eat("END");
        return if_node;
    }

    Node parseRepeatStmt() {
        eat("REPEAT");
        parseStmt();
        while (currentToken().type != "UNTIL") parseStmt();
        eat("UNTIL");
        parseExp();
    }

    Node parseAssignStmt() {
        eat("IDENTIFIER");
        eat("ASSIGN");
        parseExp();
        if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
    }

    Node parseReadStmt() {
        eat("READ");
        eat("IDENTIFIER");
        eat("SEMICOLON");
    }

    Node parseWriteStmt() {
        eat("WRITE");
        eat("IDENTIFIER");
    }

    Node parseExp() {
        Node left_node = parseSimpleExp();
        if (currentToken().type == "LESSTHAN" || currentToken().type == "EQUAL") {
            Node parent_node("op" + '\n' + '(' + currentToken().value + ')', currentIndex, false);
            eat(currentToken().type);
            Node right_node = parseSimpleExp();
            graph[parent_node].emplace_back(left_node, 1);
            graph[parent_node].emplace_back(right_node, 1);
            return parent_node;
        }
        if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
        return left_node;
    }

    Node parseSimpleExp() {
        Node left_node = parseTerm();
        if (currentToken().type == "PLUS" || currentToken().type == "MINUS") {
            Node parent_node("op" + '\n' + '(' + currentToken().value + ')', currentIndex, false);
            eat(currentToken().type);
            Node right_node = parseTerm();
            graph[parent_node].emplace_back(left_node, 1);
            graph[parent_node].emplace_back(right_node, 1);
            return parent_node;
        }
        if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
        return left_node;
    }

    Node parseTerm() {
        Node left_node = parseFactor();
        if (currentToken().type == "MUL" || currentToken().type == "DIV") {
            Node parent_node("op" + '\n' + '(' + currentToken().value + ')', currentIndex, false);
            eat(currentToken().type);
            Node right_node = parseExp();
            graph[parent_node].emplace_back(left_node, 1);
            graph[parent_node].emplace_back(right_node, 1);
            return parent_node;
        }
        if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
        return left_node;
    }

    Node parseFactor() {
        if (currentToken().type == "NUMBER") {
            Node number_node("const" + '\n' + '(' + currentToken().value + ')', currentIndex, false);
            eat("NUMBER");
            return number_node;
        }
        else if (currentToken().type == "IDENTIFIER") {
            Node id_node("id" + '\n' + '(' + currentToken().value + ')', currentIndex, false);
            eat("IDENTIFIER");
            return id_node;
        }
        else if (currentToken().type == "OPENBRACKET") {
            eat("OPENBRACKET");
            parseExp();
            eat("CLOSEDBRACKET");
            return Node("", currentIndex, false);
        }
        else{
            throw runtime_error("Syntax Error: Unexpected token " + currentToken().type);
        }
    }

public:
    Parser(const vector<Token>& t) : tokens(t), currentIndex(0) {}

    void parse() {
        parseStmtSeq();
        if (currentIndex < tokens.size()) {
            cout << currentIndex << endl;
            // throw runtime_error("Syntax Error: Extra tokens at the end");
        }
        cout << "Input is valid." << endl;
    }
};


void TT() {
    int n; cin >> n;
    vector<Token> tokens;
    for (int i = 0; i < n; ++i) {
        string value, type;
        cin >> value >> type;
        tokens.emplace_back(value, type);
    }

    Parser parser(tokens);
    parser.parse();
}

signed main() {
    speed();
    TT();
}