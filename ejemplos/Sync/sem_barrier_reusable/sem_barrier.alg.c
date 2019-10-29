main:
	// How many threads have arrived to the barrier
	shared count := 0
	// Protects the increment of the count
	shared mutex := semaphore(1)
	// Locked (0) until all threads arrive, then it is unlocked (1)
	shared turnstile1 := semaphore(0)
	shared turnstile2 := semaphore(1)
	shared thread_count := read_integer()
	create_thread(secondary, thread_count)

secondary:
	Statement A
	

	
	// Statement B must be executed until
	// all threads have executed Statement A
	Statement B

	wait(mutex)
	if --count == 0 then
		wait(turnstile1)
		signal(turnstile2)
	signal(mutex)		//evitar esperar dentro de otra espera
	
	wait(turnstile2)
	signal(turnstile2)
