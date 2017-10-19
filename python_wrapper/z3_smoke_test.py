from z3 import *

x = Int('x')
y = Int('y')
s = Solver()
s.add(x > 0)
s.add(x < 2)
s.add(y == x + 1)

print(s.check())
print(s.model())
