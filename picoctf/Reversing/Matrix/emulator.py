


stack = [0] * 128
sp = 0
pc = 0
file = open("raw_bytes_unedited","r")
program = []
for a in file:
    program.append(a.strip())
execute = True
while execute:
    if pc >= 1490:
        print("End of program")
        break

    if sp < 0:
        print("Stack underflow")
        break
    
    print(f"opcode = {program[pc]}")
    print(f"sp = {program[pc]}")
    
    input()
    if program[pc] == "81":
        value = int(program[pc + 2] + program[pc + 1], 16)
        stack[sp] = value
        sp += 1
        pc += 3
        continue

    elif program[pc] == "80":
        stack[sp] = int(program[pc + 1],16)
        sp += 1
        pc += 2
        continue
    
    elif program[pc] == "30":
        pc = stack[sp - 1]
        sp -= 1
        continue
    elif program[pc] == "10":
        temp_sp = sp
        temp_sp2 = sp - 1
        sp += 1
        stack[temp_sp] = stack[temp_sp2]
        pc += 1
        continue
    elif program[pc] == "c0":
        stack[sp] = input("Enter a value")
        sp += 1
        pc += 1

    else:
        print(f"PC: {pc}")
        print(f"Unexpected opcode {program[pc]}")
        input()  
    

    
    
