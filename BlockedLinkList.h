//
// Created by Xue Jiarui on 2023/1/1.
//

#ifndef MAIN_CPP_BLOCKEDLINKLIST_H
#define MAIN_CPP_BLOCKEDLINKLIST_H

bool check_keyword(std::string s) {
    if (s.length() > 60) {
        return false;
    }
    for (int i = 0;i < s.length();++i) {
        if (s[i] == '|' && s[i + 1] == '|') {
            return false;
        }
    }
    return true;
}

bool check_user(std::string s) {
    if (s.length() > 30) {
        return false;
    }
    for (int i = 0; i < s.length(); ++i) {
        if (!((s[i] <= '9' && s[i] >= '0') || (s[i] <= 'z' && s[i] >= 'a')
              || (s[i] <= 'Z' && s[i] >= 'A') || s[i] == '_')) {
            return false;
        }
    }
    return true;
}

double to_double(std::string s) {
    double ans = 0.0;
    int i = 0;
    while (s[i] != '.' && s[i] != '\0') {
        ans *= 10;
        ans += s[i] - '0';
        i++;
    }
    if (s[i] == '\0') {
        return ans;
    } else {
        i++;
        double place = 0.1;
        while (s[i] != '\0') {
            ans += place * (s[i] - '0');
            place *= 0.1;
            i++;
        }
    }
    return ans;
}

void keyword_process(std::string keywords, std::vector<std::string> &ans) {
    int i = 0;
    while (true) {
        std::string s;
        while (keywords[i] != '|' && keywords[i] != '\0') {
               s += keywords[i];
            i++;
        }
        ans.push_back(s);
        if (keywords[i] == '\0') {
            return;
        }
        i++;
    }
}

int to_int(std::string s) {
    int ans = 0;
    for (int i = 0; i < s.length(); ++i) {
        ans *= 10;
        ans += s[i] - '0';
    }
    return ans;
}

void errors() {
    std::cout << "Invalid" << '\n';
}

const int biggest = 300;

void to_string(std::string a, char b[]) {
    for (int i = 0; i < a.length(); ++i) {
        b[i] = a[i];
    }
    b[a.length()] = '\0';
}

std::string back_to_string(char a[]) {
    std::string ans;
    int i = 0;
    while (a[i] != '\0') {
        ans += a[i];
        i++;
    }
    return ans;
}

struct Index {
    int blocks;
    int first_node;
    int write_place;
};

struct Block {
    char index[65];
    int value;

    friend bool operator<(const Block &a, const Block &b) {
        if (std::string(a.index) != std::string(b.index)) {
            return std::string(a.index) < std::string(b.index);
        } else {
            return a.value < b.value;
        }
    }

    friend bool operator>(const Block &a, const Block &b) {
        if (std::string(a.index) != std::string(b.index)) {
            return std::string(a.index) > std::string(b.index);
        } else {
            return a.value > b.value;
        }
    }

    friend bool operator==(const Block &a, const Block &b) {
        return !(a > b || a < b);
    }
};

struct Node {
    int last_position = -1;
    int this_position = -1;
    int next_position = -1;
    int number;
    Block min;
    Block max;
    Block blocks[305];
};

class BlockedLinkList {
public:
    std::string file_name;
    std::fstream File;
    Index catalogue;

    BlockedLinkList(std::string file) {
        file_name = file;
        File.open(file);
        if (!File.good()) {
            std::ofstream out;
            out.open(file);
            catalogue.blocks = 0;
            catalogue.first_node = 1;
            catalogue.write_place = 0;
            out.write(reinterpret_cast<char *>(&catalogue), sizeof(catalogue));
            out.close();
            File.open(file);
        } else {
            File.seekg(0);
            File.read(reinterpret_cast<char *>(&catalogue), sizeof(catalogue));
        }
    }

    ~BlockedLinkList() {
        File.seekp(0);
        File.write(reinterpret_cast<char *>(&catalogue), sizeof(catalogue));
        File.close();
    }

    void add(std::string s, int a) {
        Block b;
        b.value = a;
        to_string(s, b.index);
        if (catalogue.blocks == 0) {
            //the very beginning
            catalogue.blocks = 1;
            catalogue.first_node = 1;
            catalogue.write_place = 2;
            Node n;
            n.max = b;
            n.min = b;
            n.number = 1;
            n.blocks[0] = b;
            n.this_position = 1;
            File.seekp(sizeof(catalogue));
            File.write(reinterpret_cast<char *>(&n), sizeof(Node));
            return;
        } else {
            //beginning
            Node n;
            File.seekg(sizeof(catalogue) + (catalogue.first_node - 1) * sizeof(Node));
            File.read(reinterpret_cast<char *>(&n), sizeof(Node));
            if (n.number < biggest / 2) {
                if (n.number == 0) {
                    n.blocks[0] = b;
                    n.min = b;
                    n.max = b;
                    n.number = 1;
                    File.seekp(sizeof(catalogue) + (catalogue.first_node - 1) * sizeof(Node));
                    File.write(reinterpret_cast<char *>(&n), sizeof(n));
                    return;
                }
                for (int i = 0; i < n.number; ++i) {
                    if (n.blocks[i] > b) {
                        if (i == 0) {
                            n.min = b;
                        }
                        for (int j = n.number - 1; j >= i; --j) {
                            n.blocks[j + 1] = n.blocks[j];
                        }
                        n.blocks[i] = b;
                        break;
                    } else if (i == n.number - 1) {
                        n.blocks[i + 1] = b;
                        n.max = b;
                    }
                }
                n.number++;
                File.seekp(sizeof(catalogue) + (catalogue.first_node - 1) * sizeof(Node));
                File.write(reinterpret_cast<char *>(&n), sizeof(n));
                return;
            } else {
                while (b > n.max && n.next_position != -1) {
                    File.seekg(sizeof(catalogue) + (n.next_position - 1) * sizeof(Node));
                    File.read(reinterpret_cast<char *>(&n), sizeof(Node));
                }
                if (b > n.max) {
                    //the last one
                    if (n.number < biggest - 1) {
                        n.max = b;
                        n.blocks[n.number] = b;
                        n.number++;
                        File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                        return;
                    } else {
                        n.blocks[biggest - 1] = b;
                        Node add;
                        n.number++;
                        n.number = biggest / 2;
                        n.max = n.blocks[biggest / 2 - 1];
                        n.next_position = catalogue.write_place;
                        add.next_position = -1;
                        add.last_position = n.this_position;
                        add.this_position = catalogue.write_place;
                        catalogue.write_place++;
                        catalogue.blocks++;
                        for (int i = 0; i < biggest / 2; ++i) {
                            add.blocks[i] = n.blocks[biggest / 2 + i];
                        }
                        add.min = add.blocks[0];
                        add.max = add.blocks[biggest / 2 - 1];
                        add.number = biggest / 2;
                        File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                        File.seekp(sizeof(catalogue) + (add.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&add), sizeof(Node));
                    }
                } else {
                    //not the very last;
                    if (b < n.min) {
                        n.min = b;
                    }
                    if (n.number < biggest - 1) {
                        for (int i = 0; i < n.number; ++i) {
                            if (n.blocks[i] > b) {
                                for (int j = n.number - 1; j >= i; --j) {
                                    n.blocks[j + 1] = n.blocks[j];
                                }
                                n.blocks[i] = b;
                                n.number++;
                                break;
                            }
                        }
                        File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                    } else {
                        for (int i = 0; i < n.number; ++i) {
                            if (n.blocks[i] > b) {
                                for (int j = n.number - 1; j >= i; --j) {
                                    n.blocks[j + 1] = n.blocks[j];
                                }
                                n.blocks[i] = b;
                                n.number++;
                                break;
                            }
                        }
                        Node add;
                        n.number = biggest / 2;
                        n.max = n.blocks[biggest / 2 - 1];
                        add.next_position = n.next_position;
                        add.last_position = n.this_position;
                        add.this_position = catalogue.write_place;
                        catalogue.write_place++;
                        catalogue.blocks++;
                        for (int i = 0; i < biggest / 2; ++i) {
                            add.blocks[i] = n.blocks[biggest / 2 + i];
                        }
                        add.max = add.blocks[biggest / 2 - 1];
                        add.min = add.blocks[0];
                        add.number = biggest / 2;
                        if (n.next_position != -1) {
                            Node change;
                            File.seekg(sizeof(catalogue) + (n.next_position - 1) * sizeof(Node));
                            File.read(reinterpret_cast<char *>(&change), sizeof(Node));
                            change.last_position = add.this_position;
                            File.seekp(sizeof(catalogue) + (change.this_position - 1) * sizeof(Node));
                            File.write(reinterpret_cast<char *>(&change), sizeof(Node));
                        }
                        n.next_position = add.this_position;
                        File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                        File.seekp(sizeof(catalogue) + (add.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&add), sizeof(Node));
                        return;
                    }
                }
            }
        }
    }

    void erase(std::string s, int a) {
        if (catalogue.blocks == 0) {
            return;
        }
        File.seekg(sizeof(catalogue));
        Node n;
        File.read(reinterpret_cast<char *>(&n), sizeof(Node));
        Block b;
        to_string(s, b.index);
        b.value = a;
        while (b > n.max && n.next_position != -1) {
            File.seekg(sizeof(catalogue) + (n.next_position - 1) * sizeof(Node));
            File.read(reinterpret_cast<char *>(&n), sizeof(Node));
        }
        if (b > n.max) {
            return;
        } else {
            for (int i = 0; i < n.number; ++i) {
                if (n.blocks[i] == b) {
                    if (i == 0) {
                        n.min = n.blocks[1];
                    } else if (i == n.number - 1) {
                        n.max = n.blocks[n.number - 2];
                    }
                    for (int j = i + 1; j < n.number; ++j) {
                        n.blocks[j - 1] = n.blocks[j];
                    }
                    n.number--;
                    break;
                }
                if (i == n.number - 1) {
                    return;
                }
            }
            if (n.number == biggest / 2 - 1) {
                if (n.this_position == 1) {
                    if (n.next_position != -1) {
                        Node lend;
                        File.seekg(sizeof(catalogue) + (n.next_position - 1) * sizeof(Node));
                        File.read(reinterpret_cast<char *>(&lend), sizeof(Node));
                        if (lend.number > biggest / 2) {
                            n.blocks[biggest / 2 - 1] = lend.blocks[0];
                            n.max = n.blocks[biggest / 2 - 1];
                            lend.min = lend.blocks[1];
                            lend.number--;
                            n.number++;
                            for (int i = 0; i < lend.number; ++i) {
                                lend.blocks[i] = lend.blocks[i + 1];
                            }
                            File.seekp(sizeof(catalogue) + (lend.this_position - 1) * sizeof(Node));
                            File.write(reinterpret_cast<char *>(&lend), sizeof(Node));
                            File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                            File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                            return;
                        } else {
                            for (int i = 0; i < biggest / 2; ++i) {
                                n.blocks[i + biggest / 2 - 1] = lend.blocks[i];
                            }
                            n.max = n.blocks[biggest - 2];
                            n.number = biggest - 1;
                            n.next_position = lend.next_position;
                            catalogue.blocks--;
                            if (n.next_position != -1) {
                                Node change;
                                File.seekg(sizeof(catalogue) + (lend.next_position - 1) * sizeof(Node));
                                File.read(reinterpret_cast<char *>(&change), sizeof(Node));
                                change.last_position = n.this_position;
                                File.seekp(sizeof(catalogue) + (change.this_position - 1) * sizeof(Node));
                                File.write(reinterpret_cast<char *>(&change), sizeof(Node));
                            }
                            File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                            File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                            return;
                        }
                    } else {
                        File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                        return;
                    }
                } else {
                    Node lend;
                    File.seekg(sizeof(catalogue) + (n.last_position - 1) * sizeof(Node));
                    File.read(reinterpret_cast<char *>(&lend), sizeof(Node));
                    if (lend.number == biggest / 2) {
                        lend.next_position = n.next_position;
                        for (int i = 0; i < biggest / 2 - 1; ++i) {
                            lend.blocks[biggest / 2 + i] = n.blocks[i];
                        }
                        lend.max = lend.blocks[biggest - 2];
                        lend.number = biggest - 1;
                        catalogue.blocks--;
                        if (lend.next_position != -1) {
                            Node change;
                            File.seekg(sizeof(catalogue) + (n.next_position - 1) * sizeof(Node));
                            File.read(reinterpret_cast<char *>(&change), sizeof(Node));
                            change.last_position = lend.this_position;
                            File.seekp(sizeof(catalogue) + (change.this_position - 1) * sizeof(Node));
                            File.write(reinterpret_cast<char *>(&change), sizeof(Node));
                        }
                        File.seekp(sizeof(catalogue) + (lend.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&lend), sizeof(Node));
                        return;
                    } else {
                        for (int i = n.number - 1; i >= 0; --i) {
                            n.blocks[i + 1] = n.blocks[i];
                        }
                        n.blocks[0] = lend.max;
                        n.number++;
                        lend.number--;
                        lend.max = lend.blocks[lend.number - 1];
                        n.min = n.blocks[0];
                        File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                        File.seekp(sizeof(catalogue) + (lend.this_position - 1) * sizeof(Node));
                        File.write(reinterpret_cast<char *>(&lend), sizeof(Node));
                        return;
                    }
                }
            } else {
                File.seekp(sizeof(catalogue) + (n.this_position - 1) * sizeof(Node));
                File.write(reinterpret_cast<char *>(&n), sizeof(Node));
                return;
            }
        }
    }

    void search(std::string s, std::vector<int> &ans) {
        if (catalogue.blocks == 0) {
            return;
        }
        Node n;
        File.seekg(sizeof(catalogue) + (catalogue.first_node - 1) * sizeof(Node));
        File.read(reinterpret_cast<char *>(&n), sizeof(Node));
        while (s > back_to_string(n.max.index) && n.next_position != -1) {
            File.seekg(sizeof(catalogue) + (n.next_position - 1) * sizeof(Node));
            File.read(reinterpret_cast<char *>(&n), sizeof(Node));
        }
        if (s > back_to_string(n.max.index)) {
            return;
        } else {
            while (n.next_position != -1 && back_to_string(n.max.index) == s) {
                for (int i = 0; i < n.number; ++i) {
                    if (back_to_string(n.blocks[i].index) == s) {
                        ans.push_back(n.blocks[i].value);
                    }
                }
                File.seekg(sizeof(catalogue) + (n.next_position - 1) * sizeof(Node));
                File.read(reinterpret_cast<char *>(&n), sizeof(Node));
            }
            for (int i = 0; i < n.number; ++i) {
                if (back_to_string(n.blocks[i].index) == s) {
                    ans.push_back(n.blocks[i].value);
                }
            }
        }
    }
};

#endif //MAIN_CPP_BLOCKEDLINKLIST_H
