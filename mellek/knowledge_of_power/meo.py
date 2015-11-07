#-*- coding: utf-8 -*-
lista = []
days = 0

with open('data.txt', 'r') as inp:
	days = int(inp.readline())
	tasks = int(inp.readline())
	for line in list(inp):
		lista.append(line.strip().split(';')); 

class VEs:
	def __init__(self, nev, kezdo, egyseg):
		self.nev = nev
		self.kezdo = kezdo
		self.egyseg = egyseg
	
	def printIt(self):
		return nev + ";" + str(kezdo) + ";" + str(self.end())
	
	def end(self):
		self.kezdo + egyseg * 0.5

class TaskNeed:
	def __init__(self, nev, egyseg, reszlet):
		self.nev = nev
		self.sum = egyseg
		self.minegy = 1
		self.reszlet = reszlet

	def getSome(self, fromx, egyseg):
		self.egyseg -= egyseg
		self.reszlet -= 1
		return Ves(self.nev, fromx, egyseg) 
	
	def isGood(self):
		return self.egyseg <= 0 and self.reszlet <= 0

allTaskNeed = [TaskNeed("alvás", 16, 2), TaskNeed("evés", 6, 3), TaskNeed("szórakozás", 1, 1)]

class Task:
	def __init__(self, arr):
		self.nev = arr[0]
		self.napsz = arr[1]
		self.minegy = int(float(arr[2].replace(',', '.')) * 2)
		self.maxegy = int(float(arr[3].replace(',', '.')) * 2)
		self.sum = int(float(arr[4].replace(',', '.')) * 2)
		self.hatid = arr[5]

	def getSome(self, fromx, egyseg):
		self.sum -= egyseg
		return Ves(self.nev, fromx, egyseg) 

	def isGood(self):
		return self.sum <= 0

	def mycmp(self, other):
		if self.hatid < other.hatid:
			return -1
		elif self.hatid < other.hatid:
			return 1
		elif self.sum < other.sum:
			return -1
		elif self.sum > other.sum:
			return 1
		return 0

	def __lt__(self, other):
		return self.mycmp(other) < 0
	def __gt__(self, other):
		return self.mycmp(other) > 0
	def __eq__(self, other):
		return self.mycmp(other) == 0
	def __le__(self, other):
		return self.mycmp(other) <= 0
	def __ge__(self, other):
		return self.mycmp(other) >= 0
	def __ne__(self, other):
		return self.mycmp(other) != 0

de = []
du = []

for listElem in lista:
	el = Task(listElem)
	
	if el.napsz == 'de':
		de.append(el)
	else:
		du.append(el)

 
print sorted(de)
print sorted(du)

out = open('timetable.txt', 'w')



