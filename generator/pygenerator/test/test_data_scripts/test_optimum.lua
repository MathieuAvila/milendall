function my_position(t)
    pos = {}
    pos[0] = (t - 5)*(t-5.1) + 5
    pos[1] = 0
    pos[2] = 0
    return pos
end

p = {}
p[0] = 5
p[1] = 0
p[2] = 0

print(find_optimal_minimum(my_position, p, -100, 100, 100, 2))
