#include <vector>
#include <map>

#include "Stock.h"

using namespace std;

namespace fre{
    class Container{
        
        private:
            Stock index;
            map<string, Stock> stockList;
            
            map<string,Stock*> beatEstimateGroup;
            map<string,Stock*> missEstimateGroup;
            map<string,Stock*> meetEstimateGroup;
            
        public: 
            Container() {}
            Container(map<string, Stock> stockList_): stockList(stockList_){}
            map<string, Stock>* getStockList() { return &stockList; } 
            Stock* getIndex(){return &this->index;}
            
            void setStockList( map<string, Stock> &stockList_ ) {  stockList = stockList_; } 
            void printStockList();
            void sortStockList();
            void loadIndexData(std::string start_date, std::string end_date);
            
            void setDateRange(Stock& stock, int n);
            
            map<string,Stock*> const & getMissEstimateGroup(){return this->missEstimateGroup;};
            map<string,Stock*> const & getMeetEstimateGroup(){return this->meetEstimateGroup;};
            map<string,Stock*> const & getBeatEstimateGroup(){return this->beatEstimateGroup;};
            
            map<string,Stock*> randomSelection(map<string,Stock*> group, int n);
            
        

    };
}