//
// Created by Xue Jiarui on 2023/1/1.
//

#include "BlockedLinkList.h"
#include "Books.h"
#include "Users.h"
#include "Finance.h"
#include "log.h"
#include "processor.h"

#ifndef MAIN_CPP_BOOKSTORE_H
#define MAIN_CPP_BOOKSTORE_H

class Bookstore {
public:
    BlockedLinkList Userid;
    BlockedLinkList Name;
    BlockedLinkList Author;
    BlockedLinkList ISBN;
    BlockedLinkList Keyword;
    BlockedLinkList Finance;

    std::fstream user_file;
    std::fstream book_file;
    std::fstream finance_file;
    std::fstream log_file;

    std::vector<Users> login_stack;

    int user_save;
    int book_save;
    int finance_save;
    int log_save;

    Bookstore() : Userid("Users"), Name("Name"), Author("Author"),
                  ISBN("ISBN"), Keyword("Keyword"), Finance("Finance") {
        user_file.open("userinfo");
        if (!user_file.good()) {
            std::ofstream out;
            out.open("userinfo");
            out.seekp(0);
            user_save = 1;
            out.write(reinterpret_cast<char *>(&user_save), sizeof(int));
            Users root;
            root.init("root", "sjtu", "root", 7, user_save);
            out.seekp(sizeof(int));
            out.write(reinterpret_cast<char *>(&root.itself), sizeof(Users::UserInfo));
            out.close();
            user_file.open("userinfo");
            Userid.add("root", 1);
        } else {
            user_file.seekg(0);
            user_file.read(reinterpret_cast<char *>(&user_save), sizeof(int));
        }
        book_file.open("bookinfo");
        if (!book_file.good()) {
            std::ofstream out;
            out.open("bookinfo");
            out.seekp(0);
            book_save = 0;
            out.write(reinterpret_cast<char *>(&book_save), sizeof(int));
            out.close();
            book_file.open("bookinfo");
        } else {
            book_file.seekg(0);
            book_file.read(reinterpret_cast<char *>(&book_save), sizeof(int));
        }
        finance_file.open("financerecord");
        if (!finance_file.good()) {
            std::ofstream out;
            out.open("financerecord");
            out.seekp(0);
            finance_save = 0;
            out.write(reinterpret_cast<char *>(&finance_save), sizeof(int));
            out.close();
            finance_file.open("financerecord");
        } else {
            finance_file.seekg(0);
            finance_file.read(reinterpret_cast<char *>(&finance_save), sizeof(int));
        }
        log_file.open("log");
        if (!log_file.good()) {
            std::ofstream out;
            out.open("log");
            out.seekp(0);
            log_save = 0;
            out.write(reinterpret_cast<char *>(&log_save), sizeof(int));
            out.close();
            log_file.open("log");
        } else {
            log_file.seekg(0);
            log_file.read(reinterpret_cast<char *>(&log_save), sizeof(int));
        }
    }

    ~Bookstore() {
        book_file.seekp(0);
        book_file.write(reinterpret_cast<char *>(&book_save), sizeof(int));
        user_file.seekp(0);
        user_file.write(reinterpret_cast<char *>(&user_save), sizeof(int));
        finance_file.seekp(0);
        finance_file.write(reinterpret_cast<char *>(&finance_save), sizeof(int));
        log_file.seekp(0);
        log_file.write(reinterpret_cast<char *>(&log_save), sizeof(int));
        book_file.close();
        user_file.close();
        finance_file.close();
        log_file.close();
    }

    void add_book(Books::BookInfo book) {
        book.write_place = ++book_save;
        Name.add(back_to_string(book.name), book.write_place);
        Author.add(back_to_string(book.author), book.write_place);
        ISBN.add(back_to_string(book.ISBN), book.write_place);
        std::vector<std::string> keywords;
        keyword_process(back_to_string(book.keyword), keywords);
        for (int i = 0; i < keywords.size(); ++i) {
            Keyword.add(keywords[i], book.write_place);
        }
        book_file.seekp(sizeof(int) + (book.write_place - 1) * sizeof(Books::BookInfo));
        book_file.write(reinterpret_cast<char *>(&book), sizeof(Books::BookInfo));
    }

    void import_books(double quantity, double cost) {
        if (login_stack.empty() || !login_stack[login_stack.size() - 1].itself.selected) {
            errors();
            return;
        } else {
            int place = login_stack[login_stack.size() - 1].itself.selected_book.write_place;
            book_file.seekg(sizeof(int) + (place - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                           sizeof(Books::BookInfo));
            login_stack[login_stack.size() - 1].itself.selected_book.quantity += quantity;
            book_file.seekp(sizeof(int) + (login_stack[login_stack.size() - 1].itself.selected_book.write_place - 1) *
                                          sizeof(Books::BookInfo));
            book_file.write(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                            sizeof(Books::BookInfo));
            Finance::record f;
            to_string(back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN), f.ISBN);
            f.money = -1 * cost;
            finance_save++;
            finance_file.seekp(sizeof(int) + (finance_save - 1) * sizeof(Finance::record));
            finance_file.write(reinterpret_cast<char *>(&f), sizeof(Finance::record));
            Finance.add(back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN), finance_save);
            std::string s;
            s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
            s += " import ";
            s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
            s += " quantity= ";
            s += std::to_string(quantity);
            log l(s);
            log_file.seekp(sizeof(int) + log_save*sizeof(log));
            log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
            log_save++;
        }
    }

    void buy_books(std::string target_ISBN, int quantity) {
        if (login_stack.empty() || login_stack[login_stack.size() - 1].itself.privilege < 1) {
            errors();
            return;
        }
        if (quantity <= 0) {
            errors();
            return;
        }
        std::vector<int> ans;
        ISBN.search(target_ISBN, ans);
        if (ans.empty()) {
            errors();
            return;
        }
        Books::BookInfo info;
        book_file.seekg(sizeof(int) + (ans[0] - 1) * sizeof(Books::BookInfo));
        book_file.read(reinterpret_cast<char *>(&info), sizeof(Books::BookInfo));
        if (info.quantity < quantity) {
            errors();
            return;
        }
        info.quantity -= quantity;
        std::cout << std::fixed << std::setprecision(2) << quantity * info.price << '\n';
        book_file.seekp(sizeof(int) + (ans[0] - 1) * sizeof(Books::BookInfo));
        book_file.write(reinterpret_cast<char *>(&info), sizeof(Books::BookInfo));
        Finance::record f;
        to_string(target_ISBN, f.ISBN);
        f.money = quantity * info.price;
        finance_save++;
        finance_file.seekp(sizeof(int) + (finance_save - 1) * sizeof(Finance::record));
        finance_file.write(reinterpret_cast<char *>(&f), sizeof(Finance::record));
        Finance.add(target_ISBN, finance_save);
        std::string s;
        s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
        s += " buy ";
        s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
        s += " quantity= ";
        s += std::to_string(quantity);
        s += " cost= ";
        s += std::to_string(quantity*info.price);
        log l(s);
        log_file.seekp(sizeof(int) + log_save*sizeof(log));
        log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
        log_save++;
    }

    void select(std::string target_ISBN) {
        if (login_stack.empty() || login_stack[login_stack.size() - 1].itself.privilege < 3) {
            errors();
            return;
        }
        std::vector<int> ans;
        ISBN.search(target_ISBN, ans);
        if (ans.empty()) {
            Books::BookInfo book;
            to_string(target_ISBN, book.ISBN);
            book.write_place = ++book_save;
            book_file.seekp(sizeof(int) + (book.write_place - 1) * sizeof(Books::BookInfo));
            book_file.write(reinterpret_cast<char *>(&book), sizeof(Books::BookInfo));
            ISBN.add(target_ISBN, book.write_place);
            ISBN.search(target_ISBN, ans);
        }
        Books book;
        book_file.seekg(sizeof(int) + (ans[0] - 1) * sizeof(Books::BookInfo));
        book_file.read(reinterpret_cast<char *>(&book.itself), sizeof(Books::BookInfo));
        login_stack[login_stack.size() - 1].itself.selected_book = book.itself;
        login_stack[login_stack.size() - 1].itself.selected = true;
        std::string s;
        s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
        s += " select ";
        s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
        log l(s);
        log_file.seekp(sizeof(int) + log_save*sizeof(log));
        log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
        log_save++;
    }

    void modify_ISBN(std::string new_ISBN) {
        if (new_ISBN.length() > 20) {
            errors();
            return;
        }
        if (login_stack.empty() || !login_stack[login_stack.size() - 1].itself.selected) {
            errors();
            return;
        } else {
            int place = login_stack[login_stack.size() - 1].itself.selected_book.write_place;
            book_file.seekg(sizeof(int) + (place - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                           sizeof(Books::BookInfo));
            std::string del;
            del = back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
            if (new_ISBN == del) {
                errors();
                return;
            }
            std::vector<int> check;
            ISBN.search(new_ISBN, check);
            if (!check.empty()) {
                errors();
                return;
            }
            to_string(new_ISBN, login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
            ISBN.add(new_ISBN, login_stack[login_stack.size() - 1].itself.selected_book.write_place);
            ISBN.erase(del, login_stack[login_stack.size() - 1].itself.selected_book.write_place);
            book_file.seekp(sizeof(int) + (login_stack[login_stack.size() - 1].itself.selected_book.write_place - 1) *
                                          sizeof(Books::BookInfo));
            book_file.write(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                            sizeof(Books::BookInfo));

            std::string s;
            s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
            s += " modified ";
            s += del;
            s += " to ";
            s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
            log l(s);
            log_file.seekp(sizeof(int) + log_save*sizeof(log));
            log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
            log_save++;
        }
    }

    void modify_price(std::string price) {
        if (login_stack.empty() || !login_stack[login_stack.size() - 1].itself.selected) {
            errors();
            return;
        } else {
            int place = login_stack[login_stack.size() - 1].itself.selected_book.write_place;
            book_file.seekg(sizeof(int) + (place - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                           sizeof(Books::BookInfo));
            double d = to_double(price);
            login_stack[login_stack.size() - 1].itself.selected_book.price = d;
            book_file.seekp(sizeof(int) + (login_stack[login_stack.size() - 1].itself.selected_book.write_place - 1) *
                                          sizeof(Books::BookInfo));
            book_file.write(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                            sizeof(Books::BookInfo));
            std::string s;
            s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
            s += " modified ";
            s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
            s += " price to ";
            s += price;
            log l(s);
            log_file.seekp(sizeof(int) + log_save*sizeof(log));
            log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
            log_save++;
        }
    }

    void modify_name(std::string new_name) {
        if (new_name.length() > 60) {
            errors();
            return;
        }
        if (login_stack.empty() || !login_stack[login_stack.size() - 1].itself.selected) {
            errors();
            return;
        } else {
            int place = login_stack[login_stack.size() - 1].itself.selected_book.write_place;
            book_file.seekg(sizeof(int) + (place - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                           sizeof(Books::BookInfo));
            std::string del;
            del = back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.name);
            to_string(new_name, login_stack[login_stack.size() - 1].itself.selected_book.name);
            Name.erase(del, login_stack[login_stack.size() - 1].itself.selected_book.write_place);
            Name.add(new_name, login_stack[login_stack.size() - 1].itself.selected_book.write_place);
            book_file.seekp(sizeof(int) + (login_stack[login_stack.size() - 1].itself.selected_book.write_place - 1) *
                                          sizeof(Books::BookInfo));
            book_file.write(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                            sizeof(Books::BookInfo));
            std::string s;
            s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
            s += " modified ";
            s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
            s += " name to ";
            s += new_name;
            log l(s);
            log_file.seekp(sizeof(int) + log_save*sizeof(log));
            log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
            log_save++;
        }
    }

    void modify_author(std::string new_author) {
        if (new_author.length() > 60) {
            errors();
            return;
        }
        if (login_stack.empty() || !login_stack[login_stack.size() - 1].itself.selected) {
            errors();
            return;
        } else {
            int place = login_stack[login_stack.size() - 1].itself.selected_book.write_place;
            book_file.seekg(sizeof(int) + (place - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                           sizeof(Books::BookInfo));
            std::string del;
            del = back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.author);
            to_string(new_author, login_stack[login_stack.size() - 1].itself.selected_book.author);
            Author.erase(del, login_stack[login_stack.size() - 1].itself.selected_book.write_place);
            Author.add(new_author, login_stack[login_stack.size() - 1].itself.selected_book.write_place);
            book_file.seekp(sizeof(int) + (login_stack[login_stack.size() - 1].itself.selected_book.write_place - 1) *
                                          sizeof(Books::BookInfo));
            book_file.write(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                            sizeof(Books::BookInfo));
            std::string s;
            s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
            s += " modified ";
            s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
            s += " author to ";
            s += new_author;
            log l(s);
            log_file.seekp(sizeof(int) + log_save*sizeof(log));
            log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
            log_save++;
        }
    }

    void modify_keyword(std::string new_keywords) {
        if (login_stack.empty() || !login_stack[login_stack.size() - 1].itself.selected) {
            errors();
            return;
        }
        if (!check_keyword(new_keywords)) {
            errors();
            return;
        }
        int place = login_stack[login_stack.size() - 1].itself.selected_book.write_place;
        book_file.seekg(sizeof(int) + (place - 1) * sizeof(Books::BookInfo));
        book_file.read(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                       sizeof(Books::BookInfo));
        std::vector<std::string> new_key;
        std::vector<std::string> del_key;
        keyword_process(back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.keyword), del_key);
        keyword_process(new_keywords, new_key);
        for (int i = 0;i < new_key.size();++i) {
            for (int j = 0;j < new_key.size();++j) {
                if (i != j && new_key[i] == new_key[j]) {
                    errors();
                    return;
                }
            }
        }
        for (int i = 0; i < del_key.size(); i++) {
            Keyword.erase(del_key[i], login_stack[login_stack.size() - 1].itself.selected_book.write_place);
        }
        for (int i = 0; i < new_key.size(); i++) {
            Keyword.add(new_key[i], login_stack[login_stack.size() - 1].itself.selected_book.write_place);
        }
        to_string(new_keywords, login_stack[login_stack.size() - 1].itself.selected_book.keyword);
        book_file.seekp(sizeof(int) + (login_stack[login_stack.size() - 1].itself.selected_book.write_place - 1) *
                                      sizeof(Books::BookInfo));
        book_file.write(reinterpret_cast<char *>(&login_stack[login_stack.size() - 1].itself.selected_book),
                        sizeof(Books::BookInfo));
        std::string s;
        s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
        s += " modified ";
        s += back_to_string(login_stack[login_stack.size() - 1].itself.selected_book.ISBN);
        s += " keywords to ";
        s += new_keywords;
        log l(s);
        log_file.seekp(sizeof(int) + log_save*sizeof(log));
        log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
        log_save++;
    }

    void registe(std::string Password, std::string userid) {
        std::vector<int> check;
        Userid.search(userid, check);
        if (!check.empty()) {
            errors();
            return;
        }
        if (!(check_user(userid) || check_user(Password))) {
            errors();
            return;
        }
        Users::UserInfo new_user;
        to_string(Password, new_user.password);
        to_string(userid, new_user.id);
        new_user.privilege = 1;
        new_user.save_place = ++user_save;
        user_file.seekp(sizeof(int) + (new_user.save_place - 1) * sizeof(Users::UserInfo));
        user_file.write(reinterpret_cast<char *>(&new_user), sizeof(Users::UserInfo));
        Userid.add(userid, new_user.save_place);
        std::string s;
        s += userid;
        s += " register";
        log l(s);
        log_file.seekp(sizeof(int) + log_save*sizeof(log));
        log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
        log_save++;
    }

    void login(std::string id, std::string password = "") {
        std::vector<int> res;
        Userid.search(id, res);
        if (res.empty()) {
            errors();
            return;
        }
        Users info;
        Users::UserInfo u;
        user_file.seekg(sizeof(int) + (res[0] - 1) * sizeof(Users::UserInfo));
        user_file.read(reinterpret_cast<char *>(&u), sizeof(Users::UserInfo));
        info.itself = u;
        if (password == info.itself.password) {
            login_stack.push_back(info);
            std::string s;
            s += id;
            s += " login";
            log l(s);
            log_file.seekp(sizeof(int) + log_save*sizeof(log));
            log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
            log_save++;
        } else if (password == "") {
            if (login_stack.empty()) {
                errors();
                return;
            } else if (login_stack[login_stack.size() - 1].itself.privilege > info.itself.privilege) {
                login_stack.push_back(info);
                std::string s;
                s += id;
                s += " login by administrator";
                log l(s);
                log_file.seekp(sizeof(int) + log_save*sizeof(log));
                log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
                log_save++;
            } else {
                errors();
                return;
            }
        } else {
            errors();
            return;
        }
    }

    void logout() {
        if (login_stack.empty()) {
            errors();
            return;
        }
        login_stack[login_stack.size() - 1].itself.selected = false;
        std::string s;
        s += back_to_string( login_stack[login_stack.size() - 1].itself.id);
        s += " log out";
        log l(s);
        log_file.seekp(sizeof(int) + log_save*sizeof(log));
        log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
        log_save++;
        login_stack.pop_back();
    }

    void useradd(std::string userid, std::string password, int pri) {
        if (login_stack.empty() || login_stack[login_stack.size() - 1].itself.privilege < 3 ||
            login_stack[login_stack.size() - 1].itself.privilege <= pri) {
            errors();
            return;
        }
        if (pri != 0 && pri != 1 && pri != 3) {
            errors();
            return;
        }
        if (!(check_user(userid) || check_user(password))) {
            errors();
            return;
        }
        std::vector<int> check;
        Userid.search(userid, check);
        if (!check.empty()) {
            errors();
            return;
        }
        Users::UserInfo new_user;
        to_string(password, new_user.password);
        to_string(userid, new_user.id);
        new_user.privilege = pri;
        new_user.save_place = ++user_save;
        user_file.seekp(sizeof(int) + (new_user.save_place - 1) * sizeof(Users::UserInfo));
        user_file.write(reinterpret_cast<char *>(&new_user), sizeof(Users::UserInfo));
        Userid.add(userid, new_user.save_place);
        std::string s;
        s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
        s += " add user ";
        s += userid;
        s += " privilege ";
        s += std::to_string(pri);
        log l(s);
        log_file.seekp(sizeof(int) + log_save*sizeof(log));
        log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
        log_save++;
    }

    void delete_user(std::string userid) {
        for (int i = 0; i < login_stack.size(); ++i) {
            if (back_to_string(login_stack[i].itself.id) == userid) {
                errors();
                return;
            }
        }
        if (login_stack.empty() || login_stack[login_stack.size() - 1].itself.privilege < 7) {
            errors();
            return;
        }
        std::vector<int> ans;
        Userid.search(userid, ans);
        if (ans.empty()) {
            errors();
            return;
        } else {
            Userid.erase(userid, ans[0]);
            std::string s;
            s += back_to_string(login_stack[login_stack.size() - 1].itself.id);
            s += " deleted ";
            s += userid;
            log l(s);
            log_file.seekp(sizeof(int) + log_save*sizeof(log));
            log_file.write(reinterpret_cast<char *>(&l), sizeof(log));
            log_save++;
        }
    }

    void modify_password(std::string id, std::string new_password, std::string current_password = "") {
        std::vector<int> res;
        Userid.search(id, res);
        if (res.empty()) {
            errors();
            return;
        }
        if (!check_user(new_password)) {
            errors();
            return;
        }
        if (login_stack.empty()) {
            errors();
            return;
        }
        Users info;
        user_file.seekg(sizeof(int) + (res[0] - 1) * sizeof(Users::UserInfo));
        user_file.read(reinterpret_cast<char *>(&info.itself), sizeof(Users::UserInfo));
        if (current_password == back_to_string(info.itself.password)) {
            to_string(new_password, info.itself.password);
            user_file.seekp(sizeof(int) + (res[0] - 1) * sizeof(Users::UserInfo));
            user_file.write(reinterpret_cast<char *>(&info.itself), sizeof(Users::UserInfo));
        } else if (current_password == "") {
            if (login_stack.empty()) {
                errors();
                return;
            } else if (login_stack[login_stack.size() - 1].itself.privilege == 7) {
                if (info.itself.privilege == login_stack[login_stack.size() - 1].itself.privilege) {
                    to_string(new_password, login_stack[login_stack.size() - 1].itself.password);
                }
                to_string(new_password, info.itself.password);
                user_file.seekp(sizeof(int) + (res[0] - 1) * sizeof(Users::UserInfo));
                user_file.write(reinterpret_cast<char *>(&info.itself), sizeof(Users::UserInfo));
            } else {
                errors();
                return;
            }
        } else {
            errors();
            return;
        }
    }

    void search_ISBN(std::string target_ISBN) {
        std::vector<int> ans;
        ISBN.search(target_ISBN, ans);
        std::set<Books::BookInfo> result;
        for (int i = 0; i < ans.size(); ++i) {
            Books::BookInfo b;
            book_file.seekg(sizeof(int) + (ans[i] - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&b), sizeof(Books::BookInfo));
            result.insert(b);
        }
        if (result.empty()) {
            std::cout << '\n';
            return;
        }
        for (auto iter = result.begin(); iter != result.end(); ++iter) {
            Books::BookInfo info;
            info = *iter;
            std::cout << back_to_string(info.ISBN) << '\t'
                      << back_to_string(info.name) << '\t'
                      << back_to_string(info.author) << '\t'
                      << back_to_string(info.keyword) << '\t';
            std::cout << std::fixed << std::setprecision(2) << info.price;
            std::cout << '\t' << info.quantity << '\n';
        }
    }

    void search_author(std::string target_author) {
        std::vector<int> ans;
        Author.search(target_author, ans);
        std::set<Books::BookInfo> result;
        for (int i = 0; i < ans.size(); ++i) {
            Books::BookInfo b;
            book_file.seekg(sizeof(int) + (ans[i] - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&b), sizeof(Books::BookInfo));
            result.insert(b);
        }
        if (result.empty()) {
            std::cout << '\n';
            return;
        }
        for (auto iter = result.begin(); iter != result.end(); ++iter) {
            Books::BookInfo info;
            info = *iter;
            std::cout << back_to_string(info.ISBN) << '\t'
                      << back_to_string(info.name) << '\t'
                      << back_to_string(info.author) << '\t'
                      << back_to_string(info.keyword) << '\t';
            std::cout << std::fixed << std::setprecision(2) << info.price;
            std::cout << '\t' << info.quantity << '\n';
        }
    }

    void search_name(std::string target_name) {
        std::vector<int> ans;
        Name.search(target_name, ans);
        std::set<Books::BookInfo> result;
        for (int i = 0; i < ans.size(); ++i) {
            Books::BookInfo b;
            book_file.seekg(sizeof(int) + (ans[i] - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&b), sizeof(Books::BookInfo));
            result.insert(b);
        }
        if (result.empty()) {
            std::cout << '\n';
            return;
        }
        for (auto iter = result.begin(); iter != result.end(); ++iter) {
            Books::BookInfo info;
            info = *iter;
            std::cout << back_to_string(info.ISBN) << '\t'
                      << back_to_string(info.name) << '\t'
                      << back_to_string(info.author) << '\t'
                      << back_to_string(info.keyword) << '\t';
            std::cout << std::fixed << std::setprecision(2) << info.price;
            std::cout << '\t' << info.quantity << '\n';
        }
    }

    void search_keyword(std::string target_keyword) {
        std::vector<int> ans;
        Keyword.search(target_keyword, ans);
        std::set<Books::BookInfo> result;
        for (int i = 0; i < ans.size(); ++i) {
            Books::BookInfo b;
            book_file.seekg(sizeof(int) + (ans[i] - 1) * sizeof(Books::BookInfo));
            book_file.read(reinterpret_cast<char *>(&b), sizeof(Books::BookInfo));
            result.insert(b);
        }
        if (result.empty()) {
            std::cout << '\n';
            return;
        }
        for (auto iter = result.begin(); iter != result.end(); ++iter) {
            Books::BookInfo info;
            info = *iter;
            std::cout << back_to_string(info.ISBN) << '\t'
                      << back_to_string(info.name) << '\t'
                      << back_to_string(info.author) << '\t'
                      << back_to_string(info.keyword) << '\t';
            std::cout << std::fixed << std::setprecision(2) << info.price;
            std::cout << '\t' << info.quantity << '\n';
        }
    }

    void cout_all() {
        book_file.seekg(sizeof(int));
        std::set<Books::BookInfo> result;
        for (int i = 0; i < book_save; ++i) {
            Books::BookInfo b;
            book_file.read(reinterpret_cast<char *>(&b), sizeof(Books::BookInfo));
            result.insert(b);
        }
        if (result.empty()) {
            std::cout << '\n';
            return;
        }
        for (auto iter = result.begin(); iter != result.end(); ++iter) {
            Books::BookInfo info;
            info = *iter;
            std::cout << back_to_string(info.ISBN) << '\t'
                      << back_to_string(info.name) << '\t'
                      << back_to_string(info.author) << '\t'
                      << back_to_string(info.keyword) << '\t';
            std::cout << std::fixed << std::setprecision(2) << info.price;
            std::cout << '\t' << info.quantity << '\n';
        }
    }

    void search_finance(int number = -1) {
        if (login_stack.empty() || login_stack[login_stack.size() - 1].itself.privilege < 7) {
            errors();
            return;
        }
        if (number == -1) {
            std::vector<Finance::record> result;
            if (finance_save == 0) {
                std::cout << "+ 0.00 - 0.00" << '\n';
                return;
            }
            for (int i = 0; i < finance_save; ++i) {
                Finance::record f;
                finance_file.seekg(sizeof(int) + (finance_save - 1 - i) * sizeof(Finance::record));
                finance_file.read(reinterpret_cast<char *>(&f), sizeof(Finance::record));
                result.push_back(f);
            }
            double income = 0;
            double cost = 0;
            for (int i = 0; i < result.size(); ++i) {
                if (result[i].money > 0) {
                    income += result[i].money;
                } else {
                    cost -= result[i].money;
                }
            }
            std::cout << "+ ";
            std::cout << std::fixed << std::setprecision(2) << income;
            std::cout << " - ";
            std::cout << std::fixed << std::setprecision(2) << cost;
            std::cout << '\n';
        } else {
            std::vector<Finance::record> result;
            if (number > finance_save) {
                errors();
                return;
            }
            for (int i = 0; i < number; ++i) {
                Finance::record f;
                finance_file.seekg(sizeof(int) + (finance_save - 1 - i) * sizeof(Finance::record));
                finance_file.read(reinterpret_cast<char *>(&f), sizeof(Finance::record));
                result.push_back(f);
            }
            double income = 0;
            double cost = 0;
            for (int i = 0; i < number; ++i) {
                if (result[result.size() - 1 - i].money > 0) {
                    income += result[result.size() - 1 - i].money;
                } else {
                    cost -= result[result.size() - 1 - i].money;
                }
            }
            std::cout << "+ ";
            std::cout << std::fixed << std::setprecision(2) << income;
            std::cout << " - ";
            std::cout << std::fixed << std::setprecision(2) << cost;
            std::cout << '\n';
        }
    }

    void printlog() {
        for (int i = 0;i < log_save;++i) {
            log_file.seekg(sizeof(int) + i * sizeof(log));
            log l;
            log_file.read(reinterpret_cast<char *>(&l), sizeof(log));
            l.print();
        }
    }
};

#endif //MAIN_CPP_BOOKSTORE_H
