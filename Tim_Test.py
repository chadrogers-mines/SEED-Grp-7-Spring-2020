import numpy as np

with open("datafile.txt","r") as f:
    b = eval(f.read())
     
max = 0     
     
for sample in b:
    if sample > max:
        max = sample
        
print("Max value in datafile: ")
print(max)

min = max

for number in b:
    if number < min:
        min = number
       
print("Min value in datafile: ")
print(min)
       
counter = 0;
index38 = 0;

for num in b:
    if num == 38:
        index38 = counter
    counter = counter + 1
print("The index where 38 is at: ")
print(index38)

numdic = {}

for value in b:
    if value not in numdic:
        numdic[value] = 1
        
    elif value in numdic:
        numdic[value] = numdic[value] + 1
     
max = 0
maxValue = 0

for number, count in numdic.items():
    if numdic[number] > max:
        max = numdic[number]
        maxValue = number
        
print("The value which is repeated the most: ")
print(maxValue)
print("This number is repeated ")
print(max)
print(" times.")


np_b = np.array(b)

np_b.sort()

print("The sorted list is: ")
print(np_b)

even_nums = [num for num in b if num % 2 == 0]

print("The unsorted even numbers are: ") 
print(even_nums)


