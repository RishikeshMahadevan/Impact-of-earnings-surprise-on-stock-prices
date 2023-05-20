#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <chrono>
#include <assert.h>
#include <numeric>
#include <random>
#include <algorithm>
#include <memory>

#include "curl/curl.h"
#include "Utils.h"

#include <chrono>
using namespace std::chrono;

using namespace std;
using namespace fre;

int main(){
    
    Container data;
    loadResultsData(data);

    // load data from ResultsApi.txt
    // loadHistoricalData(data, true);
    loadIndexData(data, "2022-04-01","2023-05-05");
    

    
    auto stocks = data.getStockList();
    Stock* index = data.getIndex();
    std::vector<double> index_adjusted_close = index->getAdjustedClose();
    vector<long long>index_date=index->getDates();

int x, N, int_1, test=0, group;
    string ticker;
    map<string,Stock*> miss, beat, meet;
    Vector beat_car_mean, meet_car_mean, miss_car_mean;
    vector<vector<vector<double>>> combinedResults;
    
     while(true)
    {
    cout<<"\t \t \t :Menu: "<<endl;
    cout<<"1) Enter N to retrieve 2N+1 historical prices for all stocks"<<endl;
    cout<<"2) Pull information for one stock from one group (Daily prices, Cumulative returns etc.)"<<endl;
    cout<<"3) Show AAR, AAR-std, CAAR, CAAR-std for one group"<<endl;
    cout<<"4) Show the gnuplot graph with CAAR for all 3 groups"<<endl;
    cout<<"5) Exit program"<<endl<<endl;
    cout<<"Please select appropriate option number:  "<<endl;;
    cin>>x;
    
            switch(x)
        {
            case 1:
            {
                cout << "Option 1 selected" << endl;
                cout << "Set N for all stocks - " << endl;
                while(test==0)
                {
                    cout<<"Enter N value between 60 and 90: "<<endl;
                    cin>>N;
                    int_1 = (int) N;
                                  
                    if (int_1 != N)
                    {
                        cout<<"Error, please enter an integer value"<<endl;
                        cin.clear();
                    }
                    else if(N>=60 && N<=90 && !cin.fail())  // n is an int 
                    {
                        cout<<"Check for case working cout "<<endl;
                        // object.load data for stocks
                        int successs = loadApiData(data, N, false);
                        // object.display
                        cout<<"Stock Data pulled!"<<endl;
                        test =1;
                        //break;
                    }
                    else
                    {
                        cout<<"Error, Enough data points not found. Please select N above 60 and below 90 "<<endl;
                        cin.clear();
                    }
                }
                break;
            }
            case 2:
            {   
                if (N>=60 && N<=90)
                {
                    while(test==0)
                    {
                        cout<<"Please provide ticker of stock: "<<endl;
                        cin>> ticker;
                        
                        if(stocks->find(ticker) != stocks->end())
                        {
                            (*stocks)[ticker].displayDetails(index->getAdjustedClose(), index->getDates());
                        }
                        else
                        {
                            cout<<"Please enter a valid ticker\n";
                            test = 1;
                        }
                        
                        
                        if(!cin.fail())
                        {
                            cout<<"check case 2 working "<<ticker<<endl;
                            // print_v(v);
                            test = 1;
                        }
                        else 
                        { 
                            cout<<"Error, Invalid Input field. Example: Try GOOG ";   // try and error c++ 
                        }
                    }
                }
                else
                {
                    cout<<"Data has not yet been retrieved! Try retrieving data first using Option 1 in Main Menu!"<<endl;
                }
                break;
            }
            case 3:
            {
                data.sortStockList();
                
                auto beatEstimateGroup = data.getBeatEstimateGroup();
                auto meetEstimateGroup = data.getMeetEstimateGroup();
                auto missEstimateGroup = data.getMissEstimateGroup();
                
                cout <<endl<<"Bootstrap object created" << endl;
                cout << "beat size: " << beatEstimateGroup.size() << endl;
                cout << "meet size: " << meetEstimateGroup.size() << endl;
                cout << "miss size: " << missEstimateGroup.size() << endl;
                
                array<vector<double>, 4> aar_car_beat = bootstrap(beatEstimateGroup, index_adjusted_close, index_date);
                vector<double> beat_aar_mean = aar_car_beat[0];
                beat_car_mean = aar_car_beat[1];
                vector<double> beat_aar_std_dev = aar_car_beat[2];
                vector<double> beat_car_std_dev = aar_car_beat[3];
                
                array<vector<double>, 4> aar_car_meet = bootstrap(meetEstimateGroup, index_adjusted_close, index_date);
                vector<double> meet_aar_mean = aar_car_meet[0];
                meet_car_mean = aar_car_meet[1];
                vector<double> meet_aar_std_dev = aar_car_meet[2];
                vector<double> meet_car_std_dev = aar_car_meet[3];
                
                array<vector<double>, 4> aar_car_miss = bootstrap(missEstimateGroup, index_adjusted_close, index_date);
                vector<double> miss_aar_mean = aar_car_miss[0];
                miss_car_mean = aar_car_miss[1];
                vector<double> miss_aar_std_dev = aar_car_miss[2];
                vector<double> miss_car_std_dev = aar_car_miss[3];
                
                for (int i = 0; i < 3; i++) {
                    if (i == 0) {
                        // "beat" row
                        combinedResults.push_back( { beat_aar_mean,beat_car_mean,beat_aar_std_dev,beat_car_std_dev} );
                    } else if (i == 1) {
                        // "meet" row
                        combinedResults.push_back( { meet_aar_mean,meet_car_mean,meet_aar_std_dev,meet_car_std_dev } );
                    }else {
                        // "miss" row
                        combinedResults.push_back( { miss_aar_mean,miss_car_mean,miss_aar_std_dev,miss_car_std_dev } );
                    }
                }

                if(N>=60 && N<=90)
                {
                    while(test==0)
                    {
                        cout<<"1) Beat \t 2) Meet \t 3) Miss"<<endl;
                        cout<< "Please select appropriate group number: "<<endl; 
                        cin>>group;
                        int integer2 = (int) group;
                        
                        if(integer2 != group)
                        {
                            cout<<"Invalid Input field"<<endl;
                            cin.clear();
                            
                        }
                        else if(group == 1)   //switch?
                        {
                            cout<< "Beat Estimate Group summary "<<endl;
                            cout<< combinedResults[0]<< endl;;
                            test = 1;
                        }
                        else if(group == 2)
                        {
                            cout<< "Meet Estimate Group summary "<<endl;
                            cout<< combinedResults[1] <<endl;
                            test = 1;
                        }
                        else if(group == 3)
                        {
                            cout<< "Miss Estimate Group summary "<<endl;
                           cout<< combinedResults[2]<<endl;
                            test = 1;
                        }
                        else 
                        {
                            cout<<"Error, Please select valid option number"<<endl;
                        }
                    
                    }
 
                    
                }
                
                else
                {
                    cout<<"Data has not yet been retrieved! Try retrieving data first using Option 1 in Main Menu!"<<endl;
                }
                break;
            }
            case 4:
            {   
                if(N>=60 && N<=90)
                {
                    
                    cout<<"in 4"<<endl;
                    plotResults( beat_car_mean, meet_car_mean, miss_car_mean, (int)N);
                    
                }
                else
                {
                    cout<<"Data has not yet been retrieved! Try retrieving data first using Option 1 in Main Menu!"<<endl;
                }
                break;
            }
            case 5:
            {
                exit(0);
            }
            default: 
                cout<<"Error, Please select valid option "<< endl;
                cin.clear();
                test = 1;
                break;
            
        }
        test=0;
    }

return 0;
    
}