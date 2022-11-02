-- Return the value where a parameterized function is nearest
-- to a given point, using a dichotomy method
-- func: function to analyze, input:a parameter T, output: a table with 3 float components
-- p: point to get the nearest value of
-- range_min, range_max: start and end offsets
-- steps: number of points to sample during each run
-- rounds: number of runs

function find_optimal_minimum(func, p, range_min, range_max, steps, rounds)
    local current_min = range_min
    local current_max = range_max
    local current_optimum = range_min
    local current_dist = 100000000000000
    local current_step = 0
    for r=0, rounds do
        for i=0,steps do
            local offset = (i) * (current_max - current_min) / steps + current_min
            local value = func(offset)
            local dist = (value[0] - p[0])^2 + (value[1] - p[1])^2 + (value[1] - p[1])^2
            if (dist < current_dist) then
                current_optimum = offset
                current_step = i
                current_dist = dist
            end
        end
        local new_current_min = (current_step - 1)* (current_max - current_min) / steps + current_min
        local new_current_max = (current_step + 1)* (current_max - current_min) / steps + current_min
        current_min = new_current_min
        current_max = new_current_max
        if (current_min < range_min) then
            current_min = range_min
        end
        if (current_max > range_max) then
            current_max = range_max
        end
    end
    local result = {}
    result[0] = current_optimum
    result[1] = func(current_optimum)
    return result
end
