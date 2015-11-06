
lista = []
days = 0

with open('data.txt', 'r') as inp:
	days = int(inp.readline())
	tasks = int(inp.readline())
	for line in list(inp):
		lista.append(line.strip().split(';')); 

class Day(object):
    def __init__(self):

daysto = {}

for day in xrange(days):
	days[day] =  

out = open('timetable.txt', 'w')



