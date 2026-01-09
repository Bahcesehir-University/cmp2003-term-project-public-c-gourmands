#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;             
    long long count;
};

class TripAnalyzer {
    private:
        std::unordered_map<std::string, long long> m_zoneCounts;
        std::unordered_map<std::string, std::vector<long long>> m_hourlyCounts;
        
        bool extractHour(const std::string &ts, int &hour) {
            std::size_t pos = ts.find(' ');
            if (pos == std::string::npos || pos + 2 >= ts.size()) return false;
            
            char h1 = ts[pos + 1];
            char h2 = ts[pos + 2];
            
            if (!isdigit(h1) || !isdigit(h2)) return false;
            
            hour = (h1 - '0') * 10 + (h2 - '0');
            return (hour >= 0 && hour <= 23);
        }
        
    public:
		void ingestFile(const std::string& filepath);
		
        std::vector<ZoneCount> topZones(int k = 10) const;
        std::vector<SlotCount> topBusySlots(int k = 10) const;
};
