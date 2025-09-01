import judy

v = judy.JudyIntIntMap()

v[2] = 3
v[1] = 4
v[2] = 4
print(v)
print(v.pop(2))
print(v.pop('2'))
