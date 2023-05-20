#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include <curl/curl.h>
#include <map>
#include <chrono>
#include <ctime>
#include <thread>
#include <numeric>
#include <random>
#include <algorithm>
#include "Utils.h"

using namespace std;

namespace fre{

	const char* cIWB3000SymbolFile = "../data/Russell3000EarningsAnnouncements -3rdQuarter2022 (1).csv"; 
	const char* cIWB3000HistoricalData = "../data/Results.txt"; 
	
	static string url_common = "https://eodhistoricaldata.com/api/eod/";
    static string api_token = "644413b3447017.62256157"; 
    
	int write_data(void* ptr, int size, int nmemb, FILE* stream)
    {
    	size_t written;
    	written = fwrite(ptr, size, nmemb, stream);
    	return written;
    }
    struct MemoryStruct {
    	char* memory;
    	size_t size;
    };
    void* myrealloc(void* ptr, size_t size)
    {
    	if (ptr)
    		return realloc(ptr, size);
    	else
    		return malloc(size);
    }
    int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
    {
    	size_t realsize = size * nmemb;
    	struct MemoryStruct* mem = (struct MemoryStruct*)data;
    	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
    	if (mem->memory) {
    		memcpy(&(mem->memory[mem->size]), ptr, realsize);
    		mem->size += realsize;
    		mem->memory[mem->size] = 0;
    	}
    	return realsize;
    }
    
    void loadApiSingle(const char* file_path, 
            std::string& ticker, 
            Stock& stock, 
            CURL *handle, 
            int nDays,
            bool to_file)
    {
        // get the start date and end dat of the api request using days param
        auto earnings_date = stock.getResultsData().getEarningsDate();
        auto start_date = stock.getSDate();
        auto end_date = stock.getEDate();
        
        struct MemoryStruct data;
		data.memory = NULL;
		data.size = 0;
		
		string url_request = url_common + ticker + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
		curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
    
        if(to_file)
        {
            FILE* fp = fopen(file_path, "ab");
            fprintf(fp, "%s\n", ticker.c_str());
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
            auto result_file = curl_easy_perform(handle);
            fprintf(fp, "%c", '\n');
            fclose(fp);
    		
    		if (result_file != CURLE_OK)
    		{
    			// if errors have occured, tell us what is wrong with result
    			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result_file));
    		}
        }
		
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
		auto result = curl_easy_perform(handle);
				
		if (result != CURLE_OK)
		{
			// if errors have occured, tell us what is wrong with result
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
		}
		
		stringstream sData;
		sData.str(data.memory);
		string sValue, sDate;
		string line;
		
		// initalize empty data containers
		vector<vector<double>> stockInfo;
        stockInfo.resize(6);
        vector<long long> datetimeIndex;
        vector<string> datetimeIndexString;
        
        // parse the api response into 2d matrix
		while (getline(sData, line)) {
			size_t found = line.find('-');
			if (found != std::string::npos)
			{
				//parse thre row into the data continers
				size_t i = 0;
				istringstream iss_line(line);
				string field;
				
                while (std::getline(iss_line, field, ',')) 
                {
                    if(i > 0)
                    {
                        auto value = std::stof(field);
                        stockInfo[i-1].push_back(value); 
                    }
                    else
                    {
                        datetimeIndexString.push_back(field);
                        auto epoch_time = datetime_str_to_ns_epoch(field, "%Y-%m-%d");
                        datetimeIndex.push_back(epoch_time);
                    }
                    i++;
                }
			}
		}
		
		// load the matrix and datetime index into the stock
		stock.setDates(datetimeIndex);
		stock.setDatesString(datetimeIndexString);
        stock.setStockInfo(stockInfo);
        stock.isLoaded = true;
        
		free(data.memory);
		data.size = 0;
    }
    
    void loadApiMulti(int thread_id, map<string, Stock>* stocks, 
            int start, 
            int count, 
            int nDays,
            bool to_file)
    {
        // curl_easy_init() returns a CURL easy handle 
    	CURL* handle = curl_easy_init();
    	
        //adding a user agent
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
        
        //file path to write data to, make sure its empty
        std::string file_path = "../data/Results.txt"; 
        file_path.insert(15,std::to_string(thread_id));
        std::ofstream file(file_path.c_str(), std::ofstream::trunc);
    	file.close();
        
        for(int i = start; i < count + start; i++) 
        {
            auto datIt = stocks->begin();
            advance(datIt, i);
            string ticker = datIt->first;
            if (ticker == "" || ticker == "Ticker Not Found." || ticker == "FRC" || ticker == "LICY"){
                continue;
            }
            Stock& stock = datIt->second;
            
            cout << ticker << endl;
            loadApiSingle(file_path.c_str(), ticker, stock, handle, nDays, to_file);
        }
    }

    int loadApiData(Container &data, int nDays, bool to_file)
    {
        // get the current time
        auto start_time = std::chrono::high_resolution_clock::now();  
        
        // set up the program environment that libcurl needs 
    	curl_global_init(CURL_GLOBAL_ALL);
    
        map<string, Stock>* stocks = data.getStockList();

        for(auto it = stocks->begin();it!=stocks->end();it++)
            {auto ticker = it->first;
            if (ticker == "" || ticker == "Ticker Not Found." || ticker == "FRC" || ticker == "LICY"){
                continue;
            }
                data.setDateRange(it->second,nDays);}

        int thread_count = 4;
        int n = stocks->size();
        int segment_size = n / thread_count;
        int remaining_size = n % thread_count;
        int start = 0;
        
        // split the map up into 4 segments and create thread to handle each
        std::vector<std::thread> threads;
        cout << n << " stocks to load...\n";
        for (int i = 0; i < thread_count; i++) 
        {
            //get starting index and segment size
            int size = segment_size + (remaining_size > 0);
            remaining_size--;
            int end = start + size - 1;

            //create thread worker
            cout << "creating thread at index: " << start << " with count: " << size << endl;
            threads.push_back(std::thread(
                loadApiMulti, 
                i,              //thread id
                stocks,         //pointer to Container
                start,          //start position for the thread
                size,           //number of items to load
                nDays,          //earnings band size
                to_file         //write to file?
            ));
            start = end + 1;
        }

        for (auto& t : threads) {
            t.join();
        }
        
        // if no data was written to files return
        if(!to_file)
        {
            auto end = std::chrono::high_resolution_clock::now();  // get the current time again
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start_time).count();  // calculate the duration in microseconds
            std::cout << "data from api loaded in: " << duration << " seconds." << std::endl;
            return 1;
        }
        
        // open the output file
        std::ofstream outfile("../data/ResultsApi.txt");
        
        // join the indivual thread's files into one results file
        for(size_t i = 0; i < threads.size(); i++)
        {
            std::string file_path = "../data/Results.txt"; 
            file_path.insert(15,std::to_string(i));
            std::ifstream infile(file_path);
            
            std::string line;
            while (std::getline(infile, line)) {
                outfile << line << std::endl;
            }
    
            // Close the input file
            infile.close();
            
            /// delete the thread's result file
            std::remove(file_path.c_str());
        }
        
        auto end = std::chrono::high_resolution_clock::now();  // get the current time again
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start_time).count();  // calculate the duration in microseconds
        std::cout << "data from api loaded in: " << duration << " seconds." << std::endl;
        return 1;
    }
    
    void loadIndexData(Container& data_container, std::string start_date, std::string end_date)
    {
        struct MemoryStruct data;
		data.memory = NULL;
		data.size = 0;
		
		string url_common = "https://eodhistoricaldata.com/api/eod/";
        string api_token = "644413b3447017.62256157"; 
    	
    	CURL* handle = curl_easy_init();
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
        
		string url_request = url_common + "IWV" + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
		curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
		auto result = curl_easy_perform(handle);
				
		if (result != CURLE_OK)
		{
			// if errors have occured, tell us what is wrong with result
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
		}
		
		stringstream sData;
		sData.str(data.memory);
		string sValue, sDate;
		string line;
		
		// initalize empty data containers
		vector<vector<double>> stockInfo;
        stockInfo.resize(6);
        vector<long long> datetimeIndex;
        vector<string> datetimeIndexString;
        
        // parse the api response into 2d matrix
		while (getline(sData, line)) {
			size_t found = line.find('-');
			if (found != std::string::npos)
			{
				//parse thre row into the data continers
				size_t i = 0;
				istringstream iss_line(line);
				string field;
				
                while (std::getline(iss_line, field, ',')) 
                {
                    if(i > 0)
                    {
                        auto value = std::stof(field);
                        stockInfo[i-1].push_back(value); 
                    }
                    else
                    {
                        datetimeIndexString.push_back(field);
                        auto epoch_time = datetime_str_to_ns_epoch(field, "%Y-%m-%d");
                        datetimeIndex.push_back(epoch_time);
                    }
                    i++;
                }
			}
		}
		
		// load the matrix and datetime index into a stock
		auto index = data_container.getIndex();
		index->setDates(datetimeIndex);
		index->setDatesString(datetimeIndexString);
        index->setStockInfo(stockInfo);
        index->isLoaded = true;
        
		free(data.memory);
		data.size = 0;
    }


    
   
    // loading the results data from the file data/Russel3000EarningsAnnouncements.csv
    void loadResultsData(Container &container){
        cout << "Fetching earnings from csv file! \n";
        // using fstream object to read csv input file
        ifstream fin;
        fin.open(cIWB3000SymbolFile,ios::in);
        
        string line, ticker, earningsDate, periodEnding, estimated, reported, surprise, surprisePercent;
        map<string,Stock> data;
        
        getline(fin,line);
        
        while(!fin.eof()){
            getline(fin,line);
            stringstream sin(line);
            
            // splitting the input to different temp variables
            getline(sin,ticker,',');
            getline(sin,earningsDate,',');
            getline(sin,periodEnding,',');
            getline(sin,estimated,',');
            getline(sin,reported,',');
            getline(sin,surprise,',');
            getline(sin,surprisePercent,',');
            
            ResultsData resultsData;
            resultsData.setTicker(ticker);
            resultsData.setEarningsDate(earningsDate);
            resultsData.setEstimated(stod(estimated));
            resultsData.setReported(stod(reported));
            resultsData.setSurprise(stod(surprise));
            resultsData.setSurprisePercent(stod(surprisePercent));
            
            Stock stock;
            
            stock.setResultsData(resultsData);
            
            data[ticker] = stock;
        }
        
        container.setStockList(data);
        
        cout << "Earnings for " << data.size() << " tickers fetched! \n";
    }
    
    void loadHistoricalData(Container &data, bool from_api){
        auto start_time = std::chrono::high_resolution_clock::now();  

	    auto stocks = data.getStockList();
	    
	    std::string file_path;
	    if(from_api)
	    {
	        file_path = "../data/ResultsApi.txt";
	    }
	    else
	    {
	        file_path = cIWB3000HistoricalData;   
	    }
	    
	    ifstream file(file_path);
	    
        if (!file.is_open()) {
            cerr << "Failed to open file" << std::endl;
            return;
        }
        
        //read file into memory, split by new line
        vector<string> sections;
        sections.push_back("");
        string section;
        while (getline(file, section, '\n')) {
            if (section.empty()) {
                sections.push_back("");
            } else {
                sections.back() += section + '\n';
            }
        }
        
        // loop over each section corresponding to stock data
        for (size_t i = 0; i < sections.size(); i++) {
            //cout << i << endl;
            const auto& s = sections[i];
            std::istringstream iss(s);
            std::string line;
            
            // get ticker stored in first line
            std::getline(iss, line);
            auto ticker = line;
            
            //check for missing data (ticker = Ticker)
            if(ticker == "Ticker"){
                //skip one line, restart
                std::getline(iss, line);
                std::getline(iss, line);
                auto ticker = line;
            }
            // reached the end
            // FRC delisted so skipping, LICY erroneous earnings date so skipping
            if (ticker == "" || ticker == "Ticker Not Found." || ticker == "FRC" || ticker == "LICY"){
                continue;
            }

            vector<vector<double>> stockInfo;
            stockInfo.resize(6);
            vector<long long> datetimeIndex;
            vector<string> datetimeIndexString;
            
            // get headers stored in fecond line
            std::getline(iss, line);
    
            // loop over historical price data
            while (std::getline(iss, line)) 
            {
                vector<double> fields;
                istringstream iss_line(line);
                string field;
                
                size_t i = 0;
                while (std::getline(iss_line, field, ',')) 
                {
                    if(i > 0)
                    {
                        auto value = std::stof(field);
                        stockInfo[i-1].push_back(value); 
                    }
                    else
                    {
                        datetimeIndexString.push_back(field);
                        auto epoch_time = datetime_str_to_ns_epoch(field, "%Y-%m-%d");
                        datetimeIndex.push_back(epoch_time);
                    }
                    
                    i++;
                }
            }

            if(stocks->find(ticker) == stocks->end())
            {
                continue;
            }
            
            auto stock = &((*stocks)[ticker]);
            stock->setDates(datetimeIndex);
            stock->setDatesString(datetimeIndexString);
            stock->setStockInfo(stockInfo);
            stock->isLoaded = true;
        }
        file.close();
        
        auto end = std::chrono::high_resolution_clock::now();  // get the current time again
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time).count();  // calculate the duration in microseconds
        std::cout << "data from .txt loaded in: " << duration << " milliseconds." << std::endl;
        
	}
	long long datetime_str_to_ns_epoch(const string& date_str, const char * date_format){
        std::tm date = {};
        std::istringstream ss(date_str);
        ss >> std::get_time(&date, date_format);
    
        // Convert the std::tm structure to a std::chrono::system_clock::time_point
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(std::mktime(&date));
    
        // Convert to nanoseconds since epoch
        auto ns_epoch = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp).time_since_epoch().count();
        return ns_epoch;
    }
    
    vector<string> getSymbols(map<string,Stock*> group)
    {
        vector<string> symbols;
        
        for(map<string,Stock*>::iterator it = group.begin(); it != group.end(); ++it)
        {
            symbols.push_back(it->first);
        }
        
        return symbols;
    }
    
    string getStartDate(string date, int daysToSubtract) 
    {
        tm timeStruct = {};
        const char* format = "%d-%b-%Y";
        strptime(date.c_str(), format, &timeStruct);
        
        time_t time = mktime(&timeStruct) - (daysToSubtract * 24 * 60 * 60);
        timeStruct = *localtime(&time);
    
        char buffer[80];
        const char* newFormat = "%Y-%m-%d";

        strftime(buffer, 80, newFormat, &timeStruct);
        string startDate(buffer);
        return startDate;
    }
    
    string getEndDate(string date, int daysToAdd) 
    {
        tm timeStruct = {};
        const char* format = "%d-%b-%Y";
        strptime(date.c_str(), format, &timeStruct);
        
        time_t time = mktime(&timeStruct) + (daysToAdd * 24 * 60 * 60);
        timeStruct = *localtime(&time);
    
        char buffer[80];
        const char* newFormat = "%Y-%m-%d";

        strftime(buffer, 80, newFormat, &timeStruct);
        string endDate(buffer);
        return endDate;
    }
    
    string formatDate(string date)
    {
        tm timeStruct = {};
        const char* format = "%d-%b-%Y";
        strptime(date.c_str(), format, &timeStruct);
        
        char buffer[80];
        const char* newFormat = "%Y-%m-%d";
        
        strftime(buffer, 80, newFormat, &timeStruct);
        string newDate(buffer);
        return newDate;
    }
    
    long long getEarningsDateEpoch(Stock* stock)
    {
        tm timeStruct = {};
        const char* format = "%d-%b-%Y";
        strptime(stock->getResultsData().getEarningsDate().c_str(), format, &timeStruct);
        
        time_t currentTime = mktime(&timeStruct);

        std::chrono::system_clock::time_point timePoint =
            std::chrono::system_clock::from_time_t(currentTime);
    
        auto epochNanoseconds =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                timePoint.time_since_epoch()).count();
                
        return epochNanoseconds;
    }
    
    vector<double> AAR(vector<double> prices1, vector<double> prices2, vector<long long> date1, vector<long long> date2) {

    // Trim date2 and prices2 if necessary
    if (date1.size() < date2.size()) {
        long long min_date = date1[0];
        int trim_index = -1;
        for (size_t i = 0; i < date2.size(); i++) {
            if (date2[i] == min_date) {
                trim_index = i;
                break;
            }
        }
        if (trim_index != -1) {
            date2.erase(date2.begin(), date2.begin() + trim_index);
            prices2.erase(prices2.begin(), prices2.begin() + trim_index);
        }
    }

    // Compute the returns for both price vectors
    vector<double> returns1, returns2;
    for (size_t i = 1; i < prices1.size(); ++i) {
        double r1 = prices1[i] / prices1[i-1] - 1.0;
        double r2 = prices2[i] / prices2[i-1] - 1.0;
        returns1.push_back(r1);
        returns2.push_back(r2);
    }

    // Find the differences between the two return vectors
    Vector differences;
    differences = returns1-returns2;

    // Return the differences vector
    return differences;
    }


    vector<double> CAR(vector<double>& differences) {
        // Compute the cumulative sum of the differences vector
        vector<double> cumulativeDifferences;
        double cumulativeSum = 0.0;
        for (size_t i = 0; i < differences.size(); ++i) {
            cumulativeSum += differences[i];
            cumulativeDifferences.push_back(cumulativeSum);
        }
    
        // Return the cumulative differences vector
        return cumulativeDifferences;
    }
    vector<double> colMean(const vector<vector<double>>& data) {
        size_t num_cols = 0;
        for (const auto& vec : data) {
            num_cols = max(num_cols, vec.size());
        }
        vector<double> means(num_cols, 0.0);
        vector<size_t> counts(num_cols, 0);
        for (const auto& vec : data) {
            for (size_t i = 0; i < vec.size(); i++) {
                if (!isnan(vec[i])) {
                    means[i] += vec[i];
                    counts[i]++;
                }
            }
        }
        for (size_t i = 0; i < num_cols; i++) {
            if (counts[i] > 0) {
                means[i] /= counts[i];
            }
        }
        return means;
    }
    
 array<vector<double>, 4> bootstrap(map<string, Stock*>& Group, vector<double> index_adjusted_close, vector<long long> index_date) {
    vector<vector<double>> aar3;
    vector<vector<double>> car3;

    for (size_t i = 0; i < 40; i++) {
        vector<vector<double>> aar2;
        vector<vector<double>> car2;
        vector<string> stockSymbols;
        for (auto it = Group.begin(); it != Group.end(); ++it) {
            stockSymbols.push_back(it->first);
        }
        random_device rd;
        mt19937 g(rd());
        shuffle(stockSymbols.begin(), stockSymbols.end(), g);
   
        // Process the first 80 shuffled stocks
        int count = 0;

        for (auto symbol : stockSymbols) {
            auto stk1 = Group[symbol];
            if (!stk1->isLoaded) {
                continue;
            }
            auto adjustedClose = stk1->getAdjustedClose();
            if (adjustedClose.empty()) {
                cerr << "Skipping stock " << symbol << " because it has no price data." << endl;
                continue;
            }
            auto date1 = stk1->getDates();
            if (date1.empty()) {
                cerr << "Skipping stock " << symbol << " because it has no date data." << endl;
                continue;
            }
            if (adjustedClose.size() != date1.size()) {
                cerr << "Skipping stock " << symbol << " because price and date data do not match." << endl;
                continue;
            }
   
            vector<double> stockPrices1 = adjustedClose;
            vector<double> priceDifferences1 = AAR(stockPrices1, index_adjusted_close, date1, index_date);
            vector<double> cumulativePriceDiff1 = CAR(priceDifferences1);
            aar2.push_back(priceDifferences1);
            car2.push_back(cumulativePriceDiff1);
            count++;
            if (count == 80) {
                break;
            }
        }
        aar3.push_back(colMean(aar2));
        car3.push_back(colMean(car2));
    }
    
    vector<double> aarStdDev = colStdDev(aar3);
    vector<double> carStdDev = colStdDev(car3);
    
    vector<double> AAR_vec = colMean(aar3);
    vector<double> CAR_vec = colMean(car3);

    return {AAR_vec, CAR_vec, aarStdDev, carStdDev};
}

vector<double> colStdDev(const vector<vector<double>>& matrix) {
    vector<double> result;
    
    size_t numColumns = matrix[0].size();

    for (size_t col = 0; col < numColumns; ++col) {
        std::vector<double> column;
        for (const auto& row : matrix) {
            column.push_back(row[col]);
        }
        double standardDeviation = calculate_std_dev(column);
        result.push_back(standardDeviation);
    }

    return result;
}
    
double calculate_std_dev(const std::vector<double>& vec) {
    double sum = 0.0, mean, stddev = 0.0;
    int n = vec.size();
 
    for(auto& val : vec) {
        sum += val;
    }
 
    mean = sum / n;
 
    for(auto& val : vec) {
        stddev += pow(val - mean, 2);
    }
 
    return sqrt(stddev / n);
}

void plotResults(const vector<double> vector1, const vector<double> vector2, const vector<double> vector3,int N)
{

    FILE *gnuplotPipe,*tempDataFile;

    int d = (int)vector1.size();
    std::vector<std::vector<double>> plot_matrix(4);
    for(int i=0;i<(int)plot_matrix.size();i++)
        {plot_matrix[i].resize(d+1);}

    for (int j = 0; j < 2*N; j++)
    {
        plot_matrix[0][j] = vector1[j];
        plot_matrix[1][j] = vector2[j];
        plot_matrix[2][j] = vector3[j];
    }
    for (const auto& value : plot_matrix[0])
    {
        cout << value << " ";
    }

    std::vector<const char*> tempDataFileNames = {"Beat", "Meet", "Miss"};

    //set up the gnu plot
    gnuplotPipe = popen("/usr/bin/gnuplot", "w");
    fprintf(gnuplotPipe,"set terminal png size 800,600; set output 'Results.png'\n");
    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title 'Avg CAAR for 3 groups'\n");
    fprintf(gnuplotPipe, "set arrow from 0,graph(0,0) to 0,graph(1,1) nohead lc rgb 'red'\n");
    fprintf(gnuplotPipe, "set xlabel 'Announcement Date'\nset ylabel 'Avg CAAR (%)'\n");

    if (gnuplotPipe) 
    {

        fprintf(gnuplotPipe,"plot \"%s\" with lines lw 3, \"%s\" with lines lw 3, \"%s\" with lines lw 3 \n", tempDataFileNames[0],tempDataFileNames[1],tempDataFileNames[2]);
        fflush(gnuplotPipe);
        for (int i = 0; i < tempDataFileNames.size(); i++) 
        {
            
            //plot figure
            tempDataFile = fopen(tempDataFileNames[i],"w");
            for (int j = -N; j < N; j++)
            {
                fprintf(tempDataFile, "%i %lf\n", j, plot_matrix[i][j+N]);
            }
            fclose(tempDataFile);
        }

    } 
    else 
    {        
        printf("gnuplot not found...");    
    }

    std::cout << "Vector 1: ";
    for (const auto& value : vector1)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    std::cout << "Vector 2: ";
    for (const auto& value : vector2)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    std::cout << "Vector 3: ";
    for (const auto& value : vector3)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

    
}


