#include <vector>
#include <algorithm>
#include <assert.h>
#include "curl/curl.h"

#include "Stock.h"
#include "Utils.h"

using namespace std;


namespace fre{
    Vector const & Stock::getAdjustedClose()
    {
        if(!this->isLoaded)
        {
            throw std::runtime_error("stock data must be laoded first");
        }
        return this->stockInfo[adjustedCloseColumn]; 
    }
    
    void Stock::calculateReturns()
    {
        //get the row index of the datetime t0 passed

        this->pctReturns.clear();
        
        //loop over closing prices, swapping as we go
        vector<double> adjustedClose = this->getAdjustedClose();
        double previous_close = adjustedClose[0];
        for(size_t i = 1; i < (int) adjustedClose.size(); i++){
            //get current close
            auto current_close = adjustedClose[i];
            
            //get daily returns
            this->pctReturns.push_back(100 * (current_close  / previous_close) - 100);
            
            //swap current close to previous
            previous_close = current_close;
        }
    }
        
    void Stock::displayDetails(Vector index_adj_close, vector<long long> dt){
        
        cout<<"Ticker: "<<this->getResultsData().getTicker()<<endl;
        cout<<"Earning Announcement Date: "<<this->getResultsData().getEarningsDate()<<endl;
        cout<<"Earning Estimate: "<<this->getResultsData().getEstimated()<<endl;
        cout<<"Reported Earning Per Share: "<<this->getResultsData().getReported()<<endl;
        cout<<"Earning Surprise: "<<this->getResultsData().getSurprise()<<endl;
        cout<<"Earning Surprise Percentage: "<<this->getResultsData().getSurprisePercent()<<endl;

        vector<string> d = this->getDatesString();
        cout<<"Dates: "<<endl<<d<<endl;
        
        //printing abnormal Return
        Vector a = AAR(this->getAdjustedClose(), index_adj_close, this->getDates(), dt);
        cout<<"Abnormal Returns: "<<endl<<a<<endl;

        
        //printing daily Return
        this->calculateReturns();
        Vector Ret = this->getPctReturns();
        cout<<"Dailty Returns: "<<endl<< Ret <<endl;

        Vector adjustedClose = this->getAdjustedClose();
        cout << "Adjusted Close: "<< endl << adjustedClose <<endl;
    }

}
