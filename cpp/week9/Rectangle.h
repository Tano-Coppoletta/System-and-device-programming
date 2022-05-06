//
// Created by tanoc on 03/05/2022.
//

#ifndef WEEK9_RECTANGLE_H
#define WEEK9_RECTANGLE_H


class Rectangle {
private:
    int m_length;
    int m_width;
public:

    Rectangle operator+(const Rectangle &to_be_added);
    void operator=(const Rectangle);
    bool operator==(const Rectangle &to_be_compared);
    const Rectangle operator++(int);
};


#endif //WEEK9_RECTANGLE_H
