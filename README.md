# Parser Project

## Description
**Pareser Project** or **SyntaxTreeBuilder** is a C++ project that implements a recursive descent parser for generating syntax trees from input tokens. The project validates syntax based on a predefined grammar, builds a syntax tree using child-sibling relationships, and represents it as an adjacency list for further processing in the gui.

This project is a practical implementation of compiler design principles, focusing on lexical analysis, syntax validation, and data structure representation for syntax trees.

---

## Features
- Recursive descent parsing for a the tiny language grammar.
- Syntax validation for tokenized input.
- Construction of syntax trees using child-sibling pointers.
- Representation of the syntax tree as an adjacency list (`map<TreeNode*, vector<TreeNode*>>`).
- Modular design for future extensions or integrations.
