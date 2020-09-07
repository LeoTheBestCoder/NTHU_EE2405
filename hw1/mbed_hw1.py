# Part. 1
#=======================================
# Import module
#  csv -- fileIO operation
import csv
#=======================================

# Part. 2
#=======================================
# Read cwb weather data
cwb_filename = '107061218.csv'
data = []
header = []
with open(cwb_filename) as csvfile:
   mycsv = csv.DictReader(csvfile)
   header = mycsv.fieldnames
   for row in mycsv:
      data.append(row)
#=======================================

# Part. 3
#=======================================
# Analyze data depend on your group and store it to target_data like:
# Retrive all data points which station id is "C0X260" as a list.
# target_data = list(filter(lambda item: item['station_id'] == 'C0X260', data))


station_list = sorted(['C0A880', 'C0F9A0', 'C0G640', 'C0R190', 'C0X260'])
HUMD_list = []

data = list(filter(lambda item: item.get('HUMD') \
        not in {'-99.000', '-999.000'}, data))


for station in station_list:
    humd_temp = 0
    for stats in data:
        if stats['station_id'] == station:
            humd_temp += float(stats['HUMD'])
    HUMD_list.append([station, humd_temp] if humd_temp != 0 \
            else [station, 'None'])


print(HUMD_list)




# Retrive ten data points from the beginning.

#target_data = data[:10]

#=======================================

# Part. 4
#=======================================
# Print result

#print(target_data)

#========================================
