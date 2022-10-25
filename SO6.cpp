#include <algorithm>
#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <bitset>
#include <iterator>
#include <stdint.h>
#include "Z2.hpp"
#include "SO6.hpp"

/**
 * Method to compare two Z2 arrays of length 6 lexicographically
 * @param first array of Z2 of length 6
 * @param second array of Z2 of length 6
 * @return -1 if first < second, 0 if equal, 1 if first > second
 */
bool lexLess(Z2 first[6], Z2 second[6])
{
    for (int8_t i = 0; i < 6; i++)
    {
        if (first[i] != second[i])
            return second[i] < first[i]; // Maybe changing the ordering?
    }
    return false;
}

/**
 * @brief Method to avoid multiple calls to lexLess when we need to lexicographically compare two strings
 * It doesn't seem like this will hit 0, since this only used when sorting, I think...
 *
 * @param first
 * @param second
 * @return int8_t
 */
int8_t lexComp(const Z2 first[6], const Z2 second[6])
{
    for (int i = 0; i < 6; i++)
    {
        if (first[i] == second[i])
            continue; // Presently, this is the longest time consumed, but it's faster than just doing inequalities.
        if (second[i] < first[i])
            return -1;
        return 1;
    }
    return 0;
}

/**
 * Basic constructor. Initializes Zero matrix.
 *
 */
SO6::SO6()
{
}

// /**
//  * Constructor that initializes arbitrary matrix with arbitrary name
//  * @param t the object history
//  */
// SO6::SO6(std::vector<int8_t> t){
//     // for(int8_t i=0; i<6; i++){
//     //     for(int8_t j=0; j<6; j++)
//     //         arr[i][j]=Z2();
//     // }
//     // hist = t;
// }

// Something much faster than this would be a "multiply by T" method that explicitly does the matrix multiplication given a particular T matrix instead of trying to compute it naively

/**
 * Overloads the * operator with matrix multiplication for SO6 objects
 * @param other reference to SO6 to be multiplied with (*this)
 * @return matrix multiplication of (*this) and other
 */
SO6 SO6::operator*(SO6 &other)
{
    // multiplies operators assuming COLUMN,ROW indexing
    SO6 prod;
    Z2 next;

    // Compute product
    for (int col = 0; col < 6; col++)
    {
        for (int row = 0; row < 6; row++)
        {
            for (int k = 0; k < 6; k++)
            {
                if (arr[k][row][0] == 0 || other[col][k][0] == 0)
                    continue;                       // Skip zeros
                next = arr[k][row] * other[col][k]; // This not transpose * other
                prod[col][row] += next;
            }
        }
    }
    prod.fixSign(); // Try to do this while computing the product? doesn't seem to make anything faster
    prod.lexOrder();
    return prod;
}

SO6 SO6::tMultiply(const int& i) {
            if(i<5)    return tMultiply(0,i+1);
            if(i<9)    return tMultiply(1,i-3);
            if(i<12)   return tMultiply(2,i-6);
            if(i<14)   return tMultiply(3,i-8);
            return tMultiply(4,5);
}

SO6 SO6::tMultiply(const int &i,const int &j)
{
    SO6 prod = *this;

    // Compute product
    switch (i-j) {
    case -1: 
        for(int col = 0; col < 6; col++) {
            prod[col][i] = (arr[col][i] - arr[col][j]);
            prod[col][i].increaseDE();
            prod[col][j] = (arr[col][j] + arr[col][i]);
            prod[col][j].increaseDE();
        }
        prod.fixSign(); // Try to do this while computing the product? doesn't seem to make anything faster
        prod.lexOrder();
        return prod;
        break;
    case 1:
        for(int col = 0; col < 6; col++) {
            prod[col][i] = (arr[col][i] - arr[col][j]);
            prod[col][i].increaseDE();
            prod[col][j] = (arr[col][j] + arr[col][i]);
            prod[col][j].increaseDE();
        }
        prod.fixSign(); // Try to do this while computing the product? doesn't seem to make anything faster
        prod.lexOrder();
        return prod;
        break;
    default:
        for(int col = 0; col < 6; col++) {
            prod[col][i] = (arr[col][i] + arr[col][j]);
            prod[col][i].increaseDE();
            prod[col][j] = (arr[col][j] - arr[col][i]);
            prod[col][j].increaseDE();
        }
        prod.fixSign(); // Try to do this while computing the product? doesn't seem to make anything faster
        prod.lexOrder();
        return prod;
        break;
    }
}

void SO6::fixSign()
{
    for (int col = 0; col < 6; col++)
    {
        for (int row = 0; row < 6; row++)
        {
            if (arr[col][row][0] == 0)
                continue;
            if (arr[col][row] < 0)
            {
                while (row < 6)
                    arr[col][row++].negate();
            }
            break;
        }
    }
}

// This may be slow, I think that I sped this up somehow by not sorting things
// but instead giving a permutation?
void SO6::lexOrder()
{
    Z2 *myZ2[] = {arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]};
    std::vector<Z2 *> myvector(myZ2, myZ2 + 6);
    std::sort(myvector.begin(), myvector.end(), lexLess);
    Z2 arr2[6][6];
    for (int8_t i = 0; i < 6; i++)
    {
        for (int8_t j = 0; j < 6; j++)
        {
            arr2[i][j] = (myvector.at(i))[j];
        }
    }
    for (int8_t i = 0; i < 6; i++)
    {
        for (int8_t j = 0; j < 6; j++)
        {
            arr[i][j] = arr2[i][j];
        }
    }
}

bool SO6::operator<(const SO6 &other) const
{
    for (int col = 0; col < 5; col++)
    { // There is no need to check the final column due to constraints
        switch (lexComp((*this)[col], other[col]))
        {
        case -1:
            return true;
        case 1:
            return false;
        }
    }
    return false;
}

/** overloads == method to check equality of SO6 matrices
 *  @param other reference to SO6 to be checked against
 *  @return whether or not (*this) and other are equivalent
 */
bool SO6::operator==(SO6 &other)
{
    // SO6 are the same if they have the same triangle
    // TODO: lower right triangle seems super fast, but can try out others
    for (int8_t col = 5; col > -1; col--)
    {
        for (int8_t row = 5; row > 5 - col - 1; row--)
        {
            if (arr[col][row] != other[col][row])
                return false;
        }
    }
    return true;
}

bool SO6::operator==(const SO6 &other) const
{
    // SO6 are the same if they have the same triangle
    // TODO: lower right triangle seems super fast, but can try out others
    for (int col = 5; col > -1; col--)
    {
        for (int row = 5; row > 5 - col - 1; row--)
        {
            if (arr[col][row] < other[col][row] || other[col][row] < arr[col][row])
                return false;
        }
    }
    return true;
}

/**
 * Overloads << function for SO6.
 * @param os reference to ostream object needed to implement <<
 * @param m reference to SO6 object to be displayed
 * @returns reference ostream with the matrix's display form appended
 */
std::ostream &operator<<(std::ostream &os, const SO6 &m)
{
    os << "\n";
    for (int row = 0; row < 6; row++)
    {
        os << '[';
        for (int col = 0; col < 6; col++)
            os << m[col][row] << ' ';
        os << "] \n";
    }
    os << "\n";
    return os;
}