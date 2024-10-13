#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <vector>
#include <set>

enum TokenType {
    INTEGER,
    FLOAT,
    STRING,
    CHAR,
    OPERATOR,
    IDENTIFIER,
    PUNCTUATION,
    DIRECTIVE,
    KEYWORD,
    COMMENT,
    MEMBER_ACCESS,
    UNKNOWN
};

struct Token {
    std::string value;
    TokenType type;
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
    case INTEGER: return "INTEGER";
    case FLOAT: return "FLOAT";
    case STRING: return "STRING";
    case CHAR: return "CHAR";
    case OPERATOR: return "OPERATOR";
    case IDENTIFIER: return "IDENTIFIER";
    case PUNCTUATION: return "PUNCTUATION";
    case DIRECTIVE: return "DIRECTIVE";
    case KEYWORD: return "KEYWORD";
    case COMMENT: return "COMMENT";
    case MEMBER_ACCESS: return "MEMBER_ACCESS";
    case UNKNOWN: return "UNKNOWN";
    default: return "UNKNOWN";
    }
}

std::string tokenTypeToColor(TokenType type) {
    switch (type) {
    case INTEGER: return "color: orange;";
    case FLOAT: return "color: pink;";
    case STRING: return "color: blue;";
    case CHAR: return "color: yellow;";
    case OPERATOR: return "color: purple;";
    case IDENTIFIER: return "color: darkblue;";
    case PUNCTUATION: return "color: black;";
    case DIRECTIVE: return "color: teal;";
    case KEYWORD: return "color: green;";
    case COMMENT: return "color: gray;";
    case MEMBER_ACCESS: return "color: darkviolet;";
    case UNKNOWN: return "color: red;";
    default: return "color: red;";
    }
}

Token recognizeToken(const std::string& str) {
    static const std::set<std::string> keywords = {
        "int", "float", "double", "char", "if", "else", "for", "while", "return", "void", "using", "namespace", "const", "long long"
    };

    std::regex intRegex("^[0-9]+$");
    std::regex floatRegex("^[0-9]*\\.[0-9]+([eE][+-]?[0-9]+)?$|^[0-9]+([eE][+-]?[0-9]+)?$");
    std::regex stringRegex("^\".*\"$");
    std::regex charRegex("^'.'$");
    std::regex operatorRegex("^(\\+|-|\\*|/|=|==|!=|>|<|>=|%|\\[|\\]|<=|&&|!|&|\\|\\||::)$");
    std::regex identifierRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    std::regex punctuationRegex("^[,;(){}<>]$");
    std::regex directiveRegex("^#(include|define|ifdef|ifndef|endif|pragma)[ \t]+([^ \t]+).*");
    std::regex memberAccessRegex("^([a-zA-Z_][a-zA-Z0-9_]*)\\.([a-zA-Z_][a-zA-Z0-9_]*)$");

    if (keywords.find(str) != keywords.end()) {
        return { str, KEYWORD };
    }
    if (std::regex_match(str, directiveRegex)) {
        return { str, DIRECTIVE };
    }
    else if (std::regex_match(str, intRegex)) {
        return { str, INTEGER };
    }
    else if (std::regex_match(str, floatRegex)) {
        return { str, FLOAT };
    }
    else if (std::regex_match(str, stringRegex)) {
        return { str, STRING };
    }
    else if (std::regex_match(str, charRegex)) {
        return { str, CHAR };
    }
    else if (std::regex_match(str, memberAccessRegex)) {
        return { str, MEMBER_ACCESS };
    }
    else if (std::regex_match(str, operatorRegex)) {
        return { str, OPERATOR };
    }
    else if (std::regex_match(str, identifierRegex)) {
        return { str, IDENTIFIER };
    }
    else if (std::regex_match(str, punctuationRegex)) {
        return { str, PUNCTUATION };
    }
    else {
        return { str, UNKNOWN };
    }
}

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    std::string token;
    bool insideString = false;
    bool insideChar = false;
    bool insideDirective = false;

    for (size_t i = 0; i < code.size(); ++i) {
        char current = code[i];

        if (current == '#') {
            if (!token.empty()) {
                tokens.push_back(recognizeToken(token));
                token.clear();
            }
            token += current;
            insideDirective = true;
            continue;
        }

        if (insideDirective) {
            if (current == '\n') {
                tokens.push_back(recognizeToken(token));
                token.clear();
                insideDirective = false;
            }
            else {
                token += current;
            }
            continue;
        }

        if (current == '"' || current == '\'') {
            if ((current == '"' && insideString) || (current == '\'' && insideChar)) {
                token += current;
                tokens.push_back(recognizeToken(token));
                token.clear();
                insideString = false;
                insideChar = false;
            }
            else {
                if (!token.empty()) {
                    tokens.push_back(recognizeToken(token));
                    token.clear();
                }
                token += current;
                insideString = (current == '"');
                insideChar = (current == '\'');
            }
            continue;
        }

        if (insideString || insideChar) {
            token += current;
            continue;
        }

        if (current == '/' && i + 1 < code.size() && code[i + 1] == '/') {
            if (!token.empty()) {
                tokens.push_back(recognizeToken(token));
                token.clear();
            }
            token += current;
            token += code[i + 1];
            while (i < code.size() && code[i] != '\n') {
                token += code[i++];
            }

            if (token.size() > 2) {
                tokens.push_back({ token.substr(2), COMMENT });
            }
            token.clear();
            continue;
        }

        if (current == ':' && i + 1 < code.size() && code[i + 1] == ':') {
            if (!token.empty()) {
                tokens.push_back(recognizeToken(token));
                token.clear();
            }
            token += current;
            token += code[i + 1];
            i += 1;
            tokens.push_back(recognizeToken(token));
            token.clear();
            continue;
        }

        if (isspace(current)) {
            if (!token.empty()) {
                tokens.push_back(recognizeToken(token));
                token.clear();
            }
        }

        else if (std::ispunct(current) && current != '_' && current != '.') {
            if (!token.empty()) {
                tokens.push_back(recognizeToken(token));
                token.clear();
            }
            tokens.push_back(recognizeToken(std::string(1, current)));
        }
        else {
            token += current;
        }
    }

    if (!token.empty()) {
        tokens.push_back(recognizeToken(token));
    }

    return tokens;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    return content;
}

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "<" << token.value << ", " << tokenTypeToString(token.type) << ">\n";
    }
}

void writeTokensToHtml(const std::vector<Token>& tokens, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    file << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<title>Token Output</title>\n</head>\n<body>\n";
    file << "<h1>Token Output</h1>\n<pre>\n";

    std::string currentLine;
    int indentLevel = 0; 

    for (const auto& token : tokens) {
        if (token.value == "{") {
            currentLine += token.value + "\n"; 
            indentLevel++;
            currentLine.append(indentLevel * 4, ' ');
            continue;
        }

        if (token.value == "}") {
            currentLine += "\n";
            indentLevel--;
            currentLine.append(indentLevel * 4, ' '); 
            currentLine += token.value + "\n"; 
            continue;
        }

        if (token.value == ";" || token.value == ":") {
            currentLine += token.value + "\n"; 
            currentLine.append(indentLevel * 4, ' ');
            continue;
        }

        if (token.value == "\n") {
            file << currentLine << "<br>";
            currentLine.clear();
            currentLine.append(indentLevel * 4, ' '); 
            continue;
        }

        std::string tokenValue = token.value;
        for (char& c : tokenValue) {
            if (c == ' ') {
                currentLine += "&nbsp;";
            }
            else {
                currentLine += "<span style=\"" + tokenTypeToColor(token.type) + "\">" + c + "</span>";
            }
        }
        currentLine += " "; 
    }

    if (!currentLine.empty()) {
        file << currentLine;
    }

    file << "</pre>\n</body>\n</html>";
    file.close();
}


int main() {
    try {
        std::string code = readFile("file.txt");
        std::vector<Token> tokens = tokenize(code);

        printTokens(tokens);

        writeTokensToHtml(tokens, "output.html");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return 0;
}
