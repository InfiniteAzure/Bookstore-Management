//
// Created by Xue Jiarui on 2023/1/1.
//

#ifndef MAIN_CPP_BOOKS_H
#define MAIN_CPP_BOOKS_H

class Books {
public:

    struct BookInfo {
        char ISBN[25] = {0};
        char author[65] = {0};
        char name[65] = {0};
        char keyword[65] = {0};
        int write_place;
        long long quantity = 0;
        double price = 0;

        friend bool operator<(BookInfo a, BookInfo b) {
            return back_to_string(a.ISBN) < back_to_string(b.ISBN);
        }

        friend bool operator>(BookInfo a, BookInfo b) {
            return back_to_string(a.ISBN) > back_to_string(b.ISBN);
        }

        friend bool operator==(BookInfo a, BookInfo b) {
            return !(a > b || a < b);
        }
    };

    BookInfo itself;

    Books() = default;

    ~Books() {};

};

#endif //MAIN_CPP_BOOKS_H
