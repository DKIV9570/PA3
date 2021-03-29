#include "stats.h"


#include "stats.h"

stats::stats(PNG &im) {

    for (int x = 0; x < im.width(); x++) {

        int height = im.height();
        vector<long> sumRCol(height);
        vector<long> sumBCol(height);
        vector<long> sumGCol(height);
        vector<long> sumsqRCol(height);
        vector<long> sumsqGCol(height);
        vector<long> sumsqBCol(height);

        sumRed.push_back(sumRCol);
        sumGreen.push_back(sumGCol);
        sumBlue.push_back(sumBCol);
        sumsqRed.push_back(sumsqRCol);
        sumsqGreen.push_back(sumsqGCol);
        sumsqBlue.push_back(sumsqBCol);

        for (int y = 0; y < im.height(); y++) {

            RGBAPixel *pixel = im.getPixel(x, y);

            if (x > 0 && y > 0) {
                sumRed[x][y] = sumRed[x - 1][y] + sumRed[x][y - 1] - sumRed[x - 1][y - 1] + (long) pixel->r;
                sumGreen[x][y] = sumGreen[x - 1][y] + sumGreen[x][y - 1] - sumGreen[x - 1][y - 1] + (long) pixel->g;
                sumBlue[x][y] = sumBlue[x - 1][y] + sumBlue[x][y - 1] - sumBlue[x - 1][y - 1] + (long) pixel->b;
                sumsqRed[x][y] = sumsqRed[x - 1][y] + sumsqRed[x][y - 1] - sumsqRed[x - 1][y - 1] +
                                 (long) pow((double) pixel->r, 2);
                sumsqGreen[x][y] = sumsqGreen[x - 1][y] + sumsqGreen[x][y - 1] - sumsqGreen[x - 1][y - 1] +
                                   (long) pow((double) pixel->g, 2);
                sumsqBlue[x][y] = sumsqBlue[x - 1][y] + sumsqBlue[x][y - 1] - sumsqBlue[x - 1][y - 1] +
                                  (long) pow((double) pixel->b, 2);
            } else if (x > 0 && y == 0) {
                sumRed[x][y] = sumRed[x - 1][y] + (long) pixel->r;
                sumGreen[x][y] = sumGreen[x - 1][y] + (long) pixel->g;
                sumBlue[x][y] = sumBlue[x - 1][y] + (long) pixel->b;
                sumsqRed[x][y] = sumsqRed[x - 1][y] + (long) pow((double) pixel->r, 2);
                sumsqGreen[x][y] = sumsqGreen[x - 1][y] + (long) pow((double) pixel->g, 2);
                sumsqBlue[x][y] = sumsqBlue[x - 1][y] + (long) pow((double) pixel->b, 2);
            } else if (x == 0 && y > 0) {
                sumRed[x][y] = sumRed[x][y - 1] + (long) pixel->r;
                sumGreen[x][y] = sumGreen[x][y - 1] + (long) pixel->g;
                sumBlue[x][y] = sumBlue[x][y - 1] + (long) pixel->b;
                sumsqRed[x][y] = sumsqRed[x][y - 1] + (long) pow((double) pixel->r, 2);
                sumsqGreen[x][y] = sumsqGreen[x][y - 1] + (long) pow((double) pixel->g, 2);
                sumsqBlue[x][y] = sumsqBlue[x][y - 1] + (long) pow((double) pixel->b, 2);
            } else {
                sumRed[x][y] = (long) pixel->r;
                sumGreen[x][y] = (long) pixel->g;
                sumBlue[x][y] = (long) pixel->b;
                sumsqRed[x][y] = (long) pow((double) pixel->r, 2);
                sumsqGreen[x][y] = (long) pow((double) pixel->g, 2);
                sumsqBlue[x][y] = (long) pow((double) pixel->b, 2);
            }
        }
    }
}


long stats::getSum(char channel, pair<int, int> ul, int w, int h) {

    vector <vector<long>> *vect;

    switch (channel) {
        case 'r':
            vect = &sumRed;
            break;
        case 'g':
            vect = &sumGreen;
            break;
        case 'b':
            vect = &sumBlue;
            break;
    }

    if (ul.first > 0 && ul.second > 0)
        return (*vect)[ul.first + w - 1][ul.second + h - 1]
               - (*vect)[ul.first - 1][ul.second + h - 1]
               - (*vect)[ul.first + w - 1][ul.second - 1]
               + (*vect)[ul.first - 1][ul.second - 1];

    else if (ul.first == 0 && ul.second > 0)
        return (*vect)[ul.first + w - 1][ul.second + h - 1] - (*vect)[ul.first + w - 1][ul.second - 1];

    else if (ul.first > 0 && ul.second == 0)
        return (*vect)[ul.first + w - 1][ul.second + h - 1] - (*vect)[ul.first - 1][ul.second + h - 1];

    else return (*vect)[ul.first + w - 1][ul.second + h - 1];

}

long stats::getSumSq(char channel, pair<int, int> ul, int w, int h) {

    vector <vector<long>> *vect;

    switch (channel) {
        case 'r':
            vect = &sumsqRed;
            break;
        case 'g':
            vect = &sumsqGreen;
            break;
        case 'b':
            vect = &sumsqBlue;
            break;
    }

    if (ul.first > 0 && ul.second > 0)
        return (*vect)[ul.first + w - 1][ul.second + h - 1]
               - (*vect)[ul.first - 1][ul.second + h - 1]
               - (*vect)[ul.first + w - 1][ul.second - 1]
               + (*vect)[ul.first - 1][ul.second - 1];

    else if (ul.first == 0 && ul.second > 0)
        return (*vect)[ul.first + w - 1][ul.second + h - 1] - (*vect)[ul.first + w - 1][ul.second - 1];

    else if (ul.first > 0 && ul.second == 0)
        return (*vect)[ul.first + w - 1][ul.second + h - 1] - (*vect)[ul.first - 1][ul.second + h - 1];

    else return (*vect)[ul.first + w - 1][ul.second + h - 1];

}

// given a rectangle, compute its sum of squared deviations from mean, over all color channels.
// see written specification for a description of this function.
double stats::getVar(pair<int, int> ul, int w, int h) {

    return getVarHelper('r', ul, w, h) + getVarHelper('g', ul, w, h) + getVarHelper('b', ul, w, h);

}

double stats::getVarHelper(char channel, pair<int, int> ul, int w, int h) {

    return getSumSq(channel, ul, w, h) - ((getSum(channel, ul, w, h) * getSum(channel, ul, w, h)) / (w * h));

}

RGBAPixel stats::getAvg(pair<int, int> ul, int w, int h) {

    long rav = getSum('r', ul, w, h) / (w * h);
    long gav = getSum('g', ul, w, h) / (w * h);
    long bav = getSum('b', ul, w, h) / (w * h);

    return RGBAPixel(rav, gav, bav);

}
