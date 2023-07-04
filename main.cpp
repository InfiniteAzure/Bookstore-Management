#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include "processor.h"
#include "Bookstore.h"

int main() {
    freopen("../1775/1775/1775/testcase2/1.in", "r", stdin);
    //freopen("../test.in", "r", stdin);
    freopen("../ans.out", "w", stdout);
    Bookstore bookstore;
    processor p;
    std::string line;
    while (getline(std::cin, line)) {
        std::string op;
        p.readin(line);
        if (!p.cutter.empty()) {
            op = p.cutter[0];
            if (op == "exit") {
                if (p.length != 1) {
                    errors();
                } else {
                    return 0;
                }
            } else if (op == "quit") {
                if (p.length != 1) {
                    errors();
                } else {
                    return 0;
                }
            } else if (op == "su") {
                if (p.length == 3) {
                    bookstore.login(p.cutter[1], p.cutter[2]);
                } else if (p.length == 2) {
                    bookstore.login(p.cutter[1]);
                } else {
                    errors();
                }
            } else if (op == "logout") {
                if (p.length != 1) {
                    errors();
                } else {
                    bookstore.logout();
                }
            } else if (op == "register") {
                if (p.length != 4) {
                    errors();
                } else {
                    bookstore.registe(p.cutter[2], p.cutter[1]);
                }
            } else if (op == "passwd") {
                if (p.length == 3) {
                    bookstore.modify_password(p.cutter[1], p.cutter[2]);
                } else if (p.length == 4) {
                    bookstore.modify_password(p.cutter[1], p.cutter[3], p.cutter[2]);
                } else {
                    errors();
                }
            } else if (op == "useradd") {
                if (p.length != 5) {
                    errors();
                } else {
                    bookstore.useradd(p.cutter[1], p.cutter[2], p.cutter[3][0] - '0');
                }
            } else if (op == "delete") {
                if (p.length != 2) {
                    errors();
                } else {
                    bookstore.delete_user(p.cutter[1]);
                }
            } else if (op == "show") {
                if (bookstore.login_stack.empty()) {
                    errors();
                } else {
                    if (p.length == 1) {
                        bookstore.cout_all();
                    } else if (p.cutter[1] == "finance") {
                        if (p.length == 2) {
                            bookstore.search_finance();
                        } else {
                            bool check = true;
                            for (int i = 0;i < p.cutter[2].length();++i) {
                                if (p.cutter[2][i] < '0' || p.cutter[2][i] > '9') {
                                    check = false;
                                }
                            }
                            if (check) {
                                bookstore.search_finance(to_int(p.cutter[2]));
                            } else {
                                errors();
                            }
                        }
                    } else {
                        if (p.cutter[1][1] == 'I') {
                            std::string Isbn;
                            for (int i = 6; i < p.cutter[1].length(); ++i) {
                                Isbn += p.cutter[1][i];
                            }
                            if (Isbn.empty()) {
                                errors();
                            } else {
                                bookstore.search_ISBN(Isbn);
                            }
                        } else if (p.cutter[1][1] == 'a') {
                            std::string author;
                            for (int i = 9; i < p.cutter[1].length() - 1; ++i) {
                                author += p.cutter[1][i];
                            }
                            if (author.empty()) {
                                errors();
                            } else {
                                bookstore.search_author(author);
                            }
                        } else if (p.cutter[1][1] == 'n') {
                            std::string name;
                            for (int i = 7; i < p.cutter[1].length() - 1; ++i) {
                                name += p.cutter[1][i];
                            }
                            if (name.empty()) {
                                errors();
                            } else {
                                bookstore.search_name(name);
                            }
                        } else if (p.cutter[1][1] == 'k') {
                            std::string keyword;
                            for (int i = 10; i < p.cutter[1].length() - 1; ++i) {
                                keyword += p.cutter[1][i];
                            }
                            if (keyword.empty()) {
                                errors();
                            } else {
                                bool check = true;
                                for (int i = 0;i < keyword.length();++i) {
                                    if (keyword[i] == '|') {
                                        check = false;
                                    }
                                }
                                if (check) {
                                    bookstore.search_keyword(keyword);
                                } else {
                                    errors();
                                }
                            }

                        } else {
                            errors();
                        }
                    }
                }
            } else if (op == "buy") {
                if (p.length != 3) {
                    errors();
                } else {
                    bookstore.buy_books(p.cutter[1], to_int(p.cutter[2]));
                }
            } else if (op == "select") {
                if (p.length != 2) {
                    errors();
                } else {
                    bookstore.select(p.cutter[1]);
                }
            } else if (op == "modify") {
                if (bookstore.login_stack.empty() ||
                    !(bookstore.login_stack[bookstore.login_stack.size() - 1].itself.selected) ||
                    bookstore.login_stack[bookstore.login_stack.size() - 1].itself.privilege < 3) {
                    errors();
                } else {
                    for (int j = 1; j < p.length; ++j) {
                        if (p.cutter[j][1] == 'I') {
                            std::string Isbn;
                            for (int i = 6; i < p.cutter[j].length(); ++i) {
                                Isbn += p.cutter[j][i];
                            }
                            if (Isbn.empty()) {
                                errors();
                            } else {
                                bookstore.modify_ISBN(Isbn);
                            }
                        } else if (p.cutter[j][1] == 'a') {
                            std::string author;
                            for (int i = 9; i < p.cutter[j].length() - 1; ++i) {
                                author += p.cutter[j][i];
                            }
                            if (author.empty()) {
                                errors();
                            } else {
                                bookstore.modify_author(author);
                            }
                        } else if (p.cutter[j][1] == 'n') {
                            std::string name;
                            for (int i = 7; i < p.cutter[j].length() - 1; ++i) {
                                name += p.cutter[j][i];
                            }
                            if (name.empty()) {
                                errors();
                            } else {
                                bookstore.modify_name(name);
                            }
                        } else if (p.cutter[j][1] == 'k') {
                            std::string keyword;
                            for (int i = 10; i < p.cutter[j].length() - 1; ++i) {
                                keyword += p.cutter[j][i];
                            }
                            if (keyword.empty()) {
                                errors();
                            } else {
                                bookstore.modify_keyword(keyword);
                            }
                        } else if (p.cutter[j][1] == 'p') {
                            std::string price;
                            for (int i = 7; i < p.cutter[j].length(); ++i) {
                                price += p.cutter[j][i];
                            }
                            bool check = true;
                            for (int i = 0; i < price.length(); ++i) {
                                if ((price[i] > '9' || price[i] < '0') && price[i] != '.') {
                                    check = false;
                                }
                            }
                            if (check) {
                                bookstore.modify_price(price);
                            } else {
                                errors();
                            }
                        }
                    }
                }
            } else if (op == "import") {
                if (p.length != 3) {
                    errors();
                } else {
                    bool check = true;
                    for (int i = 0; i < p.cutter[1].length(); ++i) {
                        if (p.cutter[1][i] > '9' || p.cutter[1][i] < '0') {
                            check = false;
                        }
                    }
                    if (check) {
                        bookstore.import_books(to_int(p.cutter[1]), to_double(p.cutter[2]));
                    } else {
                        errors();
                    }
                }
            } else if(op == "log") {
                if (p.length != 1) {
                    errors();
                } else {
                    bookstore.printlog();
                }
            } else {
                errors();
            }
        }
    }
}