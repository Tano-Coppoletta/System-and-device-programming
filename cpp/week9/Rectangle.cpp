//
// Created by tanoc on 03/05/2022.
//

#include "Rectangle.h"

Rectangle Rectangle::operator+(const Rectangle &to_be_added) {
    Rectangle output;
    output.m_length=this->m_length+to_be_added.m_length;
    output.m_width=this->m_width=to_be_added.m_width;

    return output;
}

bool Rectangle::operator==(const Rectangle &to_be_compared) {
    return ((this->m_width==to_be_compared.m_width) && this->m_length==to_be_compared.m_length);
}

const Rectangle Rectangle::operator++(int) {
    Rectangle output;
    output.m_length=this->m_length+1;
    output.m_width=this->m_width+1;
    return output;
}