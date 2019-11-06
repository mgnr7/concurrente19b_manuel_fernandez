main:
	shared mutex := semaphore(1)
	shared rest_count := read_integer()
	shared rest_capacity[rest_count] := 0[rest_count]
	shared count[rest_count] := 0[rest_count]
	shared rest_queue[rest_count] := 0[rest_count]
	
	for local index := 0 to rest_count do
		rest_capacity[index] := read_integer()
		rest_queue[index] = semaphore(rest_capacity[index])
		count[index] := rest_capacity[index]

	local id := 0
	while true do
		case read_char() of
		'P' : create_thread(patient(id++))
		'I' : create_thread(impatient(id++))
		EOF : return
		

patient(id):
	wait(mutex)
	--count[id mod rest_count]
	signal(mutex)
	wait(rest_queue[id mod rest_count])
	eat()
	signal(rest_queue[id mod rest_count])
	wait(mutex)
	++count
	signal(mutex)
	
impatient(id):

	local hungry := 1
	if count[id mod rest_count] > 0 do
		wait(mutex)
		--count[id mod rest_count]
		signal(mutex)
		wait(rest_queue[id mod rest_count])
		eat()
		signal(rest_queue[id mod rest_count])
		wait(mutex)
		++count[id mod rest_count]
		signal(mutex)
		hungry := 0
	
	else:
		walk()
		local minimum := 0
		for local index := 0 to rest_count do
			if count[index] > 0
				wait(mutex)
				-- count[index]
				signal(mutex)
				wait(rest_queue[index])
				eat()
				signal(rest_queue[index])
				wait(mutex)
				++count[index]
				signal(mutex)
				index := rest_count
				hungry := 0
				
			else:
				if count[minimum] > count[index] do
					minimum := index
					walk()
		if hungry > 0 do
		wait(mutex)
		--count[minimum]			
		signal(mutex)
		wait(rest_queue[minimum])
		eat()
		signal(rest_queue[minimum])		
		wait(mutex)
		++count[index]
		signal(mutex)
					
