#include <curl/curl.h>

#include "Container.h"
#include "Matrix.h"

using namespace std;

namespace fre{
    long long datetime_str_to_ns_epoch(const string& date_str, const char * date_format);
    string getStartDate(string date, int daysToSubtract);
    string getEndDate(string date, int daysToAdd);
    string formatDate(string date);
    long long getEarningsDateEpoch(Stock* stock);

    

    int loadApiData(Container &data, int nDays, bool to_file);
    void loadIndexData(Container &data, std::string start_date, std::string end_date);
    
    void loadResultsData(Container &data);
    void loadHistoricalData(Container &data, bool from_api);
    
    vector<string> getSymbols(map<string,Stock*> group);
    
    vector<double> AAR(vector<double> prices1, vector<double> prices2, vector<long long> date1, vector<long long> date2);
    vector<double> CAR(vector<double>& differences);
    
    vector<double> colMean(const vector<vector<double>>& data);
    vector<double> colStdDev(const vector<vector<double>>& matrix);
    
    array<vector<double>,4> bootstrap(map<std::string, Stock*>& Group, vector<double> index_adjusted_close, vector<long long> index_date);
    
    double calculate_std_dev(const std::vector<double>& vec);
    void plotResults(const vector<double> vector1, const vector<double> vector2, const vector<double> vector3,int N);


    
}