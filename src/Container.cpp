#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <sstream>  
#include <random>
#include <curl/curl.h>
#include <assert.h>
#include "Container.h"

namespace fre{
    
    void Container::printStockList(){
        for(auto it = stockList.cbegin(); it != stockList.cend(); ++it)
        {
            std::cout << it->first << "\n";
        }
       
    }
    
    void Container::sortStockList(){
        // comparison function to compare stocks based on earnings surpise 
        auto stockEstimateCompare = []( std::pair<std::string, Stock*>& a, 
                                        std::pair<std::string, Stock*>& b) -> bool {
            return a.second->getSurprisePercent() < b.second->getSurprisePercent();
        };
        
        // copy key-value pairs into a vector so we can sort
        std::vector<std::pair<std::string, Stock*>> vec;
        for (auto& elem : stockList) {
            vec.emplace_back(elem.first, &(elem.second));
        }
        std::sort(vec.begin(), vec.end(), stockEstimateCompare);
        
        /// split the sorted vec into three sections 
        cout<<vec.size();
        int n = vec.size() / 3;
        auto it1 = vec.begin();
        auto it2 = std::next(it1, n);
        auto it3 = std::next(it2, n); 
    
        // intialize each map
        while(it1 != it2){
            this->missEstimateGroup[it1->first] = &this->stockList[it1->first];
            it1++;
        }
        while(it2 != it3){
            this->meetEstimateGroup[it2->first] = &this->stockList[it2->first];
            it2++;
        }
        while(it3 != vec.end()){
            this->beatEstimateGroup[it3->first] = &this->stockList[it3->first];
            it3++;
        }
    }
    
    
    map<string,Stock*> Container::randomSelection(map<string,Stock*> group, int n)
    {
        std::vector<std::pair<std::string, Stock*>> vec(group.begin(), group.end());
        
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(vec.begin(), vec.end(), g);
        
        map<string,Stock*> randomGroup;
        for (int i = 0; i < n; ++i)
        {
            randomGroup.insert(vec[i]);
        }
        
        return randomGroup;
    }
    
    void Container::setDateRange(Stock& stock, int n) 
    {
        auto Date = this->getIndex()->getDatesString();
        auto earnings_dt = stock.getResultsData().getEarningsDate();
 
        tm timeStruct = {};
        const char* format = "%d-%b-%Y";
        strptime(earnings_dt.c_str(), format, &timeStruct);
        
        time_t time = mktime(&timeStruct);
        timeStruct = *localtime(&time);
    
        char buffer[80];
        const char* newFormat = "%Y-%m-%d";

        strftime(buffer, 80, newFormat, &timeStruct);
        string earnings_dt_formatted(buffer);
        
        auto itr = find(Date.begin(), Date.end(), earnings_dt_formatted);
        
        // if(itr == Date.end()) {
        //     cout<<"Index error \nEarnings date "<<earnings_dt<<" "<<earnings_dt_formatted;
        //     return ;}
            
        // // cout<<"here";    
        auto start = itr - n;
        auto end = itr + n;

        stock.setSDate(*start);
        stock.setEDate(*end);
    }
}