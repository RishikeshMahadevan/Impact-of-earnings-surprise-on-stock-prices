#pragma once

#include <vector>
#include <iostream>
#include <map>

#include "ResultsData.h"
#include "Matrix.h"

using namespace std;

namespace fre{
    
    typedef vector<double> Vector;
    typedef vector<Vector> Matrix;
    
    static int adjustedCloseColumn = 4;
    
    vector<string> generateDates();
    class Stock{
        
        protected:
            
            // stock price detail features
            vector< long long > Dates;
            vector<string> dateString;
            vector<double> pctReturns;
            vector<double> abnormalReturns;
            vector<double> abnormalCumulativeReturns;
            Matrix stockInfo;
            
            // stock earnings features
            ResultsData resultsData;

            string sDate, eDate;
            
        public:
            // default and parameterized constructor
            Stock(){}
            Stock( vector< long long > Dates_, Matrix stockInfo_, vector<string> dateString_){
                this->Dates = Dates_;
                this->stockInfo = stockInfo_;
                this->dateString = dateString_;
            }
            bool isLoaded = false;
            
            // getters and setter for stock price detail features ( features from the API )
            vector< long long >const & getDates () const { return Dates; }
            vector< string >const & getDatesString () const { return dateString; }
            Vector const & getAdjustedClose ();
            Vector getPctReturns () const { return pctReturns; }
            Vector const & getAbnormalReturns () const { return abnormalReturns; }
            Vector const & getAbnormalCumulativeReturns () const { return abnormalCumulativeReturns; }
            Matrix const & getStockInfo () const { return stockInfo; }
            
            string getSDate() { return sDate; }
            string getEDate() { return eDate; }
            
            void setDates (const vector< long long > Dates_ ) { Dates = Dates_;}
            void setDatesString (const vector< string > dateString_ ) { dateString = dateString_;}
            void setAdjustedClose (const Vector adjustedClose_) { this->stockInfo[adjustedCloseColumn] = adjustedClose_ ;}
            void setStockInfo (Matrix stockInfo_) { this->stockInfo = stockInfo_;}
            
            void setSDate(string start) {sDate=start; }
            void setEDate(string end) {eDate=end; }


            //  getter and setter for results data features
            
            ResultsData getResultsData() const { return resultsData; }
            double getSurprisePercent() {return resultsData.getSurprisePercent();}
            void setResultsData( ResultsData &resultsData_) { resultsData = resultsData_ ; }
        
            
            // compute functions
            // q 3.c
            void calculateReturns();
            void calculateAbnormalReturns (Stock* index);

            //  display functions
            void displayDetails(Vector index_adj_close, vector<long long> dt);
            
            
    };

}