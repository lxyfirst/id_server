

function min_counter()
    return 0
end

function max_counter()
    return 999999
end

function reset_seconds()
    return 86400
end

function create_id(counter,now,salt)
    local seq = counter:generate_counter()
    local new_id = string.format("O%s%02d%02d%02d%06d",salt,now:year()%100 ,now:month(),now:day(),seq)
    return new_id
end

