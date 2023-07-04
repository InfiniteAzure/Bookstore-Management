//
// Created by Xue Jiarui on 2023/1/1.
//

#ifndef MAIN_CPP_USERS_H
#define MAIN_CPP_USERS_H

class Users {
public:

    struct UserInfo {
        char id[35] = {0};
        char password[35] = {0};
        char username[35] = {0};
        int privilege;
        int save_place;
        bool selected = false;
        Books::BookInfo selected_book;
    };

    UserInfo itself;

    Users() = default;

    ~Users() {

    }

    void init(std::string username, std::string password, std::string id, int pri, int save) {
        to_string(id, this->itself.id);
        to_string(password, this->itself.password);
        to_string(username, this->itself.username);
        this->itself.privilege = pri;
        this->itself.save_place = save;
    }

    bool change_password(std::string username, std::string old_password, std::string new_password) {
        if (back_to_string(this->itself.password) == old_password) {
            to_string(new_password, this->itself.password);
            return true;
        } else {
            return false;
        }
    }

};

#endif //MAIN_CPP_USERS_H
