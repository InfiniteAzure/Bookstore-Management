//
// Created by Xue Jiarui on 2023/1/1.
//

#ifndef MAIN_CPP_LOG_H
#define MAIN_CPP_LOG_H

class log {
public:
    char things[150] = {0};

    void print() {
        std::cout << back_to_string(things) << '\n';
    }

    log() = default;

    log(std::string s) {
        for (int i = 0;i < s.length();++i) {
            things[i] = s[i];
        }
        things[s.length()] = '\0';
    }

    ~log() {

    }
};

#endif //MAIN_CPP_LOG_H
