import math
from pprint import pprint

P1_SINUSOID_RISING_WIDTH = 20
P2a_SINUSOID_FALLING_WIDTH = 20
P2b_EXP_FALLING_WIDTH = 80

dP1 = math.pi / P1_SINUSOID_RISING_WIDTH
dP2a = math.pi / P2a_SINUSOID_FALLING_WIDTH

x1 = [dP1 * i  for i in range(1, P1_SINUSOID_RISING_WIDTH+1)]
x2a = [dP2a * i for i in range(0, P1_SINUSOID_RISING_WIDTH)]
x2b = range(P2b_EXP_FALLING_WIDTH)

y1  = [(1-math.cos(x))/2 for x in x1]
y2a = [(1+math.cos(x))/2 for x in x2a]


TIME_CONSTANT = 1.0 * P2b_EXP_FALLING_WIDTH / 5.0 
y2b = [math.exp(-x / TIME_CONSTANT) for x in x2b]

# y2a.extend([0] * (P2b_EXP_FALLING_WIDTH - P2a_SINUSOID_FALLING_WIDTH))

# y2 = [y2a[i] + y2b[i] for i in range(P2b_EXP_FALLING_WIDTH)]
# y2 = [y2[i] / max(y2) for i in range(P2b_EXP_FALLING_WIDTH)]

y2 = y2b

y = y1 + y2
print(y)
pprint(y)




