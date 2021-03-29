/**
 *
 * shifty quadtree (pa3)
 * sqtree.cpp
 * This file will be used for grading.
 *
 */

#include "sqtree.h"

// First Node constructor, given.
SQtree::Node::Node(pair<int, int> ul, int w, int h, RGBAPixel a, double v)
        : upLeft(ul), width(w), height(h), avg(a), var(v), NW(NULL), NE(NULL), SE(NULL), SW(NULL) {}

// Second Node constructor, given
SQtree::Node::Node(stats &s, pair<int, int> ul, int w, int h)
        : upLeft(ul), width(w), height(h), NW(NULL), NE(NULL), SE(NULL), SW(NULL) {
    avg = s.getAvg(ul, w, h);
    var = s.getVar(ul, w, h);
}

// SQtree destructor, given.
SQtree::~SQtree() {
    clear();
}

// SQtree copy constructor, given.
SQtree::SQtree(const SQtree &other) {
    copy(other);
}

// SQtree assignment operator, given.
SQtree &SQtree::operator=(const SQtree &rhs) {
    if (this != &rhs) {
        clear();
        copy(rhs);
    }
    return *this;
}

/**
 * SQtree constructor given tolerance for variance.
 */
SQtree::SQtree(PNG &imIn, double tol) {

    int w = imIn.width();
    int h = imIn.height();
    stats s(imIn);
    pair<int, int> ul = make_pair(0, 0);
    root = buildTree(s, ul, w, h, tol);

}

/**
 * Helper for the SQtree constructor.
 */
SQtree::Node *SQtree::buildTree(stats &s, pair<int, int> &ul,
                                int w, int h, double tol) {

    RGBAPixel avg = s.getAvg(ul, w, h);
    Node *node = new Node(s, ul, w, h);

    double minAvgVar;
    pair<int, int> splitter;

    if (s.getVar(ul, w, h) <= tol || (w == 1 && h == 1)) return node;

    for (int x = ul.first + 1; x <= ul.first + w; x++) {
        for (int y = ul.second + 1; y <= ul.second + h; y++) {
            double varNW = s.getVar(ul, x, y);
            double varNE = s.getVar(make_pair(x - 1, ul.second), w - x, y);
            double varSW = s.getVar(make_pair(ul.first, y - 1), x, h - y);
            double varSE = s.getVar(make_pair(x - 1, y - 1), w - x, h - y);

            double avgVar = (varNW + varNE + varSW + varSE) / 4;

            if (avgVar < minAvgVar) {
                minAvgVar = avgVar;
                splitter.first = x;
                splitter.second = y;
            }
        }
    }

    node->NW = buildTree(s, ul, splitter.first, splitter.second, tol);
    node->NE = buildTree(s, make_pair(splitter.first - 1, ul.second), w - splitter.first, splitter.second, tol);
    node->SW = buildTree(s, make_pair(ul.first, splitter.second - 1), splitter.first, h - splitter.second, tol);
    node->SE = buildTree(s, make_pair(splitter.first - 1, splitter.second - 1), w - splitter.first, h - splitter.second,
                         tol);

}

/**
 * Render SQtree and return the resulting image.
 */
PNG SQtree::render() {

    if (root != NULL) {
        PNG result = new PNG(root->width, root->height);
        return renderHelper(root, result);
    }
}

PNG SQtree::renderHelper(Node *curr, PNG &img) {

    if (curr != NULL) {
        if (curr->NW == NULL && curr->NE == NULL && curr->SW == NULL && curr->SE == NULL) {
            for (int x = curr->upLeft.first; x < curr->upLeft.first + curr->width; x++) {
                for (int y = curr->upLeft.second; y < curr->upLeft.second + curr->height; y++) {
                    img.getPixel() = curr->avg;
                }
            }
            return img;
        }

        renderHelper(curr->NW, img);
        renderHelper(curr->NE, img);
        renderHelper(curr->SW, img);
        renderHelper(curr->SE, img);
    }
}

/**
 * Delete allocated memory.
 */
void SQtree::clear() {
   clear_helper(root);
}

void SQtree::clear_helper(Node *curr){
    if(curr != NULL){
        clear_helper(curr->NW);
        clear_helper(curr->NE);
        clear_helper(curr->SE);
        clear_helper(curr->SW);
        delete(curr);
    }
}

void SQtree::copy(const SQtree &other) {
    copy_helper(other->root, root);
}

void SQtree::copy_helper(Node* other, Node* curr) {
    curr = copy(other);
    if(other->NW != NULL){
        copy_helper(other->NW, curr->NW);
    }
    if(other->NE != NULL){
        copy_helper(other->NE, curr->NE);
    }
    if(other->SE != NULL){
        copy_helper(other->SE, curr->SE);
    }
    if(other->SW != NULL){
        copy_helper(other->SW, curr->SW);
    }
}

int SQtree::size() {
    return getSize(root);
}

int SQtree::getSize(Node *curr) {
    if (curr == NULL) {
        return 0;
    } else {
        return 1 + getSize(curr->NW) + getSize(curr->NE) + getSize(curr->SE) + getSize(curr->SW);
    }
}