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
    string value;
    int id;
    bool type;
    Node(const string& v, const string &m, const size_t& t, const bool& b) : name(v), value(m), id(t), type(b) {}

    // Define a less-than operator for Node
    bool operator<(const Node& other) const {
        if (id != other.id) {
            return id < other.id;
        }
        return name < other.name;
    }
};

//Node -> {child_node, 1}, {sibling_node, 0}
map<Node, vector<pair<Node, bool>>> graph;

class Parser {
private:
    vector<Token> tokens;
    size_t currentIndex = 0;
    Node root;


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
        }
        else {
            throw runtime_error("Syntax Error: Expected " + expectedType + ", found " + currentToken().type);
        }
    }

    // Grammar rules
    void parseStmtSeq() {
        Node prv = parseStmt();
        while (currentIndex < tokens.size()) {
            Node cur = parseStmt();
            graph[prv].emplace_back(cur, 0);
        }
    }

    Node parseStmt() {
        if (currentToken().type == "IF") {
            return parseIfStmt();
        } else if (currentToken().type == "REPEAT") {
            return parseRepeatStmt();
        } else if (currentToken().type == "IDENTIFIER") {
            return parseAssignStmt();
        } else if (currentToken().type == "READ") {
            return parseReadStmt();
        } else if (currentToken().type == "WRITE") {
            return parseWriteStmt();
        } else {
            //the error is from here
            throw runtime_error("Syntax Error: Unexpected token " + currentToken().type);
        }
    }

    Node parseIfStmt() {
        Node if_node("if", "", currentIndex, true);
        eat("IF");
        Node left_child = parseExp();
        eat("THEN");
        graph[if_node].emplace_back(left_child, 1);
        Node right_child = parseStmt();
        graph[if_node].emplace_back(right_child, 1);
        Node prv(right_child);
        while (currentToken().type != "END" && currentToken().type != "ELSE") {
            Node cur = parseStmt();
            graph[prv].emplace_back(cur, 0);
            prv = cur;
        }

        // Check for optional "ELSE" block
        if (currentToken().type == "ELSE") {
            Node else_node("else", "", currentIndex, true);
            eat("ELSE");
            Node left_else_node = parseStmt();
            graph[else_node].emplace_back(left_else_node, 1);
            Node prv_else(left_else_node);
            while (currentToken().type != "END") {
                Node cur = parseStmt();
                graph[prv_else].emplace_back(cur, 0);
                prv_else = cur;
            }
            graph[if_node].emplace_back(else_node, 1);
        }

        eat("END");
        return if_node;
    }

    Node parseRepeatStmt() {
        Node repeat("Repeat", "", currentIndex, true);
        eat("REPEAT");
        Node left_child = parseStmt();
        graph[repeat].emplace_back(left_child, 1);
        Node prv(left_child);
        while (currentToken().type != "UNTIL") {
            Node cur = parseStmt();
            graph[prv].emplace_back(cur, 0);
            prv = cur;
        }
        eat("UNTIL");
        Node right_child = parseExp();
        graph[repeat].emplace_back(right_child, 1);
        return repeat;
    }

    Node parseAssignStmt() {
        Node parent("Assign", currentToken().value, currentIndex, true);
        eat("IDENTIFIER");
        eat("ASSIGN");
        Node child = parseExp();
        graph[parent].emplace_back(child, 1);
        if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
        return parent;
    }

    Node parseReadStmt() {
        eat("READ");
        Node read("read", currentToken().value, currentIndex - 1, true);
        eat("IDENTIFIER");
        if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
        return read;
    }

    Node parseWriteStmt() {
        Node write("write", "", currentIndex, true);
        eat("WRITE");
        Node child("", "", -1, 0);
        if (currentToken().type == "IDENTIFIER") {
            child.name = "id";
            child.value = currentToken().value;
            child.id = currentIndex;
            child.type = false;
            eat("IDENTIFIER");
        }
        else if (currentToken().type == "NUMBER") {
            child.name = "const";
            child.value = currentToken().value;
            child.id = currentIndex;
            child.type = false;
            eat("NUMBER");
        }
        graph[write].emplace_back(child, 1);
        return write;
    }

    Node parseExp() {
        Node left_node = parseSimpleExp();
        if (currentToken().type == "LESSTHAN" || currentToken().type == "EQUAL") {
            Node parent_node("op", currentToken().value, currentIndex, false);
            eat(currentToken().type);
            Node right_node = parseExp();
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
            Node parent_node("op", currentToken().value, currentIndex, false);
            eat(currentToken().type);
            Node right_node = parseExp();
            graph[parent_node].emplace_back(left_node, 1);
            graph[parent_node].emplace_back(right_node, 1);
            return parent_node;
        }
        if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
        return left_node;
    }

    Node parseTerm() {
        Node left_node = parseFactor();
        if (currentToken().type == "MULT" || currentToken().type == "DIV") {
            Node parent_node("op", currentToken().value, currentIndex, false);
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
            Node number_node("const", currentToken().value, currentIndex, false);
            eat("NUMBER");
            return number_node;
        }
        else if (currentToken().type == "IDENTIFIER") {
            Node id_node("id", currentToken().value, currentIndex, false);
            eat("IDENTIFIER");
            return id_node;
        }
        else if (currentToken().type == "OPENBRACKET") {
            eat("OPENBRACKET");
            Node expr_node = parseExp();
            eat("CLOSEDBRACKET");
            return expr_node;
        }
        else{
            throw runtime_error("Syntax Error: Unexpected token " + currentToken().type);
        }
    }

public:
    Parser(const vector<Token>& t) : tokens(t), currentIndex(0), root("", "", -1, false) {
    }

    Node getRoot() {
        return root;
    }

    void parse() {
        if (currentToken().type == "READ") {
            root = Node("read", tokens[currentIndex + 1].value, currentIndex, 1);
        }
        else if (currentToken().type == "WRITE") {
            root = Node("write", "", currentIndex, 1);
        }
        else if (currentToken().type == "IDENTIFIER") {
            root = Node("assign", currentToken().value, currentIndex, 1);
        }
        else if (currentToken().type == "REPEAT") {
            root = Node("repeat", "", currentIndex, 1);
        }
        else {
            root = Node("if", "", currentIndex, 1);
        }
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

    Node st = parser.getRoot();
    function<void(Node,Node)> dfs = [&](Node currentNode,Node prev) {
        cout << currentNode.name << ' ' << currentNode.value << ' ' << currentNode.id << endl;
        for (auto &x : graph[currentNode]) {
            if (x.first.id == prev.id) continue;
            dfs(x.first, currentNode);
        }
    };
    dfs(st, Node("", "", -1, 0));
}

signed main() {
    speed();
    TT();
}
