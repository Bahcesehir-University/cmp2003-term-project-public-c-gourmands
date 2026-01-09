#include "analyzer.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>

void TripAnalyzer::ingestFile(const std::string& filepath) {
    std::ifstream file(filepath);
	if (!file.is_open())
		return;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::size_t p[6];
        p[0] = 0;
        
        for (int i = 1; i < 6; ++i) {
            p[i] = line.find(',', p[i - 1]);
            if (p[i] == std::string::npos) goto skip;
            ++p[i];
        }
        
        {
            // PickupZone ID
            std::string zone = line.substr(p[1], p[2] - p[1] - 1);
            if (zone.empty()) goto skip;
            
            // PickupDateTime
            std::string ts = line.substr(p[3], p[4] - p[3] - 1);
            
            int hour;
            if (!extractHour(ts, hour)) goto skip;
            
            ++m_zoneCounts[zone];
            
            auto &arr = m_hourlyCounts[zone];
            if (arr.empty()) arr.resize(24, 0);
            arr[hour]++;
        }
        
    skip:
        continue;
    }
}


std::vector<ZoneCount> TripAnalyzer::topZones(int k) const 
{
    std::vector<ZoneCount> result;
    result.reserve(m_zoneCounts.size());
    
    for (const auto &pair : m_zoneCounts)
        result.push_back({pair.first, pair.second});
    
    std::sort(result.begin(), result.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    });
    
    if ((int)result.size() > k) result.resize(k);
    return result;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const
{
    std::vector<SlotCount> result;
    
    for (const auto& pair : m_hourlyCounts) {
        const std::string& zone = pair.first;
        const std::vector<long long>& hours = pair.second;
        
        for (int h = 0; h < 24; ++h) {
            if (hours[h] > 0) {
                result.push_back({zone, h, hours[h]});
            }
        }
    }
    
    // sort count -> zone -> hour
    std::sort(result.begin(), result.end(), [](const SlotCount &a, const SlotCount &b) {
            if (a.count != b.count)
                return a.count > b.count; 
            if (a.zone != b.zone)
                return a.zone < b.zone;   
            return a.hour < b.hour;       
        });
        
    if ((int)result.size() > k) result.resize(k);
    return result;
}
