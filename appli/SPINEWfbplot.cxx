/*
  Author: Juan Carlos Prieto, SPINE execution of Weighted functional box plots
  Date: 07/10/14
*/

#include <stdlib.h>
#include "wfbplot.h"
#include "gaussweight.h"
using namespace std;

int main(int argv, char **argc)
{

    matrix* data;
    int pDim = 0;
    int nData = 0;

    if(argc[1]){
        for(unsigned i = 0; i < argv; i++){
            string inputstring = argc[i];

            if(inputstring.compare("--row")==0){
                inputstring = argc[i+1];
                pDim = 3*atoi(inputstring.c_str());
            }
            if(inputstring.compare("--col")==0){
                inputstring = argc[i+1];
                nData = atoi(inputstring.c_str());
            }
            if(inputstring.compare("--data")==0){
                data = new matrix(nData, pDim);
                for(unsigned row = 0; row < nData; row++){
                    for(unsigned col = 0; col < pDim; col++){
                        i++;
                        inputstring = argc[i];
                        double coord = atof(inputstring.c_str());
                        data->setElement(row, col, coord);
                    }
                }
            }
        }

    }else{

        while(!cin.eof()){
            string inputstring;
            cin >> inputstring;

            if(inputstring.compare("--row")==0){
                cin >> inputstring;
                pDim = 3*atoi(inputstring.c_str());
            }
            if(inputstring.compare("--col")==0){
                cin >> inputstring;
                nData = atoi(inputstring.c_str());
            }
            if(inputstring.compare("--data")==0){
                data = new matrix(nData, pDim);
                for(unsigned i = 0; i < nData; i++){
                    for(unsigned j = 0; j < pDim; j++){
                        cin >> inputstring;
                        double coord = atof(inputstring.c_str());
                        data->setElement(i, j, coord);
                    }
                }
            }
        }
    }


    wfbplot wfbTest(*data);
    wfbTest.print();

    double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
    wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);
    wfbTest.printVector(pDim, med);
    wfbTest.printVector(pDim, inf);
    wfbTest.printVector(pDim, sup);
    wfbTest.printVector(pDim, minBd);
    wfbTest.printVector(pDim, maxBd);
    wfbTest.printVector(nData, depth);
    delete data;

    // built atlas using weighted functional boxplot
    //cout<<" --double sigma = 30;
    /*for(int id=0; id < nData; id++)
    {
        //double weight[nData];
        // compute weight
        //gaussweight::computeModifiedGaussian( ages, nData, ages[id], sigma, ageRange[0], ageRange[1], weight);
        //wfbTest.setWeight(weight);
        //wfbTest.printVector(nData, weight);
        double med[pDim], inf[pDim], sup[pDim], minBd[pDim], maxBd[pDim], depth[nData];
        // call weight functional boxplot function
        wfbTest.computeBoxplot(med, inf, sup, minBd, maxBd, depth);
        wfbTest.printVector(nData, depth);
        wfbTest.printVector(pDim, med);
        wfbTest.printVector(pDim, inf);
        wfbTest.printVector(pDim, sup);
        wfbTest.printVector(pDim, minBd);
        wfbTest.printVector(pDim, maxBd);
    }*/

    return EXIT_SUCCESS;
}



