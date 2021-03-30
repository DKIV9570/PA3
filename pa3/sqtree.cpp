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

    Node *node = new Node(s, ul, w, h);
//    cout << "w: " << w << "h: " << h << "ul: [" << ul.first << "," << ul.second << "]" << endl;
    if (s.getVar(ul, w, h) <= tol || (w == 1 && h == 1)) {
        return node;
    }

    pair<int, int> splitter = make_pair(ul.first, ul.second);
    double minMaxVar = 10000000;

    for (int x = ul.first; x < ul.first + w; x++) {
        for (int y = ul.second; y < ul.second + h; y++) {
            if (!((x == ul.first && y == ul.second) || (x == ul.first + w && y == ul.second + h)
                  || (x == ul.first && y == ul.second + h) || (x == ul.first + w && y == ul.second))) {
                double varNW;
                double varNE;
                double varSW;
                double varSE;

                if (x == ul.first) {
                    varNW = -1;
                    varSW = -1;
                    varSE = s.getVar(make_pair(x, y), w, h - (y-ul.second));
                    varNE = s.getVar(make_pair(x, ul.second), w, y - ul.second);

                } else if (x == ul.first + w) {
                    varNE = -1;
                    varSE = -1;
                    varNW = s.getVar(ul, w, y - ul.second);
                    varSW = s.getVar(make_pair(ul.first, y), x - ul.first, h - (y-ul.second));

                } else if (y == ul.second + h) {
                    varSW = -1;
                    varSE = -1;
                    varNW = s.getVar(ul, x - ul.first, y - ul.second);
                    varNE = s.getVar(make_pair(x, ul.second), w - (x-ul.first), y - ul.second);
                } else if (y == ul.second) {
                    varNW = -1;
                    varNE = -1;
                    varSW = s.getVar(make_pair(ul.first, y), x - ul.first, h - (y-ul.second));
                    varSE = s.getVar(make_pair(x, y), w - (x-ul.first), h - (y-ul.second));
                } else {
                    varNW = s.getVar(ul, x - ul.first, y - ul.second);
                    varNE = s.getVar(make_pair(x, ul.second), w - (x-ul.first), y - ul.second);
                    varSW = s.getVar(make_pair(ul.first, y), x - ul.first, h - (y-ul.second));
                    varSE = s.getVar(make_pair(x, y), w - (x-ul.first), h - (y-ul.second));
                }

                varNW = max(varNW, -varNW);
                varNE = max(varNE, -varNE);
                varSW = max(varSW, -varSW);
                varSE = max(varSE, -varSE);

                double maxVar = max(max(max(varNW, varNE), varSW), varSE);

//                cout << "curr spli: " << x << " " << y << endl;
//                cout << varNW << " " << varNE << " " << varSW << " " << varSE << endl;
//                cout << "maxVAr: " << maxVar << endl << "minMaxVar: " << minMaxVar << endl;
//                cout << "best spli: " << splitter.first << " " << splitter.second << endl;
//                cout << endl;

                if (maxVar < minMaxVar) {
                    minMaxVar = maxVar;
                    splitter.first = x;
                    splitter.second = y;
                }
            }
        }
    }

//    cout << "result:" << splitter.first << " " << splitter.second << endl;

    pair<int, int> ul4 = make_pair(splitter.first, splitter.second);
    node->SE = buildTree(s, ul4, w - (ul4.first - ul.first), h - (ul4.second - ul.second), tol);

    if (splitter.first != ul.first) {
        pair<int, int> ul3 = make_pair(ul.first, splitter.second);
        node->SW = buildTree(s, ul3, splitter.first - ul.first, h - (ul3.second - ul.second), tol);
    }

    if (splitter.second != ul.second) {
        pair<int, int> ul2 = make_pair(splitter.first, ul.second);
        node->NE = buildTree(s, ul2, w - (ul2.first - ul.first), splitter.second - ul.second, tol);
    }

    if (splitter.first != ul.first && splitter.second != ul.second) {
        node->NW = buildTree(s, ul, splitter.first - ul.first, splitter.second - ul.second, tol);
    }

    return node;
}

/**
 * Render SQtree and return the resulting image.
 */
PNG SQtree::render() {

    if (root != NULL) {
        PNG *result = new PNG(root->width, root->height);
        vector<Node*> todo;
        todo.push_back(root);
        while(!todo.empty()){
            Node *curr = todo.back();
            todo.pop_back();
            if(curr->NW != NULL){
                todo.push_back(curr->NW);
            }
            if(curr->NE != NULL){
                todo.push_back(curr->NE);
            }
            if(curr->SW != NULL){
                todo.push_back(curr->SW);
            }
            if(curr->SE != NULL){
                todo.push_back(curr->SE);
            }
            if (curr->NW == NULL && curr->NE == NULL && curr->SW == NULL && curr->SE == NULL) {
                for (int x = curr->upLeft.first; x < curr->upLeft.first + curr->width; x++) {
                    for (int y = curr->upLeft.second; y < curr->upLeft.second + curr->height; y++) {
                        RGBAPixel *p = result->getPixel(x, y);
                        *p = curr->avg;
                    }
                }
            }
        }
        return *result;
    }
}

PNG SQtree::render_helper(Node *curr, PNG &img) {
    cout << "1" << endl;
    if (curr != NULL) {
        if (curr->NW == NULL && curr->NE == NULL && curr->SW == NULL && curr->SE == NULL) {
            for (int x = curr->upLeft.first; x < curr->upLeft.first + curr->width - 1; x++) {
                for (int y = curr->upLeft.second; y < curr->upLeft.second + curr->height - 1; y++) {
                    RGBAPixel *p = img.getPixel(x, y);
                    p = &curr->avg;
                }
            }
            return img;
        }
        cout << "2" << endl;
        img = render_helper(curr->NW, img);
        img = render_helper(curr->NE, img);
        img = render_helper(curr->SW, img);
        img = render_helper(curr->SE, img);
    }
}

/**
 * Delete allocated memory.
 */
void SQtree::clear() {
    clear_helper(root);
}

void SQtree::clear_helper(Node *curr) {
    if (curr != NULL) {
        clear_helper(curr->NW);
        clear_helper(curr->NE);
        clear_helper(curr->SE);
        clear_helper(curr->SW);
        delete (curr);
        curr = NULL;
    }
}

void SQtree::copy(const SQtree &other) {
    copy_helper(other.root, root);
}

void SQtree::copy_helper(Node *other, Node *curr) {
    curr = other;
    if (other->NW != NULL) {
        copy_helper(other->NW, curr->NW);
    }
    if (other->NE != NULL) {
        copy_helper(other->NE, curr->NE);
    }
    if (other->SE != NULL) {
        copy_helper(other->SE, curr->SE);
    }
    if (other->SW != NULL) {
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