//
// Created by Xue Jiarui on 2023/1/1.
//

#ifndef MAIN_CPP_PROCESSOR_H
#define MAIN_CPP_PROCESSOR_H

class processor {
public:
    std::string the_line;
    std::vector<std::string> cutter;
    int length;

    processor() = default;

    ~processor() {

    }

    void readin(std::string s) {
        cutter.clear();
        the_line = s;
        length = 0;
        std::string tmp;
        for (int i = 0; i < s.length(); ++i) {
            if (s[i] == ' ') {
                if (tmp != "") {
                    length++;
                    cutter.push_back(tmp);
                    tmp = "";
                }
            } else if (s[i] != '\r') {
                tmp += s[i];
            }
        }
        if (tmp != "") {
            cutter.push_back(tmp);
            length++;
        }
    }
};

#endif //MAIN_CPP_PROCESSOR_H
