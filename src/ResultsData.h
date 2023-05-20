#include <vector>
#include <string>

using namespace std;

namespace fre{
    class ResultsData{
    
        protected:
            string ticker;
            string startDate;
            string endDate;
            string earningsDate; // earnings date
            
            // Earnings results
            double estimated;
            double reported;
            double surprise;
            double surprisePercent;    
        
        public:
            // default and parameterized constructor
            ResultsData() {}
            ResultsData( string ticker_ , string startDate_ , string endDate_ , string periodEnding_ , double estimated_ , double reported_ , double surprise_ , double surprisePercent_): ticker (ticker_ ),startDate (startDate_ ),endDate (endDate_ ),earningsDate (periodEnding_ ),estimated (estimated_ ),reported (reported_ ),surprise (surprise_ ),surprisePercent (surprisePercent_ ){};
            
            // getters and setters
            
            string getTicker () { return ticker; }
            string getStartDate () { return startDate; }
            string getEndDate () { return endDate; }
            string getEarningsDate () { return earningsDate; }
            double getEstimated () { return estimated; }
            double getReported () { return reported; }
            double getSurprise () { return surprise; }
            double getSurprisePercent () { return surprisePercent; }
            
            void setTicker ( const string &ticker_) { ticker = ticker_ ; }
            void setEarningsDate ( const string &periodEnding_) { earningsDate = periodEnding_ ; }
            void setEstimated ( const double &estimated_) { estimated = estimated_ ; }
            void setReported ( const double &reported_) { reported = reported_ ; }
            void setSurprise ( const double &surprise_) { surprise = surprise_ ; }
            void setSurprisePercent ( const double &surprisePercent_) { surprisePercent = surprisePercent_ ; }
            int setStartAndEndDates();
    };
}